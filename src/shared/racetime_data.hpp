#ifndef _RCX_RACETIME_DATA_H
#define _RCX_RACETIME_DATA_H

//TODO: polymorph, never used in this baseclass form

class Racetime_Data
{
	public:
		void Destroy_All();
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
