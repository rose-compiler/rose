// Mangled nae is: _Fb_v_Gb__Complex__Fe_
void __builtin_my_function(_Complex double);

// Note that use of "Complexx" instead of "Complex" will cause funtion types to be different.
class Complex {};

void my_function(Complex x);

// Unparses as: void MY_Func_Complex(double _Complex value);
// Because mangled nae is: "_Fb_v_Gb__Complex__Fe_" which matches the 
// function type mangled name for the function declaration on line 2.
void MY_Func_Complex (Complex value);

