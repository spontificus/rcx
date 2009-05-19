//create and destroy objects, geoms and bodies
//(should only be used by other abstraction functions (look in loader.c)
//and not directly, except for free_all (to do quick full free)
//
//See main.c for licensing

#include "shared.h"
#include "main.h"


const struct data_index internal_index[] = {
	{"verbosity",		'i',1, offsetof(struct internal_struct, verbosity)},
	//TODO: MULTITHREAD
	{"stepsize",		'f',1, offsetof(struct internal_struct, stepsize)},
	{"iterations",		'i',1, offsetof(struct internal_struct, iterations)},
	{"contact_points",	'i',1, offsetof(struct internal_struct, contact_points)},
	{"finite_rotation",	'b',1, offsetof(struct internal_struct, finite_rotation)},
	//TODO: SCALE
	{"default_mu",		'f',1, offsetof(struct internal_struct, mu)},
	{"default_erp",		'f',1, offsetof(struct internal_struct, erp)},
	{"default_cfm",		'f',1, offsetof(struct internal_struct, cfm)},
	{"default_slip",		'f',1, offsetof(struct internal_struct, slip)},
	{"auto_disable_linear",	'f',1, offsetof(struct internal_struct, dis_linear)},
	{"auto_disable_angular",	'f',1, offsetof(struct internal_struct, dis_angular)},
	{"auto_disable_time",	'f',1, offsetof(struct internal_struct, dis_time)},
	{"auto_disable_steps",	'i',1, offsetof(struct internal_struct, dis_steps)},
	//graphics
	{"graphics_threshold",	'i',1, offsetof(struct internal_struct, threshold)},
	{"resolution",		'i',2, offsetof(struct internal_struct, res)},
	{"eye_distance",		'i',1, offsetof(struct internal_struct, dist)},
	{"force_angle",		'b',1, offsetof(struct internal_struct, force)},
	{"view_angle",		'f',1, offsetof(struct internal_struct, angle)},
	{"fullscreen",		'b',1, offsetof(struct internal_struct, fullscreen)},
	{"resize",		'b',1, offsetof(struct internal_struct, resize)},
	{"",0,0}};

