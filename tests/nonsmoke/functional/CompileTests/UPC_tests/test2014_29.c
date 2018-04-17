/* This is the in7.c example */
typedef struct {
shared int * ptr;
shared int * shared * pptr;
shared int * shared * shared * ppptr;
} SupLib;

void foo(SupLib ls);

void subroutine(shared SupLib *suplib) {
	SupLib lsuplib;
	lsuplib = *suplib;
	foo(lsuplib);
}
