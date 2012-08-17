/**
 * /author Tristan Vanderbruggen (vanderbruggentristan@gmail.com)
 * /date   08/2012
 */

#ifndef __TEST_XOMP_RUNTIME_HPP__
#define __TEST_XOMP_RUNTIME_HPP__

#include "UPR/xomp-runtime.hpp"

namespace UPR {

class Task_1 : public Xomp_Task {
  // TODO
};

class App_Xomp_Scheduler : public Xomp_Scheduler {
  protected:
    virtual void init();

  public:
    App_Xomp_Scheduler(unsigned long n);
    virtual ~App_Xomp_Scheduler();

    static Task * build_task_1(Executor * executor, unsigned i, unsigned j, Data * a, Data * b, Data * r);
};

}

#endif /* __TEST_XOMP_RUNTIME_HPP__ */

