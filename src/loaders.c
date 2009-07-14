//different loaders for files (3D, conf, scripts...), cars, weapons and so on.
//also handles spawning of them, and removing
//(allocating and freeing is handled in shared.c)
//
//See main.c for licensing info

FILE *open_file(char *path)
{
	FILE *fp;
	#ifdef windows
		fp = fopen(path, "rb");
	#else
		fp = fopen(path, "r");
	#endif
	return fp;
}

FILE *open_file_rel_to_file (char *path, char *file)
{
	int file_l = strlen(file);

	//find last /
	int i;
	for (i=strlen(path); 0<i; --i)
		if (path[i] == '/')
			break;

	++i;

	char *new_path;
	//no / , just append
	if (i==0)
	{
		new_path = (char*) calloc(strlen(path)+file_l+2, sizeof(char));
		strcpy (new_path, path);
		strcat (new_path, "/");
		strcat (new_path, file);
	}
	else //got length before /
	{
		new_path = (char*) calloc(i+strlen(file)+2, sizeof(char));
		int j;
		for (j=0; j<i; ++j)
			new_path[j]=path[j];
		new_path[++j]='/';
		for (j=0; j<file_l; ++j)
			new_path[i+j]=file[j];
		new_path[i+j]='\0';
	}

	FILE *fp = open_file(new_path);
	free (new_path);
	return fp;
}

#define MAX_WORDS 100

//read a line from file pointer, remove blanks and sepparate words
//guaranteed to return at least one word, or (if nothing left), NULL
int get_word_length(FILE *fp)
{
	printlog(2, " > get_word_length\n");
	int i = 0;
	char c;
	while ((c=fgetc(fp)) != EOF &&!isspace(c) &&c!='#')
		++i;
	if (fseek(fp, -(i+2), SEEK_CUR) == -1)
		printlog(0, "ERROR: error seeking in file!\n");
	return (i+1);
}

//ignore newline, return NULL on EOF
char *get_word (FILE *fp, bool newline_sensitive)
{
	printlog(2, " > get_word\n");
	char c;
	do
	{
		if ((c = fgetc(fp)) == EOF)
			return NULL;
		if (newline_sensitive&& c=='\n')
			return NULL;
	}
	while (isspace(c));

	if (c == '#')
	{
		printlog(2, " * comment\n");
		do
		{
			if ((c = fgetc(fp)) == EOF)
				return NULL;
		}
		while (c != '\n');

		if (newline_sensitive)
			return NULL;
		else
			return get_word(fp, false);
	}

	//we've got a word
	int l; //word length
	bool last_step = false; //do a step after last char (when it is ")

	if (c == '"')
	{
		printlog(2, " * quoted (special) word\n");
		l = 0;
		while ((c = fgetc(fp)) != EOF)
		{
			if (c == '\"')
			{
				last_step = true;
				break;
			}
			++l;
		}

		if (fseek(fp, -(l+1), SEEK_CUR) == -1)
			printlog(0, "ERROR: error seeking in file!\n");
	}
	else
		l = get_word_length(fp);

	printlog(2, " * length: %i\n", l);

	if (l == 0) //shouldn't be too usual
	{
		printlog (2, " * no length, trying new word\n");
		return get_word(fp, newline_sensitive);
	}

	char *word = (char *)calloc(l+1, sizeof(char));

	fread (word, sizeof(char), l, fp);

	word[l] = '\0';
	printlog(2, " * word: %s\n", word);

	//in case we need to throw away the next char (if quoted)
	if (last_step)
		fgetc(fp);

	return (word);
}

//TODO: it _might_ be better to just allocate and read one line (not sepparate words, since
//sscanf can process space when needed instead)
char **get_word_list (FILE *fp)
{
	printlog(2, " > get_word_list\n");

	char *word0;
	//get first word
	if ((word0 = get_word(fp, false)))
	{
		char **word = (char **)calloc(MAX_WORDS, sizeof(char*));
		word[0] = word0;
		int i;
		for (i=1; (word[i]=get_word(fp,true)); ++i)
			if (i==MAX_WORDS)
			{
				printlog(0, "ERROR: too many words in one line!\n");
				break;
			}

		word[i+1] = NULL;
	
		return word;
	}
	else
		return NULL;
}


//safely free ragged array
void free_word_list (char **target)
{
	printlog(2, " > free_word_list\n");
	int i;
	if (target == NULL)
		printlog(0, "ERROR: TARGET NULL");

	for (i=0; target[i]!=NULL; ++i)
		free (target[i]);

	free (target);
}

//loads configuration file to memory (using index)
//
//(you may indent my code?)
int load_conf (char *name, char *memory, struct data_index index[])
{
 printlog(1, "-> loading conf file: %s\n", name);
 FILE *fp;

 fp = open_file(name);

 if (!fp)
 {
  printlog(0, "ERROR opening file %s (doesn't exist?)\n", name);
  return -1;
 }
 else
 {
#ifdef windows
  printlog(2, "(using binary read mode)\n");
#endif
  int i,j,tmp;
  char **word;
  const char *argscan;
  size_t argsize;
  while ((word=get_word_list(fp)))
  {
   printlog(2, " * Parameter: %s\n", word[0]);
   for (i=0; index[i].type !=0; ++i)
    if (strcmp(index[i].name,word[0]) == 0)
    {
     printlog(2, " * match found!, %i, %c, %i\n",i,index[i].type,index[i].length);
     //arguments (first:what kind?)
     switch (index[i].type)
     {
      case 'f':
       argscan="%f";
       argsize=sizeof(float);
      break;

      case 'd':
       argscan="%d";
       argsize=sizeof(double);
      break;

      case 'b':
       argscan="%b";
       argsize=sizeof(bool);
      break;

      //string copy (WARNING: this requires a manual free, preferably in the appropriate <???>_free function)
      case 's':
       argscan="%s";
       argsize=sizeof(char*);
      break;

      case 'i':
       argscan="%i";
       argsize=sizeof(int);
      break;

      default:
       printlog(0, "ERROR: Parameter: %s - unknown type(%c)!\n", word[0], index[i].type);
       argscan="";
       argsize=0;
      break;
     }

    //see if ammount of args is correct
    for(tmp=0;word[tmp+1];++tmp);
    if (tmp!=index[i].length)
    {
     printlog(0, "ERROR: Parameter: %s - wrong ammount of args: %i, expected: %i!\n",word[0], tmp, index[i].length);
     break;
    }

    for (j=0;j<index[i].length;++j)
    {
     if (argscan[1]=='b')
      if(strcmp(word[j+1],"true") == 0)
       *(bool*)(memory+index[i].offset+j*argsize) = true;
      else
       *(bool*)(memory+index[i].offset+j*argsize) = false;

#ifdef windows //windows (MinGW?) sscanf can't process "inf" for infinite float
     else if (argscan[1]=='f'&&strcmp(word[j+1],"inf") == 0)
      *(float*)(memory+index[i].offset+j*argsize) = 1.0f/0.0f;
#endif

     //while there's a GNU extension for the scanf family which makes it simple to copy strings, lets do it old school!
     else if (argscan[1]=='s')
     {
	     char *str = calloc (strlen(word[j+1]) +1, sizeof(char));
	     strcpy (str , word[j+1]);
             *(char**)(memory+index[i].offset+j*argsize) = str;
     }

     else
      if(sscanf(word[j+1],argscan,(memory+index[i].offset+j*argsize)) != 1)
            printlog(0, "ERROR: Parameter: %s - Error reading argument %i!\n", word[0],j);
    }

     break;
    }
   if (index[i].type==0) //not match, got to end
    printlog(0, "ERROR: Parameter: %s - No index name match!\n", word[0]);

   free_word_list(word);

  }
  fclose (fp);
  printlog(2, "\n");
  return 0;
 }
}

