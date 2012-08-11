// Class containing member function not availabe to primative types
class A
   {
     public:
          int increment() { return 0; }
   };

// Template class containing member function (defined outside the class)
template <typename T>
class X
   {
     private:
          T t;

     public:
          int foo();
   };

template <typename T>
int X<T>::foo()
   {
  // This would be an error if T was a primative type
     return t.increment();
   }

template <>
int X<int>::foo()
   {
     int xxxxxxxxx;
     return xxxxxxxxx;
   }

int main()
   {
     X<A> a;
     X<int> b;
     a.foo();
     b.foo();
   }

