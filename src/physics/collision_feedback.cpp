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

#include "collision_feedback.hpp"

Collision_Feedback *Collision_Feedback::head = NULL;

Collision_Feedback::Collision_Feedback(dJointID joint, Geom *g1, Geom *g2)
{
	geom1 = g1;
	geom2 = g2;

	//make sure initialized to 0 (in case joint doesn't return any data...)
	feedback.f1[0]=0;
	feedback.f1[1]=0;
	feedback.f1[2]=0;
	feedback.f2[0]=0;
	feedback.f2[1]=0;
	feedback.f2[2]=0;

	//set
	dJointSetFeedback(joint, &feedback);

	//add to list
	next = head;
	head = this;
}

void Collision_Feedback::Physics_Step()
{
	Collision_Feedback *prev;

	while (head)
	{
		//just pass force to appropriate geom
		head->geom1->Damage_Buffer(dLENGTH(head->feedback.f1));
		head->geom2->Damage_Buffer(dLENGTH(head->feedback.f2));

		//remove
		prev = head;
		head = head->next;
		delete prev;
	}
}

