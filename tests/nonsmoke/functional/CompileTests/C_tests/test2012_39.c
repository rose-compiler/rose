
// Note that "extern" is critical to demonstrating the bug (after everything else was fixed).
// This test is similar to test2012_37.c but without the "extern" keyword (tests primary variable declarations)
const struct eproto
   {
     const char *s;
     short p;
   } eproto_db[];


