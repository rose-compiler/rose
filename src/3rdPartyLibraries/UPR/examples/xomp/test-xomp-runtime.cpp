/**
 * /author Tristan Vanderbruggen (vanderbruggentristan@gmail.com)
 * /date   08/2012
 */

#include "test-xomp-runtime.hpp"

namespace UPR {

void App_Xomp_Scheduler::init() {
  Xomp_Scheduler::init();

  // TODO
}

App_Xomp_Scheduler::App_Xomp_Scheduler(unsigned long n) :
  Xomp_Scheduler() //, TODO extra attributes
{
  // TODO
}

App_Xomp_Scheduler::~App_Xomp_Scheduler() {
  // cleanning
}

Task * App_Xomp_Scheduler::build_task_1(Executor * executor, unsigned i, unsigned j, Data * a, Data * b, Data * r) {
  // TODO
  return NULL;
}

}

