#include "../shared/joint.hpp"
#include "../shared/internal.hpp"
#include "../events/event_lists.hpp"

//set event
void Joint::Set_Buffer_Event(dReal thres, dReal buff, Script *scr)
{
	if (thres > 0 && buff > 0 && scr)
	{
		printlog(2, "setting Joint event");
		feedback=new dJointFeedback;
		dJointSetFeedback (joint_id, feedback);

		threshold=thres;
		buffer=buff;
		buffer_script=scr;

		//make sure no old event is left
		Buffer_Event_List::Remove(this);

		buffer_event=true;
	}
	else
	{
		printlog(2, "disabling Joint event");
		buffer_event=false;
		//remove feedback data
		if (feedback)
		{
			delete feedback;
			feedback=NULL;
		}
		Buffer_Event_List::Remove(this);
		//disable
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
		if (d->buffer_event)
		{
			//TODO: check torque also?
			delt1 = dLENGTH(d->feedback->f1);
			delt2 = dLENGTH(d->feedback->f2);

			if (delt1>delt2)
				delt = delt1 - d->threshold;
			else
				delt = delt2 - d->threshold;

			if (delt > 0)
			{
				if (d->buffer < 0) //already depleted, just damage more
					d->buffer -= delt*internal.stepsize;
				else
				{
					d->buffer -= delt*internal.stepsize;
					if (d->buffer < 0)
					{
						printlog(2, "Joint buffer depleted, generating event");
						new Buffer_Event_List(d);
					}
				}
			}
		}

		d = d->next;
	}
}
