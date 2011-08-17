#include <iostream>

#include "sage3basic.h"
#include "rose_config.h"

#include "SagePythonInterface.h"

#define ROSE_PYTHON_FRONTEND_MODULE_NAME "sageTranslator"
#define ROSE_PYTHON_FRONTEND_TRANSLATOR_FXN_NAME "translate"

using namespace std;

/*
 * Define the mapping from exported python functions to native C functions.
 */
static PyMethodDef SageBuilderMethods[] = {
    {"buildAssert",                sage_buildAssert,                METH_VARARGS, "Builds an assert node."},
    {"buildAssign",                sage_buildAssign,                METH_VARARGS, "Builds an assignment node."},
    {"buildAttr",                  sage_buildAttr,                  METH_VARARGS, "Builds an attribute reference node."},
    {"buildAugAssign",             sage_buildAugAssign,             METH_VARARGS, "Builds an augmented assignment node."},
    {"buildBinOp",                 sage_buildBinOp,                 METH_VARARGS, "Builds a binary expression node."},
    {"buildBoolOp",                sage_buildBoolOp,                METH_VARARGS, "Builds a boolean operation expression node."},
    {"buildBreak",                 sage_buildBreak,                 METH_VARARGS, "Builds a break node."},
    {"buildCall",                  sage_buildCall,                  METH_VARARGS, "Builds an SgCallExpression node."},
    {"buildClassDef",              sage_buildClassDef,              METH_VARARGS, "Builds a class def node."},
    {"buildCompare",               sage_buildCompare,               METH_VARARGS, "Builds a comparison node."},
    {"buildComprehension",         sage_buildComprehension,         METH_VARARGS, "Builds a complex value node."},
    {"buildComplexVal",            sage_buildComplexVal,            METH_VARARGS, "Builds a complex value node."},
    {"buildContinue",              sage_buildContinue,              METH_VARARGS, "Builds a continue stmt node."},
    {"buildDelete",                sage_buildDelete,                METH_VARARGS, "Builds a delete stmt node."},
    {"buildDict",                  sage_buildDict,                  METH_VARARGS, "Builds a dictionary display node."},
    {"buildDictComp",              sage_buildDictComp,              METH_VARARGS, "Builds a dictionary comprehension node."},
    {"buildExceptHandler",         sage_buildExceptHandler,         METH_VARARGS, "Builds an exception handler node."},
    {"buildExec",                  sage_buildExec,                  METH_VARARGS, "Builds an exec node."},
    {"buildExpr",                  sage_buildExpr,                  METH_VARARGS, "Builds an Expr node."},
    {"buildExprListExp",           sage_buildExprListExp,           METH_VARARGS, "Builds an expression list."},
    {"buildFloat",                 sage_buildFloat,                 METH_VARARGS, "Builds a float."},
    {"buildFor",                   sage_buildFor,                   METH_VARARGS, "Builds a for stmt."},
    {"buildFunctionDef",           sage_buildFunctionDef,           METH_VARARGS, "Builds an SgFunctionDeclaration node."},
    {"buildFunctionParameterList", sage_buildFunctionParameterList, METH_VARARGS, "Builds an SgFunctionParameterList node."},
    {"buildGlobal",                sage_buildGlobal,                METH_VARARGS, "Builds an SgPythonGlobalStmt node."},
    {"buildGlobalScope",           sage_buildGlobalScope,           METH_VARARGS, "Builds an SgGlobal node."},
    {"buildIf",                    sage_buildIf,                    METH_VARARGS, "Builds an if stmt node."},
    {"buildIfExp",                 sage_buildIfExp,                 METH_VARARGS, "Builds an if exp node."},
    {"buildImport",                sage_buildImport,                METH_VARARGS, "Builds an import stmt  node."},
    {"buildKeyword",               sage_buildKeyword,               METH_VARARGS, "Builds a keyword node."},
    {"buildKeyDatumPair",          sage_buildKeyDatumPair,          METH_VARARGS, "Builds a key datum pair node."},
    {"buildLongIntVal",            sage_buildLongIntVal,            METH_VARARGS, "Builds an SgIntVal node."},
    {"buildLambda",                sage_buildLambda,                METH_VARARGS, "Builds a lambda node."},
    {"buildListExp",               sage_buildListExp,               METH_VARARGS, "Builds a list node."},
    {"buildListComp",              sage_buildListComp,              METH_VARARGS, "Builds a list node."},
    {"buildPass",                  sage_buildPass,                  METH_VARARGS, "Builds a pass node."},
    {"buildPrintStmt",             sage_buildPrintStmt,             METH_VARARGS, "Builds an SgPrintStmt node."},
    {"buildName",                  sage_buildName,                  METH_VARARGS, "Builds an SgVarRefExp from a Name node."},
    {"buildRepr",                  sage_buildRepr,                  METH_VARARGS, "Builds string conversion node."},
    {"buildReturnStmt",            sage_buildReturnStmt,            METH_VARARGS, "Builds an SgReturnStmt node."},
    {"buildSetComp",               sage_buildSetComp,               METH_VARARGS, "Builds a set comprehension node."},
    {"buildSlice",                 sage_buildSlice,                 METH_VARARGS, "Builds a slice node."},
    {"buildStringVal",             sage_buildStringVal,             METH_VARARGS, "Builds an SgStringVal node."},
    {"buildSubscript",             sage_buildSubscript,             METH_VARARGS, "Builds a subscript node."},
    {"buildSuite",                 sage_buildSuite,                 METH_VARARGS, "Builds a suite in a basic block node."},
    {"buildTryExcept",             sage_buildTryExcept,             METH_VARARGS, "Builds an try stmt node."},
    {"buildTryFinally",            sage_buildTryFinally,            METH_VARARGS, "Builds an try/finally stmt node."},
    {"buildTuple",                 sage_buildTuple,                 METH_VARARGS, "Builds an tuple exp node."},
    {"buildUnaryOp",               sage_buildUnaryOp,               METH_VARARGS, "Builds unary operation node."},
    {"buildWhile",                 sage_buildWhile,                 METH_VARARGS, "Builds while stmt node."},
    {"buildWith",                  sage_buildWith,                  METH_VARARGS, "Builds with stmt node."},
    {"buildYield",                 sage_buildYield,                 METH_VARARGS, "Builds a yield node."},

    {"appendStatements", (PyCFunction)sage_appendStatements, METH_VARARGS | METH_KEYWORDS, "Add children to a given SgNode."},
    {"buildInitializedName", (PyCFunction)sage_buildInitializedName, METH_VARARGS|METH_KEYWORDS, "Builds an initialized name node."},

    {NULL, NULL, 0, NULL}
};

