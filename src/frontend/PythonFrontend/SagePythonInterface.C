#include <iostream>

#include "sage3basic.h"
#include "sageBuilder.h"
#include "SagePythonInterface.h"

#include "unparsePython.h" // for operator ROSE_PYTHON_<op>_OP strings. TODO put somewhere better.

/**
 * Implementation of the Sage-Python interface.
 */


using namespace std;


/*
 * Build an SgOp node from the given Python statements.
 *  - PyObject* args = (PyObject*, PyObject*)
 */
PyObject*
sage_buildAugAssign(PyObject *self, PyObject *args)
{
    PyObject* lhs_capsule = PyTuple_GetItem(args, 0);
    PyObject* rhs_capsule = PyTuple_GetItem(args, 1);
    PyObject* py_operation = PyTuple_GetItem(args, 2);

    SgExpression* lhs = PyDecapsulate<SgExpression>(lhs_capsule);
    SgExpression* rhs = PyDecapsulate<SgExpression>(rhs_capsule);
    std::string op = std::string( PyString_AsString(py_operation) );
    cerr << "lhs: " << lhs->class_name() << endl;
    cerr << "rhs: " << rhs->class_name() << endl;
    cerr << "op: " << op << endl;
    SgBinaryOp* sg_bin_op = NULL;
    if      (op == ROSE_PYTHON_AUG_ADD_OP)
        sg_bin_op = SageBuilder::buildPlusAssignOp(lhs, rhs);
    else if (op == ROSE_PYTHON_AUG_SUB_OP)
        sg_bin_op = SageBuilder::buildMinusAssignOp(lhs, rhs);
    else if (op == ROSE_PYTHON_AUG_MULT_OP)
        sg_bin_op = SageBuilder::buildMultAssignOp(lhs, rhs);
    else if (op == ROSE_PYTHON_AUG_DIV_OP)
        sg_bin_op = SageBuilder::buildDivAssignOp(lhs, rhs);
    else if (op == ROSE_PYTHON_AUG_IDIV_OP) {
        //sg_bin_op = SageBuilder::buildIntegerDivideAssignOp(lhs, rhs);
        cerr << "Error: no SgIntegerDivAssignOp node." << endl;
        ROSE_ABORT();
    }
    else if (op == ROSE_PYTHON_AUG_MOD_OP)
        sg_bin_op = SageBuilder::buildModAssignOp(lhs, rhs);
    else if (op == ROSE_PYTHON_AUG_LSHIFT_OP)
        sg_bin_op = SageBuilder::buildLshiftAssignOp(lhs, rhs);
    else if (op == ROSE_PYTHON_AUG_RSHIFT_OP)
        sg_bin_op = SageBuilder::buildRshiftAssignOp(lhs, rhs);
    else if (op == ROSE_PYTHON_AUG_BITAND_OP) {
        //sg_bin_op = SageBuilder::buildBitAndAssignOp(lhs, rhs);
        cerr << "Error: no SgBitAndAssignOp node." << endl;
        ROSE_ABORT();
    }
    else if (op == ROSE_PYTHON_AUG_BITOR_OP) {
        sg_bin_op = SageBuilder::buildBitOrOp(lhs, rhs);
        //sg_bin_op = SageBuilder::buildBitAndAssignOp(lhs, rhs);
        cerr << "Error: no SgBitOrAssignOp node." << endl;
        ROSE_ABORT();
    }
    else if (op == ROSE_PYTHON_AUG_BITXOR_OP) {
        //sg_bin_op = SageBuilder::buildBitXorAssignOp(lhs, rhs);
        cerr << "Error: no SgBitXorAssignOp node." << endl;
        ROSE_ABORT();
    }
    else if (op == ROSE_PYTHON_AUG_EXP_OP)
        sg_bin_op = SageBuilder::buildExponentiationOp(lhs, rhs);
    else {
        cerr << "Unrecognized aug assign operator: " << op << endl;
        ROSE_ABORT();
    }
    return PyEncapsulate(sg_bin_op);
}