const struct data_index car_index[] = {
	{"max_torque",		'f',1, offsetof(struct car_struct, max_torque)},
	{"motor_tweak",		'f',1, offsetof(struct car_struct, motor_tweak)},
	{"max_break",		'f',1, offsetof(struct car_struct, max_break)},
	{"body_mass",		'f',1, offsetof(struct car_struct, body_mass)},
	{"wheel_mass",		'f',1, offsetof(struct car_struct, wheel_mass)},
	{"suspension_erp",	'f',1, offsetof(struct car_struct, suspension_erp)},
	{"suspension_cfm",	'f',1, offsetof(struct car_struct, suspension_cfm)},
	{"wheel_mu",		'f',1, offsetof(struct car_struct, wheel_mu)},
	{"wheel_slip",		'f',1, offsetof(struct car_struct, wheel_slip)},
	{"wheel_erp",		'f',1, offsetof(struct car_struct, wheel_erp)},
	{"wheel_cfm",		'f',1, offsetof(struct car_struct, wheel_cfm)},
	{"wheel_bounce",		'f',1, offsetof(struct car_struct, wheel_bounce)},
	{"body_mu",		'f',1, offsetof(struct car_struct, body_mu)},
	{"body_slip",		'f',1, offsetof(struct car_struct, body_slip)},
	{"body_erp",		'f',1, offsetof(struct car_struct, body_erp)},
	{"body_cfm",		'f',1, offsetof(struct car_struct, body_cfm)},

	{"body_drag",		'f',3, offsetof(struct car_struct, body_drag)},
	{"body_rotation_drag",	'f',3, offsetof(struct car_struct, body_rotation_drag)},
	{"wheel_drag",		'f',3, offsetof(struct car_struct, wheel_drag)},
	{"wheel_rotation_drag",	'f',3, offsetof(struct car_struct, wheel_rotation_drag)},

	//body and geom (box) sizes:
	{"body",	'f',	3,	offsetof(struct car_struct, body[0])}, //not a geom
	//MUST BE THE SAME AMMOUNT AS CAR_MAX_BOXES
	{"box1",	'f',	6,	offsetof(struct car_struct, box[0][0])},
	{"box2",	'f',	6,	offsetof(struct car_struct, box[1][0])},
	{"box3",	'f',	6,	offsetof(struct car_struct, box[2][0])},
	{"box4",	'f',	6,	offsetof(struct car_struct, box[3][0])},
	{"box5",	'f',	6,	offsetof(struct car_struct, box[4][0])},
	{"box6",	'f',	6,	offsetof(struct car_struct, box[5][0])},
	{"box7",	'f',	6,	offsetof(struct car_struct, box[6][0])},
	{"box8",	'f',	6,	offsetof(struct car_struct, box[7][0])},
	{"box9",	'f',	6,	offsetof(struct car_struct, box[8][0])},
	{"box10",'f',	6,	offsetof(struct car_struct, box[9][0])},
	{"box11",'f',	6,	offsetof(struct car_struct, box[10][0])},
	{"box12",'f',	6,	offsetof(struct car_struct, box[11][0])},
	{"box13",'f',	6,	offsetof(struct car_struct, box[12][0])},
	{"box14",'f',	6,	offsetof(struct car_struct, box[13][0])},
	{"box15",'f',	6,	offsetof(struct car_struct, box[14][0])},
	{"box16",'f',	6,	offsetof(struct car_struct, box[15][0])},
	{"box17",'f',	6,	offsetof(struct car_struct, box[16][0])},
	{"box18",'f',	6,	offsetof(struct car_struct, box[17][0])},
	{"box19",'f',	6,	offsetof(struct car_struct, box[18][0])},
	{"box20",'f',	6,	offsetof(struct car_struct, box[19][0])},
	
	//the following is for sizes not yet determined
	{"s",	'f',	4,	offsetof(struct car_struct, s[0])}, //flipover
	{"w",	'f',	2,	offsetof(struct car_struct, w[0])}, //wheel
	{"wp",	'f',	2,	offsetof(struct car_struct, wp[0])}, //wheel pos
	{"jx",	'f',	1,	offsetof(struct car_struct, jx)}, //joint x position
	{"",0,0}};//end

const struct data_index profile_index[] = {
	{"steer_speed",    'f' ,1 ,offsetof(struct profile_struct, steer_speed)},
	{"steer_max",      'f' ,1 ,offsetof(struct profile_struct, steer_max)},
	{"throttle_speed", 'f' ,1 ,offsetof(struct profile_struct, throttle_speed)},
	{"",0,0}}; //end

const struct profile_key_struct profile_key_list[] = {
	{"up",			offsetof(struct profile_struct, up)},
	{"down",			offsetof(struct profile_struct, down)},
	{"right",		offsetof(struct profile_struct, right)},
	{"left",			offsetof(struct profile_struct, left)},
	{"soft_break",		offsetof(struct profile_struct, soft_break)},
	{"drift_break",		offsetof(struct profile_struct, drift_break)},

	{"camera_x+",		offsetof(struct profile_struct, cam_x_pos)},
	{"camera_x-",		offsetof(struct profile_struct, cam_x_neg)},
	{"camera_y+",		offsetof(struct profile_struct, cam_y_pos)},
	{"camera_y-",		offsetof(struct profile_struct, cam_y_neg)},
	{"camera_z+",		offsetof(struct profile_struct, cam_z_pos)},
	{"camera_z-",		offsetof(struct profile_struct, cam_z_neg)},
	{"",0}}; //end

const struct data_index track_index[] = {
	{"sky",		'f',3,	offsetof(struct track_struct, sky[0])},
	{"ambient",	'f',3,	offsetof(struct track_struct, ambient[0])},
	{"diffuse",	'f',3,	offsetof(struct track_struct, diffuse[0])},
	{"specular",	'f',3,	offsetof(struct track_struct, specular[0])},
	{"position",	'f',3,	offsetof(struct track_struct, position[0])},
	{"gravity",	'f',1,	offsetof(struct track_struct, gravity)},
	{"mu",		'f',1,	offsetof(struct track_struct, mu)},
	{"slip",		'f',1,	offsetof(struct track_struct, slip)},
	{"erp",		'f',1,	offsetof(struct track_struct, erp)},
	{"cfm",		'f',1,	offsetof(struct track_struct, cfm)},
	{"density",	'f',1,	offsetof(struct track_struct, density)},
	{"start",	'f',3,	offsetof(struct track_struct, start)},
	{"",0,0}};//end


