#ifndef SAGE_PYTHON_INTERFACE_H_
#define SAGE_PYTHON_INTERFACE_H_

#include <Python.h>

#include "SagePythonSupport.h"

PyObject* sage_buildAssert           (PyObject *self, PyObject *args);
PyObject* sage_buildAssign           (PyObject *self, PyObject *args);
PyObject* sage_buildAttr             (PyObject *self, PyObject *args);
PyObject* sage_buildAugAssign        (PyObject *self, PyObject *args);
PyObject* sage_buildBinOp            (PyObject *self, PyObject *args);
PyObject* sage_buildBoolOp           (PyObject *self, PyObject *args);
PyObject* sage_buildBreak            (PyObject *self, PyObject *args);
PyObject* sage_buildCall             (PyObject *self, PyObject *args);
PyObject* sage_buildClassDef         (PyObject *self, PyObject *args);
PyObject* sage_buildCompare          (PyObject *self, PyObject *args);
PyObject* sage_buildComprehension    (PyObject *self, PyObject *args);
PyObject* sage_buildComplexVal       (PyObject *self, PyObject *args);
PyObject* sage_buildContinue         (PyObject *self, PyObject *args);
PyObject* sage_buildDelete           (PyObject *self, PyObject *args);
PyObject* sage_buildDict             (PyObject *self, PyObject *args);
PyObject* sage_buildDictComp         (PyObject *self, PyObject *args);
PyObject* sage_buildExceptHandler    (PyObject *self, PyObject *args);
PyObject* sage_buildExec             (PyObject *self, PyObject *args);
PyObject* sage_buildExpr             (PyObject *self, PyObject *args);
PyObject* sage_buildExprListExp      (PyObject *self, PyObject *args);
PyObject* sage_buildFloat            (PyObject *self, PyObject *args);
PyObject* sage_buildFor              (PyObject *self, PyObject *args);
PyObject* sage_buildFunctionDef      (PyObject *self, PyObject *args);
PyObject* sage_buildGlobal           (PyObject *self, PyObject *args);
PyObject* sage_buildGlobalScope      (PyObject *self, PyObject *args);
PyObject* sage_buildIf               (PyObject *self, PyObject *args);
PyObject* sage_buildIfExp            (PyObject *self, PyObject *args);
PyObject* sage_buildImport           (PyObject *self, PyObject *args);
PyObject* sage_buildInitializedName  (PyObject *self, PyObject *args, PyObject* kwargs);
PyObject* sage_buildKeyword          (PyObject *self, PyObject *args);
PyObject* sage_buildKeyDatumPair     (PyObject *self, PyObject *args);
PyObject* sage_buildLongIntVal       (PyObject *self, PyObject *args);
PyObject* sage_buildLambda           (PyObject *self, PyObject *args);
PyObject* sage_buildListExp          (PyObject *self, PyObject *args);
PyObject* sage_buildListComp         (PyObject *self, PyObject *args);
PyObject* sage_buildName             (PyObject *self, PyObject *args);
PyObject* sage_buildPass             (PyObject *self, PyObject *args);
PyObject* sage_buildPrintStmt        (PyObject *self, PyObject *args);
PyObject* sage_buildRepr             (PyObject *self, PyObject *args);
PyObject* sage_buildReturnStmt       (PyObject *self, PyObject *args);
PyObject* sage_buildSetComp          (PyObject *self, PyObject *args);
PyObject* sage_buildSlice            (PyObject *self, PyObject *args);
PyObject* sage_buildStringVal        (PyObject *self, PyObject *args);
PyObject* sage_buildSubscript        (PyObject *self, PyObject *args);
PyObject* sage_buildSuite            (PyObject *self, PyObject *args);
PyObject* sage_buildTryExcept        (PyObject *self, PyObject *args);
PyObject* sage_buildTryFinally       (PyObject *self, PyObject *args);
PyObject* sage_buildTuple            (PyObject *self, PyObject *args);
PyObject* sage_buildUnaryOp          (PyObject *self, PyObject *args);
PyObject* sage_buildWhile            (PyObject *self, PyObject *args);
PyObject* sage_buildWith             (PyObject *self, PyObject *args);
PyObject* sage_buildYield            (PyObject *self, PyObject *args);

void initializePythonTypes();

#endif /* SAGE_PYTHON_INTERFACE_H_ */
