// This test code demonstrates output of the template member function 
// In this case the constructor can not be output as a prototype (error)!
// This should imply that the constructor could not be transformed within
// just the instantiation! however, this might still be the case since the 
// constructor specialization member function definition can be output, its
// prototype just can't be output!

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

         X() : t(0) {}
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