//allocate new script storage, and add it to list
//(not used yet, only for storing 3d list pointers...)
script_struct *allocate_script(void)
{
	printlog(2, " > allocating script");
	script_struct *tmp = (script_struct *)malloc(sizeof(script_struct));
	tmp->next = script_head;
	script_head = tmp;

	if (!tmp->next)
		printlog(2, " (first registered script)");

	script_head->name = NULL;

	//debug identification bools set to false
	script_head->box = false;
	script_head->flipper = false;
	script_head->NH4 = false;
	script_head->building = false;
	script_head->sphere = false;


	printlog (2, "\n");
	return script_head;
}

//allocate a new object, add it to the list and returns its pointer
object_struct *allocate_object (bool adspace, bool adjointgroup)
{
	printlog(2, " > allocating object");
	object_struct *object_next = object_head;

	object_head = (object_struct *)malloc(sizeof(object_struct));

	object_head->prev = NULL;
	object_head->next = object_next;

	//set space and jointgroup to NULL?
	printlog(2, " (create space:");
	if (adspace)
	{
		printlog(2, "yes)");
		object_head->space = dHashSpaceCreate(space); //inside world
	}
	else
	{
		printlog(2, "no)");
		object_head->space = NULL;
	}

	printlog(2, " (create jointgroup:");
	if (adjointgroup)
	{
		printlog(2, "yes)");
		object_head->jointgroup = dJointGroupCreate(0); //always 0
	}
	else
	{
		printlog(2, "no)");
		object_head->jointgroup = NULL;
	}

	if (object_next)
		object_next->prev = object_head;
	else
		printlog(2, " (first registered object)");

	//default values
	object_head->geom_count = 0;//nothing bellongs to object (yet)
	object_head->body_count = 0;
//	object_head->joint_count = 0;


	object_head->collide_space = false; //objects components doesn't collide with each other

	printlog (2, "\n");
	return object_head;
}

//allocates a new geom data, returns its pointer (and uppdate its object's count),
//ads it to the component list, and ads the data to specified geom (assumed)
geom_data *allocate_geom_data (dGeomID geom, object_struct *obj)
{
	printlog(2, " > allocating geom_data");
	geom_data *tmp_geom = (geom_data *)malloc(sizeof(geom_data));

	//parent object
	tmp_geom->object_parent = obj;
	printlog(2, " (parent object:");
	if (obj)
	{
		printlog(2, "yes) (space:");
		obj->geom_count += 1;

		if (obj->space)
		{
			printlog(2, "yes)");
			dSpaceAdd (obj->space, geom);
		}
		else
		{
			printlog(2, "no)");
			dSpaceAdd (space, geom);
		}
	}
	else
	{
		printlog(2, "no)");
		dSpaceAdd (space, geom);
	}

	//add it to the list
	tmp_geom->next = geom_data_head;
	geom_data_head = tmp_geom;
	geom_data_head->prev = NULL;

	if (geom_data_head->next)
		geom_data_head->next->prev = geom_data_head;
	else
		printlog(2, " (first registered)");

	//add it to the geom
	dGeomSetData (geom, (void*)(geom_data*)(geom_data_head));
	geom_data_head->geom_id = geom;

	//now lets set some default values...
	//event processing (triggering):
	geom_data_head->event = false; //no collision event yet
	geom_data_head->script = NULL; //nothing to run on collision (yet)
	
	//collision contactpoint data
	geom_data_head->file_3d = NULL; //default, isn't rendered
	geom_data_head->mu = internal.mu;
	geom_data_head->erp = internal.erp;
	geom_data_head->cfm = internal.cfm;
	geom_data_head->slip = internal.slip; //no FDS slip
	geom_data_head->wheel = false; //not a wheel
	geom_data_head->bounce = 0.0; //no bouncyness

	geom_data_head->collide = true; //on collision, create opposing forces

	//debug variables
	geom_data_head->flipper_geom = 0;
	geom_data_head->flipper_counter = 0;

	printlog (2, "\n");
	return geom_data_head;
}

