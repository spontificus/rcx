//handles physics simulation (mostly rigid body dynamics)
//
//See main.c about licensing
//

dWorldID world;
dSpaceID space;
dJointGroupID contactgroup;//TODO: move to shared.h data? good for event thread?

#include "physics/camera.c"
#include "physics/drag.c"

int physics_init(void)
{
	printlog(0, "=> Initiating physics\n");
	dInitODE2(0);
	//TODO: this is an ugly flag, change it
	dAllocateODEDataForThread(dAllocateMaskAll);

	world = dWorldCreate();
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
	geom_data *geom1, *geom2;
	geom1 = (geom_data*) dGeomGetData (o1);
	geom2 = (geom_data*) dGeomGetData (o2);

	if (!geom1->collide&&!geom2->collide)
		printlog(1, "not collideable, FIXME!: bitfield solution\n");

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
		geom_data *other, *wheel = NULL;
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
			//note: there's gotta be another way instead of storing a mu_rim in every geom_data...

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
		geom2->event = true;
	}

	if (geom2->collide)
	{
		geom1->event = true;
	}
}

void car_physics_step(void)
{
	car_struct *carp = car_head;
	bool antigrav;
	while (carp != NULL)
	{
		//some loaded cars might not participate in race...
		if (!carp->spawned)
		{
			carp=carp->next;
			continue;
		}

		//first flipover detection (+ antigrav forces)

		//both sensors are triggered, not flipping, only antigrav
		if (carp->sensor1->event && carp->sensor2->event)
			antigrav = true;
		//only one sensor, flipping+antigrav
		else if (carp->sensor1->event)
		{
			antigrav = true;
			carp->dir = 1.0;
		}
		//same
		else if (carp->sensor2->event)
		{
			antigrav = true;
			carp->dir = -1.0;
		}
		//no sensor active, no flipping, no antigrav
		else
			antigrav = false;

		//sensors have been read, reset them
		carp->sensor1->event = false;
		carp->sensor2->event = false;

		if (antigrav) //TODO
		{
//			dBodyAddRelForce (carp->bodyid,0,0, carp->dir*100);
		}

		//control
		if (carp->drift_breaks)
		{
			dJointSetHinge2Param (carp->joint[1],dParamVel2,0);
			dJointSetHinge2Param (carp->joint[1],dParamFMax2,dInfinity);
			dJointSetHinge2Param (carp->joint[2],dParamVel2,0);
			dJointSetHinge2Param (carp->joint[2],dParamFMax2,dInfinity);
		}
		else if (carp->breaks)
		{
			dJointSetHinge2Param (carp->joint[1],dParamVel2,0);
			dJointSetHinge2Param (carp->joint[1],dParamFMax2,carp->max_break*carp->rbreak);
			dJointSetHinge2Param (carp->joint[2],dParamVel2,0);
			dJointSetHinge2Param (carp->joint[2],dParamFMax2,carp->max_break*carp->rbreak);

			dJointSetHinge2Param (carp->joint[0],dParamVel2,0);
			dJointSetHinge2Param (carp->joint[0],dParamFMax2,carp->max_break*carp->fbreak);
			dJointSetHinge2Param (carp->joint[3],dParamVel2,0);
			dJointSetHinge2Param (carp->joint[3],dParamFMax2,carp->max_break*carp->fbreak);
		}
		else
		{
			dReal torque[4];
			int i;
			for (i=0; i<4; ++i)
			{
				//disable motor (used for breaks...)
				dJointSetHinge2Param (carp->joint[i],dParamFMax2,0);

				//add torques directly (no "motor")
				dReal rotation = dJointGetHinge2Angle2Rate (carp->joint[i]);

				//we want total speed, not negative
				if (rotation < 0)
					rotation = -rotation;

				//in case wheel is already rotating so fast we get simulation errors, no simulation
				if (rotation > internal.max_wheel_rotation)
					torque[i] = 0.0;
				else
				{
					//else we will add torque
					//motor torque is geared by stepless gearbox
					torque[i]=carp->max_torque/(1+rotation*carp->motor_tweak);
				}
			}

			dJointAddHinge2Torques (carp->joint[0],0,torque[0]*carp->throttle*carp->dir*carp->fmotor);
			dJointAddHinge2Torques (carp->joint[1],0,torque[1]*carp->throttle*carp->dir*carp->rmotor);
			dJointAddHinge2Torques (carp->joint[2],0,torque[2]*carp->throttle*carp->dir*carp->rmotor);
			dJointAddHinge2Torques (carp->joint[3],0,torque[3]*carp->throttle*carp->dir*carp->fmotor);
		}

		dJointSetHinge2Param (carp->joint[0],dParamLoStop,carp->steering*carp->dir *carp->fsteer);
		dJointSetHinge2Param (carp->joint[0],dParamHiStop,carp->steering*carp->dir *carp->fsteer);
		dJointSetHinge2Param (carp->joint[3],dParamLoStop,carp->steering*carp->dir *carp->fsteer);
		dJointSetHinge2Param (carp->joint[3],dParamHiStop,carp->steering*carp->dir *carp->fsteer);

		dJointSetHinge2Param (carp->joint[1],dParamLoStop,carp->steering*carp->dir *carp->rsteer);
		dJointSetHinge2Param (carp->joint[1],dParamHiStop,carp->steering*carp->dir *carp->rsteer);
		dJointSetHinge2Param (carp->joint[2],dParamLoStop,carp->steering*carp->dir *carp->rsteer);
		dJointSetHinge2Param (carp->joint[2],dParamHiStop,carp->steering*carp->dir *carp->rsteer);


		//set finite rotation axis (to prevent bending of rear axes)
		/*if (internal.finite_rotation)
		{
			const dReal *rot = dBodyGetRotation (carp->bodyid);

			dBodySetFiniteRotationAxis (carp->wheel_body[1],-rot[0],-rot[4],-rot[8]);
			dBodySetFiniteRotationAxis (carp->wheel_body[2],-rot[0],-rot[4],-rot[8]);
		}*/

		//save ccar velocity
		const dReal *vel = dBodyGetLinearVel (carp->bodyid);
		const dReal *rot = dBodyGetRotation  (carp->bodyid);
		carp->velocity = (rot[1]*vel[0] + rot[5]*vel[1] + rot[9]*vel[2]);

		//done, next car...
		carp=carp->next;
	}
}

