struct A
   {
     struct struct_type {};
   };

struct B
   {
     struct struct_type {};
   };

struct C : public A, public B
   {
     void foobar()
        {
          A::struct_type var_2;
        }
   };