/*
 * Build an SgAddOp node from the given Python statements.
 *  - PyObject* args = (PyObject*, PyObject*)
 */
PyObject*
sage_buildBinOp(PyObject *self, PyObject *args)
{
    PyObject* lhs_capsule = PyTuple_GetItem(args, 0);
    PyObject* rhs_capsule = PyTuple_GetItem(args, 1);
    PyObject* py_operation = PyTuple_GetItem(args, 2);

    SgExpression* lhs = PyDecapsulate<SgExpression>(lhs_capsule);
    SgExpression* rhs = PyDecapsulate<SgExpression>(rhs_capsule);
    std::string op = std::string( PyString_AsString(py_operation) );
    SgBinaryOp* sg_bin_op = NULL;
    if      (op == ROSE_PYTHON_ADD_OP)
        sg_bin_op = SageBuilder::buildAddOp(lhs, rhs);
    else if (op == ROSE_PYTHON_SUB_OP)
        sg_bin_op = SageBuilder::buildSubtractOp(lhs, rhs);
    else if (op == ROSE_PYTHON_MULT_OP)
        sg_bin_op = SageBuilder::buildMultiplyOp(lhs, rhs);
    else if (op == ROSE_PYTHON_DIV_OP)
        sg_bin_op = SageBuilder::buildDivideOp(lhs, rhs);
    else if (op == ROSE_PYTHON_IDIV_OP)
        sg_bin_op = SageBuilder::buildIntegerDivideOp(lhs, rhs);
    else if (op == ROSE_PYTHON_MOD_OP)
        sg_bin_op = SageBuilder::buildModOp(lhs, rhs);
    else if (op == ROSE_PYTHON_LSHIFT_OP)
        sg_bin_op = SageBuilder::buildLshiftOp(lhs, rhs);
    else if (op == ROSE_PYTHON_RSHIFT_OP)
        sg_bin_op = SageBuilder::buildRshiftOp(lhs, rhs);
    else if (op == ROSE_PYTHON_BITAND_OP)
        sg_bin_op = SageBuilder::buildBitAndOp(lhs, rhs);
    else if (op == ROSE_PYTHON_BITOR_OP)
        sg_bin_op = SageBuilder::buildBitOrOp(lhs, rhs);
    else if (op == ROSE_PYTHON_BITXOR_OP)
        sg_bin_op = SageBuilder::buildBitXorOp(lhs, rhs);
    else if (op == ROSE_PYTHON_EXP_OP)
        sg_bin_op = SageBuilder::buildExponentiationOp(lhs, rhs);
    else {
        cerr << "Unrecognized binary operator: " << op << endl;
        ROSE_ABORT();
    }

    return PyEncapsulate(sg_bin_op);
}

/*
 * Build an SgAddOp node from the given Python statements.
 *  - PyObject* args = (PyObject*, PyObject*)
 */
