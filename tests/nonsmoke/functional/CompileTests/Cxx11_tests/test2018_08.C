
#if 0
Unparser error in rose 0.9.9.192 identityTranslator.  Symptom is error like :

rose_foo.cc(392): error: identifier "bar" is undefined
          int bat = n *  bar;

This can be reproduced with this code:
#endif

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

#if 0
Which gets unparsed to this code, with the "parm3->" missing:

typedef struct struct1_s {
int member1;}struct1_t;
template < typename kernel_type >
inline void for_all ( kernel_type && kernel )
{
}

static void func1(struct1_t *parm3)
{
  ::for_all<  > ( [=] (int int1,int int2)
{
    int local1 =  member1;
  });
}
#endif
