#include "../shared/geom.hpp"

#include <ode/ode.h>

//loop through all geoms, see if they need rendering

void Geom::Graphics_Step()
{
	Geom *geom;
	const dReal *pos, *rot; //store rendering position
	for (geom = head; geom; geom = geom->next)
	{
		if (!geom->f_3d) //invisible
			continue;

		glPushMatrix();
			pos = dGeomGetPosition (geom->geom_id);
			rot = dGeomGetRotation (geom->geom_id);


			//create transformation matrix to render correct position
			//and rotation (float)
			GLfloat matrix[16];
			matrix[0]=rot[0];
			matrix[1]=rot[4];
			matrix[2]=rot[8];
			matrix[3]=0;
			matrix[4]=rot[1];
			matrix[5]=rot[5];
			matrix[6]=rot[9];
			matrix[7]=0;
			matrix[8]=rot[2];
			matrix[9]=rot[6];
			matrix[10]=rot[10];
			matrix[11]=0;
			matrix[12]=pos[0];
			matrix[13]=pos[1];
			matrix[14]=pos[2];
			matrix[15]=1;

			glMultMatrixf (matrix);

			//render
			glCallList (geom->f_3d->list);
		glPopMatrix();
	}
}
