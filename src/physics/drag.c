//functions for setting body drag
//NOTE: modifying specified drag to the current mass (rice-burning optimization, or simply good idea)

void Body_Data_Set_Linear_Drag (body_data *body, dReal drag)
{
	dMass mass;
	dBodyGetMass (body->body_id, &mass);

	body->linear_drag = drag/(mass.mass);
	body->use_linear_drag = true;
	body->use_advanced_linear_drag = false;
}

void Body_Data_Set_Advanced_Linear_Drag (body_data *body, dReal drag_x, dReal drag_y, dReal drag_z)
{
	dMass mass;
	dBodyGetMass (body->body_id, &mass);

	body->advanced_linear_drag[0] = drag_x/(mass.mass);
	body->advanced_linear_drag[1] = drag_y/(mass.mass);
	body->advanced_linear_drag[2] = drag_z/(mass.mass);

	body->use_advanced_linear_drag = true;
	body->use_linear_drag = false;
}

void Body_Data_Set_Angular_Drag (body_data *body, dReal drag)
{
	dMass mass;
	dBodyGetMass (body->body_id, &mass);

	//TODO: this is not realistic, should also be affected by how mass is spread
	body->angular_drag = drag/(mass.mass);
	body->use_angular_drag = true;
}
