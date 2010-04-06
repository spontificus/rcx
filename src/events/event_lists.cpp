#include "event_lists.hpp"

//make sure header pointers are NULL for the wanted lists
Event_Lists *Event_Lists::geom_head = NULL;
Event_Lists *Event_Lists::body_head = NULL;
Event_Lists *Event_Lists::joint_head = NULL;

//for adding to lists
Event_Lists::Event_Lists(Geom *geom): component(geom)
{
	next = geom_head;
	geom_head = this;
}

Event_Lists::Event_Lists(Body *body): component(body)
{
	next = body_head;
	body_head = this;
}

Event_Lists::Event_Lists(Joint *joint): component(joint)
{
	next = joint_head;
	joint_head = this;
}

//for getting
bool Event_Lists::Get_Event(Geom **geom)
{
	//if end of list
	if (!geom_head)
		return false;

	//remove from list
	Event_Lists *tmp = geom_head;
	geom_head = tmp->next;

	//set wanted data and delete
	*geom = (Geom*)tmp->component; //safe to assume this list consists of wanted component
	delete (tmp);

	return true;
}

bool Event_Lists::Get_Event(Body **body)
{
	if (!body_head)
		return false;

	Event_Lists *tmp = body_head;
	body_head = tmp->next;

	*body = (Body*)tmp->component;
	delete (tmp);

	return true;
}

bool Event_Lists::Get_Event(Joint **joint)
{
	if (!joint_head)
		return false;

	Event_Lists *tmp = joint_head;
	joint_head = tmp->next;

	*joint = (Joint*)tmp->component;
	delete (tmp);

	return true;
}

//removing
void Event_Lists::Remove(Geom *comp)
{
	printlog(2, "seeking and removing all events for specified Geom");

	Event_Lists *p = geom_head; //points at current event in list
	Event_Lists **pp = &geom_head; //points at pointer ("next") pointing at current event

	while (p)
	{
		if (p->component == comp) //remove
		{
			*pp = p->next; //change last 'next' pointer
			delete p; //delete this block
			p = *pp; //point at next block
		}
		else //keep, jump over
		{
			pp = &p->next; //change pointer-pointer to next 'next' pointer... ;-)
			p = p->next; //change pointer to next block
		}
	}
}

void Event_Lists::Remove(Body *comp)
{
	printlog(2, "seeking and removing all events for specified Body");
	Event_Lists *p = body_head;
	Event_Lists **pp = &body_head;
	while (p)
	{
		if (p->component == comp)
		{
			*pp = p->next;
			delete p;
			p = *pp;
		}
		else
		{
			pp = &p->next;
			p = p->next;
		}
	}
}

void Event_Lists::Remove(Joint *comp)
{
	printlog(2, "seeking and removing all events for specified Joint");

	Event_Lists *p = joint_head;
	Event_Lists **pp = &joint_head;
	while (p)
	{
		if (p->component == comp)
		{
			*pp = p->next;
			delete p;
			p = *pp;
		}
		else
		{
			pp = &p->next;
			p = p->next;
		}
	}
}

