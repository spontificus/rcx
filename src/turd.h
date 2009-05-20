#ifndef __TURD_H__

#include "shared.h"
#include "trimesh.h"
class trimesh;

class turd {
public:
	turd() {
		l = NULL;
		r = NULL;
		pre = NULL;
		nxt = NULL;
		
		calllist = 0;
		redraw = 1;
	}

	float x,y,z;
	float a,b,c;
	float xnx,xny,xnz;
	float ynx,yny,ynz;
	float znx,zny,znz;
	float anx,any,anz;
	float wx,wy,wz;
	
	GLfloat m[16];
	turd *l;
	turd *r;
	turd *pre;
	turd *nxt;
	
	// should be elsewhere
	trimesh *tri;
	int calllist;
	int redraw;
	
	void load(const char *filename);
	void setup( float x,float y,float z, float a,float b,float c );
	void makeMatricies();
	void calc();
};

extern turd *turd_head;
extern turd *edit_t;
extern turd *edit_h;
extern turd edit_b;
extern int edit_m;

#define X_AXIS 1
#define Y_AXIS 2
#define Z_AXIS 3




// function prototypes
void t_backup( turd *t );
void t_restore( turd *t );

#define __TURD_H__
#endif
