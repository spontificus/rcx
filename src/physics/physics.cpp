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

#include "collision_feedback.hpp"


unsigned int stepsize_warnings = 0;

int physics_init(void)
{
	printlog(0, "Initiating physics");
	dInitODE2(0);
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


void physics_step(void)
{
	car_physics_step(); //control, antigrav...
	Joint::Physics_Step(); //joint forces
	Body::Physics_Step(); //drag (air/liquid "friction")

	dSpaceCollide (space, 0, &Geom::Collision_Callback);
	dWorldQuickStep (world, internal.stepsize);
	dJointGroupEmpty (contactgroup);

	Collision_Feedback::Physics_Step(); //forces from collisions
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

