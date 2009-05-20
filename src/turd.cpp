#include "turd.h"

extern struct turd_struct edit_b;

void t_backup( turd_struct *t ) {
	//printf("t:%p t->l:%p t->r:%p t->nxt:%p t->pre:%p\n", t, t->l, t->r, t->nxt, t->pre);
	edit_b.x = t->x;
	edit_b.y = t->y;
	edit_b.z = t->z;
	edit_b.a = t->a;
	edit_b.b = t->b;
	edit_b.c = t->c;
}

void t_restore( turd_struct *t ) {
	t->x = edit_b.x;
	t->y = edit_b.y;
	t->z = edit_b.z;
	t->a = edit_b.a;
	t->b = edit_b.b;
	t->c = edit_b.c;
}
