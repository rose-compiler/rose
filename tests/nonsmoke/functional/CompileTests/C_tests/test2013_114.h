
// This ia a function prototype and a CPP macro that permits it to be called with more arguments...from busybox application.
void foobar(int x, int y);
#define foobar(x,y,z) foobar(x+y,y+z)
