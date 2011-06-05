#include "sage3basic.h"
#include "rose_config.h"

#ifndef USE_ROSE_PYTHON_SUPPORT // If Python is disabled then compile a stub

#include <iostream>

int python_main(std::string, SgFile*)
{
    std::cout << "Python support not configured. Use --with-python=PATH to enable." << endl;
    return -1;
}

#else // USE_ROSE_PYTHON_SUPPORT is defined

#include <iostream>
#include <stdio.h> // TODO remove eventually, switch to iostream

#include "SagePythonInterface.h"

#include "AstDOTGeneration.h"

#define ROSE_PYTHON_FRONTEND_MODULE_NAME "sageTranslator"
#define ROSE_PYTHON_FRONTEND_TRANSLATOR_FXN_NAME "translate" 

using namespace std;

int
runPythonFrontend(SgFile* file)
{
    PyObject *pModule, *pFunc;
    PyObject *pValue, *pArgs;

    //PyRun_SimpleString("import os; print 'cwd is', os.getcwd()");
    //PyRun_SimpleString("import sys; print 'path is', sys.path");

    Py_InitModule("sage", SageBuilderMethods);

    //PyRun_SimpleString("import os; print os.getcwd()");

    pModule = PyImport_ImportModule(ROSE_PYTHON_FRONTEND_MODULE_NAME);

    if (pModule != NULL) {
        pFunc = PyObject_GetAttrString(pModule, ROSE_PYTHON_FRONTEND_TRANSLATOR_FXN_NAME);

        std::string filename = file->get_sourceFileNameWithPath();
        pValue = PyString_FromString(filename.c_str());
        pArgs = PyTuple_New(1);
        PyTuple_SetItem(pArgs, 0, pValue);

        if (pFunc && PyCallable_Check(pFunc)) {
            pValue = PyObject_CallObject(pFunc, pArgs);
            Py_DECREF(pArgs);

            if (pValue != NULL) {
                void* capsule_ptr = PyCapsule_GetPointer(pValue, NULL);
                SgGlobal* sg_global = static_cast<SgGlobal*>(capsule_ptr);
                SgSourceFile* sg_source_file = isSgSourceFile(file);
                sg_source_file->set_globalScope(sg_global);
                sg_global->set_parent(sg_source_file);
                Py_DECREF(pValue);
            }
            else {
                Py_DECREF(pFunc);
                Py_DECREF(pModule);
                PyErr_Print();
                fprintf(stderr,"Call failed\n");
                return 1;
            }
        }
        else {
            if (PyErr_Occurred())
                PyErr_Print();
            fprintf(stderr, "Cannot find function \"%s\"\n", 
                    ROSE_PYTHON_FRONTEND_TRANSLATOR_FXN_NAME);
        }
        Py_XDECREF(pFunc);
        Py_DECREF(pModule);
    }
    else {
        PyErr_Print();
        fprintf(stderr, "Failed to load module \"%s\".\n", ROSE_PYTHON_FRONTEND_MODULE_NAME);
        return 1;
    }
    return 0;
}

int python_main(std::string filename, SgFile* file)
{
    std::cout << "Launching interpreter." << std::endl;

    Py_Initialize();
    runPythonFrontend(file);
    Py_Finalize();

    std::cout << "Interpreter terminated." << std::endl;

    return -1;
}

#endif
