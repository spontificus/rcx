#include <SDL/SDL.h>
#include <ode/ode.h>
#include "../shared/joint.hpp"

void Joint::TMP_Events_Step(Uint32 step)
{
	//loop joints
	Joint *joint = Joint::head;
	while (joint)
	{
		if (joint->event)
		{
			//assume the joint should be destroyed
			dJointDestroy (joint->joint_id);
			delete joint;
		}
		joint=joint->next;
	}
}

