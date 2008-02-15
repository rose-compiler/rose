// This is the only code that demonstrates a current ROSE bug (in Kull)
// I could not reproduce it in anything smaller and I have yet to 
// understand the problem. :-(

// This code demonstrates a failure!
#include "Python.h"

// class PyObject {};
// typedef struct my_object {	PyObject_HEAD } myPyObject;

// ROSE presently generates the following code (I can't explain how the 
// "Py_complex" appears in the parameter list of the pointer to a function!)
//      extern void foo(PyObject *(*xyz)(Py_complex ));
void foo( PyObject *(*xyz)() );

#if 0
// This code is not needed to demonstrate the error, but it will force the error to be a compile error
static PyObject *_wrap_progname_get() { return 0; }

void SWIG_init(void) 
   {
     foo(_wrap_progname_get );
   }
#endif
