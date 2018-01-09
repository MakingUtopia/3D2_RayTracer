#include "Square.h"
#include "RayTracingFramework\Ray.h"
#include "RayTracingFramework\VirtualObject\IVirtualObject.h"

RayTracingFramework::Rectangle::Rectangle(glm::vec4 p0, glm::vec4 n, glm::vec2 a, glm::vec2 b)
	: A (a)
	, B (b)
	, Plane(p0, n) {
	;
}

bool RayTracingFramework::Rectangle::testLocalCollision(RayTracingFramework::Ray& ray) {
	testLocalCollision(ray);
	return true;
}

bool RayTracingFramework::Rectangle::testRayPlaneCollision(glm::vec4 origin, glm::vec4 direction, float& t, glm::vec4& col_P, glm::vec4& col_N) {
	testRayPlaneCollision(origin, direction, t, col_P, col_N);
	
	//Find mins & maxes
	float x_min, x_max,
		y_min, y_max,
		z_min, z_max;
	x_min = (A.x < B.x) ? A.x : B.x;
	x_max = (A.x > B.x) ? A.x : B.x;
	y_min = (A.y < B.y) ? A.y : B.y;
	y_max = (A.y > B.y) ? A.y : B.y;

	//Find where collision point is on 2D plane.
	//glm::vec2 Q = col_p.

	//Check collision point is within bounds.
	if (col_P.x < x_min)
		return false;
	if (col_P.x >= x_max)
		return false;
	if (col_P.y < y_min)
		return false;
	if (col_P.y >= y_max)
		return false;
	if (col_P.z < z_min)
		return false;
	if (col_P.z >= z_max)
		return false;
}