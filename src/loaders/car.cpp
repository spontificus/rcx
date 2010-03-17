#include "../shared/car.hpp"
#include "../shared/printlog.hpp"
#include "../shared/track.hpp"
#include "../shared/geom.hpp"
#include "../shared/body.hpp"
#include "../shared/joint.hpp"
#include "colours.hpp"
#include "debug_draw.hpp"


car_struct *load_car (const char *path)
{
	printlog(1, "Loading car: %s", path);

	//see if already loaded
	car_struct *tmp = car_head;
	while (tmp)
	{
		if (!strcmp(tmp->name, path))
		{
			printlog(1, "(already loaded)");
			return tmp;
		}
		tmp = tmp->next;
	}

	//apparently not
	car_struct *target = allocate_car();
	target->name = (char *)calloc(strlen(path) + 1, sizeof(char));
	strcpy (target->name, path);

	char *conf=(char *)calloc(strlen(path)+9+1,sizeof(char));//+1 for \0
	strcpy (conf,path);
	strcat (conf,"/car.conf");

	if (load_conf(conf, (char *)target, car_index))
		return NULL;

	free (conf);



	//helper datas:
	//* inertia tensor for wheel axis (for translating motor torque to rotation speed cahnge)
	target->inertia_tensor = (target->wheel_mass*target->w[0]*target->w[0])/2; //(m*r*r)/2

	//* set up values for front/rear driving ratios
	if (target->steer_ratio>100 || target->steer_ratio<0 )
		printlog(0, "ERROR: front/rear steering ratio should be set between 0 and 100!");
	target->fsteer = (dReal) (target->steer_ratio/100.0);
	target->rsteer = (dReal) (target->fsteer-1.0);
	
	if (target->motor_ratio>100 || target->motor_ratio<0 )
		printlog(0, "ERROR: front/rear motor ratio should be set between 0 and 100!");
	target->fmotor = (dReal) (target->motor_ratio/100.0);
	target->rmotor = (dReal) (1.0-target->fmotor);

	if (target->break_ratio>100 || target->break_ratio<0 )
		printlog(0, "ERROR: front/rear breaking ratio should be set between 0 and 100!");
	target->fbreak = (dReal) (target->break_ratio/100.0);
	target->rbreak = (dReal) (1.0-target->fbreak);




	//graphics models
	float w_r = target->w[0];
	float w_w = target->w[1];
	//wheels:
	//(note: wheel axis is along z)
	target->wheel_graphics = allocate_file_3d();
	glNewList (target->wheel_graphics->list, GL_COMPILE);
	//tyre
	glMaterialfv (GL_FRONT, GL_AMBIENT_AND_DIFFUSE, black);
	glMaterialfv (GL_FRONT, GL_SPECULAR, dgray);
	glMateriali (GL_FRONT, GL_SHININESS, 30);

	glBegin (GL_QUAD_STRIP);
	float v;
	for (v=0; v<=2*M_PI; v+=2*M_PI/10)
	{
		glNormal3f (sin(v), cos(v), 0.0f);
		glVertex3f(w_r*sin(v), w_r*cos(v), -w_w/2.0f);
		glVertex3f(w_r*sin(v), w_r*cos(v), w_w/2.0f);
	}

	glMaterialfv (GL_FRONT, GL_SPECULAR, black);

	glEnd();
	//rim
	glMaterialfv (GL_FRONT, GL_AMBIENT_AND_DIFFUSE, lgray);
	glNormal3f (0.0f, 0.0f, 1.0f);
	glBegin (GL_QUADS);
		glVertex3f(w_r*0.9f, w_r/5, w_w/3.0f);
		glVertex3f(w_r*0.9f, -w_r/5, w_w/3.0f);
		glVertex3f(-w_r*0.9f, -w_r/5, w_w/3.0f);
		glVertex3f(-w_r*0.9f, w_r/5, w_w/3.0f);

		glVertex3f(w_r/5, w_r*0.9f, w_w/3.0f);
		glVertex3f(w_r/5, -w_r*0.9f, w_w/3.0f);
		glVertex3f(-w_r/5, -w_r*0.9f, w_w/3.0f);
		glVertex3f(-w_r/5, w_r*0.9f, w_w/3.0f);
	glEnd();

	glEndList();

	//loop through possible body geoms and make a model for them
	int i;
	for (i=0;i<CAR_MAX_BOXES;++i)
		if (target->box[i][0])
		{
			target->box_graphics[i] = allocate_file_3d();

			GLfloat *b = target->box[i];
			if (i==0)//first box
				debug_draw_box(target->box_graphics[i]->list,
						b[0],b[1],b[2], yellow, gray, 70);
			else
				debug_draw_box(target->box_graphics[i]->list,
						b[0],b[1],b[2], lgreen, gray, 70);
		}

	return target;
}


