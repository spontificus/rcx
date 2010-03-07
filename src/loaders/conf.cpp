#include "conf.hpp"

#include "text_file.hpp"
#include "../shared/printlog.hpp"

#include <string.h>
#include <stdlib.h>

//loads configuration file to memory (using index)
int load_conf (char *name, char *memory, const struct data_index index[])
{
	printlog(1, "loading conf file: %s", name);

	Text_File file;
	if (!file.Open(name))
		return -1;


	int i;
	int argnr;
	char *str_left; //text left in word if not completely converted
	while (file.Read_Line())
	{
		printlog(2, "Parameter: %s", file.words[0]);

		//find matching index (loop until end of list or found matching
		for (i=0; ((index[i].type !=0) && (strcmp(index[i].name,file.words[0]) != 0) ); ++i);

		if (index[i].type==0) //not match, got to end
		{
			printlog(0, "ERROR: Parameter: %s - No index name match!", file.words[0]);
			continue;
		}

		//else, we have a match
		printlog(2, "match found!, %i, %c, %i",i,index[i].type,index[i].length);

		//see if ammount of args is correct
		//argument name+values == words
		if (index[i].length+1 != file.word_count)
		{
			printlog(0, "ERROR: Parameter: %s - wrong ammount of args: %i, expected: %i!",file.words[0], file.word_count, index[i].length);
			break;
		}

		//loop through arguments
		for (argnr=0;argnr<index[i].length;++argnr)
		{
			//what type
			switch (index[i].type)
			{
				//float
				case 'f':
					*( ((float*)(memory+index[i].offset))+argnr ) = strtof(file.words[argnr+1], &str_left);
				break;

				//double
				case 'd':
					*( ((double*)(memory+index[i].offset))+argnr ) = strtod(file.words[argnr+1], &str_left);
				break;

				//bool
				case 'b':
					if ( (!strcasecmp(file.words[argnr+1], "true")) || (!strcmp(file.words[argnr+1], "1")) )
						*(((bool*)(memory+index[i].offset))+argnr) = true;
					else //false
						*(((bool*)(memory+index[i].offset))+argnr) = false;

					str_left = NULL; //assume always working
				break;

				//integer
				case 'i':
					*( ((int*)(memory+index[i].offset))+argnr ) = strtol (file.words[argnr+1], &str_left, 0);
				break;

				//unknown
				default:
					printlog(0, "ERROR: Parameter: %s - unknown type(%c)!", file.words[0], index[i].type);
				break;
			}

			//if the word wasn't processed
			if (str_left == file.words[argnr+1])
			{
				printlog(0, "ERROR: Could not translate word \"%s\" to type \"%c\"", file.words[argnr+1], index[i].type);
				//this is potentially dangerous (since the variable might not have a default value)
				//return -1; //currently assuming it has a safe default anyway (not guaranteed)
			}
		}

	}

	return 0;
}
