//different loaders for files (3D, conf, scripts...), cars, weapons and so on.
//also handles spawning of them, and removing
//(allocating and freeing is handled in shared.c)
//
//See main.c for licensing info


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

#ifdef windows
 fp = fopen(name, "rb");
#else
 fp = fopen(name, "r");
#endif

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
       argscan="%lf";
       argsize=sizeof(double);
      break;

      case 'b':
       argscan="%b";
       argsize=sizeof(bool);
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
	char *list=(char *)calloc(strlen(path)+9+1,sizeof(char));//+1 for \0
	strcpy (list,path);
	strcat (list,"/keys.lst");

	printlog(1, "-> loading key list: %s\n", list);
	FILE *fp;

#ifdef windows
	fp = fopen(list, "rb");
#else
	fp = fopen(list, "r");
#endif

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


//the following a some basic color definitions (used for lights and materials)
GLfloat black[]     = {0.0f, 0.0f, 0.0f, 1.0f}; // = nothing for lights
GLfloat dgray[]     = {0.2f, 0.2f, 0.2f, 1.0f};
GLfloat gray[]      = {0.5f, 0.5f, 0.5f, 1.0f};
GLfloat lgray[]     = {0.8f, 0.8f, 0.8f, 1.0f};
GLfloat white[]     = {1.0f, 1.0f, 1.0f, 1.0f};
GLfloat red[]       = {1.0f, 0.0f, 0.0f, 1.0f};
GLfloat green[]     = {0.0f, 1.0f, 0.0f, 1.0f};
GLfloat lgreen[]    = {0.4f, 1.0f, 0.4f, 1.0f};
GLfloat blue[]      = {0.0f, 0.0f, 1.0f, 1.0f};
GLfloat lblue[]     = {0.6f, 0.6f, 1.0f, 1.0f};
GLfloat yellow[]    = {1.0f, 1.0f, 0.0f, 1.0f};


void debug_draw_box (GLuint list, GLfloat x, GLfloat y, GLfloat z,
		GLfloat colour[], GLfloat specular[], GLint shininess)
{
	printlog(2, " > Creating rendering list for debug box\n");

	glNewList (list, GL_COMPILE);

	glMaterialfv (GL_FRONT, GL_AMBIENT_AND_DIFFUSE, colour);
	glMaterialfv (GL_FRONT, GL_SPECULAR, specular);
	glMateriali (GL_FRONT, GL_SHININESS, shininess);

	glBegin (GL_QUADS);
	glNormal3f (0.0f, 0.0f, 1.0f);
	glVertex3f (-(x/2.0f), -(y/2.0f), (z/2.0f));
	glVertex3f (-(x/2.0f), (y/2.0f), (z/2.0f));
	glVertex3f ((x/2.0f), (y/2.0f), (z/2.0f));
	glVertex3f ((x/2.0f), -(y/2.0f), (z/2.0f));

	glNormal3f (0.0f, 0.0f, -1.0f);
	glVertex3f (-(x/2.0f), -(y/2.0f), -(z/2.0f));
	glVertex3f (-(x/2.0f), (y/2.0f), -(z/2.0f));
	glVertex3f ((x/2.0f), (y/2.0f), -(z/2.0f));
	glVertex3f ((x/2.0f), -(y/2.0f), -(z/2.0f));

	glNormal3f (0.0f, -1.0f, 0.0f);
	glVertex3f (-(x/2.0f), -(y/2.0f), -(z/2.0f));
	glVertex3f (-(x/2.0f), -(y/2.0f), (z/2.0f));
	glVertex3f ((x/2.0f), -(y/2.0f), (z/2.0f));
	glVertex3f ((x/2.0f), -(y/2.0f), -(z/2.0f));

	glNormal3f (0.0f, 1.0f, 0.0f);
	glVertex3f (-(x/2.0f), (y/2.0f), -(z/2.0f));
	glVertex3f (-(x/2.0f), (y/2.0f), (z/2.0f));
	glVertex3f ((x/2.0f), (y/2.0f), (z/2.0f));
	glVertex3f ((x/2.0f), (y/2.0f), -(z/2.0f));

	glNormal3f (1.0f, 0.0f, 0.0f);
	glVertex3f ((x/2.0f), -(y/2.0f), -(z/2.0f));
	glVertex3f ((x/2.0f), -(y/2.0f), (z/2.0f));
	glVertex3f ((x/2.0f), (y/2.0f), (z/2.0f));
	glVertex3f ((x/2.0f), (y/2.0f), -(z/2.0f));

	glNormal3f (-1.0f, 0.0f, 0.0f);
	glVertex3f (-(x/2.0f), -(y/2.0f), -(z/2.0f));
	glVertex3f (-(x/2.0f), -(y/2.0f), (z/2.0f));
	glVertex3f (-(x/2.0f), (y/2.0f), (z/2.0f));
	glVertex3f (-(x/2.0f), (y/2.0f), -(z/2.0f));
	glEnd();

	glMaterialfv (GL_FRONT, GL_SPECULAR, black);

	glEndList();
}

