#include "collision_feedback.hpp"

Collision_Feedback *Collision_Feedback::head = NULL;

Collision_Feedback::Collision_Feedback(dJointID joint, Geom *g1, Geom *g2)
{
	geom1 = g1;
	geom2 = g2;

	dJointSetFeedback(joint, &feedback);

	//add to list
	next = head;
	head = this;
}

void Collision_Feedback::Physics_Step()
{
	Collision_Feedback *cf, *next;

	for (cf = head; cf; cf = next)
	{
		cf->geom1->Collision_Force(dLENGTH(cf->feedback.f1));
		cf->geom2->Collision_Force(dLENGTH(cf->feedback.f2));

		next = cf->next;
		delete cf;
	}

	head = NULL;
}
