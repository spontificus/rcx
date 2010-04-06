#ifndef _RCX_GEOM_H
#define _RCX_GEOM_H
#include "component.hpp"
#include "object.hpp"
#include "file_3d.hpp"
#include "script.hpp"
#include <SDL/SDL_stdinc.h> //definition for Uint32

//Geom: (meta)data for geometrical shape (for collision detection), for: 
//contactpoint generation (friction and saftness/hardness). also contains
//rendering data for geom
//
//(contains boolean variable indicating collision - for triggering event script)
//
//>Dynamically allocated
class Geom: public Component
{
	public:
		//methods for creating/destroying/processing Geoms
		Geom (dGeomID geom, Object *obj);
		~Geom();

		//methods for steps/simulations:
		static void Graphics_Step();
		static void TMP_Events_Step(Uint32 step);
		static void Collision_Callback(void *, dGeomID, dGeomID);

		//end of methods, variables:
		//geom data bellongs to
		dGeomID geom_id;

		//Physics data:
		//placeholder for more physics data
		dReal mu, mu_rim, erp, cfm, slip, bounce;

		bool wheel; //true if wheel side slip and connected to hinge2
		dJointID hinge2;

		//End of physics data
		
		file_3d_struct *file_3d; //points to 3d list, or NULL if invisible

		bool collide; //create physical collision when touching other components

		//register if geom is colliding
		bool colliding; //set after each collision

		//geom can respond to collision forces, TODO private:
		void Collision_Force(dReal force); //"damage" geom with specified force
		bool force_to_body; //send forces to connected body instead
		dReal threshold;
		dReal buffer;

		//for events
		Script *script; //script to execute when colliding (NULL if not used)

		//void Set_Buffer(... - TODO
		void Increase_Buffer(dReal add);

		//debug variables
		dGeomID flipper_geom;
		int flipper_counter;

		bool TMP_pillar_geom;
		file_3d_struct *TMP_pillar_graphics; //TMP
	private:
		//used to find next/prev geom in list of all geoms
		//set next to null in last link in chain (prev = NULL in first)
		static Geom *head; // = NULL;
		Geom *prev;
		Geom *next;
};

#endif
