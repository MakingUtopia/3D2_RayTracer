#pragma once
#include <RayTracingFramework\RayTracingPrerequisites.h>
#include <RayTracingFramework\VirtualObject\IVirtualObject.h>
#include <RayTracingFramework\Light\ILight.h>
#include <RayTracingFramework\ShadingModels\IShadingModel.h>
#include <vector>
namespace RayTracingFramework{

	/**
		Interface definition for a scene. This will allow full control of the scene, keeping track of the objects, lights and shading model to use.
	*/
	class IScene {
		friend class IVirtualObject;//It can access private/protected members.
		friend class ILight;
	protected:
		//These methods are only accesible to subclasses and to "friend" classes/methods. During object and light creation, they invoke these methods to be added to the SceneManager registry of lights and objects.
		virtual unsigned int registerVirtualObject(IVirtualObject* o) = 0;			
		virtual void deregisterVirtualObject(IVirtualObject* o) = 0;
		virtual void addLight(ILight* l) = 0;
	public:
		/**
			Returns the base node of the SceneGraph
		*/
		virtual IVirtualObject& getRootNode()=0;
		/**
			Returns a reference to the Virtual Object with the given ID. 
		*/
		virtual IVirtualObject& getNodeByID(unsigned int ID) = 0;

		/**
			Returns the class that shades the objects in the scene. All objects will use the same "shader". 
		*/
		virtual IShadingModel& getShadingModel() = 0;
		/**
			Returns a list with the lights currently defined in the scene.
		*/
		virtual std::vector<ILight*> getLights() = 0;
	};

	/**
		Implementation class for a IScene. This implementation makes use of the Singleton pattern, so only one Scene will be active at a time.
	*/
	class ISceneManager: public IScene
	{
		unsigned int ID_seed;								//This keeps the next ID we will provide for a newly created object (we do not want ID duplicates)
		std::map<unsigned int, IVirtualObject*> registry;	//Database with all the objects that exist in the scene. It allows us to quickly retrieve them by ID.
		IShadingModel* shadingModel;						//Shading model to use. All objects are shaded in the same way
		std::vector<ILight*> lights;						//Lights defined in the scene.
		unsigned int assignNextValidID(){					//Assigns a valid ID to an object (It is called during object creation)
			return ++ID_seed; //Increases value before returning--> It will never return INVALID_OBJECT_ID as an ID.
		}
		ISceneManager();									//Private constructor: Cannot be created directly --> This is part of the Singleton Pattern
	public:

		//OWN METHODS: 
		static ISceneManager& instance();					//Public method: Used to get the unique object	--> This is part of the Singleton pattern

		inline void setShadingModel(IShadingModel* s) {
			shadingModel = s;
		}

		//METHODS INHERITED FROM THE INTERFACE: 
		virtual IVirtualObject& getRootNode();				
		virtual IVirtualObject& getNodeByID(unsigned int ID) {
			return *(registry[ID]);
		}

		virtual IShadingModel& getShadingModel() {
			return *shadingModel;
		}

		virtual std::vector<ILight*> getLights() {
			return lights;
		}

		~ISceneManager();
	protected: 
		virtual unsigned int registerVirtualObject(IVirtualObject* o) {
			unsigned int ID = assignNextValidID();
			registry[ID] = o;
			return ID;
		}
		virtual void deregisterVirtualObject(IVirtualObject* o) {
			std::map<unsigned int, IVirtualObject*>::iterator it = registry.find(o->getID());
			if (it != registry.end())
				registry.erase(it);
		}
		virtual void addLight(ILight* l) {
			lights.push_back(l);
		}
	};

};