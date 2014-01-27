
#define NS(x) x
#define ns(x) x
#include "test2014_20_inc.c"
#undef NS
#undef ns

// #ifdef XML_NS

#define NS(x) x ## NS
#define ns(x) x ## _ns

#include "test2014_20_inc.c"

#undef NS
#undef ns

