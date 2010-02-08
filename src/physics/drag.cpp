#include "../shared/shared.hpp"


#define v_length(x, y, z) (dSqrt( (x)*(x) + (y)*(y) + (z)*(z) ))
//functions for body drag

//NOTE: modifying specified drag to the current mass (rice-burning optimization, or actually good idea?)
//(this way the body mass doesn't need to be requested and used in every calculation)
void Body_Data_Set_Linear_Drag (body_data *body, dReal drag)
{
	printlog(2, " * setting body linear drag\n");
	dMass mass;
	dBodyGetMass (body->body_id, &mass);

	body->linear_drag = drag/(mass.mass);
	body->use_linear_drag = true;
	body->use_advanced_linear_drag = false;
}

void Body_Data_Set_Advanced_Linear_Drag (body_data *body, dReal drag_x, dReal drag_y, dReal drag_z)
{
	printlog(2, " * setting body advanced linear drag\n");
	dMass mass;
	dBodyGetMass (body->body_id, &mass);

	body->advanced_linear_drag[0] = drag_x/(mass.mass);
	body->advanced_linear_drag[1] = drag_y/(mass.mass);
	body->advanced_linear_drag[2] = drag_z/(mass.mass);

	body->use_advanced_linear_drag = true;
	body->use_linear_drag = false;
}

void Body_Data_Set_Angular_Drag (body_data *body, dReal drag)
{
	printlog(2, " * setting body angular drag\n");
	dMass mass;
	dBodyGetMass (body->body_id, &mass);

	//TODO: this is not realistic, should also be affected by how mass is spread
	body->angular_drag = drag/(mass.mass);
	body->use_angular_drag = true;
}


//simulation of drag
//
//not to self: if implementing different density areas, this is where density should be chosen
void Body_Data_Linear_Drag (body_data *body)
{
	const dReal *abs_vel; //absolute vel
	abs_vel = dBodyGetLinearVel (body->body_id);
	dReal vel[3] = {abs_vel[0]-track.wind[0], abs_vel[1]-track.wind[1], abs_vel[2]-track.wind[2]}; //relative to wind
	dReal total_vel = v_length(vel[0], vel[1], vel[2]);

	//how much of original velocity is left after breaking by air/liquid drag
	dReal remain = 1-(total_vel*(track.density)*(body->linear_drag)*(internal.stepsize));

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
	dBodySetLinearVel(body->body_id, vel[0], vel[1], vel[2]);
}

//similar to linear_drag, but different drag for different directions
void Body_Data_Advanced_Linear_Drag (body_data *body)
{
	//absolute velocity
	const dReal *abs_vel;
	abs_vel = dBodyGetLinearVel (body->body_id);

	//translate movement to relative to car (and to wind)
	dVector3 vel;
	dBodyVectorFromWorld (body->body_id, (abs_vel[0]-track.wind[0]), (abs_vel[1]-track.wind[1]), (abs_vel[2]-track.wind[2]), vel);
	dReal total_vel = v_length(vel[0], vel[1], vel[2]);

	//how much of original velocities is left after breaking by air/liquid drag
	dReal remain;
	int i;
	for (i=0; i<3; ++i)
	{
		//how much of original velocity remains after drag?
		remain = 1-(total_vel*(track.density)*(body->advanced_linear_drag[i])*(internal.stepsize));

		//check so not going negative
		if (remain < 0)
			remain = 0;

		//change velocity
		vel[i]*=remain;
	}

	//make absolute
	dVector3 vel_result;
	dBodyVectorToWorld (body->body_id, vel[0], vel[1], vel[2], vel_result);

	//add wind
	vel_result[0]+=track.wind[0];
	vel_result[1]+=track.wind[1];
	vel_result[2]+=track.wind[2];

	//set velocity
	dBodySetLinearVel(body->body_id, vel_result[0], vel_result[1], vel_result[2]);
}

void Body_Data_Angular_Drag (body_data *body)
{
	const dReal *vel; //rotation velocity
	vel = dBodyGetAngularVel (body->body_id);
	dReal total_vel = v_length(vel[0], vel[1], vel[2]);

	//how much of original velocity is left after breaking by air/liquid drag
	dReal remain = 1-(total_vel*(track.density)*(body->angular_drag)*(internal.stepsize));

	if (remain < 0) //in case breaking is so extreme it will reverse movement, just change velocity to 0
		remain = 0;

	//set velocity with change
	dBodySetAngularVel(body->body_id, vel[0]*remain, vel[1]*remain, vel[2]*remain);
}
