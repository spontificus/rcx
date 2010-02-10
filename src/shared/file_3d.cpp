#include "file_3d.hpp"
#include "printlog.hpp"
#include <stdlib.h>

file_3d_struct *file_3d_head = NULL;

//allocates new link in 3d rendering list
file_3d_struct *allocate_file_3d (void)
{
	printlog(2, " > allocating 3d file storage");

	file_3d_struct *tmp_3d = (file_3d_struct *)malloc (sizeof(file_3d_struct));
	//add to list
	tmp_3d->next = file_3d_head;
	file_3d_head = tmp_3d;

	if (!tmp_3d->next)
		printlog(2, " (first one)\n");
	else
		printlog(2, "\n");

	//default values
	file_3d_head->file = NULL; //filename - here ~no name~
	printlog(2, "TODO: check for already loaded files\n");
	file_3d_head->list = glGenLists(1);

	printlog (2, "\n");
	return file_3d_head;
}


