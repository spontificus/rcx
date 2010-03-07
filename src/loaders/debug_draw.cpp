#include "debug_draw.hpp"

#include "colours.hpp"
#include "../shared/printlog.hpp"

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
	printlog(2, "Creating rendering list for debug sphere");
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

