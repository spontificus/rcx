#ifndef __SHARED_H__
#define __SHARED_H__

#include <SDL.h>
#include <SDL_opengl.h>
#include <ode/ode.h>
#include <stdbool.h> 

//Shared data structs and typedefs.
//
//See licensing info in main.c






//use a "runlevel" (enum) variable to make all threads/loops aware of status
//(locked is used when we want to pause the loops, but still try to catch
//up with real time when unlocked, basically: stop simulation, but do not
//reset "simulated time" variables... Use it when building objects)
extern enum runenum {running, done, paused, locked, error} runlevel;

// editing flag - should most likely be absorbed into the enum above.
extern int editing;


// globals
extern dWorldID world;
extern dSpaceID space;
extern dJointGroupID contactgroup;//TODO: move to shared.h data? good for event thread?


extern GLuint tex_ch;

//the following a some basic color definitions (used for lights and materials)
extern GLfloat black[4]; // = nothing for lights
extern GLfloat dgray[4];
extern GLfloat gray[4];
extern GLfloat lgray[4];
extern GLfloat white[4];
extern GLfloat red[4];
extern GLfloat green[4];
extern GLfloat lgreen[4];
extern GLfloat blue[4];
extern GLfloat lblue[4];
extern GLfloat yellow[4];


//to make the conf loader able to find variable names in structs, use indexes
extern struct data_index {
	const char *name;
	char type; //f for float, b for bool, i for int, 0 for end of list
	int length; //normaly 1 (or else more)
	size_t offset;
} data_index;

//important system configuration variables
extern struct internal_struct {
	int verbosity;
	bool multithread; //TODO

	//physics
	dReal stepsize;
	int iterations;
	int contact_points;
	bool finite_rotation;
	dReal scale; //TODO
	dReal mu,erp,cfm,slip;

	dReal dis_linear, dis_angular, dis_time;
	int dis_steps;


	//graphics
	int threshold;
	int res[2]; //resolution
	int dist;
	bool force;
	float angle;
	bool fullscreen;
	bool resize;
} internal_struct;

const extern struct data_index internal_index[];


//file_3d_struct: when a 3d file is loaded, we need a way to keep track of all
//rendering lists, so as to prevent memory leaks when unloading data
//typedef struct graphics_list_struct {
typedef struct file_3d_struct {
//	GLuint render_list;
	GLuint list;
	char *file; //filename (to prevent duplicated 3d loading)
	struct file_3d_struct *next;
} file_3d_struct;


//graphics_list *graphics_list_head = NULL;
extern file_3d_struct *file_3d_head;


//script: human readable (read: not _programming_) langue which will
//describe what should be done when spawning an object (components, joints...),
//and when an component is colliding ("sensor triggering", destroying and so on)
//function arguments can point at 3d files and other scripts and so on...
//
//(currently not used)
//
//>Allocated at start
typedef struct script_struct {
	char *name; //usefull if to see if the same object is requested more times
	//placeholder for script data, now just a single variable (what to render)
	file_3d_struct *graphics_debug1;
	file_3d_struct *graphics_debug2;
	file_3d_struct *graphics_debug3;

	//temporary solution
	bool box;
	bool flipper;
	bool NH4;
	bool building;
	bool sphere;

	struct script_struct *next;
} script_struct;

extern script_struct *script_head;

//object: one "thing" on the track, from a complex building to a tree, spawning
//will be controlled by a custom scripting langue in future versions, the most
//important role of "object" is to store the ode space for the spawned object
//(keeps track of the geoms in ode that describes the components) and joint
//group (for cleaning up object)
//
//>Dynamically allocated
typedef struct object_struct {
	//things to keep track of when cleaning out object
	//all geoms can be queried from space, all bodies from geoms and all
	//joints from bodies
	dJointGroupID jointgroup; //store all joints (if needed)
	dSpaceID space; //store all geoms
	bool collide_space; //if the internal geoms should collide with themselves

	//keep track of owned things (to get fast way of auto-removing empty objs)
	unsigned int geom_count;
	unsigned int body_count;
//	unsigned int joint_count;
	//placeholder for more data
	
	//used to find next/prev object in dynamically allocated chain
	//set next to null in last object in chain
	struct object_struct *prev;
	struct object_struct *next;
} object_struct;

