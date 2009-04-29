//Shared data structs and typedefs.
//
//See licensing info in main.c



//use a "runlevel" (enum) variable to make all threads/loops aware of status
//(locked is used when we want to pause the loops, but still try to catch
//up with real time when unlocked, basically: stop simulation, but do not
//reset "simulated time" variables... Use it when building objects)
enum {running, done, paused, locked, error} runlevel;

// editing flag - should most likely be absorbed into the enum above.
int editing = 0;


//the following a some basic color definitions (used for lights and materials)
GLfloat black[]     = {0.0f, 0.0f, 0.0f, 1.0f}; // = nothing for lights
GLfloat dgray[]     = {0.2f, 0.2f, 0.2f, 1.0f};
GLfloat gray[]      = {0.5f, 0.5f, 0.5f, 1.0f};
GLfloat lgray[]     = {0.8f, 0.8f, 0.8f, 1.0f};
GLfloat white[]     = {1.0f, 1.0f, 1.0f, 1.0f};
GLfloat red[]       = {1.0f, 0.0f, 0.0f, 1.0f};
GLfloat green[]     = {0.0f, 1.0f, 0.0f, 1.0f};
GLfloat lgreen[]    = {0.4f, 1.0f, 0.4f, 1.0f};
GLfloat blue[]      = {0.0f, 0.0f, 1.0f, 1.0f};
GLfloat lblue[]     = {0.6f, 0.6f, 1.0f, 1.0f};
GLfloat yellow[]    = {1.0f, 1.0f, 0.0f, 1.0f};


//to make the conf loader able to find variable names in structs, use indexes
typedef const struct {
	char *name;
	char type; //f for float, b for bool, i for int, 0 for end of list
	int length; //normaly 1 (or else more)
	size_t offset;
} data_index[];

//important system configuration variables
struct internal_struct {
	int verbosity;
	bool multithread; //TODO

	//physics
	dReal stepsize;
	int iterations;
	int contact_points;
	bool finite_rotation;
	dReal scale; //TODO
	dReal mu,erp,cfm;

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
} internal;

data_index internal_index = {
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


//file_3d: when a 3d file is loaded, we need a way to keep track of all
//rendering lists, so as to prevent memory leaks when unloading data
//typedef struct graphics_list_struct {
typedef struct file_3d_struct {
//	GLuint render_list;
	GLuint list;
	char *file; //filename (to prevent duplicated 3d loading)
	struct file_3d_struct *next;
} file_3d;

typedef struct trimesh_struct {
	dGeomID meshid;
	dTriMeshDataID dataid;
	
	dReal *ode_verts;
	unsigned int *ode_indices;
	struct geom_data *data;
	int v_count;
	int i_count;
} trimesh_struct;

typedef struct turd_struct {
	float x,y,z;
	float a,b,c;
	float nx,ny,nz;
	float anx,any,anz;
	float wx,wy,wz;
	float relx,rely,relz;
	float rerx,rery,rerz;
	float m00,m01,m02, m10,m11,m12, m20,m21,m22;
	GLfloat m[16];
	struct turd_struct *l;
	struct turd_struct *r;
	struct turd_struct *pre;
	struct turd_struct *nxt;
	
	// should be elsewhere
	struct trimesh_struct *tri;
} turd_struct;

turd_struct *turd_head = NULL;
turd_struct *edit_t = NULL;
turd_struct *edit_h = NULL;
turd_struct edit_b;
int edit_m = 1;

typedef struct interp_stuct {
		float ps0x,ps0y,ps0z;
		float ps1x,ps1y,ps1z;
		float pe0x,pe0y,pe0z;
		float pe1x,pe1y,pe1z;

		float scx,scy,scz;	// closest line start
		float tcx,tcy,tcz;  // closest line term

		float cpx,cpy,cpz;  // moving pivot
		float spx,spy,spz;  
		float epx,epy,epz;
		float ipx,ipy,ipz;

} interp_struct;

//graphics_list *graphics_list_head = NULL;
file_3d *file_3d_head = NULL;


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
	file_3d *graphics_debug1;
	file_3d *graphics_debug2;
	file_3d *graphics_debug3;

	//temporary solution
	bool box;
	bool flipper;
	bool NH4;
	bool building;

	struct script_struct *next;
} script;

