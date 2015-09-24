#include <wchar.h>

#define STR(x) 			L ## x

// typedef wchar wchar_t;

// typedef wchar_t Char;
typedef wchar_t Char;

// const Char hex[] = STR("0123456789ABCDEF");

const Char hex_value = STR('1');

// const Char hex_array[2] = L"12";
#ifndef __INTEL_COMPILER
// DQ (8/28/2015): from Intel v14 icpc: error: a value of type "int [8]" cannot be used to initialize an entity of type "const Char={wchar_t={int}} [2]"
const Char hex_array[2] = L"12";
#endif

const char char_value = '7';

const char char_array[2] = "34";
