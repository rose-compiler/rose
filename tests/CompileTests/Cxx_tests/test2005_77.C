// This code demonstrates the creation of data members variable declarations outside of the class
// when defined in a template (even if the template in not instantiated).

// A more complex test involves declaring a variable of the template parameter's type! (e.g. "U xyz;")

int x1_global_variable;

template<class U>
class A
   {
     public:
          int x1_data_member;

          void foo();

          int x2_data_member;
   };

int x2_global_variable;


// DQ (2/20/2010): This is a error for g++ 4.x compilers (at least g++ 4.2).
#if (__GNUC__ == 3)
// Template instantiation directives
#if (__GNUC_MINOR__ == 4)
// TOO (2/15/2011): template A<int>; is error for Thrifty (gcc 3.4.4)
template class A<int>;
#else
template A<int>;
#endif
#endif


int main()
{
   A<int> a;
   a.foo();
}


