#include "../shared/shared.hpp"
//set camera view before rendering
void camera_graphics_step()
{
		//set camera
		gluLookAt(camera.pos[0], camera.pos[1], camera.pos[2], camera.t_pos[0], camera.t_pos[1], camera.t_pos[2], camera.up[0], camera.up[1], camera.up[2]);
}


