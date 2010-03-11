#include <ode/ode.h>
#include "../shared/geom.hpp"

//temporary geom event processing
void Geom::TMP_Events_Step(Uint32 step)
{
	Geom *geom = Geom::head;
	while (geom)
	{
		if (geom->event)
		{
			printf("TODO: remove this geom!\n");
			geom->event = false;
		}

		if (geom->flipper_geom)
		{
			if (geom->colliding)
			{
				const dReal *pos;
				pos = dGeomGetPosition(geom->flipper_geom);
				dGeomSetPosition(geom->flipper_geom, pos[0], pos[1],
						pos[2]+step*0.02);
				if (++geom->flipper_counter > 10)
					geom->colliding=false;
			}
			else if (geom->flipper_counter)
			{
				const dReal *pos;
				pos = dGeomGetPosition(geom->flipper_geom);
				dGeomSetPosition(geom->flipper_geom, pos[0], pos[1],
						pos[2]-step*0.02);
				--geom->flipper_counter;
			}
		}
		geom=geom->next;
	}
}
