/**
 * /author Tristan Vanderbruggen (vanderbruggentristan@gmail.com)
 * /date   08/2012
 */

#ifndef __TEST_OPENCL_RUNTIME_HPP__
#define __TEST_OPENCL_RUNTIME_HPP__

#include "UPR/opencl-runtime.hpp"

namespace UPR {

/**
 * /brief An example of Application Specific Task for an OpenCL GPU
 */
class Task_1 : public OpenCL_GPU_Task {
  // TODO
};

/**
 * /brief An example of Application Specific Scheduler for OpenCL
 */
class App_OpenCL_Scheduler : public OpenCL_Scheduler {
  protected:

  protected:
    /*! \brief Provide parent class with the OpenCL source code.
     */
    virtual char * getApplicationSource();

    virtual Executor * getExecutorFor(cl_device_id device_id);

  public:
    /*! \brief is the first public constructor for this inheritance branch, at this point all abstract virtual should be implemented
     */
    App_OpenCL_Scheduler(unsigned long n);

    virtual ~App_OpenCL_Scheduler();

    /*! \brief A task builder produced by the code generation
     *  \return a valid task if executor is of the good type
     */
    static Task * build_task_1(Executor * executor, Data * a, Data * b, Data * r);

    virtual void print(std::ostream & out) const;
};

}

#endif /* __TEST_OPENCL_RUNTIME_HPP__ */
