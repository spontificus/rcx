#include <ode/ode.h>
#include "../shared/geom.hpp"
#include "../shared/body.hpp"
#include "../shared/track.hpp"

//temporary geom event processing
void Geom::TMP_Events_Step(Uint32 step)
{
	Geom *geom = Geom::head;
	while (geom)
	{
		//TMP demo until scripting
		if (geom->event)
		{
			geom->event = false;

			dBodyID bodyid = dGeomGetBody(geom->geom_id);

			//if has body, remove body and this geom
			if (bodyid)
			{
				Body *body = (Body*)dBodyGetData(bodyid);

				delete geom;
				delete body;
			}
			else //static geom, hmm...
			{
				if (geom->TMP_pillar_geom)
				{
					//pillar that should be getting a body (to detach), and buffer refill
					dBodyID body = dBodyCreate(world);
					new Body(body, geom->object_parent);

					//mass
					dMass m;
					dMassSetBox (&m, 1, 2,2,5);
					dMassAdjust (&m, 200); //200kg
					dBodySetMass(body, &m);

					//position
					const dReal *pos = dGeomGetPosition(geom->geom_id);
					dBodySetPosition(body, pos[0], pos[1], pos[2]);

					//attach
					dGeomSetBody(geom->geom_id, body);


					//reset buffer
					geom->Increase_Buffer(4000);
				}
			}
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
