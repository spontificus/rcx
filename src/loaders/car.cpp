#include "../shared/car.hpp"
#include "../shared/printlog.hpp"
#include "../shared/track.hpp"
#include "../shared/geom.hpp"
#include "../shared/body.hpp"
#include "../shared/joint.hpp"
#include "colours.hpp"
#include "debug_draw.hpp"


Car_Template *Car_Template::Load (const char *path)
{
	printlog(1, "Loading car: %s", path);

	//see if already loaded
	Racetime_Data *tmp;
	if ((tmp = Racetime_Data::Find(path)))
	{
		printlog(1, "(already loaded)");

		Car_Template *tmp_car = dynamic_cast<Car_Template *>(tmp);

		if (!tmp_car)
			printlog(0, "ERROR: could not convert Racetime_Data class \"%s\" to Car_Template class!");
		else
			return tmp_car; //abort loading of car, return already loaded one
	}

	//apparently not
	Car_Template *target = new Car_Template(path);

	char *conf=(char *)calloc(strlen(path)+9+1,sizeof(char));//+1 for \0
	strcpy (conf,path);
	strcat (conf,"/car.conf");

	if (load_conf(conf, (char *)&target->conf, Car_Template::conf_index))
		return NULL;

	free (conf);



	//helper datas:
	//* inertia tensor for wheel axis (for translating motor torque to rotation speed cahnge)
	target->inertia_tensor = (target->conf.wheel_mass*target->conf.w[0]*target->conf.w[0])/2; //(m*r*r)/2

	//* set up values for front/rear driving ratios
	if (target->conf.steer_ratio>100 || target->conf.steer_ratio<0 )
	{
		printlog(0, "ERROR: front/rear steering ratio should be set between 0 and 100!");
		target->conf.steer_ratio=0;
	}

	target->fsteer = (dReal) (target->conf.steer_ratio/100.0);
	target->rsteer = (dReal) (target->fsteer-1.0);
	
	if (target->conf.motor_ratio>100 || target->conf.motor_ratio<0 )
	{
		printlog(0, "ERROR: front/rear motor ratio should be set between 0 and 100!");
		target->conf.motor_ratio=0;
	}

	target->fmotor = (dReal) (target->conf.motor_ratio/100.0);
	target->rmotor = (dReal) (1.0-target->fmotor);

	if (target->conf.break_ratio>100 || target->conf.break_ratio<0 )
	{
		printlog(0, "ERROR: front/rear breaking ratio should be set between 0 and 100!");
		target->conf.break_ratio=0;
	}

	target->fbreak = (dReal) (target->conf.break_ratio/100.0);
	target->rbreak = (dReal) (1.0-target->fbreak);




	//graphics models
	float w_r = target->conf.w[0];
	float w_w = target->conf.w[1];
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
	/*int i;
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
		}*/

	return target;
}


