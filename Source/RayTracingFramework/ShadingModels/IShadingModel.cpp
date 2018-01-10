#include "IShadingModel.h"


RayTracingFramework::Colour RayTracingFramework::IShadingModel::computeShading(RayTracingFramework::Ray& ray, RayTracingFramework::IScene& scene, int recursiveLevel) {
	if (recursiveLevel < 0)
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

	//If shadow is found, don't apply diffuse or specular shading.
	if (!checkForShadow(collisionPointInWorld, lightSource)) {
		//Apply diffuse shading to output colour.
		outputColour = computeDiffuse(outputColour, lightSource, material, collisionPointInWorld, normalInWorld);
	}

	return outputColour;
}

bool RayTracingFramework::IShadingModel::checkForShadow(glm::vec4 collisionPoint, ILight* lightSource) { 
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
	return (shadowRay.getClosestIntersection().t_distance != FLT_MAX);
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