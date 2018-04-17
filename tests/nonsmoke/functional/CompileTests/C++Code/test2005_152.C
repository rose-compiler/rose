// Test code for overloaded member functions of a templated class where at least 
// one is a template function (as opposed to a non-template member fucnction).

// this is a class template
template <typename T>
class X
   {
     public:
       // This is an overloaded non-template member function
          void foobar(T t){};
#if 0
       // This is an overloaded non-template member function
          void foobar(T t, int i ){};
#endif
#if 1
       // This is an overloaded template member function
          template <typename S>
          void foobar(S t){};
#endif
   };

void foo()
   {
     X<int> a;

     a.foobar(1);
  // a.foobar(1,2);
     a.foobar(3.14159265);
   }


// Make this compile and link
int main()
   {
     return 0;
   }
