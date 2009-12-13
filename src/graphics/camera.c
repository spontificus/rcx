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
		//random values that might come handy

		//get position of target
		dVector3 target;
		dBodyGetRelPointPos (car->bodyid, settings->target[0], settings->target[1], settings->target[2]*car->dir, target);

		//position and velocity of wanted position
		dVector3 t_pos, t_vel;
		dBodyGetRelPointPos (car->bodyid, settings->anchor[0], settings->anchor[1], settings->anchor[2]*car->dir, t_pos);
		dBodyGetRelPointVel (car->bodyid, settings->anchor[0], settings->anchor[1], settings->anchor[2]*car->dir, t_vel);

		//relative vectors
		dReal pos[3] = {camera.pos[0]-t_pos[0], camera.pos[1]-t_pos[1], camera.pos[2]-t_pos[2]}; //rel to obj

		//vector lengths
		dReal pos_l = v_length(pos[0], pos[1], pos[2]);
		//dReal vel_l = v_length(vel[0], vel[1], vel[2]);

		//unit vectors
		dReal pos_u[3] = {pos[0]/pos_l, pos[1]/pos_l, pos[2]/pos_l};
		//dReal vel_u[3] = {vel[0]/vel_l, vel[1]/vel_l, vel[2]/vel_l};

		//spring physics
		dReal total_acceleration = time*camera.settings->stiffness*pos_l;

		camera.vel[0]-=pos_u[0]*total_acceleration;
		camera.vel[1]-=pos_u[1]*total_acceleration;
		camera.vel[2]-=pos_u[2]*total_acceleration;

		if (settings->relative_damping)
		{
			//damping (of relative movement)
			dReal vel[3] = {camera.vel[0]-t_vel[0], camera.vel[1]-t_vel[1], camera.vel[2]-t_vel[2]}; //rel to obj

			dReal damping = (time*settings->damping);
			if (damping > 1)
				printf("WTF?!\n");

			camera.vel[0]-=damping*vel[0];
			camera.vel[1]-=damping*vel[1];
			camera.vel[2]-=damping*vel[2];
		}
		else
		{
			//absolute damping
			dReal damping = 1-(time*settings->damping);

			if (damping < 0)
				printf("WTF?\n");

			camera.vel[0]*=damping;
			camera.vel[1]*=damping;
			camera.vel[2]*=damping;
		}

		//finally: move
		camera.pos[0]+=camera.vel[0]*time;
		camera.pos[1]+=camera.vel[1]*time;
		camera.pos[2]+=camera.vel[2]*time;


		//smooth rotation
		//(move partially from current "up" to car "up", and make unit)

		//get rotation of car body
		const dReal *rotation;
		rotation = dBodyGetRotation (car->bodyid);

		dReal target_up[3];
		target_up[0] = rotation[2]*car->dir;
		target_up[1] = rotation[6]*car->dir;
		target_up[2] = rotation[10]*car->dir;

		dReal diff[3]; //difference between
		diff[0]=target_up[0]-camera.up[0];
		diff[1]=target_up[1]-camera.up[1];
		diff[2]=target_up[2]-camera.up[2];
		
		dReal movement=time*camera.settings->rotation;
		camera.up[0]+=diff[0]*movement;
		camera.up[1]+=diff[1]*movement;
		camera.up[2]+=diff[2]*movement;

		//gluLookAt wants up to be unit
		dReal length=v_length(camera.up[0], camera.up[1], camera.up[2]);
		camera.up[0]/=length;
		camera.up[1]/=length;
		camera.up[2]/=length;
		//set camera
		gluLookAt(camera.pos[0], camera.pos[1], camera.pos[2], target[0], target[1], target[2], camera.up[0], camera.up[1], camera.up[2]);
	}
	else
		gluLookAt (10, -10, 10, 0,0,0, 0,0,1);
}