//load an obj (and the mtl file it specifies) as a trimesh (for rendering
//and optional generation of ODE trimesh geom)
//
//warning: does not do a lot of error checking!
//some limits includes requiring normals for each index, and only one mtl file
trimesh *load_obj (char *file, float resize)
{
	int i;

	printlog(1, "-> Loading obj file to trimesh: %s\n", file);

	trimesh *tmp;
	for (tmp=trimesh_head; tmp; tmp=tmp->next)
		if (!(strcmp(file, tmp->file)))
		{
			printlog(1, "   (already loaded)\n");
			return tmp;
		}

	FILE *fp_obj;
	FILE *fp_mtl = NULL;

	fp_obj = open_file(file);

	if (!fp_obj)
	{
		printlog(0, "ERROR opening file %s (doesn't exist?)\n", file);
		return NULL;
	}

	char **word;
	unsigned int vertices, normals, indices, materials, material_indices, modes;
	vertices=normals=indices=materials=material_indices=modes=0;

	const unsigned int MAX = -1; //overflow limit for unsigned int
	int last_f_number = 0;

	//first get ammount of vertices, indices etc. Set last found mtl file
	while ((word = get_word_list(fp_obj)))
	{
		if (vertices==MAX || normals==MAX || indices==MAX || material_indices==MAX) //bad for loading speed, but necessary to prevent overflow
		{
			printlog(0, "ERROR: OBJ was too bigg (overflows unsigned int)!\n");
			fclose (fp_obj);
			return NULL;
		}

		if (!strcmp(word[0], "v"))
			++vertices;
		else if (!strcmp(word[0], "vn"))
			++normals;
		else if (!strcmp(word[0], "f"))
		{
			for (i=1; word[i]; ++i);
			indices += (i-1);

			if (i!=last_f_number || i>4) //mode switch
				++modes;
		}
		else if (!strcmp(word[0], "usemtl"))
			++material_indices;
		else if (!strcmp(word[0], "mtllib"))
		{
			if (fp_mtl) //_new_ file request found(!), close this one
				fclose (fp_mtl);


			fp_mtl = open_file_rel_to_file (file, word[1]);

			if (!fp_mtl)
			{
				printlog(0, "ERROR: couldn't open file: %s\n", word[1]);
				fclose (fp_obj); //basic cleanup
				return NULL;
			}

			//free (mtl_path);
		}
		//else unrecognized!
		free_word_list(word);
	}

	if (!fp_mtl)
	{
		printlog(0, "ERROR: mtl file could not be opened!\n");
		fclose (fp_obj);
		return NULL;
	}

	//count materials
	while ((word = get_word_list(fp_mtl)))
	{
		if (!strcmp(word[0], "newmtl"))
			if (++materials == MAX)
			{
				printlog(0, "ERROR: too many materials (overflows unsigned int)!\n");
				fclose (fp_obj);
				fclose (fp_mtl);
				return NULL;
			}
		free_word_list(word);
	}

	//print counts
	printlog (1, "   (v:%u n:%u i:%u m:%u mi:%u Modes:%u)\n", vertices, normals,
			indices, materials, material_indices, modes);

	if (!(vertices&&normals&&indices&&materials))
	{
		printlog (0, "ERROR: vertices, normals, indices and materials must exist in files\n");
		fclose (fp_obj);
		fclose (fp_mtl);
		return NULL;
	}

	//allocate
	trimesh *mesh = allocate_trimesh(vertices, normals,
			indices, materials, material_indices, modes);

	char *material_names[materials]; //for tmp storing names


	//store filename (to prevent duplicated loading)
	mesh->file = (char*) calloc (strlen(file)+1, sizeof(char));
	strcpy (mesh->file, file);

	//start loading, first mtl
	fseek (fp_mtl, SEEK_SET, 0); //go to beginning

	i=-1;
	while ((word = get_word_list(fp_mtl)))
	{
		if (!strcmp(word[0], "newmtl"))
		{
			++i;
			//first store name for future lookups
			material_names[i] = (char*) calloc(strlen(word[1])+1, sizeof(char));
			strcpy(material_names[i], word[1]);

			//hmm... I might have forgotten something?
		}
		else if (i>=0)
		{
			//"shinines" of specular colour
			if (word[0][0] == 'N' &&word[0][1] == 's')
			{
				if (!(sscanf(word[1], "%f", &(mesh->materials+i)->shininess)))
					printlog(0, "WARNING: failed reading shinines %i\n", i);
			}

			//ambient colour
			else if (word[0][0] == 'K' &&word[0][1] == 'a')
			{
				if (sscanf(word[1], "%f", &(mesh->materials+i)->ambient[0])==1 &&
				    sscanf(word[2], "%f", &(mesh->materials+i)->ambient[1])==1 &&
				    sscanf(word[3], "%f", &(mesh->materials+i)->ambient[2])==1)
					(mesh->materials+i)->ambient[3]=1.0f;
				else
					printlog(0, "WARNING: failed reading ambient colour %i\n", i);
			}

			//diffuse colour
			else if (word[0][0] == 'K' &&word[0][1] == 'd')
			{
				if (sscanf(word[1], "%f", &(mesh->materials+i)->diffuse[0])==1 &&
				    sscanf(word[2], "%f", &(mesh->materials+i)->diffuse[1])==1 &&
				    sscanf(word[3], "%f", &(mesh->materials+i)->diffuse[2])==1)
					(mesh->materials+i)->diffuse[3]=1.0f;
				else
					printlog(0, "WARNING: failed reading diffuse colour %i\n", i);
			}

			//specular colour
			else if (word[0][0] == 'K' &&word[0][1] == 's')
			{
				if (sscanf(word[1], "%f", &(mesh->materials+i)->specular[0])==1 &&
				    sscanf(word[2], "%f", &(mesh->materials+i)->specular[1])==1 &&
				    sscanf(word[3], "%f", &(mesh->materials+i)->specular[2])==1)
					(mesh->materials+i)->specular[3]=1.0f;
				else
					printlog(0, "WARNING: failed reading specular colour %i\n", i);
			}
		}
		else
			printlog(0, "WARNING: ignoring %s\n", word[0]);
		free_word_list(word);
	}
	
	fclose (fp_mtl);

	//load obj
	//TODO: add resize option (preferably as float argument to function)
	fseek (fp_obj, SEEK_SET, 0); //go to beginning
	unsigned int v_count=0, n_count=0, i_count=0, inst_count=0, m_count=0, M_count=0;
	last_f_number = 0;
	GLfloat *vertex;
	while ((word = get_word_list(fp_obj)))
	{
		//ingore g and s

		//vector
		if (word[0][0]=='v' && word[0][1]=='\0')
		{
			vertex = &(mesh->vertices[v_count]);
			if (	sscanf(word[1], "%f", &vertex[0]) &&
				sscanf(word[2], "%f", &vertex[1]) &&
				sscanf(word[3], "%f", &vertex[2]))
			{
					v_count += 3;
					vertex[0]*=resize;
					vertex[1]*=resize;
					vertex[2]*=resize;
			}
			else
				printlog(0, "WARNING: failed reading vector %i\n", v_count);
		}
		//normal
		else if (word[0][0]=='v' && word[0][1]=='n' && word[0][2]=='\0')
		{
			vertex = &(mesh->normals[n_count]);
			if (	sscanf(word[1], "%f", &vertex[0]) &&
				sscanf(word[2], "%f", &vertex[1]) &&
				sscanf(word[3], "%f", &vertex[2]))
					n_count += 3;
			else
				printlog(0, "WARNING: failed reading normal %i\n", n_count);
		}
		//indices
		else if (word[0][0]=='f' && word[0][1]=='\0')
		{
			for (i=1; word[i]; ++i)
			{

				//even if obj specifies texture mapping, ignore it.
				if (	sscanf(word[i], "%i//%i/", &mesh->vector_indices[i_count], &mesh->normal_indices[i_count]) == 2 ||
					sscanf(word[i], "%i/%*i/%i/", &mesh->vector_indices[i_count], &mesh->normal_indices[i_count]) == 2)
				{
					mesh->vector_indices[i_count] -=1;
					mesh->normal_indices[i_count] -=1;
					++i_count;
				}
				else
					printlog(0, "WARNING: failed reading index %i\n", i_count);
			}

			--i;
			if (i!=last_f_number || i>4)
			{
				mesh->instructions[inst_count++]='M';

				GLenum new_mode;
				switch (i)
				{
					case 1:
						new_mode = GL_POINTS;
						break;
					case 2:
						new_mode = GL_LINES;
						break;
					case 3:
						new_mode = GL_TRIANGLES;
						break;
					case 4:
						new_mode = GL_QUADS;
						break;
					default:
						new_mode = GL_POLYGON;
						break;
				}

				mesh->modes[M_count++] = new_mode;
			}

			int j;
			for (j=0; j<i; ++j)
				mesh->instructions[inst_count++]='i';

		}

		//material
		else if (!strcmp(word[0], "usemtl"))
		{
			for (i=0; i<materials; ++i)
			{
				if (!(strcmp(material_names[i], word[1])))
					break;
			}
			if (i!=materials) //we found a match
			{
				//mesh->material_indices[m_count] = i;
				mesh->material_indices[0] = 0;
				mesh->instructions[inst_count++]='m';
			}
			else
				printlog(0, "WARNING: failed identify material %s\n", word[1]);
			++m_count;
		}
		free_word_list(word);
	}
	mesh->instructions[inst_count]='\0';
	
	fclose (fp_obj);

	//And we're done!
	for (i=0; i<materials; ++i)
		free (material_names[i]);

	return mesh;
}


