#ifndef _PREREQUISITES_RAYTRACINGFRAMEWORK
#define _PREREQUISITES_RAYTRACINGFRAMEWORK
#include <external\CImg\CImg.h>
#include <external\glm\glm.hpp>
#include <external\glm\gtc\matrix_transform.hpp>
#include <map>

namespace RayTracingFramework{
	typedef glm::vec3 Colour;
	class Ray;
	class ILight;
	class ICamera;
	class IGeometry;
	class IPlane;
	class IMaterial;
	class IShadingModel;
	class IVirtualObject;
	class IScene;
};
#endif