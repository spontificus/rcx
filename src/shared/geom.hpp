#ifndef _RCX_GEOM_H
#define _RCX_GEOM_H
#include "component.hpp"
#include "body.hpp"
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
		static void Physics_Step();
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
		
		file_3d *f_3d; //points to 3d list, or NULL if invisible


		//geom tweaks:
		bool collide; //create physical collision when touching other components

		//debug variables
		dGeomID flipper_geom;
		int flipper_counter;

		bool TMP_pillar_geom;
		file_3d *TMP_pillar_graphics; //TMP

		//register if geom is colliding
		bool colliding; //set after each collision

		//for buffer events
		void Set_Buffer_Event(dReal thresh, dReal buff, Script *scr);
		void Increase_Buffer(dReal add);
		void Set_Buffer_Body(Body*); //send damage to body instead
		void Damage_Buffer(dReal force); //"damage" geom with specified force

		//sensor events
		void Set_Sensor_Event(Script *s1, Script *s2);

	private:
		//events:
		bool buffer_event;
		bool sensor_event;
		//bool radar_event; - TODO

		//sensor events:
		bool sensor_last_state; //last state of sensor: enabled or disabled
		Script *sensor_triggered_script, *sensor_untriggered_script;

		//buffer events:
		Body *force_to_body; //send forces to this body instead

		//normal buffer handling
		dReal threshold;
		dReal buffer;
		Script *buffer_script; //script to execute when colliding (NULL if not used)


		//used to find next/prev geom in list of all geoms
		//set next to null in last link in chain (prev = NULL in first)
		static Geom *head; // = NULL;
		Geom *prev;
		Geom *next;
		friend void Body::TMP_Events_Step(Uint32 step); //this is just TMP for accessing above...
};

#endif
