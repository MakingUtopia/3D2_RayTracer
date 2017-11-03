#include "ISphere.h"
#include "RayTracingFramework\Ray.h"
#include "RayTracingFramework\VirtualObject\IVirtualObject.h"

RayTracingFramework::ISphere::ISphere(float radius):radius(radius)
{
	;
}

bool RayTracingFramework::ISphere::testLocalCollision(RayTracingFramework::Ray& ray){
	//0. Transform origin and direction coordinates to local coordinates:
	glm::vec4 origin_local = owner->getFromWorldToObjectCoordinates()*ray.origin_InWorldCoords;
	glm::vec4 direction_local = owner->getFromWorldToObjectCoordinates()*ray.direction_InWorldCoords;
	//1. Compute intersection with plane (compute collision point and normal). 
	glm::vec4 collision_Point1, collision_Normal1;
	float t1;
	glm::vec4 collision_Point2, collision_Normal2;
	float t2;
	int numSolutions;
	if (numSolutions= testRaySphereCollision(origin_local, direction_local
		, t1, collision_Point1, collision_Normal1
		, t2, collision_Point2, collision_Normal2)) {
		//2. Intersection! --> Add it to the result (ray).
		Ray::Intersection i1;
		i1.t_distance = t1;
		i1.collidingObjectID = owner->getID();
		i1.collisionPoint_InObjectCoords = collision_Point1;
		i1.collisionNormalVector_InObjectCoords = collision_Normal1;
		//3. To transform from local (object) coords to world coordinates 
		i1.fromObjectToWorldCoords = owner->getFromObjectToWorldCoordinates();
		ray.addIntersection(i1);
		//Add second solution if existing...
		if (numSolutions == 2) {
			Ray::Intersection i1;
			i1.t_distance = t2;
			i1.collidingObjectID = owner->getID();
			i1.collisionPoint_InObjectCoords = collision_Point2;
			i1.collisionNormalVector_InObjectCoords = collision_Normal2;
			//3. To transform from local (object) coords to world coordinates 
			i1.fromObjectToWorldCoords = owner->getFromObjectToWorldCoordinates();
			ray.addIntersection(i1);
		}
		return true;
	}
	return false;
}

int RayTracingFramework::ISphere::testRaySphereCollision(glm::vec4 origin_local, glm::vec4 direction_local
	, float &t1, glm::vec4& collision_Point1, glm::vec4& collision_Normal1
	, float &t2, glm::vec4& collision_Point2, glm::vec4& collision_Normal2) {

	
	return 0;
}