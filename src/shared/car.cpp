/*
 * RCX Copyright (C) Slinger
 *
 * This program comes with ABSOLUTELY NO WARRANTY!
 *
 * This is free software, and you are welcome to
 * redistribute it under certain conditions.
 *
 * See license.txt and README for more info
 */

#include "car.hpp"
#include "printlog.hpp"
Car_Template::Car_Template(const char *name) :Racetime_Data(name)
{
	conf = car_conf_defaults; //set conf values to default
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

	//linking
	next=head;
	head=this;

	if (next)
		next->prev=this;

	prev=NULL;
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


