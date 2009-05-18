#ifndef __TURD_H__

#include "trimesh.h"
class trimesh;



typedef struct turd_struct {
	float x,y,z;
	float a,b,c;
	float xnx,xny,xnz;
	float ynx,yny,ynz;
	float znx,zny,znz;
	float anx,any,anz;
	float wx,wy,wz;
	
	GLfloat m[16];
	struct turd_struct *l;
	struct turd_struct *r;
	struct turd_struct *pre;
	struct turd_struct *nxt;
	
	// should be elsewhere
	trimesh *tri;
	int calllist;
	int redraw;
	
} turd_struct;

extern turd_struct *turd_head;
extern turd_struct *edit_t;
extern turd_struct *edit_h;
extern turd_struct edit_b;
extern int edit_m;

#define X_AXIS 1
#define Y_AXIS 2
#define Z_AXIS 3

typedef struct interp_stuct {
		float ps0x,ps0y,ps0z;
		float ps1x,ps1y,ps1z;
		float pe0x,pe0y,pe0z;
		float pe1x,pe1y,pe1z;

		float scx,scy,scz;	// closest line start
		float tcx,tcy,tcz;  // closest line term

		
		// visual normals
		float snx,sny,snz;
		float enx,eny,enz;
		
		// Which axis are we interpolating along
		int axis;

} interp_struct;



// function prototypes
void doTurdTrack();
void makeTurd( struct turd_struct *tmp_turd, float x,float y,float z, float a,float b,float c );
void t_backup( turd_struct *t );
void t_restore( turd_struct *t );

#define __TURD_H__
#endif