//translate a trimesh (for rendering) to an ode trimesh geom (for collision)
dGeomID trimesh_to_geom (trimesh *mesh)
{
	printlog(1, "-> Translating trimesh to geometrical trimesh\n");
	//ode trimeshes only supports triangles ("triangle mesh"...), so indices for faces with more vertices (in mode) than 3 must be translated.
	//new indices are stored in a sepparate array in the trimesh.
	//NOTE: this could be used for rendering as well (but will require more memory, and possibly more rendering time?)
	int i;
	unsigned int i_count = 0;
	int v_count=0;
	GLenum *modes = mesh->modes;
	enum {none, triangles, quads, polygon} mode = none;
	for (i=0; mesh->instructions[i]; ++i)
	{
		if (mesh->instructions[i] == 'M')
		{
			if (*modes == GL_TRIANGLES)
				mode = triangles;
			else if (*modes == GL_QUADS)
				mode = quads;
			else if (*modes == GL_POLYGON)
				mode = polygon;

			v_count=0;
			++modes;
		}

		else if (mesh->instructions[i] == 'i')
		{
			++v_count;

			if (mode == triangles && v_count%3 == 0)
				++i_count;
			else if (mode == quads && v_count%4 == 0)
				i_count +=2;
			else if (mode == polygon && v_count >2)
				++i_count;
		}
	}
	i_count *= 3;
	mesh->geom_tri_indices = calloc (i_count, sizeof(dTriIndex));
	
	//translate
	i_count = 0;
	v_count = 0;
	int corners =0;
	mode = none;
	modes = mesh->modes;
	for (i=0; mesh->instructions[i]; ++i)
	{
		if (mesh->instructions[i] == 'M')
		{
			if (*modes == GL_TRIANGLES)
				mode = triangles;
			else if (*modes == GL_QUADS)
				mode = quads;
			else if (*modes == GL_POLYGON)
				mode = polygon;
			corners =0;
		}
		else if (mesh->instructions[i] == 'i')
		{
			if (mode == triangles)
				mesh->geom_tri_indices[i_count++] = mesh->vector_indices[v_count++];
			else if (mode == quads)
			{
				++corners;
				if (corners <= 3)
					mesh->geom_tri_indices[i_count++] = mesh->vector_indices[v_count++];
				else if (corners == 4)
				{
					mesh->geom_tri_indices[i_count++] = mesh->vector_indices[v_count-1];
					mesh->geom_tri_indices[i_count++] = mesh->vector_indices[v_count];
					mesh->geom_tri_indices[i_count++] = mesh->vector_indices[v_count-3];
					++v_count;
					corners = 0;
				}
			}
			//this ok?
			else if (mode == polygon)
			{
				++corners;
				if (corners <= 3)
					mesh->geom_tri_indices[i_count++] = mesh->vector_indices[v_count++];
				else
				{
					mesh->geom_tri_indices[i_count++] = mesh->vector_indices[v_count-2];
					mesh->geom_tri_indices[i_count++] = mesh->vector_indices[v_count-1];
					mesh->geom_tri_indices[i_count++] = mesh->vector_indices[v_count];
					++v_count;
				}
			}
		}
	}


	dTriMeshDataID data = dGeomTriMeshDataCreate();
	dGeomTriMeshDataBuildSingle (data, (void*) mesh->vertices, sizeof(float)*3, mesh->vertex_count, (void*) mesh->geom_tri_indices, i_count, sizeof(dTriIndex)*3);
	return dCreateTriMesh(0, data, 0,0,0);
}



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

	//load key list
	char *list=(char *)calloc(strlen(path)+9+1,sizeof(char));//+1 for \0
	strcpy (list,path);
	strcat (list,"/keys.lst");

	printlog(1, "-> loading key list: %s\n", list);
	FILE *fp;

	fp = open_file(list);

	if (!fp)
	{
		printlog(0, "ERROR opening file %s (doesn't exist?)\n", list);
		free (list);
		return NULL;
	}
	else
	{
		free (list);
#ifdef windows
		printlog(2, "(using binary read mode)\n");
#endif
		char **word;
		while ((word = get_word_list(fp)))
		{
		 printlog(2, " * action: %s\n", word[0]);

		 int i;
		 for (i=0; (profile_key_list[i].name != '\0'); ++i)
		  if (strcmp(profile_key_list[i].name,word[0]) == 0)
		  {
		   printlog(2, " * match found\n");
		   *(SDLKey*)((char*)prof+profile_key_list[i].offset) = get_key(word[1]);
		   break;
		  }

		 if (profile_key_list[i].name == '\0') //we reached end (no found)
			 printlog(0, "ERROR: no key action match: %s!\n",word[0]);

		 free_word_list(word);
		}
	}

	fclose(fp);

	printlog(1, "\n");
	return prof;
}


