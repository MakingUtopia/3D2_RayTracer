#include "IShadingModel.h"


RayTracingFramework::Colour RayTracingFramework::IShadingModel::computeShading(RayTracingFramework::Ray& ray, RayTracingFramework::IScene& scene, int recursiveLevel) {
	if (recursiveLevel >= recursionLimit)
		return RayTracingFramework::Colour(0, 0, 0);

	//Let's get the intersection we need to shade and the material applied to that point. 
	Ray::Intersection intersection = ray.getClosestIntersection();
	IVirtualObject& collidedObject = scene.getNodeByID(intersection.collidingObjectID);
	RayTracingFramework::Material& material = collidedObject.getMaterial();

	//Calculate collision point and collision normal.
	glm::vec4 normalInWorld = glm::normalize(intersection.fromObjectToWorldCoords * intersection.collisionNormalVector_InObjectCoords);
	glm::vec4 collisionPointInWorld = intersection.fromObjectToWorldCoords * intersection.collisionPoint_InObjectCoords;
	collisionPointInWorld /= collisionPointInWorld.w;

	//Initially equate output colour with the ambient component of the shading model.
	Colour ambientComponent = material.K_a * material.diffuseColour;
	Colour outputColour = ambientComponent;

	//Get our light source.
	RayTracingFramework::ILight* lightSource = scene.getLights()[0];
	
	//Create ShadingInfo struct.
	ShadingInfo shadingInfo = {
		outputColour, scene, lightSource, material, collisionPointInWorld, normalInWorld, ray, 
		intersection.collidingObjectID, recursiveLevel, 0.0f, 
	};

	//Get intensity of shadow at collision point.
	shadingInfo.shadowIntensity = getShadowIntensity(shadingInfo);

	//Apply diffuse shading to output colour.
	outputColour = computeDiffuse(shadingInfo);

	//Apply transparency.
	outputColour = getNextLayerColour(shadingInfo);

	//Apply potential reflections.
	outputColour = checkForReflection(shadingInfo);

	//Apply specular shading.
	outputColour = computeSpecular(shadingInfo);

	return outputColour;
}

//For transparency.
//Get next object behind one collided with and merge colours using materials.
//(Or merge with background.)
RayTracingFramework::Colour RayTracingFramework::IShadingModel::getNextLayerColour(ShadingInfo shadingInfo) {
	//By default, set the colour of the next layer to the background colour.
	Colour nextLayerColour = backgroundColour;
	//Move origin of ray a little forward to prevent finding identical collision to the one that triggered this.
	glm::vec4 origin = shadingInfo.collisionPoint + 0.1f * shadingInfo.ray.direction_InWorldCoords;
	//Create a ray that is a continuing (identical) version of the ray that collided.
	Ray continuingRay = Ray(origin, shadingInfo.ray.direction_InWorldCoords);
	//Test for collisions with scene.
	shadingInfo.scene.getRootNode().testCollision(continuingRay, glm::mat4(1.0f));
	
	//Remove any intersections found with the same object that triggered this check.
	while (continuingRay.getClosestIntersection().collidingObjectID == shadingInfo.originalObjectId) continuingRay.discardClosestIntersection();
	
	//Remove any intersections behind ray origin.
	while (continuingRay.getClosestIntersection().t_distance < 0) continuingRay.discardClosestIntersection();

	//Check if any other collisions occured.
	if (continuingRay.getClosestIntersection().t_distance != FLT_MAX)
		//Set colour of next layer to whatever the computed value of the next closest collision is.
		nextLayerColour = computeShading(continuingRay, shadingInfo.scene, shadingInfo.recursiveLevel + 1);
	//Return current layer and next layer merged based on material.
	return shadingInfo.outputColour * (1.0f - shadingInfo.material.K_t) + nextLayerColour * shadingInfo.material.K_t;
}

