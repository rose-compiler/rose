// This is an un-named struct.
typedef struct
   {
     int* integer;
   } tcl_scan;

tcl_scan* foo();

void foobar()
   {
  // This unparses as: int *z = foo() -> __anonymous_0x113675c8::integer;
     int* z = foo()->integer;
   }

