#define stepsize 0.025 //time between each simulation step
#define iterations 40 //number of iterations per step

dWorldID world;
dSpaceID space;
dJointGroupID contactgroup;//TODO: move to shared.h data? good for event thread?

int physics_init(void)
{
	printf("-> Initiating physics\n");
	dInitODE2(0);
	//TODO: this is an ugly flag, change when possible...
	dAllocateODEDataForThread(dAllocateMaskAll);

	world = dWorldCreate();
	space = dHashSpaceCreate(0);
	contactgroup = dJointGroupCreate(0);

	dWorldSetQuickStepNumIterations (world, iterations);

	return 0;
}

//when two geoms might intersect
void CollisionCallback (void *data, dGeomID o1, dGeomID o2)
{
	//check if one (or both) geoms is space
	if (dGeomIsSpace(o1) || dGeomIsSpace(o2))
	{
		dSpaceCollide2 (o1,o2,data, &CollisionCallback);
		return;
	}

	//both geoms are geoms, get component_data from geoms
	component *cmp1, *cmp2;
	cmp1 = (component*) dGeomGetData (o1);
	cmp2 = (component*) dGeomGetData (o2);

	if (!cmp1->collide&&!cmp2->collide)
		printf("not collideable, TODO: bitfield solution\n");

	//assume we don't need more than 20 contacts
	dContact contact[20];
	int count = dCollide (o1,o2,20, &contact[0].geom, sizeof(dContact));

	if (count == 0)
		return;

	//does both components want to collide for real? (not "ghosts"/"sensors")
	if (cmp1->collide&&cmp2->collide)
	{
		int mode = dContactSoftERP | dContactSoftCFM | dContactApprox1; //default
		dReal mu = ((cmp1->mu)+(cmp2->mu))/2;
		dReal erp = ((cmp1->erp)+(cmp2->erp))/2;
		dReal cfm = ((cmp1->cfm)+(cmp2->cfm))/2;

		//optional "Force-dependent-slip" (FDS) if requested
		dReal slip1 = 0.0;
		dReal slip2 = 0.0;
		if (cmp1->slip1||cmp2->slip1)
		{
			mode |= dContactSlip1;
			slip1 = ((cmp1->slip1)+(cmp2->slip1))/2;
		}

		if (cmp1->slip2||cmp2->slip2)
		{
			mode |= dContactSlip2;
			slip2 = ((cmp1->slip2)+(cmp2->slip2))/2;
		}

		int i;
		for (i=0; i<count; ++i)
		{
			contact[i].surface.slip1 = slip1;
			contact[i].surface.slip2 = slip2;
			contact[i].surface.mode = mode;
			contact[i].surface.mu = mu;
			contact[i].surface.soft_erp = erp;
			contact[i].surface.soft_cfm = cfm;
			dJointID c = dJointCreateContact (world,contactgroup,&contact[i]);
			dJointAttach (c,
					dGeomGetBody(contact[i].geom.g1),
					dGeomGetBody(contact[i].geom.g2));
		}
	}
	
	//with physical contact or not, might respond to collision events
	if (cmp1->collide)
	{
		cmp2->collision_event = true;
	}

	if (cmp2->collide)
	{
		cmp1->collision_event = true;
	}
}

//needs better name...
void car_physics_step(void)
{
	car *carp = car_head;
	while (carp != NULL)
	{
		//controll
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
			//TODO: limit force based on rotation speed (for each)
			dJointAddHinge2Torques (carp->joint[1],0,carp->max_torque*carp->throttle);
			dJointAddHinge2Torques (carp->joint[2],0,carp->max_torque*carp->throttle);
		}

		dJointSetHinge2Param (carp->joint[0],dParamLoStop,carp->steering*0.5);
		dJointSetHinge2Param (carp->joint[0],dParamHiStop,carp->steering*0.5);
		dJointSetHinge2Param (carp->joint[3],dParamLoStop,carp->steering*0.5);
		dJointSetHinge2Param (carp->joint[3],dParamHiStop,carp->steering*0.5);


		//set finite rotation axis (to prevent bending of axis)
		int i;
		for (i=0; i<4; ++i)
		{
			//get what axis direction is, then force the wheel
			dVector3 axis;
			dJointGetHinge2Axis2 (carp->joint[i], axis);
			dBodySetFiniteRotationAxis (carp->wheel_body[i],axis[0],axis[1],axis[2]);
//			dBodySetFiniteRotationAxis (carp->wheel_body[i],0,0,0);
		}

		//done, next car...
		carp=carp->next;
	}
}

void physics_step(void)
{
	car_physics_step();

	dSpaceCollide (space, 0, &CollisionCallback);

	//se if any object "would like" to collide its components
	object *obj = object_head;
	while (obj != NULL)
	{
		if (obj->collide_space)
		{
			dSpaceCollide (obj->space_id, 0, &CollisionCallback);
		}
		obj = obj->next;
	}

	dWorldQuickStep (world, stepsize);
	dJointGroupEmpty (contactgroup);
}

//TODO: add physics loop

void physics_quit (void)
{
	printf("Quit physics\n");
	dJointGroupDestroy (contactgroup);
	dSpaceDestroy (space);
	dWorldDestroy (world);
	dCloseODE();
}