extern object_struct *object_head;


//geom_data: data for geometrical shape (for collision detection), for: 
//contactpoint generation (friction and saftness/hardness). also contains
//rendering data for geom
//
//(contains boolean variable indicating collision - for triggering event script)
//
//>Dynamically allocated
typedef struct geom_data_struct {
	//keep track of the "owning" object
	object_struct * object_parent;
	//geom data bellongs to
	dGeomID geom_id;

	file_3d_struct *file_3d; //points to 3d list, or NULL if invisible

	//Physics data:
	//placeholder for more physics data
	dReal mu, erp, cfm, slip, bounce;

	bool wheel; //true if wheel side slip and connected to hinge2
	dJointID hinge2;

	//End of physics data
	
	bool collide; //create physical collision when touching other components

	bool event; //set after each collision
	script_struct *script; //script to execute when colliding (NULL if not used)

	//debug variables
	dGeomID flipper_geom;
	int flipper_counter;

	//used to find next/prev link in dynamically allocated chain
	//set next to null in last link in chain (prev = NULL in first)
	struct geom_data_struct *prev;
	struct geom_data_struct *next;
} geom_data;

extern geom_data *geom_data_head; //points at the first component in chain

//body_data: data for body (describes mass and mass positioning), used for:
//currently only for triggering event script (force threshold and event variables)
//as well as simple air/liquid drag simulations
//
//>Dynamically allocated
typedef struct body_data_struct {
	//keep track of the "owning" object
	object_struct *object_parent;
	//geom data bellongs to
	dBodyID body_id;

	//data for drag (air+water friction)
	bool use_drag;
	bool use_rotation_drag;
	dReal drag[3];
	dReal rot_drag[3];

	dReal threshold; //if allocated forces exceeds, eat buffer
	dReal buffer; //if buffer reaches zero, trigger event
	bool event; //set after each buffer empty
	script_struct *script; //execute on event

	//used to find next/prev link in dynamically allocated chain
	//set next to null in last link in chain (prev = NULL in first)
	struct body_data_struct *prev;
	struct body_data_struct *next;
} body_data;

extern body_data *body_data_head;


//joint_data: data for joint (connects bodies), is used for:
//currently only for triggering event script (force threshold)
//
//>Dynamically allocated
typedef struct joint_data_struct {
	//keep track of the "owning" object
	object_struct * object_parent;
	//geom data bellongs to
	dJointID joint_id;

	dJointFeedback feedback; //used if checking forces
	dReal threshold; //if force on body exceeds threshold, eat buffer
	dReal buffer; //if buffer reaches zero, trigger event
	bool event; //if event triggered, run script
	script_struct *script; //the script to run

	//used to find next/prev link in dynamically allocated chain
	//set next to null in last link in chain (prev = NULL in first)
	struct joint_data_struct *prev;
	struct joint_data_struct *next;
} joint_data;

extern joint_data *joint_data_head;


//car: pointer to object and extra data, adjusted for controlled cars. No
//scripting - used to keep track of components and objects (like weapons)
//bellonging to the player during the race
//Allocated at start

#define CAR_MAX_BOXES 20

