#include "../shared/joint.hpp"
#include "../shared/internal.hpp"

//set event
void Joint::Set_Event(dReal thres, dReal buff, script_struct *scr)
{
	if (thres > 0 && buff > 0 && scr)
	{
		printlog(2, "setting Joint event");
		feedback=new dJointFeedback;
		dJointSetFeedback (joint_id, feedback);

		threshold=thres;
		buffer=buff;
		script=scr;

		//make sure no old event is left
		event=false;
	}
	else
	{
		printlog(2, "disabling Joint event");
		//remove feedback data
		if (feedback)
		{
			delete feedback;
			feedback=NULL;
		}

		//disable
		threshold = 0;
		dJointSetFeedback(joint_id, 0);
	}
}



//check for joint triggering
void Joint::Physics_Step (void)
{
	Joint *d = Joint::head;
	dReal delt1, delt2, delt;

	while (d)
	{
		delt1 = dLENGTH(d->feedback->f1);
		delt2 = dLENGTH(d->feedback->f2);

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
