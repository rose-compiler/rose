// This test demonstrates that the template class name sepecified in the 
// preinitialization list is not reset and thus has a name like 
// "inline B::B(int i) : A____L8(i)" instead of "inline B::B(int i) : A<int>(i)"

template <typename T>
class A
   {
      public:
         A(int i) {};
   };

#if ( (__GNUC__ == 3) || (__GNUC__ == 4) && (__GNUC_MINOR__ < 1) )
// This test code does not compile with g++ 4.1.2 (at least), but does compile with g++ 3.4.6
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
#else
#warning "error: specialization of 'A<T>::A(int) [with T = int]' after instantiation"
#endif

