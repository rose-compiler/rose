#include <iostream>

#include "sage3basic.h"
#include "sageBuilder.h"
#include "SagePythonInterface.h"

/**
 * Implementation of the Sage-Python interface.
 */

using namespace std;

/*
 * Convenience function for encapsulating a C++ pointer in a Python
 * object. Uses an optional name and no destructor.
 */
PyObject* PyEncapsulate(void* ptr, char* name = NULL) {
    return PyCapsule_New(ptr, name, /* destructor= */ NULL);
}

/*
 * Convenience function for extracting and casting a C++ pointer from
 * a Python object. Uses a NULL name.
 */
template <typename SgNode_T>
SgNode_T* PyDecapsulate(PyObject* capsule) {
    return static_cast<SgNode_T*>( PyCapsule_GetPointer(capsule, NULL) );
}

/*
 * Set the SgNode's Sg_File_Info parameters based on values from
 * the given FileInfo Python object.
 */
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
        cerr << "No start of construct for: " << sg_node->class_name() << endl;
    if (sg_node->get_endOfConstruct() == NULL)
        cerr << "No end of construct for: " << sg_node->class_name() << endl;
    cout << "File_Info for " << sg_node->class_name() 
        << "\n\tlineno = " << sg_file_info->get_line() 
        << "\n\tcolno = " << sg_file_info->get_col() 
        << "\n\tfname = " << sg_file_info->get_filenameString() << endl;
#endif
}

/*
 * Adds children to a node.
 * TODO: Add support for other SgNode subclasses besides SgGlobal.
 */
PyObject*
sage_addChildrenToNode(PyObject *self, PyObject *args)
{
    PyObject* sg_node_capsule = PyTuple_GetItem(args, 0);
    SgGlobal* sg_global = PyDecapsulate<SgGlobal>(sg_node_capsule);
    ROSE_ASSERT(sg_global);

    PyObject* argv = PyTuple_GetItem(args, 1);
    Py_ssize_t argc = PyList_Size(argv);
    for (int i = 0; i < argc; i++) {
       PyObject* capsule = PyList_GetItem(argv, i);
       SgDeclarationStatement* sg_child = 
           PyDecapsulate<SgDeclarationStatement>(capsule);
       sg_global->append_declaration(sg_child);
    }
    return Py_None;
}

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
    PyObject* scope_capsule = PyTuple_GetItem(args, 1);
    SgScopeStatement* sg_scope_statement = 
        PyDecapsulate<SgScopeStatement>(scope_capsule);

    // TODO: Figure out types, parse parameter list
    SgFunctionDeclaration* sg_func_decl = 
        SageBuilder::buildDefiningFunctionDeclaration("newfunc",
                SageBuilder::buildUnknownType(), 
                SageBuilder::buildFunctionParameterList(),
                sg_scope_statement);

    return PyEncapsulate(sg_func_decl);
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
    SgGlobal* sg_global = new SgGlobal(sg_file_info);
    sg_global->set_startOfConstruct(sg_file_info);
    sg_global->set_endOfConstruct(new Sg_File_Info(filename, 0, 0));
    return PyEncapsulate(sg_global);
}

/*
 * Build an SgLongIntVal node from the given Python integer.
 *  - PyObject* args = (PyObject*,)
 */
PyObject*
sage_buildLongIntVal(PyObject *self, PyObject *args)
{
    PyObject* val_obj = PyTuple_GetItem(args, 0);
    long value = PyInt_AsLong(val_obj);
    SgLongIntVal* sg_long_int_val = 
        SageBuilder::buildLongIntVal_nfi(value, "TODO");
    return PyEncapsulate(sg_long_int_val);
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
