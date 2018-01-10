#ifndef _BOXGEOMETRY_RAYTRACINGFRAMEWORK
#define _BOXGEOMETRY_RAYTRACINGFRAMEWORK
#include <RayTracingFramework\RayTracingPrerequisites.h>
#include "IGeometry.h"
#include "Square.h"

namespace RayTracingFramework {
	class Box : public IGeometry {
		glm::vec4 A, B;
		Plane front, back, left, right, top, bottom;
	public:
		Box(glm::vec4 pointA, glm::vec4 pointB);
		virtual bool testLocalCollision(RayTracingFramework::Ray& ray);
	private:
		bool checkConstraint(glm::vec3 collisionPoint, Plane& faceRef);
		bool testRayBoxCollision(glm::vec4 origin, glm::vec4 direction, float& t, glm::vec4& col_P, glm::vec4& col_N);
	};
}

#endif