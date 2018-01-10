#include "Box.h"
#include "RayTracingFramework\Ray.h"
#include "RayTracingFramework\VirtualObject\IVirtualObject.h"

//PointA -> frontTopLeft AKA smallest x, biggest y & smallest z
//PointB -> backBottomRight AKA biggest x, smallest y & biggest z
RayTracingFramework::Box::Box(glm::vec4 pointA, glm::vec4 pointB)
	: A (pointA)
	, B (pointB)
	, front (Plane(A, glm::vec4(0.0f, 0.0f, 1.0f, 0.0f)))
	, back (Plane(B, glm::vec4(0.0f, 0.0f, 1.0f, 0.0f)))
	, left (Plane(A, glm::vec4(-1.0f, 0.0f, 0.0f, 0.0f)))
	, right (Plane(B, glm::vec4(1.0f, 0.0f, 0.0f, 0.0f)))
	, top (Plane(A, glm::vec4(0.0f, 1.0f, 0.0f, 0.0f)))
	, bottom (Plane(B, glm::vec4(0.0f, -1.0f, 0.0f, 0.0f)))
{
	;
}

bool RayTracingFramework::Box::testLocalCollision(RayTracingFramework::Ray& ray) {
	//Transform from world coords to local (object) coordinates.
	glm::vec4 origin_local = owner->getFromWorldToObjectCoordinates() * ray.origin_InWorldCoords;
	glm::vec4 direction_local = ray.direction_InWorldCoords;
	//Test local intersection.
	glm::vec4 collision_Point, collision_Normal;
	float t;
	if (testRayBoxCollision(origin_local, direction_local, t, collision_Point, collision_Normal)) {
		//Get details of collision and add them to ray.
		//(This passes the details to the framework.)
		Ray::Intersection i1;
		i1.t_distance = t;
		i1.collidingObjectID = owner->getID();
		i1.collisionPoint_InObjectCoords = collision_Point;
		i1.collisionNormalVector_InObjectCoords = collision_Normal;
		i1.fromObjectToWorldCoords = owner->getFromObjectToWorldCoordinates();
		ray.addIntersection(i1);
		return true;
	}
	return false;
}

//Check if collision with one of the box's planes actually exists within the box's spatial constraints.
bool RayTracingFramework::Box::checkConstraint(glm::vec3 collisionPoint, Plane& faceRef) {
	//Don't check constraint on the same axis that the plane's normal faces.
	//(Otherwise part's won't render due to rounding errors.)
	if (faceRef.N.x == 0) { 
		if (collisionPoint.x < A.x)
			return false;
		if (collisionPoint.x > B.x)
			return false;
	}
	if (faceRef.N.y == 0) {
		if (collisionPoint.y < B.y)
			return false;
		if (collisionPoint.y > A.y)
			return false;
	}
	if (faceRef.N.z == 0) {
		if (collisionPoint.z < A.z)
			return false;
		if (collisionPoint.z > B.z)
			return false;
	}
	return true;
}

bool RayTracingFramework::Box::testRayBoxCollision(glm::vec4 origin, glm::vec4 direction, float& t, glm::vec4& col_P, glm::vec4& col_N) {
	//Indices
	int _front, _back, _left, _right, _top, _bottom;
	_front = 0;	_back = 1;
	_left = 2;	_right = 3;
	_top = 4;	_bottom = 5;
	
	//Lengths of each potentially collided ray.
	//(Distance travelled before they collided.)
	//(-1.0f implies no collision occured.)
	float rayLengths[6] = { -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f };
	//List of collision points found.
	glm::vec4 collisionPoints[6];
	//List of collision normals found.
	glm::vec4 collisionNormals[6];
	//Enumerated references to each face.
	Plane faceRef[6] = { front, back, left, right, top, bottom };
	
	//Set shortestRayIndex to -1 to indicate a ray hasn't been found yet.
	int shortestRayIndex = -1;

	//Loop through faces, testing and validating collisions.
	for (int i = 0; i < 6; i++) {
		if (faceRef[i].testRayPlaneCollision(origin, direction, rayLengths[i], collisionPoints[i], collisionNormals[i])
			&& checkConstraint(collisionPoints[i], faceRef[i])
			&& (shortestRayIndex == -1 || rayLengths[i] < rayLengths[shortestRayIndex]))
			shortestRayIndex = i;
	}

	//If collision found...
	if (shortestRayIndex != -1) {
		//Send collision data of shortest ray back to framework.
		t = rayLengths[shortestRayIndex];
		col_P = collisionPoints[shortestRayIndex];
		col_N = collisionNormals[shortestRayIndex];
		return true;
	}

	return false;
}