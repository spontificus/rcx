#include "../shared/car.hpp"
#include "../shared/internal.hpp"

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
		if (carp->sensor1->colliding && carp->sensor2->colliding)
			antigrav = true;
		//only one sensor, flipping+antigrav
		else if (carp->sensor1->colliding)
		{
			antigrav = true;
			carp->dir = 1.0;
		}
		//same
		else if (carp->sensor2->colliding)
		{
			antigrav = true;
			carp->dir = -1.0;
		}
		//no sensor active, no flipping, no antigrav
		else
			antigrav = false;

		//sensors have been read, reset them
		carp->sensor1->colliding = false;
		carp->sensor2->colliding = false;

		if (antigrav) //TODO
		{
//			dBodyAddRelForce (carp->bodyid,0,0, carp->dir*100);
		}

		//control
		if (carp->drift_breaks)
		{
			if (carp->torque_compensator)
			{
				const dReal *r = dBodyGetAngularVel(carp->bodyid);
				dBodySetAngularVel(carp->wheel_body[1], r[0], r[1], r[2]);
				dBodySetAngularVel(carp->wheel_body[2], r[0], r[1], r[2]);
			}
			else
			{
				dJointSetHinge2Param (carp->joint[1],dParamVel2,0);
				dJointSetHinge2Param (carp->joint[1],dParamFMax2,dInfinity);
				dJointSetHinge2Param (carp->joint[2],dParamVel2,0);
				dJointSetHinge2Param (carp->joint[2],dParamFMax2,dInfinity);
			}
		}
		else if (carp->breaks)
		{
			if (carp->torque_compensator)
			{
				const dReal *r = dBodyGetAngularVel(carp->bodyid); //in case stopping wheel
				dReal rotation, torque_needed;
				dReal torque[4] = {0,0,0,0};

				int i;
				for (i=0; i<4; ++i)
				{
					rotation = dJointGetHinge2Angle2Rate (carp->joint[i]);
					torque_needed = (carp->inertia_tensor*rotation/internal.stepsize); //T=I*a/t

					//negative rotation, negative values...
					if (torque_needed < 0)
					{
						//the usual situation: only enough torque to slow down the wheel
						if (-torque_needed > carp->max_break)
							torque[i] = +carp->max_break;
						else //wheel will stop rotating
							dBodySetAngularVel(carp->wheel_body[i], r[0],r[1],r[2]);
					}
					else //positive rotation, positive values
					{
						//the usual situation: only enough torque to slow down the wheel
						if (torque_needed > carp->max_break)
							torque[i] = -carp->max_break;
						else //wheel will stop rotating
							dBodySetAngularVel(carp->wheel_body[i], r[0],r[1],r[2]);
					}
				}

				//add breaking torques (even if possibly 0)
				dBodyAddRelTorque(carp->wheel_body[0], 0, 0, -torque[0]);
				dBodyAddRelTorque(carp->wheel_body[1], 0, 0, -torque[1]);
				dBodyAddRelTorque(carp->wheel_body[2], 0, 0, torque[2]);
				dBodyAddRelTorque(carp->wheel_body[3], 0, 0, torque[3]);
			}
			else
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
				//only when wheel is in air
				if ( !(carp->wheel_geom_data[i]->colliding) && rotation > internal.max_wheel_rotation)
					torque[i] = 0.0;
				else
				{
					//else we will add torque
					//motor torque is geared by stepless gearbox
					torque[i]=carp->max_torque/(1+rotation*carp->motor_tweak);
				}

				//since we are using the wheel collision detection, reset it each time
				carp->wheel_geom_data[i]->colliding = false; //reset
			}

			if (carp->torque_compensator)
			{
				dBodyAddRelTorque(carp->wheel_body[0], 0, 0, -torque[0]*carp->throttle*carp->dir*carp->fmotor);
				dBodyAddRelTorque(carp->wheel_body[1], 0, 0, -torque[1]*carp->throttle*carp->dir*carp->rmotor);
				dBodyAddRelTorque(carp->wheel_body[2], 0, 0, torque[2]*carp->throttle*carp->dir*carp->rmotor);
				dBodyAddRelTorque(carp->wheel_body[3], 0, 0, torque[3]*carp->throttle*carp->dir*carp->fmotor);
			}
			else
			{
				dJointAddHinge2Torques (carp->joint[0],0,torque[0]*carp->throttle*carp->dir*carp->fmotor);
				dJointAddHinge2Torques (carp->joint[1],0,torque[1]*carp->throttle*carp->dir*carp->rmotor);
				dJointAddHinge2Torques (carp->joint[2],0,torque[2]*carp->throttle*carp->dir*carp->rmotor);
				dJointAddHinge2Torques (carp->joint[3],0,torque[3]*carp->throttle*carp->dir*carp->fmotor);
			}
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
