#include "racetime_data.hpp"
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
Racetime_Data *Racetime_Data::Find(const char *name)
{
	Racetime_Data *tmp;

	for (tmp=head; tmp; tmp=tmp->next) //loop
		if (!strcmp(tmp->name, name)) //match
			return tmp;

	return NULL; //else
}

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

