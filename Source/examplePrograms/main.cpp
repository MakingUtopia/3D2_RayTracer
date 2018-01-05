#include "RayTracingFramework\VirtualObject\ISceneManager.h"
#include "RayTracingFramework\Ray.h"
#include "RayTracingFramework\VirtualObject\Camera\Camera.h"
#include "RayTracingFramework\VirtualObject\IVirtualObject.h"
#include "RayTracingFramework\ShadingModels\IShadingModel.h"
#include "RayTracingFramework\GeometricPrimitives\IGeometry.h"
#include "RayTracingFramework\GeometricPrimitives\Plane.h"
#include "RayTracingFramework\GeometricPrimitives\ISphere.h"
#include "RayTracingFramework\GeometricPrimitives\ITriangle.h"
//Add your new geometries here as you implement them.
//

#include "RayTracingFramework\Material.h"
#include "RayTracingFramework\Light\ILight.h"
#include "RayTracingFramework\Light\DirectionalLight.h"
//Add your new types of lights here.
//

using namespace cimg_library;

//Create scene declaration.
RayTracingFramework::IScene& createScene();				

/*
 * MAIN
 * Run entire program:
 * - Create image.
 * - Create scene.
 * - Define camera.
 * - Perform raytracing.
 * - Save image & display.
 */
int main(int arg, char **argv)
{
	//Create image: params -> (width, height, bytes per colour channel, number of colour channels)
	int imageWidth = 600, imageHeight = 600;
	CImg<unsigned char> img = CImg<unsigned char>(imageWidth, imageHeight, 1, 3);
	//Fill with grey.
	img.fill((const unsigned char)128);
	//Display image in window.
	CImgDisplay disp(img, "Ray tracing output", false);
	
	//Create scene.
	RayTracingFramework::IScene& scene = createScene();

	//Define Camera.
	//Create camera using fields top, bottom, left, right, near and far
	RayTracingFramework::Camera cam(scene, imageWidth, imageHeight, 1, -1, -1, 1, 1, 1000);								
	
	//Perform raytracing.
	for (int r = 0; r < imageHeight; r++) {
		for (int c = 0; c < imageWidth; c++) {
			//Create a single ray per pixel.
			RayTracingFramework::Ray ray = cam.createPrimaryRay(c, r);

			//Test collisions.
			RayTracingFramework::ISceneManager::instance().getRootNode().testCollision(ray, glm::mat4(1.0f));
			
			//Check there are any valid collisions.
			//Discard collisions behind camera.
			while (ray.getClosestIntersection().t_distance < 0) ray.discardClosestIntersection();
			//If there are none closer than the max distance.
			if (ray.getClosestIntersection().t_distance == FLT_MAX)
				continue;

			//Get scene, & compute shaded colour for this pixel. 
			RayTracingFramework::IScene& scene = (RayTracingFramework::IScene&)RayTracingFramework::ISceneManager::instance();
			RayTracingFramework::Colour shadedColour = scene.getShadingModel().computeShading(ray, scene, 2);

			//Assign the computed colour to the pixel in the output image.
			img(c, r, 0) = (unsigned char)((shadedColour.r <= 1 ? shadedColour.r : 1) * 255);// (r % 256);
			img(c, r, 1) = (unsigned char)((shadedColour.g <= 1 ? shadedColour.g : 1) * 255);
			img(c, r, 2) = (unsigned char)((shadedColour.b <= 1 ? shadedColour.b : 1) * 255);
		}
		//Update display each time row is rendered.
		disp.display(img);
	}
	
	//Save image to file and display in window for 30 seconds.
	img.save("rayTracingResult.bmp");
	while (!disp.is_closed())
		disp.display(img).wait(30);

	return 0;
}

/* 
 * CREATE SCENE
 * - Gets/creates scene instance.
 * - Creates geometries & materials.
 * - Makes virtual objects using geometries & materials.
 * - Applies transformations to virtual objects.
 * - Creates lights.
 */
RayTracingFramework::IScene& createScene() {
	//Get scene instance, or create one if it doesn't exist.
	RayTracingFramework::IScene& scene = RayTracingFramework::ISceneManager::instance();
	
	//Create geometries, materials & virtual objects...
	//(Upon creation, virtual objects are automatically added to the scene.)

	//Horizontal plane
	//Geometry
	RayTracingFramework::IGeometry*g = (RayTracingFramework::IGeometry*)new RayTracingFramework::Plane(glm::vec4(0, -30, 0, 1), glm::vec4(0, 1, 0, 0));
	//Material
	RayTracingFramework::Material*m = new RayTracingFramework::Material;
	m->K_a = 0.15f; 
	m->K_d = 0.85f;
	m->diffuseColour = RayTracingFramework::Colour(0.65f, 0, 1);
	//Virtual Object
	RayTracingFramework::IVirtualObject* groundPlane = new RayTracingFramework::IVirtualObject(g, m, scene);

	//Sphere
	//Geometry
	RayTracingFramework::IGeometry*g2 = (RayTracingFramework::IGeometry*)new RayTracingFramework::ISphere(20);
	//Material
	RayTracingFramework::Material*m2 = new RayTracingFramework::Material;
	m2->K_a = 0.15f;	//ambient coefficient
	m2->K_d = 0.85f;	//diffuse coefficient
	m2->diffuseColour = RayTracingFramework::Colour(0.05f, 0.70f, 0.2);			
	//Virtual Object
	RayTracingFramework::IVirtualObject* sphere = new RayTracingFramework::IVirtualObject(g2, m2, scene);
	//Apply transformation
	sphere->setLocalToParent(glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 70)));

	//Triangle
	//Geometry
	RayTracingFramework::IGeometry*g3 = (RayTracingFramework::IGeometry*)new RayTracingFramework::ITriangle(
		glm::vec4(-10.0f, -10.0f, 0.0f, 0.0f),
		glm::vec4(0.0f, 10.0f, 0.0f, 0.0f),
		glm::vec4(10.0f, -10.0f, 0.0f, 0.0f)
	);
	//Material
	RayTracingFramework::Material*m3 = new RayTracingFramework::Material;
	m3->K_a = 0.15f;	//ambient coefficient
	m3->K_d = 0.85f;	//diffuse coefficient
	m3->diffuseColour = RayTracingFramework::Colour(0.0f, 0.00f, 0.80f);
	//Virtual Object
	RayTracingFramework::IVirtualObject* triangle = new RayTracingFramework::IVirtualObject(g3, m3, scene);
	//Apply transformation
	triangle->setLocalToParent(glm::translate(glm::mat4(1.0f), glm::vec3(-15, 0, 40)));

	//Create Lights

	//Directional Light
	RayTracingFramework::DirectionalLight* pl = new RayTracingFramework::DirectionalLight(scene, glm::vec4(0.5, -0.5, 0.5, 0));

	return scene;
}