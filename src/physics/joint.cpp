#include "../shared/joint.hpp"
#include "../shared/internal.hpp"

//check for joint triggering
void joint_physics_step (void)
{
	joint_data *d = joint_data_head;
	dReal delt1, delt2, delt;

	while (d)
	{
		delt1 = dLENGTH(d->feedback.f1);
		delt2 = dLENGTH(d->feedback.f2);

		if (delt1>delt2)
			delt = delt1 - d->threshold;
		else
			delt = delt2 - d->threshold;

		if (delt > 0)
		{
			d->buffer -= delt*internal.stepsize;
			if (d->buffer < 0)
				d->event = true;
		}

		d = d->next;
	}
}
