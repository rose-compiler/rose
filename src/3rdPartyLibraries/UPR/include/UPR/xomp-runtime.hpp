/**
 * /author Tristan Vanderbruggen (vanderbruggentristan@gmail.com)
 * /date   08/2012
 */

#ifndef __XOMP_RUNTIME_HPP_
#define __XOMP_RUNTIME_HPP_

#include "UPR/runtime.hpp"

namespace UPR {

class Xomp_CPU : public Executor {
  // TODO

  protected:
    Xomp_CPU();

  public:
    virtual ~Xomp_CPU();

  public:
    static const unsigned long executor_kind;

  friend class Xomp_Scheduler;
};

/*! \brief Only one type of task for Xomp: computation on CPU (Xomp target multi-thread and shared memory)
 */
class Xomp_Task : public ExecTask {
  protected:
    Xomp_Task(Executor * executor_);

  public:
    ~Xomp_Task();

  friend class Xomp_Scheduler;
};

class Xomp_Scheduler : public Scheduler {
  protected:
    // TODO extra attributes

  protected:
    virtual void init();

  public:
    Xomp_Scheduler();
    virtual ~Xomp_Scheduler();

    virtual Executor * getExecutor(unsigned type, unsigned id);

    virtual Data * createData(unsigned dim_cnt, unsigned long * dims, unsigned data_size);

    virtual void launch();
};

}

#endif /* __XOMP_RUNTIME_HPP_ */

