#ifndef _RCX_LOADERS_H
#define _RCX_LOADERS_H
#include "GL/gl.h"
#include "../shared/profile.hpp"
#include "../shared/script.hpp"
#include "../shared/car.hpp"
#include "../shared/object.hpp"
#include "colours.hpp"
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
