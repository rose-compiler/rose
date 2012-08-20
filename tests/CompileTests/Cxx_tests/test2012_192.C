// typedef int (*func_t)(int); --- This works
// typedef int func_t(int);

// This translates into a pointer (tk_pointer)
// typedef int (*good_func_t)(int);

// This translates into atype reference (tk_typeref)
typedef float bad_func_t(int);

// Fails in this statement
// extern func_t my_function; // Should appear like this in unparsed code.
// extern func_t my_function;
// extern good_func_t my_good_function;

// extern bad_func_t my_bad_function;
bad_func_t my_bad_function;

// The way that we specify functions is equivalent to using the typedef's base type directly.
// float alt_bad_function(int);

#if 1
void foo(int x);

void foo( int x )
   {
     my_bad_function(7);
   }
#endif
