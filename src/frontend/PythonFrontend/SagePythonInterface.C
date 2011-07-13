#include <iostream>

#include "sage3basic.h"
#include "sageBuilder.h"
#include "SagePythonInterface.h"

#include "unparsePython.h" // for operator ROSE_PYTHON_<op>_OP strings. TODO put somewhere better.

/**
 * Implementation of the Sage-Python interface.
 */


using namespace std;


PyObject* py_op_lt = NULL;
PyObject* py_op_gt = NULL;
PyObject* py_op_lte = NULL;
PyObject* py_op_gte = NULL;
PyObject* py_op_eq = NULL;
PyObject* py_op_noteq = NULL;
PyObject* py_op_is = NULL;
PyObject* py_op_isnot = NULL;
PyObject* py_op_in = NULL;
PyObject* py_op_notin = NULL;

PyObject* py_op_add = NULL;
PyObject* py_op_sub = NULL;
PyObject* py_op_mult = NULL;
PyObject* py_op_div = NULL;
PyObject* py_op_idiv = NULL;
PyObject* py_op_mod = NULL;
PyObject* py_op_lshift = NULL;
PyObject* py_op_rshift = NULL;
PyObject* py_op_bitand = NULL;
PyObject* py_op_bitor = NULL;
PyObject* py_op_bitxor = NULL;
PyObject* py_op_pow = NULL;

PyObject* py_op_uadd= NULL;
PyObject* py_op_usub = NULL;
PyObject* py_op_invert = NULL;

#define SAGE_PY_EVAL(str) \
    PyRun_String("str", Py_eval_input, PyEval_GetGlobals(), PyEval_GetLocals())

void
initializePythonTypes()
{
    PyObject *astModule = PyImport_ImportModule("ast");
    if (astModule == NULL)
        ROSE_ASSERT(!"Unable to initialize python types.");

    py_op_lt    = PyObject_GetAttrString(astModule, "Lt");
    py_op_lte   = PyObject_GetAttrString(astModule, "LtE");
    py_op_gt    = PyObject_GetAttrString(astModule, "Gt");
    py_op_gte   = PyObject_GetAttrString(astModule, "GtE");
    py_op_eq    = PyObject_GetAttrString(astModule, "Eq");
    py_op_noteq = PyObject_GetAttrString(astModule, "NotEq");
    py_op_is    = PyObject_GetAttrString(astModule, "Is");
    py_op_isnot = PyObject_GetAttrString(astModule, "IsNot");
    py_op_in    = PyObject_GetAttrString(astModule, "In");
    py_op_notin = PyObject_GetAttrString(astModule, "NotIn");

    py_op_add    = PyObject_GetAttrString(astModule, "Add");
    py_op_sub    = PyObject_GetAttrString(astModule, "Sub");
    py_op_mult   = PyObject_GetAttrString(astModule, "Mult");
    py_op_div    = PyObject_GetAttrString(astModule, "Div");
    py_op_idiv   = PyObject_GetAttrString(astModule, "FloorDiv");
    py_op_mod    = PyObject_GetAttrString(astModule, "Mod");
    py_op_lshift = PyObject_GetAttrString(astModule, "LShift");
    py_op_rshift = PyObject_GetAttrString(astModule, "RShift");
    py_op_bitand = PyObject_GetAttrString(astModule, "BitAnd");
    py_op_bitor  = PyObject_GetAttrString(astModule, "BitOr");
    py_op_bitxor = PyObject_GetAttrString(astModule, "BitXor");
    py_op_pow    = PyObject_GetAttrString(astModule, "Pow");

    py_op_uadd   = PyObject_GetAttrString(astModule, "UAdd");
    py_op_usub   = PyObject_GetAttrString(astModule, "USub");
    py_op_invert = PyObject_GetAttrString(astModule, "Invert");
}

/*
 */
PyObject*
sage_buildAssert(PyObject *self, PyObject *args)
{
    SgExpression *sg_test;
    if (! PyArg_ParseTuple(args, "O&", SAGE_CONVERTER(SgExpression), &sg_test))
        return NULL;

    SgAssertStmt* sg_assert_stmt = SageBuilder::buildAssertStmt(sg_test);
    return PyEncapsulate(sg_assert_stmt);
}

/*
 */
