class A
   {
     public:
          static int static_data_member;
   };

class B : private A 
   {
   };

class C: public B 
   {
     public:
          void foo();
   };


void C::foo()
   {
     ::A::static_data_member = 3;
   }
