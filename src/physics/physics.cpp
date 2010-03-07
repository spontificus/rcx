//handles physics simulation (mostly rigid body dynamics)
//
//See main.c about licensing
//

#include <SDL/SDL_timer.h>
#include <SDL/SDL_mutex.h>

#include "../shared/threads.hpp"
#include "../shared/internal.hpp"
#include "../shared/runlevel.hpp"
#include "../shared/track.hpp"
#include "../shared/printlog.hpp"
#include "../shared/body.hpp"
#include "../shared/geom.hpp"
#include "../shared/camera.hpp"
#include "../shared/car.hpp"
#include "../shared/joint.hpp"


unsigned int stepsize_warnings = 0;

int physics_init(void)
{
	printlog(0, "Initiating physics");
	dInitODE2(0);
	printlog(1, "TODO: change ODE allocation flags...");
	dAllocateODEDataForThread(dAllocateFlagBasicData | dAllocateFlagCollisionData);

	world = dWorldCreate();

	//TODO: move to "dQuadTreeSpaceCreate()" - much better performance!!!
	printlog(1, "TODO: create world space using dQuadTreeSpaceCreate() - much better performance!");
	space = dHashSpaceCreate(0);
	contactgroup = dJointGroupCreate(0);

	dWorldSetQuickStepNumIterations (world, internal.iterations);

	//autodisable
	dWorldSetAutoDisableFlag (world, 1);
	dWorldSetAutoDisableLinearThreshold (world, internal.dis_linear);
	dWorldSetAutoDisableAngularThreshold (world, internal.dis_angular);
	dWorldSetAutoDisableSteps (world, internal.dis_steps);
	dWorldSetAutoDisableTime (world, internal.dis_time);
	
	return 0;
}

//when two geoms might intersect
void CollisionCallback (void *data, dGeomID o1, dGeomID o2)
{
	//check if one (or both) geom is space
	if (dGeomIsSpace(o1) || dGeomIsSpace(o2))
	{
		dSpaceCollide2 (o1,o2,data, &CollisionCallback);
		return;
	}

	//both geoms are geoms, get component_data from geoms
	Geom *geom1, *geom2;
	geom1 = (Geom*) dGeomGetData (o1);
	geom2 = (Geom*) dGeomGetData (o2);

	if (!geom1->collide&&!geom2->collide)
		printlog(1, "not collideable, TODO: bitfield solution");

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
					dJointAttach (c,
							dGeomGetBody(contact[i].geom.g1),
							dGeomGetBody(contact[i].geom.g2));
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
					dJointAttach (c,
							dGeomGetBody(contact[i].geom.g1),
							dGeomGetBody(contact[i].geom.g2));
				}

			}
		}

		else
		{
			for (i=0; i<count; ++i)
			{
				contact[i].surface.mode = mode;

				contact[i].surface.mu = mu;
				contact[i].surface.soft_erp = erp;
				contact[i].surface.soft_cfm = cfm;
				contact[i].surface.bounce = bounce; //in case specified
				dJointID c = dJointCreateContact (world,contactgroup,&contact[i]);
				dJointAttach (c,
						dGeomGetBody(contact[i].geom.g1),
						dGeomGetBody(contact[i].geom.g2));
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

void physics_step(void)
{
	car_physics_step(); //control, antigrav...
	joint_physics_step(); //joint forces
	body_physics_step(); //drag (air/liquid "friction")

	dSpaceCollide (space, 0, &CollisionCallback);

	dWorldQuickStep (world, internal.stepsize);
	dJointGroupEmpty (contactgroup);

	camera_physics_step(); //move camera to wanted postion
}

int physics_loop (void *d)
{
	printlog(1, "Starting physics loop");

	Uint32 simtime = SDL_GetTicks(); //set simulated time to realtime
	Uint32 realtime; //real time (with possible delay since last update)
	Uint32 stepsize_ms = internal.stepsize*1000+0.0001;

	while (runlevel == running)
	{
		//technically, collision detection doesn't need this, but since it's already in step function, this is easier
		SDL_mutexP(ode_mutex);
		physics_step();

		//one with ode
		SDL_mutexV(ode_mutex);
		

		//broadcast to wake up sleeping threads
		if (internal.sync_events || internal.sync_graphics)
		{
			SDL_mutexP(sync_mutex);
			SDL_CondBroadcast (sync_cond);
			SDL_mutexV(sync_mutex);
		}

		simtime += stepsize_ms;
		realtime = SDL_GetTicks();
		if (simtime > realtime)
			SDL_Delay (simtime - realtime);
		else
			++stepsize_warnings;
	}
	return 0;
}

//TODO: add physics loop

void physics_quit (void)
{
	printlog(1, "Quit physics");
	dJointGroupDestroy (contactgroup);
	dSpaceDestroy (space);
	dWorldDestroy (world);
	dCloseODE();
}

