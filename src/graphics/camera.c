//length of vector
#define v_length(x, y, z) (dSqrt( (x)*(x) + (y)*(y) + (z)*(z) ))

void set_camera_settings (camera_settings *settings)
{
	if (settings)
	{
		camera.settings = settings;

		if (settings->radius != 0)
		{
			if (!camera.geom)
				camera.geom = dCreateSphere(0, settings->radius);
			else
				dGeomSphereSetRadius (camera.geom, settings->radius);
		}
		else
		{
			if (camera.geom)
				dGeomDestroy(camera.geom);
			camera.geom=NULL;
		}
	}
}

void camera_graphics_step(Uint32 step)
{
	dReal time = step/1000.0;
	car_struct *car = camera.car;
	camera_settings *settings = camera.settings;
	//dReal max_accel = settings->accel_max;
	//dReal up[3] = {0,0,1};

	if (car && settings) //do some magic ;-)
	{
		//store old velocity
		dReal old_vel[3] = {camera.vel[0], camera.vel[1], camera.vel[2]};

		//random values that might come handy:

		//wanted position of "target" - position on car that should be focused
		dVector3 t_pos;
		dBodyGetRelPointPos (car->bodyid, settings->target[0], settings->target[1], settings->target[2]*car->dir, t_pos);
		//wanted position of camera relative to anchor (translated to world coords)
		dVector3 pos_wanted;
		dBodyVectorToWorld(car->bodyid, settings->distance[0], settings->distance[1], settings->distance[2]*car->dir, pos_wanted);

		//position and velocity of anchor
		dVector3 a_pos, a_vel;
		dBodyGetRelPointPos (car->bodyid, settings->anchor[0], settings->anchor[1], settings->anchor[2]*car->dir, a_pos);
		dBodyGetRelPointVel (car->bodyid, settings->anchor[0], settings->anchor[1], settings->anchor[2]*car->dir, a_vel);

		//relative pos and vel of camera (from anchor)
		dReal pos[3] = {camera.pos[0]-a_pos[0], camera.pos[1]-a_pos[1], camera.pos[2]-a_pos[2]};

		//vector lengths
		dReal pos_l = v_length(pos[0], pos[1], pos[2]);
		//how far from car we want to stay
		//(TODO: could be computed just once - only when changing camera)
		dReal pos_wanted_l = v_length(pos_wanted[0], pos_wanted[1], pos_wanted[2]);

		//unit vectors
		dReal pos_u[3] = {pos[0]/pos_l, pos[1]/pos_l, pos[2]/pos_l};
		dReal pos_wanted_u[3] = {pos_wanted[0]/pos_wanted_l, pos_wanted[1]/pos_wanted_l, pos_wanted[2]/pos_wanted_l};

		//
		//spring physics
		//

		//linear spring between anchor and camera (based on distance)
		dReal dist = pos_l-pos_wanted_l;

		if (settings->linear_stiffness == 0) //disabled smooth movement, jump directly
		{
			//set position
			camera.pos[0]-=pos_u[0]*dist;
			camera.pos[1]-=pos_u[1]*dist;
			camera.pos[2]-=pos_u[2]*dist;

			//chanses are we have an anchor distance of 0, then vel=wanted
			if (pos_wanted_l == 0)
			{
				camera.vel[0]=a_vel[0];
				camera.vel[1]=a_vel[1];
				camera.vel[2]=a_vel[2];
			}
			else //velocity towards/from anchor = 0
			{
				//relative vel
				dReal vel[3] = {camera.vel[0]-a_vel[0], camera.vel[1]-a_vel[1], camera.vel[2]-a_vel[2]};
				//vel towards anchor
				dReal dot = (pos_u[0]*vel[0]+pos_u[1]*vel[1]+pos_u[2]*vel[2]);

				//remove vel towards anchor
				camera.vel[0]-=pos_u[0]*dot;
				camera.vel[1]-=pos_u[1]*dot;
				camera.vel[2]-=pos_u[2]*dot;
			}
		}
		else //smooth movement
		{
			//how much acceleration (based on distance from wanted distance)
			dReal acceleration = time*(camera.settings->linear_stiffness)*dist;

			camera.vel[0]-=pos_u[0]*acceleration;
			camera.vel[1]-=pos_u[1]*acceleration;
			camera.vel[2]-=pos_u[2]*acceleration;
		}

		//perpendicular "angular" spring to move camera behind car
		if (pos_wanted_l > 0)
		{
			//dot between wanted and current rotation
			dReal dot = (pos_wanted_u[0]*pos_u[0] + pos_wanted_u[1]*pos_u[1] + pos_wanted_u[2]*pos_u[2]);

			if (dot < 1.0) //if we aren't exactly at wanted position (and prevent possibility of acos a number bigger than 1.0)
			{
				//angle
				dReal angle = acos(dot);

				//how much acceleration
				dReal accel = time*angle*(settings->angular_stiffness);

				//direction of acceleration (remove part of wanted that's along current pos)
				dReal dir[3];
				dir[0]=pos_wanted_u[0]-dot*pos_u[0];
				dir[1]=pos_wanted_u[1]-dot*pos_u[1];
				dir[2]=pos_wanted_u[2]-dot*pos_u[2];

				//not unit, get length and modify accel to compensate for not unit
				accel /= v_length(dir[0], dir[1], dir[2]);

				camera.vel[0]+=(accel*dir[0]);
				camera.vel[1]+=(accel*dir[1]);
				camera.vel[2]+=(accel*dir[2]);
			}
		}

		//
		//damping
		//
	
		if (settings->relative_damping)
		{
			//damping (of relative movement)
			//(recalculate relative vel, to account for current velocity)
			dReal vel[3] = {camera.vel[0]-a_vel[0], camera.vel[1]-a_vel[1], camera.vel[2]-a_vel[2]};

			dReal damping = (time*settings->damping);
			if (damping > 1)
				damping=1;

			camera.vel[0]-=damping*vel[0];
			camera.vel[1]-=damping*vel[1];
			camera.vel[2]-=damping*vel[2];
		}
		else
		{
			//absolute damping
			dReal damping = 1-(time*settings->damping);

			if (damping < 0)
				damping=0;

			camera.vel[0]*=damping;
			camera.vel[1]*=damping;
			camera.vel[2]*=damping;
		}

		//
		//move
		//
	
		//during the step, camera will have linear acceleration from old velocity to new
		//avarge velocity over the step is between new and old velocity
		camera.pos[0]+=((camera.vel[0]+old_vel[0])/2)*time;
		camera.pos[1]+=((camera.vel[1]+old_vel[1])/2)*time;
		camera.pos[2]+=((camera.vel[2]+old_vel[2])/2)*time;


		//smooth rotation (if enabled)
		//(move partially from current "up" to car "up", and make unit)

		const dReal *rotation = dBodyGetRotation (car->bodyid);
		dReal target_up[3];
		target_up[0] = rotation[2]*car->dir;
		target_up[1] = rotation[6]*car->dir;
		target_up[2] = rotation[10]*car->dir;

		if (settings->rotation_tightness == 0)
		{
			camera.up[0]=target_up[0];
			camera.up[1]=target_up[1];
			camera.up[2]=target_up[2];
		}
		else
		{
			dReal diff[3]; //difference between
			diff[0]=target_up[0]-camera.up[0];
			diff[1]=target_up[1]-camera.up[1];
			diff[2]=target_up[2]-camera.up[2];
			
			dReal movement=time*(settings->rotation_tightness);

			if (movement > 1)
				movement=1;

			camera.up[0]+=diff[0]*movement;
			camera.up[1]+=diff[1]*movement;
			camera.up[2]+=diff[2]*movement;

			//gluLookAt wants up to be unit
			dReal length=v_length(camera.up[0], camera.up[1], camera.up[2]);
			camera.up[0]/=length;
			camera.up[1]/=length;
			camera.up[2]/=length;
		}

		//smooth movement of target focus (if enabled)
		if (settings->target_tightness == 0)
		{
			camera.t_pos[0] = t_pos[0];
			camera.t_pos[1] = t_pos[1];
			camera.t_pos[2] = t_pos[2];
		}
		else
		{
			dReal diff[3], movement;

			diff[0]=t_pos[0]-camera.t_pos[0];
			diff[1]=t_pos[1]-camera.t_pos[1];
			diff[2]=t_pos[2]-camera.t_pos[2];

			movement = time*(settings->target_tightness);

			if (movement>1)
				movement=1;

			camera.t_pos[0]+=diff[0]*movement;
			camera.t_pos[1]+=diff[1]*movement;
			camera.t_pos[2]+=diff[2]*movement;
		}

		//set camera
		gluLookAt(camera.pos[0], camera.pos[1], camera.pos[2], camera.t_pos[0], camera.t_pos[1], camera.t_pos[2], camera.up[0], camera.up[1], camera.up[2]);
	}
	else
		gluLookAt (10, -10, 10, 0,0,0, 0,0,1);
}


