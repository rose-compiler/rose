// This test demonstrates that the template class name sepecified in the 
// preinitialization list is not reset and thus has a name like 
// "inline B::B(int i) : A____L8(i)" instead of "inline B::B(int i) : A<int>(i)"

template <typename T>
class A
   {
      public:
         A(int i) {};
   };

class B : public A<int>
   {
     private:
          int x;
       // A a;  // DQ (4/9/2005): bad code when used with gnu extensions, ok otherwise!
          A<int> a;

     public:
          B(int i): A<int>(i), x(7),a(99) {};
   };

B b(42);

