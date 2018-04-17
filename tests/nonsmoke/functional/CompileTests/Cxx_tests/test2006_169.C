typedef struct
{
  long b;
} A;

void foo(void *x) {
   A *temp = (A *) x;
   A *ap = (A *) new char[sizeof(A) + /*(unsigned)*/(temp->b)];
}
