
// here the left side is an lvalue
#define f_(T, i) \
	typedef T T##_t; \
	T##_t T##_v = i; \
	T##_t *T##_p = &T##_v; \
        T##_p->T##_t::~T##_t();

int main(int argc, char *argv[])
   {
     struct S { char c; };
     typedef char S::*volatile PM;
     f_(PM, 0);

     return 42;
   }