//load data for spawning object (object data), hard-coded debug version
//(objects are loaded as script instructions, executed for spawning)
script_struct *load_object(char *path)
{
	printlog(1, "-> Loading object: %s", path);

	script_struct *tmp = script_head;
	//see if already loaded
	while (tmp)
	{
		if (!strcmp(tmp->name, path))
		{
			printlog(1, " (already loaded)\n");
			return tmp;
		}
		tmp = tmp->next;
	}

	//new object
	script_struct *script;
	
	//currently no scripting, only hard-coded solutions
	if (!strcmp(path,"data/objects/misc/box"))
	{
		//"load" 3d box
		printlog(1, " (hard-coded box)\n");

		script = allocate_script();
		script->name = (char *)calloc(strlen(path) + 1, sizeof(char));
		strcpy (script->name, path);

		//the debug box will only spawn one component - one "3D file"
		//load box.obj (first build path)
		char obj[strlen(path) + strlen("/box.obj") + 1];
		strcpy (obj, path);
		strcat (obj, "/box.obj");

		if (!(script->tmp_trimesh1 = load_obj (obj, 0.5)))
			return NULL;

		script->box = true;
	}
	else if (!strcmp(path, "data/objects/misc/flipper"))
	{
		printlog(1, " (hard-coded flipper)\n");

		script = allocate_script();
		script->name = (char *)calloc(strlen(path) + 1, sizeof(char));
		strcpy (script->name, path);

		//load obj
		char obj[strlen(path) + strlen("/flipper.obj") + 1];
		strcpy (obj, path);
		strcat (obj, "/flipper.obj");

		script->tmp_trimesh1 = load_obj(obj, 1.0);

		if (!script->tmp_trimesh1)
			return NULL;

		script->flipper = true;
	}
	else if (!strcmp(path, "data/objects/misc/NH4"))
	{
		printlog(1, " (hard-coded \"molecule\")\n");

		script = allocate_script();
		script->name = (char *)calloc(strlen(path) + 1, sizeof(char));
		strcpy (script->name, path);

		char obj1[strlen(path) + strlen("/sphere1.obj") + 1];
		strcpy (obj1, path);
		strcat (obj1, "/sphere1.obj");

		if (!(script->tmp_trimesh1 = load_obj (obj1, 1.0))) //no resize
			return NULL;

		char obj2[strlen(path) + strlen("/sphere2.obj") + 1];
		strcpy (obj2, path);
		strcat (obj2, "/sphere2.obj");

		if (!(script->tmp_trimesh2 = load_obj (obj2, 1.0))) //no resize
			return NULL;

		script->NH4 = true;
	}
	else if (!strcmp(path, "data/objects/misc/sphere"))
	{
		printlog(1, " (hard-coded sphere)\n");

		script = allocate_script();
		script->name = (char *)calloc(strlen(path) + 1, sizeof(char));
		strcpy (script->name, path);

		char obj[strlen(path) + strlen("/sphere.obj") + 1];
		strcpy (obj, path);
		strcat (obj, "/sphere.obj");

		if (!(script->tmp_trimesh1 = load_obj (obj, 1.0))) //no resize
			return NULL;

		script->sphere = true;
	}
	else if (!strcmp(path, "data/objects/misc/building"))
	{
		printlog(1, " (hard-coded building)\n");

		//name
		script = allocate_script();
		script->name = (char *)calloc(strlen(path) + 1, sizeof(char));
		strcpy (script->name, path);

		//create graphics
		//pillars
		char obj[strlen(path) + strlen("/pillar.obj") + 1];
		strcpy (obj, path);
		strcat (obj, "/pillar.obj");

		if (!(script->tmp_trimesh3 = load_obj (obj, 1.0))) //no resize
			return NULL;

		//wallss
		char obj2[strlen(path) + strlen("/walls.obj") + 1];
		strcpy (obj2, path);
		strcat (obj2, "/wall.obj");

		if (!(script->tmp_trimesh1 = load_obj (obj2, 1.0))) //no resize
			return NULL;

		//roofs
		char obj3[strlen(path) + strlen("/roof.obj") + 1];
		strcpy (obj3, path);
		strcat (obj3, "/roof.obj");

		if (!(script->tmp_trimesh2 = load_obj (obj3, 1.0))) //no resize
			return NULL;

		script->building = true;
	}

	else
	{
		printlog(1, "\n");
		printlog(0, "ERROR: path didn't match any hard-coded object\n");
		script = NULL;
	}

	return script;
}

//bind two bodies together using fixed joint (simplify connection of many bodies)
void debug_joint_fixed(dBodyID body1, dBodyID body2, object_struct *obj)
{
	dJointID joint;
	joint = dJointCreateFixed (world, 0);
	dJointAttach (joint, body1, body2);
	dJointSetFixed (joint);

	//use feedback
	joint_data *data = (joint_data *)allocate_joint_data (joint, obj, true);
	data->threshold = 25000;
	data->buffer = 1000;
}

