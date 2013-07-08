// This is an un-named struct.
typedef struct
   {
     int* integer;
   } tcl_scan;

class Class_A
   {
     public:
          tcl_scan* foo();
   };

struct
   {
     Class_A scan;
   } tcl;

void foobar()
   {
  // This unparses as: class Class_A *x = tcl.scan. foo () -> __anonymous_0x71ef808::entry_fn;
     int* y = tcl.scan.foo()->integer;
   }

