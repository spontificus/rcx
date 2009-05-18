#ifndef __TRIMESH_H__
#include <ode/ode.h>
#include "shared.h"
#include "main.h"
// prototypes
geom_data *allocate_geom_data (dGeomID geom, object_struct *obj);

class trimesh {

	dGeomID meshid;
	dTriMeshDataID dataid;
	
	dVector3 *ode_verts;
	unsigned int *ode_indices;
	struct geom_data_struct *data;
	int v_count;
	int i_count;

	struct turd_struct *head;
public:
	int i; 

	trimesh(struct turd_struct *h) {
		head = h;
		ode_verts = NULL;
		ode_indices = NULL;
		v_count = 0;
		i_count = 0;
		i = 0;
		
		dataid = dGeomTriMeshDataCreate();
		meshid = dCreateTriMesh(NULL, dataid, NULL, NULL, NULL);
		
		geom_data *data = allocate_geom_data(meshid, track.object);
		data->mu = track.mu;
		data->slip = track.slip;
		data->erp = track.erp;
		data->cfm = track.cfm;
		data->collide=1;
	}
	
	void init(int numx, int numy);
	void addVert(int i, float *v);
	void link(int numx, int numy);
	void drawDebug();

};

#define __TRIMESH_H__
#endif
