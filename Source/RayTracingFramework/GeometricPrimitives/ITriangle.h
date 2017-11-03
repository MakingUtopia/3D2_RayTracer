#ifndef _TRIANGLEGEOMETRY_RAYTRACINGFRAMEWORK
#define _TRIANGLEGEOMETRY_RAYTRACINGFRAMEWORK
#include <RayTracingFramework\RayTracingPrerequisites.h>
#include "IGeometry.h"

namespace RayTracingFramework{

	class ITriangle : public IGeometry
	{
		glm::vec4 A, B, C;
		//To represent equation system related to D*[t, beta, sigma]=Res
		glm::mat3 D, D_t, D_beta, D_sigma;	//These is the equation system and the determinants to solve it using Cramer's rule.
		glm::vec3 Res;
	public:
		ITriangle(glm::vec4 A, glm::vec4 B, glm::vec4 C)
			: A(A)
			, B(B)
			, C(C)
		{; }
		virtual bool testLocalCollision(RayTracingFramework::Ray& ray);
		bool testRayTriangleCollision(glm::vec4 origin_local, glm::vec4 direction_local
			, float &t, glm::vec4& collision_Point, glm::vec4& collision_Normal);
	};

};
#endif