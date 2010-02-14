#ifndef _RCX_DEBUG_DRAW_H
#define _RCX_DEBUG_DRAW_H
#include <GL/gl.h>

void debug_draw_box (GLuint list, GLfloat x, GLfloat y, GLfloat z,
		const GLfloat colour[], const GLfloat specular[], const GLint shininess);

void debug_draw_sphere (GLuint list, GLfloat d, const GLfloat colour[],
		const GLfloat specular[], const GLint shininess);

#endif
