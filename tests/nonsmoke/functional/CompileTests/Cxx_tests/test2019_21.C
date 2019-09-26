class A
   {
     public:
       // A::static_data_member IS (trivially) visible within A
          static int static_data_member;
   };

class B : private A 
   {
  // A::static_data_member IS visible within B
   };

class C: public B 
   {
  // A::static_data_member is NOT visible within C
     public: 
          void foobar();
   };

void C::foobar() 
   {
  // The data_member can not be accessed though the private base A class of the public
  // base class B, so name qualification is required. Also, the global qualified is 
  // required to avoid the restricted access through A which is a base class of C 
  // (through B).  Note that both A and it data member static_data_member require name 
  // qualification for the same reason (private derivation from class A in declaration 
  // of class B).
     ::A::static_data_member = 3;
   }