//spawn a "loaded" (actually hard-coded) object
//TODO: rotation
void spawn_object(script_struct *script, dReal x, dReal y, dReal z)
{
	printlog(1, "-> Spawning object at: %f %f %f", x,y,z);
	//prettend to be executing the script... just load debug values from
	//script structure
	//
	if (script->box)
	{
	printlog(1, " (hard-coded box)\n");
	//
	//
	//


	dGeomID geom  = dCreateBox (0, 1,1,1); //geom
	geom_data *data = allocate_geom_data(geom, NULL);
	dBodyID body = dBodyCreate (world);

	dMass m;
	dMassSetBox (&m,1,1,1,1); //sides
	dMassAdjust (&m,400); //mass
	dBodySetMass (body, &m);

	dGeomSetBody (geom, body);

	dBodySetPosition (body, x, y, z);

	//now add friction
	data->mu = 1;
	//use default
//	data->erp = 0.8;
//	data->cfm = 0.001;
//	data->slip1 = 0.0;
//	data->slip2 = 0.0;
//	data->bounce = 2.0;
	
	//Next, Graphics
	//data->file_3d = script->graphics_debug1;
	data->geom_trimesh = script->tmp_trimesh1;

	//done
	//
	//
	}
	//
	//
	else if (script->flipper)
	{
	printlog(1, " (hard-coded flipper)\n");
	//
	//
	//

	//flipper surface
	object_struct *obj = allocate_object(true, true);
	
	dGeomID geom  = dCreateBox (0, 8,8,0.5); //geom
	geom_data *data = allocate_geom_data(geom, obj);
	dGeomSetPosition (geom, x, y, z);

	//use default
//	data->mu = 1;
	data->erp = 0.8;
	data->cfm = 0.000;
//	data->slip1 = 0.0;
//	data->slip2 = 0.0;
//	data->bounce = 4.0;
	
	//Graphics
	data->geom_trimesh = script->tmp_trimesh1;


	//flipper sensor
	dGeomID geom2 = dCreateBox (0, 3,3,2);
	data = allocate_geom_data(geom2, obj);
	data->collide = false;
	dGeomSetPosition (geom2, x, y, z+0.76);

	data->flipper_geom = geom; //tmp debug solution

	//
	}
	//
	else if (script->NH4)
	{
	printlog(1, " (hard-coded \"molecule\")\n");
	//
	//
	//

	object_struct *obj = allocate_object(true, true);

	//center sphere
	dGeomID geom  = dCreateSphere (0, 1); //geom
	geom_data *data = allocate_geom_data(geom, obj);
	dBodyID body1 = dBodyCreate (world);

	dMass m;
	dMassSetSphere (&m,1,1); //radius
	dMassAdjust (&m,60); //mass
	dBodySetMass (body1, &m);

	dGeomSetBody (geom, body1);

	dBodySetPosition (body1, x, y, z);

	data->mu = 0;
	data->bounce = 1.5;
	
	//Next, Graphics
	//data->file_3d = script->graphics_debug1;
	data->geom_trimesh = script->tmp_trimesh1;

	dReal pos[4][3] = {
		{0, 0, 1.052},

		{0, 1, -0.326},
		{0.946,  -0.326, -0.326},
		{-0.946, -0.326, -0.326}};

	dJointID joint;
	dBodyID body;

	int i;
	for (i=0; i<4; ++i) {
	//connected spheres
	geom  = dCreateSphere (0, 0.8); //geom
	data = allocate_geom_data(geom, obj);
	body = dBodyCreate (world);

	dMassSetSphere (&m,1,0.5); //radius
	dMassAdjust (&m,30); //mass
	dBodySetMass (body, &m);

	dGeomSetBody (geom, body);

	dBodySetPosition (body, x+pos[i][0], y+pos[i][1], z+pos[i][2]);

	data->mu = 1;
	data->bounce = 2.0;
	
	//Next, Graphics
	data->geom_trimesh = script->tmp_trimesh2;

	//connect to main sphere
	
	joint = dJointCreateBall (world, obj->jointgroup);
	dJointAttach (joint, body1, body);
	dJointSetBallAnchor (joint, x+pos[i][0], y+pos[i][1], z+pos[i][2]);
	}
	//done
	//
	//
	}
	else if (script->sphere)
	{
	printlog(1, " (sphere)\n");
	//
	//
	//


	dGeomID geom  = dCreateSphere (0, 1); //geom
	geom_data *data = allocate_geom_data(geom, NULL);
	dBodyID body = dBodyCreate (world);

	dMass m;
	dMassSetSphere (&m,1,1); //radius
	dMassAdjust (&m,60); //mass
	dBodySetMass (body, &m);

	dGeomSetBody (geom, body);

	dBodySetPosition (body, x, y, z);

	data->mu = 1;
	data->bounce = 1.5;
	
	//Next, Graphics
	data->geom_trimesh = script->tmp_trimesh1;
	}
	//
	else if (script->building)
	{
	printlog(1, " (hard-coded building)\n");
	//
	//

	object_struct *obj = allocate_object(true, false); //no space (no geoms collide)
	dBodyID old_body[12] = {0,0,0,0,0,0,0,0,0,0,0,0};
	dBodyID old_pillar[4] = {0,0,0,0};

	dBodyID body[4];

	int j;
	for (j=0; j<2; ++j)
	{
		int i;
		dBodyID body1[12], body2[9];
		for (i=0; i<12; ++i)
		{
			dGeomID geom  = dCreateBox (0, 4,0.4,2.7); //geom
			geom_data *data = allocate_geom_data(geom, obj);
			data->mu = 1;

			body1[i] = dBodyCreate (world);
			dGeomSetBody (geom, body1[i]);

			dMass m;
			dMassSetBox (&m,1,4,0.4,2.7); //sides
			dMassAdjust (&m,400); //mass
			dBodySetMass (body1[i], &m);

			data->geom_trimesh = script->tmp_trimesh1;
		}
		
		const dReal k = 1.5*4+0.4/2;

		dBodySetPosition (body1[0], x-4, y-k, z+(2.7/2));
		dBodySetPosition (body1[1], x,   y-k, z+(2.7/2));
		dBodySetPosition (body1[2], x+4, y-k, z+(2.7/2));

		dBodySetPosition (body1[6], x+4, y+k, z+(2.7/2));
		dBodySetPosition (body1[7], x,   y+k, z+(2.7/2));
		dBodySetPosition (body1[8], x-4, y+k, z+(2.7/2));

		dMatrix3 rot;
		dRFromAxisAndAngle (rot, 0,0,1, M_PI/2);
		for (i=3; i<6; ++i)
			dBodySetRotation (body1[i], rot);
		for (i=9; i<12; ++i)
			dBodySetRotation (body1[i], rot);

		dBodySetPosition (body1[3], x+k,  y-4, z+(2.7/2));
		dBodySetPosition (body1[4], x+k, y, z+(2.7/2));
		dBodySetPosition (body1[5], x+k, y+4, z+(2.7/2));

		dBodySetPosition (body1[9], x-k, y+4, z+(2.7/2));
		dBodySetPosition (body1[10], x-k, y, z+(2.7/2));
		dBodySetPosition (body1[11], x-k, y-4, z+(2.7/2));

		//connect wall blocks in height
		for (i=0; i<12; ++i)
		{
			debug_joint_fixed(body1[i], old_body[i], obj);
			//move these bodies to list of old bodies
			old_body[i] = body1[i];
		}

		//connect wall blocks in sideway
		for (i=0; i<11; ++i)
			debug_joint_fixed (body1[i], body1[i+1], obj);
		debug_joint_fixed (body1[0], body1[11], obj);

		//walls done, floor/ceiling
		for (i=0; i<9; ++i)
		{
			dGeomID geom  = dCreateBox (0, 4,4,0.2); //geom
			geom_data *data = allocate_geom_data(geom, obj);
			data->mu = 1;

			body2[i] = dBodyCreate (world);
			dGeomSetBody (geom, body2[i]);

			dMass m;
			dMassSetBox (&m,1,4,4,0.2); //sides
			dMassAdjust (&m,400); //mass
			dBodySetMass (body2[i], &m);

			data->geom_trimesh = script->tmp_trimesh2;
		}

		const dReal k2=2.7-0.2/2;

		dBodySetPosition (body2[0], x-4, y-4, z+k2);
		debug_joint_fixed(body2[0], body1[0], obj);
		debug_joint_fixed(body2[0], body1[11], obj);
		dBodySetPosition (body2[1], x,   y-4, z+k2);
		debug_joint_fixed(body2[1], body1[1], obj);
		dBodySetPosition (body2[2], x+4, y-4, z+k2);
		debug_joint_fixed(body2[2], body1[2], obj);
		debug_joint_fixed(body2[2], body1[3], obj);

		dBodySetPosition (body2[3], x-4, y, z+k2);
		debug_joint_fixed(body2[3], body1[10], obj);
		dBodySetPosition (body2[4], x,   y, z+k2);
		dBodySetPosition (body2[5], x+4, y, z+k2);
		debug_joint_fixed(body2[5], body1[4], obj);

		dBodySetPosition (body2[6], x-4, y+4, z+k2);
		debug_joint_fixed(body2[6], body1[9], obj);
		debug_joint_fixed(body2[6], body1[8], obj);
		dBodySetPosition (body2[7], x,   y+4, z+k2);
		debug_joint_fixed(body2[7], body1[7], obj);
		dBodySetPosition (body2[8], x+4, y+4, z+k2);
		debug_joint_fixed(body2[8], body1[6], obj);
		debug_joint_fixed(body2[8], body1[5], obj);

		//join floor blocks
		//1: horisontal
		debug_joint_fixed (body2[0], body2[1], obj);
		debug_joint_fixed (body2[1], body2[2], obj);
		debug_joint_fixed (body2[3], body2[4], obj);
		debug_joint_fixed (body2[4], body2[5], obj);
		debug_joint_fixed (body2[6], body2[7], obj);
		debug_joint_fixed (body2[7], body2[8], obj);
		//2: vertical
		debug_joint_fixed (body2[0], body2[3], obj);
		debug_joint_fixed (body2[3], body2[6], obj);
		debug_joint_fixed (body2[1], body2[4], obj);
		debug_joint_fixed (body2[4], body2[7], obj);
		debug_joint_fixed (body2[2], body2[5], obj);
		debug_joint_fixed (body2[5], body2[8], obj);
	
		//pillars
		dGeomID geom;
		geom_data *data;
		for (i=0; i<4; ++i)
		{
			geom  = dCreateCapsule (0, 0.5,1.5); //geom
			data = allocate_geom_data(geom, obj);
			body[i] = dBodyCreate (world);
	
			dMass m;
			dMassSetCapsule (&m,1,3,1,0.5); //sides (3=z-axis)
			dMassAdjust (&m,400); //mass
			dBodySetMass (body[i], &m);
	
			dGeomSetBody (geom, body[i]);
	
			//friction
			data->mu = 1;
			//Next, Graphics
			data->geom_trimesh = script->tmp_trimesh3;
		}

		dBodySetPosition (body[0], x+2, y+2, z+2.5/2);
		debug_joint_fixed(body[0], body2[8], obj);
		debug_joint_fixed(body[0], body2[7], obj);
		debug_joint_fixed(body[0], body2[5], obj);
		debug_joint_fixed(body[0], body2[4], obj);

		dBodySetPosition (body[1], x+2, y-2, z+2.5/2);
		debug_joint_fixed(body[0], body2[1], obj);
		debug_joint_fixed(body[0], body2[2], obj);
		debug_joint_fixed(body[0], body2[4], obj);
		debug_joint_fixed(body[0], body2[5], obj);

		dBodySetPosition (body[2], x-2, y+2, z+2.5/2);
		debug_joint_fixed(body[0], body2[7], obj);
		debug_joint_fixed(body[0], body2[6], obj);
		debug_joint_fixed(body[0], body2[4], obj);
		debug_joint_fixed(body[0], body2[3], obj);

		dBodySetPosition (body[3], x-2, y-2, z+2.5/2);
		debug_joint_fixed(body[0], body2[0], obj);
		debug_joint_fixed(body[0], body2[1], obj);
		debug_joint_fixed(body[0], body2[3], obj);
		debug_joint_fixed(body[0], body2[4], obj);

		for (i=0; i<4; ++i)
		{
			debug_joint_fixed(body[i], old_pillar[i], obj);
			old_pillar[i] = body[i];
		}

		z+=2.7;
	}
	//
	//
	}
	//
	//
	else
		printlog(0, "\nERROR: trying to spawn unidentified object?!\n");

}

