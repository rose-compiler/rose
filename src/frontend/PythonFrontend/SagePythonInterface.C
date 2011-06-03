#include <iostream>

#include "sage3basic.h"
#include "sageBuilder.h"
#include "SagePythonInterface.h"

/**
 * Implementation of the Sage-Python interface.
 */

using namespace std;
using namespace SageBuilder;

/*
 * Encapsulate the pointer in a Python object. 
 * Use no name or destructor.
 */
PyObject* PyEncapsulate(void* ptr, char* name = NULL) {
    return PyCapsule_New(ptr, name, /* destructor= */ NULL);
}

/*
 * Build an SgAddOp node from the given Python statements.
 *  - PyObject* args = (PyObject*, PyObject*)
 */
PyObject*
sage_buildAddOp(PyObject *self, PyObject *args)
{
    cout << "Hello from buildAddOp()" << endl;
    return Py_BuildValue("i", 0);
}

/*
 * Build an SgGlobal node from the given list of Python statements.
 *  - PyObject* args = ( [PyObject*, PyObject*, ...], )
 */
PyObject*
sage_buildGlobal(PyObject *self, PyObject *args)
{
    cout << "Hello from buildGlobal()" << endl;
    return Py_BuildValue("i", 0);
}

/*
 * Build an SgPrintStmt node from the given list of Python
 * expressions.
 *  - PyObject* args = ( [PyObject*, PyObject*, ... ], )
 */
PyObject*
sage_buildPrintStmt(PyObject *self, PyObject *args)
{
    cout << "Hello from buildPrintStmt()" << endl;
    PyObject* arg1v = PyTuple_GetItem(args, 0);
    Py_ssize_t arg1c = PyList_Size(arg1v);
#if 0
    SgPrintStmt* sg_print_statement = buildSgPrintStmt_nfi();
#endif
    for (int i = 0; i < arg1c; i++) {
       PyObject* capsule = PyList_GetItem(arg1v, i);
#if 0
       SgExpression* exp = 
           (SgExpression*) PyCapsule_GetPointer(capsule, NULL);
       sg_print_statement->append_expression(exp);
#endif
    }
#if 0
    return PyEncapsulate(sg_print_statement);
#else
    return Py_BuildValue("i", 0);
#endif
}

/* 
 * Build an SgStringVal node from the given Python String object.
 *  - PyObject* args = ( PyStringObject*, ) 
 */
PyObject*
sage_buildStringVal(PyObject *self, PyObject *args)
{
    cout << "Hello from buildStringVal()" << endl;
    char* cstr = NULL;
    PyArg_ParseTuple(args, "s", &cstr); // TODO error handling
    string str = string(cstr);
    SgStringVal* sg_string_val = buildStringVal_nfi(str);
    return PyEncapsulate(sg_string_val);
}
