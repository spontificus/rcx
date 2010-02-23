#ifndef _RCX_CAR_H
#define _RCX_CAR_H
//car: pointer to object and extra data, adjusted for controlled cars. No
//scripting - used to keep track of components and objects (like weapons)
//bellonging to the player during the race
//Allocated at start
#include "file_3d.hpp"
#include "body.hpp"
#include "geom.hpp"
#include "../loaders/conf.hpp"

#define CAR_MAX_BOXES 20

typedef struct car_struct {
	//data loaded from file (to be implemented)
	//(max_break is for non-locking breaks, not drifting break (they are infinite))
	char *name;
	bool spawned; //don't assume loaded cars are participating in race (TODO: split struct into 2: loaded and spawned)


	dReal max_torque, motor_tweak, max_break;
	dReal body_mass, wheel_mass;
	dReal suspension_erp, suspension_cfm;
	dReal wheel_mu, rim_mu, wheel_slip, wheel_erp, wheel_cfm, wheel_bounce;
	dReal body_mu, body_slip, body_erp, body_cfm;

	dReal body_linear_drag[3], body_angular_drag, wheel_linear_drag, wheel_angular_drag;

	file_3d_struct *wheel_graphics; //add right/left wheels
	file_3d_struct *box_graphics[CAR_MAX_BOXES];

	//just for keeping track
	object_struct *object; //one object to store car components

//	dGeomID body_geom; //for focusing
	dBodyID bodyid,wheel_body[4];
	dJointID joint[4];

	geom_data *wheel_geom_data[4];

	//flipover sensors
	geom_data *sensor1, *sensor2;
	dReal dir; //direction, 1 or -1

	//controlling values
	bool drift_breaks, breaks;
	dReal throttle, steering; //-1.0 to +1.0
	dReal velocity; //keep track of car velocity

	dReal body[3];
	dReal box[CAR_MAX_BOXES][6];

	//values for moving steering/breaking/turning between front/rear wheels
	int steer_ratio, motor_ratio, break_ratio;
	dReal fsteer, rsteer, fmotor, rmotor, fbreak, rbreak;
	
	//debug sizes
	dReal s[4],w[2],wp[2],jx;

	struct car_struct *next;
	struct car_struct *prev;
} car_struct;

extern car_struct *car_head;

const struct data_index car_index[] = {
	{"max_torque",		'f',1, offsetof(struct car_struct, max_torque)},
	{"motor_tweak",		'f',1, offsetof(struct car_struct, motor_tweak)},
	{"max_break",		'f',1, offsetof(struct car_struct, max_break)},
	{"body_mass",		'f',1, offsetof(struct car_struct, body_mass)},
	{"wheel_mass",		'f',1, offsetof(struct car_struct, wheel_mass)},

	{"front/rear_steer",	'i',1, offsetof(struct car_struct, steer_ratio)},
	{"front/rear_motor",	'i',1, offsetof(struct car_struct, motor_ratio)},
	{"front/rear_break",	'i',1, offsetof(struct car_struct, break_ratio)},

	{"suspension_erp",	'f',1, offsetof(struct car_struct, suspension_erp)},
	{"suspension_cfm",	'f',1, offsetof(struct car_struct, suspension_cfm)},
	{"wheel_mu",		'f',1, offsetof(struct car_struct, wheel_mu)},
	{"rim_mu",		'f',1, offsetof(struct car_struct, rim_mu)},
	{"wheel_slip",		'f',1, offsetof(struct car_struct, wheel_slip)},
	{"wheel_erp",		'f',1, offsetof(struct car_struct, wheel_erp)},
	{"wheel_cfm",		'f',1, offsetof(struct car_struct, wheel_cfm)},
	{"wheel_bounce",		'f',1, offsetof(struct car_struct, wheel_bounce)},
	{"body_mu",		'f',1, offsetof(struct car_struct, body_mu)},
	{"body_slip",		'f',1, offsetof(struct car_struct, body_slip)},
	{"body_erp",		'f',1, offsetof(struct car_struct, body_erp)},
	{"body_cfm",		'f',1, offsetof(struct car_struct, body_cfm)},

	{"body_linear_drag",	'f',3, offsetof(struct car_struct, body_linear_drag)},
	{"body_angular_drag",	'f',1, offsetof(struct car_struct, body_angular_drag)},
	{"wheel_linear_drag",	'f',1, offsetof(struct car_struct, wheel_linear_drag)},
	{"wheel_angular_drag",	'f',1, offsetof(struct car_struct, wheel_angular_drag)},

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

car_struct *load_car(char *);
void spawn_car(car_struct *target, dReal x, dReal y, dReal z);
void remove_car (car_struct* target);
car_struct *allocate_car(void);
void free_car (car_struct *target);
extern car_struct *venom;
#endif
