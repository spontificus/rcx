#include "../shared/geom.hpp"

#include "../shared/internal.hpp"
#include "../shared/track.hpp"
#include "../shared/printlog.hpp"

#include "collision_feedback.hpp"

#include "../events/event_lists.hpp"

#include <ode/ode.h>

//when two geoms might intersect
void Geom::Collision_Callback (void *data, dGeomID o1, dGeomID o2)
{
	//check if one (or both) geom is space
	if (dGeomIsSpace(o1) || dGeomIsSpace(o2))
	{
		dSpaceCollide2 (o1,o2,data, &Collision_Callback);
		return;
	}

	//both geoms are geoms, get component_data from geoms
	Geom *geom1, *geom2;
	geom1 = (Geom*) dGeomGetData (o1);
	geom2 = (Geom*) dGeomGetData (o2);

	//get attached bodies
	dBodyID b1, b2;
	b1 = dGeomGetBody(o1);
	b2 = dGeomGetBody(o2);

	//none connected to bodies
	if (!b1 && !b2)
		return;

	//none wants to create collisions..
	if (!geom1->collide&&!geom2->collide)
	{
		printlog(1, "not collideable, TODO: bitfield solution");
		return;
	}

	dContact contact[internal.contact_points];
	int count = dCollide (o1,o2,internal.contact_points, &contact[0].geom, sizeof(dContact));

	if (count == 0)
		return;

	//does both components want to collide for real? (not "ghosts"/"sensors")
	if (geom1->collide&&geom2->collide)
	{
		int mode = dContactSoftERP | dContactSoftCFM | dContactApprox1;
		dReal slip,mu,erp,cfm;
		dReal bounce = 0;
		dVector3 fdir = {0,0,0};

		mu = (geom1->mu)*(geom2->mu);
		erp = (geom1->erp)*(geom2->erp);
		cfm = (geom1->cfm)*(geom2->cfm);
		slip = 0.0;

		bool feedback = false;
		//if any of the geoms responds to forces or got a body that responds to force, enable force feedback
		if (geom1->buffer_event || geom2->buffer_event || geom1->force_to_body || geom2->force_to_body)
			feedback = true;

		//optional bouncyness (good for wheels?)
		if (geom1->bounce||geom2->bounce)
		{
			mode |= dContactBounce;

			bounce = (geom1->bounce)*(geom2->bounce);
		}

		//determine if _one_of the geoms is a wheel
		Geom *other, *wheel = NULL;
		if (geom1->wheel&&!geom2->wheel)
		{
			wheel = geom1;
			other = geom2;
		}
		else if (!geom1->wheel&&geom2->wheel)
		{
			wheel = geom2;
			other = geom1;
		}

		int i;
		if (wheel)
		{
			int mode_tyre = mode | dContactSlip1 | dContactFDir1; //add slip calculations and specified direction

			//get slip value (based on the two geoms' slip value and the wheel's rotation speed)
			dReal speed = dJointGetHinge2Angle2Rate (wheel->hinge2);

			if (speed < 0)
				speed = -speed;

			slip = (geom1->slip)*(geom2->slip)*speed;

			//now get the axis direction of the wheel (for slip and rim detection), note: axis is along Z
			const dReal *rot = dGeomGetRotation(wheel->geom_id);
			fdir[0] = rot[2];
			fdir[1] = rot[6];
			fdir[2] = rot[10];

			//when rim is colliding, no slip, different mu...
			dReal mu_rim = (wheel->mu_rim)*(other->mu);
			//note: there's gotta be another way instead of storing a mu_rim in every Geom...

			for (i=0; i<count; ++i)
			{
				//dot product between wheel axis and force direction (contact normal)
				dReal dot = contact[i].geom.normal[0]*fdir[0]+contact[i].geom.normal[1]*fdir[1]+contact[i].geom.normal[2]*fdir[2];

				//tyre
				if (-internal.rim_angle < dot && dot < internal.rim_angle)
				{
					contact[i].surface.mode = mode_tyre;

					contact[i].fdir1[0] = fdir[0];
					contact[i].fdir1[1] = fdir[1];
					contact[i].fdir1[2] = fdir[2];

					contact[i].surface.slip1 = slip;
					contact[i].surface.mu = mu;
					contact[i].surface.soft_erp = erp;
					contact[i].surface.soft_cfm = cfm;
					contact[i].surface.bounce = bounce; //in case specified
					dJointID c = dJointCreateContact (world,contactgroup,&contact[i]);
					dJointAttach (c,b1,b2);

					if (feedback)
						new Collision_Feedback(c, geom1, geom2);
				}
				//rim
				else
				{
					contact[i].surface.mode = mode;

					contact[i].surface.mu = mu_rim;
					contact[i].surface.soft_erp = erp;
					contact[i].surface.soft_cfm = cfm;
					contact[i].surface.bounce = bounce; //in case specified
					dJointID c = dJointCreateContact (world,contactgroup,&contact[i]);
					dJointAttach (c,b1,b2);

					if (feedback)
						new Collision_Feedback(c, geom1, geom2);
				}

			}
		}

		else //normal collision
		{
			for (i=0; i<count; ++i)
			{
					contact[i].surface.mode = mode;

					contact[i].surface.mu = mu;
					contact[i].surface.soft_erp = erp;
					contact[i].surface.soft_cfm = cfm;
					contact[i].surface.bounce = bounce; //in case specified
					dJointID c = dJointCreateContact (world,contactgroup,&contact[i]);
					dJointAttach (c,b1,b2);

					if (feedback)
						new Collision_Feedback(c, geom1, geom2);
			}
		}
	}
	
	//with physical contact or not, might respond to collision events
	if (geom1->collide)
	{
		geom2->colliding = true;
	}

	if (geom2->collide)
	{
		geom1->colliding = true;
	}
}

//set event
void Geom::Set_Buffer_Event(dReal thres, dReal buff, Script *scr)
{
	if (thres > 0 && buff > 0 && scr)
	{
		printlog(2, "setting Geom event");

		threshold=thres;
		buffer=buff;
		buffer_script=scr;

		//make sure no old event is left
		Buffer_Event_List::Remove(this);

		buffer_event=true;
	}
	else
	{
		printlog(2, "disabling Geom event");
		buffer_event=false;
		Buffer_Event_List::Remove(this);
	}
}

bool Geom::Set_Buffer_Body(Body *b)
{
	force_to_body = b;

	return true;
}

void Geom::Damage_Buffer(dReal force)
{
	if (force_to_body)
	{
		force_to_body->Damage_Buffer(force);
		return;
	}

	//if not configured or force not exceeds threshold, stop
	if (!buffer_event || (force<threshold))
		return;

	//buffer still got health
	if (buffer > 0)
	{
		buffer -= (force-threshold)*internal.stepsize;

		//now it's negative, issue event
		if (buffer < 0)
		{
			printlog(2, "Geom buffer depleted, generating event");
			new Buffer_Event_List(this);
		}
	}
	else //just damage buffer even more
		buffer -= (force-threshold)*internal.stepsize;
}

void Geom::Increase_Buffer(dReal buff)
{
	buffer+=buff;

	if (buffer < 0) //still depleted, regenerate event
		new Buffer_Event_List(this);
}