float RayTracingFramework::IShadingModel::getShadowIntensity(ShadingInfo shadingInfo) {
	//By default, shadow intensity is zero.
	float shadowIntensity = 0.0f;
	//Fire shadow ray back towards light source.
	glm::vec4 shadowRayDirection = -shadingInfo.lightSource->lightDirectionAtPoint(shadingInfo.collisionPoint);
	//Origin of shadow ray is at collision point.
	//(+0.1f to avoid self collision due to rounding errors.)
	glm::vec4 shadowRayOrigin = shadingInfo.collisionPoint + 0.1f * shadowRayDirection;
	//Create ray.
	Ray shadowRay = Ray(shadowRayOrigin, shadowRayDirection);
	//Test shadow ray for collisions with scene.
	shadingInfo.scene.getRootNode().testCollision(shadowRay, glm::mat4(1.0f));
	//Get rid of self shadows.
	while (shadowRay.getClosestIntersection().collidingObjectID == shadingInfo.originalObjectId)
		shadowRay.discardClosestIntersection();
	//Loop through any shadows found and discard so that we can combine multiple shadows.
	while (shadowRay.getClosestIntersection().t_distance != FLT_MAX && shadowIntensity < 1.0f) {
		//Get shadow forming object.
		IVirtualObject& shadowObject = shadingInfo.scene.getNodeByID(shadowRay.getClosestIntersection().collidingObjectID);
		//Calculate shadow intensity based on shadow object's transparency.
		//Make sure it accumulates multiple shadows...
		shadowIntensity += 1.0f - shadowObject.getMaterial().K_t;
		//Discard and onto the next...
		shadowRay.discardClosestIntersection();
	}
	//Cap at 1.0f
	return (shadowIntensity > 1.0f) ? 1.0f : shadowIntensity;
}

RayTracingFramework::Colour RayTracingFramework::IShadingModel::checkForReflection(ShadingInfo shadingInfo) {
	//Create reflection ray.
	glm::vec3 lightDirection = shadingInfo.ray.direction_InWorldCoords;
	glm::vec3 normal = shadingInfo.collisionNormal;
	glm::vec3 reflectionDirection = glm::normalize(normal - lightDirection);
	glm::vec3 reflectionOrigin = glm::vec3(shadingInfo.collisionPoint) + 0.1f * reflectionDirection;
	Ray reflectionRay = Ray(glm::vec4(reflectionOrigin, 1.0f), glm::vec4(reflectionDirection, 0.0f));
	//Test reflection ray for collisions with scene.
	shadingInfo.scene.getRootNode().testCollision(reflectionRay, glm::mat4(1.0f));
	//Initialise reflection colour as ambient background colour (White).
	float offWhite = 200.0f / 256.0f;
	Colour reflectionColour = backgroundColour;
	//Remove any intersections behind ray origin.
	while (reflectionRay.getClosestIntersection().t_distance < 0) reflectionRay.discardClosestIntersection();
	//Check if collision was found.
	if (reflectionRay.getClosestIntersection().t_distance != FLT_MAX) {
		//Compute shading for next surface found.
		reflectionColour = computeShading(reflectionRay, shadingInfo.scene, shadingInfo.recursiveLevel + 1);
	}
	//Apply diffuse intensity gradient to reflection.
	Colour newColour = reflectionColour * shadingInfo.material.K_r * (calculateDiffuseIntensity(shadingInfo) + shadingInfo.material.K_a);
	//Blend with output colour.
	return shadingInfo.outputColour * (1.0f - shadingInfo.material.K_r) + newColour;
}

float RayTracingFramework::IShadingModel::calculateDiffuseIntensity(ShadingInfo shadingInfo) {
	glm::vec4 intersectionPointToLight_Direction = -1.0f * (shadingInfo.lightSource->lightDirectionAtPoint(shadingInfo.collisionPoint));
	float cos_angle = glm::dot(shadingInfo.collisionNormal, intersectionPointToLight_Direction);
	cos_angle = (cos_angle > 1 ? 1 : (cos_angle < 0 ? 0 : cos_angle));
	//Simplest Lambertian model (AND no light attenuation with distance)
	return cos_angle * shadingInfo.material.K_d * (1.0f - shadingInfo.shadowIntensity);
}

RayTracingFramework::Colour RayTracingFramework::IShadingModel::computeDiffuse(ShadingInfo shadingInfo) {
	//K_d: DIFFUSE COMPONENT
	RayTracingFramework::Colour diffuseComponent(0, 0, 0);
	diffuseComponent = calculateDiffuseIntensity(shadingInfo) * shadingInfo.material.diffuseColour * shadingInfo.lightSource->baseColour();
	return shadingInfo.outputColour + diffuseComponent;
}

RayTracingFramework::Colour RayTracingFramework::IShadingModel::computeSpecular(ShadingInfo shadingInfo) {
	RayTracingFramework::Colour specularComponent(0, 0, 0);

	glm::vec4 reflect = glm::reflect(-shadingInfo.lightSource->lightDirectionAtPoint(shadingInfo.collisionPoint), shadingInfo.collisionNormal);
	float spec_angle = glm::max(glm::dot(reflect, shadingInfo.ray.direction_InWorldCoords), 0.0f);

	specularComponent = glm::pow(spec_angle, shadingInfo.material.shininess / 4.0f) * shadingInfo.material.K_s * shadingInfo.material.specularColour * shadingInfo.lightSource->baseColour();
	return shadingInfo.outputColour + specularComponent;
}