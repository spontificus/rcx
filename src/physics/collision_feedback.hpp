#ifndef _RCX_COLLISION_FEEDBACK_H
#define _RCX_COLLISION_FEEDBACK_H

#include <ode/ode.h>
#include "../shared/geom.hpp"

//geoms can be "damaged" by collision forces, computed from feedback of ode "collision joint"s

class Collision_Feedback
{
	public:
		Collision_Feedback(dJointID joint, Geom *g1, Geom *g2);
		static void Physics_Step(); //processes and clears list

	private:
		//data for simulation
		Geom *geom1, *geom2;
		dJointFeedback feedback;

		//data for keeping track of link members
		Collision_Feedback *next;
		static Collision_Feedback *head;
};
#endif
