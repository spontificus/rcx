#include "../shared/body.hpp"
#include "../shared/printlog.hpp"
#include "../shared/track.hpp"
#include "../shared/internal.hpp"

#define v_length(x, y, z) (dSqrt( (x)*(x) + (y)*(y) + (z)*(z) ))
//functions for body drag

//NOTE: modifying specified drag to the current mass (rice-burning optimization, or actually good idea?)
//(this way the body mass doesn't need to be requested and used in every calculation)
void Body::Set_Linear_Drag (dReal drag)
{
	printlog(2, "setting body linear drag");
	dMass mass;
	dBodyGetMass (body_id, &mass);

	linear_drag = drag/(mass.mass);
	use_linear_drag = true;
	use_advanced_linear_drag = false;
}

void Body::Set_Advanced_Linear_Drag (dReal drag_x, dReal drag_y, dReal drag_z)
{
	printlog(2, "setting body advanced linear drag");
	dMass mass;
	dBodyGetMass (body_id, &mass);

	advanced_linear_drag[0] = drag_x/(mass.mass);
	advanced_linear_drag[1] = drag_y/(mass.mass);
	advanced_linear_drag[2] = drag_z/(mass.mass);

	use_advanced_linear_drag = true;
	use_linear_drag = false;
}

void Body::Set_Angular_Drag (dReal drag)
{
	printlog(2, "setting body angular drag");
	dMass mass;

	//TODO: use the body's inertia tensor instead...?
	dBodyGetMass (body_id, &mass);

	angular_drag = drag/(mass.mass);
	use_angular_drag = true;
}


//simulation of drag
//
//not to self: if implementing different density areas, this is where density should be chosen
void Body::Linear_Drag ()
{
	const dReal *abs_vel; //absolute vel
	abs_vel = dBodyGetLinearVel (body_id);
	dReal vel[3] = {abs_vel[0]-track.wind[0], abs_vel[1]-track.wind[1], abs_vel[2]-track.wind[2]}; //relative to wind
	dReal total_vel = v_length(vel[0], vel[1], vel[2]);

	//how much of original velocity is left after breaking by air/liquid drag
	dReal remain = 1-(total_vel*(track.density)*(linear_drag)*(internal.stepsize));

	if (remain < 0) //in case breaking is so extreme it will reverse movement, just change velocity to 0
		remain = 0;

	//change velocity
	vel[0]*=remain;
	vel[1]*=remain;
	vel[2]*=remain;

	//make absolute
	vel[0]+=track.wind[0];
	vel[1]+=track.wind[1];
	vel[2]+=track.wind[2];

	//set velocity
	dBodySetLinearVel(body_id, vel[0], vel[1], vel[2]);
}

//similar to linear_drag, but different drag for different directions
void Body::Advanced_Linear_Drag ()
{
	//absolute velocity
	const dReal *abs_vel;
	abs_vel = dBodyGetLinearVel (body_id);

	//translate movement to relative to car (and to wind)
	dVector3 vel;
	dBodyVectorFromWorld (body_id, (abs_vel[0]-track.wind[0]), (abs_vel[1]-track.wind[1]), (abs_vel[2]-track.wind[2]), vel);
	dReal total_vel = v_length(vel[0], vel[1], vel[2]);

	//how much of original velocities is left after breaking by air/liquid drag
	dReal remain;
	int i;
	for (i=0; i<3; ++i)
	{
		//how much of original velocity remains after drag?
		remain = 1-(total_vel*(track.density)*(advanced_linear_drag[i])*(internal.stepsize));

		//check so not going negative
		if (remain < 0)
			remain = 0;

		//change velocity
		vel[i]*=remain;
	}

	//make absolute
	dVector3 vel_result;
	dBodyVectorToWorld (body_id, vel[0], vel[1], vel[2], vel_result);

	//add wind
	vel_result[0]+=track.wind[0];
	vel_result[1]+=track.wind[1];
	vel_result[2]+=track.wind[2];

	//set velocity
	dBodySetLinearVel(body_id, vel_result[0], vel_result[1], vel_result[2]);
}

void Body::Angular_Drag ()
{
	const dReal *vel; //rotation velocity
	vel = dBodyGetAngularVel (body_id);
	dReal total_vel = v_length(vel[0], vel[1], vel[2]);

	//how much of original velocity is left after breaking by air/liquid drag
	dReal remain = 1-(total_vel*(track.density)*(angular_drag)*(internal.stepsize));

	if (remain < 0) //in case breaking is so extreme it will reverse movement, just change velocity to 0
		remain = 0;

	//set velocity with change
	dBodySetAngularVel(body_id, vel[0]*remain, vel[1]*remain, vel[2]*remain);
}


void Body::Set_Event(dReal thres, dReal buff, script_struct *scr)
{
	if (thres > 0 && buff > 0 && scr)
	{
		printlog(2, "setting Body event");

		threshold=thres;
		buffer=buff;
		script=scr;

		//make sure no old event is left
		event=false;
	}
	else
	{
		printlog(2, "disabling Body event");

		//disable
		threshold = 0;
	}
}


void Body::Physics_Step (void)
{
	Body *d = Body::head;
	const dReal *force;
	dReal force_total;

	while (d)
	{
		//drag
		if (d->use_advanced_linear_drag)
			d->Advanced_Linear_Drag();
		else if (d->use_linear_drag) //might have simple drag instead
			d->Linear_Drag();
		if (d->use_angular_drag)
			d->Angular_Drag();

		d = d->next;
	}
}

