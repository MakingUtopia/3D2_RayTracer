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

	//Origin of sphere.
	glm::vec4 O = glm::vec4(0, 0, 0, 1);

	//Sphere origin to Ray origin.
	glm::vec4 OP0 = origin_local - O;

	//Calculate quadratic coefficients.
	float a = glm::dot(direction_local, direction_local);
	float b = 2.0f * glm::dot(direction_local, OP0);
	float c = glm::dot(OP0, OP0) - radius * radius;

	//Calculate discriminant.
	float discriminant = b*b - 4.0f * a * c;
	//Return t = infinite (no collision) if discriminant < 0.
	//Negative discriminant means imaginary roots.
	//Which tells us there's no intersection.
	if (discriminant < 0.0f)
		return 0;

	//Find first root.
	t1 = (-1.0f * b + glm::sqrt(discriminant)) / (2.0f * a);

	//Use root to calculate first collision point.
	collision_Point1 = origin_local + direction_local * t1;

	//Use collision points to work out first normal.
	glm::vec4 OQ_1 = collision_Point1 - O;
	float OQ_mag_1 = glm::length(OQ_1);
	collision_Normal1 = OQ_1 / OQ_mag_1;
	
	//A discriminant of exactly zero means there's one point of intersection,
	//so we exit here.
	if (discriminant == 0.0f)
		return 1;

	//Otherwise calculate second root, collision point & normal.
	t2 = (-1.0f * b - glm::sqrt(discriminant)) / (2.0f * a);
	collision_Point2 = origin_local + direction_local * t2;
	glm::vec4 OQ_2 = collision_Point2 - O;
	float OQ_mag_2 = glm::length(OQ_2);
	collision_Normal2 = OQ_2 / OQ_mag_2;

	//>0 means 2 points of intersection.
	return 2;
}