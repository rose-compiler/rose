#include <wchar.h>

#define STR(x) 			L ## x

// typedef wchar wchar_t;

// typedef wchar_t Char;
typedef wchar_t Char;

// const Char hex[] = STR("0123456789ABCDEF");

const Char hex_value = STR('1');

// const Char hex_array[2] = L"12";
const Char hex_array[2] = L"12";

const char char_value = '7';

const char char_array[2] = "34";
