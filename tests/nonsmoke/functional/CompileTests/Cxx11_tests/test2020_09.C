struct A
   {
     enum type { LAST };
   };

struct B
   {
     enum type { LAST };
   };

struct C : public A, public B
   {
     void foobar()
        {
          A::type var_1;
        }
   };
