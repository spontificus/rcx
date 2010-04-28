/*
 * RCX Copyright (C) Slinger
 *
 * This program comes with ABSOLUTELY NO WARRANTY!
 *
 * This is free software, and you are welcome to
 * redistribute it under certain conditions.
 *
 * See license.txt and README for more info
 */

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

		static void Physics_Step();
		static void TMP_Events_Step(Uint32 step);

		//geom data bellongs to
		dJointID joint_id;

		//buffer event
		void Set_Buffer_Event(dReal thresh, dReal buff, Script *scr);
		void Increase_Buffer(dReal add);

	private:
		//used to find next/prev link in dynamically allocated chain
		//set next to null in last link in chain (prev = NULL in first)
		static Joint *head;
		Joint *prev, *next;

		//events
		bool buffer_event;
		//for buffer event processing
		dJointFeedback *feedback; //used if checking forces
		dReal threshold; //if force on body exceeds threshold, eat buffer
		dReal buffer; //if buffer reaches zero, trigger event
		Script *buffer_script; //the script to run
};

#endif
