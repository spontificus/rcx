#ifndef __ORIENTATION_H__
#define __ORIENTATION_H__
#ifdef __cplusplus
#include "string.h"

/**
 * stored as matrix for now
 * - move to quats later
 */
class orientation {
private:
	float m[16];
	
public:
	orientation();
	orientation(float *v);
	
	float *getMatrix();
	void setMatrix(float *v);

};

#endif
#endif
