#ifndef _PLANEGEOMETRY_RAYTRACINGFRAMEWORK
#define _PLANEGEOMETRY_RAYTRACINGFRAMEWORK
#include <RayTracingFramework\RayTracingPrerequisites.h>
#include "IGeometry.h"

namespace RayTracingFramework{

	class Plane : public IGeometry
	{
		glm::vec4 P0, N;		//Origin point and Normal vector describing the plane.
		float A, B, C, D;		//Variables from the implicit equations. Ax + By +Cz +D=0
	public:
		Plane(glm::vec4 P0, glm::vec4 N);
		virtual bool testLocalCollision(RayTracingFramework::Ray& ray);
	private: 
		/**
			Computes a collision of a ray (in coords local to the plane) with the plane
			@param origin, direction: definition of the ray (origin point and unitary direction vector)
			@param t (Output parameter): Distance of the intersection point to the origin
			@param col_P (Output parameter): Point of intersection (point on the plane where the ray hits the plane)
			@param col_N (Output parameter): Normal of the plane at the intersection point (... all points on a plane share the same normal vector).

		*/
		bool testRayPlaneCollision(glm::vec4 origin, glm::vec4 direction
					, float& t, glm::vec4& col_P, glm::vec4& col_N);
	};
};
#endif