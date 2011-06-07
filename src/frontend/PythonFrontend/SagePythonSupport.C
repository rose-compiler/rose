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
sage_addChildrenToNode(PyObject *self, PyObject *args)
{
    PyObject* sg_node_capsule = PyTuple_GetItem(args, 0);
    SgNode* sg_node = PyDecapsulate<SgNode>(sg_node_capsule);
    ROSE_ASSERT(sg_node);

    PyObject* argv = PyTuple_GetItem(args, 1);
    Py_ssize_t argc = PyList_Size(argv);

    switch (sg_node->variantT()) {
        case V_SgGlobal: {
                 SgGlobal* sg_global = isSgGlobal(sg_node);
                 for (int i = 0; i < argc; i++) {
                     PyObject* capsule = PyList_GetItem(argv, i);
                     SgDeclarationStatement* sg_child =
                         PyDecapsulate<SgDeclarationStatement>(capsule);
                     sg_global->append_declaration(sg_child);
                 }
                 break;
             }
        case V_SgFunctionDeclaration: {
                 SgFunctionDeclaration* sg_fun_decl =
                     isSgFunctionDeclaration(sg_node);
                 SgBasicBlock* sg_basic_block =
                     sg_fun_decl->get_definition()->get_body();
                 ROSE_ASSERT(sg_basic_block);
                 for (int i = 0; i < argc; i++) {
                     PyObject* capsule = PyList_GetItem(argv, i);
                     SgStatement* sg_child =
                         PyDecapsulate<SgStatement>(capsule);
                     sg_basic_block->get_statements().push_back(sg_child);
                 }
                 break;
             }
        default: {
                 cerr << "Unhandled node type in sage_addChildrenToNode "
                     << sg_node->class_name() << endl;
                 ROSE_ASSERT(!"unhandled node type");
                 break;
             }
    }
    return Py_None;
}

SgFunctionParameterList*
buildFunctionParameterList(PyObject* args, PyObject* py_defaults_list) {
    PyObject* py_args = PyObject_GetAttrString(args, "args");
    SgFunctionParameterList* sg_params =
        SageBuilder::buildFunctionParameterList();

    Py_ssize_t py_argc = PyList_Size(py_args);
    Py_ssize_t py_defaults_argc = PyList_Size(py_defaults_list);
    Py_ssize_t py_simples_argc = py_argc - py_defaults_argc;

    /* Handle simple parameters */
    for (int i = 0; i < py_simples_argc; i++) {
        PyObject* py_name = PyList_GetItem(py_args, i);
        PyObject* py_id = PyObject_GetAttrString(py_name, "id");
        char* id = PyString_AsString(py_id);

        SgType* sg_type = SageBuilder::buildVoidType();
        SgInitializedName* sg_name =
            SageBuilder::buildInitializedName(id, sg_type);

        sg_params->append_arg(sg_name);
    }

    /* Handle default parameters */
    for (int i = 0; i < py_defaults_argc; i++) {
        PyObject* py_name = PyList_GetItem(py_args, py_simples_argc+i);
        PyObject* py_default = PyList_GetItem(py_defaults_list, i);
        PyObject* py_id = PyObject_GetAttrString(py_name, "id");
        char* id = PyString_AsString(py_id);

        SgExpression* sg_default = PyDecapsulate<SgExpression>(py_default);
        SgType* sg_type = SageBuilder::buildVoidType();
        SgInitializer* sg_init =
            SageBuilder::buildAssignInitializer(sg_default);
        SgInitializedName* sg_name =
            SageBuilder::buildInitializedName(id, sg_type, sg_init);

        sg_params->append_arg(sg_name);
    }

    return sg_params;
}

