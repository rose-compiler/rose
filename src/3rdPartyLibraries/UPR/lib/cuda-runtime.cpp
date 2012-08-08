/**
 * /author Tristan Vanderbruggen (vanderbruggentristan@gmail.com)
 * /date   08/2012
 */

#include "UPR/cuda-runtime.hpp"

namespace UPR {

/*************/
/* Executors */
/*************/

  /* GPU */

const unsigned long CUDA_GPU::executor_kind = 1UL;

CUDA_GPU::CUDA_GPU() :
  Executor()
{
  kind = CUDA_GPU::executor_kind;
}

CUDA_GPU::~CUDA_GPU() {}

/*************/
/* Scheduler */
/*************/

void CUDA_Scheduler::init() {
  Scheduler::init();

  // TODO
}

CUDA_Scheduler::CUDA_Scheduler() :
  Scheduler() //, TODO extra attributes
{
  // TODO
}

CUDA_Scheduler::~CUDA_Scheduler() {
  // cleanning
}

Executor * CUDA_Scheduler::getExecutor(unsigned type, unsigned id) {
  // TODO
  return NULL;
}

Data * CUDA_Scheduler::createData(unsigned dim_cnt, unsigned long * dims, unsigned data_size) {
  // TODO
  return NULL;
}

void CUDA_Scheduler::launch() {
  // TODO
}

Task * CUDA_Scheduler::transfert(Data * data, Executor * from, Executor * to) {
  // TODO
  return NULL;
}

}