body_data *allocate_body_data (dBodyID body, object_struct *obj)
{
	printlog(2, " > allocating body_data");
	body_data *tmp_body = (body_data *)malloc(sizeof(body_data));

	//parent object
	tmp_body->object_parent = obj;
	printlog(2, " (parent object:");
	if (obj)
	{
		printlog(2, "yes)");
		obj->body_count += 1;

		printlog(2, "Note: no way of assigning body to object, check for body attachment in geoms!\n");
	}
	else
		printlog(2, "no)");

	//ad it to the list
	tmp_body->next = body_data_head;
	body_data_head = tmp_body;
	body_data_head->prev = NULL;

	if (body_data_head->next)
		body_data_head->next->prev = body_data_head;
	else
		printlog(2, " (first registered)");

	//add it to the body
	dBodySetData (body, (void*)(body_data*)(body_data_head));
	body_data_head->body_id = body;

	//default values
	body_data_head->use_drag = false;
	body_data_head->use_rotation_drag = false;

	body_data_head->threshold = 0; //no threshold (disables event testing)
	body_data_head->buffer = 1; //almost empty buffer
	body_data_head->event = false;
	body_data_head->script = NULL;

	printlog (2, "\n");
	return body_data_head;
}

joint_data *allocate_joint_data (dJointID joint, object_struct *obj, bool feedback)
{
	printlog(2, " > allocating joint_data");
	bool warn = false;
	joint_data *tmp_joint = (joint_data *)malloc(sizeof(joint_data));

	//parent object
	tmp_joint->object_parent = obj;
	printlog(2, " (parent object:");
	if (obj)
	{
		printlog(2, "yes)");
//		obj->joint_count += 1;

		warn = true;
	}
	else
		printlog(2, "no)");

	//add it to the list
	tmp_joint->next = joint_data_head;
	joint_data_head = tmp_joint;
	joint_data_head->prev = NULL;

	if (joint_data_head->next)
		joint_data_head->next->prev = joint_data_head;
	else
		printlog(2, " (first registered)");

	//add it to the joint
	dJointSetData (joint, (void*)(joint_data*)(joint_data_head));
	joint_data_head->joint_id = joint;

	//default values (currently only event triggering)
	joint_data_head->threshold = 0; //no threshold (disables event testing)
	joint_data_head->buffer = 1; //almost empty
	joint_data_head->event = false;
	joint_data_head->script = NULL;

	if (feedback)
	{
		printlog(2, " (with feedback)");
		dJointSetFeedback (joint, &(joint_data_head->feedback));
	}

	printlog (2, "\n");

	if (warn)
		printlog(2, "Warning: can't change jointgroup (specify when creating!)\n");
	return joint_data_head;
}


profile *allocate_profile(void)
{
	printlog(2, " > allocating profile");
	profile *profile_next = profile_head;
	profile_head = (profile *)malloc(sizeof(profile));

	//add to list
	profile_head->prev=NULL;
	profile_head->next=profile_next;
	if (profile_next)
		profile_next->prev=profile_head;
	else
		printlog(2, " (first registered)");
	printlog(2, "\n");

	//default values
	profile_head->steer_speed = 1;
	profile_head->steer_max = 1;
	profile_head->throttle_speed = 1;

	//map all input keys to questionmark
	profile_head->up = UNUSED_KEY;
	profile_head->down = UNUSED_KEY;
	profile_head->right = UNUSED_KEY;
	profile_head->left = UNUSED_KEY;
	profile_head->soft_break = UNUSED_KEY;
	profile_head->drift_break = UNUSED_KEY;

	profile_head->cam_x_pos = UNUSED_KEY;
	profile_head->cam_x_neg = UNUSED_KEY;
	profile_head->cam_y_pos = UNUSED_KEY;
	profile_head->cam_y_neg = UNUSED_KEY;
	profile_head->cam_z_pos = UNUSED_KEY;
	profile_head->cam_z_neg = UNUSED_KEY;

	return profile_head;
}

