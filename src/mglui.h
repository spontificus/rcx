#ifndef __MGLUI_H__
#define __MGLUI_H__

#include "shared.h"
#include "main.h"
#include <GL/glu.h>
#include "glui.h"

void graphics_step();

#ifdef __cplusplus

void setupGLUI();

class mglui {
public:
	static mglui *Instance();

private:
	mglui(){};
	static mglui *m_pInstance;

};

#endif
#endif
