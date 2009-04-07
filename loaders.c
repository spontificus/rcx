//different loaders for files (3D, conf, scripts...), cars, weapons and so on.
//also handles spawning of them, and removing
//(allocating and freeing is handled in shared.c)


int load_world_debug (float size)
{
	//build hard-coded world, with gravity and box for environment

	printf("--- Loading (debug) world\n");
	glClearColor (0.0f, 0.0f, 1.0f, 1.0f); //nice blue "sky"
	dWorldSetGravity (world,0,0,-9.8);

	//ground plane
	track.plane[0] = allocate_component(NULL, NULL); //no data, no owning object
	track.plane[0]->geom_id = dCreatePlane (space,0,0,1,0);
	//--IMPORTANT--\\
	//to make it possible to add special reactions on collisions and
	//friction (again on collisions) - add pointer to the cmp to
	//geom data
	dGeomSetData (track.plane[0]->geom_id, (void*)(component*)(track.plane[0]));//???
	

	//4 more planes as walls
	track.plane[1] = allocate_component(NULL, NULL);
	track.plane[1]->geom_id = dCreatePlane (space,1,0,0,-size);
	dGeomSetData (track.plane[1]->geom_id, (void*)(component*)(track.plane[1]));//???

	track.plane[2] = allocate_component(NULL, NULL);
	track.plane[2]->geom_id = dCreatePlane (space,-1,0,0,-size);
	dGeomSetData (track.plane[2]->geom_id, (void*)(component*)(track.plane[2]));//???

	track.plane[3] = allocate_component(NULL, NULL);
	track.plane[3]->geom_id = dCreatePlane (space,0,1,0,-size);
	dGeomSetData (track.plane[3]->geom_id, (void*)(component*)(track.plane[3]));//???

	track.plane[4] = allocate_component(NULL, NULL);
	track.plane[4]->geom_id = dCreatePlane (space,0,-1,0,-size);
	dGeomSetData (track.plane[4]->geom_id, (void*)(component*)(track.plane[4]));//???


	//since a plane is a non-placeable geom, the sepparate components will
	//not be "rendered" separately, instead create one 3d image sepparately

	track.graphics_list = allocate_graphics_list();
	glNewList (track.graphics_list->render_list, GL_COMPILE);
	//the ground and walls for the environment box
	glColor3f (0.0f, 1.0f, 0.0f);
	glBegin (GL_QUADS);
	glVertex3f (-size, -size, 0.0f);
	glVertex3f (-size, size, 0.0f);
	glVertex3f (size, size, 0.0f);
	glVertex3f (size, -size, 0.0f);
	glEnd();

	glColor3f (0.5f, 0.5f, 0.5f);
	glBegin (GL_QUAD_STRIP);
	glVertex3f (-size, -size, 0.0f);
	glVertex3f (-size, -size, 10.0f);
	glVertex3f (-size, size, 0.0f);
	glVertex3f (-size, size, 10.0f);
	glVertex3f (size, size, 0.0f);
	glVertex3f (size, size, 10.0f);
	glVertex3f (size, -size, 0.0f);
	glVertex3f (size, -size, 10.0f);
	glVertex3f (-size, -size, 0.0f);
	glVertex3f (-size, -size, 10.0f);
	glEnd();

	//draw black lines to make corners more "sharp"
	glColor3f (0.0f, 0.0f, 0.0f);
	glBegin (GL_LINE_LOOP);
	glVertex3f (-(size - 0.1), (size - 0.1), 0.1f);
	glVertex3f ((size - 0.1), (size - 0.1), 0.1f);
	glVertex3f ((size - 0.1), -(size - 0.1), 0.1f);
	glVertex3f (-(size - 0.1), -(size - 0.1), 0.1f);
	glEnd();

	glBegin (GL_LINE_LOOP);
	glVertex3f (-size, size, 10.0f);
	glVertex3f (size, size, 10.0f);
	glVertex3f (size, -size, 10.0f);
	glVertex3f (-size, -size, 10.0f);
	glEnd();

	
	glBegin (GL_LINES);
	glVertex3f (-(size - 0.1), (size - 0.1), 0.0f);
	glVertex3f (-(size - 0.1), (size - 0.1), 10.0f);

	glVertex3f ((size - 0.1), (size - 0.1), 0.0f);
	glVertex3f ((size - 0.1), (size - 0.1), 10.0f);

	glVertex3f ((size - 0.1), -(size - 0.1), 0.0f);
	glVertex3f ((size - 0.1), -(size - 0.1), 10.0f);

	glVertex3f (-(size - 0.1), -(size - 0.1), 0.0f);
	glVertex3f (-(size - 0.1), -(size - 0.1), 10.0f);
	glEnd();

	glEndList();

	//that's it!
	printf("---\n\n");
	return 0;
}

