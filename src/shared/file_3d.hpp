
//file_3d_struct: when a 3d file is loaded, we need a way to keep track of all
//rendering lists, so as to prevent memory leaks when unloading data
//typedef struct graphics_list_struct {
typedef struct file_3d_struct {
//	GLuint render_list;
	GLuint list;
	char *file; //filename (to prevent duplicated 3d loading)
	struct file_3d_struct *next;
} file_3d_struct;

file_3d_struct *allocate_file_3d (void);
extern file_3d_struct *file_3d_head;


