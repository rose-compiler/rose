/**
 * /author Tristan Vanderbruggen (vanderbruggentristan@gmail.com)
 * /date   08/2012
 */

#include "test-opencl-runtime.hpp"

#include <cassert>

#define N 256

float * matmul(unsigned int n, float * a_, float * b_) {

  std::cout << "[1] - Instantiate the application scheduler" << std::endl;

    // Instantiate a Model Specfic Scheduler, take problem specific parameters
    UPR::Scheduler * scheduler = new UPR::App_OpenCL_Scheduler(n);
  
  std::cout << "[2] - Retrieve the executors" << std::endl;

    // The scheduler have detected the platform, it can provide pointers to the different executors
    UPR::Host     * host = scheduler->getHost();
    UPR::Executor *  gpu = scheduler->getExecutor(0);
  
  std::cout << "[3] - Declare the data" << std::endl;

    // Create arrays: float[n][n]
    unsigned long dim[2] = {n, n};
  
    std::cout << "[3.1] - Array a" << std::endl;

      UPR::Data * a = scheduler->createData(2, dim, sizeof(float));
        a->resideOn(host);
        a->resideOn(gpu);
  
    std::cout << "[3.2] - Array b" << std::endl;

      UPR::Data * b = scheduler->createData(2, dim, sizeof(float));
        b->resideOn(host);
        b->resideOn(gpu);
  
    std::cout << "[3.3] - Array r" << std::endl;

      UPR::Data * r = scheduler->createData(2, dim, sizeof(float));
        r->resideOn(host);
        r->resideOn(gpu);
  
  std::cout << "[4] - Initialize the Data on host side" << std::endl;

    a->setHostData(host, (void *)a_);
    b->setHostData(host, (void *)b_);
  
  std::cout << "[5] - Initial communications" << std::endl;
  
    std::cout << "[5.1] - Send a from host to gpu" << std::endl;
  
      UPR::Task * send_a = UPR::OpenCL_Scheduler::transfert(a, host, gpu);
      scheduler->add(send_a);
  
    std::cout << "[5.2] - Send b from host to gpu" << std::endl;

      UPR::Task * send_b = UPR::OpenCL_Scheduler::transfert(b, host, gpu);
      scheduler->add(send_b);
  
  std::cout << "[6] - Main computation task" << std::endl;

    std::cout << "[6.1] - Build an Application Specific Coputation Task" << std::endl;

      // Tasks builder are provided by the Model Specfic Scheduler, including instance specific parameter
      UPR::Task * task_1 = UPR::App_OpenCL_Scheduler::build_task_1(gpu, a, b, r);
  
    std::cout << "[6.2] - Register the task on the scheduler include dependence on data transfert" << std::endl;

      // Once created the Tasks can be added to the scheduler
      UPR::Task * send_dep[2] = {send_a, send_b};
      scheduler->add(task_1, 2, send_dep);

  std::cout << "[7] - Final communication" << std::endl;

    UPR::Task * rec_r = UPR::OpenCL_Scheduler::transfert(r, gpu, host);
    scheduler->add(rec_r, 1, &task_1);
  
  std::cout << "[8] - Execute the tasks" << std::endl;

    // Once all the task have been instantiated, the scheduler can be launch. It will return only when all tasks have been executed
    //   the launch method instantiate all computation specific object (memory, devices, ...) and free them.
    scheduler->launch();
  
  std::cout << "[9] - Back to the normal application (commit results and clean)" << std::endl;

    // a was provided
    a->keep(host);
    delete a;

    // b was provided
    b->keep(host);
    delete b;

    // save r as it is the result
    float * r_ = (float *)(r->getDataOn(host));
    assert(r_ != NULL);
  
    // Need to preserve r
    r->keep(host);
    delete r;

    // delete the scheduler, it calls Model Specific cleaning functions
    delete scheduler;

  return r_;
}

int main() {
  float a[N][N];
  float b[N][N];

  float * res = matmul(N, &(a[0][0]), &(b[0][0]));

  assert(res != NULL);
}

