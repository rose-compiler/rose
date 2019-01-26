class B 
   {
     public:
          int data_member;                 // nonstatic member
          static int static_data_member;          // static member
   };

class D: private B {};

class DD: public D 
   {
     public: int f();
   };

int DD::f() 
   {
     ::B b;
     b.data_member = 1;               // OK (b.data_member is different from this->data_member)
     b.static_data_member = 2;        // OK (b.static_data_member is different from this->si)
  // ::B::static_data_member = 3;     // OK
     ::B* bp2 = (::B*)this;           // OK with cast
     bp2->data_member = 4;            // OK: access through a pointer to B
  // return 1000*b.data_member + 100*b.static_data_member + 10*(::B::static_data_member) + bp2->data_member;
     return 0;
   }

int B::static_data_member = 0;
