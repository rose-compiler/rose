#include <iostream>

#include "sage3basic.h"
#include "sageBuilder.h"

#include "SagePythonSupport.h"

using namespace std;
using namespace SageBuilder;

PyObject* PyEncapsulate(void* ptr, char* name) {
    return PyCapsule_New(ptr, name, /* destructor= */ NULL);
}

void set_File_Info(SgNode* sg_node, PyObject* py_file_info) {
    ROSE_ASSERT(sg_node != NULL);
    Sg_File_Info* sg_file_info = sg_node->get_file_info();

    /* Create a new Sg_File_Info node if one doesn't exist. */
    if (sg_file_info == NULL) {
        cerr << "Warning: no existing file info for "
             << sg_node->class_name() << endl;
        sg_file_info = new Sg_File_Info();
        SgLocatedNode* sg_located_node = isSgLocatedNode(sg_node);
        if (sg_located_node == NULL) {
            cerr << "warning: cannot set file info on "
                << sg_node->class_name() << endl;
        } else {
            sg_located_node->set_file_info( sg_file_info );
        }
    }

    /* General settings */
    sg_file_info->unsetTransformation();
    sg_file_info->setOutputInCodeGeneration();

    /* Set the line number */
    PyObject* py_lineno = PyObject_GetAttrString(py_file_info, "lineno");
    int lineno = (int) PyInt_AsLong(py_lineno);
    sg_file_info->set_line(lineno);

    /* Set the column offset */
    PyObject* py_col_offset = PyObject_GetAttrString(py_file_info, "col_offset");
    int col_offset = (int) PyInt_AsLong(py_col_offset);
    sg_file_info->set_col(col_offset);

    /* Set the file name */
    PyObject* py_filename = PyObject_GetAttrString(py_file_info, "filename");
    std::string filename = std::string( PyString_AsString(py_filename) );
    sg_file_info->set_filenameString(filename);

#if 0
    if (sg_node->get_startOfConstruct() == NULL)
        cerr << "No start of construct for: "
             << sg_node->class_name() << endl;
    if (sg_node->get_endOfConstruct() == NULL)
        cerr << "No end of construct for: "
             << sg_node->class_name() << endl;
    cout << "File_Info for " << sg_node->class_name()
        << "\n\tlineno = " << sg_file_info->get_line()
        << "\n\tcolno = " << sg_file_info->get_col()
        << "\n\tfname = " << sg_file_info->get_filenameString() << endl;
#endif
}

PyObject*
sage_appendStatements(PyObject *self, PyObject *args)
{
    PyObject *py_stmts;
    SgScopeStatement *sg_target;
    if (! PyArg_ParseTuple(args, "O&O!", SAGE_CONVERTER(SgScopeStatement), &sg_target,
                                         &PyList_Type, &py_stmts))
        return NULL;

    Py_ssize_t stmtc = PyList_Size(py_stmts);
    for (int i = 0; i < stmtc; i++) {
        PyObject* capsule = PyList_GetItem(py_stmts, i);
        ROSE_ASSERT(PyCapsule_CheckExact(capsule));

        SgStatement* sg_child = PyDecapsulate<SgStatement>(capsule);
        switch (sg_target->variantT()) {
            case V_SgBasicBlock:
              break;
            case V_SgFunctionDeclaration:
              sg_target = isSgFunctionDeclaration(sg_target)->get_definition()->get_body();
              break;
            case V_SgLambdaRefExp:
              sg_target = isSgLambdaRefExp(sg_target)->get_functionDeclaration()->get_definition()->get_body();
              break;
            case V_SgGlobal:
            case V_SgClassDefinition:
              sg_child = normalizeToDeclarationStatement(sg_child);
              break;
            default: {
              cout << "Unhandled node type in sage_appendStatements: " << sg_target->class_name() << endl;
              ROSE_ASSERT(false);
              break;
            }
        }
        SageInterface::appendStatement(sg_child, sg_target);
    }

    return Py_None;
}

