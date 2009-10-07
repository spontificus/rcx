//collide camera with track, generate acceleration on camera if collisding
void camera_physics_step(dReal step)
{
	if (camera.geom && camera.settings)
	{
		dGeomSetPosition(camera.geom, camera.pos[0], camera.pos[1], camera.pos[2]);

		dContactGeom contact[internal.contact_points];
		int count = dCollide ( (dGeomID)(track.object->space), camera.geom, internal.contact_points, &contact[0], sizeof(dContactGeom));

		if (!count)
			return;

		int i;
		dReal depth;
		dReal V;
		for (i=0; i<count; ++i)
		{
			depth = contact[i].depth;
			camera.pos[0]-=contact[i].normal[0]*depth;
			camera.pos[1]-=contact[i].normal[1]*depth;
			camera.pos[2]-=contact[i].normal[2]*depth;

			//remove movement into colliding object
			//1) velocity along collision axis
			V = camera.vel[0]*contact[i].normal[0] + camera.vel[1]*contact[i].normal[1] + camera.vel[2]*contact[i].normal[2];
			if (V > 0) //along right direction?
			{
				//remove direction
				camera.vel[0]-=V*contact[i].normal[0];
				camera.vel[1]-=V*contact[i].normal[1];
				camera.vel[2]-=V*contact[i].normal[2];
			}
		}
	}
}
