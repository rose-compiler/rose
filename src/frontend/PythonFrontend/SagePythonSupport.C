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

    PyObject* py_target = PyTuple_GetItem(args, 0);
    PyObject* py_stmts  = PyTuple_GetItem(args, 1);

    SgScopeStatement* sg_target =
        PyDecapsulate<SgScopeStatement>(py_target);
    ROSE_ASSERT(sg_target != NULL);

    Py_ssize_t stmtc = PyList_Size(py_stmts);
    for (int i = 0; i < stmtc; i++) {
        PyObject* capsule = PyList_GetItem(py_stmts, i);
        SgStatement* sg_child = PyDecapsulate<SgStatement>(capsule);
        // SageInterface::appendStatement calls functions that assert because of
        // (the lack of)? declaration statements in the given scopes. Until I suppress
        // this behavior for Python, use this simple version of the appendStatement
        // implementation:
        //SageInterface::appendStatement(sg_child, sg_target);
        switch (sg_target->variantT()) {
            case V_SgGlobal: {
              if (isSgDeclarationStatement(sg_child) == NULL) {
                  cerr << "adding non-declaration statements to the global scope is not yet supported" << endl;
              } else {
                  isSgGlobal(sg_target)->get_declarations().push_back(isSgDeclarationStatement(sg_child));
                  sg_child->set_parent(sg_target);
              }
              break;
            }
            case V_SgFunctionDeclaration: {
              SgBasicBlock* body = isSgFunctionDeclaration(sg_target)->get_definition()->get_body();
              body->append_statement(sg_child);
              sg_child->set_parent(body);
              break;
            }
            case V_SgLambdaRefExp: {
              SgBasicBlock* body = isSgLambdaRefExp(sg_target)->get_functionDeclaration()->get_definition()->get_body();
              body->append_statement(sg_child);
              sg_child->set_parent(body);
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
sage_buildInitializedName(PyObject* self, PyObject* py_args) {
    PyObject* py_name = PyTuple_GetItem(py_args, 0);

    PyObject* py_id = PyObject_GetAttrString(py_name, "id");
    string id = string( PyString_AsString(py_id) );

    SgType* sg_type = SageBuilder::buildVoidType();

    SgInitializedName* sg_init_name = SageBuilder::buildInitializedName(id, sg_type);
    return PyEncapsulate(sg_init_name);
}

PyObject*
sage_buildFunctionParameterList(PyObject* self, PyObject* py_argv) {
    PyObject* py_args = PyTuple_GetItem(py_argv, 0);
    PyObject* py_defaults = PyTuple_GetItem(py_argv, 1);

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

