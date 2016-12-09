void subroutine(shared int * ptr, shared int * shared * pptr, shared int * shared * shared * ppptr) {
        int i;
        i = *ptr;
        i = **pptr;
        i = ***ppptr;
        *ptr = i;
        **pptr = i;
        ***ppptr = i;
}