int load_track (char *path)
{
	printlog(1, "=> Loading track: %s\n", path);
	char *conf=(char *)calloc(strlen(path)+11+1,sizeof(char));//+1 for \0
	strcpy (conf,path);
	strcat (conf,"/track.conf");

	track.obj = NULL; //in order to know if string is allocated (and needs free)

	if (load_conf(conf, (char *)&track, track_index))
		return -1;

	free (conf);

	if (!track.obj)
	{
		printlog(0, "ERROR: track needs to specify obj file!\n");
		return -1;
	}

	//append forced data (lighting)
	track.position[3] = 0.0f; //directional
	track.ambient[3] = 1.0f; //a = 1.0f
	track.diffuse[3] = 1.0f; //-''-
	track.specular[3] = 1.0f; //-''-

	//all data loaded, start building
	glClearColor (track.sky[0],track.sky[1],track.sky[2],1.0f); //background
	
	glLightfv (GL_LIGHT0, GL_AMBIENT, track.ambient);
	glLightfv (GL_LIGHT0, GL_DIFFUSE, track.diffuse);
	glLightfv (GL_LIGHT0, GL_SPECULAR, track.specular);
	glLightfv (GL_LIGHT0, GL_POSITION, track.position);
	glEnable (GL_LIGHT0);
	glEnable (GL_LIGHTING);

	dWorldSetGravity (world,0,0,-track.gravity);

	//load track from obj (to rendering trimesh)
	char *obj=(char *)calloc(strlen(path)+strlen(track.obj) +2, sizeof(char));//+2 for \0 and /
	strcpy (obj,path);
	strcat (obj,"/");
	strcat (obj,track.obj);

	track.track_trimesh = load_obj (obj, track.obj_resize);
	free (obj);

	if (!track.track_trimesh)
		return -1;

	//tmp vars
	dGeomID geom;
	geom_data *data;

	//maybe just create space instead of "object"?
	track.object = allocate_object(true,false); //space + no jointgroup

	//translate rendering trimesh to collision geom
	geom = trimesh_to_geom (track.track_trimesh);
	data = allocate_geom_data(geom, track.object);
	data->mu = track.mu;
	data->slip = track.slip;
	data->erp = track.erp;
	data->cfm = track.cfm;

	//currently just create a plane for respawn level...
	geom = dCreatePlane (0, 0,0,1,track.respawn);
	data = allocate_geom_data(geom, track.object);
	data->mu = track.mu;
	data->slip = track.slip;
	data->erp = track.erp;
	data->cfm = track.cfm;
	
	//now lets load some objects!
	char *list=(char *)calloc(strlen(path)+12+1,sizeof(char));//+1 for \0
	strcpy (list,path);
	strcat (list,"/objects.lst");

	printlog(1, "-> Loading track object list: %s\n", path);
	FILE *fp;

	fp = open_file(list);

	if (!fp)
	{
		printlog(0, "ERROR opening file %s (doesn't exist?)\n", list);
		free (list);
		return -1;
	}
	else
	{
		free (list);
#ifdef windows
		printlog(2, "(using binary read mode)\n");
#endif
		char **w;
		w = get_word_list(fp);
		if (w[0][0]!='>'||w[1]=='\0')
		{
			printlog(0, "ERROR: expected first word to be load request (> object)\n");
			free_word_list(w);
			return -1;
		}
		else
		{
			script_struct *obj;
			char *obj_list = (char *)calloc(strlen(w[1])+13+1, sizeof(char));
			strcpy (obj_list,"data/objects/");
			strcat (obj_list,w[1]);

			obj = load_object(obj_list);
			dReal x,y,z;
			bool fail = false;

			if (!obj)
				return -1;

			while (!fail && (w = get_word_list(fp)))
			{
				if (w[0][0] == '>'&&w[1][0] != '\0')
				{
					obj_list = (char *)calloc(strlen(w[1])+13+1,
							sizeof(char));
					strcpy (obj_list,"data/objects/");
					strcat (obj_list,w[1]);

					obj = load_object(obj_list);

					if (!obj)
						fail = true;
				}
				//three words
				else if ( w[0][0]!='\0' && w[1][0]!='\0'
						&& w[2][0]!='\0')
				{
					if (!(sscanf(w[0], "%f", &x) == 1&&
						sscanf(w[1], "%f", &y) == 1&&
						sscanf(w[2], "%f", &z)))
					{
					 printlog(0, "ERROR: couldn't read position value!\n");
					 fail = true;
					}
					else
						spawn_object(obj, x, y, z);
				}
				else
				{
					printlog(0, "ERROR: unknown line!\n");
					fail = true;
				}
				free_word_list(w);
			}
			fclose (fp);
		}
	}
	//that's it!
	printlog(1, "\n");
	return 0;
}


