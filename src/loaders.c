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

	char *word = calloc(l+1, sizeof(char));

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
		char **word = calloc(MAX_WORDS, sizeof(char*));
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
int load_conf (char *name, void *memory, data_index index)
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
  char *argscan;
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
	char *conf=calloc(strlen(path)+13+1,sizeof(char));//+1 for \0
	strcpy (conf,path);
	strcat (conf,"/profile.conf");

	if (load_conf(conf, prof, profile_index))
		return NULL;

	free (conf);

	//load key list
	char *list=calloc(strlen(path)+9+1,sizeof(char));//+1 for \0
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
		   *(SDLKey*)((void*)prof+profile_key_list[i].offset) = get_key(word[1]);
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
	glVertex3f ((x/2.0f), -(y/2.0f), (z/2.0f));
	glVertex3f ((x/2.0f), (y/2.0f), (z/2.0f));
	glVertex3f (-(x/2.0f), (y/2.0f), (z/2.0f));
	glVertex3f (-(x/2.0f), -(y/2.0f), (z/2.0f));

	glNormal3f (0.0f, 0.0f, -1.0f);
	glVertex3f (-(x/2.0f), -(y/2.0f), -(z/2.0f));
	glVertex3f (-(x/2.0f), (y/2.0f), -(z/2.0f));
	glVertex3f ((x/2.0f), (y/2.0f), -(z/2.0f));
	glVertex3f ((x/2.0f), -(y/2.0f), -(z/2.0f));

	glNormal3f (0.0f, -1.0f, 0.0f);
	glVertex3f ((x/2.0f), -(y/2.0f), -(z/2.0f));
	glVertex3f ((x/2.0f), -(y/2.0f), (z/2.0f));
	glVertex3f (-(x/2.0f), -(y/2.0f), (z/2.0f));
	glVertex3f (-(x/2.0f), -(y/2.0f), -(z/2.0f));

	glNormal3f (0.0f, 1.0f, 0.0f);
	glVertex3f (-(x/2.0f), (y/2.0f), -(z/2.0f));
	glVertex3f (-(x/2.0f), (y/2.0f), (z/2.0f));
	glVertex3f ((x/2.0f), (y/2.0f), (z/2.0f));
	glVertex3f ((x/2.0f), (y/2.0f), -(z/2.0f));

	glNormal3f (1.0f, 0.0f, 0.0f);
	glVertex3f ((x/2.0f), (y/2.0f), -(z/2.0f));
	glVertex3f ((x/2.0f), (y/2.0f), (z/2.0f));
	glVertex3f ((x/2.0f), -(y/2.0f), (z/2.0f));
	glVertex3f ((x/2.0f), -(y/2.0f), -(z/2.0f));

	glNormal3f (-1.0f, 0.0f, 0.0f);
	glVertex3f (-(x/2.0f), -(y/2.0f), -(z/2.0f));
	glVertex3f (-(x/2.0f), -(y/2.0f), (z/2.0f));
	glVertex3f (-(x/2.0f), (y/2.0f), (z/2.0f));
	glVertex3f (-(x/2.0f), (y/2.0f), -(z/2.0f));
	glEnd();

	glMaterialfv (GL_FRONT, GL_SPECULAR, black);

	glEndList();
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

	// normals still not perfect, but then it's an imperfect algorithm
	int slices = 10;
	int divisions = 10;
	int i,j,ii,jj;

	float slp = M_PI/slices;
	float sld = 2.0*M_PI/divisions;
	float dx,dy,dz;

	for (i = 0; i < slices; i++ ) {
		ii = i + 1;
		glBegin (GL_TRIANGLE_STRIP);
		for (j = 0; j <= divisions; j++ ) {
			jj = j + 1;
			
			dx = sin(slp * i) * sin (sld * j);
			dy = sin(slp * i) * cos (sld * j);
			dz = -cos(slp*i);
			//printf("dx:%f dy:%f dz:%f\n", dx, dy, dz);
			glNormal3f(dx, dy, dz);
			glVertex3f( radius * dx, radius * dy, radius * dz );
			
			dx = sin(slp * ii) * sin (sld * jj);
			dy = sin(slp * ii) * cos (sld * jj);
			dz = -cos(slp*ii);
			//printf("2dx:%f dy:%f dz:%f\n", dx, dy, dz);
			glNormal3f(dx, dy, dz);
			glVertex3f( radius * dx, radius * dy, radius * dz );
			
		}
		glEnd();
	}
	

	glMaterialfv (GL_FRONT, GL_SPECULAR, black);

	glDisable(GL_NORMALIZE);
	glEndList();
}

