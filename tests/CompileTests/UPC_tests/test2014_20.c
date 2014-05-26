typedef struct {
shared int * ptr;
shared int * shared * pptr;
shared int * shared * shared * ppptr;
} SupLib;

void subroutine(shared SupLib *suplib) {
	int i;

// Older version generated correct answers.
// i =  *((int *)(((char *)( *((SupLib *)(((char *)suplib) + MPISMOFFSET))) . ptr) + MPISMOFFSET));
// i =  *((int *)(((char *)( *((int **)(((char *)( *((SupLib *)(((char *)suplib) + MPISMOFFSET))) . pptr) + MPISMOFFSET)))) + MPISMOFFSET));
// i =  *((int *)(((char *)( *((int **)(((char *)( *((int ***)(((char *)( *((SupLib *)(((char *)suplib) + MPISMOFFSET))) . ppptr) + MPISMOFFSET)))) + MPISMOFFSET)))) + MPISMOFFSET));
// *((int *)(((char *)( *((SupLib *)(((char *)suplib) + MPISMOFFSET))) . ptr) + MPISMOFFSET)) = i;
// *((int *)(((char *)( *((int **)(((char *)( *((SupLib *)(((char *)suplib) + MPISMOFFSET))) . pptr) + MPISMOFFSET)))) + MPISMOFFSET)) = i;
// *((int *)(((char *)( *((int **)(((char *)( *((int ***)(((char *)( *((SupLib *)(((char *)suplib) + MPISMOFFSET))) . ppptr) + MPISMOFFSET)))) + MPISMOFFSET)))) + MPISMOFFSET)) = i;

	i = *((*suplib).ptr);
	i = **((*suplib).pptr);
	i = ***((*suplib).ppptr);
	*((*suplib).ptr) = i;
	**((*suplib).pptr) = i;
	***((*suplib).ppptr) = i;
}
