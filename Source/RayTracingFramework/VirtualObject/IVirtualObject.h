#pragma once
#include <RayTracingFramework\RayTracingPrerequisites.h>

namespace RayTracingFramework{
	class Material;
	class IGeometry;
	class IScene;
	class IVirtualObject
	{
	private: //(Derived classes DO NOT get access to these. They need to use our public methods to access/modify these fields)
		//DESCRIPTION AS A NODE:
		IScene& scene;
		unsigned int ID;						//Unique identifier for the object. The SceneManager helps us get a unique identifier during the construction.
		unsigned int parent_ID;					//Identifier of the parent. Defaults to Root node on creation, but it can change as we attach the object to different parts of the SceneGraph
		glm::mat4 fromLocalToParent;			//Describes how to transform from local coordinates to parent's coordinates
		glm::mat4 _fromParentToLocal;			//... this is the inverse of the above (it is generally a good idea to keep a pre-computed version, as this will be used a lot).
		glm::mat4 _fromLocalToWorld;			//This describes how to transform from object coords to global world coordinates. It is the accumulated transformation of all the nodes above the current one (chained multiplication of all their local matrices). This object makes sure they are correctly maintained
		glm::mat4 _fromWorldToLocal;			//... this is the inverse of the above (it is generally a good idea to keep a pre-computed version, as this will be used a lot).		
		std::map<unsigned int, IVirtualObject*> children;	//Children of the local node.

		//COMPONENTS ATTACHED: Each component has a different functionality
		Material* material;
		IGeometry* geometry;

	public://(Anyone can access these members. It is generally good practice to only have METHODS as public, but no variables).
		static const unsigned int INVALID_OBJECT_ID=0;								//This identifies an incorrect object (or a non existent object).
		static const unsigned int ROOT_OBJECT_ID = 1;								//This identifies the Root object of the Scene Graph.
		IVirtualObject(IGeometry* _geometry, Material* _material, IScene& scene);
		virtual ~IVirtualObject();
	
		/**
			This method returns the ID for the current object. Note the ID cannot be changed (no equivalent "setID" method). IObjectFactory gives a unique IDs during creation, but they cannot be changed afterwards.
		*/
		inline unsigned int getID(){ return ID; }

		/**
		This method returns the ID for the current object's parent. 
		*/
		inline unsigned int getParentID() { return parent_ID; }

		/**
			Return the current position/orientation/scale of the object, relative to its parent node. 
			Transforms coordinates from local space to parent space ( glm::vec4 p_local_to_parent = Object.getLocalToParent() * p_local_to_O)
		*/
		inline glm::mat4 getLocalToParent(){
			return fromLocalToParent;
		}

		/**
			Sets the position of the current object, relative to its parent node. 
			It internally updates the auxiliary matrix (_fromParentToLocal), to make sure they remain synchronized.
			This is the only method that can modify fromLocalToParent and _fromParentToLocal (they are private to this class), so this ensures they will always be synchronized.
			This method also updates transformations to/from world coordinates, and propagates changes to children.
		*/
		inline void setLocalToParent(glm::mat4 m){
		
			glm::mat4 prev_fromLocalToParent = fromLocalToParent, prev_fromParentToLocal = _fromParentToLocal;
			//0. Compute the positions of our parent to/from world.
			glm::mat4 parentToWorld = _fromLocalToWorld*prev_fromParentToLocal; 
			glm::mat4 worldToParent = prev_fromLocalToParent*_fromWorldToLocal;
			//1. Update local matrices. 
			fromLocalToParent=m;
			_fromParentToLocal = glm::inverse(m);
			//2. Update world matrices (and propagate changes to children): 
			_updateParentWorldPosition(parentToWorld, worldToParent); 
		}

		
		/**
			Return the current position/orientation of the parent, relative to this object.
			Transforms coordinates from parent space to local space (e.g. glm::vec4 p_local_to_O = Object.getParentToLocal() * p_local_to_parent)
		*/
		inline glm::mat4 getParentToLocal(){
			return _fromParentToLocal;
		}

		
		/**
			Return the position/orientation of World's system of reference, relative to this object.
			Transforms coordinates from world space to local space (e.g. glm::vec4 p_local_to_O = Object.getFromWorldToObjectCoordinates() * p_world)
		*/
		inline glm::mat4 getFromWorldToObjectCoordinates() {
			return _fromWorldToLocal;
		}

		
		/**
			Return the current position/orientation of the object, in the world.
			Transforms coordinates from object space to world space (e.g. glm::vec4 p_world = Object.getFromObjectToWorldCoordinates() * p_local_to_O)
		*/
		inline glm::mat4 getFromObjectToWorldCoordinates() {
			return _fromLocalToWorld;
		}

		Material& getMaterial();

		void setMaterial(Material* m);

		IGeometry& getGeometry();

		void setGeometry(IGeometry* g);

		/**
			This method traverses the scene graph of this object's children, detecting collisions of the ray with all of their geometries and generating a descriptor for the Intersection point/s.
			@Result ray: Parameter ray contains a list with al intersections found, ordered according to distance.  
			This method cannot be overwriten by subclasses, so this behaviour cannot be changed.
		*/
		void testCollision(RayTracingFramework::Ray& ray, glm::mat4 fromWorldToParentCoordinates = glm::mat4(1.0f));

		/**
			Add a child to the current object.
		*/
		void addChild(RayTracingFramework::IVirtualObject* child);

		/**
			Remove a child from the current object (if it exists). The child object is returned, so that the caller can re-use it (e.g. add it in another place of the scene)
		*/
		IVirtualObject* removeChild(unsigned int ID){
			std::map<unsigned int, IVirtualObject*>::iterator it = children.find(ID); //Look for the children with that identifier
			IVirtualObject* result = NULL; 
			if (it != children.end()){//If found: 
				result = it->second;	//we keep a pointer to it
				children.erase(it);		//we remove it from our map (this does not delete the object).
				result->_updateParentWorldPosition(glm::mat4(1.0f), glm::mat4(1.0f));//It is no-one's child now -> Update its world matrices (and propagate changes to sub-children):  
				return result;			//we return the object.
			}
			return NULL;
		}

		/**
			Remove a child from the current object (if it exists). The child object is returned, so that the caller can re-use it (e.g. add it in another place of the scene)
		*/
		inline IVirtualObject* removeChild(RayTracingFramework::IVirtualObject* o){ return removeChild(o->getID()); }


	private: 
		inline void _updateParentID(unsigned int newParent) {
			parent_ID = newParent;
		}

		inline void _updateParentWorldPosition(glm::mat4 fromParentToWorld, glm::mat4 fromWorldToParent) {
			//1. Update world matrices: 
			_fromLocalToWorld = fromParentToWorld *fromLocalToParent;
			_fromWorldToLocal = _fromParentToLocal * fromWorldToParent;
			//3. Now, lets update our children (Our changes will also affect their matrices...)
			for (std::map<unsigned int, IVirtualObject*>::iterator it = children.begin(); it != children.end(); it++)
				it->second->_updateParentWorldPosition(_fromLocalToWorld, _fromWorldToLocal);
		}
	};

};
