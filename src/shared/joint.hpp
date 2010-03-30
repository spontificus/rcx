#ifndef _RCX_JOINT_H
#define _RCX_JOINT_H
#include <ode/ode.h>
#include <SDL/SDL.h>
#include "object.hpp"
#include "component.hpp"
#include "script.hpp"
#include "printlog.hpp"

//Joint: (meta)data for joint (connects bodies), is used for:
//currently only for triggering event script (force threshold)
//
//>Dynamically allocated
class Joint: public Component
{
	public:
		Joint (dJointID joint, Object *obj);
		~Joint();

		void Set_Event(dReal thresh, dReal buff, Script *scr);

		static void Physics_Step();
		static void TMP_Events_Step(Uint32 step);

		//geom data bellongs to
		dJointID joint_id;

	private:
		//used to find next/prev link in dynamically allocated chain
		//set next to null in last link in chain (prev = NULL in first)
		static Joint *head;
		Joint *prev, *next;

		//for event processing
		dJointFeedback *feedback; //used if checking forces
		dReal threshold; //if force on body exceeds threshold, eat buffer
		dReal buffer; //if buffer reaches zero, trigger event
		bool event; //if event triggered, run script
		Script *script; //the script to run
};

#endif