//load data for spawning object (object data), hard-coded debug version
//(objects are loaded as script instructions, executed for spawning)
script *load_object_debug(void)
{
	printf("--- Loading debug box\n");
	//"load" 3d box
	script *script = allocate_script();

	//the debug box will only spawn one component - one "3D file"
	script->graphics_debug = allocate_graphics_list();
	glNewList (script->graphics_debug->render_list, GL_COMPILE);

	glColor3f (1.0f, 0.0f, 0.0f);
	glBegin (GL_QUAD_STRIP);
	glVertex3f (-0.5, -0.5, -0.5);
	glVertex3f (-0.5, -0.5, 0.5);

	glVertex3f (-0.5, 0.5, -0.5);
	glVertex3f (-0.5, 0.5, 0.5);

	glVertex3f (0.5, 0.5, -0.5);
	glVertex3f (0.5, 0.5, 0.5);

	glVertex3f (0.5, -0.5, -0.5);
	glVertex3f (0.5, -0.5, 0.5);

	//
	
	glVertex3f (-0.5, 0.5, 0.5);
	glVertex3f (0.5, 0.5, 0.5);

	glVertex3f (-0.5, -0.5, 0.5);
	glVertex3f (0.5, -0.5, 0.5);

	glVertex3f (-0.5, -0.5, -0.5);
	glVertex3f (0.5, -0.5, -0.5);

	glVertex3f (-0.5, 0.5, -0.5);
	glVertex3f (0.5, 0.5, -0.5);
	glEnd();

	//black lines to sharpen the edges
	glColor3f (0.0f, 0.0f, 0.0f);
	glBegin (GL_LINE_LOOP);
	glVertex3f (-0.51, 0.51, -0.51);
	glVertex3f (-0.51, -0.51, -0.51);
	glVertex3f (0.51, -0.51, -0.51);
	glVertex3f (0.51, 0.51, -0.51);
	glEnd();

	glBegin (GL_LINE_LOOP);
	glVertex3f (-0.51, 0.51, 0.51);
	glVertex3f (-0.51, -0.51, 0.51);
	glVertex3f (0.51, -0.51, 0.51);
	glVertex3f (0.51, 0.51, 0.51);
	glEnd();

	glBegin (GL_LINES);
	glVertex3f (-0.51, 0.51, -0.51);
	glVertex3f (-0.51, 0.51, 0.51);

	glVertex3f (-0.51, -0.51, -0.51);
	glVertex3f (-0.51, -0.51, 0.51);

	glVertex3f (0.51, -0.51, 0.51);
	glVertex3f (0.51, -0.51, -0.51);

	glVertex3f (0.51, 0.51, 0.51);
	glVertex3f (0.51, 0.51, -0.51);
	glEnd();

	glEndList();

	//placeholder, TODO: add phys data
	printf("---\n\n");
	return script;
}

