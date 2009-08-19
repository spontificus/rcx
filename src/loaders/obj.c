//load an obj (and the mtl file it specifies) as a trimesh (for rendering
//and optional generation of ODE trimesh geom)
//
//warning: does not do a lot of error checking!
//some limits includes requiring normals for each index, and only one mtl file
trimesh *load_obj (char *file, float resize, float rotate[3], float offset[3])
{
	int i;
	bool do_rotate = false, do_resize = false, do_move = false;

	printlog(1, "-> Loading obj file to trimesh: %s (resize: %f, rotate: %f %f %f, offset: %f %f %f)", file, resize, rotate[0], rotate[1], rotate[2], offset[0], offset[1], offset[2]);

	//now something fun, create rotation matrix (from Euler rotation angles, in degrees)
	dMatrix3 rot;
	if (rotate[0] || rotate[1] || rotate[2])
	{
		printlog(1, " (rotate)");
		do_rotate = true;
		dRFromEulerAngles (rot, rotate[0]*(M_PI/180), rotate[1]*(M_PI/180), rotate[2]*(M_PI/180));
	}
	if (resize != 1.0)
	{
		printlog(1, " (resize)");
		do_resize = true;
	}
	if (offset[0] || offset[1] || offset[2])
	{
		printlog(1, " (move)");
		do_move = true;
	}
	
	printlog(1, "\n");

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

		if (word[0][0] == 'v' && word[0][1] == '\0')
			++vertices;
		else if (word[0][0] == 'v' && word[0][1] == 'n')
			++normals;
		else if (word[0][0] == 'f')
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
	GLfloat vertex_tmp[3];
	while ((word = get_word_list(fp_obj)))
	{
		//ingore g and s

		//vector
		if (word[0][0]=='v' && word[0][1]=='\0')
		{
			vertex = &(mesh->vertices[v_count]);
			if (	sscanf(word[1], "%f", &vertex_tmp[0]) &&
				sscanf(word[2], "%f", &vertex_tmp[1]) &&
				sscanf(word[3], "%f", &vertex_tmp[2]))
			{
					v_count += 3;

					//resize
					if (do_resize)
					{
						vertex_tmp[0]*=resize;
						vertex_tmp[1]*=resize;
						vertex_tmp[2]*=resize;
					}

					//rotate:
					if (do_rotate)
						dMultiply1 (vertex, rot, vertex_tmp,	3,3,1);
					else //not!
						for (i=0; i<3; ++i)
							vertex[i] = vertex_tmp[i];

					if (do_move)
						for (i=0; i<3; ++i)
							vertex[i] += offset[i];
			}
			else
				printlog(0, "WARNING: failed reading vector %i\n", v_count);
		}
		//normal
		else if (word[0][0]=='v' && word[0][1]=='n' && word[0][2]=='\0')
		{
			vertex = &(mesh->normals[n_count]);
			if (	sscanf(word[1], "%f", &vertex_tmp[0]) &&
				sscanf(word[2], "%f", &vertex_tmp[1]) &&
				sscanf(word[3], "%f", &vertex_tmp[2]))
			{
					n_count += 3;
					
					//rotate:
					if (do_rotate)
						dMultiply1 (vertex, rot, vertex_tmp,	3,3,1);
					else //not!
						for (i=0; i<3; ++i)
							vertex[i] = vertex_tmp[i];
			}
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
				mesh->material_indices[m_count] = i;
				//mesh->material_indices[0] = 0;
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