SgGlobal*
runPythonFrontend(SgFile* file)
{
    PyObject *pModule, *pFunc;
    PyObject *pValue, *pArgs;

    Py_InitModule("sage", SageBuilderMethods);

    // Hack to set sys.path (the list of paths that Python uses to look up modules)
    // so that the subsequent import succeeds. The program first looks in the
    // installation directory, then the source directory. (Automake's support for
    // Python does not copy .py (or .pyo, .pyc) into the compile tree).
    // TODO: find a better way to run/distribute the code in sageTranslator.py
    stringstream cmd;
    cmd << "import sys" << endl;
    cmd << "sys.path.insert(0, '" << ROSE_INSTALLATION_PATH << "/lib/python')" << endl;
    cmd << "sys.path.insert(0, '" << ROSE_AUTOMAKE_TOP_SRCDIR << "/src/frontend/PythonFrontend')" << endl;
    PyRun_SimpleString(cmd.str().c_str());
    pModule = PyImport_ImportModule(ROSE_PYTHON_FRONTEND_MODULE_NAME);


    if (pModule != NULL) {
        pFunc = PyObject_GetAttrString(pModule, ROSE_PYTHON_FRONTEND_TRANSLATOR_FXN_NAME);

        std::string filename = file->get_sourceFileNameWithPath();
        pValue = PyString_FromString(filename.c_str());
        pArgs = PyTuple_New(1);
        PyTuple_SetItem(pArgs, 0, pValue);

        if (pFunc && PyCallable_Check(pFunc)) {
            pValue = PyObject_CallObject(pFunc, pArgs);
            Py_DECREF(pArgs);

            if (pValue != NULL) {
                void* capsule_ptr = PyCapsule_GetPointer(pValue, NULL);
                SgGlobal* sg_global = static_cast<SgGlobal*>(capsule_ptr);
                Py_DECREF(pValue);
                return sg_global;
            }
            else {
                Py_DECREF(pFunc);
                Py_DECREF(pModule);
                PyErr_Print();
                ROSE_ABORT("Python frontend failed.");
                return NULL;
            }
        }
        else {
            if (PyErr_Occurred())
                PyErr_Print();
            cerr << "Cannot find function " <<
                    ROSE_PYTHON_FRONTEND_TRANSLATOR_FXN_NAME << endl;
        }
        Py_XDECREF(pFunc);
        Py_DECREF(pModule);
    }
    else {
        PyErr_Print();
        cerr << "Failed to load module " << ROSE_PYTHON_FRONTEND_MODULE_NAME << endl;
        return NULL;
    }
    return NULL;
}

int python_main(std::string filename, SgFile* file)
{
    //std::cout << "Launching interpreter." << std::endl;

    Py_Initialize();

    initializePythonTypes();
    SgGlobal* sg_global = runPythonFrontend(file);
    SageInterface::setSourcePositionForTransformation(sg_global);

    Py_Finalize();

    //const char* str = (sg_global != NULL) ? "success" : "failed";
    //std::cout << "Interpreter terminated (" << str << ")." << std::endl;

    SgSourceFile* sg_source_file = isSgSourceFile(file);
    sg_source_file->set_globalScope(sg_global);
    sg_global->set_parent(sg_source_file);
    file->set_skip_commentsAndDirectives(true);

    return -1;
}
