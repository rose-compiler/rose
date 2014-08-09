
typedef struct {

} A;

typedef struct {
   A a;
   shared A * a0;
} B;

void foobar()
{
    shared B * b = 0;
    void myfunc(shared A *);
    myfunc(b->a0);
    myfunc(&(b->a));
}

