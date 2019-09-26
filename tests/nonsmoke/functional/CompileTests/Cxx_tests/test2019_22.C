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
          void foobar();
   };

void C::foobar() 
   {
  // data_member can not be accessed though the private base A class of the public base class B
     ::A::static_data_member = 3;
   }

// We don't need this to demonstrate the error.
int A::static_data_member = 0;
