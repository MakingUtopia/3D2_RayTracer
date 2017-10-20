#include "Plane.h"
#include "RayTracingFramework\Ray.h"
#include "RayTracingFramework\VirtualObject\IVirtualObject.h"

RayTracingFramework::Plane::Plane(glm::vec4 P0, glm::vec4 N)
	: P0(P0)
	, N(N)
	, A(N.x)
	, B(N.y)
	, C(N.z)
	, D(A*P0.x + B*P0.y +C*P0.z)	//Check slides, we compute D, so that P0 belongs to the plane.
{
	;
}

bool RayTracingFramework::Plane::testLocalCollision(RayTracingFramework::Ray& ray){
	//0. Transform origin and direction coordinates to local coordinates (Ray is described in World coordinates):
	glm::vec4 origin_local = owner->getFromWorldToObjectCoordinates()*ray.origin_InWorldCoords;
	glm::vec4 direction_local = owner->getFromWorldToObjectCoordinates()*ray.direction_InWorldCoords;
	//1. Compute intersection with plane (compute collision point and normal). 
	glm::vec4 collision_Point, collision_Normal;
	float t;
	if (testRayPlaneCollision(origin_local, direction_local
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

bool RayTracingFramework::Plane::testRayPlaneCollision(glm::vec4 origin, glm::vec4 direction
	, float& t, glm::vec4& col_P, glm::vec4& col_N)
{
	if ((glm::dot(N, direction)) == 0)//Plane is parallel to ray (intersection at infinity)
		return false;				  // Intersection at infinite
	else { 
		//Common intersection: Compute using parametric & implicit equations (as in slides), and fill in our "Output parameters t, col_P and col_N.
		t = (D - glm::dot(N, origin)) / (glm::dot(N, direction));
		col_P = origin + t*direction;	//According to parametric equation: P(t)=P0 + t* direction
		col_N = this->N;				//Normal of the plane
		return true;
	}	
}