PyObject*
sage_buildCall(PyObject *self, PyObject *args)
{
    PyObject* py_name    = PyTuple_GetItem(args, 0);
    PyObject* py_args    = PyTuple_GetItem(args, 1);
    PyObject* py_kwargs  = PyTuple_GetItem(args, 2);
    PyObject* py_scope   = PyTuple_GetItem(args, 3);

    std::vector<SgExpression*> sg_exprs;
    Py_ssize_t argc = PyList_Size(py_args);
    for (int i = 0; i < argc; i++) {
        PyObject* py_arg = PyList_GetItem(py_args, i);
        SgExpression* sg_exp = PyDecapsulate<SgExpression>(py_arg);
        sg_exprs.push_back(sg_exp);
    }
    Py_ssize_t kwargc = PyList_Size(py_kwargs);
    for (int i = 0; i < kwargc; i++) {
        PyObject* py_kwarg = PyList_GetItem(py_kwargs, i);
        SgExpression* sg_exp = PyDecapsulate<SgExpression>(py_kwarg);
        if (sg_exp == NULL) {
            SgNode* sg_node = PyDecapsulate<SgNode>(py_kwarg);
            if (sg_node == NULL) {
                cout << "null sg node" << endl;
            } else {
                cout << "bad node: " << sg_node->class_name() << endl;
            }
        }
        sg_exprs.push_back(sg_exp);
    }
    SgExprListExp* sg_expr_list_exp =
        SageBuilder::buildExprListExp(sg_exprs);

    SgScopeStatement* sg_scope = PyDecapsulate<SgScopeStatement>(py_scope);
    ROSE_ASSERT(sg_scope != NULL);
    SgName sg_func_name = SgName( PyString_AsString(py_name) );
    SgFunctionSymbol* sg_func_symbol = isSgFunctionSymbol(
        SageInterface::lookupSymbolInParentScopes(sg_func_name, sg_scope));
    if (sg_func_symbol == NULL) {
        cerr << "Cannot find symbol: " << sg_func_name.str() << endl;
    }

    SgFunctionCallExp* sg_function_call_exp =
        SageBuilder::buildFunctionCallExp(sg_func_symbol, sg_expr_list_exp);
    return PyEncapsulate(sg_function_call_exp);
}

/*
 * Build an SgOp node from the given Python statements.
 *  - PyObject* args = (PyObject*, PyObject*)
 */
PyObject*
sage_buildCompare(PyObject *self, PyObject *args)
{
    PyObject* py_left        = PyTuple_GetItem(args, 0);
    PyObject* py_ops         = PyTuple_GetItem(args, 1);
    PyObject* py_comparators = PyTuple_GetItem(args, 2);

    cerr << "Error: Comparisons require new sage node. Skipping." << endl;
    SgExpression* left = SageBuilder::buildStringVal("COMPARISON");
    return PyEncapsulate(left);
}

/*
 * Build an Expr node from the given Python statements.
 *  - PyObject* args = (PyObject*)
 */
PyObject*
sage_buildExceptHandler(PyObject *self, PyObject *args)
{
    PyObject* py_name = PyTuple_GetItem(args, 0);
    PyObject* py_type = PyTuple_GetItem(args, 1);
    PyObject* py_body = PyTuple_GetItem(args, 2);

    // TODO: add buildCatchOptionStmt to SageBuilder
    cerr << "sage_buildExceptHandler is unimplemented" << endl;
    //SgNode* sg_value = SageBuilder::buildBasicBlock();
    return PyEncapsulate(NULL);
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
    ROSE_ASSERT(sg_value != NULL);
    return PyEncapsulate(sg_value);
}

/*
 * Build a FunctionDef
 */
