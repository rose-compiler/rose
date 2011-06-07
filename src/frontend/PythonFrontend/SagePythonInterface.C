#include <iostream>

#include "sage3basic.h"
#include "sageBuilder.h"
#include "SagePythonInterface.h"

/**
 * Implementation of the Sage-Python interface.
 */


using namespace std;


/*
 * Build an SgAddOp node from the given Python statements.
 *  - PyObject* args = (PyObject*, PyObject*)
 */
PyObject*
sage_buildAddOp(PyObject *self, PyObject *args)
{
    PyObject* lhs_capsule = PyTuple_GetItem(args, 0);
    PyObject* rhs_capsule = PyTuple_GetItem(args, 1);
    SgExpression* lhs = PyDecapsulate<SgExpression>(lhs_capsule);
    SgExpression* rhs = PyDecapsulate<SgExpression>(rhs_capsule);
    SgAddOp* sg_add_op = SageBuilder::buildAddOp(lhs, rhs);
    return PyEncapsulate(sg_add_op);
}

/*
 * Build an Expr node from the given Python statements.
 *  - PyObject* args = (PyObject*)
 */
PyObject*
sage_buildExpr(PyObject *self, PyObject *args)
{
    PyObject* py_value = PyTuple_GetItem(args, 0);
    SgNode* sg_value = PyDecapsulate<SgNode>(py_value);
    return PyEncapsulate(sg_value);
}

/*
 * Build a FunctionDef
 */
PyObject*
sage_buildFunctionDef(PyObject *self, PyObject *args)
{
    PyObject* func_def_capsule = PyTuple_GetItem(args, 0);
    PyObject* py_defaults_capsules = PyTuple_GetItem(args, 1);
    PyObject* file_info_capsule = PyTuple_GetItem(args, 2);
    PyObject* scope_capsule = PyTuple_GetItem(args, 3);


    PyObject* py_name = PyObject_GetAttrString(func_def_capsule, "name");
    string func_name = string( PyString_AsString(py_name) );

    PyObject* py_args = PyObject_GetAttrString(func_def_capsule, "args");
    SgFunctionParameterList* sg_params =
        buildFunctionParameterList(py_args, py_defaults_capsules);

    SgScopeStatement* sg_scope_statement =
        PyDecapsulate<SgScopeStatement>(scope_capsule);

    // TODO: Figure out types, parse parameter list
    SgFunctionDeclaration* sg_func_decl =
        SageBuilder::buildDefiningFunctionDeclaration(func_name,
                SageBuilder::buildVoidType(),
                sg_params,
                sg_scope_statement);

    PyObject* return_tuple = PyTuple_New(2);
    PyTuple_SetItem(return_tuple, 0, PyEncapsulate(sg_func_decl));
    PyTuple_SetItem(return_tuple, 1, PyEncapsulate(sg_func_decl->get_definition()));
    return return_tuple;
}

/*
 * Build an SgGlobal node from the given list of Python statements.
 *  - PyObject* args = ( [PyObject*, PyObject*, ...], )
 */
PyObject*
sage_buildGlobal(PyObject *self, PyObject *args)
{
    PyObject* arg0 = PyTuple_GetItem(args, 0);
    std::string filename = std::string( PyString_AsString(arg0) );

    Sg_File_Info* sg_file_info = new Sg_File_Info(filename, 0, 0);
    sg_file_info->unsetTransformation();
    sg_file_info->setOutputInCodeGeneration();

    SgGlobal* sg_global = new SgGlobal(sg_file_info);
    sg_global->set_startOfConstruct(sg_file_info);
    sg_global->set_endOfConstruct(new Sg_File_Info(filename, 0, 0));

    return PyEncapsulate(sg_global);
}

/*
 * Build an SgIfStmt node from the given list of Python statements.
 *  - PyObject* args = ( PyObject* test,
 *                       [ PyObject* body0, PyObject* body1, ...],
 *                       [ PyObject* orelse0, PyObject* orelse1, ...]
 *                     )
 */
