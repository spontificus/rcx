#include "racetime_data.hpp"
#include "printlog.hpp"
#include <stdio.h>
#include <string.h>

Racetime_Data *Racetime_Data::head = NULL;

Racetime_Data::Racetime_Data(const char *n)
{
	name = new char[strlen(n)+1];
	strcpy (name, n);

	next = head;
	head = this;
}

Racetime_Data::~Racetime_Data()
{
	delete[] name;
}

//Racetime_Data *Racetime_Data::Find(const char *name)
template<typename T>
T *Racetime_Data::Find(const char *name)
{
	Racetime_Data *tmp;
	T *tmp_conv;

	for (tmp=head; tmp; tmp=tmp->next) //loop
		if (!strcmp(tmp->name, name)) //match
		{

			tmp_conv = dynamic_cast<T *>(tmp);

			if (!tmp_conv)
				printlog(0, "ERROR: could not convert Racetime_Data class \"%s\"!", name);
			else
				return tmp_conv;
		}

	return NULL; //else
}

//instantiation... of above template for needed cases...
#include "object.hpp"
#include "car.hpp"
//template Object_Template* Racetime_Data::Find<Object_Template>(char *);
//class Object_Template;
template Object_Template* Racetime_Data::Find<Object_Template>(const char*);
template Car_Template* Racetime_Data::Find<Car_Template>(const char*);

void Racetime_Data::Destroy_All()
{
	Racetime_Data *tmp, *data = head;
	while (data)
	{
		tmp = data;
		data=data->next;

		delete tmp;
	}

	head = NULL;
}

