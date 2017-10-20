#ifndef _ILIGHT_RAYTRACINGFRAMEWORK
#define _ILIGHT_RAYTRACINGFRAMEWORK
#include <RayTracingFramework\RayTracingPrerequisites.h>

namespace RayTracingFramework{
	class ILight {
	public:
		ILight(IScene& scene);
		virtual glm::vec4 lightDirectionAtPoint(glm::vec4 pointInWorld)=0;
		virtual float lightDistanceFromPoint(glm::vec4 pointInWorld) = 0;
		virtual float illuminanceAtPoint(glm::vec4 pointInWorld)=0;
		virtual Colour baseColour()=0;

	};
};
#endif