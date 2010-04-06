#include <SDL/SDL.h>
#include <ode/ode.h>
#include "../shared/joint.hpp"
#include "../events/event_lists.hpp"

void Joint::TMP_Events_Step(Uint32 step)
{
	//loop joints
	Joint *joint;
	while (Event_Lists::Get_Event(&joint))
	{
		//assume the joint should be destroyed
		delete joint;
	}
}

