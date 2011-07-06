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

template <typename SgNode_T>
static int sage_converter(PyObject* object, void** address) {
    if (! PyCapsule_CheckExact(object))
        return false;

    SgNode_T* sg_node = PyDecapsulate<SgNode_T>(object);
    if (! isSgNode(sg_node))
        return false;

    *address = sg_node;
    return true;
}
#define SAGE_CONVERTER(sg_t) \
  (int (*)(PyObject*,void**)) &sage_converter<sg_t>

static int pylist_checker(PyObject* object, void** address) {
    if (! PyList_Check(object)) {
        return false;
    } else {
        *address = object;
        return true;
    }
}

#endif /* SAGE_PYTHON_SUPPORT_H_ */
