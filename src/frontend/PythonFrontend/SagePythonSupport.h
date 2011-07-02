#ifndef SAGE_PYTHON_SUPPORT_H_
#define SAGE_PYTHON_SUPPORT_H_

#include <Python.h>

class SgNode;

/*
 * Convenience function for encapsulating a C++ pointer in a Python
 * object. Uses an optional name and no destructor.
 */
PyObject* PyEncapsulate(void* ptr, char* name = NULL);

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
void set_File_Info(SgNode* sg_node, PyObject* py_file_info);

/*
 * Adds children to a node.
 * TODO: Add support for other SgNode subclasses besides SgGlobal.
 */
PyObject*
sage_appendStatements(PyObject *self, PyObject *args);


/**
 * Build a SgFunctionParameterList from the given Python
 * Arg object.
 */
PyObject*
sage_buildFunctionParameterList(PyObject* args, PyObject* kwargs);

#endif /* SAGE_PYTHON_SUPPORT_H_ */
