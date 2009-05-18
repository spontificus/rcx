#ifndef __MAIN_H__
#include "shared.h"

#define VERSION "0.05" //supports alphanumeric versioning

extern car_struct *venom;
extern script_struct *box; //keep track of our loaded debug box
extern car_struct *focused_car;

extern struct internal_struct internal;
extern struct track_struct track;

/* main.c */
void printlog(int level, const char *text, ...);
void emergency_quit(void);

#define __MAIN_H__
#endif