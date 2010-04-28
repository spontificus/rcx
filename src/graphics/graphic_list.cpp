#include "graphic_list.hpp"

#include "../shared/printlog.hpp"
#include "../shared/geom.hpp"
#include "../shared/body.hpp"

#include <stdlib.h>
#include <GL/gl.h>
#include <ode/ode.h>

//just normal (component) list for now:

//consists of two buffers:
//each element in buffer:
struct list_element
{
	GLfloat matrix[16]; //4x4
	GLuint  list; //render list
};

//keeps track of a buffer of elements:
struct list_buffer
{
	size_t count;
	list_element *list;
};

size_t buffer_size = INITIAL_GRAPHIC_LIST_BUFFER_SIZE;

//buffers
list_buffer buffer1 = {0, (list_element*)malloc(sizeof(list_element)*buffer_size)};
list_buffer buffer2 = {0, (list_element*)malloc(sizeof(list_element)*buffer_size)};

//pointers at buffers
list_buffer *buffer_in = &buffer1; //filled with data
list_buffer *buffer_out = &buffer2; //rendered



void Graphic_List_Update()
{
	//pointers:
	list_buffer *tmp=buffer_in;
	size_t *count=&(tmp->count);
	list_element *list=tmp->list;

	*count=0; //set to zero (empty)

	//variables
	const dReal *pos, *rot;
	GLfloat *matrix;

	for (Geom *g=Geom::head; g; g=g->next)
	{
		if (g->f_3d)
		{
			pos = dGeomGetPosition(g->geom_id);
			rot = dGeomGetRotation(g->geom_id);
			matrix = list[*count].matrix;

			//set matrix
			matrix[0]=rot[0];
			matrix[1]=rot[4];
			matrix[2]=rot[8];
			matrix[3]=0;
			matrix[4]=rot[1];
			matrix[5]=rot[5];
			matrix[6]=rot[9];
			matrix[7]=0;
			matrix[8]=rot[2];
			matrix[9]=rot[6];
			matrix[10]=rot[10];
			matrix[11]=0;
			matrix[12]=pos[0];
			matrix[13]=pos[1];
			matrix[14]=pos[2];
			matrix[15]=1;

			//set what to render
			list[*count].list = g->f_3d->list;

			//if buffer full...
			if (++(*count) == buffer_size)
			{
				printlog(1, "Note: Graphic_List buffers were too small, resizing");

				buffer_size+=INITIAL_GRAPHIC_LIST_BUFFER_SIZE;
				buffer1.list = (list_element*) realloc(buffer1.list, sizeof(list_element)*buffer_size);
				buffer2.list = (list_element*) realloc(buffer2.list, sizeof(list_element)*buffer_size);
			}
		}
	}
	//same as above, but for bodies... disabled for now
	/*for (Body *b=Body::head; b; b=b->next)
	{
		if (b->f_3d)
		{
			pos = dBodyGetPosition(b->body_id);
			rot = dBodyGetRotation(b->body_id);
			matrix = list[*count].matrix;

			//set matrix
			matrix[0]=rot[0];
			matrix[1]=rot[4];
			matrix[2]=rot[8];
			matrix[3]=0;
			matrix[4]=rot[1];
			matrix[5]=rot[5];
			matrix[6]=rot[9];
			matrix[7]=0;
			matrix[8]=rot[2];
			matrix[9]=rot[6];
			matrix[10]=rot[10];
			matrix[11]=0;
			matrix[12]=pos[0];
			matrix[13]=pos[1];
			matrix[14]=pos[2];
			matrix[15]=1;

			//set what to render
			list[*count].list = g->f_3d->list;

			//if buffer full...
			if (++(*count) == buffer_size)
			{
				printlog(1, "Note: Graphic_List buffers were too small, resizing");

				buffer_size+=INITIAL_GRAPHIC_LIST_BUFFER_SIZE;
				buffer1.list = (list_element*) realloc(buffer1.list, sizeof(list_element)*buffer_size);
				buffer2.list = (list_element*) realloc(buffer2.list, sizeof(list_element)*buffer_size);
			}
		}
	}*/
	
}

void Graphic_List_Render()
{
	//see if in buffer got any data, if so switch
	if (buffer_in->count) //nonzero
	{
		list_buffer *tmp=buffer_out;
		buffer_out=buffer_in;
		buffer_in = tmp;

		buffer_in->count=0; //indicates empty
	}

	//copy needed data
	size_t count=buffer_out->count;
	list_element *list=buffer_out->list;

	for (size_t i=0; i<count; ++i)
	{
		glPushMatrix();
			glMultMatrixf (list[i].matrix);
			glCallList (list[i].list);
		glPopMatrix();
	}

}
