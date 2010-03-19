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
				//break into two pieces
				if (geom->TMP_pillar_geom)
				{
					const dReal *rot = dBodyGetRotation(bodyid);
					dVector3 pos1, pos2;
					dBodyGetRelPointPos(bodyid, 0,0,5.0/4.0, pos1);
					dBodyGetRelPointPos(bodyid, 0,0,-5.0/4.0, pos2);

					//geom1
					dGeomID g = dCreateBox(0, 2,2,5.0/2.0);
					Geom *gd = new Geom(g, geom->object_parent);
					gd->threshold = 100000;
					gd->buffer = 500;

					//body1
					dBodyID b = dBodyCreate(world);
					dMass m;
					dMassSetBox (&m, 1, 2,2,5.0/2.0);
					dMassAdjust (&m, 100); //200kg
					dBodySetMass(b, &m);
					new Body(b, geom->object_parent);
					dBodySetPosition(b, pos1[0], pos1[1], pos1[2]);
					dBodySetRotation(b, rot);
					dGeomSetBody(g,b);

					gd->file_3d = geom->script->graphics_debug2;

					//geom2
					g = dCreateBox(0, 2,2,5.0/2.0);
					gd = new Geom(g, geom->object_parent);
					gd->threshold = 100000;
					gd->buffer = 500;

					//body2
					b = dBodyCreate(world);
					dMassSetBox (&m, 1, 2,2,5.0/2.0);
					dMassAdjust (&m, 100); //200kg
					dBodySetMass(b, &m);
					new Body(b, geom->object_parent);
					dBodySetPosition(b, pos2[0], pos2[1], pos2[2]);
					dBodySetRotation(b, rot);
					dGeomSetBody(g,b);

					gd->file_3d = geom->script->graphics_debug2;
				}
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

					//mass
					dMass m;
					dMassSetBox (&m, 1, 2,2,5);
					dMassAdjust (&m, 200); //200kg
					dBodySetMass(body, &m);

					new Body(body, geom->object_parent);
					//position
					const dReal *pos = dGeomGetPosition(geom->geom_id);
					dBodySetPosition(body, pos[0], pos[1], pos[2]);

					//attach
					dGeomSetBody(geom->geom_id, body);


					//reset buffer
					geom->Increase_Buffer(8000);
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
				if ((geom->flipper_counter+=step) > 10)
					geom->colliding=false;
			}
			else if (geom->flipper_counter>0)
			{
				const dReal *pos;
				pos = dGeomGetPosition(geom->flipper_geom);
				dGeomSetPosition(geom->flipper_geom, pos[0], pos[1],
						pos[2]-step*0.02);
				geom->flipper_counter-=step;
			}
			else
				geom->flipper_counter=0;
		}
		geom=geom->next;
	}
}
