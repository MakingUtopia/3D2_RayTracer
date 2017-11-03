#include "ITriangle.h"
#include "RayTracingFramework\Ray.h"
#include "RayTracingFramework\VirtualObject\IVirtualObject.h"


bool RayTracingFramework::ITriangle::testLocalCollision(RayTracingFramework::Ray& ray){
	//0. Transform origin and direction coordinates to local coordinates:
	glm::vec4 origin_local = owner->getFromWorldToObjectCoordinates()*ray.origin_InWorldCoords;
	glm::vec4 direction_local = owner->getFromWorldToObjectCoordinates()*ray.direction_InWorldCoords;
	//1. Compute intersection with plane (compute collision point and normal). 
	glm::vec4 collision_Point, collision_Normal;
	float t;
	if (testRayTriangleCollision(origin_local, direction_local
		, t, collision_Point, collision_Normal)) {
		//2. Intersection! --> Add it to the result (ray).
		Ray::Intersection i1;
		i1.t_distance = t;
		i1.collidingObjectID = owner->getID();
		i1.collisionPoint_InObjectCoords = collision_Point;
		i1.collisionNormalVector_InObjectCoords = collision_Normal;
		//3. To transform from local (object) coords to world coordinates 
		i1.fromObjectToWorldCoords = owner->getFromObjectToWorldCoordinates();
		ray.addIntersection(i1);
		
		return true;
	}
	return false;
}

bool RayTracingFramework::ITriangle::testRayTriangleCollision(glm::vec4 origin_local, glm::vec4 direction_local
	, float &t, glm::vec4& collision_Point, glm::vec4& collision_Normal
	) {
	//Build 3x3 matrix D ... breakpoint and check
	//Build 3x3 matrix D_t ... breakpoint and check
	//...build also D_beta and D_sigma 
	//solve using Cramer's rule
	

	
	return false;
}