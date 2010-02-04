
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
	char list[strlen(path)+12+1];
	strcpy (list,path);
	strcat (list,"/objects.lst");

	printlog(1, "-> Loading track object list: %s\n", path);
	Text_File file(list);

	//each object is loaded/selected at a time (NULL if none loaded so far)
	script_struct *obj = NULL;

	//don't fail if can't find file, maybe there is no need for it anyway
	if (file.open)
	{
		while (file.Read_Line())
		{
			//object load request
			if (file.word_count==2 && !strcmp(file.words[0], ">"))
			{
				printlog(2, " * object load request: %s\n", file.words[1]);
				char obj_name[13+strlen(file.words[1])+1];
				strcpy (obj_name, "data/objects/");
				strcat (obj_name, file.words[1]);

				obj = load_object(obj_name);

				//failure to load object
				if (!obj)
					break;
			}
			//three words (x, y and z coord for spawning): spawning
			else if (file.word_count == 3)
			{
				printlog(2, " * object spawn request\n");
				//in case no object has been loaded yet
				if (!obj)
				{
					printlog(0, "ERROR: trying to spawn object without specifying what object!\n");
					break;
				}

				//translate words to values
				float x,y,z;

				//assume conversion is succesfully (not reliable, but it shouldn't be a problem)
				x = atof(file.words[0]);
				y = atof(file.words[1]);
				z = atof(file.words[2]);

				spawn_object(obj, x, y, z);
			}
			else
			{
				printlog(0, "ERROR: unknown line in object list!\n");
				break;
			}
		}
	}

	//that's it!
	printlog(1, "\n");
	return 0;
}

