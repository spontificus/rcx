//collide camera with track, generate acceleration on camera if collisding
void camera_physics_step(dReal step)
{
	if (camera.geom && camera.settings)
	{
		dGeomSetPosition(camera.geom, camera.pos[0], camera.pos[1], camera.pos[2]);

		dContactGeom contact[internal.contact_points];
		int count = dCollide (track.object->space, camera.geom, internal.contact_points, &contact[0], sizeof(dContactGeom));

		if (!count)
			return;

		int i;
		dReal depth;
		dReal caccel = camera.settings->accel_coll;
		dReal accel[3] = {0,0,0};
		for (i=0; i<count; ++i)
		{
			depth = contact[i].depth;
			//depth = 1;
			accel[0]+=contact[i].normal[0]*depth*caccel;
			accel[1]+=contact[i].normal[1]*depth*caccel;
			accel[2]+=contact[i].normal[2]*depth*caccel;
		}

		camera.vel[0] -= accel[0]*internal.stepsize;
		camera.vel[1] -= accel[1]*internal.stepsize;
		camera.vel[2] -= accel[2]*internal.stepsize;
	}
}
