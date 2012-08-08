/**
 * /author Tristan Vanderbruggen (vanderbruggentristan@gmail.com)
 * /date   08/2012
 */

#include "test-cuda-runtime.hpp"

#define N 256

int main(int argc, char ** argv) {
  // Instantiate a Model Specfic Scheduler, take problem specific parameters
  UPR::Scheduler * scheduler = new UPR::App_CUDA_Scheduler(N);

  // The scheduler have detected the platform, it can provide pointers to the different executors
  UPR::Executor * cpu = scheduler->getHost();
  UPR::Executor * gpu = scheduler->getExecutor(1, 0);

  // Create an array: float[N][N]
  unsigned long dim_a[2] = {N, N};
  UPR::Data * a = scheduler->createData(2, dim_a, sizeof(float));
    a->resideOn(cpu);
    a->resideOn(gpu);

  unsigned long dim_b[2] = {N, N};
  UPR::Data * b = scheduler->createData(2, dim_b, sizeof(float));
    b->resideOn(cpu);
    b->resideOn(gpu);

  unsigned long dim_r[2] = {N, N};
  UPR::Data * r = scheduler->createData(2, dim_r, sizeof(float));
    r->resideOn(cpu);
    r->resideOn(gpu);

  // TODO init data on CPU side
  
  UPR::Task * send_a = UPR::CUDA_Scheduler::transfert(a, cpu, gpu);
  scheduler->add(send_a);

  UPR::Task * send_b = UPR::CUDA_Scheduler::transfert(b, cpu, gpu);
  scheduler->add(send_b);

  UPR::Task * run_dep[N * N];
  for (unsigned i = 0; i < N; i++) {
    for (unsigned j = 0; j < N; j++) {
      // Tasks builder are provided by the Model Specfic Scheduler, including instance specific parameter
      UPR::Task * task_1 = UPR::App_CUDA_Scheduler::build_task_1(gpu, i, j, a, b, r);

      // Once created the Tasks can be added to the scheduler
      UPR::Task * send_dep[2] = {send_a, send_b};
      scheduler->add(task_1, 2, send_dep);
      scheduler->add(task_1);

      run_dep[i * N + j] = task_1;
    }
  }

  UPR::Task * rec_r = UPR::CUDA_Scheduler::transfert(r, gpu, cpu);
  scheduler->add(rec_r, N * N, run_dep);

  // Once all the task have been instantiated, the scheduler can be launch. It will return only when all tasks have been executed
  //   the launch method instantiate all computation specific object (memory, devices, ...) and free them.
  scheduler->launch();

  // TODO use data on CPU side

  return 0;
}

