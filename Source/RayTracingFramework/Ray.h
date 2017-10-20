/**
	CLASS: Ray
	DESCRIPTION: This class contains a description of a ray (origin and direction), but it will also contain all the intersections of this ray with the objects of the scene. 


*/

#ifndef _RAY_RAYTRACINGFRAMEWORK
#define _RAY_RAYTRACINGFRAMEWORK
#include <RayTracingFramework\RayTracingPrerequisites.h>
namespace RayTracingFramework{
	class Ray{
	public:
		/**
			This class is used to describe an intersection of the ray with an object. It describes the colliding object, the point of collision and the normal 
			vector at that point (all in coordinates local to the object). It also contains a matrix to transform these elements to world coordinates (needed to shade, as lights are in world coordinates).
		*/
		struct  Intersection{
		public: 
			float t_distance;
			unsigned int collidingObjectID;
			glm::vec4 collisionPoint_InObjectCoords;					
			glm::mat4 fromObjectToWorldCoords; //The ray would be relative to the camera, the world, etc... this matrix accumulates the transformation to get origin and direction in local coordinates.
			glm::vec4 collisionNormalVector_InObjectCoords;// You might need to add others (collisionNormalVector?)
		};

		//PUBLIC ATTRIBUTES: Description of the ray itself
		glm::vec4 origin_InWorldCoords, direction_InWorldCoords;
		float refractiveIndex;	//Refractive index of the material that the ray is travelling through (it it goping through air, crystal, etc...)
		std::map<float, struct Intersection> intersections;	//Container of all the intersections with objects. The internal order is from smaller to higher distance t (distance to origin of ray).

		/**
			Creates a ray with the specified direction and origin, travelling through a medium with a specific refractive index (default air~vacuum).
		*/
		Ray(glm::vec4 origin_InWorldCoords, glm::vec4 direction_InWorldCoords, float refractiveIndex=1)
			: origin_InWorldCoords(origin_InWorldCoords)
			, direction_InWorldCoords(direction_InWorldCoords)
			, refractiveIndex(refractiveIndex)
		{
			;
		}
		/**
			Add an intersection with an object.
		*/
		inline void addIntersection(Intersection i){
			if(i.t_distance>0)
				intersections[i.t_distance]=i;
		}

		/**
			Returns the closest intersection to the ray origin.
		*/
		Intersection getClosestIntersection(){//Maps order elements internally, using the < operator. This is a cheap way of ordering the intersections we detect...
			if(intersections.size()>0)
				return intersections.begin()->second;
			else{//Signify there were no collisions, by returning an intersection at inifinity.
				Intersection noCollision;
				noCollision.t_distance=FLT_MAX;
				return noCollision;
			}
		}

		/**
			Discards the first intersection (closest to origin). This is usefull when the intersection is behind the camera (negative t), or the first collision is with the starting object (to avoid self-shadowing).
		*/
		void discardClosestIntersection() {
			if(intersections.size()>0)
				intersections.erase(intersections.begin());
		}
	};

};
#endif