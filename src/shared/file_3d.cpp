#include "file_3d.hpp"
#include "printlog.hpp"
#include <stdlib.h>

//allocates new link in 3d rendering list
file_3d::file_3d():Racetime_Data("tmp")
{
	printlog(2, "allocating 3d file storage");

	//default values
	printlog(2, "TODO: check for already loaded files");
	list = glGenLists(1);
}


file_3d::~file_3d()
{
	glDeleteLists (list, 1);
}
