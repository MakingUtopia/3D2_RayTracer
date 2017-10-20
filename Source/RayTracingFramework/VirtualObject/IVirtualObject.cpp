#include "IVirtualObject.h"
#include "ISceneManager.h"
#include <RayTracingFramework\Material.h>
#include "RayTracingFramework\GeometricPrimitives\IGeometry.h"
RayTracingFramework::IVirtualObject::IVirtualObject(IGeometry* _geometry, Material* _material, IScene& scene)
	: scene(scene)
	, ID(INVALID_OBJECT_ID)
	, parent_ID(INVALID_OBJECT_ID)
	, fromLocalToParent(1.0f)								//Initialize to identity matrix
	, _fromParentToLocal(1.0f)
	, _fromLocalToWorld(1.0f)								//Initialize to identity matrix
	, _fromWorldToLocal(1.0f)
	, geometry(_geometry)
	, material(_material)											//Default material.
{
	
	//0. Add this object to the registry
	ID = scene.registerVirtualObject(this);
	//1. Add it to the scene graph (Child of root node, by default)
	if(this->getID()!= ROOT_OBJECT_ID)
		scene.getRootNode().addChild(this);//Otherwise, we would add root as child of root (infinite loops are not generally healthy).
	//2. Initialize our components with a pointer to us:
	if(geometry)geometry->setOwner(this);
	if (material)material->setOwner(this);
}

void RayTracingFramework::IVirtualObject::addChild(RayTracingFramework::IVirtualObject* child) {
	//0. notify the previous parent
	if (child->getParentID()!=INVALID_OBJECT_ID){
		IVirtualObject& old_parent = scene.getNodeByID(child->getParentID());
		old_parent.removeChild(child);
	}
	//1. Add the children, and notify it that we are its new parent.
	children[child->getID()] = child;
	child->_updateParentID(this->getID());
	//2. Update world matrices (and propagate changes to sub-children):  
	child->_updateParentWorldPosition(_fromLocalToWorld, _fromWorldToLocal);
}

void RayTracingFramework::IVirtualObject::testCollision(RayTracingFramework::Ray& ray, glm::mat4 fromWorldToParentCoordinates ) {
	//0. #Test the collisions with the local primitive 
	if(geometry)geometry->testLocalCollision(ray);
	//1. Propagate message through all other children.
	for (std::map<unsigned int, IVirtualObject*>::iterator it = children.begin(); it != children.end(); it++)
		it->second->testCollision(ray, this->_fromParentToLocal*fromWorldToParentCoordinates);
}

RayTracingFramework::Material& RayTracingFramework::IVirtualObject::getMaterial() {
	return *material;
}

void RayTracingFramework::IVirtualObject::setMaterial(RayTracingFramework::Material* m) {
	if (material != NULL)
		delete material;
	material = m;
}

RayTracingFramework::IGeometry& RayTracingFramework::IVirtualObject::getGeometry() {
	return *geometry;
}

void RayTracingFramework::IVirtualObject::setGeometry(RayTracingFramework::IGeometry* g) {
	if (geometry != NULL)
		delete geometry;
	geometry = g;
}

RayTracingFramework::IVirtualObject::~IVirtualObject()
{
	scene.deregisterVirtualObject(this);
	if (geometry) delete geometry;
	if (material) delete material;
}
