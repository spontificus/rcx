#include "timers.hpp"
#include "../shared/object.hpp"
#include "../shared/geom.hpp"

Animation_Timer *Animation_Timer::head = NULL;

Animation_Timer::Animation_Timer (Object *obj, Script *scr, dReal start, dReal stop,
		dReal duration):object(obj), script(scr), counter(start), goal(stop)
{
	speed = (stop-start)/(duration*1000);

	//increase object activity (to prevent object from selfdelete while timer is counting)
	object->Increase_Activity();

	//add to list
	next=head;
	prev=NULL;
	head=this;
}

Animation_Timer::~Animation_Timer()
{
	//remove from list
	if (prev)
		prev->next=next;
	else
		head=next;

	if (next)
		next->prev=prev;

	//timer not part of object activity anymore
	object->Decrease_Activity();
}

void Animation_Timer::Events_Step(Uint32 step)
{
	Animation_Timer *timer, *tmp;
	timer = head;
	while (timer)
	{
		//process timer:

		//
		//TMP: assumes flipper
		//

		//note: TMP: the script is actually a pointer to the flipper geom...
		dGeomID geom = (dGeomID)timer->script;
		const dReal *pos = dGeomGetPosition(geom);
		dGeomSetPosition(geom, pos[0], pos[1], timer->counter);


			//const dReal *pos;
			//pos = dGeomGetPosition(geom->flipper_geom);
			//dGeomSetPosition(
			/*if (geom->colliding)
			{
				const dReal *pos;
				pos = dGeomGetPosition(geom->flipper_geom);
				dGeomSetPosition(geom->flipper_geom, pos[0], pos[1],
						pos[2]+step*0.02);
				if ((geom->flipper_counter+=step) > 10)
					//geom->colliding=false; //no
			}
			else if (geom->flipper_counter>0)
			{
				const dReal *pos;
				pos = dGeomGetPosition(geom->flipper_geom);
				dGeomSetPosition(geom->flipper_geom, pos[0], pos[1],
						pos[2]-step*0.02);
				geom->flipper_counter-=step;
			}
			else
				geom->flipper_counter=0;*/

		//
		//end of TMP
		//

		
		//depending on which direction counter goes (increase/decrease) determine if reached goal
		if (	(timer->speed > 0 && timer->counter >= timer->goal) || //counter increased to goal
			(timer->speed < 0 && timer->counter <= timer->goal)  ) //counter decreased to goal
		{
			//delete
			tmp=timer;
			timer=timer->next; //next
			delete tmp;
		}
		else //just move to next timer
		{
			//increase counter
			timer->counter += (timer->speed)*step;

			//move to next
			timer=timer->next;
		}
	}
}
