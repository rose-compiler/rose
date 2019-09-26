struct A
   {
     struct X {};
   };

template<class T>
struct B 
   : public A
   {
     using typename A::X;
   };