car_struct *load_car (char *path)
{
	printlog(1, "=> Loading car: %s", path);

	//see if already loaded
	car_struct *tmp = car_head;
	while (tmp)
	{
		if (!strcmp(tmp->name, path))
		{
			printlog(1, " (already loaded)\n");
			return tmp;
		}
		tmp = tmp->next;
	}

	printlog(1, "\n");
	//apparently not
	car_struct *target = allocate_car();
	target->name = (char *)calloc(strlen(path) + 1, sizeof(char));
	strcpy (target->name, path);

	char *conf=(char *)calloc(strlen(path)+9+1,sizeof(char));//+1 for \0
	strcpy (conf,path);
	strcat (conf,"/car.conf");

	if (load_conf(conf, (char *)target, car_index))
		return NULL;

	free (conf);

	//graphics models
	int i;
	for (i=0; i<4; ++i)
	{
		char obj[strlen(path)+strlen(target->obj_wheel[i])+2];
		strcpy (obj, path);
		strcat (obj, "/");
		strcat (obj, target->obj_wheel[i]);

		target->wheel_trimesh[i] = load_obj (obj, 1.0);
	}
	char obj2[strlen(path)+strlen(target->obj_body)+2];
	strcpy (obj2, path);
	strcat (obj2, "/");
	strcat (obj2, target->obj_body);

	target->body_trimesh = load_obj (obj2, 1.0);

	printlog(1, "\n");
	return target;
}


