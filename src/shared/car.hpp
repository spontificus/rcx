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

#define CAR_MAX_BOXES 20

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

class Car_Template:public Racetime_Data
{
	public:
		static Car_Template *Load(const char *path);
		class Car *Spawn(dReal x, dReal y, dReal z);

	private:
		Car_Template(const char *name); //only allocate through spawn function

		//conf:
		struct Car_Conf conf; //loads from conf
		static const struct Car_Conf conf_defaults; //needs to be initialized in car.cpp
		static const struct Conf_Index conf_index[]; //initialized in car.cpp

		//more data:
		char *name;
		//std::vector<dReal[6]> box;
		dReal fsteer, rsteer, fmotor, rmotor, fbreak, rbreak;
		file_3d_struct *wheel_graphics; //add right/left wheels
		//file_3d_struct *box_graphics[CAR_MAX_BOXES];
		dReal inertia_tensor;
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
