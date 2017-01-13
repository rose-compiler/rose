// Test code for case of eok_lvalue_dot_static: calling sage_gen_lvalue()

#if 0
class A
   {
     public:
          void foo();
          int x;
   };
#endif

class B
   {
     public:
       // A a;
          static void foo();
   };

class C
   {
     public:
          B b;
   };

// B C::b;

void foobar()
   {
     B b;

  // A simple static member function call
     b.foo();

  // The following is output as "b.foo()" (so there is some normalization internally in EDG)
     (*(b.foo))();
   }