//load data for spawning object (object data), hard-coded debug version
//(objects are loaded as script instructions, executed for spawning)
script *load_object(char *path)
{
	printlog(1, "-> Loading object: %s", path);

	script *tmp = script_head;
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
	script *script;
	
	//currently no scripting, only hard-coded solutions
	if (!strcmp(path,"data/objects/misc/box"))
	{
		//"load" 3d box
		printlog(1, " (hard-coded box)\n");

		script = allocate_script();
		script->name = calloc(strlen(path) + 1, sizeof(char));
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
		script->name = calloc(strlen(path) + 1, sizeof(char));
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
		script->name = calloc(strlen(path) + 1, sizeof(char));
		strcpy (script->name, path);

		//draw approximate sphere
		script->graphics_debug1 = allocate_file_3d();
		debug_draw_sphere (script->graphics_debug1->list,2, lblue,white,42);
		script->graphics_debug2 = allocate_file_3d();
		debug_draw_sphere (script->graphics_debug2->list,1.6,white,white,42);

		script->NH4 = true;
	}
	else if (!strcmp(path, "data/objects/misc/building"))
	{
		printlog(1, " (hard-coded building)\n");

		//name
		script = allocate_script();
		script->name = calloc(strlen(path) + 1, sizeof(char));
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
void debug_joint_fixed(dBodyID body1, dBodyID body2, object *obj)
{
	dJointID joint;
	joint = dJointCreateFixed (world, 0);
	dJointAttach (joint, body1, body2);
	dJointSetFixed (joint);

	//use feedback
	joint_data *data = allocate_joint_data (joint, obj, true);
	data->threshold = 25000;
	data->buffer = 1000;
}

//spawn a "loaded" (actually hard-coded) object
//TODO: rotation
void spawn_object(script *script, dReal x, dReal y, dReal z)
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
	object *obj = allocate_object(true, true);
	
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

	object *obj = allocate_object(true, true);

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
	//
	else if (script->building)
	{
	printlog(1, " (hard-coded building)\n");
	//
	//

	object *obj = allocate_object(true, false); //no space (no geoms collide)
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





float *mbv(float *m, float x, float y, float z) {
	static float v[3];
	v[0] = x*m[0] + y*m[4] + z*m[8] + m[12];
	v[1] = x*m[1] + y*m[5] + z*m[9] + m[13];
	v[2] = x*m[2] + y*m[6] + z*m[10] + m[14];
	
	return (float *)&v;
}


void makeTurdMatricies( struct turd_struct *tmp_turd ) {
	float *mvr;
	
	// store current matrix
	glGetFloatv(GL_MODELVIEW_MATRIX,tmp_turd->m);
	
	// real world coords
	mvr = mbv(tmp_turd->m, 0,0,0);
	tmp_turd->wx = mvr[0];
	tmp_turd->wy = mvr[1];
	tmp_turd->wz = mvr[2];

	// direction of travel (y-axis)
	mvr = mbv(tmp_turd->m, 0,1,0);
	tmp_turd->nx = mvr[0];
	tmp_turd->ny = mvr[1];
	tmp_turd->nz = mvr[2];
	
	// and the actual normal
	mvr = mbv(tmp_turd->m, 0,0,1);
	tmp_turd->anx = mvr[0] - tmp_turd->wx;
	tmp_turd->any = mvr[1] - tmp_turd->wy;
	tmp_turd->anz = mvr[2] - tmp_turd->wz;
}

void setupTurdValues( struct turd_struct *tmp_turd, float x,float y,float z, float a,float b,float c ) {
	tmp_turd->x = x;
	tmp_turd->y = y;
	tmp_turd->z = z;
	tmp_turd->a = a;
	tmp_turd->b = b;
	tmp_turd->c = c;
}


void calcTurd( struct turd_struct *t ) {
	glPushMatrix();		
	glMatrixMode(GL_MODELVIEW);
	
	// go through entire list, using gl to process the relational
	// offsets, and store the matrix for each node, which we'll
	// use later. It would perhaps be nicer to use our own matrix
	// math library, rather than invoking gl, but i don't know how
	// to do that, and this works for now.
	//
	while ( t ) {
		glTranslatef(t->x,t->y,t->z);
		glRotatef(t->a,1,0,0);
		glRotatef(t->b,0,1,0);
		glRotatef(t->c,0,0,1);
		makeTurdMatricies( t );
		
		glPushMatrix();
		glTranslatef(t->l->x, t->l->y, t->l->z);
		glRotatef(t->l->a,1,0,0);
		glRotatef(t->l->b,0,1,0);
		glRotatef(t->l->c,0,0,1);
		makeTurdMatricies( t->l );
		glPopMatrix();		
				
		glPushMatrix();
		glTranslatef(t->r->x, t->r->y, t->r->z);
		glRotatef(t->r->a,1,0,0);
		glRotatef(t->r->b,0,1,0);
		glRotatef(t->r->c,0,0,1);
		makeTurdMatricies( t->r );
		glPopMatrix();
			
		t = t->nxt;
	}
	glPopMatrix();
}



// this _so_ should be a generic loader
turd_struct *loadTurd(char *filename) {
	FILE *fp;
	char buf[100];
	void *ptr;

#ifdef windows
	fp = fopen(filename, "rb");
#else
	fp = fopen(filename, "r");
#endif


	float x,y,z,a,b,c;

  int first = 1;
	struct turd_struct *tmp_turd = NULL;
	struct turd_struct *head_turd = NULL;
	struct turd_struct *last_turd = NULL;
	struct turd_struct *bast_turd = NULL;

	float mod=15;
	float xmod=10;
	

	int count = 0;
	
	while ( (ptr = fgets((char *)&buf, 100, fp)) ) {
		count++;
		sscanf(buf, "%f %f %f %f %f %f", &x, &y, &z, &a, &b, &c);
			
		tmp_turd = malloc(sizeof(turd_struct));

		x *= mod;
		y *= mod;
		z *= mod;

		setupTurdValues(tmp_turd, x,y,z, a,b,c);
		
		// left and right side of road are offset from center
		bast_turd = malloc(sizeof(turd_struct));
		setupTurdValues( bast_turd, -xmod,0,0, 0,0,0 );
		bast_turd->r = tmp_turd;
		tmp_turd->l = bast_turd;
		
		bast_turd = malloc(sizeof(turd_struct));
		setupTurdValues( bast_turd, xmod,0,0, 0,0,0 );
		bast_turd->l = tmp_turd;
		tmp_turd->r = bast_turd;

		if (first == 1) {
			first = 0;
			head_turd = tmp_turd;
		}

		if (last_turd != NULL) {
			last_turd->nxt = tmp_turd;
			tmp_turd->pre = last_turd;
			
			last_turd->l->nxt = tmp_turd->l;
			last_turd->r->nxt = tmp_turd->r;
			tmp_turd->l->pre = last_turd->l;
			tmp_turd->r->pre = last_turd->r;
		}
		
		
		last_turd = tmp_turd;
	}
	
	fclose(fp);

	calcTurd( head_turd );
	
	/*
	// yeach - a holder for the global list
	tmp_turd = malloc(sizeof(turd_struct));
	*/
	if ( turd_head == NULL ) {
		// i kan coed gud
		turd_head = head_turd;
		edit_t = head_turd;
		edit_h = head_turd;
	}
	/*
	tmp_turd->nxt = head_turd;
	tmp_turd->
	turd_head
	*/
	


	return head_turd;
}



// inner product of two vectors
float dot(dVector3 u, dVector3 v) {
	return u[0] * v[0] + u[1] * v[1] + u[2] * v[2];
}

// creates an interpolation object from two control points
interp_struct *interpInit( interp_struct *in, turd_struct *cur_turd, turd_struct *nxt_turd ) {
		
		in->ps0x = cur_turd->wx;
		in->ps0y = cur_turd->wy;
		in->ps0z = cur_turd->wz;
		in->ps1x = cur_turd->nx;
		in->ps1y = cur_turd->ny;
		in->ps1z = cur_turd->nz;

		in->pe0x = nxt_turd->wx;
		in->pe0y = nxt_turd->wy;
		in->pe0z = nxt_turd->wz;
		in->pe1x = nxt_turd->nx;
		in->pe1y = nxt_turd->ny;
		in->pe1z = nxt_turd->nz;
		
		return in;
}

// Following function derived from:
// http://www.softsurfer.com/Archive/algorithm_0106/algorithm_0106.htm#dist3D_Line_to_Line()
//
// Copyright 2001, softSurfer (www.softsurfer.com)
// This code may be freely used and modified for any purpose
// providing that this copyright notice is included with it.
// SoftSurfer makes no warranty for this code, and cannot be held
// liable for any real or imagined damage resulting from its use.
// Users of this code must verify correctness for their application.
void interpGenClosestLine( interp_struct *in ) {
		float sc, tc;
		dVector3 u;
		dVector3 v;
		dVector3 w;	
		
		u[0] = in->ps1x - in->ps0x;
		u[1] = in->ps1y - in->ps0y;
		u[2] = in->ps1z - in->ps0z;
		
		v[0] = in->pe1x - in->pe0x;
		v[1] = in->pe1y - in->pe0y;
		v[2] = in->pe1z - in->pe0z;
		
		w[0] = in->ps0x - in->pe0x;
		w[1] = in->ps0y - in->pe0y;
		w[2] = in->ps0z - in->pe0z;
		
		float ca = dot(u,u);
		float cb = dot(u,v);
		float cc = dot(v,v);
		float cd = dot(u,w);
		float ce = dot(v,w);
		float cD = ca*cc - cb*cb;
		
		if (cD < 0.01) {
			// if almost parallel, then we want to return the midpoint
			if ( 1 ) {
				//printf("parallel\n");
				in->scx = in->ps0x - (w[0] / 2.0);
				in->scy = in->ps0y - (w[1] / 2.0);
				in->scz = in->ps0z - (w[2] / 2.0);
				
				in->tcx = in->scx;
				in->tcy = in->scy;
				in->tcz = in->scz;
			} else {
				sc = 0;
				tc = (cb>cc ? cd/cb : ce/cc);
			}
		} else {
			sc = (cb*ce - cc*cd) / cD;
			tc = (ca*ce - cb*cd) / cD;
			
			// now the line should be defined by ( (ps1-ps0) * sc, (pe1-pe0) * tc )
			in->scx = in->ps0x + ((in->ps1x - in->ps0x) * sc);
			in->scy = in->ps0y + ((in->ps1y - in->ps0y) * sc);
			in->scz = in->ps0z + ((in->ps1z - in->ps0z) * sc);
		
			in->tcx = in->pe0x + ((in->pe1x - in->pe0x) * tc);
			in->tcy = in->pe0y + ((in->pe1y - in->pe0y) * tc);
			in->tcz = in->pe0z + ((in->pe1z - in->pe0z) * tc);
		}
	

}


// inputs an interpolation struct, with a desired scale (t = 0->1), and populates
// point 'p'
//
// cp[xyz] is a moving control point, which moves (with t) along the 3d line
//   represented by the closest points the of the y-axis of each interpolation
//   object (sc[xyz]->tc[xyz]
//
// sp[xyz] is the control point which moves (with t) from the starting
//   point to cp[xyz]
//
// ep[xyz] moves from cp[xyz] to the end point
//
// The point p is derived from the line sp[xyz]->ep[xyz], scaled by t.
//
// Once sc[xyz]->tc[xyz] has been discovered, interpolation is equally cheap
// for any value of 't'.
//
void interpDraw( interp_struct *in, float t, float *p ) {
	in->cpx = in->scx + t * (in->tcx - in->scx);
	in->cpy = in->scy + t * (in->tcy - in->scy);
	in->cpz = in->scz + t * (in->tcz - in->scz);
	
	in->spx = in->ps0x + t * (in->cpx - in->ps0x);
	in->spy = in->ps0y + t * (in->cpy - in->ps0y);
	in->spz = in->ps0z + t * (in->cpz - in->ps0z);
	
	in->epx = in->cpx + t * (in->pe0x - in->cpx);
	in->epy = in->cpy + t * (in->pe0y - in->cpy);
	in->epz = in->cpz + t * (in->pe0z - in->cpz);
	
	p[0] = in->spx + t * (in->epx - in->spx);
	p[1] = in->spy + t * (in->epy - in->spy);
	p[2] = in->spz + t * (in->epz - in->spz);
}




trimesh_struct *calcTrimesh(struct turd_struct *head) {
	struct turd_struct *cur_turd = head;
	struct turd_struct *nxt_turd;
	struct turd_struct *lct,*lnt, *rct,*rnt;
	int i;	
	float t;
	
	if ( head->tri ) {
		// clear out old data
		//dGeomTriMeshDataDestroy(head->tri->dataid);
		//dSpaceRemove(space, head->tri->meshid);
		
		// is this automatic?
		free(head->tri->ode_verts);
		free(head->tri->ode_indices);
	} else {
		head->tri = malloc(sizeof(trimesh_struct));
		head->tri->dataid = dGeomTriMeshDataCreate();
		head->tri->meshid = dCreateTriMesh(NULL, head->tri->dataid, NULL, NULL, NULL);
		
		geom_data *data = allocate_geom_data(head->tri->meshid, track.object);
		data->mu = track.mu;
		data->slip = track.slip;
		data->erp = track.erp;
		data->cfm = track.cfm;
		data->collide=1;
	}

	struct trimesh_struct *tri = head->tri;

	dReal *ode_verts = tri->ode_verts;
	unsigned int *ode_indices = tri->ode_indices;
	
	
	float ls[3];
	float cs[3];
	float rs[3];

	interp_struct lin;
	interp_struct cin;
	interp_struct rin;
	
	int num=10;
	int t_count=0;
	int v_count=0;
	int i_count=0;

	cur_turd = head;
	while (cur_turd) {	
		cur_turd = cur_turd->nxt;
		t_count++;
	}
		
	// allocate memory
	ode_verts = malloc( t_count * num * 4 * 4 * sizeof(dVector3));
	ode_indices = malloc( t_count * num * 4 * 4 * sizeof(int));
	printf("Making: %d verts, %d indices\n", t_count * num * 4 * 3, t_count * num * 4 * 4);

	cur_turd = head;
	while (cur_turd->nxt) {	
		nxt_turd = cur_turd->nxt;
	//printf("---\n");
		lct = cur_turd->l;
		lnt = nxt_turd->l;
		rct = cur_turd->r;
		rnt = nxt_turd->r;
	
		// generate 3 interpolation structs
		// xxx - should store these for later, no?
		interpInit(&cin, cur_turd, nxt_turd);
		interpGenClosestLine( &cin );
		
		interpInit(&lin, lct, lnt);
		interpGenClosestLine( &lin );
		
		interpInit(&rin, rct, rnt);
		interpGenClosestLine( &rin );
		

		
		float plx = lct->wx;
		float ply = lct->wy;
		float plz = lct->wz;
		
		float pcx = cur_turd->wx;
		float pcy = cur_turd->wy;
		float pcz = cur_turd->wz;
		
		float prx = rct->wx;
		float pry = rct->wy;
		float prz = rct->wz;
		
		// add first three vertices
		ode_verts[v_count++] = plx;
		ode_verts[v_count++] = ply;
		ode_verts[v_count++] = plz;
		v_count++;
		
		ode_verts[v_count++] = pcx;
		ode_verts[v_count++] = pcy;
		ode_verts[v_count++] = pcz;
		v_count++;
		
		ode_verts[v_count++] = prx;
		ode_verts[v_count++] = pry;
		ode_verts[v_count++] = prz;
		v_count++;			
				
		
		for (i=0; i<=num; i++) {
			t = (float)i/num;
			
			interpDraw( &lin, t, (float *)&ls );
			interpDraw( &cin, t, (float *)&cs );
			interpDraw( &rin, t, (float *)&rs );
					
			plx = ls[0];
			ply = ls[1];
			plz = ls[2];
			
			pcx = cs[0];
			pcy = cs[1];
			pcz = cs[2];
			
			prx = rs[0];
			pry = rs[1];
			prz = rs[2];
			
			ode_verts[v_count++] = plx;
			ode_verts[v_count++] = ply;
			ode_verts[v_count++] = plz;
			v_count++;
			
			ode_verts[v_count++] = pcx;
			ode_verts[v_count++] = pcy;
			ode_verts[v_count++] = pcz;
			v_count++;
			
			ode_verts[v_count++] = prx;
			ode_verts[v_count++] = pry;
			ode_verts[v_count++] = prz;
			v_count++;
			
			int p_start = (v_count / 4) - 6;
			int s_start = (v_count / 4) - 3;
			
			
			// should be clockwise, according to manual,
			// but wheels don't interact with it
			// are they wound the wrong way?
			if (0) {
				// clockwise winding
				ode_indices[i_count++] = p_start+1;
				ode_indices[i_count++] = p_start;
				ode_indices[i_count++] = s_start;
				
				ode_indices[i_count++] = s_start;
				ode_indices[i_count++] = s_start+1;
				ode_indices[i_count++] = p_start+1;
				
				ode_indices[i_count++] = p_start+2;
				ode_indices[i_count++] = p_start+1;
				ode_indices[i_count++] = s_start+1;
				
				ode_indices[i_count++] = s_start+1;
				ode_indices[i_count++] = s_start+2;
				ode_indices[i_count++] = p_start+2;
			} else {
				// anti-clockwise winding
				ode_indices[i_count++] = s_start;
				ode_indices[i_count++] = p_start;
				ode_indices[i_count++] = p_start+1;
				
				ode_indices[i_count++] = p_start+1;
				ode_indices[i_count++] = s_start+1;
				ode_indices[i_count++] = s_start;
				
				ode_indices[i_count++] = s_start+1;
				ode_indices[i_count++] = p_start+1;
				ode_indices[i_count++] = p_start+2;

				ode_indices[i_count++] = p_start+2;
				ode_indices[i_count++] = s_start+2;
				ode_indices[i_count++] = s_start+1;
			}
		}
			
	
	
		cur_turd = nxt_turd;
	}
	
	dGeomTriMeshDataBuildSimple( tri->dataid, ode_verts, v_count, ode_indices, i_count );
	dGeomTriMeshSetData( tri->meshid, tri->dataid );
	
	
	
	return tri;
}



void drawRoad(struct turd_struct *head) {
	struct turd_struct *cur_turd = head;
	struct turd_struct *nxt_turd;
	struct turd_struct *lct,*lnt, *rct,*rnt;
	


	glMaterialfv (GL_FRONT, GL_DIFFUSE, gray);
	glMaterialfv (GL_FRONT, GL_AMBIENT, black);
	glMaterialfv (GL_FRONT, GL_SPECULAR, dgray);
	//glMateriali (GL_FRONT, GL_SHININESS, 1);
	
	
	float ls[3];
	float cs[3];
	float rs[3];
// 
	interp_struct lin;
	interp_struct cin;
	interp_struct rin;
	
	int num=10;


	cur_turd = head;
	while (cur_turd->nxt) {	
		nxt_turd = cur_turd->nxt;
	//printf("---\n");
		lct = cur_turd->l;
		lnt = nxt_turd->l;
		rct = cur_turd->r;
		rnt = nxt_turd->r;
	
		// generate 3 interpolation structs
		// xxx - should store these for later, no?
		interpInit(&cin, cur_turd, nxt_turd);
		interpGenClosestLine( &cin );
		
		interpInit(&lin, lct, lnt);
		interpGenClosestLine( &lin );
		
		interpInit(&rin, rct, rnt);
		interpGenClosestLine( &rin );
		
		int i;
		
		float t;
		
		float plx = lct->wx;
		float ply = lct->wy;
		float plz = lct->wz;
		
		float pcx = cur_turd->wx;
		float pcy = cur_turd->wy;
		float pcz = cur_turd->wz;
		
		float prx = rct->wx;
		float pry = rct->wy;
		float prz = rct->wz;
			
		
		for (i=0; i<=num; i++) {
	
			glBegin(GL_TRIANGLE_STRIP);
			t = (float)i/num;
			
			interpDraw( &lin, t, (float *)&ls );
			interpDraw( &cin, t, (float *)&cs );
			interpDraw( &rin, t, (float *)&rs );
			
			// Current Mormal MOD Near/Far
			float cnmodn = (1.0-t);
			float cnmodf = t;
			
			float nnmodn = (1.0-(i+1.0)/num);
			float nnmodf = (i+1.0)/num;
			
			glNormal3f(	cur_turd->l->anx*cnmodn + nxt_turd->l->anx * cnmodf,
									cur_turd->l->any*cnmodn + nxt_turd->l->any * cnmodf,
									cur_turd->l->anz*cnmodn + nxt_turd->l->anz * cnmodf);
			glVertex3f(plx, ply, plz);
			glNormal3f(	cur_turd->l->anx*nnmodn + nxt_turd->l->anx * nnmodf,
						cur_turd->l->any*nnmodn + nxt_turd->l->any * nnmodf,
						cur_turd->l->anz*nnmodn + nxt_turd->l->anz * nnmodf);
			glVertex3f(ls[0], ls[1], ls[2]);
			
			glNormal3f(	cur_turd->anx*(1-t) + nxt_turd->anx * t,
									cur_turd->any*(1-t) + nxt_turd->any * t,
									cur_turd->anz*(1-t) + nxt_turd->anz * t);
			glVertex3f(pcx, pcy, pcz);
			glVertex3f(cs[0], cs[1], cs[2]);
			
			glNormal3f(	cur_turd->r->anx*cnmodn + nxt_turd->r->anx * cnmodf,
									cur_turd->r->any*cnmodn + nxt_turd->r->any * cnmodf,
									cur_turd->r->anz*cnmodn + nxt_turd->r->anz * cnmodf);
			glVertex3f(prx, pry, prz);
			glNormal3f(	cur_turd->r->anx*nnmodn + nxt_turd->r->anx * nnmodf,
						cur_turd->r->any*nnmodn + nxt_turd->r->any * nnmodf,
						cur_turd->r->anz*nnmodn + nxt_turd->r->anz * nnmodf);
			glVertex3f(rs[0], rs[1], rs[2]);		
			glEnd();
			
			plx = ls[0];
			ply = ls[1];
			plz = ls[2];
			
			pcx = cs[0];
			pcy = cs[1];
			pcz = cs[2];
			
			prx = rs[0];
			pry = rs[1];
			prz = rs[2];

		}
		cur_turd = nxt_turd;
	}
		
	if ( head->tri == NULL ) {
		head->tri = calcTrimesh( head );
	}
}

void recalcTurd( turd_struct *t ) {
	calcTurd( t );
	
	// technically we don't need to do this if nothing is running
	if ( editing == 0 ) {
		calcTrimesh( t );
	}
}


struct turd_struct *spiral;
struct turd_struct *ramp;
struct turd_struct *loop;
struct turd_struct *helix;

void initTurdTrack() {

	ramp = loadTurd("./data/worlds/Sandbox/tracks/Box/ramp2.conf");
	spiral = loadTurd("./data/worlds/Sandbox/tracks/Box/spiral.conf");
	loop = loadTurd("./data/worlds/Sandbox/tracks/Box/loopd.conf");
	helix = loadTurd("./data/worlds/Sandbox/tracks/Box/helix.conf");

	glBegin(GL_LINES);
		glVertex3f(0,0,0);
		glVertex3f(0,0,0);
	glEnd();
	
}

void doTurdTrack() {
	
	//drawRoad(spiral);
	drawRoad(ramp);
	//drawRoad(loop);
	//drawRoad(helix);
}


int load_track (char *path)
{
	printlog(1, "=> Loading track: %s\n", path);
	char *conf=calloc(strlen(path)+11+1,sizeof(char));//+1 for \0
	strcpy (conf,path);
	strcat (conf,"/track.conf");

	if (load_conf(conf, &track, track_index))
		return -1;

	free (conf);

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
/*
	geom = dCreatePlane (0, 0,-1,0,-100);
	data = allocate_geom_data(geom, track.object);
	data->mu = track.mu;
	data->slip = track.slip;
	data->erp = track.erp;
	data->cfm = track.cfm;
*/

	//since a plane is a non-placeable geom, the sepparate components will
	//not be "rendered" separately, instead create one 3d image sepparately

	track.file_3d = allocate_file_3d();
	glNewList (track.file_3d->list, GL_COMPILE);
	//the ground and walls for the environment box
	glMaterialfv (GL_FRONT, GL_AMBIENT_AND_DIFFUSE, green);
	glNormal3f (0.0f, 0.0f, 1.0f);
	glBegin (GL_QUADS);
	glVertex3f (100.0f, -100.0f, 0.0f);
	glVertex3f (100.0f, 100.0f, 0.0f);
	glVertex3f (-100.0f, 100.0f, 0.0f);
	glVertex3f (-100.0f, -100.0f, 0.0f);
	glEnd();

	glMaterialfv (GL_FRONT, GL_AMBIENT_AND_DIFFUSE, gray);
	glBegin (GL_QUADS);
	glNormal3f (1.0f, 0.0f, 0.0f);
	glVertex3f (-100.0f, 100.0f, 0.0f);
	glVertex3f (-100.0f, 100.0f, 10.0f);
	glVertex3f (-100.0f, -100.0f, 10.0f);
	glVertex3f (-100.0f, -100.0f, 0.0f);

	glNormal3f (0.0f, -1.0f, 0.0f);
	glVertex3f (100.0f, 100.0f, 0.0f);
	glVertex3f (100.0f, 100.0f, 10.0f);
	glVertex3f (-100.0f, 100.0f, 10.0f);
	glVertex3f (-100.0f, 100.0f, 0.0f);

	glNormal3f (-1.0f, 0.0f, 0.0f);
	glVertex3f (100.0f, -100.0f, 0.0f);
	glVertex3f (100.0f, -100.0f, 10.0f);
	glVertex3f (100.0f, 100.0f, 10.0f);
	glVertex3f (100.0f, 100.0f, 0.0f);

	glNormal3f (0.0f, 1.0f, 0.0f);
	glVertex3f (-100.0f, -100.0f, 0.0f);
	glVertex3f (-100.0f, -100.0f, 10.0f);
	glVertex3f (100.0f, -100.0f, 10.0f);
	glVertex3f (100.0f, -100.0f, 0.0f);

	glEnd();
	//doTurdTrack();
	glEndList();

	initTurdTrack();

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
	char *list=calloc(strlen(path)+12+1,sizeof(char));//+1 for \0
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
			script *obj;
			char *obj_list = calloc(strlen(w[1])+13+1, sizeof(char));
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
					obj_list = calloc(strlen(w[1])+13+1,
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


car *load_car (char *path)
{
	printlog(1, "=> Loading car: %s", path);

	//see if already loaded
	car *tmp = car_head;
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
	car *target = allocate_car();
	target->name = calloc(strlen(path) + 1, sizeof(char));
	strcpy (target->name, path);

	char *conf=calloc(strlen(path)+9+1,sizeof(char));//+1 for \0
	strcpy (conf,path);
	strcat (conf,"/car.conf");

	if (load_conf(conf, target, car_index))
		return NULL;

	free (conf);

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
	glEnd();
	
	// draw tyre in reverse too
	glBegin (GL_QUAD_STRIP);
	glMaterialfv (GL_FRONT, GL_SPECULAR, black);
	glMaterialfv (GL_FRONT, GL_AMBIENT_AND_DIFFUSE, dgray);
	for (v=0; v<=2*M_PI; v+=2*M_PI/10)
	{
		glNormal3f (-sin(v), -cos(v), 0.0f);
		glVertex3f(w_r*sin(v), w_r*cos(v), w_w/2.0f);
		glVertex3f(w_r*sin(v), w_r*cos(v), -w_w/2.0f);
	}

	glEnd();
	
	//rim
	glMaterialfv (GL_FRONT, GL_AMBIENT_AND_DIFFUSE, lgray);

	glBegin (GL_QUADS);
		glNormal3f (0.0f, 0.0f, 1.0f);
		glVertex3f(-w_r*0.9f, w_r/5, w_w/3.0f);
		glVertex3f(-w_r*0.9f, -w_r/5, w_w/3.0f);
		glVertex3f(w_r*0.9f, -w_r/5, w_w/3.0f);
		glVertex3f(w_r*0.9f, w_r/5, w_w/3.0f);

		glVertex3f(-w_r/5, w_r*0.9f, w_w/3.0f);
		glVertex3f(-w_r/5, -w_r*0.9f, w_w/3.0f);
		glVertex3f(w_r/5, -w_r*0.9f, w_w/3.0f);
		glVertex3f(w_r/5, w_r*0.9f, w_w/3.0f);
	glEnd();

	glBegin (GL_QUADS);
		glNormal3f (0.0f, 0.0f, -1.0f);
		glVertex3f(w_r*0.9f, w_r/5, w_w/3.1f);
		glVertex3f(w_r*0.9f, -w_r/5, w_w/3.1f);
		glVertex3f(-w_r*0.9f, -w_r/5, w_w/3.1f);
		glVertex3f(-w_r*0.9f, w_r/5, w_w/3.1f);

		glVertex3f(w_r/5, w_r*0.9f, w_w/3.1f);
		glVertex3f(w_r/5, -w_r*0.9f, w_w/3.1f);
		glVertex3f(-w_r/5, -w_r*0.9f, w_w/3.1f);
		glVertex3f(-w_r/5, w_r*0.9f, w_w/3.1f);
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


void spawn_car(car *target, dReal x, dReal y, dReal z)
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
	geom_data *wheel_data;
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
		wheel_data = allocate_geom_data(wheel_geom, target->object);

		//friction
		wheel_data->mu = target->wheel_mu;
		wheel_data->use_slip = true;
		wheel_data->slip = target->wheel_slip;
		wheel_data->bounce = target->wheel_bounce;

		//hardness
		wheel_data->erp = target->wheel_erp;
		wheel_data->cfm = target->wheel_cfm;


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
		wheel_data->file_3d = target->wheel_graphics;
		
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
void remove_object(object *target)
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

void remove_car (car* target)
{
	printlog(1, "removing car\n");
	remove_object (target->object);
	free_car (target);
}
