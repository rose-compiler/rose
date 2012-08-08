/**
 * /author Tristan Vanderbruggen (vanderbruggentristan@gmail.com)
 * /date   08/2012
 */

#include "test-xomp-runtime.hpp"

#define N 256

int main(int argc, char ** argv) {
  // Instantiate a Model Specfic Scheduler, take problem specific parameters
  UPR::Scheduler * scheduler = new UPR::App_Xomp_Scheduler(N);

  // Create arrays: float[N][N]
  // Xomp => shared memory => no need data reside on all executor already
  unsigned long dim_a[2] = {N, N};
  UPR::Data * a = scheduler->createData(2, dim_a, sizeof(float));

  unsigned long dim_b[2] = {N, N};
  UPR::Data * b = scheduler->createData(2, dim_b, sizeof(float));

  unsigned long dim_r[2] = {N, N};
  UPR::Data * r = scheduler->createData(2, dim_r, sizeof(float));

  // TODO init data
  
  for (unsigned i = 0; i < N; i++) {
    for (unsigned j = 0; j < N; j++) {
      // Tasks builder are provided by the Model Specfic Scheduler, including instance specific parameter
      UPR::Task * task_1 = UPR::App_Xomp_Scheduler::build_task_1(i, j, a, b, r);

      // Once created the Tasks can be added to the scheduler
      scheduler->add(task_1);
    }
  }

  // Once all the task have been instantiated, the scheduler can be launch. It will return only when all tasks have been executed
  //   the launch method instantiate all computation specific object (memory, devices, ...) and free them.
  scheduler->launch();

  // TODO use data

  return 0;
}

