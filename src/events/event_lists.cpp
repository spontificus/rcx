#include "event_lists.hpp"


//
//component buffers:
//

//make sure header pointers are NULL for the wanted lists
Buffer_Event_List *Buffer_Event_List::geom_head = NULL;
Buffer_Event_List *Buffer_Event_List::body_head = NULL;
Buffer_Event_List *Buffer_Event_List::joint_head = NULL;

//for adding to lists
Buffer_Event_List::Buffer_Event_List(Geom *geom): component(geom)
{
	next = geom_head;
	geom_head = this;
}

Buffer_Event_List::Buffer_Event_List(Body *body): component(body)
{
	next = body_head;
	body_head = this;
}

Buffer_Event_List::Buffer_Event_List(Joint *joint): component(joint)
{
	next = joint_head;
	joint_head = this;
}

//for getting
bool Buffer_Event_List::Get_Event(Geom **geom)
{
	//if end of list
	if (!geom_head)
		return false;

	//remove from list
	Buffer_Event_List *tmp = geom_head;
	geom_head = tmp->next;

	//set wanted data and delete
	*geom = (Geom*)tmp->component; //safe to assume this list consists of wanted component
	delete (tmp);

	return true;
}

bool Buffer_Event_List::Get_Event(Body **body)
{
	if (!body_head)
		return false;

	Buffer_Event_List *tmp = body_head;
	body_head = tmp->next;

	*body = (Body*)tmp->component;
	delete (tmp);

	return true;
}

bool Buffer_Event_List::Get_Event(Joint **joint)
{
	if (!joint_head)
		return false;

	Buffer_Event_List *tmp = joint_head;
	joint_head = tmp->next;

	*joint = (Joint*)tmp->component;
	delete (tmp);

	return true;
}

//removing
void Buffer_Event_List::Remove(Geom *comp)
{
	printlog(2, "seeking and removing all events for specified Geom");

	Buffer_Event_List *p = geom_head; //points at current event in list
	Buffer_Event_List **pp = &geom_head; //points at pointer ("next") pointing at current event

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

void Buffer_Event_List::Remove(Body *comp)
{
	printlog(2, "seeking and removing all events for specified Body");
	Buffer_Event_List *p = body_head;
	Buffer_Event_List **pp = &body_head;
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

void Buffer_Event_List::Remove(Joint *comp)
{
	printlog(2, "seeking and removing all events for specified Joint");

	Buffer_Event_List *p = joint_head;
	Buffer_Event_List **pp = &joint_head;
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

//
//sensor triggering/untriggering
//

Sensor_Event_List *Sensor_Event_List::head = NULL;

Sensor_Event_List::Sensor_Event_List(Geom *g): geom(g)
{
	next = head;
	head = this;
}

bool Sensor_Event_List::Get_Event(Geom **g)
{
	if (!head)
		return false;

	Sensor_Event_List *tmp = head;
	head = tmp->next;

	*g = tmp->geom;
	delete (tmp);

	return true;
}

void Sensor_Event_List::Remove(Geom *geom)
{
	printlog(2, "removing all events for specified Sensor");

	Sensor_Event_List *p = head; //points at first event
	Sensor_Event_List **pp = &head; //points at pointer for first event

	while (p)
	{
		if (p->geom == geom)
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

//
//object inactivity:
//
Object_Event_List *Object_Event_List::head = NULL;

Object_Event_List::Object_Event_List(Object *obj): object(obj)
{
	next = head;
	head = this;
}

bool Object_Event_List::Get_Event(Object **obj)
{
	if (!head)
		return false;

	*obj = head->object;

	Object_Event_List *tmp = head;
	head = head->next;

	delete tmp;
	return true;
}

void Object_Event_List::Remove(Object *obj)
{
	printlog(2, "removing all events for specified Object");

	Object_Event_List *p = head;
	Object_Event_List **pp = &head;
	while (p)
	{
		if (p->object == obj)
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
