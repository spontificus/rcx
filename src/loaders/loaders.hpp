#ifndef _RCX_LOADERS_H
#define _RCX_LOADERS_H
#include "GL/gl.h"
#include "../shared/profile.hpp"
#include "../shared/script.hpp"
#include "../shared/car.hpp"
#include "../shared/object.hpp"
//the following a some basic color definitions (used for lights and materials)
const GLfloat black[]     = {0.0f, 0.0f, 0.0f, 1.0f}; // = nothing for lights
const GLfloat dgray[]     = {0.2f, 0.2f, 0.2f, 1.0f};
const GLfloat gray[]      = {0.5f, 0.5f, 0.5f, 1.0f};
const GLfloat lgray[]     = {0.8f, 0.8f, 0.8f, 1.0f};
const GLfloat white[]     = {1.0f, 1.0f, 1.0f, 1.0f};
const GLfloat red[]       = {1.0f, 0.0f, 0.0f, 1.0f};
const GLfloat green[]     = {0.0f, 1.0f, 0.0f, 1.0f};
const GLfloat lgreen[]    = {0.4f, 1.0f, 0.4f, 1.0f};
const GLfloat blue[]      = {0.0f, 0.0f, 1.0f, 1.0f};
const GLfloat lblue[]     = {0.6f, 0.6f, 1.0f, 1.0f};
const GLfloat yellow[]    = {1.0f, 1.0f, 0.0f, 1.0f};

#include "text_file.hpp"
#include "conf.hpp"


void debug_draw_box (GLuint list, GLfloat x, GLfloat y, GLfloat z,
		const GLfloat colour[], const GLfloat specular[], const GLint shininess);

profile *load_profile (char *path);
script_struct *load_object(char *path);
void spawn_object(script_struct *script, dReal x, dReal y, dReal z);
int load_track (char *path);
car_struct *load_car (char *path);
void spawn_car(car_struct *target, dReal x, dReal y, dReal z);
void remove_object(object_struct *target);
void remove_car (car_struct* target);

#endif
