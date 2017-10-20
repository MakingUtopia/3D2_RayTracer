#include <RayTracingFramework\Light\ILight.h>
#include <RayTracingFramework\VirtualObject\ISceneManager.h>


RayTracingFramework::ILight::ILight(RayTracingFramework::IScene& scene) {
	scene.addLight(this);
}