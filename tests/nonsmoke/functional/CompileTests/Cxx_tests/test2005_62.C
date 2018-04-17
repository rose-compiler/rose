
/* Expression as initializers in global scope are not allowed in C, but are fine in C++. */
#if __cplusplus
int x = 1;
int y = 1+2;
int z = x + y;

const int x_const = 1;
const int y_const = 1+2;
const int z_const = x_const + y_const;
#endif

float x_float = 1;
float y_float = 1+2;
#if __cplusplus
float z_float = x_float + y_float;
#endif

const float x_float_const = 1;
const float y_float_const = 1+2;
#if __cplusplus
const float z_float_const = x_float_const + y_float_const;
#endif

#define CONSTANT ((long double) 2.0) / ((long double) 3.0);

// Unparses to "float constant_float = (0.666666686535);"  (8 digits)
float constant_float = CONSTANT;

// Unparses to: "double constant_double = (0.66666666666666663);"  (16 digits)
double constant_double = CONSTANT;

// Unparses to: "long double constant_long_double = (0.66666666666666666668);"  (19 digits)
long double constant_long_double = CONSTANT;

// Unparses to: "long double constant_alt_long_double = (0.66666666666666666668);"  (19 digits)
long double constant_alt_long_double = ((long double) 2.0) / ((long double) 3.0);

// Note: long long double is not supported in EDG
// long long double pi_long_long_double = 3.14159265;

// This is an internal error in EDG!
// void* nullPointer = ((void*)(&x_float_const))[0];
