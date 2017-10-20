#include "IShadingModel.h"
#include "RayTracingFramework\Ray.h"
#include "RayTracingFramework\Light\ILight.h"
#include "RayTracingFramework\Material.h"
#include "RayTracingFramework\GeometricPrimitives\IGeometry.h"
#include "RayTracingFramework\VirtualObject\ISceneManager.h"

RayTracingFramework::Colour RayTracingFramework::IShadingModel::computeShading(RayTracingFramework::Ray& ray, RayTracingFramework::IScene& scene, int recursiveLevel) {
	if (recursiveLevel < 0)
		return RayTracingFramework::Colour(0, 0, 0);
	//Let's get the intersection we need to shade and the material applied to that point. 
	Ray::Intersection inter = ray.getClosestIntersection();
	IVirtualObject& collidedObject = scene.getNodeByID(inter.collidingObjectID);
	RayTracingFramework::Material& m = collidedObject.getMaterial();
	
	
	//0. Let's model the light
	Colour lightColour = scene.getLights()[0]->baseColour();

	//1. For shading, we need everything back in world's coordinates.
	glm::vec4 normalInWorld = glm::normalize(inter.fromObjectToWorldCoords*inter.collisionNormalVector_InObjectCoords);
	glm::vec4 collisionPointInWorld = inter.fromObjectToWorldCoords*inter.collisionPoint_InObjectCoords;
	collisionPointInWorld /= collisionPointInWorld.w; //Make homogeneous (scaling could break this).

	//K_a:  AMBIENT COMPONENT:
	RayTracingFramework::Colour ambientComponent = m.K_a*m.diffuseColour;

	//K_d: DIFFUSE COMPONENT
	RayTracingFramework::Colour diffuseComponent(0, 0, 0);
	glm::vec4 intersectionPointToLight_Direction = -1.0f* (scene.getLights()[0]->lightDirectionAtPoint(collisionPointInWorld));
	float cos_angle = glm::dot(normalInWorld, intersectionPointToLight_Direction);
	cos_angle = (cos_angle > 1 ? 1 : (cos_angle < 0 ? 0 : cos_angle));
	diffuseComponent = cos_angle*m.K_d*m.diffuseColour*lightColour;//Simplest Lambertian model ( AND no light attenuation with distance)
	return ambientComponent + diffuseComponent;
	
}