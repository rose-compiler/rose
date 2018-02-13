typedef struct struct1_s {
   int member1;
} struct1_t;

template < typename kernel_type >
inline void for_all(kernel_type&& kernel ) {}

static void func1(struct1_t* parm3) {
   for_all< > ( [=] (int int1, int int2) {
         int local1 = parm3->member1;
   } );
}
