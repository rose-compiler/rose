int a[6] = { [4] = 29, [2] = 15 };

const int v1 = 1;
const int v2 = 2;
const int v4 = 4;

#if defined(__EDG_VERSION__) && __EDG_VERSION__ == 49
// Only literal values are allowed in the generated code and using EDG 4.9 
// this code was normalized to literals, but using EDG 4.11 and great there
// is no such normalization and so it generates non-standard C code (same as
// the input).
int b[6] = { [1] = v1, v2, [4] = v4 };
#endif

// This is a GNU extension...
int widths[] = { [0 ... 9] = 1, [10 ... 99] = 2, [100] = 3 };

int whitespace[256] = { [' '] = 1, ['\t'] = 1, ['\h'] = 1, ['\f'] = 1, ['\n'] = 1, ['\r'] = 1 };

struct point { int x, y; };

const int xvalue = 1;
const int yvalue = 1;

#if defined(__EDG_VERSION__) && __EDG_VERSION__ == 49
// Only literal values are allowed in the generated code and using EDG 4.9 
// this code was normalized to literals, but using EDG 4.11 and great there
// is no such normalization and so it generates non-standard C code (same as
// the input).
struct point p = { .y = yvalue, .x = xvalue };
#endif

const int xv0 = 0;
const int xv2 = 20;
const int yv2 = 2;

// Should be unparsed as: struct point ptarray[10] = { [2].y = yv2, [2].x = xv2, [0].x = xv0 };
// struct point ptarray[10] = { [2].y = yv2, [2].x = xv2, [0].x = xv0 };

// I think this example supports why the designator must be a list (to hold the array
// or references that represents multi-deminsional array indexing such as "[0][0]").
static short grid[3] [4] = { [0][0]=8, [0][1]=6,
                             [0][2]=4, [0][3]=1,
                             [2][0]=9, [2][1]=3,
                             [2][2]=1, [2][3]=1 };
