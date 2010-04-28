/*
 * RCX Copyright (C) Slinger
 *
 * This program comes with ABSOLUTELY NO WARRANTY!
 *
 * This is free software, and you are welcome to
 * redistribute it under certain conditions.
 *
 * See license.txt and README for more info
 */

#include "profile.hpp"

Profile *profile_head = NULL;

/*Profile *allocate_profile(void)
{
	printlog(2, "allocating profile");
	profile *profile_next = profile_head;
	profile_head = (profile *)malloc(sizeof(profile));

	//add to list
	profile_head->prev=NULL;
	profile_head->next=profile_next;
	if (profile_next)
		profile_next->prev=profile_head;
	else
		printlog(2, "(first registered)");

	//default values
	profile_head->car = NULL;


	profile_head->steer_speed = 1;
	profile_head->steer_max = 1;
	profile_head->throttle_speed = 1;

	//map all input keys to questionmark
	profile_head->up = UNUSED_KEY;
	profile_head->down = UNUSED_KEY;
	profile_head->right = UNUSED_KEY;
	profile_head->left = UNUSED_KEY;
	profile_head->soft_break = UNUSED_KEY;
	profile_head->drift_break = UNUSED_KEY;

	profile_head->cam_x_pos = UNUSED_KEY;
	profile_head->cam_x_neg = UNUSED_KEY;
	profile_head->cam_y_pos = UNUSED_KEY;
	profile_head->cam_y_neg = UNUSED_KEY;
	profile_head->cam_z_pos = UNUSED_KEY;
	profile_head->cam_z_neg = UNUSED_KEY;

	return profile_head;
}*/


void Profile_Remove (Profile *target)
{
	printlog(2, "removing profile");

	//remove from list
	if (!target->prev) //head
		profile_head = target->next;
	else //not head
		target->prev->next = target->next;

	if (target->next) //not last
		target->next->prev = target->prev;

	//remove profile
	free (target);
}

void Profile_Remove_All()
{
	while (profile_head)
		Profile_Remove(profile_head);
}

