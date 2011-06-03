#ifndef SAGE_PYTHON_INTERFACE_H_
#define SAGE_PYTHON_INTERFACE_H_

#include "Python.h"

/* Return the number of arguments of the application command line */
PyObject* sage_buildGlobal(PyObject *self, PyObject *args);
PyObject* sage_buildPrintStmt(PyObject *self, PyObject *args);
PyObject* sage_buildStringVal(PyObject *self, PyObject *args);

static PyMethodDef SageBuilderMethods[] = {
    {"buildGlobal", sage_buildGlobal, METH_KEYWORDS, "Builds an SgGlobal node."},
    {"buildPrintStmt", sage_buildPrintStmt, METH_KEYWORDS, "Builds an SgPrintStmt node."},
    {"buildStringVal", sage_buildStringVal, METH_KEYWORDS, "Builds an SgStringVal node."},
    {NULL, NULL, 0, NULL}
};

#endif /* SAGE_PYTHON_INTERFACE_H_ */