typedef struct car_struct {
	//data loaded from file (to be implemented)
	//(max_break is for non-locking breaks, not drifting break (they are infinite))
	char *name;
	bool spawned; //don't assume loaded cars are participating in race


	dReal max_torque, motor_tweak, max_break;
	dReal body_mass, wheel_mass;
	dReal suspension_erp, suspension_cfm;
	dReal wheel_mu, wheel_slip, wheel_erp, wheel_cfm, wheel_bounce;
	dReal body_mu, body_slip, body_erp, body_cfm;

	dReal body_drag[3], body_rotation_drag[3], wheel_drag[3], wheel_rotation_drag[3];

	file_3d_struct *wheel_graphics; //add right/left wheels
	file_3d_struct *box_graphics[CAR_MAX_BOXES];

	//just for keeping track
	object_struct *object; //one object to store car components

//	dGeomID body_geom; //for focusing
	dBodyID bodyid,wheel_body[4]; //for "Finite Rotation" arror reduction
	dJointID joint[4]; //for applying forces on wheels

	//flipover sensors
	geom_data *sensor1, *sensor2;
	dReal dir; //direction, 1 or -1

	//controlling values
	bool drift_breaks, breaks;
	dReal throttle, steering; //-1.0 to +1.0

	dReal body[3];
	dReal box[CAR_MAX_BOXES][6];
	//debug sizes
	dReal s[4],w[2],wp[2],jx;

	struct car_struct *next;
	struct car_struct *prev;
} car_struct;

extern car_struct *car_head;

const extern struct data_index car_index[];

#define UNUSED_KEY SDLK_QUESTION //key that's not used during race ("safe" default)

//profile: stores the user's settings (including key list)
typedef struct profile_struct {
	//the car the user is controlling
	car_struct *car;
	struct profile_struct *next;
	struct profile_struct *prev;

	//settings (loaded from conf)
	dReal steer_speed;
	dReal steer_max;
	dReal throttle_speed;

	//keys (loaded from keys.lst)
	SDLKey up;
	SDLKey down;
	SDLKey right;
	SDLKey left;
	SDLKey soft_break;
	SDLKey drift_break;

	SDLKey cam_x_pos;
	SDLKey cam_x_neg;
	SDLKey cam_y_pos;
	SDLKey cam_y_neg;
	SDLKey cam_z_pos;
	SDLKey cam_z_neg;
} profile;

extern profile *profile_head;

const extern struct data_index profile_index[];

//list of all buttons
typedef struct profile_key_struct {
	const char *name;
	size_t offset;
} profile_key_struct;

const extern struct profile_key_struct profile_key_list[];
	

//track: the main "world", contains simulation and rendering data for one
//large 3D file for the rigid environment, and more simulation data (like
//gravity) - crappy solution for now...
//Allocated at start
//(in contrary to the other structs, this is actually not allocated on runtime!)
extern struct track_struct {
	//placeholder for stuff like if it's raining/snowing and lightsources
	GLfloat sky[3]; //RGB, alpha is always 1.0f

	GLfloat ambient[4];
	GLfloat diffuse[4];
	GLfloat specular[4];
	GLfloat position[4]; //light position
	
	dReal gravity;
	dReal mu; //friction (normal)
	dReal slip; //for wheel friction
	dReal erp;
	dReal cfm;

	dReal density; //for air drag (friction)

	dReal start[3];

	file_3d_struct *file_3d;
	//NOTE/TODO: currently coded to store 5 planes (components) - only temporary!
	object_struct *object;
} track_struct;
//index:

const extern struct data_index track_index[];


//TODO: weapons


/* shared.c */
void free_all(void);
void print_matrix(float *m);
void free_joint_data (joint_data *target);
profile *allocate_profile(void);
script_struct *allocate_script(void);
file_3d_struct *allocate_file_3d (void);
object_struct *allocate_object (bool adspace, bool adjointgroup);
joint_data *allocate_joint_data (dJointID joint, object_struct *obj, bool feedback);
geom_data *allocate_geom_data (dGeomID geom, object_struct *obj);
car_struct *allocate_car(void);
body_data *allocate_body_data (dBodyID body, object_struct *obj);
void free_object(object_struct *target);
void free_car (car_struct *target);
float dot(dVector3 u, dVector3 v);
void normalise(float *n);
float *mbv(float *m, float x, float y, float z);

#endif
