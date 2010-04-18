#ifndef _RCX_CAR_H
#define _RCX_CAR_H
//car: pointer to object and extra data, adjusted for controlled cars. No
//scripting - used to keep track of components and objects (like weapons)
//bellonging to the player during the race
//Allocated at start
#include "racetime_data.hpp"
#include "file_3d.hpp"
#include "object.hpp"
#include "body.hpp"
#include "geom.hpp"
#include "../loaders/conf.hpp"

#include <vector>

//for loading car.conf
struct Car_Conf
{
	dReal max_torque, motor_tweak, max_break;
	bool torque_compensator;
	dReal body_mass, wheel_mass;
	dReal suspension_erp, suspension_cfm;
	dReal wheel_mu, rim_mu, wheel_slip, wheel_erp, wheel_cfm, wheel_bounce;
	dReal body_mu, body_slip, body_erp, body_cfm;

	dReal body_linear_drag[3], body_angular_drag, wheel_linear_drag, wheel_angular_drag;

	dReal body[3];

	//values for moving steering/breaking/turning between front/rear wheels
	int steer_ratio, motor_ratio, break_ratio;

	//debug sizes
	dReal s[4],w[2],wp[2],jx;
};

const struct Car_Conf car_conf_defaults = {
	800000, 1.0, 60000,
	true,
	6000, 500,
	0.3, 0.00006,
	2.0, 0.1, 0.00004, 0.8, 0.001, 0.0,
	0.1, 0.01, 0.8, 0.01,
	{10,5,15}, 1, 4, 0.5,
	{3.5,8.2,1},
	100, 0, 50,
	{5.8,4.4,2,1.5}, {1.5,1.7}, {2.9,2.2}, 2.4};

const struct Conf_Index car_conf_index[] = {
	{"max_torque",		'f',1, offsetof(struct Car_Conf, max_torque)},
	{"motor_tweak",		'f',1, offsetof(struct Car_Conf, motor_tweak)},
	{"max_break",		'f',1, offsetof(struct Car_Conf, max_break)},
	{"torque_compensator",	'b',1, offsetof(struct Car_Conf, torque_compensator)},
	{"body_mass",		'f',1, offsetof(struct Car_Conf, body_mass)},
	{"wheel_mass",		'f',1, offsetof(struct Car_Conf, wheel_mass)},

	{"front/rear_steer",	'i',1, offsetof(struct Car_Conf, steer_ratio)},
	{"front/rear_motor",	'i',1, offsetof(struct Car_Conf, motor_ratio)},
	{"front/rear_break",	'i',1, offsetof(struct Car_Conf, break_ratio)},

	{"suspension_erp",	'f',1, offsetof(struct Car_Conf, suspension_erp)},
	{"suspension_cfm",	'f',1, offsetof(struct Car_Conf, suspension_cfm)},
	{"wheel_mu",		'f',1, offsetof(struct Car_Conf, wheel_mu)},
	{"rim_mu",		'f',1, offsetof(struct Car_Conf, rim_mu)},
	{"wheel_slip",		'f',1, offsetof(struct Car_Conf, wheel_slip)},
	{"wheel_erp",		'f',1, offsetof(struct Car_Conf, wheel_erp)},
	{"wheel_cfm",		'f',1, offsetof(struct Car_Conf, wheel_cfm)},
	{"wheel_bounce",	'f',1, offsetof(struct Car_Conf, wheel_bounce)},
	{"body",		'f',3, offsetof(struct Car_Conf, body[0])},
	{"body_mu",		'f',1, offsetof(struct Car_Conf, body_mu)},
	{"body_slip",		'f',1, offsetof(struct Car_Conf, body_slip)},
	{"body_erp",		'f',1, offsetof(struct Car_Conf, body_erp)},
	{"body_cfm",		'f',1, offsetof(struct Car_Conf, body_cfm)},

	{"body_linear_drag",	'f',3, offsetof(struct Car_Conf, body_linear_drag)},
	{"body_angular_drag",	'f',1, offsetof(struct Car_Conf, body_angular_drag)},
	{"wheel_linear_drag",	'f',1, offsetof(struct Car_Conf, wheel_linear_drag)},
	{"wheel_angular_drag",	'f',1, offsetof(struct Car_Conf, wheel_angular_drag)},
	
	//the following is for sizes not yet determined
	{"s",	'f',	4,	offsetof(struct Car_Conf, s[0])}, //flipover
	{"w",	'f',	2,	offsetof(struct Car_Conf, w[0])}, //wheel
	{"wp",	'f',	2,	offsetof(struct Car_Conf, wp[0])}, //wheel pos
	{"jx",	'f',	1,	offsetof(struct Car_Conf, jx)}, //joint x position
	{"",0,0}};//end


class Car_Template:public Racetime_Data
{
	public:
		static Car_Template *Load(const char *path);
		class Car *Spawn(dReal x, dReal y, dReal z);

	private:
		Car_Template(const char *name); //only allocate through spawn function

		//conf:
		struct Car_Conf conf; //loads from conf

		//more data:
		char *name;
		dReal fsteer, rsteer, fmotor, rmotor, fbreak, rbreak;
		file_3d *wheel_graphics; //add right/left wheels
		dReal inertia_tensor;

		//geoms
		struct box {
			dReal size[3];
			dReal pos[3];
			dReal rot[3];
		};

		std::vector<class box> boxes;
		std::vector<file_3d*> box_graphics; //TMP: for storing graphics

		struct sphere {
			dReal radius;
			dReal pos[3];
		};

		std::vector<class sphere> spheres;
		std::vector<file_3d*> sphere_graphics;

		struct capsule {
			dReal size[2];
			dReal pos[3];
			dReal rot[3];
		};

		std::vector<class capsule> capsules;
		std::vector<file_3d*> capsule_graphics;
};

class Car:public Object
{
	public:
		~Car();

		static void Physics_Step();

		//public for now
		//controlling values
		bool drift_breaks, breaks;
		dReal throttle, steering; //-1.0 to +1.0
		dReal velocity; //keep track of car velocity

	private:
		Car(); //not allowed to be allocated freely
		friend class Car_Template; //only one allowed to create Car objects
		friend class Camera; //needs access to car info

		//configuration data (copied from Car_Template)
		dReal max_torque, motor_tweak, max_break;
		bool torque_compensator;
		dReal fsteer, rsteer, fmotor, rmotor, fbreak, rbreak;
		dReal inertia_tensor;

		//just for keeping track
		dBodyID bodyid,wheel_body[4];
		dJointID joint[4];

		Geom *wheel_geom_data[4];

		//flipover sensors
		Geom *sensor1, *sensor2;
		dReal dir; //direction, 1 or -1




		//appart from the object list, keep a list of all cars
		static Car *head;
		Car *prev, *next;
};

#endif
