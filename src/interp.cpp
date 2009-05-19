
#include "interp.h"

// creates an interpolation object from two control points
void interp::init( int axis, turd_struct *cur_turd, turd_struct *nxt_turd ) {
		float dist;
		float psdx,psdy,psdz, pedx,pedy,pedz;
		axis = axis;
		
		ps0x = cur_turd->wx;
		ps0y = cur_turd->wy;
		ps0z = cur_turd->wz;
		
		pe0x = nxt_turd->wx;
		pe0y = nxt_turd->wy;
		pe0z = nxt_turd->wz;
		
		float xydist = pow( pe0x - ps0x, 2 ) + pow( pe0y - ps0y, 2 );
		dist = sqrt( xydist + pow( pe0z - ps0z, 2 ) ); // 2.82842712;
		
		// generate bezier control points as half distance along normal vectors
		switch ( axis ) {
			case X_AXIS:
				psdx = cur_turd->xnx - ps0x;
				psdy = cur_turd->xny - ps0y;
				psdz = cur_turd->xnz - ps0z;
				pedx = nxt_turd->xnx - pe0x;
				pedy = nxt_turd->xny - pe0y;
				pedz = nxt_turd->xnz - pe0z;
				break;
			
			case Y_AXIS:
				psdx = cur_turd->ynx - ps0x;
				psdy = cur_turd->yny - ps0y;
				psdz = cur_turd->ynz - ps0z;
				pedx = nxt_turd->ynx - pe0x;
				pedy = nxt_turd->yny - pe0y;
				pedz = nxt_turd->ynz - pe0z;				
				break;
		
			case Z_AXIS:
				psdx = cur_turd->znx - ps0x;
				psdy = cur_turd->zny - ps0y;
				psdz = cur_turd->znz - ps0z;
				pedx = nxt_turd->znx - pe0x;
				pedy = nxt_turd->zny - pe0y;
				pedz = nxt_turd->znz - pe0z;
				break;
				
			default:
				printlog(0, "Axis rotation type not supported, try another dimension");
				exit(0);
				break;
		}
		
		scx = ps0x + psdx * dist;
		scy = ps0y + psdy * dist;
		scz = ps0z + psdz * dist;
		
		tcx = pe0x - pedx * dist;
		tcy = pe0y - pedy * dist;
		tcz = pe0z - pedz * dist;
		
		snx = cur_turd->anx;
		sny = cur_turd->any;
		snz = cur_turd->anz;
		
		enx = nxt_turd->anx;
		eny = nxt_turd->any;
		enz = nxt_turd->anz;

}


// inputs an interpolation struct, with a desired scale (t = 0->1), and populates
// point 'p'
//
// cp[xyz] is a moving control point, which moves (with t) along the 3d line
//   represented by the closest points the of the y-axis of each interpolation
//   object (sc[xyz]->tc[xyz]
//
// sp[xyz] is the control point which moves (with t) from the starting
//   point to cp[xyz]
//
// ep[xyz] moves from cp[xyz] to the end point
//
// The point p is derived from the line sp[xyz]->ep[xyz], scaled by t.
//
// Once sc[xyz]->tc[xyz] has been discovered, interpolation is equally cheap
// for any value of 't'.
//
void interp::draw(float t, float *p , float *n) {
//	static float cp[3], sp[3], ep[3];
	static float s2cp[3], cp2e[3];
	static float cn[3], sn[3], en[3];
	
	s2cp[0] = ps0x + t * (scx - ps0x);
	s2cp[1] = ps0y + t * (scy - ps0y);
	s2cp[2] = ps0z + t * (scz - ps0z);
	
	cp2e[0] = tcx + t * (pe0x - tcx);
	cp2e[1] = tcy + t * (pe0y - tcy);
	cp2e[2] = tcz + t * (pe0z - tcz);

	p[0] = s2cp[0] + t * (cp2e[0] - s2cp[0]);
	p[1] = s2cp[1] + t * (cp2e[1] - s2cp[1]);
	p[2] = s2cp[2] + t * (cp2e[2] - s2cp[2]);
	
	/*
	sp[0] = in->ps0x + t * (cp[0] - in->ps0x);
	sp[1] = in->ps0y + t * (cp[1] - in->ps0y);
	sp[2] = in->ps0z + t * (cp[2] - in->ps0z);
	
	ep[0] = cp[0] + t * (in->pe0x - cp[0]);
	ep[1] = cp[1] + t * (in->pe0y - cp[1]);
	ep[2] = cp[2] + t * (in->pe0z - cp[2]);
	
	p[0] = sp[0] + t * (ep[0] - sp[0]);
	p[1] = sp[1] + t * (ep[1] - sp[1]);
	p[2] = sp[2] + t * (ep[2] - sp[2]);
	*/
	
	// more smoothing
	//p[0] = (in->ps0x + t * (in->scx - in->ps0x)) + 
	
	// any better way to calculate normal?
	cn[0] = snx + t * (enx - snx);
	cn[1] = sny + t * (eny - sny);
	cn[2] = snz + t * (enz - snz);
	normalise(cn);

	sn[0] = snx + t * (cn[0] - snx);
	sn[1] = sny + t * (cn[1] - sny);
	sn[2] = snz + t * (cn[2] - snz);
	normalise(sn);
	
	en[0] = enx + t * (enx - cn[0]);
	en[1] = eny + t * (eny - cn[1]);
	en[2] = enz + t * (enz - cn[2]);
	normalise(en);
	
	n[0] = sn[0] + t * (en[0] - sn[0]);
	n[1] = sn[1] + t * (en[1] - sn[1]);
	n[2] = sn[2] + t * (en[2] - sn[2]);
	normalise(n);

}


