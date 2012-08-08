/**
 * /author Tristan Vanderbruggen (vanderbruggentristan@gmail.com)
 * /date   08/2012
 */

#include "UPR/xomp-runtime.hpp"

namespace UPR {

/*************/
/* Executors */
/*************/

  /* CPU */

const unsigned long Xomp_CPU::executor_kind = 1UL;

Xomp_CPU::Xomp_CPU() :
  Executor()
{
  kind = Xomp_CPU::executor_kind;
}

Xomp_CPU::~Xomp_CPU() {}

/********/
/* Task */
/********/

Xomp_Task::Xomp_Task(Executor * executor_) :
  ExecTask(executor_)
{}

Xomp_Task::~Xomp_Task() {}

/*************/
/* Scheduler */
/*************/

void Xomp_Scheduler::init() {
  Scheduler::init();

  // TODO
}

Xomp_Scheduler::Xomp_Scheduler() :
  Scheduler() //, TODO extra attributes
{
  // TODO
}

Xomp_Scheduler::~Xomp_Scheduler() {
  // cleanning
}

Executor * Xomp_Scheduler::getExecutor(unsigned type, unsigned id) {
  // TODO
  return NULL;
}

Data * Xomp_Scheduler::createData(unsigned dim_cnt, unsigned long * dims, unsigned data_size) {
  // TODO
  return NULL;
}

void Xomp_Scheduler::launch() {
  // TODO
}

}

