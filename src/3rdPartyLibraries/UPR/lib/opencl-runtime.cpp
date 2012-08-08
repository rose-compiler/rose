/**
 * /author Tristan Vanderbruggen (vanderbruggentristan@gmail.com)
 * /date   08/2012
 */

#include "UPR/opencl-runtime.hpp"

#include <cassert>

#include <iostream>
#include <fstream>

namespace UPR {

/*************/
/* Executors */
/*************/

  /* GPU */

const unsigned long OpenCL_GPU::executor_kind = 1UL;

OpenCL_GPU::OpenCL_GPU() :
  Executor()
{
  kind = OpenCL_GPU::executor_kind;
}

OpenCL_GPU::~OpenCL_GPU() {}

void OpenCL_GPU::print(std::ostream & out) const {
  // TODO
}

  /* CPU */

const unsigned long OpenCL_CPU::executor_kind = 2UL;

OpenCL_CPU::OpenCL_CPU() :
  Executor()
{
  kind = OpenCL_CPU::executor_kind;
}

OpenCL_CPU::~OpenCL_CPU() {}

void OpenCL_CPU::print(std::ostream & out) const {
  // TODO
}

/********/
/* Data */
/********/

OpenCL_Data::OpenCL_Data(unsigned dim_cnt_, unsigned long * dims_, unsigned data_size_) :
  Data(dim_cnt_, dims_, data_size_)
{}

OpenCL_Data::~OpenCL_Data() {}

void OpenCL_Data::resideOn(Executor * executor) {
  // TODO
}

void * OpenCL_Data::getDataOn(Executor * executor) const {
  return NULL;
}

bool OpenCL_Data::keep(Executor * executor) {
  bool res = Data::keep(executor);
  if (!res) {
    // TODO
  }
  return res;
}

void OpenCL_Data::print(std::ostream & out) const {
  // TODO
}

/********/
/* Task */
/********/

  /************/
  /* DataTask */
  /************/

    /**/

OpenCL_DataTask::OpenCL_DataTask(Data * data_, Executor * from_, Executor * to_) :
  DataTask(data_, from_, to_)
{}

OpenCL_DataTask::~OpenCL_DataTask() {}

void OpenCL_DataTask::launch() {
  // TODO
}

bool OpenCL_DataTask::isTerminated(bool wait) {
  // TODO
  return true;
}

    /**/

OpenCL_Transfert_Host_to_Acc::OpenCL_Transfert_Host_to_Acc(Data * data_, Executor * from_, Executor * to_) :
  OpenCL_DataTask(data_, from_, to_)
{}

OpenCL_Transfert_Host_to_Acc::~OpenCL_Transfert_Host_to_Acc() {}

void OpenCL_Transfert_Host_to_Acc::print(std::ostream & out) const {
  // TODO
}

