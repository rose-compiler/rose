
// This is currently and error on ROSE.
// The unparsed output does not include the "{ int x; }" part
// and so the resulting file does not compile.  I appears that
// the problem is in the EDG/SAGE connection. The constructs
// representing "{ int x; }" don't appear to make it iinto SAGE.
// Using the EDG -> C++ test program yields the correct result.
// so the problem appears to be in the EDG/SAGE connection.

typedef struct
   {
     int x;
   } mystruct;

mystruct X;

/*
#if 0
typedef int myint;
#endif

#if 0
typedef enum
   {
     myTrue,
     myFalse
   } myBoolean;
#endif
*/

// mystruct Astruct;
