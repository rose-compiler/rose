template <class T>
class A
   {
     public:
          A();
   };

class B
   {
     public:
          struct C {};
          A<C> x;
   };

