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

#ifndef _RCX_FILE_3D_H
#define _RCX_FILE_3D_H
#include <GL/gl.h>
#include "racetime_data.hpp"

//file_3d_struct: when a 3d file is loaded, we need a way to keep track of all
//rendering lists, so as to prevent memory leaks when unloading data
//typedef struct graphics_list_struct {
class file_3d: public Racetime_Data {
	public:
		GLuint list;
		file_3d();
		~file_3d();
};

#endif
