#include "debug_draw.hpp"

#include "colours.hpp"
#include "../shared/printlog.hpp"

#include <math.h>

void debug_draw_box (GLuint list, GLfloat x, GLfloat y, GLfloat z,
		const GLfloat colour[], const GLfloat specular[], const GLint shininess)
{
	printlog(2, "Creating rendering list for debug box");

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

void debug_draw_sphere (GLuint list, GLfloat d, const GLfloat colour[],
		const GLfloat specular[], const GLint shininess)
{
	printlog(2, " > Creating rendering list for debug sphere\n");
	GLfloat radius = d/2;

	glNewList (list, GL_COMPILE);
	glEnable(GL_NORMALIZE); //easier to specify normals

	glMaterialfv (GL_FRONT, GL_AMBIENT_AND_DIFFUSE, colour);
	glMaterialfv (GL_FRONT, GL_SPECULAR, specular);
	glMateriali (GL_FRONT, GL_SHININESS, shininess);

	// normals still not perfect, but then it's an imperfect algorithm
	int slices = 12;
	int divisions = 12;
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
