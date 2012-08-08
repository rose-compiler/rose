/**
 * /author Tristan Vanderbruggen (vanderbruggentristan@gmail.com)
 * /date   08/2012
 */

#ifndef __OPENCL_RUNTIME_HPP_
#define __OPENCL_RUNTIME_HPP_

#include "UPR/runtime.hpp"

#include <OpenCL/cl.h>

#include <map>

namespace UPR {

/************/
/* Executor */
/************/

class OpenCL_GPU : public Executor {
  // TODO

  protected:
    OpenCL_GPU();

  public:
    virtual ~OpenCL_GPU();

    virtual void print(std::ostream & out) const;

  public:
    static const unsigned long executor_kind;

  friend class OpenCL_Scheduler;
};

class OpenCL_CPU : public Executor {
  // TODO

  protected:
    OpenCL_CPU();

  public:
    virtual ~OpenCL_CPU();

    virtual void print(std::ostream & out) const;

  public:
    static const unsigned long executor_kind;

  friend class OpenCL_Scheduler;
};

/********/
/* Data */
/********/

class OpenCL_Data : public Data {
  // TODO

  protected:
    OpenCL_Data(unsigned dim_cnt_, unsigned long * dims_, unsigned data_size_);

  public:
    virtual ~OpenCL_Data();

    /*! \brief Model level implementation
     */
    virtual void resideOn(Executor * executor);

    /*! \brief Model level implementation
     */
    virtual void * getDataOn(Executor * executor) const;

    /*! \brief Model level implementation
     */
    virtual bool keep(Executor * executor);

    /*! \brief Model level implementation
     */
    virtual void print(std::ostream & out) const;

  friend class OpenCL_Scheduler;
};

/************/
/* DataTask */
/************/

class OpenCL_DataTask : public DataTask {
  // TODO

  protected:
    OpenCL_DataTask(Data * data_, Executor * from_, Executor * to_);

  public:
    virtual ~OpenCL_DataTask();

    /*! \brief Model level implementation
     */
    virtual void launch();

    /*! \brief Model level implementation
     */
    virtual bool isTerminated(bool wait = true);

  friend class OpenCL_Scheduler;
};

class OpenCL_Transfert_Host_to_Acc : public OpenCL_DataTask {
  // TODO

  protected:
    OpenCL_Transfert_Host_to_Acc(Data * data_, Executor * from_, Executor * to_);

  public:
    virtual ~OpenCL_Transfert_Host_to_Acc();

    /*! \brief Model level implementation
     */
    virtual void print(std::ostream & out) const;

  friend class OpenCL_Scheduler;
};

class OpenCL_Transfert_Acc_to_Host : public OpenCL_DataTask {
  // TODO

  protected:
    OpenCL_Transfert_Acc_to_Host(Data * data_, Executor * from_, Executor * to_);

  public:
    virtual ~OpenCL_Transfert_Acc_to_Host();

    /*! \brief Model level implementation
     */
    virtual void print(std::ostream & out) const;

  friend class OpenCL_Scheduler;
};

/************/
/* ExecTask */
/************/

class OpenCL_ExecTask : public ExecTask {
  // TODO
};

class OpenCL_GPU_Task : public OpenCL_ExecTask {
  // TODO
};

class OpenCL_CPU_Task : public OpenCL_ExecTask {
  // TODO
};

/*************/
/* Scheduler */
/*************/

class OpenCL_Scheduler : public Scheduler {
  protected:
    cl_platform_id ocl_platform;
    cl_context ocl_context;
    cl_program ocl_program;
    std::map<unsigned long, cl_command_queue> ocl_queues;
    std::map<unsigned long, cl_device_id> ocl_devices;
    std::map<unsigned long, Executor *> executors;

  protected:
    /*! \brief Method to implement OpenCL source code retrieval in the ASR layer (enable application generated OpenCL code)
     *  \return OpenCL source code in a C string, memory will be free by method user (in constructor of this class after it build ocl_program)
     */
    virtual char * getApplicationSource() = 0;

    /*! \brief Provide parent class with an Executor corresponding to the given cl_device_id
      *  \param device_id an OpenCL device ID
      *  \return NULL if the cl_device_id is not support by the implementation of the application,
      *          ow it returns a valid pointer to a valid (and unique) Executor instance
      */
    virtual Executor * getExecutorFor(cl_device_id device_id) = 0;

    /*! \brief contains all the OpenCL initialisation
     */
    virtual void init();

  public:
    OpenCL_Scheduler();
    virtual ~OpenCL_Scheduler();

    /*! \brief MSR layer implementation of Executor Accessors 
     */
    virtual Executor * getExecutor(unsigned long id) const;

    /*! \brief MSR layer implementation of the data builder
    */
    virtual Data * createData(unsigned dim_cnt, unsigned long * dims, unsigned data_size);

    /*! \brief MSR layer implementation of the launcher
     */
    virtual void launch();

    /*! \brief A task builder provided by the model: OpenCL data transfert
     *  \param data the data (in the virtual sens) that need to be move
     *  \param from sender executor
     *  \param to   receiver executor
     *  \return a valid data transfert task, if param are valid and coherent
     */
    static Task * transfert(Data * data, Executor * from, Executor * to);

  protected:
    /*! \brief Utility function for file reading
     *  \param filename the name of the file to read
     *  \return a pointer to a c_str
     */
    static char * readFromFile(const char * filename);

    /*! \brief Utility function for Executor generation
     */
    static Executor * genDefaultExecutorFor(cl_device_id device_id);
};

}

#endif /* __OPENCL_RUNTIME_HPP_ */

