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
	Collision_Feedback *prev;

	while (head)
	{
		head->geom1->Collision_Force(dLENGTH(head->feedback.f1));
		head->geom2->Collision_Force(dLENGTH(head->feedback.f2));


		prev = head;
		head = head->next;
		delete prev;
	}
}

