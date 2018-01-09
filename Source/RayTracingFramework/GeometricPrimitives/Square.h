#ifndef _SQUAREGEOMETRY_RAYTRACINGFRAMEWORK
#define _SQUAREGEOMETRY_RAYTRACINGFRAMEWORK
#include <RayTracingFramework\RayTracingPrerequisites.h>
#include "Plane.h"

namespace RayTracingFramework {
	class Rectangle : public Plane
	{
		glm::vec2 A, B;
	public:
		Rectangle(glm::vec4 p0, glm::vec4 n, glm::vec2 A, glm::vec2 B);
		virtual bool testLocalCollision(RayTracingFramework::Ray& ray);
		virtual bool testRayPlaneCollision(glm::vec4 origin, glm::vec4 direction, float& t, glm::vec4& col_P, glm::vec4& col_N);
	};
}

#endif