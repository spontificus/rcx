#ifndef __GRAPHICS_H__
#define __GRAPHICS_H__

#include "shared.h"
#include "loaders.h"

extern SDL_Surface *screen;
extern GLdouble cpos[3];
extern GLdouble ecpos[3];
extern Uint32 flags;




/* graphics.c */

GLuint genTex_chequers(void);
void graphics_resize(int w, int h);
int graphics_init(void);
void graphics_step(Uint32 step);
void graphics_loop(void);
void graphics_quit(void);

#endif