//free profile
void free_profile (profile *target)
{
	printlog(2, " > freeing profile\n");

	//remove from list
	if (!target->prev) //head
		profile_head = target->next;
	else //not head
		target->prev->next = target->next;

	if (target->next) //not last
		target->next->prev = target->prev;

	//remove profile
	free (target);
}


//allocates car, ad to list...
//(no object allocations - since we want to allocate car without spawning)
car_struct *allocate_car(void)
{
	printlog(2, " > allocating car");
	car_struct *car_next = car_head;
	car_head = (car_struct *)malloc(sizeof(car_struct));

	//add to list
	car_head->prev=NULL;
	car_head->next=car_next;
	if (car_next)
		car_next->prev=car_head;
	else
		printlog(2, " (first registered)");


	//default values
	printlog(2, "\nFIXME: set more values to default");

	car_head->name = NULL;
	car_head->spawned = false;

	int i;
	for (i=0;i<CAR_MAX_BOXES;++i)
		car_head->box_graphics[i] = NULL;
	car_head->wheel_graphics = NULL;

	car_head->drift_breaks = true; //if the user does nothing, lock wheels
	car_head->breaks = false;
	car_head->throttle = 0;
	car_head->steering = 0;

	//default mass, friction, different stats... just to prevent segfaults
	car_head->max_torque  = 8000;
	car_head->max_break   = 1000;

	car_head->body_mass   = 500;

	car_head->suspension_erp = 0.02;
	car_head->suspension_cfm = 0.0003;

	car_head->wheel_mass  = 200;
	car_head->wheel_mu    = dInfinity;
	car_head->wheel_slip  = 0.0005;
	car_head->wheel_erp   = 0.8;
	car_head->wheel_cfm   = 0.0;
	
	//set x in all boxes to 0 (disable)
	for (i=0;i<CAR_MAX_BOXES;++i)
		car_head->box[i][0]=0;

	//needed for keeping track for different things (mostly for graphics)
	car_head->bodyid        = NULL;
	car_head->wheel_body[0] = NULL;
	car_head->wheel_body[1] = NULL;
	car_head->wheel_body[2] = NULL;
	car_head->wheel_body[3] = NULL;
	car_head->joint[0] = NULL;
	car_head->joint[1] = NULL;
	car_head->joint[2] = NULL;
	car_head->joint[3] = NULL;

	car_head->object = NULL;

	car_head->w[0] = 4; //r
	car_head->w[1] = 2; //w
	car_head->wp[0] = 3; //x
	car_head->wp[1] = 4; //y

	car_head->jx = 3; //wheel offset

	printlog(2, "\n");
	return car_head;
}

//allocates new link in 3d rendering list
file_3d_struct *allocate_file_3d (void)
{
	printlog(2, " > allocating 3d file storage");

	file_3d_struct *tmp_3d = (file_3d_struct *)malloc (sizeof(file_3d_struct));
	//add to list
	tmp_3d->next = file_3d_head;
	file_3d_head = tmp_3d;

	if (!tmp_3d->next)
		printlog(2, " (first one)\n");
	else
		printlog(2, "\n");

	//default values
	file_3d_head->file = NULL; //filename - here ~no name~
	printlog(2, "TODO: check for already loaded files\n");
	file_3d_head->list = glGenLists(1);

	printlog (2, "\n");
	return file_3d_head;
}

//destroys an object
void free_object(object_struct *target)
{
	//lets just hope the given pointer is ok...
	printlog(2, " > freeing object");

	//1: remove it from the list
	if (target->prev == NULL) //first link
	{
		printlog(2, " (object is head)");
		object_head = target->next;
	}
	else
		target->prev->next = target->next;

	if (target->next) //not last link
		target->next->prev = target->prev;
	else
		printlog(2, " (object is last)");


	printlog (2, "\n");

	//check if has jointgroup
	if (target->jointgroup)
	{
		printlog(2, "FIXME: free_object - detected jointgroup, destroying\n\n");
		dJointGroupDestroy (target->jointgroup);
	}

	//2: remove it from memory

	free(target);
}


