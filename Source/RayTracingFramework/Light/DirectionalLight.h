#ifndef _DIRECTIONALLIGHT_RAYTRACINGFRAMEWORK
#define _DIRECTIONALLIGHT_RAYTRACINGFRAMEWORK
#include <RayTracingFramework\RayTracingPrerequisites.h>
#include <RayTracingFramework\Light\ILight.h>
namespace RayTracingFramework {
	class DirectionalLight :public ILight {
		glm::vec4 directionInWorld;
		RayTracingFramework::Colour colour;		//Perceived light colour (if radiometric is used, this is the colour perceived for luminance = 1Lux)*/
	public:
		DirectionalLight(IScene& scene, glm::vec4 directionInWorld, Colour baseColour = Colour(1, 1, 1))
			: ILight(scene)
			, directionInWorld(glm::normalize(directionInWorld))
			, colour(baseColour)
		{
			;
		}
		virtual glm::vec4 lightDirectionAtPoint(glm::vec4 pointInWorld) {
			return directionInWorld;
		}

		virtual float lightDistanceFromPoint(glm::vec4 pointInWorld) {
			return FLT_MAX;
		}

		virtual float illuminanceAtPoint(glm::vec4 pointInWorld) {
			return 1; //Luminance equivalent to 1 lux --> produces the perceived colour on a lambertian surface.
		}

		virtual Colour baseColour() {
			return colour;
		}

	};
};
#endif
