/**
 * /author Tristan Vanderbruggen (vanderbruggentristan@gmail.com)
 * /date   08/2012
 */

#ifndef __CUDA_RUNTIME_HPP_
#define __CUDA_RUNTIME_HPP_

#include "UPR/runtime.hpp"

namespace UPR {

/*! \brief Executor for GPU in CUDA
 */
class CUDA_GPU : public Executor {
  // TODO

  protected:
    CUDA_GPU();

  public:
    virtual ~CUDA_GPU();

  public:
    static const unsigned long executor_kind;

  friend class CUDA_Scheduler;
};

/************/
/* DataTask */
/************/

class CUDA_DataTask : public DataTask {
  // TODO
};

class CUDA_Transfert_CPU_to_GPU : public CUDA_DataTask {
  // TODO
};

class CUDA_Transfert_GPU_to_CPU : public CUDA_DataTask {
  // TODO
};

/************/
/* ExecTask */
/************/

class CUDA_ExecTask : public ExecTask {
  // TODO
};

/*************/
/* Scheduler */
/*************/

class CUDA_Scheduler : public Scheduler {
  protected:
    // TODO extra attributes

  protected:
    virtual void init();

  public:
    CUDA_Scheduler();
    virtual ~CUDA_Scheduler();

    virtual Executor * getExecutor(unsigned type, unsigned id);

    virtual Data * createData(unsigned dim_cnt, unsigned long * dims, unsigned data_size);

    virtual void launch();

    static Task * transfert(Data * data, Executor * from, Executor * to);
};

}

#endif /* __CUDA_RUNTIME_HPP_ */

