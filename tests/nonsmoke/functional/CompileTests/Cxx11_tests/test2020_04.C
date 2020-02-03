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
     void foobar()
        {
       // this->A::i;
          A::i;
        }
   };
