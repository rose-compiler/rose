
#include <iostream>

#include <vector>
#include <map>

namespace UPR {

/*! \brief Base class for the notion of executor (CPU, GPU, other acc)
 */
class Executor {
  protected:
    unsigned long kind;

  protected:
    Executor();

  public:
    virtual ~Executor();

    /*! \brief Printer
     */
    virtual void print(std::ostream & out) const = 0;

  public:
    const unsigned long id;

  static unsigned long id_cnt;

  friend class Scheduler;
};

/*! \brief represents the notion of Host, it is the thread 0 or the host processor in a CPU/GPU system.
 *         It is here for sequential task and contains the original data
 */
class Host : public Executor {
  protected:
    Host();

  public:
    virtual ~Host();

    virtual void print(std::ostream & out) const;

  public:
    static const unsigned long executor_kind;

  friend class Scheduler;
};

/*! \brief Represent some data (as multi-dimensional array) but not the allocation of the data
 */
class Data {
  protected:
    unsigned dim_cnt; /// Dimension of this data (array dimension), if (dim_cnt == 0) then it is a scalar
    unsigned long * dims; /// Array storing the size of each dimension, if (dim_cnt > 0) then typeof(dims) = unsigned long[dim_cnt] else dims = NULL
    unsigned data_size;

    void * host_data;

  protected:
    Data(unsigned dim_cnt_, unsigned long * dims_, unsigned data_size_);

  public:
    virtual ~Data();

    /*! \brief requires the datas to be allocated on an Executor, provided at the model level
     *  \param executor the executor which need an allocation of this Data
     */
    virtual void resideOn(Executor * executor) = 0;

    /*! \return a pointer to the allocation on this device if exist, NULL otherwise
     */
    virtual void * getDataOn(Executor * executor) const = 0;

    /*! /brief specify data pointer for the Host
     *  /param host the application Host
     *  /param host_data_ is an 1D-array of size = data_size * prod_{0 <= i < dim_cnt}{dims[i]}
     *  /post imply a call to resideOn(host)
     */
    void setHostData(Host * host, void * host_data_);

    /*! /brief specify that the version of this data hosted by an executor need to survive the destructor
     *  /param executor for which we keep the data
     */
    virtual bool keep(Executor * executor);

    /*! \brief Printer
     */
    virtual void print(std::ostream & out) const = 0;

  friend class Scheduler;
};

/*! \brief Represent the notion of task, for the scheduler the task is the smallest unit of scheduling
 */
class Task {
  protected:
    unsigned long id; /// Unique task id of this task

    static unsigned long id_cnt; /// Unique task id counter

  protected:
    Task();

  public:
    virtual ~Task();

    /*! \brief start the execution of the task
     */
    virtual void launch() = 0;

    /*! \brief Check if a task is terminated
     *  \param wait if true this method will only return once the task is complete
     *  \return true if the task is complete
     */
    virtual bool isTerminated(bool wait = true) = 0;

    /*! \brief Printer
     */
    virtual void print(std::ostream & out) const = 0;

  friend class Scheduler;
};

/*! \brief Represent computation task using an executor
 */
class ExecTask : public Task {
  protected:
    Executor * executor; /// Executor to which this task as been assigned

  protected:
    ExecTask(Executor * executor_);

  public:
    virtual ~ExecTask();

    /*! \brief Printer
     */
    virtual void print(std::ostream & out) const = 0;

  friend class Scheduler;
};

/*! \brief Represent dtata transfert task which use a link between two executor.
 */
class DataTask : public Task {
  protected:
    Data * data;     /// Data to be transfered
    Executor * from; /// Sender
    Executor * to;   /// Receiver

  protected:
    DataTask(Data * data_, Executor * from_, Executor * to_);

  public:
    virtual ~DataTask();

    /*! \brief Printer
     */
    virtual void print(std::ostream & out) const = 0;

  friend class Scheduler;
};

/*! \brief Main component of the Unified Runtime, it provides basic function that need to be expand by model and application specific implementations.
 */
class Scheduler {
  protected:
    /*! \brief Internal class for scheduling graph representation
     */
    struct SchedNode {
      SchedNode(Task * task_, unsigned dep_cnt = 0, Task ** deps = 0);
      ~SchedNode();

      Task * task; /// The task represented by this node of the graph

      std::vector<Task *> pred; /// tasks preceding the current one in execution order, provided at the time of the registrationof the task
      std::vector<Task *> next; /// tasks to be executed after the current task. computed after registration of all tasks
    };

    std::map<Task *, SchedNode *> p_schedule_nodes_map; /// mapping from any task to the associated scheduling graph node
    std::vector<Task *> p_entry_point_list;             /// set of tasks that not depending on any other task

    std::vector<Task *>::iterator p_entry_point_it; /// iterator used by the method 'next()' 

    bool p_schedule_done; /// true if the field 'next' have been compute for every node in the scheduling graph

    Host * p_host;

  protected:
    Scheduler();

    /*! \brief Fill the 'next' field of 'SchedNode'
     *  \pre 'p_schedule_done == false && p_entry_point_list.size() == 0'
     *  \post 'has_cycle == false && p_entry_point_list.size() > 0'
     */
    void genScheduleNodes();

    /*! \brief Mark a task as done and update 'p_entry_point_list'
     *  \pre 'p_schedule_done == true'
     *  \post No more reference to 'task' in any of the 'pred' list, 'p_entry_point_it == p_entry_point_list.begin()'
     *  \param task the finish Tast *
     *  \return true when all task have been executed
     */
    bool done(Task * task);

    /*! \brief Return the next task to exec
     *  \pre 'p_schedule_done == true'
     *  \post p_entry_point_it == p_entry_point_it + 1
     *  \return 'p_entry_point_it != p_entry_point_list.end() ? *p_entry_point_it : NULL'
     */
    virtual Task * next();

    /*! \brief initialize the Scheduler
     */
    virtual void init();

  public:
    virtual ~Scheduler();

    /*! \return the Host of the current system. It represents the system instantiating the scheduler (not a physical executor)
     */
    Host * getHost() const;

    /*! \brief Retrieve an executor by id
     *  \param id of the executor (id = 0 is the Host)
     *  \return a pointer to an Executor instance or NULL
     */
    virtual Executor * getExecutor(unsigned long id) const = 0;

    /*! \brief Data factory, provided at the model level.
     *  \param dim_cnt number of dimension of the data block
     *  \param dims size of each dimension
     *  \param 
     *  \return 
     */
    virtual Data * createData(unsigned dim_cnt, unsigned long * dims, unsigned data_size) = 0;

    /*! \brief add a Task to the computation
     *  \pre 'p_schedule_done == false && p_entry_point_list.size() == 0'
     *  \post 'task' \\in 'p_schedule_nodes_map'
     *  \param task the Task to be added
     *  \param deps an array of Task * representing the dependences/predecessors of task
     *  \param dep_cnt number of element in deps
     */
    virtual void add(Task * task, unsigned dep_cnt = 0, Task ** deps = 0);

    /*! \brief Launch all the tasks in a consistent order, return only when all task have been executed
     */
    virtual void launch() = 0;

    /*! \brief Printer
     */
    virtual void print(std::ostream & out) const = 0;
};

}

