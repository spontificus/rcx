#ifndef _RCX_RUNLEVEL_H
#define _RCX_RUNLEVEL_H

//use a "runlevel" (enum) variable to make all threads/loops aware of status
//(locked is used when we want to pause the loops, but still try to catch
//up with real time when unlocked, basically: stop simulation, but do not
//reset "simulated time" variables... Use it when building objects)
typedef enum {running, done, paused, locked, error} runlevel_type;
extern runlevel_type runlevel;

#endif
