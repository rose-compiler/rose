// Explicit conversion operators

// C++98 added the explicit keyword as a modifier on constructors to prevent single-argument constructors 
// from being used as implicit type conversion operators. However, this does nothing for actual conversion 
// operators. For example, a smart pointer class may have an operator bool() to allow it to act more like 
// a primitive pointer: if it includes this conversion, it can be tested with if(smart_ptr_variable) (which 
// would be true if the pointer was non-null and false otherwise). However, this allows other, unintended 
// conversions as well. Because C++ bool is defined as an arithmetic type, it can be implicitly converted 
// to integral or even floating-point types, which allows for mathematical operations that are not intended 
// by the user.

// In C++11, the explicit keyword can now be applied to conversion operators. As with constructors, it 
// prevents the use of those conversion functions in implicit conversions. However, language contexts that 
// specifically require a boolean value (the conditions of if-statements and loops, as well as operands to 
// the logical operators) count as explicit conversions and can thus use a bool conversion operator.

// #error "NEED AND EXAMPLE OF THIS!"

struct X {
    //implicit conversion
    operator int() const { return 7; }
 
    // explicit conversion
    explicit operator int*() const { return nullptr; }
 
//   Error: array operator not allowed in conversion-type-id
//   operator int(*)[3]() const { return nullptr; }
    using arr_t = int[3];
    operator arr_t*() const { return nullptr; } // OK if done through typedef
//  operator arr_t () const; // Error: conversion to array not allowed in any case
};
 
int main()
{
    X x;
 
    int n = static_cast<int>(x);   // OK: sets n to 7
    int m = x;                     // OK: sets m to 7
 
    int* p = static_cast<int*>(x);  // OK: sets p to null
//  int* q = x; // Error: no implicit conversion
 
    int (*pa)[3] = x;  // OK
}

