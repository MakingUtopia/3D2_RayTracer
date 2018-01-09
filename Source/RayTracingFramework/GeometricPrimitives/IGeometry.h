#ifndef _GEOMETRY_RAYTRACINGFRAMEWORK
#define _GEOMETRY_RAYTRACINGFRAMEWORK
#include <RayTracingFramework\RayTracingPrerequisites.h>
#include <RayTracingFramework\VirtualObject\IVirtualObject.h>

namespace RayTracingFramework{
	/*
		Component (part of IVirtualObject), describing the geometry/shape of the 3D object.
		It allows us to detect colisions of a ray with the geometry, described as intersections.
	*/
	class IGeometry
	{
	protected:
		IVirtualObject* owner;
	public:
		virtual ~IGeometry() { ; }
		inline void setOwner(IVirtualObject* _owner) {
			owner = _owner;
		}
		virtual bool testLocalCollision(Ray& ray) { return false; }
	};

};
#endif