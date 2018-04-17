
struct point { int x, y; };

const int xv0 = 0;
const int xv2 = 20;
const int yv2 = 4;

// Should be unparsed as: struct point ptarray[10] = { [2].y = yv2, [2].x = xv2, [0].x = xv0 };
// Or at least: struct point ptarray[10] = {[2UL].y = (2), [2UL].x = (20), [0UL].x = (0)};
// Currently unparses as: struct point ptarray[10] = {[2UL] = ({.y = (2)}), [2UL] = ({.x = (20)}), [0UL] = ({.x = (0)})};
// struct point ptarray[10] = { [2].y = yv2, [2].x = xv2, [0].x = xv0 };

#if defined(__EDG_VERSION__) && __EDG_VERSION__ == 49
// Only literal values are allowed in the generated code and using EDG 4.9 
// this code was normalized to literals, but using EDG 4.11 and great there
// is no such normalization and so it generates non-standard C code (same as
// the input).
// Original code:
// struct point ptarray[10] = { [2].y = yv2 };
struct point ptarray[10] = { [2].y = yv2 };
#endif
