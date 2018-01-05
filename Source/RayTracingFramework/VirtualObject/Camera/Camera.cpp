#include "Camera.h"
#include "RayTracingFramework\Ray.h"

RayTracingFramework::Camera::Camera(IScene& scene, int width, int height, float top, float bottom, float left, float right, float n, float f)
    : IVirtualObject(NULL,NULL,scene)
	, pixelWidth(width), pixelHeight(height)
	, topLeft    (left, top, n, 1.0f)
	, topRight   (right, top, n, 1.0f)
	, bottomLeft (left, bottom, n, 1.0f)
	, bottomRight(right, bottom, n, 1.0f)
	, _near(n)
	, _far(f)
{
	;
}


RayTracingFramework::Camera::~Camera()
{
	//We need to release any memory we allocated dynamically, suring object creation. 
	//As all our variables are basic objects (no pointers), we have nothing to do.
}

RayTracingFramework::Ray RayTracingFramework::Camera::createPrimaryRay(int x_pixel, int y_pixel){
	//1. Compute the ray on coordinates local to the camera (Workshop 2)
	glm::vec4 origin_local(0, 0, 0, 1);
	glm::vec4 direction_local = _createLocalPrimaryRay(x_pixel, y_pixel);
	//2. Transform it to world coordinates (explained in workshop 3):
	glm::vec4 origin_world=this->getFromObjectToWorldCoordinates() * origin_local;
	glm::vec4 direction_world=this->getFromObjectToWorldCoordinates() * direction_local;
	//3. Return ray in world coordinates.
	return  RayTracingFramework::Ray(origin_world,direction_world);
}

glm::vec4 RayTracingFramework::Camera::_createLocalPrimaryRay(int x_pixel, int y_pixel){
	//0. Check if we are in bounds (in pixel space).	
	if (x_pixel < 0 || x_pixel >= pixelWidth
		|| y_pixel < 0 || y_pixel >= pixelHeight)
		return glm::vec4(0, 0, 0, 0.0f);

	//2. Compute position of pixel P(x_pixel, y_pixel)
	float nearWidth = topRight.x - topLeft.x;
	float nearHeight = bottomLeft.y - topLeft.y;
	float proportionWidth = (float)x_pixel / (float)pixelWidth;
	float proportionHeight = (float)y_pixel / (float)pixelHeight;
	glm::vec4 P = topLeft + glm::vec4(nearWidth * proportionWidth, nearHeight * proportionHeight, 0, 0);

	//O->P
	glm::vec4 OP = glm::normalize(P);

	//3. Return direction vector OP. Normalize before returning (glm::normalize(<your vector>)) 
	return OP;


}