/**
 * /author Tristan Vanderbruggen (vanderbruggentristan@gmail.com)
 * /date   08/2012
 */

#ifndef __TEST_CUDA_RUNTIME_HPP__
#define __TEST_CUDA_RUNTIME_HPP__

#include "UPR/cuda-runtime.hpp"

namespace UPR {

class Task_1 : public CUDA_ExecTask {
  // TODO
};

class App_CUDA_Scheduler : public CUDA_Scheduler {
  protected:
    virtual void init();

  public:
    App_CUDA_Scheduler(unsigned long n);
    virtual ~App_CUDA_Scheduler();

    virtual void print(std::ostream&) const;

    static Task * build_task_1(Executor * executor, unsigned i, unsigned j, Data * a, Data * b, Data * r);
};

}

#endif /* __TEST_CUDA_RUNTIME_HPP__ */
