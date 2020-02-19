struct A
   {
     int i;
     void foo();
     typedef int type;
     struct struct_type {};
   };

struct B
   {
     int i;
     void foo();
     typedef int type;
     struct struct_type {};
   };

struct C : public A, public B
   {
     void foobar()
        {
          A::i;
          A::foo();
          A::type var_1;
          A::struct_type var_2;
        }
   };
