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
		for (i=0; i<count; ++i)
		{
			depth = contact[i].depth;
			camera.pos[0]-=contact[i].normal[0]*depth;
			camera.pos[1]-=contact[i].normal[1]*depth;
			camera.pos[2]-=contact[i].normal[2]*depth;
		}
	}
}
