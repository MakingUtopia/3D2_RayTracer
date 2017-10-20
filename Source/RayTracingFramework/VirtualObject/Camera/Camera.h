#ifndef _CAMERA_RAYTRACINGFRAMEWORK
#define _CAMERA_RAYTRACINGFRAMEWORK
#include <RayTracingFramework\RayTracingPrerequisites.h>
#include <RayTracingFramework\VirtualObject\IVirtualObject.h>

namespace RayTracingFramework{

	class Camera: public IVirtualObject
	{
	protected:
		//Define camera parameters in image space (size of the sensor, in pixels):
		int pixelWidth, pixelHeight; 
		//Define the camera parameters in 3D space (the "View volume" of our camera).
		glm::vec4 topLeft, topRight, bottomLeft, bottomRight;	//These are equivalent to points A,B,C and D in the slides
		float _near, _far; 
		//Please note: the 3D location/orientation of the camera is implicitly contained in field "localToParent" (inherited from IVirtualObject)
		virtual glm::vec4 _createLocalPrimaryRay(int x_pixel, int y_pixel);
	public:
		Camera(IScene& scene, int width, int height, float top, float bottom, float left, float right, float n, float f);
	
		virtual ~Camera();

		Ray createPrimaryRay(int x_pixel, int y_pixel);
	};

};
#endif