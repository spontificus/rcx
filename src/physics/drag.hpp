#ifndef _RCX_DRAG_H
#define _RCX_DRAG_H
#include "../shared/body.hpp"

void Body_Data_Set_Linear_Drag (body_data *body, dReal drag);
void Body_Data_Set_Advanced_Linear_Drag (body_data *body, dReal drag_x, dReal drag_y, dReal drag_z);
void Body_Data_Set_Angular_Drag (body_data *body, dReal drag);
void Body_Data_Linear_Drag (body_data *body);
void Body_Data_Advanced_Linear_Drag (body_data *body);
void Body_Data_Angular_Drag (body_data *body);
#endif
