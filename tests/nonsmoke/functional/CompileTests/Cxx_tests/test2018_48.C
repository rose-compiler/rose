
#if 0
// DQ (4/7/2018): This is a C++11 test, moved to the Cxx11_tests directory.

typedef struct struct1_s
{ int member1; }

struct1_t;

template < typename kernel_type >
inline void for_all(kernel_type&& kernel ) {}

static void func1(struct1_t* parm3) {
for_all< > ( [=] (int int1, int int2)
{ int local1 = parm3->member1; }

);
}

#endif
