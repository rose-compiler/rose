/* This is a copy of in13.c */
#ifdef NOTDEF
#define SHARED /**/
#else
#define SHARED shared
#endif

typedef struct {
SHARED int * ptr;
SHARED int * SHARED * pptr;
SHARED int * SHARED * SHARED * ppptr;
} SupLib;

void subroutine(SHARED SupLib *suplib) {
	SHARED int * lptr;
	SHARED int * SHARED * lpptr;
	SHARED int * SHARED * SHARED * lppptr;

	suplib = 0;
	/* suplib = 0; */
	lptr = suplib->ptr;
	/* lptr = ((SupLib *)(((char *)suplib) + MPISMOFFSET)) -> ptr; */
	lpptr = suplib->pptr;
	/* lpptr = ((SupLib *)(((char *)suplib) + MPISMOFFSET)) -> pptr; */
	lppptr = suplib->ppptr;
	/* lppptr = ((SupLib *)(((char *)suplib) + MPISMOFFSET)) -> ppptr; */
}
