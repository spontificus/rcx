//#include "../shared/shared.hpp"
#include "../shared/profile.hpp"
#include "../shared/printlog.hpp"
#include "text_file.hpp"
//#include "loaders.hpp"

#ifdef __cplusplus
// required to iterate through an enum in C++
template <class Enum>
Enum & enum_increment(Enum & value, Enum begin, Enum end)
{
	return value = (value == end) ? begin : Enum(value + 1);
}

SDLKey & operator++ (SDLKey & key)
{
	return enum_increment(key, SDLK_FIRST, SDLK_LAST);
}
#endif

//translate button name to key number
SDLKey get_key (char *name)
{
	printlog(2, "translating key name");
	SDLKey key;

	for (key=SDLK_FIRST; key<SDLK_LAST; ++key)
		if (strcmp(SDL_GetKeyName(key), name) == 0)
		{
			printlog(2, "name match found");
			return key;
		}

	//we only get here if no match found
	printlog(0, "ERROR: Key name %s didn't match any known key!", name);
	return UNUSED_KEY;
}

//load profile (conf and key list)
Profile *Profile_Load (const char *path)
{
	printlog(1, "loading profile: %s", path);

	//create
	Profile *prof = new Profile; //allocate
	prof->next = profile_head;
	prof->prev = NULL;
	profile_head = prof;
	if (prof->next)
		prof->next->prev=prof;

	*prof = profile_defaults; //set all to defaults

	//load personal conf
	char *conf=(char *)calloc(strlen(path)+13+1,sizeof(char));//+1 for \0
	strcpy (conf,path);
	strcat (conf,"/profile.conf");

	load_conf(conf, (char *)prof, profile_index); //try to load conf

	free (conf);

	//set camera
	if (prof->camera >0 && prof->camera <5)
		camera.Set_Settings (&(prof->cam[prof->camera -1]));
	else
		printlog(0, "ERROR: default camera should be a value between 1 and 4!");

	//load key list
	char list[strlen(path)+9+1];
	strcpy (list,path);
	strcat (list,"/keys.lst");

	printlog(1, "loading key list: %s", list);
	Text_File file;

	if (file.Open(list))
	{
		while (file.Read_Line())
		{
			printlog(2, "action: %s", file.words[0]);

			//find match
			int i;
			for (i=0; (profile_key_list[i].offset != 0) && (strcmp(profile_key_list[i].name, file.words[0])); ++i);

			if (profile_key_list[i].offset == 0) //we reached end (no found)
				printlog(0, "ERROR: no key action match: %s!",file.words[0]);
			else //found
			{
				printlog(2, "match found");
				if (file.word_count == 2) //got a key name
					*(SDLKey*)((char*)prof+profile_key_list[i].offset) = get_key(file.words[1]);
				else
					printlog(0, "ERROR: no key specified for action \"%s\"", file.words[i]);
			}
		}
	}
	else
		printlog(0, "ERROR: could not open file");

	return prof;
}

