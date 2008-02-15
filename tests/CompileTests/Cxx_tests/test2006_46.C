// Lingxiao's bug:  Calling unparseToString at the "typedef struct B_s"
// in a translator causes an assertion failure.   It also fails in the 
// unparser (simplest possible test).

struct B_s;
 typedef struct B_s {
   int nblx;
 } B_t;
