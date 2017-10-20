#include "RayTracingFramework\VirtualObject\ISceneManager.h"
#include "RayTracingFramework\Ray.h"
#include "RayTracingFramework\VirtualObject\Camera\Camera.h"
#include "RayTracingFramework\VirtualObject\IVirtualObject.h"
#include "RayTracingFramework\ShadingModels\IShadingModel.h"
#include "RayTracingFramework\GeometricPrimitives\IGeometry.h"
#include "RayTracingFramework\GeometricPrimitives\Plane.h"
//Add your new geometries here as you implement them.
#include "RayTracingFramework\Material.h"
#include "RayTracingFramework\Light\ILight.h"
#include "RayTracingFramework\Light\DirectionalLight.h"
//Add your new types of lights here.

using namespace cimg_library;

RayTracingFramework::IScene& createScene();				//Function declaration (it's implementation can be found after the main method)

int main(int arg, char **argv)
{
	//0. Create output frame (600x600 pixels) 
	int imageWidth = 600, imageHeight = 600;
	//0.We create an image of the specified size. The image uses 1 byte per channel (0..255), and three chanels per pixel. That describes a conventional RGB image.
	CImg<unsigned char> img= CImg<unsigned char>(imageWidth, imageHeight, 1, 3);
	img.fill((const unsigned char)128);//The image is filled up with (128,128,128) values-> medium grey
	CImgDisplay disp(img, "Ray tracing output", false);//1. We create a window to display our image. First parameter defines the size of the window (size of our image). The other parameters describe the title and no use of full screen (false).
	
	//1. Define scene
	RayTracingFramework::IScene& scene=createScene();

	//2. Define camera
	RayTracingFramework::Camera cam(scene, imageWidth, imageHeight, 1, -1, -1, 1, -1, -100);								//create camera using fields top, bottom, left, right, near and far

	//3. Raytracing loop. 
	for (int r = 0; r < imageHeight;r++){
		for (int c = 0; c < imageWidth; c++) {
			//2.1. Create one ray per pixel
			RayTracingFramework::Ray ray = cam.createPrimaryRay(c, r); 
			//2.2. Test collisions
			RayTracingFramework::ISceneManager::instance().getRootNode().testCollision(ray, glm::mat4(1.0f));
			//2.3. Look for closest collision and compute shading
			RayTracingFramework::Colour shadedColour;
			while (ray.getClosestIntersection().t_distance < 0)ray.discardClosestIntersection(); //Intersections behind the camera... let's ignore them
			if (ray.getClosestIntersection().t_distance == FLT_MAX)
				continue; //There have been no collisions.
			//2.4. There is a positive collision, and not at infinite-> Cool, we have something to paint!!
			//Shade using that list of intersections and the material.
			RayTracingFramework::IScene& scene = (RayTracingFramework::IScene&)RayTracingFramework::ISceneManager::instance();
			shadedColour = scene.getShadingModel().computeShading(ray, scene, 2);

			//2.4. Apply colour to pixel.
			img(c, r, 0) = (unsigned char)((shadedColour.r <= 1 ? shadedColour.r : 1) * 255);// (r % 256);
			img(c, r, 1) = (unsigned char)((shadedColour.g <= 1 ? shadedColour.g : 1) * 255);
			img(c, r, 2) = (unsigned char)((shadedColour.b <= 1 ? shadedColour.b : 1) * 255);
		}
		disp.display(img);
	}
	img.save("rayTracingResult.bmp");
	//Done! Let's enjoy our nice little picture...
	while(!disp.is_closed())
		disp.display(img).wait(30);

	
	return 0;
}

RayTracingFramework::IScene& createScene() {
	//0. Get access to the scene (automatically created the first time we call ISceneManager::instance())
	RayTracingFramework::IScene& scene = RayTracingFramework::ISceneManager::instance();
	//1. Fill in the scene with some geometry:

	//1.1. A horizontal plane: 
	//  A) Its geometry: A plane described using the position of its origin, as point (0,-30,0,1); and its normal vector (0,1,0,0).
	RayTracingFramework::IGeometry*g = (RayTracingFramework::IGeometry*)new RayTracingFramework::Plane(glm::vec4(0, -30, 0, 1), glm::vec4(0, 1, 0, 0));
	//  B) Its material:
	RayTracingFramework::Material*m = new RayTracingFramework::Material;
	m->K_a = 0.15f; m->K_d = 0.85f; m->diffuseColour = RayTracingFramework::Colour(0.65f, 0, 1);			//Material defines other properties (not used yet). 
	//  C) The object itself (using the geometry and meterial described). Creating an object automatically adds it to the scene.
	RayTracingFramework::IVirtualObject* groundPlane = new RayTracingFramework::IVirtualObject(g, m, scene);

	//1.2. A simple light: Creating it automatically adds it to the scene. 
	RayTracingFramework::DirectionalLight* pl = new RayTracingFramework::DirectionalLight(scene, glm::vec4(-1, -1, 0, 0));

	return scene;
}