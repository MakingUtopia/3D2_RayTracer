#include "ITriangle.h"
#include "RayTracingFramework\Ray.h"
#include "RayTracingFramework\VirtualObject\IVirtualObject.h"


bool RayTracingFramework::ITriangle::testLocalCollision(RayTracingFramework::Ray& ray){
	//0. Transform origin and direction coordinates to local coordinates:
	glm::vec4 origin_local = owner->getFromWorldToObjectCoordinates()*ray.origin_InWorldCoords;
	glm::vec4 direction_local = ray.direction_InWorldCoords;
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
	
	//Unknowns
	float _t, beta, gamma;

	//Ray Equation: P(t) = P0 + v*t
	//Ray origin.
	glm::vec3 P0 = origin_local;  
	//Direction of ray.
	glm::vec3 v = direction_local;  
	v = glm::normalize(v);

	//Points on triangle: a, b, c
	glm::vec3 a = A;
	glm::vec3 b = B;
	glm::vec3 c = C;

	//Plane Equation: P(beta, gamma) = a + beta*(b-a) + gamma*(c-a)
	//Equate plane with ray.
	//  v.t + beta*(b-a) + gamma*(a-c) = a - P0
	//Build matrix representing linear version of above equation.
	glm::mat3x3 A = glm::mat3x3(
		v.x, a.x - b.x, a.x - c.x,
		v.y, a.y - b.y, a.y - c.y,
		v.z, a.z - b.z, a.z - c.z
	);

	//Find determinant
	float D = glm::determinant(A);

	//Cramer's rule.
	//Repeat above for each unknown (t, beta, gamma)...
	//Swap corresponding unknown's coefficient with the other side of the equation.
	//(E.g. for 't', swap v with (a - P0).)
	// t
	float Dt = glm::determinant(
		glm::mat3x3(
			a.x - P0.x, a.x - b.x, a.x - c.x,
			a.y - P0.y, a.y - b.y, a.y - c.y,
			a.z - P0.z, a.z - b.z, a.z - c.z
		)
	);
	// beta
	float Dbeta = glm::determinant(
		glm::mat3x3(
			v.x, a.x - P0.x, a.x - c.x,
			v.y, a.y - P0.y, a.y - c.y,
			v.z, a.z - P0.z, a.z - c.z
		)
	);
	// gamma
	float Dgamma = glm::determinant(
		glm::mat3x3(
			v.x, a.x - b.x, a.x - P0.x,
			v.y, a.y - b.y, a.y - P0.y,
			v.z, a.z - b.z, a.z - P0.z
		)
	);

	//Divide by zero
	if (D == 0)
		return false;

	//Use above to calculate the unknowns.
	_t = Dt / D;
	beta = Dbeta / D;
	gamma = Dgamma / D;

	//Check that the point of intersection on the plane lies within out triangle.
	if (beta < 0.0f || beta > 1.0f)
		return false;
	if (gamma < 0.0f || gamma > 1.0f)
		return false;
	if (beta + gamma < 0.0f || beta + gamma > 1.0f)
		return false;

	//Assign calculated value of 't' to pointer supplied via arguments.
	t = _t;

	//Calculate collision point.
	glm::vec3 Q = P0 + v * _t;
	collision_Point = glm::vec4(Q, 1);

	//Calculate collision normal.
	//Get a->b && a->c
	glm::vec3 ab = b - a;
	glm::vec3 ac = c - a;
	//Find the cross product of these two to get the normal.
	glm::vec3 n = glm::cross(ab, ac);
	n = glm::normalize(n);
	collision_Normal = glm::vec4(n, 0.0f);

	return true;
}