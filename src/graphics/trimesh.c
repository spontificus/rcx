
//new trimesh rendering method, instead of calling rendering list
//TODO: some of this data (vertices) can be moved to video ram, which
//would make the necessary data to send to gpu less (=higher fps)
void render_trimesh (trimesh* target)
{

	//variables for fast looping
	char *inst = target->instructions;
	unsigned int *v_index = target->vector_indices;
	unsigned int *n_index = target->normal_indices;
	unsigned int *mat_index = target->material_indices;
	GLfloat *vertex, *normal; //vectors
	material *mat;
	GLenum *mode = target->modes;
	
	bool reset_gl = false;
	//future instructions might be different, like vertex and normal indices
	//sepparately (but since they are often specified at the same time,
	//maybe not?)
	while (1)
	{
		if (*inst == 'i') //vertex&normal index
		{
			normal = &target->normals[(*n_index)*3];
			glNormal3f (normal[0], normal[1], normal[2]);
			vertex = &target->vertices[(*v_index)*3];
			glVertex3f (vertex[0], vertex[1], vertex[2]);

			++v_index;
			++n_index;

		}
		else if (*inst == 'M') //opengl mode
		{
			if (reset_gl)
				glEnd();
			glBegin (*mode);
			++mode;
			reset_gl = true; //run glEnd before setting next mode
		}
		else if (*inst == 'm') //material "index"
		{
			mat = &target->materials[*mat_index];

			glMaterialfv (GL_FRONT, GL_AMBIENT,mat->ambient);
			glMaterialfv (GL_FRONT, GL_DIFFUSE,mat->diffuse);
			glMaterialfv (GL_FRONT, GL_SPECULAR, mat->specular);
			glMaterialf  (GL_FRONT, GL_SHININESS, mat->shininess);

			++mat_index;
		}
		else //assumed to be '\0'
			break;

		++inst;
	}
	glEnd();

	//glMaterialfv (GL_FRONT, GL_SPECULAR, black);
}
