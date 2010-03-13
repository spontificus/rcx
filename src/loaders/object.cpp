#include "../shared/object.hpp"

#include <ode/ode.h>

#include "../shared/printlog.hpp"
#include "colours.hpp"
#include "../shared/track.hpp"
#include "debug_draw.hpp"
#include "../shared/joint.hpp"
#include "../shared/geom.hpp"
#include "../shared/body.hpp"

//load data for spawning object (object data), hard-coded debug version
//(objects are loaded as script instructions, executed for spawning)
script_struct *load_object(char *path)
{
	printlog(1, "Loading object: %s", path);

	script_struct *tmp = script_head;
	//see if already loaded
	while (tmp)
	{
		if (!strcmp(tmp->name, path))
		{
			printlog(1, "(already loaded)");
			return tmp;
		}
		tmp = tmp->next;
	}

	//new object
	script_struct *script;
	
	//currently no scripting, only hard-coded solutions
	if (!strcmp(path,"data/objects/misc/box"))
	{
		//"load" 3d box
		printlog(2, "(hard-coded box)");

		script = allocate_script();
		script->name = (char *)calloc(strlen(path) + 1, sizeof(char));
		strcpy (script->name, path);

		//the debug box will only spawn one component - one "3D file"
		script->graphics_debug1 = allocate_file_3d();
		debug_draw_box (script->graphics_debug1->list, 1,1,1, red,gray, 50);
		script->box = true;
	}
	else if (!strcmp(path, "data/objects/misc/flipper"))
	{
		printlog(2, "(hard-coded flipper)");

		script = allocate_script();
		script->name = (char *)calloc(strlen(path) + 1, sizeof(char));
		strcpy (script->name, path);

		script->graphics_debug1 = allocate_file_3d();
		script->graphics_debug2 = allocate_file_3d();

		debug_draw_box (script->graphics_debug1->list, 8,8,0.5, red,gray, 30);
		debug_draw_box (script->graphics_debug2->list, 3,3,2, lblue,black, 0);
		script->flipper = true;
	}
	else if (!strcmp(path, "data/objects/misc/NH4"))
	{
		printlog(2, "(hard-coded \"molecule\")");

		script = allocate_script();
		script->name = (char *)calloc(strlen(path) + 1, sizeof(char));
		strcpy (script->name, path);

		//draw approximate sphere
		script->graphics_debug1 = allocate_file_3d();
		debug_draw_sphere (script->graphics_debug1->list,2, lblue,white,42);
		script->graphics_debug2 = allocate_file_3d();
		debug_draw_sphere (script->graphics_debug2->list,1.6,white,white,42);

		script->NH4 = true;
	}
	else if (!strcmp(path, "data/objects/misc/sphere"))
	{
		printlog(2, "(hard-coded sphere)");

		script = allocate_script();
		script->name = (char *)calloc(strlen(path) + 1, sizeof(char));
		strcpy (script->name, path);

		//draw approximate sphere
		script->graphics_debug1 = allocate_file_3d();
		debug_draw_sphere (script->graphics_debug1->list,2, lblue,white,42);
//		script->graphics_debug2 = allocate_file_3d();
//		debug_draw_sphere (script->graphics_debug2->list,1.6,white,white,42);

		script->sphere = true;
	}
	else if (!strcmp(path, "data/objects/misc/building"))
	{
		printlog(2, "(hard-coded building)");

		//name
		script = allocate_script();
		script->name = (char *)calloc(strlen(path) + 1, sizeof(char));
		strcpy (script->name, path);

		//create graphics
		script->graphics_debug1 = allocate_file_3d(); //walls
		script->graphics_debug2 = allocate_file_3d(); //floor/ceiling
		script->graphics_debug3 = allocate_file_3d(); //pillars

		debug_draw_box (script->graphics_debug1->list, 4,0.4,2.7, dgray,black, 0);
		debug_draw_box (script->graphics_debug2->list, 4,4,0.2, lgray,gray, 30);

		glNewList (script->graphics_debug3->list, GL_COMPILE);

		glMaterialfv (GL_FRONT, GL_AMBIENT_AND_DIFFUSE, dgray);
		glMaterialfv (GL_FRONT, GL_SPECULAR, gray);
		glMateriali (GL_FRONT, GL_SHININESS, 30);

		glBegin (GL_QUAD_STRIP);
		float v;
		for (v=0; v<=2*M_PI; v+=2*M_PI/10)
		{
			glNormal3f (sin(v), cos(v), 0.0f);
			glVertex3f(sin(v)/2, cos(v)/2, 2.5/2.0f);
			glVertex3f(sin(v)/2, cos(v)/2, -2.5/2.0f);
		}
		glEnd();

		glMaterialfv (GL_FRONT, GL_SPECULAR, black);

		glEndList();

		script->building = true;
	}
	else if (!strcmp(path,"data/objects/misc/pillar"))
	{
		//"load" 3d box
		printlog(2, "(hard-coded pillar)");

		script = allocate_script();
		script->name = (char *)calloc(strlen(path) + 1, sizeof(char));
		strcpy (script->name, path);

		script->graphics_debug1 = allocate_file_3d();
		script->graphics_debug2 = allocate_file_3d();
		debug_draw_box (script->graphics_debug1->list, 2,2,5, gray,gray, 50); //complete
		debug_draw_box (script->graphics_debug2->list, 2,2,5/2, gray,gray, 50); //broken in half
		script->pillar = true;
	}


	else
	{
		printlog(0, "ERROR: path didn't match any hard-coded object");
		script = NULL;
	}

	return script;
}

