#ifndef _RCX_BODY_H
#define _RCX_BODY_H
#include <ode/ode.h>
#include <SDL/SDL.h>
#include "script.hpp"
#include "object.hpp"
#include "component.hpp"
#include "script.hpp"

//body_data: data for body (describes mass and mass positioning), used for:
//currently only for triggering event script (force threshold and event variables)
//as well as simple air/liquid drag simulations
//
//>Dynamically allocated
class Body: public Component
{
	public:
		//methods
		Body (dBodyID body, Object *obj);
		~Body();

		void Set_Event(dReal thresh, dReal buff, Script *scr);
		void Set_Linear_Drag(dReal drag);
		void Set_Angular_Drag(dReal drag);
		void Set_Advanced_Linear_Drag(dReal x, dReal y, dReal z);

		static void Physics_Step();
		static void TMP_Events_Step(Uint32 step);

		//body data bellongs to
		dBodyID body_id;

	private:
		//used to find next/prev link in dynamically allocated chain
		//set next to null in last link in chain (prev = NULL in first)
		Body *prev, *next;
		static Body *head;

		//data for drag (air+water friction)
		//values for enabled/disabled drag
		bool use_linear_drag, use_advanced_linear_drag;
		bool use_angular_drag;
		//drag values (must be adjusted to the body mass)
		dReal linear_drag, advanced_linear_drag[3];
		dReal angular_drag;

		//event processing
		dReal threshold; //if allocated forces exceeds, eat buffer
		dReal buffer; //if buffer reaches zero, trigger event
		bool event; //set after each buffer empty
		Script *script; //execute on event

		//private methods for drag
		void Linear_Drag();
		void Angular_Drag();
		void Advanced_Linear_Drag();
};

#endif