script *script_head = NULL;

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
} object;

object *object_head = NULL;


//geom_data: data for geometrical shape (for collision detection), for: 
//contactpoint generation (friction and saftness/hardness). also contains
//rendering data for geom
//
//(contains boolean variable indicating collision - for triggering event script)
//
//>Dynamically allocated
typedef struct geom_data_struct {
	//keep track of the "owning" object
	object * object_parent;
	//geom data bellongs to
	dGeomID geom_id;

	file_3d *file_3d; //points to 3d list, or NULL if invisible

	//Physics data:
	//placeholder for more physics data
	bool use_slip;//true if object needs fds slip friction
	dReal mu, erp, cfm, slip, bounce;
	//(setting mu to dInfinity and using slip{1,2} is great for wheels)
	//End of physics data
	
	bool collide; //create physical collision when touching other components

	bool event; //set after each collision
	script *script; //script to execute when colliding (NULL if not used)

	//debug variables
	dGeomID flipper_geom;
	int flipper_counter;

	//used to find next/prev link in dynamically allocated chain
	//set next to null in last link in chain (prev = NULL in first)
	struct geom_data_struct *prev;
	struct geom_data_struct *next;
} geom_data;

geom_data *geom_data_head = NULL; //points at the first component in chain

//body_data: data for body (describes mass and mass positioning), used for:
//currently only for triggering event script (force threshold and event variables)
//as well as simple air/liquid drag simulations
//
//>Dynamically allocated
typedef struct body_data_struct {
	//keep track of the "owning" object
	object *object_parent;
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
	script *script; //execute on event

	//used to find next/prev link in dynamically allocated chain
	//set next to null in last link in chain (prev = NULL in first)
	struct body_data_struct *prev;
	struct body_data_struct *next;
} body_data;

body_data *body_data_head = NULL;


//joint_data: data for joint (connects bodies), is used for:
//currently only for triggering event script (force threshold)
//
//>Dynamically allocated
typedef struct joint_data_struct {
	//keep track of the "owning" object
	object * object_parent;
	//geom data bellongs to
	dJointID joint_id;

	dJointFeedback feedback; //used if checking forces
	dReal threshold; //if force on body exceeds threshold, eat buffer
	dReal buffer; //if buffer reaches zero, trigger event
	bool event; //if event triggered, run script
	script *script; //the script to run

	//used to find next/prev link in dynamically allocated chain
	//set next to null in last link in chain (prev = NULL in first)
	struct joint_data_struct *prev;
	struct joint_data_struct *next;
} joint_data;

joint_data *joint_data_head = NULL;


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

	file_3d *wheel_graphics; //add right/left wheels
	file_3d *box_graphics[CAR_MAX_BOXES];

	//just for keeping track
	object *object; //one object to store car components

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
} car;

car *car_head = NULL;

data_index car_index = {
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



#define UNUSED_KEY SDLK_QUESTION //key that's not used during race ("safe" default)

//profile: stores the user's settings (including key list)
typedef struct profile_struct {
	//the car the user is controlling
	car *car;
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

profile *profile_head;

data_index profile_index = {
	{"steer_speed",    'f' ,1 ,offsetof(struct profile_struct, steer_speed)},
	{"steer_max",      'f' ,1 ,offsetof(struct profile_struct, steer_max)},
	{"throttle_speed", 'f' ,1 ,offsetof(struct profile_struct, throttle_speed)},
	{"",0,0}}; //end

//list of all buttons
const struct {
	char *name;
	size_t offset;
} profile_key_list[] = {
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
	

//track: the main "world", contains simulation and rendering data for one
//large 3D file for the rigid environment, and more simulation data (like
//gravity) - crappy solution for now...
//Allocated at start
//(in contrary to the other structs, this is actually not allocated on runtime!)
struct track_struct {
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

	file_3d *file_3d;
	//NOTE/TODO: currently coded to store 5 planes (components) - only temporary!
	object *object;
} track;
//index:

data_index track_index = {
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


//TODO: weapons


// function prototypes
void doTurdTrack();
void makeTurd( struct turd_struct *tmp_turd, float x,float y,float z, float a,float b,float c );
