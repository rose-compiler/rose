// This test code demonstrates output of the template member function 

// template function containing member function that would 
// be an error to instantiate with a primative type
template <typename T>
class X
   {
     private:
          T t;

     public:
          int foo()
             {
            // This would be an error if T was a primative type
               return t++;
             }
   };

// Template Instantiation Directive
// This would fail if the specialization is not output properly
// template X<int>;

#if 1
int main()
   {
     X<int> b;
     b.foo();
   }
#endif

