#ifndef __LOADERS_H__

#include <ode/ode.h>
#include <SDL.h>
#include <SDL_opengl.h>
#include "shared.h"

#include "trimesh.h"
#include "main.h"
#include "turd.h"
#include "interp.h"

/* loaders.c */
int get_word_length(FILE *fp);
char **get_word_list(FILE *fp);
void free_word_list(char **target);
int load_conf(char *name, char *memory, const struct data_index index[]);
SDLKey get_key(char *name);
profile *load_profile(char *path);
void debug_draw_box(GLuint list, GLfloat x, GLfloat y, GLfloat z, GLfloat colour[], GLfloat specular[], GLint shininess);
void debug_draw_sphere_part(GLfloat x, GLfloat y, GLfloat z);
void debug_draw_sphere(GLuint list, GLfloat d, GLfloat colour[], GLfloat specular[], GLint shininess);
script_struct *load_object(char *path);
void debug_joint_fixed(dBodyID body1, dBodyID body2, object_struct *obj);
void spawn_object(script_struct *script, dReal x, dReal y, dReal z);
float *mbv(float *m, float x, float y, float z);
void makeTurdMatricies(struct turd_struct *tmp_turd);
void setupTurdValues(struct turd_struct *tmp_turd, float x, float y, float z, float a, float b, float c);
void calcTurd(struct turd_struct *t);
void recalcTurd(struct turd_struct *t);
void drawRoad(struct turd_struct *head);
void initTurdTrack(void);
void doTurdTrack(void);
int load_track(char *path);
car_struct *load_car(char *path);
void spawn_car(car_struct *target, dReal x, dReal y, dReal z);
void remove_object(object_struct *target);
void remove_car(car_struct *target);

#define __LOADERS_H__
#endif