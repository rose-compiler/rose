// This test code demonstrates where a function pointer typedef is 
// unparsed with too many "*"'s

// Should unparse to be: typedef void *(*swig_converter_func)(void *);
// but was unparsed as:  typedef void * *(*swig_converter_func)(void *);
typedef void *(*swig_converter_func)(void *);

// Function pointer declarations (not using typedef)
void *(*correctFunctionPointer)(void *) = 0L;
void *(*altCorrectFunctionPointer)(void *) = 0L;

// Function pointer declaration using typedef
swig_converter_func incorrectFunctionPointer;

void foo()
   {
  // This is correct and works even if the bug is present
     altCorrectFunctionPointer = correctFunctionPointer;

  // Assign these to generate error (in compilation of unparsed code if types are unparsed to be the same)
     correctFunctionPointer = incorrectFunctionPointer;
   }

class X {};

X* x = 0L;
