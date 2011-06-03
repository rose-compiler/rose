#include <iostream>

#include "sage3basic.h"
#include "sageBuilder.h"
#include "SagePythonInterface.h"

/**
 * Implementation of the Sage-Python interface.
 */

using namespace std;
using namespace SageBuilder;

PyObject* PyEncapsulate(void* ptr) {
    return PyCapsule_New(ptr, /* name= */ NULL, /* destructor= */ NULL);
}

/* TODO doc */
PyObject*
sage_buildGlobal(PyObject *self, PyObject *args)
{
    cout << "Hello from buildGlobal()" << endl;
    return Py_BuildValue("i", 0);
}

/* TODO doc */
PyObject*
sage_buildPrintStmt(PyObject *self, PyObject *args)
{
    cout << "Hello from buildPrintStmt()" << endl;
    return Py_BuildValue("i", 0);
}

/* TODO doc */
PyObject*
sage_buildStringVal(PyObject *self, PyObject *args)
{
    char* cstr = NULL;
    PyArg_ParseTuple(args, "s", &cstr); // TODO error handling
    string str = string(cstr);
    SgStringVal* sg_string_val = buildStringVal_nfi(str);
    return PyEncapsulate(sg_string_val);
}
