#ifndef SAGE_PYTHON_INTERFACE_H_
#define SAGE_PYTHON_INTERFACE_H_

#include <Python.h>

#include "SagePythonSupport.h"

PyObject* sage_buildAddOp(PyObject *self, PyObject *args);
PyObject* sage_buildBinOp(PyObject *self, PyObject *args);
PyObject* sage_buildCall(PyObject *self, PyObject *args);
PyObject* sage_buildExceptHandler(PyObject *self, PyObject *args);
PyObject* sage_buildExpr(PyObject *self, PyObject *args);
PyObject* sage_buildFunctionDef(PyObject *self, PyObject *args);
PyObject* sage_buildGlobal(PyObject *self, PyObject *args);
PyObject* sage_buildIf(PyObject *self, PyObject *args);
PyObject* sage_buildKeyword(PyObject *self, PyObject *args);
PyObject* sage_buildLongIntVal(PyObject *self, PyObject *args);
PyObject* sage_buildName(PyObject *self, PyObject *args);
PyObject* sage_buildPower(PyObject *self, PyObject *args);
PyObject* sage_buildPrintStmt(PyObject *self, PyObject *args);
PyObject* sage_buildReturnStmt(PyObject *self, PyObject *args);
PyObject* sage_buildStringVal(PyObject *self, PyObject *args);
PyObject* sage_buildTryExcept(PyObject *self, PyObject *args);
PyObject* sage_buildTryFinally(PyObject *self, PyObject *args);

static PyMethodDef SageBuilderMethods[] = {
    {"buildAddOp", sage_buildAddOp, METH_VARARGS, "Builds an SgAddOp node."},
    {"buildBinOp", sage_buildBinOp, METH_VARARGS, "Builds a binary expression node."},
    {"buildCall", sage_buildCall, METH_VARARGS, "Builds an SgFunctionCallExp node."},
    {"buildExceptHandler", sage_buildExceptHandler, METH_VARARGS, "Builds an exception handler node."},
    {"buildExpr", sage_buildExpr, METH_VARARGS, "Builds an Expr node."},
    {"buildFunctionDef", sage_buildFunctionDef, METH_VARARGS, "Builds an SgFunctionDeclaration node."},
    {"buildGlobal", sage_buildGlobal, METH_VARARGS, "Builds an SgGlobal node."},
    {"buildIf", sage_buildIf, METH_VARARGS, "Builds an if stmt  node."},
    {"buildKeyword", sage_buildKeyword, METH_VARARGS, "Builds a keyword node."},
    {"buildLongIntVal", sage_buildLongIntVal, METH_VARARGS, "Builds an SgIntVal node."},
    {"buildPrintStmt", sage_buildPrintStmt, METH_VARARGS, "Builds an SgPrintStmt node."},
    {"buildPower", sage_buildPower, METH_VARARGS, "Builds a power (exponentiation) node."},
    {"buildName", sage_buildName, METH_VARARGS, "Builds an SgVarRefExp from a Name node."},
    {"buildReturnStmt", sage_buildReturnStmt, METH_VARARGS, "Builds an SgReturnStmt node."},
    {"buildStringVal", sage_buildStringVal, METH_VARARGS, "Builds an SgStringVal node."},
    {"buildTryExcept", sage_buildTryExcept, METH_VARARGS, "Builds an try stmt node."},
    {"buildTryFinally", sage_buildTryFinally, METH_VARARGS, "Builds an try/finally stmt node."},

    {"appendStatements", (PyCFunction)sage_appendStatements, METH_VARARGS | METH_KEYWORDS, "Add children to a given SgNode."},

    {NULL, NULL, 0, NULL}
};


#endif /* SAGE_PYTHON_INTERFACE_H_ */