void spawn_car(car_struct *target, dReal x, dReal y, dReal z)
{
	printlog(1, "-> spawning car at: %f %f %f\n", x,y,z);

	printlog(1, "Warning: wheels will not collide to other wheels... (wheels use cylinders)\n");
	printlog(1, "(note to self: only solution would be moving to capped cylinders... :-/ )\n");

	if (target->spawned)
	{
		printlog(0, "ERROR: trying to spawn already spawned car!\n");
		return;
	}

	printlog(1, "TODO: antigravity forces\n\n");

	//not spawned, continue
	target->spawned = true;

	//create object to store components and joints
	target->object = allocate_object(true,true); //activate space + jointgroup

	dMass m;
	target->bodyid = dBodyCreate (world);
	dBodySetAutoDisableFlag (target->bodyid, 0); //never disable main body
	

	//set up air (and liquid) drag for body
	body_data *odata;
	odata = allocate_body_data (target->bodyid, target->object);
	odata->use_drag = true;
	odata->drag[0] = target->body_drag[0];
	odata->drag[1] = target->body_drag[1];
	odata->drag[2] = target->body_drag[2];

	//rotational drag
	odata->use_rotation_drag = true;
	odata->rot_drag[0] = target->body_rotation_drag[0];
	odata->rot_drag[1] = target->body_rotation_drag[1];
	odata->rot_drag[2] = target->body_rotation_drag[2];

	dMassSetBox (&m,1,target->body[0], target->body[1], target->body[2]); //sides
	dMassAdjust (&m,target->body_mass); //mass
	dBodySetMass (target->bodyid, &m);


	dBodySetPosition (target->bodyid, x, y, z);

	//graphics (obj file)
	odata->body_trimesh = target->body_trimesh;

	//add geoms
	geom_data *gdata;
	dGeomID geom;

	int i;
	dReal *b;
	for (i=0;i<CAR_MAX_BOXES;++i)
		if (target->box[i][0])
		{
			b = target->box[i];
			geom = dCreateBox(0,b[0],b[1],b[2]);
			gdata = allocate_geom_data (geom, target->object);

			dGeomSetBody (geom, target->bodyid);

			if (b[3]||b[4]||b[5]) //need offset
				dGeomSetOffsetPosition(geom,b[3],b[4],b[5]);

			//friction
			gdata->mu = target->body_mu;
			gdata->slip = target->body_slip;
			gdata->erp = target->body_erp;
			gdata->cfm = target->body_cfm;
			//graphics
			//gdata->file_3d = target->box_graphics[i];


		}

	//side detection sensors:
	dReal *s = target->s;

	geom = dCreateBox(0,s[0],s[1],s[2]);
	target->sensor1 = allocate_geom_data (geom, target->object);
	target->sensor1->collide = false; //untouchable "ghost" geom - sensor
	dGeomSetBody (geom, target->bodyid);
	dGeomSetOffsetPosition(geom,0,0,-s[3]);

	geom = dCreateBox(0,s[0],s[1],s[2]);
	target->sensor2 = allocate_geom_data (geom, target->object);
	target->sensor2->collide = false; //sensor
	dGeomSetBody (geom, target->bodyid);
	dGeomSetOffsetPosition(geom,0,0,s[3]);

	//wheels:
	geom_data *wheel_data[4];
	dGeomID wheel_geom;
	dBodyID wheel_body[4];
	for (i=0;i<4;++i)
	{
		//create cylinder
		//(geom)
		wheel_geom = dCreateCylinder (0, target->w[0], target->w[1]);

		//(body)
		wheel_body[i] = dBodyCreate (world);
		//never disable wheel body
		dBodySetAutoDisableFlag (wheel_body[i], 0);

		//3=z axis of cylinder
		dMassSetCylinder (&m, 1, 3, target->w[0], target->w[1]);
		dMassAdjust (&m, target->wheel_mass);
		dBodySetMass (wheel_body[i], &m);

		dGeomSetBody (wheel_geom, wheel_body[i]);

		//allocate (geom) data
		wheel_data[i] = allocate_geom_data(wheel_geom, target->object);

		//friction
		wheel_data[i]->mu = target->wheel_mu;
		wheel_data[i]->wheel = true;
		wheel_data[i]->slip = target->wheel_slip;
		wheel_data[i]->bounce = target->wheel_bounce;

		//hardness
		wheel_data[i]->erp = target->wheel_erp;
		wheel_data[i]->cfm = target->wheel_cfm;


		//drag
		odata = allocate_body_data (wheel_body[i], target->object);
		odata->use_drag = true;
		odata->drag[0] = target->wheel_drag[0];
		odata->drag[1] = target->wheel_drag[1];
		odata->drag[2] = target->wheel_drag[2];

		//rotational drag
		odata->use_rotation_drag = true;
		odata->rot_drag[0] = target->wheel_rotation_drag[0];
		odata->rot_drag[1] = target->wheel_rotation_drag[1];
		odata->rot_drag[2] = target->wheel_rotation_drag[2];

		//graphics
		wheel_data[i]->geom_trimesh = target->wheel_trimesh[i];
		
		//(we need easy access to wheel body ids if using finite rotation)
		target->wheel_body[i] = wheel_body[i];
	}

	//place and rotate wheels
	dMatrix3 rot;
	dRFromAxisAndAngle (rot, 0, 1, 0, M_PI/2);
	dBodySetPosition (wheel_body[0], x+target->wp[0], y+target->wp[1], z);
	dBodySetRotation (wheel_body[0], rot);
	dBodySetPosition (wheel_body[1], x+target->wp[0], y-target->wp[1], z);
	dBodySetRotation (wheel_body[1], rot);

	dRFromAxisAndAngle (rot, 0, 1, 0, -M_PI/2);
	dBodySetPosition (wheel_body[2], x-target->wp[0], y-target->wp[1], z);
	dBodySetRotation (wheel_body[2], rot);
	dBodySetPosition (wheel_body[3], x-target->wp[0], y+target->wp[1], z);
	dBodySetRotation (wheel_body[3], rot);

	//enable finite rotation on rear wheels
	if (internal.finite_rotation)
	{
		dBodySetFiniteRotationMode (wheel_body[1], 1);
		dBodySetFiniteRotationMode (wheel_body[2], 1);
	}

	//create joints (hinge2) for wheels
	for (i=0; i<4; ++i)
	{
		target->joint[i]=dJointCreateHinge2 (world, target->object->jointgroup);
		//body is still body of car main body
		dJointAttach (target->joint[i], target->bodyid, wheel_body[i]);
		dJointSetHinge2Axis1 (target->joint[i],0,0,1);
		dJointSetHinge2Axis2 (target->joint[i],1,0,0);

		//setup suspension
		dJointSetHinge2Param (target->joint[i],dParamSuspensionERP,target->suspension_erp);
		dJointSetHinge2Param (target->joint[i],dParamSuspensionCFM,target->suspension_cfm);

		//lock steering axis on all wheels
		dJointSetHinge2Param (target->joint[i],dParamLoStop,0);
		dJointSetHinge2Param (target->joint[i],dParamHiStop,0);

		//to easily get rotation speed (for slip in sideway), set all geom datas to specify connected hinge2
		wheel_data[i]->hinge2 = target->joint[i];
	}

	//to make it possible to tweak the hinge2 anchor x position:
	
	dJointSetHinge2Anchor (target->joint[0],x+target->jx,y+target->wp[1],z);
	dJointSetHinge2Anchor (target->joint[1],x+target->jx,y-target->wp[1],z);
	dJointSetHinge2Anchor (target->joint[2],x-target->jx,y-target->wp[1],z);
	dJointSetHinge2Anchor (target->joint[3],x-target->jx,y+target->wp[1],z);

}


//functions for "unspawning"/removing objects (and cars)
//not used at the moment, might need some tweaking...

//removes an object
void remove_object(object_struct *target)
{
	//lets just hope the given pointer is ok...
	printlog(1, " > remove object");

	if (target->space)
	{
		printlog(1, " (space)");
		printlog(1, " TODO: loop through space...");
		dSpaceDestroy (target->space);
	}

	if (target->jointgroup)
	{
		printlog(1, " (joingroup)");
		dJointGroupDestroy (target->jointgroup);
	}

	/*
	if (target->cmp_count != 0)
	{
		printf("ERROR: got components, search&remove)\n");
		component *cmp = component_head;
		//loop through all components, see if any match
		while (cmp)
		{
			//only free, if geoms, they are destroyed when destroying space (above)
			if (cmp->object_parent == target)
				free_component (cmp);
			cmp=cmp->next;
		}
	}*/

	free_object (target);
	printlog(1, "\n");
}

void remove_car (car_struct* target)
{
	printlog(1, "removing car\n");
	remove_object (target->object);
	free_car (target);
}