//hard-coded to spawn a box ("loaded" in the function above, given as argument) TODO: rotation
void spawn_object_debug(script *script, dReal x, dReal y, dReal z)
{
	printf("-- spawning debug box\n");
	//prettend to be executing the script... just load debug values from script structure
	//
	//
	//
	//
	//first allocate an object structure
//	object* obj = allocate_object();
//	obj->space_id = dSimpleSpaceCreate(space);

	//now allocate the component which should be spawned
//	component_data *cmp_d = obj_d->component_data_head; //first component_data shall be spawned




	//not point in assigning een triggered script to component
	component* cmp = allocate_component(NULL, NULL);
//	component* cmp = allocate_component(cmp_d, NULL);



//	cmp->data = object_data_head->component_data_head;

	//two steps: physics (box with mass and friction) and graphics (rendering list)
	//physics
	//(not assigning to any object...)
//	cmp->geom_id = dCreateBox (obj->space_id, 1,1,1); //geom
	cmp->geom_id = dCreateBox (space, 1,1,1); //geom
//	cmp->geom_id = dCreateSphere (space, 1); //geom

	//--IMPORTANT--\\
	//to make it possible to add special reactions on collisions and
	//friction (again on collisions) - add pointer to the cmp to
	//geom data
	dGeomSetData (cmp->geom_id, (void*)(component*)(cmp));//???
	

	cmp->body_id = dBodyCreate (world);

	dMass m;
	dMassSetBox (&m,1,1,1,1); //sides
	dMassAdjust (&m,1); //mass
	cmp->body_id = dBodyCreate (world);
	dBodySetMass (cmp->body_id, &m);
	dBodySetPosition (cmp->body_id, x, y, z);

	dGeomSetBody (cmp->geom_id, cmp->body_id);

	//now add friction
	cmp->mu = 10000;
	cmp->erp = 0.8;
	cmp->cfm = 0.0; //default
	cmp->slip1 = 1.0;
	cmp->slip2 = 1.0;
	
	//Next, Graphics
	cmp->graphics_list = script->graphics_debug;

        // everything is a cube
        cmp->c = new_cube(0.5);

	//done
//	focused_geom = cmp->geom_id;
	printf("--\n\n");
}

