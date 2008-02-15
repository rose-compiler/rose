// Friend template functions are not correctly marked as friend (internally)
// The unparser likely works to generate the correct code because the template
// declaration is saved (and output) as a string.

template<typename T> 
void foo();

template<typename T>
class X 
   {
     public:
       // The generated code does not pass g++ (T shadows previous use of T)
       // template<typename T> friend void foo();
          template<typename S> friend void foo();
   };

void foobar()
  {
    X<int> x;
    foo<int>();
  }

