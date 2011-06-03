#ifndef SAGE_PYTHON_INTERFACE_H_
#define SAGE_PYTHON_INTERFACE_H_

#include "Python.h"

/* Return the number of arguments of the application command line */
PyObject*
emb_numargs(PyObject *self, PyObject *args);

#endif /* SAGE_PYTHON_INTERFACE_H_ */