//destroys a geom, and removes it from the list
void free_geom_data(geom_data *target)
{
	//lets just hope the given pointer is ok...
	printlog(2, " > freeing geom");

	//1: remove it from the list
	if (!target->prev) //head in list, change head pointer
	{
		printlog(2, " (geom is head)");
		geom_data_head = target->next;
	}
	else //not head in list, got a previous link to update
		target->prev->next = target->next;

	if (target->next) //not last link in list
		target->next->prev = target->prev;
	else
		printlog(2, " (geom is last)");

	//2: remove it from memory

	free(target);

	printlog (2, "\n");
}

//destroys a body, and removes it from the list
void free_body_data (body_data *target)
{
	//lets just hope the given pointer is ok...
	printlog(2, " > freeing body");

	//1: remove it from the list
	if (!target->prev) //head in list, change head pointer
	{
		printlog(2, " (body is head)");
		body_data_head = target->next;
	}
	else //not head in list, got a previous link to update
		target->prev->next = target->next;

	if (target->next) //not last link in list
		target->next->prev = target->prev;
	else
		printlog(2, " (body is last)");

	//2: remove it from memory

	free(target);

	printlog (2, "\n");
}

//destroys a joint, and removes it from the list
void free_joint_data (joint_data_struct *target)
{
	//lets just hope the given pointer is ok...
	printlog(2, " > freeing joint");

	//1: remove it from the list
	if (!target->prev) //head in list, change head pointer
	{
		printlog(2, " (is head)");
		joint_data_head = target->next;
	}
	else //not head in list, got a previous link to update
		target->prev->next = target->next;

	if (target->next) //not last link in list
		target->next->prev = target->prev;
	else
		printlog(2, " (is last)");

	//2: remove it from memory

	free(target);

	printlog (2, "\n");
}


//run _before_ starting full erase of object/component lists (at race end)
void free_car (car_struct *target)
{
	printlog(2, " > freeing car\n");

	//remove from list
	if (!target->prev) //head
		car_head = target->next;
	else //not head
		target->prev->next = target->next;

	if (target->next) //not last
		target->next->prev = target->prev;

	//remove car
	free_object (target->object);


	free (target->name);
	free (target);
}

//free all things allocated (to simplify memory leak preventing)
//simple dirty way of removing everything
void free_all (void)
{
	printlog(1, "-> Freeing all data\n");
	printlog(2, ">>> TODO: optimize???\n");
	//first thing to destroy: cars
	printlog(2, ">>> TODO: change from free_car to destroy_car?\n\n");
	while (car_head)
		free_car(car_head);

	while (profile_head)
		free_profile(profile_head);

	//take care of object (without calling ode - not needed)
	while (geom_data_head)
		free_geom_data(geom_data_head);
	while (body_data_head)
		free_body_data(body_data_head);
	while (joint_data_head)
		free_joint_data(joint_data_head);
       	
	while (object_head)
		free_object(object_head);

	//only place where scripts and 3d lists are removed
	script_struct *script_tmp = script_head;
	while (script_head)
	{
		script_tmp = script_head->next;
		free (script_head->name);
		free (script_head);
		script_head = script_tmp;
	}

	//destroy loaded 3d files
	file_3d_struct *file_3d_tmp;
	while (file_3d_head)
	{
		file_3d_tmp = file_3d_head; //copy from from list
		file_3d_head = file_3d_head->next; //remove from list

		glDeleteLists (file_3d_tmp->list, 1);
		free (file_3d_tmp);
	}

	//no need to destroy track, since it's not allocated by program
}

void print_matrix( float *m ) {
	printf("%f %f %f %f\n", m[0], m[4], m[8], m[12]);
	printf("%f %f %f %f\n", m[1], m[5], m[9], m[13]);
	printf("%f %f %f %f\n", m[2], m[6], m[10], m[14]);
	printf("%f %f %f %f\n", m[3], m[7], m[11], m[15]);
}


// inner product of two vectors
float dot(dVector3 u, dVector3 v) {
	return u[0] * v[0] + u[1] * v[1] + u[2] * v[2];
}

void normalise(float *n) {
	float d = sqrt( pow(n[0],2) + pow(n[1],2) + pow(n[2],2) );
	
	if ( d == 0 ) {
		return;
	}

	n[0] /= d;
	n[1] /= d;
	n[2] /= d;
}

