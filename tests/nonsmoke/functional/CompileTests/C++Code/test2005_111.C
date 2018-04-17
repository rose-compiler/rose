// Milind discovered a bit more about this problem.
// The type of the first parameter is constructed in Sage III as
// a typedef type and the declaration of the typedef type is
// the declaration "int _April_12_2005 = 0;"

// This code appears in rose_edg_macros_and_functions_required_for_gnu.h
// which is in the ROSE/config directory and is copied into the 
// compile tree: ROSE/g++_HEADERS.

// Previously it appeared that the va_start function was eating the first declaration
// appearing after its location.  A simple and unpleasant fix was to give it a
// declaration to eat (hence the variable declaration "int _April_12_2005 = 0;"

#define __builtin_va_start va_start
void va_start(__builtin_va_list __builtin__x, void* __builtin__y);

// DQ (4/12/2005): This is a bizzar EDG bug which I have not figured out (except to verify that it is present).
// Bug: EDG will ignore this declaration following the declaration of 
//      either "void __builtin_va_start(__builtin_va_list &, void*);" or
//      "void va_start(__builtin_va_list &, void*);"
// int _April_12_2005 = 0;

int _July_13_2005 = 0;
