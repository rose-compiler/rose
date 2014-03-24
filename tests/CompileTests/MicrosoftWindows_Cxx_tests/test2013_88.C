
class Class_A
   {
     public:
          Class_A();
   };


typedef struct
   {
     Class_A* entry_fn;
     int* integer;
   } tcl_scan;

// template<typename T> 
class Template_Class_A
   {
     public:
          tcl_scan* foo();
   };

// struct xyz
struct
   {
  // Template_Class_A<tcl_scan> scan;
     Template_Class_A scan;
  // tcl_scan scan_2;
   } tcl;


void foobar()
   {
  // This unparses as: class Class_A *x = tcl.scan. foo () -> __anonymous_0x71ef808::entry_fn;
     Class_A* x = tcl.scan.foo()->entry_fn;
  // Class_A* y = tcl.scan_2.foo()->entry_fn;
     int* y = tcl.scan.foo()->integer;
   }

