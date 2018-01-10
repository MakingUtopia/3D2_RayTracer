#include "IShadingModel.h"


RayTracingFramework::Colour RayTracingFramework::IShadingModel::computeShading(RayTracingFramework::Ray& ray, RayTracingFramework::IScene& scene, int recursiveLevel) {
	if (recursiveLevel == recursionLimit)
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
	
	//Apply diffuse shading to output colour.
	outputColour = computeDiffuse(outputColour, lightSource, material, collisionPointInWorld, normalInWorld);

	//Apply specular shading.
	outputColour = computeSpecular(outputColour, lightSource, material, collisionPointInWorld, normalInWorld);

	//Apply potential reflections.
	checkForReflection(outputColour, scene, collisionPointInWorld, normalInWorld, ray, material.K_r, recursiveLevel);

	//If shadow is found, reduce brightness.
	if (checkForShadow(collisionPointInWorld, lightSource, ray.getClosestIntersection().collidingObjectID))
		outputColour *= 0.5f;

	return outputColour;
}

bool RayTracingFramework::IShadingModel::checkForShadow(glm::vec4 collisionPoint, ILight* lightSource, unsigned int originalObjectId) { 
	//Fire shadow ray back towards light source.
	glm::vec4 shadowRayDirection = -lightSource->lightDirectionAtPoint(collisionPoint);
	//Origin of shadow ray is at collision point.
	//(+0.1f to avoid self collision due to rounding errors.)
	glm::vec4 shadowRayOrigin = collisionPoint + 0.1f * shadowRayDirection;
	//Create ray.
	Ray shadowRay = Ray(shadowRayOrigin, shadowRayDirection);
	//Test shadow ray for collisions with scene.
	RayTracingFramework::ISceneManager::instance().getRootNode().testCollision(shadowRay, glm::mat4(1.0f));
	//Return true if any intersections occured, otherwise false.
	return (shadowRay.getClosestIntersection().t_distance != FLT_MAX && shadowRay.getClosestIntersection().collidingObjectID != originalObjectId);
}

bool RayTracingFramework::IShadingModel::checkForReflection(Colour& outputColour, IScene& scene, glm::vec4 collisionPoint, glm::vec4 collisionNormal, Ray& originalRay, float reflectiveness, int recursiveLevel) {
	bool reflection = false;
	//Create reflection ray.
	glm::vec3 lightDirection = originalRay.direction_InWorldCoords;
	glm::vec3 normal = collisionNormal;
	glm::vec3 reflectionDirection = glm::normalize(normal - lightDirection);
	glm::vec3 reflectionOrigin = glm::vec3(collisionPoint) + 0.1f * reflectionDirection;
	Ray reflectionRay = Ray(glm::vec4(reflectionOrigin, 1.0f), glm::vec4(reflectionDirection, 0.0f));
	//Test reflection ray for collisions with scene.
	RayTracingFramework::ISceneManager::instance().getRootNode().testCollision(reflectionRay, glm::mat4(1.0f));
	//Initialise reflection colour as ambient background colour (White).
	float offWhite = 200.0f / 256.0f;
	Colour newColour = Colour(offWhite, offWhite, offWhite);
	//Check if collision was found.
	if (reflectionRay.getClosestIntersection().t_distance != FLT_MAX) {
		//Compute shading for next surface found.
		newColour = computeShading(reflectionRay, scene, recursiveLevel + 1);
		reflection = true;
	}
	//Blend with output colour.
	outputColour = outputColour * (1.0f - reflectiveness) + newColour * reflectiveness;
	//outputColour += newColour * reflectiveness;
	return reflection;
}

RayTracingFramework::Colour RayTracingFramework::IShadingModel::computeDiffuse(
	Colour inputColour, ILight* lightSource, Material& material, glm::vec4 collisionPoint, glm::vec4 collisionNormal) {

	//K_d: DIFFUSE COMPONENT
	RayTracingFramework::Colour diffuseComponent(0, 0, 0);
	glm::vec4 intersectionPointToLight_Direction = -1.0f* (lightSource->lightDirectionAtPoint(collisionPoint));
	float cos_angle = glm::dot(collisionNormal, intersectionPointToLight_Direction);
	cos_angle = (cos_angle > 1 ? 1 : (cos_angle < 0 ? 0 : cos_angle));
	//Simplest Lambertian model ( AND no light attenuation with distance)
	diffuseComponent = cos_angle * material.K_d * material.diffuseColour * lightSource->baseColour();

	return inputColour + diffuseComponent;
}

RayTracingFramework::Colour RayTracingFramework::IShadingModel::computeSpecular(
	Colour inputColour, ILight* lightSource, Material& material, glm::vec4 collisionPoint, glm::vec4 collisionNormal) {
	RayTracingFramework::Colour specularComponent(0, 0, 0);
	glm::vec4 intersectionPointToLight_Direction = -1.0f* (lightSource->lightDirectionAtPoint(collisionPoint));
	float cos_angle = glm::dot(collisionNormal, intersectionPointToLight_Direction);
	cos_angle = (cos_angle > 1 ? 1 : (cos_angle < 0 ? 0 : cos_angle));
	specularComponent = glm::pow(cos_angle, material.shininess) * material.K_s * material.specularColour * lightSource->baseColour();
	return inputColour + specularComponent;
}