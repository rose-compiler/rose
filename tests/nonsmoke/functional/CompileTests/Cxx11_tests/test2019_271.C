struct A
   {
     int i;
   };

struct B
   {
     int i;
   };

struct C : public A, public B
   {
     void bar()
        {
          A::i;
          B::i;
        }
   };