void debug_draw_sphere_part(GLfloat x, GLfloat y, GLfloat z)
{
	glNormal3f (1.0f*x, -0.8f*y, 1.0f*z);
	glVertex3f (0.0f*x,  0.0f*y, 1.0f*z);
	glVertex3f (1.0f*x,  0.0f*y, 0.0f*z);
	glVertex3f (0.5f*x, -0.5f*y, 0.5f*z);

	glNormal3f (0.8f*x, -1.0f*y, 1.0f*z);
	glVertex3f (0.0f*x,  0.0f*y, 1.0f*z);
	glVertex3f (0.0f*x, -1.0f*y, 0.0f*z);
	glVertex3f (0.5f*x, -0.5f*y, 0.5f*z);

	glNormal3f (0.8f*x, -1.0f*y, 1.0f*z);
	glVertex3f (0.0f*x, -1.0f*y, 0.0f*z);
	glVertex3f (1.0f*x,  0.0f*y, 0.0f*z);
	glVertex3f (0.5f*x, -0.5f*y, 0.5f*z);
}

void debug_draw_sphere (GLuint list, GLfloat d, GLfloat colour[],
		GLfloat specular[], GLint shininess)
{
	printlog(2, " > Creating rendering list for debug sphere\n");
	GLfloat radius = d/2;

	glNewList (list, GL_COMPILE);
	glEnable(GL_NORMALIZE); //easier to specify normals

	glMaterialfv (GL_FRONT, GL_AMBIENT_AND_DIFFUSE, colour);
	glMaterialfv (GL_FRONT, GL_SPECULAR, specular);
	glMateriali (GL_FRONT, GL_SHININESS, shininess);

	glBegin (GL_TRIANGLES);
	debug_draw_sphere_part(-radius	,radius		,radius);
	debug_draw_sphere_part(radius	,radius		,radius);
	debug_draw_sphere_part(-radius	,-radius	,radius);
	debug_draw_sphere_part(radius	,-radius	,radius);
	debug_draw_sphere_part(-radius	,radius		,-radius);
	debug_draw_sphere_part(radius	,radius		,-radius);
	debug_draw_sphere_part(-radius	,-radius	,-radius);
	debug_draw_sphere_part(radius	,-radius	,-radius);
	glEnd();

	glMaterialfv (GL_FRONT, GL_SPECULAR, black);

	glDisable(GL_NORMALIZE);
	glEndList();
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
		script->graphics_debug1 = allocate_file_3d();
		debug_draw_box (script->graphics_debug1->list, 1,1,1, red,gray, 50);
		script->box = true;
	}
	else if (!strcmp(path, "data/objects/misc/flipper"))
	{
		printlog(1, " (hard-coded flipper)\n");

		script = allocate_script();
		script->name = (char *)calloc(strlen(path) + 1, sizeof(char));
		strcpy (script->name, path);

		script->graphics_debug1 = allocate_file_3d();
		script->graphics_debug2 = allocate_file_3d();

		debug_draw_box (script->graphics_debug1->list, 8,8,0.5, red,gray, 30);
		debug_draw_box (script->graphics_debug2->list, 3,3,2, lblue,black, 0);
		script->flipper = true;
	}
	else if (!strcmp(path, "data/objects/misc/NH4"))
	{
		printlog(1, " (hard-coded \"molecule\")\n");

		script = allocate_script();
		script->name = (char *)calloc(strlen(path) + 1, sizeof(char));
		strcpy (script->name, path);

		//draw approximate sphere
		script->graphics_debug1 = allocate_file_3d();
		debug_draw_sphere (script->graphics_debug1->list,2, lblue,white,42);
		script->graphics_debug2 = allocate_file_3d();
		debug_draw_sphere (script->graphics_debug2->list,1.6,white,white,42);

		script->NH4 = true;
	}
	else if (!strcmp(path, "data/objects/misc/sphere"))
	{
		printlog(1, " (hard-coded sphere)\n");

		script = allocate_script();
		script->name = (char *)calloc(strlen(path) + 1, sizeof(char));
		strcpy (script->name, path);

		//draw approximate sphere
		script->graphics_debug1 = allocate_file_3d();
		debug_draw_sphere (script->graphics_debug1->list,2, lblue,white,42);
//		script->graphics_debug2 = allocate_file_3d();
//		debug_draw_sphere (script->graphics_debug2->list,1.6,white,white,42);

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
		script->graphics_debug1 = allocate_file_3d(); //walls
		script->graphics_debug2 = allocate_file_3d(); //floor/ceiling
		script->graphics_debug3 = allocate_file_3d(); //pillars

		debug_draw_box (script->graphics_debug1->list, 4,0.4,2.7, dgray,black, 0);
		debug_draw_box (script->graphics_debug2->list, 4,4,0.2, lgray,gray, 30);

		glNewList (script->graphics_debug3->list, GL_COMPILE);

		glMaterialfv (GL_FRONT, GL_AMBIENT_AND_DIFFUSE, dgray);
		glMaterialfv (GL_FRONT, GL_SPECULAR, gray);
		glMateriali (GL_FRONT, GL_SHININESS, 30);

		glBegin (GL_QUAD_STRIP);
		float v;
		for (v=0; v<=2*M_PI; v+=2*M_PI/10)
		{
			glNormal3f (sin(v), cos(v), 0.0f);
			glVertex3f(sin(v)/2, cos(v)/2, 2.5/2.0f);
			glVertex3f(sin(v)/2, cos(v)/2, -2.5/2.0f);
		}
		glEnd();

		glMaterialfv (GL_FRONT, GL_SPECULAR, black);

		glEndList();

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

	allocate_body_data(body, NULL); //just for drag

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
	data->file_3d = script->graphics_debug1;

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
	data->file_3d = script->graphics_debug1;


	//flipper sensor
	dGeomID geom2 = dCreateBox (0, 3,3,2);
	data = allocate_geom_data(geom2, obj);
	data->collide = false;
	dGeomSetPosition (geom2, x, y, z+0.76);

	data->flipper_geom = geom; //tmp debug solution

	//graphics
	data->file_3d = script->graphics_debug2;
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

	allocate_body_data(body1, NULL);

	dGeomSetBody (geom, body1);

	dBodySetPosition (body1, x, y, z);

	data->mu = 0;
	data->bounce = 1.5;
	
	//Next, Graphics
	data->file_3d = script->graphics_debug1;

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

	allocate_body_data(body, NULL);

	dGeomSetBody (geom, body);

	dBodySetPosition (body, x+pos[i][0], y+pos[i][1], z+pos[i][2]);

	data->mu = 1;
	data->bounce = 2.0;
	
	//Next, Graphics
	data->file_3d = script->graphics_debug2;

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

	object_struct *obj = allocate_object(true, true);

	//center sphere
	dGeomID geom  = dCreateSphere (0, 1); //geom
	geom_data *data = allocate_geom_data(geom, obj);
	dBodyID body1 = dBodyCreate (world);

	dMass m;
	dMassSetSphere (&m,1,1); //radius
	dMassAdjust (&m,60); //mass
	dBodySetMass (body1, &m);

	allocate_body_data(body1, NULL);

	dGeomSetBody (geom, body1);

	dBodySetPosition (body1, x, y, z);

	data->mu = 1;
	data->bounce = 1.5;
	
	//Next, Graphics
	data->file_3d = script->graphics_debug1;
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

			allocate_body_data(body1[i], NULL);

			data->file_3d = script->graphics_debug1;
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

			allocate_body_data(body2[i], NULL);

			data->file_3d = script->graphics_debug2;
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
	
			allocate_body_data(body[i], NULL);

			dGeomSetBody (geom, body[i]);
	
			//friction
			data->mu = 1;
			//Next, Graphics
			data->file_3d = script->graphics_debug3;
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

	if (load_conf(conf, (char *)&track, track_index))
		return -1;

	free (conf);

	//set camera default values, some from track specs
	camera.pos[0] = track.cam_start[0];
	camera.pos[1] = track.cam_start[1];
	camera.pos[2] = track.cam_start[2];

	camera.t_pos[0] = track.target_start[0];
	camera.t_pos[1] = track.target_start[1];
	camera.t_pos[2] = track.target_start[2];

	camera.vel[0] = 0;
	camera.vel[1] = 0;
	camera.vel[2] = 0;

	camera.up[0] = 0;
	camera.up[1] = 0;
	camera.up[2] = 1;

	camera.air_timer = 0;
	camera.reverse = false;
	camera.in_air = false;

	//append forced data
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

	//(for now, use geoms to describe world)
	track.object = allocate_object(true,false); //space + no jointgroup

	//tmp vars
	dGeomID geom;
	geom_data *data;
	//ground plane
	geom = dCreatePlane (0, 0,0,1,0);
	data = allocate_geom_data(geom, track.object);
	data->mu = track.mu;
	data->slip = track.slip;
	data->erp = track.erp;
	data->cfm = track.cfm;

	//4 more planes as walls
	geom = dCreatePlane (0, 1,0,0,-100);
	data = allocate_geom_data(geom, track.object);
	data->mu = track.mu;
	data->slip = track.slip;
	data->erp = track.erp;
	data->cfm = track.cfm;

	geom = dCreatePlane (0, -1,0,0,-100);
	data = allocate_geom_data(geom, track.object);
	data->mu = track.mu;
	data->slip = track.slip;
	data->erp = track.erp;
	data->cfm = track.cfm;

	geom = dCreatePlane (0, 0,1,0,-100);
	data = allocate_geom_data(geom, track.object);
	data->mu = track.mu;
	data->slip = track.slip;
	data->erp = track.erp;
	data->cfm = track.cfm;

	geom = dCreatePlane (0, 0,-1,0,-100);
	data = allocate_geom_data(geom, track.object);
	data->mu = track.mu;
	data->slip = track.slip;
	data->erp = track.erp;
	data->cfm = track.cfm;


	//since a plane is a non-placeable geom, the sepparate components will
	//not be "rendered" separately, instead create one 3d image sepparately

	track.file_3d = allocate_file_3d();
	glNewList (track.file_3d->list, GL_COMPILE);
	//the ground and walls for the environment box
	glMaterialfv (GL_FRONT, GL_AMBIENT_AND_DIFFUSE, green);
	glNormal3f (0.0f, 0.0f, 1.0f);
	glBegin (GL_QUADS);
	glVertex3f (-100.0f, -100.0f, 0.0f);
	glVertex3f (-100.0f, 100.0f, 0.0f);
	glVertex3f (100.0f, 100.0f, 0.0f);
	glVertex3f (100.0f, -100.0f, 0.0f);
	glEnd();

	glMaterialfv (GL_FRONT, GL_AMBIENT_AND_DIFFUSE, gray);
	glBegin (GL_QUADS);
	glNormal3f (1.0f, 0.0f, 0.0f);
	glVertex3f (-100.0f, -100.0f, 0.0f);
	glVertex3f (-100.0f, -100.0f, 10.0f);
	glVertex3f (-100.0f, 100.0f, 10.0f);
	glVertex3f (-100.0f, 100.0f, 0.0f);

	glNormal3f (0.0f, -1.0f, 0.0f);
	glVertex3f (-100.0f, 100.0f, 0.0f);
	glVertex3f (-100.0f, 100.0f, 10.0f);
	glVertex3f (100.0f, 100.0f, 10.0f);
	glVertex3f (100.0f, 100.0f, 0.0f);

	glNormal3f (-1.0f, 0.0f, 0.0f);
	glVertex3f (100.0f, 100.0f, 0.0f);
	glVertex3f (100.0f, 100.0f, 10.0f);
	glVertex3f (100.0f, -100.0f, 10.0f);
	glVertex3f (100.0f, -100.0f, 0.0f);

	glNormal3f (0.0f, 1.0f, 0.0f);
	glVertex3f (100.0f, -100.0f, 0.0f);
	glVertex3f (100.0f, -100.0f, 10.0f);
	glVertex3f (-100.0f, -100.0f, 10.0f);
	glVertex3f (-100.0f, -100.0f, 0.0f);
	glEnd();

	glEndList();

	//temp solution, ramp
	geom = dCreateBox (0,8,12,1);
	data = allocate_geom_data(geom, track.object);

	dMatrix3 rot;
	dRFromAxisAndAngle (rot, 1, 0, 0, 0.3);
	dGeomSetPosition (geom, 0, 3, 1.5);
	dGeomSetRotation (geom, rot);
	
	data->mu = track.mu;
	data->slip = track.slip;
	data->erp = track.erp;
	data->cfm = track.cfm;

	//render box using built in
	data->file_3d = allocate_file_3d();
	debug_draw_box (data->file_3d->list, 8,12,1, gray, black, 0);


	//now lets load some objects!
	char *list=(char *)calloc(strlen(path)+12+1,sizeof(char));//+1 for \0
	strcpy (list,path);
	strcat (list,"/objects.lst");

	printlog(1, "-> Loading track object list: %s\n", path);
	FILE *fp;

#ifdef windows
	fp = fopen(list, "rb");
#else
	fp = fopen(list, "r");
#endif

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

	//set up values for front/rear driving ratios
	if (target->steer_ratio>100 || target->steer_ratio<0 )
		printlog(0, "ERROR: front/rear steering ratio should be set between 0 and 100!\n");
	target->fsteer = (dReal) (target->steer_ratio/100.0);
	target->rsteer = (dReal) (target->fsteer-1.0);
	
	if (target->motor_ratio>100 || target->motor_ratio<0 )
		printlog(0, "ERROR: front/rear motor ratio should be set between 0 and 100!\n");
	target->fmotor = (dReal) (target->motor_ratio/100.0);
	target->rmotor = (dReal) (1.0-target->fmotor);

	if (target->break_ratio>100 || target->break_ratio<0 )
		printlog(0, "ERROR: front/rear breaking ratio should be set between 0 and 100!\n");
	target->fbreak = (dReal) (target->break_ratio/100.0);
	target->rbreak = (dReal) (1.0-target->fbreak);

	//graphics models
	float w_r = target->w[0];
	float w_w = target->w[1];
	//wheels:
	//(note: wheel axis is along z)
	target->wheel_graphics = allocate_file_3d();
	glNewList (target->wheel_graphics->list, GL_COMPILE);
	//tyre
	glMaterialfv (GL_FRONT, GL_AMBIENT_AND_DIFFUSE, black);
	glMaterialfv (GL_FRONT, GL_SPECULAR, dgray);
	glMateriali (GL_FRONT, GL_SHININESS, 30);

	glBegin (GL_QUAD_STRIP);
	float v;
	for (v=0; v<=2*M_PI; v+=2*M_PI/10)
	{
		glNormal3f (sin(v), cos(v), 0.0f);
		glVertex3f(w_r*sin(v), w_r*cos(v), -w_w/2.0f);
		glVertex3f(w_r*sin(v), w_r*cos(v), w_w/2.0f);
	}

	glMaterialfv (GL_FRONT, GL_SPECULAR, black);

	glEnd();
	//rim
	glMaterialfv (GL_FRONT, GL_AMBIENT_AND_DIFFUSE, lgray);
	glNormal3f (0.0f, 0.0f, 1.0f);
	glBegin (GL_QUADS);
		glVertex3f(w_r*0.9f, w_r/5, w_w/3.0f);
		glVertex3f(w_r*0.9f, -w_r/5, w_w/3.0f);
		glVertex3f(-w_r*0.9f, -w_r/5, w_w/3.0f);
		glVertex3f(-w_r*0.9f, w_r/5, w_w/3.0f);

		glVertex3f(w_r/5, w_r*0.9f, w_w/3.0f);
		glVertex3f(w_r/5, -w_r*0.9f, w_w/3.0f);
		glVertex3f(-w_r/5, -w_r*0.9f, w_w/3.0f);
		glVertex3f(-w_r/5, w_r*0.9f, w_w/3.0f);
	glEnd();

	glEndList();

	//loop through possible body geoms and make a model for them
	int i;
	for (i=0;i<CAR_MAX_BOXES;++i)
		if (target->box[i][0])
		{
			target->box_graphics[i] = allocate_file_3d();

			GLfloat *b = target->box[i];
			if (i==0)//first box
				debug_draw_box(target->box_graphics[i]->list,
						b[0],b[1],b[2], yellow, gray, 70);
			else
				debug_draw_box(target->box_graphics[i]->list,
						b[0],b[1],b[2], lgreen, gray, 70);
		}

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
		//TODO: separate car into car_data and car_spawned to make one car spawnable for several players...
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
	

	//set mass
	dMassSetBox (&m,1,target->body[0], target->body[1], target->body[2]); //sides
	dMassAdjust (&m,target->body_mass); //mass
	dBodySetMass (target->bodyid, &m);

	//set up air (and liquid) drag for body
	body_data *odata;
	odata = allocate_body_data (target->bodyid, target->object);
	Body_Data_Set_Advanced_Linear_Drag (odata, target->body_linear_drag[0],
			target->body_linear_drag[1], target->body_linear_drag[2]);
	//rotational drag
	Body_Data_Set_Angular_Drag ( odata, target->body_angular_drag);


	dBodySetPosition (target->bodyid, x, y, z);

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
			gdata->file_3d = target->box_graphics[i];


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
		wheel_data[i]->mu_rim = target->rim_mu;
		wheel_data[i]->wheel = true;
		wheel_data[i]->slip = target->wheel_slip;
		wheel_data[i]->bounce = target->wheel_bounce;

		//hardness
		wheel_data[i]->erp = target->wheel_erp;
		wheel_data[i]->cfm = target->wheel_cfm;


		//drag
		odata = allocate_body_data (wheel_body[i], target->object);
		Body_Data_Set_Linear_Drag (odata, target->wheel_linear_drag);
		//rotational drag
		Body_Data_Set_Angular_Drag (odata, target->wheel_angular_drag);

		//graphics
		wheel_data[i]->file_3d = target->wheel_graphics;
		
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
	/*if (internal.finite_rotation)
	{
		dBodySetFiniteRotationMode (wheel_body[1], 1);
		dBodySetFiniteRotationMode (wheel_body[2], 1);
	}*/

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
