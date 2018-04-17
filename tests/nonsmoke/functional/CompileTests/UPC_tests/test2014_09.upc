typedef struct {
shared int * ptr;
// shared int * shared * pptr;
// shared int * shared * shared * ppptr;
} SupLib;

void subroutine(shared SupLib *suplib) {
        int i;
        i = *(suplib->ptr);
     // i = **(suplib->pptr);
     // i = ***(suplib->ppptr);
     // *(suplib->ptr) = i;
     // **(suplib->pptr) = i;
     // ***(suplib->ppptr) = i;
}
