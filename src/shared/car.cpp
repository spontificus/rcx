#include "car.hpp"
#include "printlog.hpp"

//template
const struct Car_Conf Car_Template::conf_defaults = {
	800000, 60000, 1.0,
	true,
	6000, 500,
	0.3, 0.00006,
	2.0, 0.1, 0.00004, 0.8, 0.001, 0.0,
	0.1, 0.01, 0.8, 0.01,
	{10,5,15}, 1, 4, 0.5,
	{3.5,8.2,1},
	100, 0, 50,
	{5.8,4.4,2,1.5}, {1.5,1.7}, {2.9,2.2}, 2.4};

const struct Conf_Index Car_Template::conf_index[] = {
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

Car_Template::Car_Template(const char *name) :Racetime_Data(name)
{
	conf = conf_defaults; //set conf values to default
}



Car *Car::head = NULL;

//allocates car, add to list...
Car::Car(void)
{
	printlog(2, "configuring Car class");

	//default values
	//control values
	dir = 1; //initiate to 1 for default

	drift_breaks = true; //if the user does nothing, lock wheels
	breaks = false;
	throttle = 0;
	steering = 0;
	
	velocity = 0;

	next=head;
	head=this;
	if (next)
		next=prev;

	//set x in all boxes to 0 (disable)
	/*for (i=0;i<CAR_MAX_BOXES;++i)
		car_head->box[i][0]=0;*/
}

//run _before_ starting full erase of object/component lists (at race end)
Car::~Car()
{
	printlog(2, "clearing Car class");

	//remove from list
	if (!prev) //head
		head = next;
	else //not head
		prev->next = next;

	if (next) //not last
		next->prev = prev;
}