PyObject*
sage_buildFunctionDef(PyObject *self, PyObject *args)
{
    PyObject* func_def_capsule     = PyTuple_GetItem(args, 0);
    PyObject* py_defaults_capsules = PyTuple_GetItem(args, 1);
    PyObject* py_decorators        = PyTuple_GetItem(args, 2);
    PyObject* file_info_capsule    = PyTuple_GetItem(args, 3);
    PyObject* scope_capsule        = PyTuple_GetItem(args, 4);


    PyObject* py_name = PyObject_GetAttrString(func_def_capsule, "name");
    string func_name = string( PyString_AsString(py_name) );

    PyObject* py_args = PyObject_GetAttrString(func_def_capsule, "args");
              py_args = PyObject_GetAttrString(py_args, "args");
    SgFunctionParameterList* sg_params =
        buildFunctionParameterList(py_args, py_defaults_capsules);

    SgScopeStatement* sg_scope_statement =
        PyDecapsulate<SgScopeStatement>(scope_capsule);

    SgFunctionDeclaration* sg_func_decl =
        SageBuilder::buildDefiningFunctionDeclaration(func_name,
                SageBuilder::buildVoidType(),
                sg_params,
                sg_scope_statement);

#if 0 // awaiting resolution of abstract handle bug
    SgExprListExp* decorators = sg_func_decl->get_decoratorList();
    Py_ssize_t decc = PyList_Size(py_decorators);
    for(int i = 0; i < decc; i++) {
        PyObject* py_exp = PyList_GetItem(py_decorators, i);
        SgExpression* exp = PyDecapsulate<SgExpression>(py_exp);
        decorators->append_expression(exp);
    }
#endif

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

    SgFunctionDeclaration* sg_main_func_decl =
        SageBuilder::buildDefiningFunctionDeclaration( SgName(ROSE_PYTHON_WRAPPER_FXN_NAME),
                SageBuilder::buildVoidType(),
                SageBuilder::buildFunctionParameterList(),
                sg_global);

    SageInterface::appendStatement(sg_main_func_decl, sg_global);

    PyObject* return_tuple = PyTuple_New(2);
    PyTuple_SetItem(return_tuple, 0, PyEncapsulate(sg_global));
    PyTuple_SetItem(return_tuple, 1, PyEncapsulate(sg_main_func_decl));
    return return_tuple;
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
 * Builds a keyword node.
 */
PyObject*
sage_buildKeyword(PyObject *self, PyObject *args)
{
    PyObject* py_key   = PyTuple_GetItem(args, 0);
    PyObject* py_value = PyTuple_GetItem(args, 1);

    SgExpression* sg_key   = PyDecapsulate<SgExpression>(py_key);
    SgExpression* sg_value = PyDecapsulate<SgExpression>(py_value);

    SgAssignOp* sg_assign_op =
        SageBuilder::buildAssignOp(sg_key, sg_value);

    return PyEncapsulate(sg_assign_op);
}

/*
 */
PyObject*
sage_buildLambda(PyObject *self, PyObject *args)
{
    PyObject* py_args = PyTuple_GetItem(args, 0);
    PyObject* py_body = PyTuple_GetItem(args, 1);

    PyObject* py_arg_list = PyObject_GetAttrString(py_args, "args");
    SgFunctionParameterList* sg_params =
        buildFunctionParameterList(py_arg_list, NULL);

    SgStatement* sg_body = PyDecapsulate<SgStatement>(py_body);

    SgLambdaExp* sg_lambda_exp =
        SageBuilder::buildLambdaExp(sg_params, sg_body, NULL);

    return PyEncapsulate(sg_lambda_exp);
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

    SgName sg_name(id);
    SgVariableSymbol* sg_sym = scope->lookup_variable_symbol( sg_name );
    if (sg_sym == NULL) {
#if 0 //TODO figure out vardecls/initnames/symbols/varrefexps
        cerr << "Decl: " << id << endl;
        SgType* sg_type = SageBuilder::buildVoidType();
        SgVariableDeclaration* sg_var_decl =
            SageBuilder::buildVariableDeclaration(sg_name, sg_type, NULL, scope);
#endif
        SgStringVal* sg_str = SageBuilder::buildStringVal(id); //TODO this is wrong
        return PyEncapsulate(sg_str);
    } else {
        SgVarRefExp* sg_var_ref = SageBuilder::buildVarRefExp(sg_sym);
        return PyEncapsulate(sg_var_ref);
    }
}

/*
 */
PyObject*
sage_buildPower(PyObject *self, PyObject *args)
{
    PyObject* rhs = PyTuple_GetItem(args, 0);
    PyObject* lhs = PyTuple_GetItem(args, 1);

    SgExpression* sg_base = PyDecapsulate<SgExpression>(lhs);
    SgExpression* sg_pow  = PyDecapsulate<SgExpression>(rhs);
    SgExponentiationOp* sg_exp_op =
      SageBuilder::buildExponentiationOp(sg_base, sg_pow);
    return PyEncapsulate(sg_exp_op);
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
 */
PyObject*
sage_buildSuite(PyObject *self, PyObject *args)
{
    SgBasicBlock* sg_basic_block = SageBuilder::buildBasicBlock();
    PyObject* py_body = PyTuple_GetItem(args, 0);
    Py_ssize_t body_c = PyList_Size(py_body);
    for (int i = 0; i < body_c; i++) {
        PyObject* capsule = PyList_GetItem(py_body, i);
        SgStatement* sg_stmt = PyDecapsulate<SgStatement>(capsule);
        SageInterface::appendStatement(sg_stmt, sg_basic_block);
    }
    return PyEncapsulate(sg_basic_block);
}


/*
 */
PyObject*
sage_buildTryExcept(PyObject *self, PyObject *args)
{
    PyObject* py_body     = PyTuple_GetItem(args, 0);
    PyObject* py_handlers = PyTuple_GetItem(args, 1);
    PyObject* py_orelse   = PyTuple_GetItem(args, 2);

    // TODO: add buildTryExcept to SageBuilder
    cerr << "sage_buildTryExcept is unimplemented" << endl;
    SgNode* sg_value = SageBuilder::buildBasicBlock();
    return PyEncapsulate(sg_value);
}

/*
 */
PyObject*
sage_buildTryFinally(PyObject *self, PyObject *args)
{
    PyObject* py_body      = PyTuple_GetItem(args, 0);
    PyObject* py_finalbody = PyTuple_GetItem(args, 1);

    // TODO: add buildTryExcept to SageBuilder
    cerr << "sage_buildTryFinally is unimplemented" << endl;
    SgNode* sg_value = SageBuilder::buildBasicBlock();
    return PyEncapsulate(sg_value);
}

/*
 */
PyObject*
sage_buildUnaryOp(PyObject *self, PyObject *args)
{
    PyObject* py_operation = PyTuple_GetItem(args, 0);
    PyObject* py_operand   = PyTuple_GetItem(args, 1);

    SgExpression* operand = PyDecapsulate<SgExpression>(py_operand);
    std::string op = std::string( PyString_AsString(py_operation) );
    SgUnaryOp* sg_unary_op = NULL;
    if      (op == ROSE_PYTHON_UADD_OP)
        sg_unary_op = SageBuilder::buildUnaryAddOp(operand);
    else if (op == ROSE_PYTHON_USUB_OP)
        sg_unary_op = SageBuilder::buildMinusOp(operand);
    else if (op == ROSE_PYTHON_INVERT_OP)
        sg_unary_op = SageBuilder::buildBitComplementOp(operand);
    else {
        cerr << "Unrecognized unary operator: " << op << endl;
        ROSE_ABORT();
    }
    ROSE_ASSERT(sg_unary_op != NULL);
    return PyEncapsulate(sg_unary_op);
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
    return PyEncapsulate(sg_string_val);
}

/*
 */
PyObject*
sage_buildWhile(PyObject *self, PyObject *args)
{
    PyObject* py_test   = PyTuple_GetItem(args, 0);
    PyObject* py_body   = PyTuple_GetItem(args, 1);
    //PyObject* py_orelse = PyTuple_GetItem(args, 2);

    SgStatement* test = PyDecapsulate<SgStatement>(py_test);
    SgStatement* body = PyDecapsulate<SgStatement>(py_body);
    //SgStatement* orelse = PyDecapsulate<SgStatement>(py_orelse);

    SgWhileStmt* sg_while_stmt = SageBuilder::buildWhileStmt(test, body);
    return PyEncapsulate(sg_while_stmt);
}

/*
 */
PyObject*
sage_buildWith(PyObject *self, PyObject *args)
{
    PyObject* py_expr = PyTuple_GetItem(args, 0);
    PyObject* py_vars = PyTuple_GetItem(args, 1);
    PyObject* py_body = PyTuple_GetItem(args, 2);

    SgExpression* expr = PyDecapsulate<SgExpression>(py_expr);
    std::vector<SgVariableDeclaration*> vars; //PyDecapsulate<SgStatement>(py_vars);
    SgStatement*  body = PyDecapsulate<SgStatement>(py_body);

    ROSE_ASSERT(expr);
    ROSE_ASSERT(body);

    SgWithStatement* sg_with_stmt = SageBuilder::buildWithStatement(expr, vars, body);
    return PyEncapsulate(sg_with_stmt);
}
