
#include "trimesh.h"
#include "turd.h"

void trimesh::init(int numx, int numy) {
	struct turd_struct *cur_turd = head;
	int t_count=0;
		
	if ( ode_verts != NULL ) {
		free(ode_verts);
	}
		
	if ( ode_indices != NULL ) {
		free(ode_indices);	
	}

	// count how many triangles we're gonna need
	while (cur_turd->nxt) {	
		cur_turd = cur_turd->nxt;
		
		// 2 for each left + right patch
		t_count += 2;
	}
		
	// allocate memory
	v_count = t_count * (numx+1) * (numy+1);
	ode_verts = (dVector4 *)calloc(1, v_count * sizeof(dVector4));
	
	i_count = t_count * numx * numy * 2 * 3;
	ode_indices = (unsigned int *)calloc(1, i_count * sizeof(int));
	
}


void trimesh::addVert(int i, float *v) {
	ode_verts[i][0] = v[0];
	ode_verts[i][1] = v[1];
	ode_verts[i][2] = v[2];
}


void trimesh::link(int numx, int numy) {
	struct turd_struct *cur_turd = head;
	struct turd_struct *lr_turd;
	int xloop,yloop;
	int i = 0;
	int v_off = 0;
	
	while ( cur_turd->nxt ) {
		lr_turd = cur_turd->l;
		while ( lr_turd->r ) {
			for (yloop = 0; yloop < numy; yloop++) {
				for (xloop = 0; xloop < numx; xloop++) {

					// anticlockwise winding
					ode_indices[i++] = v_off + (numx+1);
					ode_indices[i++] = v_off;
					ode_indices[i++] = v_off + (numx+1) + 1;
					
					ode_indices[i++] = v_off + (numx+1) + 1;
					ode_indices[i++] = v_off;
					ode_indices[i++] = v_off + 1;				
					
					// skip to next vertex
					v_off++;
				}
				// skip to beginning of next row
				v_off++;
			}	
			
			// we're at the end of a patch, skip the next row of vertices as
			// they've already been linked
			v_off +=  numx + 1;
			
			lr_turd = lr_turd->r;
		}
		
		cur_turd = cur_turd->nxt;
	}

/*
	printf("num ver:%d  v:%d\n", tri->v_count, v_off);
	printf("num ind:%d  i:%d\n", tri->i_count, i);
*/
	
	dGeomTriMeshDataBuildSimple( dataid, ode_verts[0], v_count, ode_indices, i_count );
	dGeomTriMeshSetData( meshid, dataid );
}


void trimesh::drawDebug() {
	int i;
	int t = 0;
	
	
	for ( i = 0; i < i_count; i += 3 ) {
		int i1 = ode_indices[i];
		int i2 = ode_indices[i+1];
		int i3 = ode_indices[i+2];
		
//		printf("i1:%d i2:%d i3:%d\n", i1,i2,i3);
		
		float v1x = ode_verts[i1][0];
		float v1y = ode_verts[i1][1];
		float v1z = ode_verts[i1][2];
		
		float v2x = ode_verts[i2][0];
		float v2y = ode_verts[i2][1];
		float v2z = ode_verts[i2][2];
		
		float v3x = ode_verts[i3][0];
		float v3y = ode_verts[i3][1];
		float v3z = ode_verts[i3][2];
		
		glBegin(GL_LINE_STRIP);
		if ( t == 0 ) {
			glMaterialfv (GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, black);
		} else {
			glMaterialfv (GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, red);
		}
		glVertex3f(v1x, v1y, v1z);
		glVertex3f(v2x, v2y, v2z);
		glVertex3f(v3x, v3y, v3z);
		glVertex3f(v1x, v1y, v1z);
		
		glEnd();
		
		t = 1-t;
	}
}
