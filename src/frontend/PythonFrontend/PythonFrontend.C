#include <iostream>

#include "sage3basic.h"
#include "rose_config.h"

#include "SagePythonInterface.h"

#define ROSE_PYTHON_FRONTEND_MODULE_NAME "sageTranslator"
#define ROSE_PYTHON_FRONTEND_TRANSLATOR_FXN_NAME "translate"

using namespace std;

SgGlobal*
runPythonFrontend(SgFile* file)
{
    PyObject *pModule, *pFunc;
    PyObject *pValue, *pArgs;

    Py_InitModule("sage", SageBuilderMethods);

    // Hack to set sys.path (the list of paths that Python uses to look up modules)
    // so that the subsequent import succeeds. The program first looks in the
    // installation directory, then the source directory. (Automake's support for
    // Python does not copy .py (or .pyo, .pyc) into the compile tree).
    // TODO: find a better way to run/distribute the code in sageTranslator.py
    stringstream cmd;
    cmd << "import sys" << endl;
    cmd << "sys.path.insert(0, '" << ROSE_INSTALLATION_PATH << "/lib/python')" << endl;
    cmd << "sys.path.insert(0, '" << ROSE_AUTOMAKE_TOP_SRCDIR << "/src/frontend/PythonFrontend')" << endl;
    PyRun_SimpleString(cmd.str().c_str());
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
                Py_DECREF(pValue);
                return sg_global;
            }
            else {
                Py_DECREF(pFunc);
                Py_DECREF(pModule);
                PyErr_Print();
                ROSE_ABORT("Python frontend failed.");
                return NULL;
            }
        }
        else {
            if (PyErr_Occurred())
                PyErr_Print();
            cerr << "Cannot find function " <<
                    ROSE_PYTHON_FRONTEND_TRANSLATOR_FXN_NAME << endl;
        }
        Py_XDECREF(pFunc);
        Py_DECREF(pModule);
    }
    else {
        PyErr_Print();
        cerr << "Failed to load module " << ROSE_PYTHON_FRONTEND_MODULE_NAME << endl;
        return NULL;
    }
    return NULL;
}

int python_main(std::string filename, SgFile* file)
{
    //std::cout << "Launching interpreter." << std::endl;

    Py_Initialize();
    SgGlobal* sg_global = runPythonFrontend(file);
    Py_Finalize();

    //const char* str = (sg_global != NULL) ? "success" : "failed";
    //std::cout << "Interpreter terminated (" << str << ")." << std::endl;

    SgSourceFile* sg_source_file = isSgSourceFile(file);
    sg_source_file->set_globalScope(sg_global);
    sg_global->set_parent(sg_source_file);
    file->set_skip_commentsAndDirectives(true);

    return -1;
}
