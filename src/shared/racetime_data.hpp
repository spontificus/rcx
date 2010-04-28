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

#ifndef _RCX_RACETIME_DATA_H
#define _RCX_RACETIME_DATA_H

#include <typeinfo>

//TODO: polymorph, never used in this baseclass form

class Racetime_Data
{
	public:
		static void Destroy_All();

		//find data that matches specified name and type
		static Racetime_Data *Find(const char *name);

	protected:
		Racetime_Data(const char *name);
		//just make sure the subclass destructor gets called
		virtual ~Racetime_Data();

	private:
		char *name;
		static Racetime_Data *head;
		Racetime_Data *next;
};
#endif
