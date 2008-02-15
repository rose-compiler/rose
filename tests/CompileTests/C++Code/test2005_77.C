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

// Template instantiation directives
template A<int>;

int main()
{
   A<int> a;
   a.foo();
}