Car *Car_Template::Spawn (dReal x, dReal y, dReal z)
{
	printlog(1, "spawning car at: %f %f %f", x,y,z);

	printlog(1, "Warning: wheels will not collide to other wheels... (wheels use cylinders)");
	printlog(1, "(note to self: only solution would be moving to capped cylinders... :-/ )");


	printlog(1, "TODO: antigravity forces");


	//begin copying of needed configuration data
	Car *car = new Car();
	car->max_torque = conf.max_torque;
	car->motor_tweak = conf.motor_tweak;
	car->max_break = conf.max_break;
	car->torque_compensator = conf.torque_compensator;
	car->fsteer = fsteer;
	car->rsteer = rsteer;
	car->fmotor = fmotor;
	car->rmotor = rmotor;
	car->fbreak = fbreak;
	car->rbreak = rbreak;
	car->inertia_tensor = inertia_tensor;

	//start building
	new Space(car);

	dMass m;
	car->bodyid = dBodyCreate (world);
	dBodySetAutoDisableFlag (car->bodyid, 0); //never disable main body
	

	//set mass
	dMassSetBox (&m,1,conf.body[0], conf.body[1], conf.body[2]); //sides
	dMassAdjust (&m,conf.body_mass); //mass
	dBodySetMass (car->bodyid, &m);

	//set up air (and liquid) drag for body
	Body *bdata = new Body (car->bodyid, car);
	bdata->Set_Advanced_Linear_Drag (conf.body_linear_drag[0], conf.body_linear_drag[1], conf.body_linear_drag[2]);
	//rotational drag
	bdata->Set_Angular_Drag (conf.body_angular_drag);


	dBodySetPosition (car->bodyid, x, y, z);

	dGeomID geom;
	/*Geom *gdata;

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


		}*/

	//side detection sensors:
	dReal *s = conf.s;

	geom = dCreateBox(0,s[0],s[1],s[2]);
	car->sensor1 = new Geom (geom, car);
	car->sensor1->collide = false; //untouchable "ghost" geom - sensor
	dGeomSetBody (geom, car->bodyid);
	dGeomSetOffsetPosition(geom,0,0,-s[3]);

	geom = dCreateBox(0,s[0],s[1],s[2]);
	car->sensor2 = new Geom (geom, car);
	car->sensor2->collide = false; //sensor
	dGeomSetBody (geom, car->bodyid);
	dGeomSetOffsetPosition(geom,0,0,s[3]);

	//wheels:
	Geom *wheel_data[4];
	dGeomID wheel_geom;
	dBodyID wheel_body[4];
	int i;
	for (i=0;i<4;++i)
	{
		//create cylinder
		//(geom)
		wheel_geom = dCreateCylinder (0, conf.w[0], conf.w[1]);

		//(body)
		wheel_body[i] = dBodyCreate (world);
		//never disable wheel body
		dBodySetAutoDisableFlag (wheel_body[i], 0);

		//3=z axis of cylinder
		dMassSetCylinder (&m, 1, 3, conf.w[0], conf.w[1]);
		dMassAdjust (&m, conf.wheel_mass);
		dBodySetMass (wheel_body[i], &m);

		dGeomSetBody (wheel_geom, wheel_body[i]);

		//allocate (geom) data
		wheel_data[i] = new Geom(wheel_geom, car);

		//friction
		wheel_data[i]->mu = conf.wheel_mu;
		wheel_data[i]->mu_rim = conf.rim_mu;
		wheel_data[i]->wheel = true;
		wheel_data[i]->slip = conf.wheel_slip;
		wheel_data[i]->bounce = conf.wheel_bounce;

		//hardness
		wheel_data[i]->erp = conf.wheel_erp;
		wheel_data[i]->cfm = conf.wheel_cfm;


		//drag
		bdata = new Body (wheel_body[i], car);
		bdata->Set_Linear_Drag (conf.wheel_linear_drag);
		//rotational drag
		bdata->Set_Angular_Drag (conf.wheel_angular_drag);

		//graphics
		wheel_data[i]->file_3d = wheel_graphics;
		
		//(we need easy access to wheel body ids if using finite rotation)
		car->wheel_body[i] = wheel_body[i];
		car->wheel_geom_data[i] = wheel_data[i];
	}

	//place and rotate wheels
	dMatrix3 rot;
	dRFromAxisAndAngle (rot, 0, 1, 0, M_PI/2);
	dBodySetPosition (wheel_body[0], x+conf.wp[0], y+conf.wp[1], z);
	dBodySetRotation (wheel_body[0], rot);
	dBodySetPosition (wheel_body[1], x+conf.wp[0], y-conf.wp[1], z);
	dBodySetRotation (wheel_body[1], rot);

	dRFromAxisAndAngle (rot, 0, 1, 0, -M_PI/2);
	dBodySetPosition (wheel_body[2], x-conf.wp[0], y-conf.wp[1], z);
	dBodySetRotation (wheel_body[2], rot);
	dBodySetPosition (wheel_body[3], x-conf.wp[0], y+conf.wp[1], z);
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
		car->joint[i]=dJointCreateHinge2 (world, 0);
		new Joint(car->joint[i], car);
		//body is still body of car main body
		dJointAttach (car->joint[i], car->bodyid, wheel_body[i]);
		dJointSetHinge2Axis1 (car->joint[i],0,0,1);
		dJointSetHinge2Axis2 (car->joint[i],1,0,0);

		//setup suspension
		dJointSetHinge2Param (car->joint[i],dParamSuspensionERP,conf.suspension_erp);
		dJointSetHinge2Param (car->joint[i],dParamSuspensionCFM,conf.suspension_cfm);

		//lock steering axis on all wheels
		dJointSetHinge2Param (car->joint[i],dParamLoStop,0);
		dJointSetHinge2Param (car->joint[i],dParamHiStop,0);

		//to easily get rotation speed (for slip in sideway), set all geom datas to specify connected hinge2
		wheel_data[i]->hinge2 = car->joint[i];
	}

	//to make it possible to tweak the hinge2 anchor x position:
	
	dJointSetHinge2Anchor (car->joint[0],x+conf.jx,y+conf.wp[1],z);
	dJointSetHinge2Anchor (car->joint[1],x+conf.jx,y-conf.wp[1],z);
	dJointSetHinge2Anchor (car->joint[2],x-conf.jx,y-conf.wp[1],z);
	dJointSetHinge2Anchor (car->joint[3],x-conf.jx,y+conf.wp[1],z);

	//return
	return car;
}

