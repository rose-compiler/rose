class A
   {
     public:
          static int static_data_member;
   };

class B : private A 
   {
     public: 
          void foobar();
   };

void B::foobar() 
   {
  // This can be referenced as just "static_data_member = 3;" without qualification.
     ::A::static_data_member = 3;
   }

int A::static_data_member = 0;
