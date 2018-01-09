#include "IShadingModel.h"


RayTracingFramework::Colour RayTracingFramework::IShadingModel::computeShading(RayTracingFramework::Ray& ray, RayTracingFramework::IScene& scene, int recursiveLevel) {
	if (recursiveLevel < 0)
		return RayTracingFramework::Colour(0, 0, 0);

	//Let's get the intersection we need to shade and the material applied to that point. 
	Ray::Intersection inter = ray.getClosestIntersection();
	IVirtualObject& collidedObject = scene.getNodeByID(inter.collidingObjectID);
	RayTracingFramework::Material& m = collidedObject.getMaterial();

	return computeAmbientDiffuseSpecular(inter, m, scene.getLights()[0]);
}

bool RayTracingFramework::IShadingModel::checkForShadow(Ray::Intersection originalIntersection, ILight* lightSource) {
	//Origin of shadow ray is at collision point.
	glm::vec4 shadowOrigin = originalIntersection.collisionPoint_InObjectCoords * originalIntersection.fromObjectToWorldCoords;
	//Fire it back towards light source.
	glm::vec4 shadowRayDirection = -lightSource->lightDirectionAtPoint(shadowOrigin);
	//Create ray.
	Ray shadowRay = Ray(shadowOrigin, shadowRayDirection);
	//Return true if any intersections occured, otherwise false.
	return (shadowRay.getClosestIntersection().t_distance != FLT_MAX);
}

RayTracingFramework::Colour RayTracingFramework::IShadingModel::computeAmbientDiffuseSpecular(
	RayTracingFramework::Ray::Intersection intersection,
	RayTracingFramework::Material& material, 
	RayTracingFramework::ILight* lightSource
) {
	//0. Let's model the light
	Colour lightColour = lightSource->baseColour();

	//1. For shading, we need everything back in world's coordinates.
	glm::vec4 normalInWorld = glm::normalize(intersection.fromObjectToWorldCoords*intersection.collisionNormalVector_InObjectCoords);
	glm::vec4 collisionPointInWorld = intersection.fromObjectToWorldCoords*intersection.collisionPoint_InObjectCoords;
	collisionPointInWorld /= collisionPointInWorld.w; //Make homogeneous (scaling could break this).
													  //K_a:  AMBIENT COMPONENT:
	RayTracingFramework::Colour ambientComponent = material.K_a * material.diffuseColour;

	//K_d: DIFFUSE COMPONENT
	RayTracingFramework::Colour diffuseComponent(0, 0, 0);
	glm::vec4 intersectionPointToLight_Direction = -1.0f* (lightSource->lightDirectionAtPoint(collisionPointInWorld));
	float cos_angle = glm::dot(normalInWorld, intersectionPointToLight_Direction);
	cos_angle = (cos_angle > 1 ? 1 : (cos_angle < 0 ? 0 : cos_angle));
	//Simplest Lambertian model ( AND no light attenuation with distance)
	diffuseComponent = cos_angle * material.K_d * material.diffuseColour * lightColour;

	//TODO: specular.

	return ambientComponent + diffuseComponent;
}

RayTracingFramework::Colour RayTracingFramework::IShadingModel::computeDiffuse(
	Colour inputColour, ILight* lightSource, Material& material,
	glm::vec4 collisionPoint, glm::vec4 collisionNormal