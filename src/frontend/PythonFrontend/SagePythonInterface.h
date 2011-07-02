#ifndef SAGE_PYTHON_INTERFACE_H_
#define SAGE_PYTHON_INTERFACE_H_

#include <Python.h>

#include "SagePythonSupport.h"

PyObject* sage_buildAugAssign(PyObject *self, PyObject *args);
PyObject* sage_buildBinOp(PyObject *self, PyObject *args);
PyObject* sage_buildCall(PyObject *self, PyObject *args);
PyObject* sage_buildCompare(PyObject *self, PyObject *args);
PyObject* sage_buildExceptHandler(PyObject *self, PyObject *args);
PyObject* sage_buildExpr(PyObject *self, PyObject *args);
PyObject* sage_buildExprListExp(PyObject *self, PyObject *args);
PyObject* sage_buildFunctionDef(PyObject *self, PyObject *args);
PyObject* sage_buildGlobal(PyObject *self, PyObject *args);
PyObject* sage_buildIf(PyObject *self, PyObject *args);
PyObject* sage_buildInitializedName(PyObject *self, PyObject *args);
PyObject* sage_buildKeyword(PyObject *self, PyObject *args);
PyObject* sage_buildLongIntVal(PyObject *self, PyObject *args);
PyObject* sage_buildLambda(PyObject *self, PyObject *args);
PyObject* sage_buildName(PyObject *self, PyObject *args);
PyObject* sage_buildPower(PyObject *self, PyObject *args);
PyObject* sage_buildPrintStmt(PyObject *self, PyObject *args);
PyObject* sage_buildReturnStmt(PyObject *self, PyObject *args);
PyObject* sage_buildStringVal(PyObject *self, PyObject *args);
PyObject* sage_buildSuite(PyObject *self, PyObject *args);
PyObject* sage_buildTryExcept(PyObject *self, PyObject *args);
PyObject* sage_buildTryFinally(PyObject *self, PyObject *args);
PyObject* sage_buildUnaryOp(PyObject *self, PyObject *args);
PyObject* sage_buildWhile(PyObject *self, PyObject *args);
PyObject* sage_buildWith(PyObject *self, PyObject *args);

static PyMethodDef SageBuilderMethods[] = {
    {"buildAugAssign", sage_buildAugAssign, METH_VARARGS, "Builds an augmented assignment node."},
    {"buildBinOp", sage_buildBinOp, METH_VARARGS, "Builds a binary expression node."},
    {"buildCall", sage_buildCall, METH_VARARGS, "Builds an SgFunctionCallExp node."},
    {"buildCompare", sage_buildCompare, METH_VARARGS, "Builds a comparison node."},
    {"buildExceptHandler", sage_buildExceptHandler, METH_VARARGS, "Builds an exception handler node."},
    {"buildExpr", sage_buildExpr, METH_VARARGS, "Builds an Expr node."},
    {"buildExprListExp", sage_buildExprListExp, METH_VARARGS, "Builds an expression list."},
    {"buildFunctionDef", sage_buildFunctionDef, METH_VARARGS, "Builds an SgFunctionDeclaration node."},
    {"buildFunctionParameterList", sage_buildFunctionParameterList, METH_VARARGS, "Builds an SgFunctionParameterList node."},
    {"buildGlobal", sage_buildGlobal, METH_VARARGS, "Builds an SgGlobal node."},
    {"buildIf", sage_buildIf, METH_VARARGS, "Builds an if stmt  node."},
    {"buildInitializedName", sage_buildInitializedName, METH_VARARGS, "Builds an initialized name node."},
    {"buildKeyword", sage_buildKeyword, METH_VARARGS, "Builds a keyword node."},
    {"buildLongIntVal", sage_buildLongIntVal, METH_VARARGS, "Builds an SgIntVal node."},
    {"buildLambda", sage_buildLambda, METH_VARARGS, "Builds an lambda node."},
    {"buildPrintStmt", sage_buildPrintStmt, METH_VARARGS, "Builds an SgPrintStmt node."},
    {"buildPower", sage_buildPower, METH_VARARGS, "Builds a power (exponentiation) node."},
    {"buildName", sage_buildName, METH_VARARGS, "Builds an SgVarRefExp from a Name node."},
    {"buildReturnStmt", sage_buildReturnStmt, METH_VARARGS, "Builds an SgReturnStmt node."},
    {"buildStringVal", sage_buildStringVal, METH_VARARGS, "Builds an SgStringVal node."},
    {"buildSuite", sage_buildSuite, METH_VARARGS, "Builds a suite in a basic block node."},
    {"buildTryExcept", sage_buildTryExcept, METH_VARARGS, "Builds an try stmt node."},
    {"buildTryFinally", sage_buildTryFinally, METH_VARARGS, "Builds an try/finally stmt node."},
    {"buildUnaryOp", sage_buildUnaryOp, METH_VARARGS, "Builds unary operation node."},
    {"buildWhile", sage_buildWhile, METH_VARARGS, "Builds while stmt node."},
    {"buildWith", sage_buildWith, METH_VARARGS, "Builds with stmt node."},

    {"appendStatements", (PyCFunction)sage_appendStatements, METH_VARARGS | METH_KEYWORDS, "Add children to a given SgNode."},

    {NULL, NULL, 0, NULL}
};

#endif /* SAGE_PYTHON_INTERFACE_H_ */
