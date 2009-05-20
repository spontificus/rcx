#include "turd.h"

void turd::setup( float x,float y,float z, float a,float b,float c ) {
//	printf("x:%f y:%f z:%f a:%f b:%f c:%f\n", x,y,z, a,b,c);
	this->x = x;
	this->y = y;
	this->z = z;
	this->a = a;
	this->b = b;
	this->c = c;
}

void turd::makeMatricies() {
	float *mvr;
	
	// store current matrix
	glGetFloatv(GL_MODELVIEW_MATRIX,m);
	
	// real world coords
	mvr = mbv(m, 0,0,0);
	wx = mvr[0];
	wy = mvr[1];
	wz = mvr[2];

	// (x-axis)
	mvr = mbv(m, 1,0,0);
	xnx = mvr[0];
	xny = mvr[1];
	xnz = mvr[2];

	// direction of travel (y-axis)
	mvr = mbv(m, 0,1,0);
	ynx = mvr[0];
	yny = mvr[1];
	ynz = mvr[2];

	// (z-axis)
	mvr = mbv(m, 0,0,1);
	znx = mvr[0];
	zny = mvr[1];
	znz = mvr[2];

	// and the actual normal
	mvr = mbv(m, 0,0,1);
	anx = mvr[0] - wx;
	any = mvr[1] - wy;
	anz = mvr[2] - wz;
}
	
	
void turd::calc() {
	glPushMatrix();		
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	turd *t = this;
	
	// go through entire list, using gl to process the relational
	// offsets, and store the matrix for each node, which we'll
	// use later. It would perhaps be nicer to use our own matrix
	// math library, rather than invoking gl, but i don't know how
	// to do that, and this works for now.
	//
	while ( t ) {
		glTranslatef(t->x,t->y,t->z);
		glRotatef(t->a,1,0,0);
		glRotatef(t->b,0,1,0);
		glRotatef(t->c,0,0,1);
		t->makeMatricies();
		
		glPushMatrix();
		glTranslatef(t->l->x, t->l->y, t->l->z);
		glRotatef(t->l->a,1,0,0);
		glRotatef(t->l->b,0,1,0);
		glRotatef(t->l->c,0,0,1);
		t->l->makeMatricies();
		glPopMatrix();		
				
		glPushMatrix();
		glTranslatef(t->r->x, t->r->y, t->r->z);
		glRotatef(t->r->a,1,0,0);
		glRotatef(t->r->b,0,1,0);
		glRotatef(t->r->c,0,0,1);
		t->r->makeMatricies();
		glPopMatrix();
			
		t = t->nxt;
	}
	glPopMatrix();
	
}


void t_backup( turd *t ) {
	//printf("t:%p t->l:%p t->r:%p t->nxt:%p t->pre:%p\n", t, t->l, t->r, t->nxt, t->pre);
	edit_b.x = t->x;
	edit_b.y = t->y;
	edit_b.z = t->z;
	edit_b.a = t->a;
	edit_b.b = t->b;
	edit_b.c = t->c;
}


void t_restore( turd *t ) {
	t->x = edit_b.x;
	t->y = edit_b.y;
	t->z = edit_b.z;
	t->a = edit_b.a;
	t->b = edit_b.b;
	t->c = edit_b.c;
}


void turd::load(const char *filename) {
	FILE *fp;
	char buf[100];
	void *ptr;

#ifdef windows
	fp = fopen(filename, "rb");
#else
	fp = fopen(filename, "r");
#endif


	float ix,iy,iz,ia,ib,ic;
	char sec;

	turd *tmp_turd = NULL;
	turd *last_turd = NULL;
	turd *bast_turd = NULL;

	float mod=15;
	float xmod=10;

	int count = 0;
	int res;
	int first = 1;

	printlog(0, "Loading Trackfile: %s\n", filename);

	while ( (ptr = fgets((char *)&buf, 100, fp)) ) {
		count++;
		res = sscanf(buf, "%f %f %f %f %f %f %c", &ix, &iy, &iz, &ia, &ib, &ic, &sec);
//		printf("res:%d\n", res);
		if ( res != 7 ) {
			res = sscanf(buf, "%f %f %f %f %f %f", &ix, &iy, &iz, &ia, &ib, &ic);
			if ( res != 6 ) {
				printlog(0, "Track format not recognised\n");
				exit(1);
			}
		
			printlog(0, "add track section type (c,l,r) to end of line\n");
			sec = 'c';
		}

		ix *= mod;
		iy *= mod;
		iz *= mod;
		

		switch ( sec ) {
			case 'c':
				if ( first == 1 ) {
					tmp_turd = this;
					first = 0;
				} else {
					tmp_turd = new turd();
				}
				
				tmp_turd->setup(ix,iy,iz, ia,ib,ic);
				//printf("x:%f y:%f z:%f\n", tmp_turd->x,tmp_turd->y,tmp_turd->z);

				// left and right side of road are offset from center
				bast_turd = new turd();
				bast_turd->setup( -xmod,0,0, 0,0,0 );
				bast_turd->r = tmp_turd;
				tmp_turd->l = bast_turd;

				bast_turd = new turd();
				bast_turd->setup( xmod,0,0, 0,0,0 );
				bast_turd->l = tmp_turd;
				tmp_turd->r = bast_turd;

				if (last_turd != NULL) {
					last_turd->nxt = tmp_turd;
					tmp_turd->pre = last_turd;
					
					last_turd->l->nxt = tmp_turd->l;
					last_turd->r->nxt = tmp_turd->r;
					tmp_turd->l->pre = last_turd->l;
					tmp_turd->r->pre = last_turd->r;
				}

				last_turd = tmp_turd;
				break;
				
			case 'l':
//				printf("l\n");
				tmp_turd->l->setup(ix,iy,iz, ia,ib,ic);
				break;
			
			case 'r':
//				printf("r\n");
				tmp_turd->r->setup(ix,iy,iz, ia,ib,ic);
				break;
				
			default:
				printf("Shouldn't be here (%c)\n", sec);
				break;
		}
		
		
	}
	
	fclose(fp);

	printf("--\n");
	this->calc();
	
	
	// yeach - a holder for the global list
	tmp_turd = new turd();
	
	if ( turd_head == NULL ) {
		// i kan coed gud
		turd_head = tmp_turd;
		tmp_turd->l = tmp_turd;
		tmp_turd->r = tmp_turd;
		
		edit_t = this;
		edit_h = this;
	}
	
	tmp_turd->nxt = this;
	tmp_turd->l = turd_head;
	tmp_turd->r = turd_head->r;
	turd_head->r->l = tmp_turd;
	turd_head->r = tmp_turd;
		
	calllist = 0;
	redraw = 1;
	

	tri = new trimesh(this);
}

