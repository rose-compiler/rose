/**
 * /author Tristan Vanderbruggen (vanderbruggentristan@gmail.com)
 * /date   08/2012
 */

#include "test-opencl-runtime.hpp"

namespace UPR {

App_OpenCL_Scheduler::App_OpenCL_Scheduler(unsigned long n) :
  OpenCL_Scheduler()
{
  OpenCL_Scheduler::init();
}

App_OpenCL_Scheduler::~App_OpenCL_Scheduler() {
  // cleaning
}

char * App_OpenCL_Scheduler::getApplicationSource() {
  return OpenCL_Scheduler::readFromFile("test.cl");
}

Executor * App_OpenCL_Scheduler::getExecutorFor(cl_device_id device_id) {
  return OpenCL_Scheduler::genDefaultExecutorFor(device_id);
}

Task * App_OpenCL_Scheduler::build_task_1(Executor * executor, Data * a, Data * b, Data * r) {
  // TODO
  return NULL;
}

void App_OpenCL_Scheduler::print(std::ostream & out) const {
  // TODO
}

}