    /**/

OpenCL_Transfert_Acc_to_Host::OpenCL_Transfert_Acc_to_Host(Data * data_, Executor * from_, Executor * to_) :
  OpenCL_DataTask(data_, from_, to_)
{}

OpenCL_Transfert_Acc_to_Host::~OpenCL_Transfert_Acc_to_Host() {}

void OpenCL_Transfert_Acc_to_Host::print(std::ostream & out) const {
  // TODO
}

/*************/
/* Scheduler */
/*************/

OpenCL_Scheduler::OpenCL_Scheduler() :
  Scheduler(),
  ocl_platform(),
  ocl_context(),
  ocl_program(),
  ocl_queues(),
  ocl_devices(),
  executors()
{}

void OpenCL_Scheduler::init() {
  Scheduler::init();

  cl_int status;

  // Get the platforms
  {
    cl_uint nbr_platform;

    status = clGetPlatformIDs(0, NULL, &nbr_platform);
    assert(status == CL_SUCCESS); // TODO display status if error

    assert(nbr_platform == 1); // TODO clean failure/choice

    status = clGetPlatformIDs(1, &ocl_platform, NULL);
    assert(status == CL_SUCCESS); // TODO display status if error
  }

  // Get the devices
  cl_uint nbr_device;
  cl_device_id * devices = NULL;
  {

    status = clGetDeviceIDs(ocl_platform, CL_DEVICE_TYPE_ALL, 0, NULL, &nbr_device);
    assert(status == CL_SUCCESS); // TODO display status if error

    assert(nbr_device > 0); // TODO clean error

  //assert(nbr_device == 1); // FIXME tmp

    devices = new cl_device_id[nbr_device];
    assert(devices != NULL);

    status = clGetDeviceIDs(ocl_platform, CL_DEVICE_TYPE_ALL, nbr_device, devices, NULL);
    assert(status == CL_SUCCESS); // TODO display status if error
  }

  // Create the context
  ocl_context = clCreateContext(NULL, nbr_device, devices, NULL, NULL, &status);
  assert(status == CL_SUCCESS); // TODO display status if error

  // Create the program
  {
    char * source = getApplicationSource(); // call to Application layer
    assert(source != NULL);

    ocl_program = clCreateProgramWithSource(ocl_context, 1, (const char**)&source, NULL, &status);
    assert(status == CL_SUCCESS); // TODO display status if error

    status = clBuildProgram(ocl_program, 0, NULL, NULL, NULL, NULL);
    assert(status == CL_SUCCESS); // TODO display build status if error
  }
  
  // Create one queue for each device
  for (cl_uint i = 0; i < nbr_device; i++) {
    // Executor
    Executor * executor = getExecutorFor(devices[i]);
    assert(executor != NULL);
    executors.insert(std::pair<unsigned long, Executor *>(executor->id, executor));

    // Device
    ocl_devices.insert(std::pair<unsigned long, cl_device_id>(executor->id, devices[i]));

    // Queue
    cl_command_queue queue = clCreateCommandQueue(ocl_context, devices[i], 0, &status);
    assert(status == CL_SUCCESS); // TODO display build status if error
    ocl_queues.insert(std::pair<unsigned long, cl_command_queue>(executor->id, queue));
  }
}

OpenCL_Scheduler::~OpenCL_Scheduler() {
  // cleanning
}

Executor * OpenCL_Scheduler::getExecutor(unsigned long id) const {
  std::map<unsigned long, Executor *>::const_iterator it = executors.find(id);
  if (it == executors.end()) return NULL;
  else return it->second;
}

Data * OpenCL_Scheduler::createData(unsigned dim_cnt, unsigned long * dims, unsigned data_size) {
  return new OpenCL_Data(dim_cnt, dims, data_size);
}

void OpenCL_Scheduler::launch() {
  // TODO
}

Task * OpenCL_Scheduler::transfert(Data * data, Executor * from, Executor * to) {
  Host * host_from = dynamic_cast<Host *>(from);
  Host * host_to   = dynamic_cast<Host *>(to);

  assert(host_from != NULL xor host_to != NULL);

  if (host_from != NULL) {
//  TODO
//  OpenCL_GPU * gpu_to = dynamic_cast<OpenCL_GPU *>(to);
//  assert(gpu_to != NULL);
    return new OpenCL_Transfert_Host_to_Acc(data, from, to); 
  }

  if (host_to != NULL) {
//  TODO
//  OpenCL_GPU * gpu_from = dynamic_cast<OpenCL_GPU *>(from);
//  assert(gpu_from != NULL);
    return new OpenCL_Transfert_Acc_to_Host(data, from, to);
  }

  assert(false);

  return NULL;
}

char * OpenCL_Scheduler::readFromFile(const char * filename) {
  std::ifstream source_file(filename, std::ifstream::in);

 // if (source_file.is_open()) return NULL;

  char * res = NULL;
  long length = 0;

  // get length of file:
  source_file.seekg(0, std::ios::end);
  length = source_file.tellg();
  source_file.seekg(0, std::ios::beg);

  // allocate memory:
  res = new char [length];

  // read data as a block:
  source_file.read(res, length);

  source_file.close();

  return res;
}

Executor * OpenCL_Scheduler::genDefaultExecutorFor(cl_device_id device_id) {
  cl_int status;

  size_t actual_size;
  cl_device_type device_type;

  status = clGetDeviceInfo(device_id, CL_DEVICE_TYPE, sizeof(cl_device_type), &device_type, &actual_size);
  assert(status == CL_SUCCESS);

  Executor * res = NULL;

  switch (device_type) {
    case CL_DEVICE_TYPE_CPU:
      res = new OpenCL_CPU();
      break;
    case CL_DEVICE_TYPE_GPU:
      res = new OpenCL_GPU();
      break;
    default:;
  }

  assert(res != NULL);

  return res;
}

}

