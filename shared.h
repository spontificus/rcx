//Shared data structs and typedefs.

//use a "runlevel" (enum) variable to make all threads/loops aware of status
//(locked is used when we want to pause the loops, but still try to catch
//up with real time when unlocked, basically: stop simulation, but do not
//reset "simulated time" variables... Use it when building objects)
enum {running, done, paused, locked, error} runlevel;

//graphics_list: when a 3d file is loaded, we need a way to keep track of all
//rendering lists, so as to prevent memory leaks when unloading data
typedef struct graphics_list_struct {
	GLuint render_list;
	char *file; //filename (to prevent duplicated 3d loading)
	struct graphics_list_struct *next;
} graphics_list;
graphics_list *graphics_list_head = NULL;

//script: human readable (read: not _programming_ language) langue which will
//describe what should be done when spawning an object (components, joints...),
//and when an component is colliding ("sensor triggering", destroying and so on)
//function arguments can point at 3d files and other scripts and so on...
//
//>Allocated at start
typedef struct script_struct {
	//placeholder for script data, now just a single variable (what to render)
	graphics_list *graphics_debug;


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
	//things to keep track of when cleaning out object (after all components have been destroyed)
	dSpaceID space_id; //store all geoms
	bool collide_space; //if the internal geoms should collide with themselves
	dJointGroupID joint_group; //store all joints

	unsigned int cmp_count;
	//placeholder for more data
	
	//used to find next/prev object in dynamically allocated chain
	//set next to null in last object in chain
	struct object_struct *prev;
	struct object_struct *next;
} object;

object *object_head = NULL;


//component: one unsplitable part bellonging to object. provides data for
//simulation (when needed), and its rendering data. created by request
//by objects and automatically created for cars (together with object) at start
//(behaviour will be described by a custom scripting language in future
//versions, stored in script structure. script execution will be triggered
//primarily by collision events)
//
//>Dynamically allocated
typedef struct component_struct {
	//keep track of the "owning" object
	object * object_parent;

	//identifiers, use them apropriately (now one component got one geom and one body)
	dBodyID body_id; //each component can have one body, but several geoms (to be added)
	dGeomID geom_id;

//	component_data *data;
	script *script;

	graphics_list *graphics_list; //points to 3d list, or NULL if invisible

	//placeholder for more physics data
	dReal mu, erp, cfm, slip1, slip2;
	//(setting mu to dInfinity and using slip{1,2} is great for wheels)
	
	bool collide; //create physical collision when touching other components
	bool collision_event; //set after each collision

	//used to find next/prev link in dynamically allocated chain
	//set prev to null in last link in chain
	struct component_struct *prev;
	struct component_struct *next;
        
        // used to store surface data for shadows
        // most likely there are better ways to do this
        struct surfaces *s;
        
        // but for now everything is a cube
        struct cube *c;
} component;

component *component_head = NULL; //points at the first component in chain


//car: pointer to object and extra data, adjusted for controlled cars. No
//scripting - used to keep track of components and objects (like weapons)
//bellonging to the player during the race
//Allocated at start
typedef struct car_struct {
	//data loaded from file (to be implemented)
	//(max_break is for non-locking breaks, not drifting break (they are infinite))
	dReal max_torque, max_break;
	dReal body_mass, wheel_mass;
	dReal suspension_erp, suspension_cfm;
	dReal wheel_mu, wheel_slip, wheel_erp, wheel_cfm;
	graphics_list *body_graphics, *wheel_graphics; //add right/left wheels

	//just for keeping track
	dGeomID body_geom; //for focusing
	dBodyID wheel_body[4]; //for "Finite Rotation" arror reduction
	dJointID joint[4]; //for applying forces on wheels

	//to add more geoms to the body of the car - rams, "spikes" and stuff
//	component collisionbox[10];
//	(currently not used)

	object *object; //one object to store car components

	//controlling values
	bool drift_breaks, breaks;
	dReal throttle, steering; //-1.0 to +1.0

	//debug data
	dReal b_x, b_y, b_z, w_r, w_w, w_x, w_y;
        
        // ugly hack
        // but how else to transfer the vertices?
        struct surfaces *s;

	struct car_struct *next;
	struct car_struct *prev;
} car;

car *car_head = NULL;


//track: the main "world", contains simulation and rendering data for one
//large 3D file for the rigid environment, and more simulation data (like
//gravity) - crappy solution for now...
//Allocated at start
struct {
	//placeholder for stuff like if it's raining/snowing and lightsources
	//
	
	graphics_list *graphics_list;
	//NOTE/TODO: currently coded to store 5 planes (components) - only temporary!
//	object obj;
//	object_data obj_d;
//	component_data *plane_data;
	component *plane[5];
} track;


//
//THE FOLLOWING IS NOT USED YET (AND PROBABLY NEEDS UPDATING TO, SHOULD BE REWRITTEN)
//


//weapon: similar to object, but with a sender and (optional) target
//Dynamically allocated
//to be added
//struct weapon_struct {
//} weapon;

//weapon_data: quite the same as object_data, but with aditional weapon
//information (like if it offers target tracking, hybrids, and so on)
//Allocated at start

//struct {
//} *weapon_data;


