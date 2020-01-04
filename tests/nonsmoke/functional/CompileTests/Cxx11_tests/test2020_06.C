struct A
   {
     typedef int type;
     struct struct_type {};
   };

struct B
   {
     typedef int type;
     struct struct_type {};
   };

struct C : public A, public B
   {
     void foobar()
        {
          A::type var_1;
          A::struct_type var_2;
        }
   };
