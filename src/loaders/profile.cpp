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
	printlog(2, " > translating key name\n");
	SDLKey key;

	for (key=SDLK_FIRST; key<SDLK_LAST; ++key)
		if (strcmp(SDL_GetKeyName(key), name) == 0)
		{
			printlog(2, " * name match found\n");
			return key;
		}

	//we only get here if no match found
	printlog(0, "ERROR: Key name %s didn't match any known key!\n", name);
	return UNUSED_KEY;
}

//load profile (conf and key list)
profile *load_profile (char *path)
{
	printlog(1, "=> loading profile: %s\n", path);
	profile *prof = allocate_profile();

	//load personal conf
	char *conf=(char *)calloc(strlen(path)+13+1,sizeof(char));//+1 for \0
	strcpy (conf,path);
	strcat (conf,"/profile.conf");

	if (load_conf(conf, (char *)prof, profile_index))
		return NULL;

	free (conf);

	//set camera
	if (prof->camera >0 && prof->camera <5)
		set_camera_settings (&(prof->cam[prof->camera -1]));
	else
		printlog(0, "ERROR: default camera should be a value between 1 and 4!\n");

	//load key list
	char list[strlen(path)+9+1];
	strcpy (list,path);
	strcat (list,"/keys.lst");

	printlog(1, "-> loading key list: %s\n", list);
	Text_File file(list);

	if (!file.open)
		return NULL;

	while (file.Read_Line())
	{
		printlog(2, " * action: %s\n", file.words[0]);

		//find match
		int i;
		for (i=0; (profile_key_list[i].name != '\0') && (strcmp(profile_key_list[i].name, file.words[0])); ++i);

		if (profile_key_list[i].name == '\0') //we reached end (no found)
			printlog(0, "ERROR: no key action match: %s!\n",file.words[0]);
		else //found
		{
			printlog(2, " * match found\n");
			if (file.word_count == 2) //got a key name
				*(SDLKey*)((char*)prof+profile_key_list[i].offset) = get_key(file.words[1]);
			else
				printlog(0, "ERROR: no key specified for action \"%s\"\n", file.words[i]);
		}
	}

	printlog(1, "\n");
	return prof;
}

