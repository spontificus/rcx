#include "../shared/geom.hpp"
#include "../shared/internal.hpp"

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
			event = true;
		}
	}
	else //just damage buffer even more
		buffer -= (force-threshold)*internal.stepsize;
}