//bind two bodies together using fixed joint (simplify connection of many bodies)
void debug_joint_fixed(dBodyID body1, dBodyID body2, object_struct *obj)
{
	dJointID joint;
	joint = dJointCreateFixed (world, 0);
	Joint *jd = new Joint(joint, obj);
	dJointAttach (joint, body1, body2);
	dJointSetFixed (joint);

	//use feedback
	//set threshold, buffer and dummy script
	jd->Set_Event(25000, 1000, (script_struct*)1337);
}

//spawn a "loaded" (actually hard-coded) object
//TODO: rotation
void spawn_object(script_struct *script, dReal x, dReal y, dReal z)
{
	printlog(1, "Spawning object at: %f %f %f", x,y,z);
	//prettend to be executing the script... just load debug values from
	//script structure
	//
	object_struct *obj;
	Body *bd;

	if (script->box)
	{
	printlog(2, "(hard-coded box)");
	//
	//
	//

	obj = allocate_object();

	dGeomID geom  = dCreateBox (0, 1,1,1); //geom
	Geom *data = new Geom(geom, obj);
	data->threshold = 100000;
	data->buffer = 100;
	dBodyID body = dBodyCreate (world);

	dMass m;
	dMassSetBox (&m,1,1,1,1); //sides
	dMassAdjust (&m,400); //mass
	dBodySetMass (body, &m);

	bd = new Body(body, obj); //just for drag
	//bd->Set_Event (100, 10, (script_struct*)1337);

	dGeomSetBody (geom, body);

	dBodySetPosition (body, x, y, z);

	//now add friction
	data->mu = 1;
	//use default
//	data->erp = 0.8;
//	data->cfm = 0.001;
//	data->slip1 = 0.0;
//	data->slip2 = 0.0;
//	data->bounce = 2.0;
	
	//Next, Graphics
	data->file_3d = script->graphics_debug1;

	//done
	//
	//
	}
	//
	//
	else if (script->flipper)
	{
	printlog(2, "(hard-coded flipper)");
	//
	//
	//

	//flipper surface
	obj = allocate_object();
	new Space(obj);
	
	dGeomID geom  = dCreateBox (0, 8,8,0.5); //geom
	Geom *data = new Geom(geom, obj);
	dGeomSetPosition (geom, x, y, z);

	//use default
//	data->mu = 1;
	data->erp = 0.8;
	data->cfm = 0.000;
//	data->slip1 = 0.0;
//	data->slip2 = 0.0;
//	data->bounce = 4.0;
	
	//Graphics
	data->file_3d = script->graphics_debug1;


	//flipper sensor
	dGeomID geom2 = dCreateBox (0, 3,3,2);
	data = new Geom(geom2, obj);
	data->collide = false;
	dGeomSetPosition (geom2, x, y, z+0.76);

	data->flipper_geom = geom; //tmp debug solution

	//graphics
	data->file_3d = script->graphics_debug2;
	//
	}
	//
	else if (script->NH4)
	{
	printlog(2, "(hard-coded \"molecule\")");
	//
	//
	//

	object_struct *obj = allocate_object();
	new Space(obj);

	//center sphere
	dGeomID geom  = dCreateSphere (0, 1); //geom
	Geom *data = new Geom(geom, obj);
	data->threshold = 100000;
	data->buffer = 100;

	dBodyID body1 = dBodyCreate (world);

	dMass m;
	dMassSetSphere (&m,1,1); //radius
	dMassAdjust (&m,60); //mass
	dBodySetMass (body1, &m);

	new Body (body1, obj);

	dGeomSetBody (geom, body1);

	dBodySetPosition (body1, x, y, z);

	data->mu = 0;
	data->bounce = 1.5;
	
	//Next, Graphics
	data->file_3d = script->graphics_debug1;

	dReal pos[4][3] = {
		{0, 0, 1.052},

		{0, 1, -0.326},
		{0.946,  -0.326, -0.326},
		{-0.946, -0.326, -0.326}};

	dJointID joint;
	dBodyID body;

	int i;
	for (i=0; i<4; ++i) {
	//connected spheres
	geom  = dCreateSphere (0, 0.8); //geom
	data = new Geom(geom, obj);
	data->threshold = 100000;
	data->buffer = 100;
	body = dBodyCreate (world);

	dMassSetSphere (&m,1,0.5); //radius
	dMassAdjust (&m,30); //mass
	dBodySetMass (body, &m);

	new Body (body, obj);

	dGeomSetBody (geom, body);

	dBodySetPosition (body, x+pos[i][0], y+pos[i][1], z+pos[i][2]);

	data->mu = 1;
	data->bounce = 2.0;
	
	//Next, Graphics
	data->file_3d = script->graphics_debug2;

	//connect to main sphere
	
	joint = dJointCreateBall (world, 0);

	Joint *jd = new Joint(joint, obj);
	jd->Set_Event(1000, 500, (script_struct*)1337);

	dJointAttach (joint, body1, body);
	dJointSetBallAnchor (joint, x+pos[i][0], y+pos[i][1], z+pos[i][2]);
	}
	//done
	//
	//
	}
	else if (script->sphere)
	{
	printlog(2, "(sphere)");
	//
	//
	//

	object_struct *obj = allocate_object();

	//center sphere
	dGeomID geom  = dCreateSphere (0, 1); //geom
	Geom *data = new Geom(geom, obj);
	data->threshold = 100000;
	data->buffer = 100;
	dBodyID body1 = dBodyCreate (world);

	dMass m;
	dMassSetSphere (&m,1,1); //radius
	dMassAdjust (&m,60); //mass
	dBodySetMass (body1, &m);

	new Body (body1, obj);

	dGeomSetBody (geom, body1);

	dBodySetPosition (body1, x, y, z);

	data->mu = 1;
	data->bounce = 1.5;
	
	//Next, Graphics
	data->file_3d = script->graphics_debug1;
	}
	//
	else if (script->building)
	{
	printlog(2, "(hard-coded building)");
	//
	//

	object_struct *obj = allocate_object(); //no space (no geoms collide)
	new Space(obj);
	dBodyID old_body[12] = {0,0,0,0,0,0,0,0,0,0,0,0};
	dBodyID old_pillar[4] = {0,0,0,0};

	dBodyID body[4];

	int j;
	for (j=0; j<2; ++j)
	{
		int i;
		dBodyID body1[12], body2[9];
		for (i=0; i<12; ++i)
		{
			dGeomID geom  = dCreateBox (0, 4,0.4,2.7); //geom
			Geom *data = new Geom(geom, obj);
			data->threshold = 100000;
			data->buffer = 10000;
			data->mu = 1;

			body1[i] = dBodyCreate (world);
			dGeomSetBody (geom, body1[i]);

			dMass m;
			dMassSetBox (&m,1,4,0.4,2.7); //sides
			dMassAdjust (&m,400); //mass
			dBodySetMass (body1[i], &m);

			new Body (body1[i], obj);

			data->file_3d = script->graphics_debug1;
		}
		
		const dReal k = 1.5*4+0.4/2;

		dBodySetPosition (body1[0], x-4, y-k, z+(2.7/2));
		dBodySetPosition (body1[1], x,   y-k, z+(2.7/2));
		dBodySetPosition (body1[2], x+4, y-k, z+(2.7/2));

		dBodySetPosition (body1[6], x+4, y+k, z+(2.7/2));
		dBodySetPosition (body1[7], x,   y+k, z+(2.7/2));
		dBodySetPosition (body1[8], x-4, y+k, z+(2.7/2));

		dMatrix3 rot;
		dRFromAxisAndAngle (rot, 0,0,1, M_PI/2);
		for (i=3; i<6; ++i)
			dBodySetRotation (body1[i], rot);
		for (i=9; i<12; ++i)
			dBodySetRotation (body1[i], rot);

		dBodySetPosition (body1[3], x+k,  y-4, z+(2.7/2));
		dBodySetPosition (body1[4], x+k, y, z+(2.7/2));
		dBodySetPosition (body1[5], x+k, y+4, z+(2.7/2));

		dBodySetPosition (body1[9], x-k, y+4, z+(2.7/2));
		dBodySetPosition (body1[10], x-k, y, z+(2.7/2));
		dBodySetPosition (body1[11], x-k, y-4, z+(2.7/2));

		//connect wall blocks in height
		for (i=0; i<12; ++i)
		{
			debug_joint_fixed(body1[i], old_body[i], obj);
			//move these bodies to list of old bodies
			old_body[i] = body1[i];
		}

		//connect wall blocks in sideway
		for (i=0; i<11; ++i)
			debug_joint_fixed (body1[i], body1[i+1], obj);
		debug_joint_fixed (body1[0], body1[11], obj);

		//walls done, floor/ceiling
		for (i=0; i<9; ++i)
		{
			dGeomID geom  = dCreateBox (0, 4,4,0.2); //geom
			Geom *data = new Geom(geom, obj);
			data->threshold = 100000;
			data->buffer = 10000;
			data->mu = 1;

			body2[i] = dBodyCreate (world);
			dGeomSetBody (geom, body2[i]);

			dMass m;
			dMassSetBox (&m,1,4,4,0.2); //sides
			dMassAdjust (&m,400); //mass
			dBodySetMass (body2[i], &m);

			new Body (body2[i], obj);

			data->file_3d = script->graphics_debug2;
		}

		const dReal k2=2.7-0.2/2;

		dBodySetPosition (body2[0], x-4, y-4, z+k2);
		debug_joint_fixed(body2[0], body1[0], obj);
		debug_joint_fixed(body2[0], body1[11], obj);
		dBodySetPosition (body2[1], x,   y-4, z+k2);
		debug_joint_fixed(body2[1], body1[1], obj);
		dBodySetPosition (body2[2], x+4, y-4, z+k2);
		debug_joint_fixed(body2[2], body1[2], obj);
		debug_joint_fixed(body2[2], body1[3], obj);

		dBodySetPosition (body2[3], x-4, y, z+k2);
		debug_joint_fixed(body2[3], body1[10], obj);
		dBodySetPosition (body2[4], x,   y, z+k2);
		dBodySetPosition (body2[5], x+4, y, z+k2);
		debug_joint_fixed(body2[5], body1[4], obj);

		dBodySetPosition (body2[6], x-4, y+4, z+k2);
		debug_joint_fixed(body2[6], body1[9], obj);
		debug_joint_fixed(body2[6], body1[8], obj);
		dBodySetPosition (body2[7], x,   y+4, z+k2);
		debug_joint_fixed(body2[7], body1[7], obj);
		dBodySetPosition (body2[8], x+4, y+4, z+k2);
		debug_joint_fixed(body2[8], body1[6], obj);
		debug_joint_fixed(body2[8], body1[5], obj);

		//join floor blocks
		//1: horisontal
		debug_joint_fixed (body2[0], body2[1], obj);
		debug_joint_fixed (body2[1], body2[2], obj);
		debug_joint_fixed (body2[3], body2[4], obj);
		debug_joint_fixed (body2[4], body2[5], obj);
		debug_joint_fixed (body2[6], body2[7], obj);
		debug_joint_fixed (body2[7], body2[8], obj);
		//2: vertical
		debug_joint_fixed (body2[0], body2[3], obj);
		debug_joint_fixed (body2[3], body2[6], obj);
		debug_joint_fixed (body2[1], body2[4], obj);
		debug_joint_fixed (body2[4], body2[7], obj);
		debug_joint_fixed (body2[2], body2[5], obj);
		debug_joint_fixed (body2[5], body2[8], obj);
	
		//pillars
		dGeomID geom;
		Geom *data;
		for (i=0; i<4; ++i)
		{
			geom  = dCreateCapsule (0, 0.5,1.5); //geom
			data = new Geom(geom, obj);
			data->threshold = 100000;
			data->buffer = 10000;
			body[i] = dBodyCreate (world);
	
			dMass m;
			dMassSetCapsule (&m,1,3,1,0.5); //sides (3=z-axis)
			dMassAdjust (&m,400); //mass
			dBodySetMass (body[i], &m);
	
			new Body (body[i], obj);

			dGeomSetBody (geom, body[i]);
	
			//friction
			data->mu = 1;
			//Next, Graphics
			data->file_3d = script->graphics_debug3;
		}

		dBodySetPosition (body[0], x+2, y+2, z+2.5/2);
		debug_joint_fixed(body[0], body2[8], obj);
		debug_joint_fixed(body[0], body2[7], obj);
		debug_joint_fixed(body[0], body2[5], obj);
		debug_joint_fixed(body[0], body2[4], obj);

		dBodySetPosition (body[1], x+2, y-2, z+2.5/2);
		debug_joint_fixed(body[0], body2[1], obj);
		debug_joint_fixed(body[0], body2[2], obj);
		debug_joint_fixed(body[0], body2[4], obj);
		debug_joint_fixed(body[0], body2[5], obj);

		dBodySetPosition (body[2], x-2, y+2, z+2.5/2);
		debug_joint_fixed(body[0], body2[7], obj);
		debug_joint_fixed(body[0], body2[6], obj);
		debug_joint_fixed(body[0], body2[4], obj);
		debug_joint_fixed(body[0], body2[3], obj);

		dBodySetPosition (body[3], x-2, y-2, z+2.5/2);
		debug_joint_fixed(body[0], body2[0], obj);
		debug_joint_fixed(body[0], body2[1], obj);
		debug_joint_fixed(body[0], body2[3], obj);
		debug_joint_fixed(body[0], body2[4], obj);

		for (i=0; i<4; ++i)
		{
			debug_joint_fixed(body[i], old_pillar[i], obj);
			old_pillar[i] = body[i];
		}

		z+=2.7;
	}
	//
	//
	}
	//
	//
	else if (script->pillar)
	{
		printlog(2, "(hard-coded pillar)");

		//just one geom in this object
		Geom *g = new Geom(dCreateBox(0, 2,2,5), new object_struct);

		//position
		dGeomSetPosition(g->geom_id, x,y,z+5/2);

		//render
		g->file_3d = script->graphics_debug1;

		//identification
		g->TMP_pillar_geom = true;

		//destruction
		g->threshold = 200000;
		g->buffer = 10000;
		g->script = script; //got some data well need
	}
	else
		printlog(0, "ERROR: trying to spawn unidentified object?!");

}

//removes an object
void remove_object(object_struct *target)
{
	//lets just hope the given pointer is ok...
	printlog(1, "remove object");

	//if (target->space)
	//{
		//printlog(1, "(space)");
		//printlog(1, "TODO: loop through space...");
		//dSpaceDestroy (target->space);
	//}

	/*
	if (target->cmp_count != 0)
	{
		printf("ERROR: got components, search&remove)\n");
		component *cmp = component_head;
		//loop through all components, see if any match
		while (cmp)
		{
			//only free, if geoms, they are destroyed when destroying space (above)
			if (cmp->object_parent == target)
				free_component (cmp);
			cmp=cmp->next;
		}
	}*/

	free_object (target);
}
