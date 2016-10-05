#include <wchar.h>

typedef wchar_t Char;

#ifndef __INTEL_COMPILER
// DQ (8/28/2015): from Intel v14 icpc: error: a value of type "int [8]" cannot be used to initialize an entity of type "const Char={wchar_t={int}} [2]"
const Char hex_array[2] = L"12";
#endif
