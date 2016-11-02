// This is a similar example to test2013_303.C which shows how
// functions can be overloaded based only on template parameters.
// This example shows how similar classes can be defined, but 
// since overloading of classes is not legal, the classes must
// have different names.

class foo
   {
     public:
          int bar_variable;
   };

// This is the template class for member data.
template <int foo::*M> class B
   {
     public:
          B() {};         
   };

int main()
   {
  // Unparses as: "B< &bar_variable > ();" when specified as: "B<&foo::bar_variable>();" (which is an SgConstructorInitializer).
  // If specified as: "B<&foo::bar_variable> b();" then it will work well (since it is a function declaration).
     B<&foo::bar_variable>();
   }

// Template Member function must be unparsed with header "template < int ::foo::* M >"
// Bug: unparsed as: "template < int (::foo::*) M >"
