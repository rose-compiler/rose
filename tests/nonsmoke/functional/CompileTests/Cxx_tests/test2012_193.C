// typedef int (*func_t)(int); --- This works
// typedef int func_t(int);

// This translates into a pointer (tk_pointer)
// typedef int (*good_func_t)(int);

// This translates into atype reference (tk_typeref)
typedef int bad_func_t(void);

// Fails in this statement
// extern func_t my_function; // Should appear like this in unparsed code.
// extern func_t my_function;
// extern good_func_t my_good_function;

// extern bad_func_t my_bad_function;
bad_func_t my_bad_function;


void foo()
   {
     my_bad_function();
   }