//size is wheel diameter in meters
//TODO: rotation
car *load_car_debug(dReal size)
{
	printf("--- Loading (debug) car\n");
	//currently, no data loaded, except allocating and setting basic values
        
        struct surface *ns;
        struct surfaces *nsc;
        
	car *target = allocate_car();

	//set wheel data
	target->wheel_mu = dInfinity;
	target->wheel_slip = 0.005/(size*size*size);

	//TODO: use size?
	target->wheel_erp = 0.8;
	target->wheel_cfm = 0.0;

	//set cars wheel torques (also for wheels)
	target->max_torque = (size*size*size*size)*200;
	target->max_break = (size*size*size*size)*200;

	//wheel suspension
	target->suspension_erp = 0.9/(size*size*size*size*size);
	target->suspension_cfm = 0.010/(size*size*size*size*size);

	//TODO: add bounce settings to wheels?
	//set mass
	target->body_mass = (size*size*size)*20;
	target->wheel_mass = (size*size*size)*10;

	//set debug sizes for wheels and body (and position)
	//(note: local vars for simplifying)
	float b_x = target->b_x=size*2;
	float b_y = target->b_y=size*3;
	float b_z = target->b_z=size/3;
	float w_r = target->w_r=size/2;
	float w_w = target->w_w=size/2;
	float w_x = target->w_x=size*1.5;//position
	float w_y = target->w_y=size*1;//position
	//create rendering data
	//wheels:
	//(not: wheel axis is along z)
	target->wheel_graphics = allocate_graphics_list();
	glNewList (target->wheel_graphics->render_list, GL_COMPILE);
        
        struct surfaces *mys = NULL;
        
	//tyre
	glColor3f (0.0f, 0.0f, 0.0f);
	glBegin (GL_QUAD_STRIP);
	float v;
        float inc = 2.0*M_PI/10.0;
        float surf[4][3];
        int first = 1;
        
	for (v=0; v<=2*M_PI; v+= inc)
	{
		glVertex3f(w_r*sin(v), w_r*cos(v), -w_w/2.0f);
		glVertex3f(w_r*sin(v), w_r*cos(v), w_w/2.0f);
                
                if (first == 1) {
                  first = 0;
                  surf[0][0] = w_r*sin(v);
                  surf[0][1] = w_r*cos(v);
                  surf[0][2] = -w_w/2.0f;
                  
                  surf[1][0] = w_r*sin(v);
                  surf[1][1] = w_r*cos(v);
                  surf[1][2] = w_w/2.0f;
                }
                
                surf[2][0] = w_r*sin(v+inc);
                surf[2][1] = w_r*cos(v+inc);
                surf[2][2] = -w_w/2.0f;
                
                surf[3][0] = w_r*sin(v+inc);
                surf[3][1] = w_r*cos(v+inc);
                surf[3][2] = w_w/2.0f;
                
                ns = new_surface(surf);
                nsc = malloc(sizeof(surfaces));;
                nsc->s = ns;
                nsc->nxt = mys;
                mys = nsc;
                
                surf[0][0] = surf[3][0];
                surf[0][1] = surf[3][1];
                surf[0][2] = surf[3][2];
                
                surf[1][0] = surf[2][0];
                surf[1][1] = surf[2][1];
                surf[1][2] = surf[2][2];
	}
	glEnd();
	//rim
	glColor3f (0.7f, 0.7f, 0.7f);
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

	//body:
	target->body_graphics = allocate_graphics_list();
	glNewList (target->body_graphics->render_list, GL_COMPILE);
	glColor3f(1.0f, 1.0f, 0.0f);
	glBegin(GL_QUAD_STRIP);
		glVertex3f(b_x/2.0f, b_y/2.0f, b_z/2.0f);
		glVertex3f(-b_x/2.0f, b_y/2.0f, b_z/2.0f);

		glVertex3f(b_x/2.0f, -b_y/2.0f, b_z/2.0f);
		glVertex3f(-b_x/2.0f, -b_y/2.0f, b_z/2.0f);

                surf[0][0] = b_x/2.0f;
                surf[0][1] = b_y/2.0f;
                surf[0][2] = b_z/2.0f;
                
                surf[1][0] = -b_x/2.0f;
                surf[1][1] = b_y/2.0f;
                surf[1][2] = b_z/2.0f;
                
                surf[2][0] = -b_x/2.0f;
                surf[2][1] = -b_y/2.0f;
                surf[2][2] = b_z/2.0f;
                
                surf[3][0] = b_x/2.0f;
                surf[3][1] = -b_y/2.0f;
                surf[3][2] = b_z/2.0f;
                
                ns = new_surface(surf);
                nsc = malloc(sizeof(surfaces));
                nsc->s = ns;
                nsc->nxt = mys;
                mys = nsc;

                surf[0][0] = surf[3][0];
                surf[0][1] = surf[3][1];
                surf[0][2] = surf[3][2];
                
                surf[1][0] = surf[2][0];
                surf[1][1] = surf[2][1];
                surf[1][2] = surf[2][2];

		glVertex3f(b_x/2.0f, -b_y/2.0f, -b_z/2.0f);
		glVertex3f(-b_x/2.0f, -b_y/2.0f, -b_z/2.0f);
               
                surf[2][0] = -b_x/2.0f;
                surf[2][1] = -b_y/2.0f;
                surf[2][2] = -b_z/2.0f;
                
                surf[3][0] = b_x/2.0f;
                surf[3][1] = -b_y/2.0f;
                surf[3][2] = -b_z/2.0f;
                
                ns = new_surface(surf);
                nsc = malloc(sizeof(surfaces));
                nsc->s = ns;
                nsc->nxt = mys;
                mys = nsc;

                surf[0][0] = surf[3][0];
                surf[0][1] = surf[3][1];
                surf[0][2] = surf[3][2];
                
                surf[1][0] = surf[2][0];
                surf[1][1] = surf[2][1];
                surf[1][2] = surf[2][2];

		glVertex3f(b_x/2.0f, b_y/2.0f, -b_z/2.0f);
		glVertex3f(-b_x/2.0f, b_y/2.0f, -b_z/2.0f);

                surf[2][0] = -b_x/2.0f;
                surf[2][1] = -b_y/2.0f;
                surf[2][2] = -b_z/2.0f;
                
                surf[3][0] = b_x/2.0f;
                surf[3][1] = b_y/2.0f;
                surf[3][2] = -b_z/2.0f;
                
                ns = new_surface(surf);
                nsc = malloc(sizeof(surfaces));
                nsc->s = ns;
                nsc->nxt = mys;
                mys = nsc;

                surf[0][0] = surf[3][0];
                surf[0][1] = surf[3][1];
                surf[0][2] = surf[3][2];
                
                surf[1][0] = surf[2][0];
                surf[1][1] = surf[2][1];
                surf[1][2] = surf[2][2];

		glVertex3f(b_x/2.0f, b_y/2.0f, b_z/2.0f);
		glVertex3f(-b_x/2.0f, b_y/2.0f, b_z/2.0f);
                
                surf[2][0] = -b_x/2.0f;
                surf[2][1] = b_y/2.0f;
                surf[2][2] = b_z/2.0f;
                
                surf[3][0] = b_x/2.0f;
                surf[3][1] = b_y/2.0f;
                surf[3][2] = b_z/2.0f;
                
                ns = new_surface(surf);
                nsc = malloc(sizeof(surfaces));
                nsc->s = ns;
                nsc->nxt = mys;
                mys = nsc;

                
	glEnd();
	glBegin(GL_QUADS);
		glVertex3f(b_x/2.0f, b_y/2.0f, b_z/2.0f);
		glVertex3f(b_x/2.0f, b_y/2.0f, -b_z/2.0f);
		glVertex3f(b_x/2.0f, -b_y/2.0f, -b_z/2.0f);
		glVertex3f(b_x/2.0f, -b_y/2.0f, b_z/2.0f);

                surf[0][0] = b_x/2.0f;
                surf[0][1] = b_y/2.0f;
                surf[0][2] = b_z/2.0f;
                
                surf[1][0] = b_x/2.0f;
                surf[1][1] = b_y/2.0f;
                surf[1][2] = -b_z/2.0f;
                
                surf[2][0] = b_x/2.0f;
                surf[2][1] = -b_y/2.0f;
                surf[2][2] = -b_z/2.0f;
                
                surf[3][0] = b_x/2.0f;
                surf[3][1] = -b_y/2.0f;
                surf[3][2] = b_z/2.0f;
                
                ns = new_surface(surf);
                nsc = malloc(sizeof(surfaces));
                nsc->s = ns;
                nsc->nxt = mys;
                mys = nsc;

		glVertex3f(-b_x/2.0f, b_y/2.0f, b_z/2.0f);
		glVertex3f(-b_x/2.0f, b_y/2.0f, -b_z/2.0f);
		glVertex3f(-b_x/2.0f, -b_y/2.0f, -b_z/2.0f);
		glVertex3f(-b_x/2.0f, -b_y/2.0f, b_z/2.0f);
                
                surf[0][0] = -b_x/2.0f;
                surf[0][1] = b_y/2.0f;
                surf[0][2] = b_z/2.0f;
                
                surf[1][0] = -b_x/2.0f;
                surf[1][1] = b_y/2.0f;
                surf[1][2] = -b_z/2.0f;
                
                surf[2][0] = -b_x/2.0f;
                surf[2][1] = -b_y/2.0f;
                surf[2][2] = -b_z/2.0f;
                
                surf[3][0] = -b_x/2.0f;
                surf[3][1] = -b_y/2.0f;
                surf[3][2] = b_z/2.0f;
                
                ns = new_surface(surf);
                nsc = malloc(sizeof(surfaces));
                nsc->s = ns;
                nsc->nxt = mys;
                mys = nsc;
                
	glEnd();
	glEndList();

        target->s = mys;

	printf("---\n\n");
	return (target);
}

