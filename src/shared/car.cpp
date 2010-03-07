#include "car.hpp"
#include "printlog.hpp"

car_struct *car_head = NULL;

//allocates car, add to list...
//(no object allocations - since we want to allocate car without spawning)
car_struct *allocate_car(void)
{
	printlog(2, "allocating car");
	car_struct *car_next = car_head;
	car_head = (car_struct *)malloc(sizeof(car_struct));

	//add to list
	car_head->prev=NULL;
	car_head->next=car_next;
	if (car_next)
		car_next->prev=car_head;
	else
		printlog(2, "(first registered)");


	//default values
	printlog(2, "FIXME: set more values to default");

	car_head->name = NULL;
	car_head->spawned = false;

	int i;
	for (i=0;i<CAR_MAX_BOXES;++i)
		car_head->box_graphics[i] = NULL;
	car_head->wheel_graphics = NULL;

	car_head->dir = 1; //initiate to 1 for default

	car_head->drift_breaks = true; //if the user does nothing, lock wheels
	car_head->breaks = false;
	car_head->throttle = 0;
	car_head->steering = 0;
	
	car_head->velocity = 0;

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

	return car_head;
}

//run _before_ starting full erase of object/component lists (at race end)
void free_car (car_struct *target)
{
	printlog(2, "freeing car");

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


