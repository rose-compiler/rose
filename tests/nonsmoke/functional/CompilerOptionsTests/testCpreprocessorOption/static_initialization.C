// P++ requires that either COMPILE_PPP or COMPILE_SERIAL_APP be defined
// where as A++ defines COMPILE_APP in config.h (P++ can't do that and
// still maintain simplicity using AutoMake)
// So if we don't see COMPILE_APP then we assume we are compiling for P++!
#if !defined(COMPILE_APP)
#define COMPILE_PPP
#endif

#include "A++.h"

int APP_Global_Array_Base =  0;
Range APP_Unit_Range (0,0);

// This function is called in the Initialization function for the array objects
// its purpose is to force this file (which contains the APP_Global_Array_Base
// and APP_Unit_Range variabels above) to be included at compile time and
// thus force the static constructors to be called.  The problem was that
// the Overture dynamic link libraries where using the APP_Unit_Range
// variable but the users application might not be using it and so it
// was never included and thus never initialized (static constructor called
// at load time).  Hopefully this will not cause problems on other architectures
// using different compilers.
void APP_Null_Initialization_Function(void) {};

// The implementation of the block where option with expression templates
// is best done using a global where mask pointer (rather than as a static
// member of the where support class).
// intArray* & APP_Global_Where_Mask_Pointer = Where_Statement_Support::Where_Statement_Mask;

// The KCC compiler wants to see these when building the 
// libA++_static.a library from static_initialization.o
// BUT - We can't define this twice (once in the users application and here)
// It was defined here because we were having trouble getting KCC to build
// us a static library and this seemed to help, though I don't recall why.
// void Internal_Index::Consistency_Check(const char *) const {};
// int main () {}

