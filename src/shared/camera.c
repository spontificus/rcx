void set_camera_settings (camera_settings *settings)
{
	if (settings)
	{
		camera.settings = settings;

		if (settings->radius != 0)
		{
			if (!camera.geom)
				camera.geom = dCreateSphere(0, settings->radius);
			else
				dGeomSphereSetRadius (camera.geom, settings->radius);
		}
		else
		{
			if (camera.geom)
				dGeomDestroy(camera.geom);
			camera.geom=NULL;
		}

		//if this camera mode doesn't have reverse enabled, make sure camera isn't stuck in reverse
		if (!settings->reverse)
			camera.reverse = false;

		//if this camera mode has "air mode" disabled, make sure camera isn't stuck in "in air" mode from before
		if (!settings->in_air)
		{
			camera.in_air = false;
			camera.offset_scale = 1;
		}
	}
}

