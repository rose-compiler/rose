
// In this case two class definitions are built and the scope stored within the EDG AST is 
// initialized the first time (to global scope and then reset the second time (to global scope)).
// so it is reset to the same value each time.  This would not have to be the case if they were 
// in the same name space but within separate namespace definitions!

#if 1
typedef struct _IO_FILE __FILE;
struct _IO_FILE 
   {
     int x;
   };
#endif

struct __foo;
// struct __foo {};
// struct __foo;
