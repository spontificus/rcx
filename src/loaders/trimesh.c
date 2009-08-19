//translate a trimesh (for rendering) to an ode trimesh geom (for collision)
dGeomID trimesh_to_geom (trimesh *mesh)
{
	printlog(1, "-> Translating trimesh to geometrical trimesh ");
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
	
	printlog (1, "(%u triangles)\n", i_count/3);

	//translate
	i_count = 0;
	v_count = 0;
	int corners =0;
	mode = none;
	modes = mesh->modes;
	unsigned int poly_first_vert=0, poly_last_vert=0;
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
			++modes;
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
				if (corners == 1)
					poly_first_vert = mesh->vector_indices[v_count];

				if (corners <= 3)
					mesh->geom_tri_indices[i_count++] = mesh->vector_indices[v_count++];
				else
				{
					mesh->geom_tri_indices[i_count++] = poly_first_vert;
					mesh->geom_tri_indices[i_count++] = poly_last_vert;
					mesh->geom_tri_indices[i_count++] = mesh->vector_indices[v_count++];
				}
				poly_last_vert = mesh->vector_indices[v_count-1];
			}
		}
	}


	dTriMeshDataID data = dGeomTriMeshDataCreate();
	dGeomTriMeshDataBuildSingle (data, (void*) mesh->vertices, sizeof(float)*3, mesh->vertex_count, (void*) mesh->geom_tri_indices, i_count, sizeof(dTriIndex)*3);
	return dCreateTriMesh(0, data, 0,0,0);
}