PyObject*
sage_buildAssign(PyObject *self, PyObject *args)
{
    SgExpression *sg_lhs_exp, *sg_rhs_exp;
    if (! PyArg_ParseTuple(args, "O&O&", SAGE_CONVERTER(SgExpression), &sg_lhs_exp,
                                         SAGE_CONVERTER(SgExpression), &sg_rhs_exp))
        return NULL;

    SgAssignOp* sg_assign_op = SageBuilder::buildAssignOp(sg_lhs_exp, sg_rhs_exp);
    return PyEncapsulate(sg_assign_op);
}

/*
 * Build an SgOp node from the given Python statements.
 *  - PyObject* args = (PyObject*, PyObject*)
 */
PyObject*
sage_buildAugAssign(PyObject *self, PyObject *args)
{
    SgExpression *lhs, *rhs;
    PyObject *op;
    if (! PyArg_ParseTuple(args, "O&O&O!", SAGE_CONVERTER(SgExpression), &lhs,
                                           SAGE_CONVERTER(SgExpression), &rhs,
                                           &PyType_Type, &op))
        return NULL;

    SgBinaryOp* sg_bin_op = NULL;
         if (op == py_op_add)    sg_bin_op = SageBuilder::buildPlusAssignOp(lhs, rhs);
    else if (op == py_op_sub)    sg_bin_op = SageBuilder::buildMinusAssignOp(lhs, rhs);
    else if (op == py_op_mult)   sg_bin_op = SageBuilder::buildMultAssignOp(lhs, rhs);
    else if (op == py_op_div)    sg_bin_op = SageBuilder::buildDivAssignOp(lhs, rhs);
    else if (op == py_op_idiv)   ROSE_ASSERT(!"Error: no SgIntegerDivAssignOp node.");
    else if (op == py_op_mod)    sg_bin_op = SageBuilder::buildModAssignOp(lhs, rhs);
    else if (op == py_op_lshift) sg_bin_op = SageBuilder::buildLshiftAssignOp(lhs, rhs);
    else if (op == py_op_rshift) sg_bin_op = SageBuilder::buildRshiftAssignOp(lhs, rhs);
    else if (op == py_op_bitand) ROSE_ASSERT(!"Error: no SgBitAndAssignOp node.");
    else if (op == py_op_bitor)  ROSE_ASSERT(!"Error: no SgBitOrAssignOp node.");
    else if (op == py_op_bitxor) ROSE_ASSERT(!"Error: no SgBitXorAssignOp node.");
    else if (op == py_op_pow)    sg_bin_op = SageBuilder::buildExponentiationOp(lhs, rhs);
    else {
        cout << "Unrecognized operator in augmented assignment: ";
        PyObject_Print(op, stdout, Py_PRINT_RAW);
        cout << endl;
        ROSE_ASSERT(false);
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
    SgExpression *lhs, *rhs;
    PyObject *op;
    if (! PyArg_ParseTuple(args, "O&O&O!", SAGE_CONVERTER(SgExpression), &lhs,
                                           SAGE_CONVERTER(SgExpression), &rhs,
                                           &PyType_Type, &op))
        return NULL;

    SgBinaryOp* sg_bin_op = NULL;
         if (op == py_op_add)    sg_bin_op = SageBuilder::buildAddOp(lhs, rhs);
    else if (op == py_op_sub)    sg_bin_op = SageBuilder::buildSubtractOp(lhs, rhs);
    else if (op == py_op_mult)   sg_bin_op = SageBuilder::buildMultiplyOp(lhs, rhs);
    else if (op == py_op_div)    sg_bin_op = SageBuilder::buildDivideOp(lhs, rhs);
    else if (op == py_op_idiv)   sg_bin_op = SageBuilder::buildIntegerDivideOp(lhs, rhs);
    else if (op == py_op_mod)    sg_bin_op = SageBuilder::buildModOp(lhs, rhs);
    else if (op == py_op_lshift) sg_bin_op = SageBuilder::buildLshiftOp(lhs, rhs);
    else if (op == py_op_rshift) sg_bin_op = SageBuilder::buildRshiftOp(lhs, rhs);
    else if (op == py_op_bitand) sg_bin_op = SageBuilder::buildBitAndOp(lhs, rhs);
    else if (op == py_op_bitor)  sg_bin_op = SageBuilder::buildBitOrOp(lhs, rhs);
    else if (op == py_op_bitxor) sg_bin_op = SageBuilder::buildBitXorOp(lhs, rhs);
    else if (op == py_op_pow)    sg_bin_op = SageBuilder::buildExponentiationOp(lhs, rhs);
    else {
        cout << "Unrecognized binary operator: ";
        PyObject_Print(op, stdout, Py_PRINT_RAW);
        cout << endl;
        ROSE_ASSERT(false);
    }

    return PyEncapsulate(sg_bin_op);
}

/*
 */
PyObject*
sage_buildBreak(PyObject *self, PyObject *args)
{
    if (! PyArg_ParseTuple(args, ""))
        return NULL;

    SgBreakStmt* sg_break = SageBuilder::buildBreakStmt();
    return PyEncapsulate(sg_break);
}

/*
 * Build an SgAddOp node from the given Python statements.
 *  - PyObject* args = (PyObject*, PyObject*)
 */
PyObject*
sage_buildCall(PyObject *self, PyObject *args)
{
    char *name_str;
    PyObject *py_args, *py_kwargs;
    SgScopeStatement *sg_scope;
    if (! PyArg_ParseTuple(args, "sO!O!O&", &name_str,
                                            &PyList_Type, &py_args,
                                            &PyList_Type, &py_kwargs,
                                            SAGE_CONVERTER(SgScopeStatement), &sg_scope))
        return NULL;

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

    SgName sg_func_name = SgName(name_str);
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
 */
PyObject*
sage_buildComprehension(PyObject *self, PyObject *args)
{
    SgExpression *sg_target, *sg_iter;
    SgExprListExp *sg_ifs;
    if (! PyArg_ParseTuple(args, "O&O&O&", SAGE_CONVERTER(SgExpression), &sg_target,
                                           SAGE_CONVERTER(SgExpression), &sg_iter,
                                           SAGE_CONVERTER(SgExprListExp), &sg_ifs))
        return NULL;

    SgComprehension* sg_comp = SageBuilder::buildComprehension(sg_target, sg_iter, sg_ifs);
    return PyEncapsulate(sg_comp);
}

/*
 * Build an SgOp node from the given Python statements.
 *  - PyObject* args = (PyObject*, PyObject*)
 */
PyObject*
sage_buildCompare(PyObject *self, PyObject *args)
{
    //char *op;
    PyObject *op;
    SgExpression *sg_lhs_exp, *sg_rhs_exp;
    if (! PyArg_ParseTuple(args, "O!O&O&", &PyType_Type, &op,
                                           SAGE_CONVERTER(SgExpression), &sg_lhs_exp,
                                           SAGE_CONVERTER(SgExpression), &sg_rhs_exp))
        return NULL;

    SgBinaryOp *sg_bin_op;
         if (op == py_op_lt)    sg_bin_op = SageBuilder::buildLessThanOp(sg_lhs_exp, sg_rhs_exp);
    else if (op == py_op_lte)   sg_bin_op = SageBuilder::buildLessOrEqualOp(sg_lhs_exp, sg_rhs_exp);
    else if (op == py_op_gt)    sg_bin_op = SageBuilder::buildGreaterThanOp(sg_lhs_exp, sg_rhs_exp);
    else if (op == py_op_gte)   sg_bin_op = SageBuilder::buildGreaterOrEqualOp(sg_lhs_exp, sg_rhs_exp);
    else if (op == py_op_is)    sg_bin_op = SageBuilder::buildGreaterOrEqualOp(sg_lhs_exp, sg_rhs_exp);
    else if (op == py_op_isnot) sg_bin_op = SageBuilder::buildGreaterOrEqualOp(sg_lhs_exp, sg_rhs_exp);
    else if (op == py_op_in)    sg_bin_op = SageBuilder::buildGreaterOrEqualOp(sg_lhs_exp, sg_rhs_exp);
    else if (op == py_op_notin) sg_bin_op = SageBuilder::buildGreaterOrEqualOp(sg_lhs_exp, sg_rhs_exp);
    else if (op == py_op_eq)    sg_bin_op = SageBuilder::buildEqualityOp(sg_lhs_exp, sg_rhs_exp);
    else if (op == py_op_noteq) sg_bin_op = SageBuilder::buildNotEqualOp(sg_lhs_exp, sg_rhs_exp);
    else {
        cout << "Unrecognized comparison operator: ";
        PyObject_Print(op, stdout, Py_PRINT_RAW);
        cout << endl;
        ROSE_ASSERT(false);
    }

    return PyEncapsulate(sg_bin_op);
}

/*
 */
PyObject*
sage_buildComplexVal(PyObject *self, PyObject *args)
{
    Py_complex value;
    if (! PyArg_ParseTuple(args, "D", &value))
        return NULL;

    SgDoubleVal* real_part = SageBuilder::buildDoubleVal(value.real);
    SgDoubleVal* imag_part = SageBuilder::buildDoubleVal(value.imag);
    SgComplexVal* sg_complex_val =
        SageBuilder::buildComplexVal(real_part, imag_part);
    return PyEncapsulate(sg_complex_val);
}

/*
 */
PyObject*
sage_buildContinue(PyObject *self, PyObject *args)
{
    if (! PyArg_ParseTuple(args, ""))
        return NULL;
    SgContinueStmt* sg_continue = SageBuilder::buildContinueStmt();
    return PyEncapsulate(sg_continue);
}

/*
 */
PyObject*
sage_buildDelete(PyObject *self, PyObject *args)
{
    SgExpression *sg_exp;
    if (! PyArg_ParseTuple(args, "O&", SAGE_CONVERTER(SgExpression), &sg_exp))
        return NULL;

    SgDeleteExp* sg_delete = SageBuilder::buildDeleteExp(sg_exp);
    return PyEncapsulate(sg_delete);
}

/*
 */
PyObject*
sage_buildDictComp(PyObject *self, PyObject *args)
{
    SgKeyDatumPair *kd_pair;
    SgExprListExp *generators;
    if (! PyArg_ParseTuple(args, "O&O&", SAGE_CONVERTER(SgKeyDatumPair), &kd_pair,
                                         SAGE_CONVERTER(SgExprListExp), &generators))
        return NULL;

    SgDictionaryComprehension *sg_dict_comp =
        SageBuilder::buildDictionaryComprehension(kd_pair, generators);
    return PyEncapsulate(sg_dict_comp);
}

/*
 */
PyObject*
sage_buildDict(PyObject *self, PyObject *args)
{
    PyObject *py_pairs;
    if (! PyArg_ParseTuple(args, "O!", &PyList_Type, &py_pairs))
        return NULL;

    std::vector<SgKeyDatumPair*> pairs;
    Py_ssize_t pairc = PyList_Size(py_pairs);
    for(int i = 0; i < pairc; i++) {
        PyObject* py_pair = PyList_GetItem(py_pairs, i);
        SgKeyDatumPair* kd_pair = PyDecapsulate<SgKeyDatumPair>(py_pair);
        pairs.push_back(kd_pair);
    }

    SgKeyDatumList* key_datum_list =
        SageBuilder::buildKeyDatumList(pairs);
    return PyEncapsulate(key_datum_list);
}

/*
 * Build an Expr node from the given Python statements.
 *  - PyObject* args = (PyObject*)
 */
PyObject*
sage_buildExceptHandler(PyObject *self, PyObject *args)
{
    char *name_str, *classobj_str;
    PyObject *py_body;
    SgScopeStatement *scope;
    if (! PyArg_ParseTuple(args, "zzO!O&", &name_str,
                                         &classobj_str,
                                         &PyList_Type, &py_body,
                                         SAGE_CONVERTER(SgScopeStatement), &scope))
        return NULL;

#if 1
    SgVariableDeclaration *var = NULL;
    if (classobj_str != NULL) {
        if (name_str != NULL) {
            SgExpression *exp = SageBuilder::buildOpaqueVarRefExp(string(name_str), scope);
            SgInitializer *init = SageBuilder::buildAssignInitializer(exp);
            var = SageBuilder::buildVariableDeclaration(name_str, SageBuilder::buildVoidType(), init);
        }
    }

    std::vector<SgStatement*> stmts;
    Py_ssize_t body_c = PyList_Size(py_body);
    for (int i = 0; i < body_c; i++)
        stmts.push_back( PyDecapsulate<SgStatement>( PyList_GetItem(py_body, i) ));
    SgBasicBlock *body = SageBuilder::buildBasicBlock_nfi(stmts);

    SgCatchOptionStmt *sg_catch =
        SageBuilder::buildCatchOptionStmt(var, body);
#else
        SgCatchOptionStmt *sg_catch = NULL;
#endif

    return PyEncapsulate(sg_catch);
}

/*
 * Build an Expr node from the given Python statements.
 *  - PyObject* args = (PyObject*)
 */
PyObject*
sage_buildExpr(PyObject *self, PyObject *args)
{
    SgExpression *sg_exp;
    if (! PyArg_ParseTuple(args, "O&", SAGE_CONVERTER(SgExpression), &sg_exp))
        return NULL;

    return PyEncapsulate(sg_exp);
}

/*
 */
PyObject*
sage_buildExprListExp(PyObject *self, PyObject *args)
{
    PyObject* py_exprs;
    if (! PyArg_ParseTuple(args, "O!", &PyList_Type, &py_exprs))
        return NULL;

    std::vector<SgExpression*> sg_exprs;
    Py_ssize_t exprc = PyList_Size(py_exprs);
    for(int i = 0; i < exprc; i++) {
        PyObject* py_exp = PyList_GetItem(py_exprs, i);
        SgExpression* sg_exp = PyDecapsulate<SgExpression>(py_exp);
        sg_exprs.push_back(sg_exp);
    }

    SgExprListExp* sg_expr_list_exp =
        SageBuilder::buildExprListExp(sg_exprs);
    return PyEncapsulate(sg_expr_list_exp);
}

/*
 */
PyObject*
sage_buildFloat(PyObject *self, PyObject *args)
{
    double val;
    if (! PyArg_ParseTuple(args, "d", &val))
        return NULL;

    SgDoubleVal* sg_double_val = SageBuilder::buildDoubleVal(val);
    return PyEncapsulate(sg_double_val);
}

/*
 */
PyObject*
sage_buildFor(PyObject *self, PyObject *args)
{
    SgExpression *iter;
    SgStatement *target, *body, *orelse;
    if (! PyArg_ParseTuple(args, "O&O&O&O&", SAGE_CONVERTER(SgStatement),  &target,
                                             SAGE_CONVERTER(SgExpression), &iter,
                                             SAGE_CONVERTER(SgStatement),  &body,
                                             SAGE_CONVERTER(SgStatement),  &orelse))
        return NULL;

    SgNullStatement* null = SageBuilder::buildNullStatement_nfi();
    SgForStatement* sg_for_stmt = SageBuilder::buildForStatement_nfi(target, null, iter, body, orelse);
    return PyEncapsulate(sg_for_stmt);
}

/*
 * Build a FunctionDef
 */
PyObject*
sage_buildFunctionDef(PyObject *self, PyObject *args)
{
    char *name;
    SgFunctionParameterList *sg_params;
    SgExprListExp *sg_decorators;
    SgScopeStatement *sg_scope;

    if (! PyArg_ParseTuple(args, "sO&OO&", &name,
                                            SAGE_CONVERTER(SgFunctionParameterList), &sg_params,
                                            /*SAGE_CONVERTER(SgExprListExp),*/ &sg_decorators,
                                            SAGE_CONVERTER(SgScopeStatement), &sg_scope))
        return NULL;

    SgFunctionDeclaration* sg_func_decl =
        SageBuilder::buildDefiningFunctionDeclaration(name,
                SageBuilder::buildVoidType(),
                sg_params,
                sg_scope);

#if 0 // awaiting resolution of abstract handle bug
    SgExprListExp* decorators = sg_func_decl->get_decoratorList();
    Py_ssize_t decc = PyList_Size(py_decorators);
    for(int i = 0; i < decc; i++) {
        PyObject* py_exp = PyList_GetItem(py_decorators, i);
        SgExpression* exp = PyDecapsulate<SgExpression>(py_exp);
        decorators->append_expression(exp);
    }
#endif

    PyObject *py_func_decl = PyEncapsulate(sg_func_decl);
    PyObject *py_func_def = PyEncapsulate(sg_func_decl->get_definition());
    return Py_BuildValue("(OO)", py_func_decl, py_func_def);
}

/*
 * Build an SgGlobal node from the given list of Python statements.
 *  - PyObject* args = ( [PyObject*, PyObject*, ...], )
 */
PyObject*
sage_buildGlobal(PyObject *self, PyObject *args)
{
    char *filename;
    if (! PyArg_ParseTuple(args, "s", &filename))
        return NULL;

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

    PyObject *py_global = PyEncapsulate(sg_global);
    PyObject *py_main = PyEncapsulate(sg_main_func_decl);
    return Py_BuildValue("(OO)", py_global, py_main);
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
    SgExpression *test;
    PyObject *py_body_list, *py_orelse_list;
    if (! PyArg_ParseTuple(args, "O&O!O!", SAGE_CONVERTER(SgExpression), &test,
                                           &PyList_Type, &py_body_list,
                                           &PyList_Type, &py_orelse_list))
        return NULL;

    SgBasicBlock* true_body = SageBuilder::buildBasicBlock();
    Py_ssize_t tbodyc = PyList_Size(py_body_list);
    for (int i = 0; i < tbodyc; i++) {
        PyObject* py_stmt = PyList_GetItem(py_body_list, i);
        SgStatement* sg_stmt = PyDecapsulate<SgStatement>(py_stmt);
        true_body->append_statement(sg_stmt);
    }

    Py_ssize_t fbodyc = PyList_Size(py_orelse_list);
    SgBasicBlock* false_body = (fbodyc > 0) ? SageBuilder::buildBasicBlock() : NULL;
    for (int i = 0; i < fbodyc; i++) {
        PyObject* py_stmt = PyList_GetItem(py_orelse_list, i);
        SgStatement* sg_stmt = PyDecapsulate<SgStatement>(py_stmt);
        false_body->append_statement(sg_stmt);
    }

    SgIfStmt* sg_if_stmt =
        SageBuilder::buildIfStmt(test, true_body, false_body);
    return PyEncapsulate(sg_if_stmt);
}

/*
 */
PyObject*
sage_buildKeyDatumPair(PyObject *self, PyObject *args)
{
    SgExpression *sg_key, *sg_value;
    if (! PyArg_ParseTuple(args, "O&O&", SAGE_CONVERTER(SgExpression), &sg_key,
                                         SAGE_CONVERTER(SgExpression), &sg_value))
        return NULL;

    SgKeyDatumPair* sg_kd_pair =
        SageBuilder::buildKeyDatumPair(sg_key, sg_value);

    return PyEncapsulate(sg_kd_pair);
}

/*
 * Builds a keyword node.
 */
PyObject*
sage_buildKeyword(PyObject *self, PyObject *args)
{
    SgExpression *sg_key, *sg_value;
    if (! PyArg_ParseTuple(args, "O&O&", SAGE_CONVERTER(SgExpression), &sg_key,
                                         SAGE_CONVERTER(SgExpression), &sg_value))
        return NULL;

    SgAssignOp* sg_assign_op =
        SageBuilder::buildAssignOp(sg_key, sg_value);

    return PyEncapsulate(sg_assign_op);
}

/*
 */
PyObject*
sage_buildLambda(PyObject *self, PyObject *args)
{
    SgFunctionParameterList *sg_params;
    SgScopeStatement *sg_scope;
    if (! PyArg_ParseTuple(args, "O&O&", SAGE_CONVERTER(SgFunctionParameterList), &sg_params,
                                         SAGE_CONVERTER(SgScopeStatement), &sg_scope))
        return NULL;

    SgType* sg_ret_type = SageBuilder::buildVoidType();
    SgLambdaRefExp* sg_lambda_exp =
        SageBuilder::buildLambdaRefExp(sg_ret_type, sg_params, sg_scope);

    ROSE_ASSERT(sg_lambda_exp);
    ROSE_ASSERT(sg_lambda_exp->get_functionDeclaration());
    ROSE_ASSERT(sg_lambda_exp->get_functionDeclaration()->get_definition());

    PyObject *py_lambda = PyEncapsulate(sg_lambda_exp);
    PyObject *py_body = PyEncapsulate(sg_lambda_exp->get_functionDeclaration()->get_definition());
    return Py_BuildValue("(OO)", py_lambda, py_body);
}

/*
 */
PyObject*
sage_buildListComp(PyObject *self, PyObject *args)
{
    SgExpression *elt;
    SgExprListExp *gens;
    if (! PyArg_ParseTuple(args, "O&O&", SAGE_CONVERTER(SgExpression), &elt,
                                         SAGE_CONVERTER(SgExprListExp), &gens))
        return NULL;

    SgListComprehension *sg_list_comp = SageBuilder::buildListComprehension(elt, gens);
    return PyEncapsulate(sg_list_comp);
}

/*
 */
PyObject*
sage_buildListExp(PyObject *self, PyObject *args)
{
    PyObject* py_elts;
    if (! PyArg_ParseTuple(args, "O!", &PyList_Type, &py_elts))
        return NULL;

    std::vector<SgExpression*> sg_elts;
    Py_ssize_t eltsc = PyList_Size(py_elts);
    for(int i = 0; i < eltsc; i++) {
        PyObject* py_exp = PyList_GetItem(py_elts, i);
        SgExpression* sg_exp = PyDecapsulate<SgExpression>(py_exp);
        sg_elts.push_back(sg_exp);
    }

    SgListExp* sg_list_exp =
        SageBuilder::buildListExp(sg_elts); //TODO change to SgListExp
    return PyEncapsulate(sg_list_exp);
}

/*
 * Build an SgLongIntVal node from the given Python integer.
 *  - PyObject* args = (PyObject*,)
 */
PyObject*
sage_buildLongIntVal(PyObject *self, PyObject *args)
{
    long long value;
    if (! PyArg_ParseTuple(args, "L", &value))
        return NULL;

    SgLongLongIntVal* sg_long_int_val =
        SageBuilder::buildLongLongIntVal(value);
    return PyEncapsulate(sg_long_int_val);
}

/*
 * Build an SgVarRefExp node from the given Python Name node.
 *  - PyObject* args = ( PyObject* id, PyObject* scope )
 */
PyObject*
sage_buildName(PyObject *self, PyObject *args)
{
    char *id;
    SgScopeStatement *sg_scope;
    if (! PyArg_ParseTuple(args, "sO&", &id,
                                        SAGE_CONVERTER(SgScopeStatement), &sg_scope))
        return NULL;

    SgVarRefExp* sg_var_ref =
        SageBuilder::buildOpaqueVarRefExp( std::string(id), sg_scope );
    return PyEncapsulate(sg_var_ref);
}


/*
 */
PyObject*
sage_buildPass(PyObject *self, PyObject *args)
{
    if (! PyArg_ParseTuple(args, ""))
        return NULL;

    SgPassStatement *sg_pass = SageBuilder::buildPassStatement();
    return PyEncapsulate(sg_pass);
}

/*
 * Build an SgPrintStmt node from the given list of Python
 * expressions.
 *  - PyObject* args = ( [PyObject*, PyObject*, ... ], )
 */
PyObject*
sage_buildPrintStmt(PyObject *self, PyObject *args)
{
    SgExpression *sg_dest;
    SgExprListExp *sg_vals;
    if (! PyArg_ParseTuple(args, "O&O&", SAGE_CONVERTER(SgExpression), &sg_dest,
                                         SAGE_CONVERTER(SgExprListExp), &sg_vals))
        return NULL;

    SgPythonPrintStmt* sg_print_stmt =
        SageBuilder::buildPythonPrintStmt(sg_dest, sg_vals);
    return PyEncapsulate(sg_print_stmt);
}

/*
 * Build an SgReturnStmt node from the given Python object.
 *  - PyObject* args = ( PyObject*, )
 */
PyObject*
sage_buildReturnStmt(PyObject *self, PyObject *args)
{
    SgExpression* sg_exp;
    if (! PyArg_ParseTuple(args, "O&", SAGE_CONVERTER(SgExpression), &sg_exp))
        return NULL;

    SgReturnStmt* sg_return = SageBuilder::buildReturnStmt(sg_exp);
    return PyEncapsulate(sg_return);
}

/*
 */
PyObject*
sage_buildSetComp(PyObject *self, PyObject *args)
{
    SgExpression *elt;
    SgExprListExp *generators;
    if (! PyArg_ParseTuple(args, "O&O&", SAGE_CONVERTER(SgExpression), &elt,
                                         SAGE_CONVERTER(SgExprListExp), &generators))
        return NULL;

    SgSetComprehension *sg_set_comp =
        SageBuilder::buildSetComprehension(elt, generators);
    return PyEncapsulate(sg_set_comp);
}

/*
 */
PyObject*
sage_buildSuite(PyObject *self, PyObject *args)
{
    PyObject* py_body;
    if (! PyArg_ParseTuple(args, "O!", &PyList_Type, &py_body))
        return NULL;

    SgBasicBlock* sg_basic_block = SageBuilder::buildBasicBlock();
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
    PyObject *py_handlers;
    SgStatement *body, *orelse;
    if (! PyArg_ParseTuple(args, "O&O!O&", SAGE_CONVERTER(SgStatement), &body,
                                           &PyList_Type, &py_handlers,
                                           SAGE_CONVERTER(SgStatement), &orelse))
        return NULL;

    SgTryStmt* sg_try = SageBuilder::buildTryStmt(body);

    Py_ssize_t handler_c = PyList_Size(py_handlers);
    for (int i = 0; i < handler_c; i++)
        sg_try->append_catch_statement( PyDecapsulate<SgCatchOptionStmt>( PyList_GetItem(py_handlers, i) ) );

    return PyEncapsulate(sg_try);
}

/*
 */
PyObject*
sage_buildTryFinally(PyObject *self, PyObject *args)
{
    PyObject *py_body, *py_finalbody;
    if (! PyArg_ParseTuple(args, ""))
        return NULL;

    // TODO: add buildTryExcept to SageBuilder
    cerr << "sage_buildTryFinally is unimplemented" << endl;
    SgNode* sg_value = SageBuilder::buildBasicBlock();
    return PyEncapsulate(sg_value);
}

/*
 */
PyObject*
sage_buildTuple(PyObject *self, PyObject *args)
{
    PyObject *py_elts;
    if (! PyArg_ParseTuple(args, "O!", &PyList_Type, &py_elts))
        return NULL;

    std::vector<SgExpression*> sg_exprs;
    Py_ssize_t eltsc = PyList_Size(py_elts);
    for(int i = 0; i < eltsc; i++) {
        PyObject* py_exp = PyList_GetItem(py_elts, i);
        SgExpression* sg_exp = PyDecapsulate<SgExpression>(py_exp);
        sg_exprs.push_back(sg_exp);
    }

    SgTupleExp* sg_tuple_exp = SageBuilder::buildTupleExp(sg_exprs);
    return PyEncapsulate(sg_tuple_exp);
}

/*
 */
PyObject*
sage_buildUnaryOp(PyObject *self, PyObject *args)
{
    PyObject *op;
    SgExpression *operand;
    if (! PyArg_ParseTuple(args, "O!O&", &PyType_Type, &op,
                                         SAGE_CONVERTER(SgExpression), &operand))
        return NULL;

    SgUnaryOp* sg_unary_op = NULL;
         if (op == py_op_uadd)   sg_unary_op = SageBuilder::buildUnaryAddOp(operand);
    else if (op == py_op_usub)   sg_unary_op = SageBuilder::buildMinusOp(operand);
    else if (op == py_op_invert) sg_unary_op = SageBuilder::buildBitComplementOp(operand);
    else {
        cout << "Unrecognized unary operator: ";
        PyObject_Print(op, stdout, Py_PRINT_RAW);
        cout << endl;
        ROSE_ASSERT(false);
    }

    return PyEncapsulate(sg_unary_op);
}

/*
 * Build an SgStringVal node from the given Python String object.
 *  - PyObject* args = ( PyStringObject*, )
 */
PyObject*
sage_buildStringVal(PyObject *self, PyObject *args)
{
    char *c_str;
    if (! PyArg_ParseTuple(args, "s", &c_str))
        return NULL;

    std::string str = std::string(c_str);
    SgStringVal* sg_string_val = SageBuilder::buildStringVal(str);
    return PyEncapsulate(sg_string_val);
}

/*
 */
PyObject*
sage_buildWhile(PyObject *self, PyObject *args)
{
    SgStatement *test, *body, *orelse;
    if (! PyArg_ParseTuple(args, "O&O&|O&", SAGE_CONVERTER(SgStatement), &test,
                                            SAGE_CONVERTER(SgStatement), &body,
                                            SAGE_CONVERTER(SgStatement), &orelse))
        return NULL;

    SgWhileStmt* sg_while_stmt = SageBuilder::buildWhileStmt(test, body);
    return PyEncapsulate(sg_while_stmt);
}

/*
 */
PyObject*
sage_buildWith(PyObject *self, PyObject *args)
{
    SgExpression *expr;
    SgStatement *vars, *body;
    if (! PyArg_ParseTuple(args, "O&O&|O&", SAGE_CONVERTER(SgExpression), &expr,
                                            SAGE_CONVERTER(SgStatement), &body,
                                            SAGE_CONVERTER(SgStatement), &vars))
        return NULL;

    std::vector<SgVariableDeclaration*> no_vars; //PyDecapsulate<SgStatement>(py_vars);
    SgWithStatement* sg_with_stmt = SageBuilder::buildWithStatement(expr, no_vars, body);
    return PyEncapsulate(sg_with_stmt);
}

/*
 */
PyObject*
sage_buildYield(PyObject *self, PyObject *args)
{
    SgExpression *expr;
    if (! PyArg_ParseTuple(args, "O&", SAGE_CONVERTER(SgExpression), &expr))
        return NULL;

    SgYieldStatement* sg_yield = SageBuilder::buildYieldStatement(expr);
    return PyEncapsulate(sg_yield);
}
