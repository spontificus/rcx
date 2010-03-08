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
		Geom (dGeomID geom, object_struct *obj);
		~Geom();

		//methods for steps/simulations:
		static void Graphics_Step();
		static void TMP_Events_Step(Uint32 step);

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

		bool colliding; //set after each collision
		script_struct *script; //script to execute when colliding (NULL if not used)

		//debug variables
		dGeomID flipper_geom;
		int flipper_counter;

	private:
		//used to find next/prev geom in list of all geoms
		//set next to null in last link in chain (prev = NULL in first)
		static Geom *head; // = NULL;
		Geom *prev;
		Geom *next;
};

#endif