void spawn_car(car_struct *target, dReal x, dReal y, dReal z)
{
	printlog(1, "spawning car at: %f %f %f", x,y,z);

	printlog(1, "Warning: wheels will not collide to other wheels... (wheels use cylinders)");
	printlog(1, "(note to self: only solution would be moving to capped cylinders... :-/ )");

	if (target->spawned)
	{
		printlog(0, "ERROR: trying to spawn already spawned car!");
		//TODO: separate car into car_data and car_spawned to make one car spawnable for several players...
		return;
	}

	printlog(1, "TODO: antigravity forces");

	//not spawned, continue
	target->spawned = true;

	//create object to store components and joints
	target->object = new Object();
	new Space(target->object);

	dMass m;
	target->bodyid = dBodyCreate (world);
	dBodySetAutoDisableFlag (target->bodyid, 0); //never disable main body
	

	//set mass
	dMassSetBox (&m,1,target->body[0], target->body[1], target->body[2]); //sides
	dMassAdjust (&m,target->body_mass); //mass
	dBodySetMass (target->bodyid, &m);

	//set up air (and liquid) drag for body
	Body *bdata = new Body (target->bodyid, target->object);
	bdata->Set_Advanced_Linear_Drag (target->body_linear_drag[0], target->body_linear_drag[1], target->body_linear_drag[2]);
	//rotational drag
	bdata->Set_Angular_Drag (target->body_angular_drag);


	dBodySetPosition (target->bodyid, x, y, z);

	Geom *gdata;
	dGeomID geom;

	int i;
	dReal *b;
	for (i=0;i<CAR_MAX_BOXES;++i)
		if (target->box[i][0])
		{
			b = target->box[i];
			geom = dCreateBox(0,b[0],b[1],b[2]);
			gdata = new Geom (geom, target->object);

			dGeomSetBody (geom, target->bodyid);

			if (b[3]||b[4]||b[5]) //need offset
				dGeomSetOffsetPosition(geom,b[3],b[4],b[5]);

			//friction
			gdata->mu = target->body_mu;
			gdata->slip = target->body_slip;
			gdata->erp = target->body_erp;
			gdata->cfm = target->body_cfm;
			//graphics
			gdata->file_3d = target->box_graphics[i];


		}

	//side detection sensors:
	dReal *s = target->s;

	geom = dCreateBox(0,s[0],s[1],s[2]);
	target->sensor1 = new Geom (geom, target->object);
	target->sensor1->collide = false; //untouchable "ghost" geom - sensor
	dGeomSetBody (geom, target->bodyid);
	dGeomSetOffsetPosition(geom,0,0,-s[3]);

	geom = dCreateBox(0,s[0],s[1],s[2]);
	target->sensor2 = new Geom (geom, target->object);
	target->sensor2->collide = false; //sensor
	dGeomSetBody (geom, target->bodyid);
	dGeomSetOffsetPosition(geom,0,0,s[3]);

	//wheels:
	Geom *wheel_data[4];
	dGeomID wheel_geom;
	dBodyID wheel_body[4];
	for (i=0;i<4;++i)
	{
		//create cylinder
		//(geom)
		wheel_geom = dCreateCylinder (0, target->w[0], target->w[1]);

		//(body)
		wheel_body[i] = dBodyCreate (world);
		//never disable wheel body
		dBodySetAutoDisableFlag (wheel_body[i], 0);

		//3=z axis of cylinder
		dMassSetCylinder (&m, 1, 3, target->w[0], target->w[1]);
		dMassAdjust (&m, target->wheel_mass);
		dBodySetMass (wheel_body[i], &m);

		dGeomSetBody (wheel_geom, wheel_body[i]);

		//allocate (geom) data
		wheel_data[i] = new Geom(wheel_geom, target->object);

		//friction
		wheel_data[i]->mu = target->wheel_mu;
		wheel_data[i]->mu_rim = target->rim_mu;
		wheel_data[i]->wheel = true;
		wheel_data[i]->slip = target->wheel_slip;
		wheel_data[i]->bounce = target->wheel_bounce;

		//hardness
		wheel_data[i]->erp = target->wheel_erp;
		wheel_data[i]->cfm = target->wheel_cfm;


		//drag
		bdata = new Body (wheel_body[i], target->object);
		bdata->Set_Linear_Drag (target->wheel_linear_drag);
		//rotational drag
		bdata->Set_Angular_Drag (target->wheel_angular_drag);

		//graphics
		wheel_data[i]->file_3d = target->wheel_graphics;
		
		//(we need easy access to wheel body ids if using finite rotation)
		target->wheel_body[i] = wheel_body[i];
		target->wheel_geom_data[i] = wheel_data[i];
	}

	//place and rotate wheels
	dMatrix3 rot;
	dRFromAxisAndAngle (rot, 0, 1, 0, M_PI/2);
	dBodySetPosition (wheel_body[0], x+target->wp[0], y+target->wp[1], z);
	dBodySetRotation (wheel_body[0], rot);
	dBodySetPosition (wheel_body[1], x+target->wp[0], y-target->wp[1], z);
	dBodySetRotation (wheel_body[1], rot);

	dRFromAxisAndAngle (rot, 0, 1, 0, -M_PI/2);
	dBodySetPosition (wheel_body[2], x-target->wp[0], y-target->wp[1], z);
	dBodySetRotation (wheel_body[2], rot);
	dBodySetPosition (wheel_body[3], x-target->wp[0], y+target->wp[1], z);
	dBodySetRotation (wheel_body[3], rot);

	//enable finite rotation on rear wheels
	/*if (internal.finite_rotation)
	{
		dBodySetFiniteRotationMode (wheel_body[1], 1);
		dBodySetFiniteRotationMode (wheel_body[2], 1);
	}*/

	//create joints (hinge2) for wheels
	for (i=0; i<4; ++i)
	{
		target->joint[i]=dJointCreateHinge2 (world, 0);
		new Joint(target->joint[i], target->object);
		//body is still body of car main body
		dJointAttach (target->joint[i], target->bodyid, wheel_body[i]);
		dJointSetHinge2Axis1 (target->joint[i],0,0,1);
		dJointSetHinge2Axis2 (target->joint[i],1,0,0);

		//setup suspension
		dJointSetHinge2Param (target->joint[i],dParamSuspensionERP,target->suspension_erp);
		dJointSetHinge2Param (target->joint[i],dParamSuspensionCFM,target->suspension_cfm);

		//lock steering axis on all wheels
		dJointSetHinge2Param (target->joint[i],dParamLoStop,0);
		dJointSetHinge2Param (target->joint[i],dParamHiStop,0);

		//to easily get rotation speed (for slip in sideway), set all geom datas to specify connected hinge2
		wheel_data[i]->hinge2 = target->joint[i];
	}

	//to make it possible to tweak the hinge2 anchor x position:
	
	dJointSetHinge2Anchor (target->joint[0],x+target->jx,y+target->wp[1],z);
	dJointSetHinge2Anchor (target->joint[1],x+target->jx,y-target->wp[1],z);
	dJointSetHinge2Anchor (target->joint[2],x-target->jx,y-target->wp[1],z);
	dJointSetHinge2Anchor (target->joint[3],x-target->jx,y+target->wp[1],z);
}

void remove_car (car_struct* target)
{
	printlog(1, "removing car");
	delete (target->object);
	free_car (target);
}
