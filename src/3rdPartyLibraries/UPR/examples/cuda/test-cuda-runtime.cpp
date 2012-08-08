/**
 * /author Tristan Vanderbruggen (vanderbruggentristan@gmail.com)
 * /date   08/2012
 */

#include "test-cuda-runtime.hpp"

namespace UPR {

void App_CUDA_Scheduler::init() {
  CUDA_Scheduler::init();

  // TODO
}

App_CUDA_Scheduler::App_CUDA_Scheduler(unsigned long n) :
  CUDA_Scheduler() //, TODO extra attributes
{
  // TODO
}

App_CUDA_Scheduler::~App_CUDA_Scheduler() {
  // cleanning
}

void App_CUDA_Scheduler::print(std::ostream&) const {
  // TODO
}

Task * App_CUDA_Scheduler::build_task_1(Executor * executor, unsigned i, unsigned j, Data * a, Data * b, Data * r) {
  // TODO
  return NULL;
}

}
