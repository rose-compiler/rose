#include "SagePythonInterface.h"

/**
 * Implementation of the Sage-Python interface.
 */

/* Return the number of arguments of the application command line */
PyObject*
emb_numargs(PyObject *self, PyObject *args)
{
    if(!PyArg_ParseTuple(args, ":numargs"))
        return NULL;
    return Py_BuildValue("i", 0);
}
