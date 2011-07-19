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
        ROSE_ASSERT(capsule != NULL);
        SgStatement* sg_child = PyDecapsulate<SgStatement>(capsule);
        if (sg_child == NULL) {
            SgNode* sg_node = PyDecapsulate<SgNode>(capsule);
            cout << "bad node: " << sg_node->class_name() << endl;
        }
        // SageInterface::appendStatement calls functions that assert because of
        // (the lack of)? declaration statements in the given scopes. Until I suppress
        // this behavior for Python, use this simple version of the appendStatement
        // implementation:
        //SageInterface::appendStatement(sg_child, sg_target);
        switch (sg_target->variantT()) {
            case V_SgGlobal: {
              SgDeclarationStatement* sg_decl_stmt = isSgDeclarationStatement(sg_child);
              if (sg_decl_stmt == NULL) {
                  sg_decl_stmt = SageBuilder::buildStmtDeclarationStatement(sg_child);
              }
              isSgGlobal(sg_target)->get_declarations().push_back(sg_decl_stmt);
              sg_decl_stmt->set_parent(sg_target);
              break;
            }
            case V_SgFunctionDeclaration: {
              SgBasicBlock* body = isSgFunctionDeclaration(sg_target)->get_definition()->get_body();
              ROSE_ASSERT(body);
              ROSE_ASSERT(sg_child);
              body->append_statement(sg_child);
              sg_child->set_parent(body);
              break;
            }
            case V_SgLambdaRefExp: {
              SgBasicBlock* body = isSgLambdaRefExp(sg_target)->get_functionDeclaration()->get_definition()->get_body();
              SageInterface::appendStatement(sg_child, body);
              break;
            }
            case V_SgClassDefinition: {
              SgClassDefinition* class_def = isSgClassDefinition(sg_target);
              SageInterface::appendStatement(sg_child, class_def);
              break;
            }
            default:
              cerr << "Unhandled node type in sage_appendStatements: " << sg_target->class_name() << endl;
              break;
        }
    }

    return Py_None;
}

PyObject*
sage_buildInitializedName(PyObject* self, PyObject* args) {
    char *id;
    if (! PyArg_ParseTuple(args, "s", &id))
        return NULL;

    SgType* sg_type = SageBuilder::buildVoidType();
    SgInitializedName* sg_init_name = SageBuilder::buildInitializedName(id, sg_type);
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

SgExpression*
buildReference(char* id, SgScopeStatement* scope) {
    ROSE_ASSERT(scope != NULL);

    SgSymbol* sg_sym = scope->lookup_symbol( SgName(id) );
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
