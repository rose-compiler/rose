// This code is from the LLNL Silo project (silo.h)

// This statement appears to be non-relavant to the bug
typedef struct DBfile *___DUMMY_TYPE;  /* Satisfy ANSI scope rules */

typedef struct DBfile_pub 
   {
  // This works!
  // struct DBfile *(*altClose)(int*);

  // This works!
  // struct DBfile *altVariable;

  // This fails!
     int            (*closeFunctionPointer)(struct DBfile *);
   } DBfile_pub;

typedef struct DBfile
   {
     DBfile_pub     pub;
   } DBfile;

