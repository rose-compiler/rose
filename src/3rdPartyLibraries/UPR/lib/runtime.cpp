/**
 * /author Tristan Vanderbruggen (vanderbruggentristan@gmail.com)
 * /date   08/2012
 */

#include "UPR/runtime.hpp"

#include <cassert>
#include <algorithm>

namespace UPR {

/* Executor */
unsigned long Executor::id_cnt = 0;

Executor::Executor() :
  id(id_cnt++)
{}

Executor::~Executor() {}

  /* Host */
const unsigned long Host::executor_kind = 0UL;

Host::Host() :
  Executor()
{
  kind = executor_kind;
}

Host::~Host() {}

void Host::print(std::ostream & out) const {
  // TODO
}

/* Data */

Data::Data(unsigned dim_cnt_, unsigned long * dims_, unsigned data_size_) :
  dim_cnt(dim_cnt_),
  dims(0),
  data_size(data_size_),
  host_data(NULL)
{
  dims = new unsigned long[dim_cnt];
  for (unsigned i = 0; i < dim_cnt; i++) dims[i] = dims_[i];
}

Data::~Data() {
  if (dims != NULL)
    delete [] dims;

  if (host_data != NULL)
    delete [] host_data;
}

void Data::setHostData(Host * host, void * host_data_) {
  resideOn(host);
  host_data = host_data_;
}

bool Data::keep(Executor * executor) {
  Host * host = dynamic_cast<Host *>(executor);
  if (host != NULL) {
    host_data = NULL;
  }
  return false;
}

/* Task */

unsigned long Task::id_cnt = 0;

Task::Task() :
  id(id_cnt++)
{}

Task::~Task() {}

  /* ExecTask */

ExecTask::ExecTask(Executor * executor_) :
  Task(),
  executor(executor_)
{}

ExecTask::~ExecTask() {}

  /* DataTask */

DataTask::DataTask(Data * data_, Executor * from_, Executor * to_) :
  Task(),
  data(data_),
  from(from_),
  to(to_)
{}

DataTask::~DataTask() {}

/* Scheduler */

Scheduler::SchedNode::SchedNode(Task * task_, unsigned dep_cnt, Task ** deps) :
  task(task_),
  pred(deps, deps + dep_cnt),
  next()
{
  // FIXME Does 'pred' init work in any case (empty deps)
}

Scheduler::SchedNode::~SchedNode() {}

Scheduler::Scheduler() :
  p_schedule_nodes_map(),
  p_entry_point_list(),
  p_entry_point_it(p_entry_point_list.begin()),
  p_schedule_done(false),
  p_host(new Host())
{}

void Scheduler::init() {
  /// sink of this method, it was recursing up the class hierarchy
}

Host * Scheduler::getHost() const {
  return p_host;
}

void Scheduler::genScheduleNodes() {
  assert(p_schedule_done == false && p_entry_point_list.size() == 0);

  std::map<Task *, SchedNode *>::iterator it_map;
  for (it_map = p_schedule_nodes_map.begin(); it_map != p_schedule_nodes_map.end(); it_map++) {
    Task * curr_task = it_map->first;
    if (it_map->second->pred.size() == 0) {
      p_entry_point_list.push_back(curr_task);
    }
    else {
      std::vector<Task *>::iterator it_pred;
      for (it_pred = it_map->second->pred.begin(); it_pred != it_map->second->pred.end(); it_pred++) {
        Task * pred_task = *it_pred;

        std::map<Task *, SchedNode *>::iterator it_pred_sched_node = p_schedule_nodes_map.find(pred_task);
        assert(it_pred_sched_node != p_schedule_nodes_map.end());

        it_pred_sched_node->second->next.push_back(curr_task);
      }
    }
  }

  bool has_cycle = false; // TODO

  assert(has_cycle == false && p_entry_point_list.size() > 0);
}

bool Scheduler::done(Task * task) {
  assert(p_schedule_done == true);

  std::vector<Task *>::iterator it_task = std::find(p_entry_point_list.begin(), p_entry_point_list.end(), task);
  assert(it_task != p_entry_point_list.end());
  p_entry_point_list.erase(it_task);

  std::map<Task *, SchedNode *>::iterator it_map_task = p_schedule_nodes_map.find(task);
  assert(it_map_task != p_schedule_nodes_map.end());

  for (it_task = it_map_task->second->next.begin(); it_task != it_map_task->second->next.end(); it_task++) {
    std::map<Task *, SchedNode *>::iterator it_map_next = p_schedule_nodes_map.find(*it_task);
    assert(it_map_next != p_schedule_nodes_map.end());

    std::vector<Task *>::iterator it_pred = std::find(it_map_next->second->pred.begin(), it_map_next->second->pred.end(), task);
    assert(it_pred != it_map_next->second->pred.end());

    it_map_next->second->pred.erase(it_pred);

    if (it_map_next->second->pred.size() == 0)
       p_entry_point_list.push_back(it_map_next->first);
  }

  p_entry_point_it = p_entry_point_list.begin();

  return p_entry_point_list.size() == 0;
}

Task * Scheduler::next() {
  assert(p_schedule_done == true);

  if (p_entry_point_it != p_entry_point_list.end()) {
    Task * res = *p_entry_point_it;
    p_entry_point_it++;
    return res;
  }
  else return NULL;
}

Scheduler::~Scheduler() {
  // clean the graph
}

void Scheduler::add(Task * task, unsigned dep_cnt, Task ** deps) {
  assert(p_schedule_done == false && p_entry_point_list.size() == 0);

  SchedNode * sched_node = new SchedNode(task, dep_cnt, deps);
  bool success = p_schedule_nodes_map.insert(std::pair<Task *, SchedNode *>(task, sched_node)).second;
  assert(success); // if fails, it means that we tried to add a task for the second time.
}

}

