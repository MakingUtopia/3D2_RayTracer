#ifndef _SPHEREGEOMETRY_RAYTRACINGFRAMEWORK
#define _SPHEREGEOMETRY_RAYTRACINGFRAMEWORK
#include <RayTracingFramework\RayTracingPrerequisites.h>
#include "IGeometry.h"

namespace RayTracingFramework{

	class ISphere : public IGeometry
	{
		float radius;
	public:
		ISphere(float radius);
		virtual bool testLocalCollision(RayTracingFramework::Ray& ray);
		int testRaySphereCollision(glm::vec4 origin_local, glm::vec4 direction_local
			, float &t1, glm::vec4& collision_Point1, glm::vec4& collision_Normal1
			, float &t2, glm::vec4& collision_Point2, glm::vec4& collision_Normal2);
	};

};
#endif