PyObject*
sage_buildIf(PyObject *self, PyObject *args)
{
    PyObject* py_test =        PyTuple_GetItem(args, 0);
    PyObject* py_body_list =   PyTuple_GetItem(args, 1);
    PyObject* py_orelse_list = PyTuple_GetItem(args, 2);

    SgBasicBlock* true_body = SageBuilder::buildBasicBlock();
    Py_ssize_t tbodyc = PyList_Size(py_body_list);
    for (int i = 0; i < tbodyc; i++) {
        PyObject* py_stmt = PyList_GetItem(py_body_list, i);
        SgStatement* sg_stmt = PyDecapsulate<SgStatement>(py_stmt);
        true_body->append_statement(sg_stmt);
    }

    Py_ssize_t fbodyc = PyList_Size(py_orelse_list);
    SgBasicBlock* false_body =
        (fbodyc > 0) ? SageBuilder::buildBasicBlock() : NULL;
    for (int i = 0; i < fbodyc; i++) {
        PyObject* py_stmt = PyList_GetItem(py_orelse_list, i);
        SgStatement* sg_stmt = PyDecapsulate<SgStatement>(py_stmt);
        false_body->append_statement(sg_stmt);
    }

    SgExpression* test = PyDecapsulate<SgExpression>(py_test);
    SgIfStmt* sg_if_stmt =
        SageBuilder::buildIfStmt(test, true_body, false_body);
    return PyEncapsulate(sg_if_stmt);
}

/*
 * Build an SgLongIntVal node from the given Python integer.
 *  - PyObject* args = (PyObject*,)
 */
PyObject*
sage_buildLongIntVal(PyObject *self, PyObject *args)
{
    PyObject* py_value = PyTuple_GetItem(args, 0);
    PyObject* file_info_capsule = PyTuple_GetItem(args, 1);

    long value = PyInt_AsLong(py_value);
    SgLongIntVal* sg_long_int_val =
        SageBuilder::buildLongIntVal(value);

    set_File_Info(sg_long_int_val, file_info_capsule);
    return PyEncapsulate(sg_long_int_val);
}

/*
 * Build an SgVarRefExp node from the given Python Name node.
 *  - PyObject* args = ( PyObject* id, PyObject* scope )
 */
PyObject*
sage_buildName(PyObject *self, PyObject *args)
{
    PyObject* py_id = PyTuple_GetItem(args, 0);
    char* id = PyString_AsString(py_id);

    PyObject* py_scope_capsule = PyTuple_GetItem(args, 1);
    SgScopeStatement* scope =
        PyDecapsulate<SgScopeStatement>(py_scope_capsule);
    ROSE_ASSERT(scope != NULL);

    SgVarRefExp* sg_var_ref = SageBuilder::buildVarRefExp(id, scope);
    return PyEncapsulate(sg_var_ref);
}

/*
 * Build an SgPrintStmt node from the given list of Python
 * expressions.
 *  - PyObject* args = ( [PyObject*, PyObject*, ... ], )
 */
PyObject*
sage_buildPrintStmt(PyObject *self, PyObject *args)
{
    PyObject* arg1v = PyTuple_GetItem(args, 0);
    Py_ssize_t arg1c = PyList_Size(arg1v);
    for (int i = 0; i < arg1c; i++) {
       PyObject* capsule = PyList_GetItem(arg1v, i);
    }
    return Py_BuildValue("i", 0);
}

/*
 * Build an SgReturnStmt node from the given Python object.
 *  - PyObject* args = ( PyObject*, )
 */
PyObject*
sage_buildReturnStmt(PyObject *self, PyObject *args)
{
    PyObject* py_exp_capsule = PyTuple_GetItem(args, 0);
    SgExpression* sg_exp = PyDecapsulate<SgExpression>(py_exp_capsule);
    SgReturnStmt* sg_return = SageBuilder::buildReturnStmt(sg_exp);
    return PyEncapsulate(sg_return);
}

/*
 * Build an SgStringVal node from the given Python String object.
 *  - PyObject* args = ( PyStringObject*, )
 */
PyObject*
sage_buildStringVal(PyObject *self, PyObject *args)
{
    PyObject* py_str = PyTuple_GetItem(args, 0);
    char* c_str = PyString_AsString(py_str);
    std::string str = std::string(c_str);
    SgStringVal* sg_string_val = SageBuilder::buildStringVal(str);
    set_File_Info(sg_string_val, PyTuple_GetItem(args, 1));
    return PyEncapsulate(sg_string_val);
}
