// DQ (1/19/2004): test code showing bug!  Currently no application or test code demonstrates this error!

/*
   What I think is happening:
      The first time the struct _IO_FILE is seen it checks to see if it
      has been previously output and it has not so the full type (with 
      class definition) is output.  Then the 2nd time, although the
      class definition has been output it is not marked as an autonomous 
      declaration so it is output.  The output of both definitions in the 
      final unparsed code results in a redefinition of the struct _IO_FILE
      type and a compile time error in the back-end compilation (bad 
      code is generated).
 */

#if 1
// typedef of forward declaration of type
// typedef struct _IO_FILE _IO_FILE;
typedef struct _IO_FILE __FILE;

typedef struct _IO_FILE
   {
     int _flags;
   } _IO_FILE;

typedef struct _IO_FILE __FILE;
typedef struct _IO_FILE _IO_FILE;
typedef struct _IO_FILE _IO_FILE;

#else

typedef struct _IO_FILE
   {
     int _flags;
   } __FILE;

// Test of redundent typedefs
typedef struct _IO_FILE _IO_FILE;
typedef struct _IO_FILE _IO_FILE;

// typedef struct _IO_FILE _IO_FILE;

#endif

#if 0
typedef struct X
   {
     int x;
   } X;
#endif
