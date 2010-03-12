#include "../shared/geom.hpp"
#include "../shared/internal.hpp"
#include "../shared/printlog.hpp"

void Geom::Collision_Force(dReal force)
{
	//if not true, no point continuing
	if (!(threshold>0 && force>threshold))
		return;

	//buffer still got health
	if (buffer > 0)
	{
		buffer -= (force-threshold)*internal.stepsize;

		//now it's negative, issue event
		if (buffer < 0)
		{
			printlog(2, "Geom buffer depleted, generating event");
			event = true;
		}
	}
	else //just damage buffer even more
		buffer -= (force-threshold)*internal.stepsize;
}

void Geom::Increase_Buffer(dReal buff)
{
	buffer+=buff;

	if (buffer < 0) //still depleted, regenerate event
		event = true;
}
