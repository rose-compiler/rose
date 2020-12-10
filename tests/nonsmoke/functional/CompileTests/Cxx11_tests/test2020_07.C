struct A
   {
     typedef int type;
   };

struct B
   {
     typedef int type;
   };

struct C : public A, public B
   {
     void foobar()
        {
          A::type var_1;
        }
   };
