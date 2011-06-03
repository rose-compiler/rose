#ifndef SAGE_PYTHON_INTERFACE_H_
#define SAGE_PYTHON_INTERFACE_H_

#include "Python.h"

/* Return the number of arguments of the application command line */
PyObject* sage_buildAddOp(PyObject *self, PyObject *args);
PyObject* sage_buildGlobal(PyObject *self, PyObject *args);
PyObject* sage_buildLongIntVal(PyObject *self, PyObject *args);
PyObject* sage_buildPrintStmt(PyObject *self, PyObject *args);
PyObject* sage_buildStringVal(PyObject *self, PyObject *args);

static PyMethodDef SageBuilderMethods[] = {
    {"buildAddOp", sage_buildAddOp, METH_VARARGS, "Builds an SgAddOp node."},
    {"buildGlobal", sage_buildGlobal, METH_VARARGS, "Builds an SgGlobal node."},
    {"buildLongIntVal", sage_buildLongIntVal, METH_VARARGS, "Builds an SgIntVal node."},
    {"buildPrintStmt", sage_buildPrintStmt, METH_VARARGS, "Builds an SgPrintStmt node."},
    {"buildStringVal", sage_buildStringVal, METH_VARARGS, "Builds an SgStringVal node."},
    {NULL, NULL, 0, NULL}
};

#endif /* SAGE_PYTHON_INTERFACE_H_ */
