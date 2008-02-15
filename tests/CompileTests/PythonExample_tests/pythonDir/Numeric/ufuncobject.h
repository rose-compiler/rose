#ifndef Py_UFUNCOBJECT_H
#define Py_UFUNCOBJECT_H
#ifdef __cplusplus
extern "C" {
#endif

typedef void (VBinaryFunction) (char *, int, char *, int,
				char *, int, int, void *);
typedef void (VUnaryFunction) (char *, int, char *, int, int, void *);

typedef void (*PyUFuncGenericFunction) (char **, int *, int *, void *);

typedef struct {
  PyObject_HEAD
    /* XXXX Add your own stuff here */
  int *ranks, *canonical_ranks;
  int nin, nout, nargs;
  int identity;
  PyUFuncGenericFunction *functions;
  void **data;
  int ntypes, nranks, attributes;
  char *name, *types;
  int check_return;
  char *doc;
} PyUFuncObject;

#define PyUFunc_Unbounded 120
#define PyUFunc_One 1
#define PyUFunc_Zero 0
#define PyUFunc_None -1


#define PyUFunc_Type_NUM 0

#define PyUFunc_FromFuncAndData_RET PyObject *
#define PyUFunc_FromFuncAndData_PROTO (PyUFuncGenericFunction *func, void **data, char *types, int ntypes, int nin, int nout, int identity, char *name, char *doc, int check_return)
#define PyUFunc_FromFuncAndData_NUM 1

#define PyUFunc_GenericFunction_RET int
#define PyUFunc_GenericFunction_PROTO (PyUFuncObject *self, PyObject *args, PyArrayObject **mps)
#define PyUFunc_GenericFunction_NUM 2

#define PyUFunc_f_f_As_d_d_RET void
#define PyUFunc_f_f_As_d_d_PROTO (char **args, int *dimensions, int *steps, void *func)
#define PyUFunc_f_f_As_d_d_NUM 3

#define PyUFunc_d_d_RET void
#define PyUFunc_d_d_PROTO (char **args, int *dimensions, int *steps, void *func)
#define PyUFunc_d_d_NUM 4

#define PyUFunc_F_F_As_D_D_RET void
#define PyUFunc_F_F_As_D_D_PROTO (char **args, int *dimensions, int *steps, void *func)
#define PyUFunc_F_F_As_D_D_NUM 5

#define PyUFunc_D_D_RET void
#define PyUFunc_D_D_PROTO (char **args, int *dimensions, int *steps, void *func)
#define PyUFunc_D_D_NUM 6

#define PyUFunc_O_O_RET void
#define PyUFunc_O_O_PROTO (char **args, int *dimensions, int *steps, void *func)
#define PyUFunc_O_O_NUM 7

#define PyUFunc_ff_f_As_dd_d_RET void
#define PyUFunc_ff_f_As_dd_d_PROTO (char **args, int *dimensions, int *steps, void *func)
#define PyUFunc_ff_f_As_dd_d_NUM 8

#define PyUFunc_dd_d_RET void
#define PyUFunc_dd_d_PROTO (char **args, int *dimensions, int *steps, void *func)
#define PyUFunc_dd_d_NUM 9

#define PyUFunc_FF_F_As_DD_D_RET void
#define PyUFunc_FF_F_As_DD_D_PROTO (char **args, int *dimensions, int *steps, void *func)
#define PyUFunc_FF_F_As_DD_D_NUM 10

#define PyUFunc_DD_D_RET void
#define PyUFunc_DD_D_PROTO (char **args, int *dimensions, int *steps, void *func)
#define PyUFunc_DD_D_NUM 11

#define PyUFunc_OO_O_RET void
#define PyUFunc_OO_O_PROTO (char **args, int *dimensions, int *steps, void *func)
#define PyUFunc_OO_O_NUM 12

#define PyUFunc_O_O_method_RET void
#define PyUFunc_O_O_method_PROTO (char **args, int *dimensions, int *steps, void *func)
#define PyUFunc_O_O_method_NUM 13

#define PyArray_Map_RET PyObject *
#define PyArray_Map_PROTO (PyObject *function, PyObject *args)
#define PyArray_Map_NUM 14

/* Total number of C API pointers */
#define PyUFunc_API_pointers 15


#ifdef _UFUNC_MODULE

extern PyTypeObject PyUFunc_Type;

#define PyUFunc_Check(op) ((op)->ob_type == &PyUFunc_Type)

extern PyUFunc_FromFuncAndData_RET PyUFunc_FromFuncAndData \
       PyUFunc_FromFuncAndData_PROTO;
extern PyUFunc_GenericFunction_RET PyUFunc_GenericFunction \
       PyUFunc_GenericFunction_PROTO;
extern PyUFunc_f_f_As_d_d_RET PyUFunc_f_f_As_d_d PyUFunc_f_f_As_d_d_PROTO;
extern PyUFunc_d_d_RET PyUFunc_d_d PyUFunc_d_d_PROTO;
extern PyUFunc_F_F_As_D_D_RET PyUFunc_F_F_As_D_D PyUFunc_F_F_As_D_D_PROTO;
extern PyUFunc_D_D_RET PyUFunc_D_D PyUFunc_D_D_PROTO;
extern PyUFunc_O_O_RET PyUFunc_O_O PyUFunc_O_O_PROTO;
extern PyUFunc_ff_f_As_dd_d_RET PyUFunc_ff_f_As_dd_d PyUFunc_ff_f_As_dd_d_PROTO;
extern PyUFunc_dd_d_RET PyUFunc_dd_d PyUFunc_dd_d_PROTO;
extern PyUFunc_FF_F_As_DD_D_RET PyUFunc_FF_F_As_DD_D PyUFunc_FF_F_As_DD_D_PROTO;
extern PyUFunc_DD_D_RET PyUFunc_DD_D PyUFunc_DD_D_PROTO;
extern PyUFunc_OO_O_RET PyUFunc_OO_O PyUFunc_OO_O_PROTO;
extern PyUFunc_O_O_method_RET PyUFunc_O_O_method PyUFunc_O_O_method_PROTO;
extern PyArray_Map_RET PyArray_Map PyArray_Map_PROTO;

#else

#if defined(PY_UFUNC_UNIQUE_SYMBOL)
#define PyUFunc_API PY_UFUNC_UNIQUE_SYMBOL
#endif

/* C API address pointer */ 
#if defined(NO_IMPORT) || defined(NO_IMPORT_UFUNC)
extern void **PyUFunc_API;
#else
#if defined(PY_UFUNC_UNIQUE_SYMBOL)
void **PyUFunc_API;
#else
static void **PyUFunc_API;
#endif
#endif

#define PyUFunc_Check(op) \
   ((op)->ob_type == (PyTypeObject *)PyUFunc_API[PyUFunc_Type_NUM])
#define PyUfunc_Type *(PyTypeObject *)PyUFunc_API[PyUFunc_Type_NUM]

#define PyUFunc_FromFuncAndData \
  (*(PyUFunc_FromFuncAndData_RET (*)PyUFunc_FromFuncAndData_PROTO) \
   PyUFunc_API[PyUFunc_FromFuncAndData_NUM])
#define PyUFunc_GenericFunction \
  (*(PyUFunc_GenericFunction_RET (*)PyUFunc_GenericFunction_PROTO) \
   PyUFunc_API[PyUFunc_GenericFunction_NUM])
#define PyUFunc_f_f_As_d_d \
  (*(PyUFunc_f_f_As_d_d_RET (*)PyUFunc_f_f_As_d_d_PROTO) \
   PyUFunc_API[PyUFunc_f_f_As_d_d_NUM])
#define PyUFunc_d_d \
  (*(PyUFunc_d_d_RET (*)PyUFunc_d_d_PROTO) \
   PyUFunc_API[PyUFunc_d_d_NUM])
#define PyUFunc_F_F_As_D_D \
  (*(PyUFunc_F_F_As_D_D_RET (*)PyUFunc_F_F_As_D_D_PROTO) \
   PyUFunc_API[PyUFunc_F_F_As_D_D_NUM])
#define PyUFunc_D_D \
  (*(PyUFunc_D_D_RET (*)PyUFunc_D_D_PROTO) \
   PyUFunc_API[PyUFunc_D_D_NUM])
#define PyUFunc_O_O \
  (*(PyUFunc_O_O_RET (*)PyUFunc_O_O_PROTO) \
   PyUFunc_API[PyUFunc_O_O_NUM])
#define PyUFunc_ff_f_As_dd_d \
  (*(PyUFunc_ff_f_As_dd_d_RET (*)PyUFunc_ff_f_As_dd_d_PROTO) \
   PyUFunc_API[PyUFunc_ff_f_As_dd_d_NUM])
#define PyUFunc_dd_d \
  (*(PyUFunc_dd_d_RET (*)PyUFunc_dd_d_PROTO) \
   PyUFunc_API[PyUFunc_dd_d_NUM])
#define PyUFunc_FF_F_As_DD_D \
  (*(PyUFunc_FF_F_As_DD_D_RET (*)PyUFunc_FF_F_As_DD_D_PROTO) \
   PyUFunc_API[PyUFunc_FF_F_As_DD_D_NUM])
#define PyUFunc_DD_D \
  (*(PyUFunc_DD_D_RET (*)PyUFunc_DD_D_PROTO) \
   PyUFunc_API[PyUFunc_DD_D_NUM])
#define PyUFunc_OO_O \
  (*(PyUFunc_OO_O_RET (*)PyUFunc_OO_O_PROTO) \
   PyUFunc_API[PyUFunc_OO_O_NUM])
#define PyUFunc_O_O_method \
  (*(PyUFunc_O_O_method_RET (*)PyUFunc_O_O_method_PROTO) \
   PyUFunc_API[PyUFunc_O_O_method_NUM])
#define PyArray_Map \
  (*(PyArray_Map_RET (*)PyArray_Map_PROTO) \
   PyUFunc_API[PyArray_Map_NUM])

#define import_ufunc() \
{ \
  PyObject *numpy = PyImport_ImportModule("_numpy"); \
  if (numpy != NULL) { \
    PyObject *module_dict = PyModule_GetDict(numpy); \
    PyObject *c_api_object = PyDict_GetItemString(module_dict, "_UFUNC_API"); \
    if (PyCObject_Check(c_api_object)) { \
      PyUFunc_API = (void **)PyCObject_AsVoidPtr(c_api_object); \
    } \
  } \
}

#endif


#ifdef __cplusplus
}
#endif
#endif /* !Py_UFUNCOBJECT_H */
