
#include "orientation.h"

orientation::orientation() {
}

orientation::orientation(float *v) {
	memcpy(m,v,16 * sizeof(float));
}

float *orientation::getMatrix() {
	return m;
}

void orientation::setMatrix(float *v) {
	memcpy(m,v,16 * sizeof(float));
}