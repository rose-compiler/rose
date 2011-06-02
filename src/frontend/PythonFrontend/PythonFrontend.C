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

#include <Python.h>

int python_main(std::string, SgFile*)
{
    std::cout << "python_main was called. Launching interpreter." << std::endl;

    Py_Initialize();
    PyRun_SimpleString("print 'Hello world!'");
    Py_Finalize();

    std::cout << "Interpreter terminated." << std::endl;

    return -1;
}

#endif
