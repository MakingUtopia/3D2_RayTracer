#include "ISceneManager.h"
#include "IVirtualObject.h"
#include "RayTracingFramework\ShadingModels\IShadingModel.h"


RayTracingFramework::ISceneManager::ISceneManager() :ID_seed(IVirtualObject::INVALID_OBJECT_ID)
{
	this->shadingModel = new RayTracingFramework::IShadingModel();

}

RayTracingFramework::ISceneManager::~ISceneManager() 
{
}

RayTracingFramework::ISceneManager& RayTracingFramework::ISceneManager::instance(){
	static ISceneManager _instance;
	_instance.getRootNode();//Make sure the root object has ben initialized
	return _instance;
}

RayTracingFramework::IVirtualObject& RayTracingFramework::ISceneManager::getRootNode() {
	static RayTracingFramework::IVirtualObject _root(NULL,NULL, *this);
	return _root;
}


