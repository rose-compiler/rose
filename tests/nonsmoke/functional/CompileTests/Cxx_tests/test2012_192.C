// This translates into a pointer (tk_pointer) to a function type.
typedef float (*good_func_t)(int);

// This translates into a type reference (tk_typeref)
typedef float strange_func_t(int);

// Fails in this statement
// extern func_t my_function; // Should appear like this in unparsed code.
good_func_t my_good_function;

// extern bad_func_t my_bad_function;
strange_func_t my_strange_function;

// It does not appear to be possible to build the definition in the same way.
// But using the base type of the typedef is equivalent (in this case).
// strange_func_t my_strange_function(int x)
float my_strange_function (int x)
  {
    return 0;
  }

void foo(int x);

void foo( int x )
   {
  // Assignment of function pointer.
     my_good_function = my_strange_function;

  // This is equivalent to the function pointer assignment above.
     my_good_function = &my_strange_function;

  // Calling the strange function.
     my_strange_function(7);
   }
