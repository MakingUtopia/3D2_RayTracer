#ifndef _MATERIAL_RAYTRACINGFRAMEWORK
#define _MATERIAL_RAYTRACINGFRAMEWORK
#include <RayTracingFramework\RayTracingPrerequisites.h>

namespace RayTracingFramework{

	/*
	Component (part of IVirtualObject), describing the material properties of an object.
	It is a simple collection of values (struct).
	*/
	class Material{
	protected:
		IVirtualObject* owner;
	public: 
		RayTracingFramework::Colour diffuseColour, specularColour; 
		float K_a, K_d, K_s, shininess;									//Shading coefficients for ambient, diffuse and specular components (shininess describes the power for K_s components);
		float K_t, K_r;													//Reflectiveness and transparency of the object.			
		float refractiveIndex;											//Refraction Index
		cimg_library::CImg<unsigned char> *diffuseMap, *specularMap;	//UV map for diffuse colour and specular reflections (extra marks)
		
		//Default constructor: Matte red material.
		Material():
			diffuseColour(1,0,0), specularColour (1,1,1)
			, K_a(0), K_d(1), K_s(0), shininess(3) 
			, K_t (0), K_r(0), refractiveIndex(1)
			, diffuseMap(0)
			, specularMap(0)
			{ ; }
		inline void setOwner(IVirtualObject* _owner) {
			owner = _owner;
		}
		virtual ~Material() { ; }
	};
};

#endif