
struct Y
   {
  // template<typename T> void foo();
  // template<typename T> void foo() restrict;

  // DQ (3/8/2012): Note that currently, function type modifiers are not unparsed.
  // typedef void *restrict_function_type() __restrict__;
     typedef void *restrict_function_type();
  // typedef void (*restrict_function_type() const);
     typedef int Y::*restrict_member_function_type() const;

  // void foo() __restrict__;
  // void foo() const;

  // template<typename T> void foo() __restrict__;
  // template<typename T> void foo() const;
   };


// This type qualifier is not allowed on a nonmember function
// void foo() __restrict__; // error
