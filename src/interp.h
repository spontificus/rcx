#ifndef __INTERP_H__
#define __INTERP_H__

#include "shared.h"
#include "turd.h"

#ifdef __cplusplus

class interp {
public:
	interp() {
	}

	void draw(float t, float *p , float *n);
  void init( int axis, turd *cur_turd, turd *nxt_turd );

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

};

#endif  // __cplusplus
#endif
