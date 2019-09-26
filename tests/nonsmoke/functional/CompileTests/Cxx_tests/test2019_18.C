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
     ::A::static_data_member = 3;
   }

int A::static_data_member = 0;