PyObject*
sage_buildInitializedName(PyObject* self, PyObject* args, PyObject* kwargs) {
    char *id;
    SgExpression *sg_init_val = NULL;
    PyObject *starred = NULL, *dstarred = NULL;
    static char *kwlist[] = {"id", "init_val", "starred", "dstarred", NULL};
    if (! PyArg_ParseTupleAndKeywords(args, kwargs, "s|O&O!O!", kwlist,
                                         &id,
                                         SAGE_CONVERTER(SgExpression), &sg_init_val,
                                         &PyBool_Type, &starred,
                                         &PyBool_Type, &dstarred))
        return NULL;

    SgType* sg_type = SageBuilder::buildVoidType();
    SgInitializer* sg_init = (sg_init_val == NULL) ?
        NULL : SageBuilder::buildAssignInitializer(sg_init_val);
    SgInitializedName* sg_init_name = SageBuilder::buildInitializedName(id, sg_type, sg_init);

    ROSE_ASSERT(! (starred == Py_True && dstarred == Py_True));
    if (starred == Py_True)
        sg_init_name->set_excess_specifier( SgInitializedName::e_excess_specifier_positionals );
    if (dstarred == Py_True)
        sg_init_name->set_excess_specifier( SgInitializedName::e_excess_specifier_keywords );

    return PyEncapsulate(sg_init_name);
}

PyObject*
sage_buildFunctionParameterList(PyObject* self, PyObject* args) {
    PyObject *py_args, *py_defaults;
    if (! PyArg_ParseTuple(args, "O!O!", &PyList_Type, &py_args,
                                         &PyList_Type, &py_defaults))
        return NULL;

    SgFunctionParameterList* sg_params =
        SageBuilder::buildFunctionParameterList();

    Py_ssize_t py_argc = PyList_Size(py_args);
    Py_ssize_t py_defaults_argc = (py_defaults != NULL) ?
        PyList_Size(py_defaults) : 0;
    Py_ssize_t py_simples_argc = py_argc - py_defaults_argc;

    /* Handle positional parameters */
    for (int i = 0; i < py_simples_argc; i++) {
        PyObject* py_name = PyList_GetItem(py_args, i);
        SgInitializedName* sg_init_name =
            PyDecapsulate<SgInitializedName>(py_name);
        sg_params->append_arg(sg_init_name);
    }

    /* Handle default parameters */
    for (int i = 0; i < py_defaults_argc; i++) {
        PyObject* py_name = PyList_GetItem(py_args, py_simples_argc+i);
        SgInitializedName* sg_name =
            PyDecapsulate<SgInitializedName>(py_name);

        PyObject* py_default = PyList_GetItem(py_defaults, i);
        SgExpression* sg_default = PyDecapsulate<SgExpression>(py_default);
        SgInitializer* sg_init =
            SageBuilder::buildAssignInitializer(sg_default);

        sg_name->set_initptr(sg_init);
        sg_init->set_parent(sg_name);

        sg_params->append_arg(sg_name);
    }

    return PyEncapsulate(sg_params);
}

SgDeclarationStatement*
normalizeToDeclarationStatement(SgStatement* stmt) {
    ROSE_ASSERT(stmt != NULL);

    SgDeclarationStatement* decl_stmt = isSgDeclarationStatement(stmt);
    if (decl_stmt == NULL)
        decl_stmt = SageBuilder::buildStmtDeclarationStatement(stmt);

    return decl_stmt;
}

SgExpression*
buildReference(char* id, SgScopeStatement* scope) {
    ROSE_ASSERT(scope != NULL);

    SgSymbol* sg_sym = SageInterface::lookupSymbolInParentScopes( SgName(id), scope );
    if (sg_sym == NULL) {
        return SageBuilder::buildOpaqueVarRefExp(id, scope);
    }
    else switch (sg_sym->variantT()) {
        case V_SgFunctionSymbol:
            return SageBuilder::buildFunctionRefExp( isSgFunctionSymbol(sg_sym) );
        case V_SgVariableSymbol:
            return SageBuilder::buildVarRefExp( isSgVariableSymbol(sg_sym) );
        case V_SgClassSymbol:
            return SageBuilder::buildClassNameRefExp( isSgClassSymbol(sg_sym) );
        default: {
            cout << "error: unable to convert python name: " << id
                 << ". Corresponding symbol had type: " << sg_sym->class_name() << endl;
            ROSE_ASSERT(false);
            break;
        }
    }
    return NULL;
}