void spawn_car_debug(car *target, dReal x, dReal y, dReal z)
{
	printf("-- spawning car\n");
	dMass m;

	printf("Warning: wheels will not collide to other wheels... (wheels use cylinders)\n");
	printf("(note to self: only solution would be moving to capped cylinders... :-/ )\n\n");
	//create object to store components and joints
	printf("(manually setting up jointgroup and space for object)\n");
	target->object = allocate_object();
	target->object->space_id = dSimpleSpaceCreate(space); //inside normal space
	target->object->joint_group = dJointGroupCreate(0); //always 0



	component *body=allocate_component(NULL, target->object);
        
        // copy over shadow surfaces
        body->s = target->s;
        
	//body:
	printf("\nTODO: add transformed geoms to body (for detecting side flipping)\n\n");
	//(physics)
	body->geom_id = dCreateBox(target->object->space_id, target->b_x, target->b_y, target->b_z);
	dGeomSetData (body->geom_id, (void*)(component*)(body));

	target->body_geom = body->geom_id; //for camera focus

	body->body_id = dBodyCreate (world);

	dMassSetBox (&m,1,target->b_x, target->b_y, target->b_z); //sides
	dMassAdjust (&m,target->body_mass); //mass
	dBodySetMass (body->body_id, &m);
	dBodySetPosition (body->body_id, x, y, z);

	dGeomSetBody (body->geom_id, body->body_id);
	//(graphics)
	body->graphics_list = target->body_graphics;

	//wheels:
	component *wheel[4];
	int i;
	for (i=0;i<4;++i)
	{
		//allocate
		wheel[i] = allocate_component(NULL, target->object);

		//friction
		wheel[i]->mu = target->wheel_mu;
		wheel[i]->slip1 = target->wheel_slip;
		wheel[i]->slip2 = target->wheel_slip;

		//hardness
		wheel[i]->erp = target->wheel_erp;
		wheel[i]->cfm = target->wheel_cfm;

		//create cylinder
		//(geom)
		wheel[i]->geom_id = dCreateCylinder (target->object->space_id, target->w_r, target->w_w);
		dGeomSetData (wheel[i]->geom_id, (void*)(component*)(wheel[i]));

		//(body)
		wheel[i]->body_id = dBodyCreate (world);

		dMassSetCylinder (&m, 1, 3, target->w_r, target->w_w);//3=z axis of cylinder
		dMassAdjust (&m, target->wheel_mass);
		dBodySetMass (wheel[i]->body_id, &m);

		dGeomSetBody (wheel[i]->geom_id, wheel[i]->body_id);


		//graphics
		wheel[i]->graphics_list = target->wheel_graphics;

		//to reduce errors on high rotaions, use finite rotation
		//(we need easy access to body ids)
		target->wheel_body[i] = wheel[i]->body_id;
		//enable finite rotaion mode
		dBodySetFiniteRotationMode (wheel[i]->body_id, 1);
	}

	//place and rotate wheels
	dMatrix3 rot;
	dRFromAxisAndAngle (rot, 0, 1, 0, M_PI/2);
	dBodySetPosition (wheel[0]->body_id, x+target->w_x, y+target->w_y, z);
	dBodySetRotation (wheel[0]->body_id, rot);
	dBodySetPosition (wheel[1]->body_id, x+target->w_x, y-target->w_y, z);
	dBodySetRotation (wheel[1]->body_id, rot);

	dRFromAxisAndAngle (rot, 0, 1, 0, -M_PI/2);
	dBodySetPosition (wheel[2]->body_id, x-target->w_x, y-target->w_y, z);
	dBodySetRotation (wheel[2]->body_id, rot);
	dBodySetPosition (wheel[3]->body_id, x-target->w_x, y+target->w_y, z);
	dBodySetRotation (wheel[3]->body_id, rot);

	//create joints (hinge2) for wheels
	printf("\n(note: placing wheel hinge anchor _inside_ wheel - as in original rollcage)\n");
	for (i=0; i<4; ++i)
	{
		target->joint[i]=dJointCreateHinge2 (world, target->object->joint_group);
		dJointAttach (target->joint[i], body->body_id, wheel[i]->body_id);
		//place anchor inside wheel
		const dReal *pos = dBodyGetPosition (wheel[i]->body_id);
		dJointSetHinge2Anchor (target->joint[i],pos[0],pos[1],pos[2]);
		dJointSetHinge2Axis1 (target->joint[i],0,0,1);
		dJointSetHinge2Axis2 (target->joint[i],1,0,0);

		//setup suspension
		dJointSetHinge2Param (target->joint[i],dParamSuspensionERP,target->suspension_erp);
		dJointSetHinge2Param (target->joint[i],dParamSuspensionCFM,target->suspension_cfm);

		//lock steering axis on all wheels
		dJointSetHinge2Param (target->joint[i],dParamLoStop,0);
		dJointSetHinge2Param (target->joint[i],dParamHiStop,0);
	}

	printf("--\n\n");
}


//functions for "unspawning"/removing objects (and cars)
//not used at the moment, might need some tweaking...

//removes an object
void remove_object(object *target)
{
	//lets just hope the given pointer is ok...
	printf("> remove object\n");

	if (target->space_id)
		dSpaceDestroy (target->space_id);

	if (target->joint_group)
		dJointGroupDestroy (target->joint_group);

	if (target->cmp_count != 0)
	{
		printf("(got components, search&remove)\n");
		component *cmp = component_head;
		//loop through all components, see if any match
		while (cmp)
		{
			//only free, if geoms, they are destroyed when destroying space (above)
			if (cmp->object_parent == target)
				free_component (cmp);
			cmp=cmp->next;
		}
	}

	free_object (target);
}

void remove_car (car* target)
{
	printf("removing car\n");
	remove_object (target->object);
	free_car (target);
}
