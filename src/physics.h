#ifndef __PHYSICS_H__
#define __PHYSICS_H__

#include <ode/ode.h>
#include "shared.h"



/* physics.c */
int physics_init(void);
void CollisionCallback(void *data, dGeomID o1, dGeomID o2);
void car_physics_step(void);
void joint_physics_step(void);
void body_physics_step(void);
void physics_step(void);
void physics_quit(void);

#endif
