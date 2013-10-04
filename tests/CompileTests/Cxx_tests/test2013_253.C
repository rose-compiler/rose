// Note that this is similar to the test2013_46.c example, but the explicit cast is required.
// It is however a bug in ROSE since the generated name of the unnamed variable is unparsed 
// instead of the value of the constant.
int *sample_fmts_alt = (int[]) { 2,3 };

// This is not legal code in C++, but it is valid for C code.
// int *sample_fmts_alt = { 2,3 };


void foo()
   {
     sample_fmts_alt[0] = 4;
   }


