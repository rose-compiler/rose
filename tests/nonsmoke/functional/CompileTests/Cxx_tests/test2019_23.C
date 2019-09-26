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

// class C: public B 
class C : private B 
// class C 
   {
  // A::static_data_member is NOT visible within C
     public: 
          void foobar();
   };

void C::foobar() 
   {
  // Class A is inaccessible, because it is hidden by the nested base class (no matter how deep the nesting where A is private).
     ::A a;
   }

