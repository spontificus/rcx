//handles physics simulation (mostly rigid body dynamics)
//
//See main.c about licensing
//

dWorldID world;
dSpaceID space;
dJointGroupID contactgroup;//TODO: move to shared.h data? good for event thread?

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
		slip = 0.0;

		//determine if _one_of the geoms is a wheel
		geom_data *wheel = NULL;
		if (geom1->wheel&&!geom2->wheel)
			wheel = geom1;
		else if (!geom1->wheel&&geom2->wheel)
			wheel = geom2;

		if (wheel)
		{
			mode |= dContactSlip1 | dContactFDir1; //add slip calculations and specified direction

			//get slip value (based on the two geoms' slip value and the wheel's rotation speed)
			dReal speed = dJointGetHinge2Angle2Rate (wheel->hinge2);

			if (speed < 0)
				speed = -speed;

			slip = (geom1->slip)*(geom2->slip)*speed;

			//now get the axis direction of the wheel (slip in the right direction) note: axis is along Z
			const dReal *rot = dGeomGetRotation(wheel->geom_id);
			fdir[0] = rot[2];
			fdir[1] = rot[6];
			fdir[2] = rot[10];
		}

		//optional bouncyness (good for wheels?)
		if (geom1->bounce||geom2->bounce)
		{
			mode |= dContactBounce;

			bounce = ((geom1->bounce)+(geom2->bounce))/2;
		}

		erp = ((geom1->erp)+(geom2->erp))/2;
		cfm = ((geom1->cfm)+(geom2->cfm))/2;

		int i;
		for (i=0; i<count; ++i)
		{
			contact[i].fdir1[0] = fdir[0];
			contact[i].fdir1[1] = fdir[1];
			contact[i].fdir1[2] = fdir[2];

			contact[i].surface.mode = mode;
			contact[i].surface.slip1 = slip;
			contact[i].surface.mu = mu;
			contact[i].surface.soft_erp = erp;
			contact[i].surface.soft_cfm = cfm;
			contact[i].surface.bounce = bounce;
			dJointID c = dJointCreateContact (world,contactgroup,&contact[i]);
			dJointAttach (c,
					dGeomGetBody(contact[i].geom.g1),
					dGeomGetBody(contact[i].geom.g2));
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
			dJointSetHinge2Param (carp->joint[1],dParamFMax2,carp->max_break);
			dJointSetHinge2Param (carp->joint[2],dParamVel2,0);
			dJointSetHinge2Param (carp->joint[2],dParamFMax2,carp->max_break);
		}
		else
		{
			//disable motor (used for breaks...)
			dJointSetHinge2Param (carp->joint[1],dParamFMax2,0);
			dJointSetHinge2Param (carp->joint[2],dParamFMax2,0);

			//add torques directly (no "motor")
			dReal wheel1 = dJointGetHinge2Angle2Rate (carp->joint[1]);
			dReal wheel2 = dJointGetHinge2Angle2Rate (carp->joint[2]);
			if (wheel1 < 0)
				wheel1 = -wheel1;
			if (wheel2 < 0)
				wheel2 = -wheel2;

			dReal torque1=carp->max_torque/(1+wheel1*carp->motor_tweak);
			dReal torque2=carp->max_torque/(1+wheel2*carp->motor_tweak);

			dJointAddHinge2Torques (carp->joint[1],0,torque1*carp->throttle*carp->dir);
			dJointAddHinge2Torques (carp->joint[2],0,torque2*carp->throttle*carp->dir);
		}

		dJointSetHinge2Param (carp->joint[0],dParamLoStop,carp->steering*carp->dir);
		dJointSetHinge2Param (carp->joint[0],dParamHiStop,carp->steering*carp->dir);
		dJointSetHinge2Param (carp->joint[3],dParamLoStop,carp->steering*carp->dir);
		dJointSetHinge2Param (carp->joint[3],dParamHiStop,carp->steering*carp->dir);


		//set finite rotation axis (to prevent bending of rear axes)
		if (internal.finite_rotation)
		{
			const dReal *rot = dBodyGetRotation (carp->bodyid);

			dBodySetFiniteRotationAxis (carp->wheel_body[1],-rot[0],-rot[4],-rot[8]);
			dBodySetFiniteRotationAxis (carp->wheel_body[2],-rot[0],-rot[4],-rot[8]);
		}

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
	const dReal *a_vel;
	dVector3 r_vel;
	dReal forceX, forceY, forceZ;

	while (d)
	{
		if (d->use_drag)
		{
			a_vel = dBodyGetLinearVel(d->body_id);

			dBodyVectorFromWorld (d->body_id, a_vel[0], a_vel[1],
					a_vel[2], r_vel);

			forceX = r_vel[0]*r_vel[0]*track.density*d->drag[0];
			forceY = r_vel[1]*r_vel[1]*track.density*d->drag[1];
			forceZ = r_vel[2]*r_vel[2]*track.density*d->drag[2];

			if (r_vel[0] > 0.0)
				forceX = -forceX;
			if (r_vel[1] > 0.0)
				forceY = -forceY;
			if (r_vel[2] > 0.0)
				forceZ = -forceZ;

			dBodyAddRelForce(d->body_id, forceX, forceY, forceZ);
		}

		if (d->use_rotation_drag)
		{
			a_vel = dBodyGetAngularVel (d->body_id);
	
			dBodyVectorFromWorld (d->body_id, a_vel[0], a_vel[1],
					a_vel[2], r_vel);

			forceX = r_vel[0]*r_vel[0]*track.density*d->rot_drag[0];
			forceY = r_vel[1]*r_vel[1]*track.density*d->rot_drag[1];
			forceZ = r_vel[2]*r_vel[2]*track.density*d->rot_drag[2];

			if (r_vel[0] > 0.0)
				forceX = -forceX;
			if (r_vel[1] > 0.0)
				forceY = -forceY;
			if (r_vel[2] > 0.0)
				forceZ = -forceZ;

			dBodyAddRelTorque (d->body_id, forceX, forceY, forceZ);
		}

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
}

int physics_loop (void *d)
{
	printlog(1, "Starting physics loop\n");

	Uint32 simtime = SDL_GetTicks(); //set simulated time to realtime
	Uint32 realtime; //real time (with possible delay since last update)
	Uint32 stepsize_ms = internal.stepsize*1000;

	while (runlevel == running)
	{
		physics_step();
		

		simtime += stepsize_ms;
		realtime = SDL_GetTicks();
		if (simtime > realtime)
			SDL_Delay (simtime - realtime);
	}
	return 0;
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

