// This test code tests the use of pointers to functions and pointers to data
// as handled within typedefs for global data and data within classes.

class alignment_dummy;

#if 0
// Working examples

typedef void (*function_ptr)();
typedef int (alignment_dummy::*member_function_ptr)();

typedef int integerType;

// This fails to compile with ROSE (fixed!)
typedef int (*data_ptr);


// This used to fail to compile with ROSE (failed in the unparser!)
typedef int (alignment_dummy::*member_int_ptr);
typedef float (alignment_dummy::*member_flost_ptr);
#endif

// DQ (2/3/2019): The extra "()" are required only when naming the type in the typedef.
typedef alignment_dummy (alignment_dummy::*member_class_ptr);


