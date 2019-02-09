class AA
   {
     public:
          static int another_static_data_member;
   };

class A : public AA
   {
     public:
          static int static_data_member;
   };

// BUG: This causes both A and AA to become inaccessible (but only A is markes as such).
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
     ::AA::another_static_data_member = 3;
   }