//check for joint triggering
void joint_physics_step (void)
{
	joint_data *d = joint_data_head;
	dReal delt1, delt2, delt;

	while (d)
	{
		delt1 = dLENGTH(d->feedback.f1);
		delt2 = dLENGTH(d->feedback.f2);

		if (delt1>delt2)
			delt = delt1 - d->threshold;
		else
			delt = delt2 - d->threshold;

		if (delt > 0)
		{
			d->buffer -= delt*internal.stepsize;
			if (d->buffer < 0)
				d->event = true;
		}

		d = d->next;
	}
}

//currently just simulates air drag
//NOTE: ode provides linear/angular dampening, but this should be more realistic
void body_physics_step (void)
{
	body_data *d = body_data_head;

	while (d)
	{
		if (d->use_advanced_linear_drag)
			Body_Data_Advanced_Linear_Drag (d);
		else if (d->use_linear_drag) //might have simple drag instead
			Body_Data_Linear_Drag (d);
		if (d->use_angular_drag)
			Body_Data_Angular_Drag (d);

		d = d->next;
	}
}

void physics_step(void)
{
	car_physics_step(); //control, antigrav...
	joint_physics_step(); //joint forces
	body_physics_step(); //drag (air/liquid "friction")

	dSpaceCollide (space, 0, &CollisionCallback);

	//se if any object "would like" to collide its components
	object_struct *obj = object_head;
	while (obj != NULL)
	{
		if (obj->collide_space)
		{
			dSpaceCollide (obj->space, 0, &CollisionCallback);
		}
		obj = obj->next;
	}

	dWorldQuickStep (world, internal.stepsize);
	dJointGroupEmpty (contactgroup);

	camera_physics_step(); //move camera to wanted postion
}

//TODO: add physics loop

void physics_quit (void)
{
	printlog(1, "=> Quit physics\n");
	dJointGroupDestroy (contactgroup);
	dSpaceDestroy (space);
	dWorldDestroy (world);
	dCloseODE();
}

