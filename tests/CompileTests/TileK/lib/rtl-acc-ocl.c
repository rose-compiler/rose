
#include "RTL/Host/kernel.h"
#include "KLT/RTL/loop.h"
#include "KLT/RTL/tile.h"
#include "KLT/RTL/context.h"

#include <CL/opencl.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <assert.h>

#ifndef COMPILE_OPENCL_KERNEL_WITH_DEBUG
#define COMPILE_OPENCL_KERNEL_WITH_DEBUG 0
#endif

extern char * opencl_kernel_file;
extern char * opencl_kernel_options;
extern char * opencl_klt_runtime_lib;

char * read_file(const char * filename);
void dbg_get_ocl_build_log(cl_device_id device, cl_program program);
const char * ocl_status_to_char(cl_int status);

void launch(struct kernel_t * kernel, struct klt_loop_context_t * klt_loop_context) {
  int i, j;
  cl_int err;

  // Platform & Device

  cl_platform_id platform;
  err = clGetPlatformIDs(1, &platform, NULL);
  assert(err == CL_SUCCESS);

  cl_device_id device;
  err = clGetDeviceIDs(platform, CL_DEVICE_TYPE_ALL, 1, &device, NULL);
  assert(err == CL_SUCCESS);

  // Context & Queue

  cl_context cl_context = clCreateContext(0, 1, &device, NULL, NULL, &err);
  assert(err == CL_SUCCESS);

  cl_command_queue cl_queue = clCreateCommandQueue(cl_context, device, 0, &err);
  assert(err == CL_SUCCESS);

  // Kernel

  char * cl_sources[2] = { read_file(opencl_kernel_file) , read_file(opencl_klt_runtime_lib) };
  cl_program cl_program = clCreateProgramWithSource(cl_context, 2, cl_sources, NULL, &err);
  assert(err == CL_SUCCESS);

  size_t opts_length = strlen(opencl_kernel_options) + 1;

  char * defn_storage_modifier = " -DSTORAGE_MODIFIER=__global";
  opts_length += strlen(defn_storage_modifier);

#if COMPILE_OPENCL_KERNEL_WITH_DEBUG == 1
  char * debug_flags = " -g";
  opts_length += strlen(debug_flags);
#endif

  char * options = (char *)malloc(opts_length * sizeof(char));
  memset(options, 0, opts_length * sizeof(char));

  strcat(options, opencl_kernel_options);
  strcat(options, defn_storage_modifier);
#if COMPILE_OPENCL_KERNEL_WITH_DEBUG == 1
  strcat(options, debug_flags);
#endif

//printf("options = \"%s\"\n", options);

  err = clBuildProgram(cl_program, 1, &device, options, NULL, NULL);
  if (err == CL_BUILD_PROGRAM_FAILURE)
    dbg_get_ocl_build_log(device, cl_program);
  assert(err == CL_SUCCESS);

  cl_kernel cl_kernel = clCreateKernel(cl_program, kernel->desc->kernel_name, &err);
  assert(err == CL_SUCCESS);

  // Allocation

  cl_mem * cl_data = (cl_mem *)malloc(kernel->desc->data.num_data * sizeof(cl_mem));
  size_t * size_data = (cl_mem *)malloc(kernel->desc->data.num_data * sizeof(size_t));
  for (i = 0; i < kernel->desc->data.num_data; i++) {
    size_data[i] = kernel->desc->data.sizeof_data[i];
    for (j = 0; j < kernel->desc->data.ndims_data[i]; j++) {
      assert(kernel->data[i].sections[j].offset == 0);
      size_data[i] *= kernel->data[i].sections[j].length;
    }
    cl_data[i] = clCreateBuffer(cl_context, CL_MEM_READ_WRITE, size_data[i], NULL, NULL);
  }
  cl_mem * cl_priv = (cl_mem *)malloc(kernel->desc->data.num_priv * sizeof(cl_mem));
  size_t * size_priv = (cl_mem *)malloc(kernel->desc->data.num_priv * sizeof(size_t));
  for (i = 0; i < kernel->desc->data.num_priv; i++) {
    size_priv[i] = kernel->desc->data.sizeof_priv[i];
    for (j = 0; j < kernel->desc->data.ndims_priv[i]; j++) {
      assert(kernel->priv[i].sections[j].offset == 0);
      size_priv[i] *= kernel->priv[i].sections[j].length;
    }
    cl_priv[i] = clCreateBuffer(cl_context, CL_MEM_READ_WRITE, size_priv[i], NULL, NULL);
  }
  size_t size_ctx = sizeof(struct klt_loop_context_t) + 3 * klt_loop_context->num_loops * sizeof(int) + 2 * klt_loop_context->num_tiles * sizeof(int);
  cl_mem context = clCreateBuffer(cl_context, CL_MEM_READ_ONLY, size_ctx, NULL, NULL);

  // Move data to device (+ ctx)

  for (i = 0; i < kernel->desc->data.num_data; i++) {
    err = clEnqueueWriteBuffer(cl_queue, cl_data[i], CL_FALSE, 0, size_data[i], kernel->data[i].ptr, 0, NULL, NULL);
    assert(err == CL_SUCCESS);
  }
  for (i = 0; i < kernel->desc->data.num_priv; i++) {
    err = clEnqueueWriteBuffer(cl_queue, cl_priv[i], CL_FALSE, 0, size_priv[i], kernel->priv[i].ptr, 0, NULL, NULL);
    assert(err == CL_SUCCESS);
  }
  err = clEnqueueWriteBuffer(cl_queue, context, CL_FALSE, 0, size_ctx, klt_loop_context, 0, NULL, NULL);
  assert(err == CL_SUCCESS);

  clFinish(cl_queue);

  // Set kernel arguments

  size_t arg_cnt = 0;
  for (i = 0; i < kernel->desc->data.num_param; i++) {
    err = clSetKernelArg(cl_kernel, arg_cnt++, kernel->desc->data.sizeof_param[i], kernel->param[i]);
    assert(err == CL_SUCCESS);
  }
  for (i = 0; i < kernel->desc->data.num_scalar; i++) {
    err = clSetKernelArg(cl_kernel, arg_cnt++, kernel->desc->data.sizeof_scalar[i], kernel->scalar[i]);
    assert(err == CL_SUCCESS);
  }
  for (i = 0; i < kernel->desc->data.num_data; i++) {
    err = clSetKernelArg(cl_kernel, arg_cnt++, sizeof(cl_mem), &cl_data[i]);
    assert(err == CL_SUCCESS);
  }
  for (i = 0; i < kernel->desc->data.num_priv; i++) {
    err = clSetKernelArg(cl_kernel, arg_cnt++, sizeof(cl_mem), &cl_priv[i]);
    assert(err == CL_SUCCESS);
  }
  err = clSetKernelArg(cl_kernel, arg_cnt++, sizeof(cl_mem), &context);
  assert(err == CL_SUCCESS);

  // Launch kernel

  size_t global_work_size[3] = {
                                 kernel->num_gangs[0] * kernel->num_workers[0],
                                 kernel->num_gangs[1] * kernel->num_workers[1],
                                 kernel->num_gangs[2] * kernel->num_workers[2]
                               };
  size_t local_work_size[3] =  {
                                 kernel->num_workers[0],
                                 kernel->num_workers[1],
                                 kernel->num_workers[2]
                               };

//printf("global_work_size = { %d , %d , %d }\n", global_work_size[0], global_work_size[1], global_work_size[2]);
//printf("local_work_size  = { %d , %d , %d }\n", local_work_size [0], local_work_size [1], local_work_size [2]);

  err = clEnqueueNDRangeKernel(cl_queue, cl_kernel, 3, NULL, global_work_size, local_work_size, 0, NULL, NULL);
  if (err != CL_SUCCESS) {
    const char * str = ocl_status_to_char(err);
    printf("clEnqueueNDRangeKernel error: %s\n", str);
  }
  assert(err == CL_SUCCESS);

  clFinish(cl_queue);

  // Move data from device

  for (i = 0; i < kernel->desc->data.num_data; i++) {
    err = clEnqueueReadBuffer(cl_queue, cl_data[i], CL_FALSE, 0, size_data[i], kernel->data[i].ptr, 0, NULL, NULL);
    assert(err == CL_SUCCESS);
  }

  clFinish(cl_queue);

  // Free

  // TODO
}

char * read_file(const char * filename) {

   FILE *fp;
   int err;
   int size;

   char *source;

   fp = fopen(filename, "rb");
   if(fp == NULL) {
      printf("Could not open kernel file: %s\n", filename);
      assert(0);
   }
   
   err = fseek(fp, 0, SEEK_END);
   if(err != 0) {
      printf("Error seeking to end of file\n");
      assert(0);
   }

   size = ftell(fp);
   if(size < 0) {
      printf("Error getting file position\n");
      assert(0);
   }

   err = fseek(fp, 0, SEEK_SET);
   if(err != 0) {
      printf("Error seeking to start of file\n");
      assert(0);
   }

   source = (char*)malloc(size+1);
   if(source == NULL) {
      printf("Error allocating %d bytes for the program source\n", size+1);
      assert(0);
   }

   err = fread(source, 1, size, fp);
   if(err != size) {
      printf("only read %d bytes\n", err);
      assert(0);
   }

   source[size] = '\0';

   return source;
}

void dbg_get_ocl_build_log(cl_device_id device, cl_program program) {
  char * build_log;
  size_t build_log_size;
  clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, 0, NULL, &build_log_size);
  if (build_log_size == 0)
    printf("[warning] OpenCL return an empty log...\n");
  else {
    build_log = (char*)malloc(build_log_size);
    if (build_log == NULL) {
      perror("[fatal] malloc : build_log");
      exit(-1);
    }
    clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, build_log_size, build_log, NULL);
    build_log[build_log_size-1] = '\0';
    printf("\n\n%s\n\n", build_log);
    free(build_log);
  }
}

const char * ocl_status_to_char(cl_int status) {
  char * status_str;
  switch (status) {
      case CL_DEVICE_NOT_FOUND:                          return (char *)"CL_DEVICE_NOT_FOUND";
      case CL_DEVICE_NOT_AVAILABLE:                      return (char *)"CL_DEVICE_NOT_AVAILABLE";
      case CL_COMPILER_NOT_AVAILABLE:                    return (char *)"CL_COMPILER_NOT_AVAILABLE";
      case CL_MEM_OBJECT_ALLOCATION_FAILURE:             return (char *)"CL_MEM_OBJECT_ALLOCATION_FAILURE";
      case CL_OUT_OF_RESOURCES:                          return (char *)"CL_OUT_OF_RESOURCES";
      case CL_OUT_OF_HOST_MEMORY:                        return (char *)"CL_OUT_OF_HOST_MEMORY";
      case CL_PROFILING_INFO_NOT_AVAILABLE:              return (char *)"CL_PROFILING_INFO_NOT_AVAILABLE";
      case CL_MEM_COPY_OVERLAP:                          return (char *)"CL_MEM_COPY_OVERLAP";
      case CL_IMAGE_FORMAT_MISMATCH:                     return (char *)"CL_IMAGE_FORMAT_MISMATCH";
      case CL_IMAGE_FORMAT_NOT_SUPPORTED:                return (char *)"CL_IMAGE_FORMAT_NOT_SUPPORTED";
      case CL_BUILD_PROGRAM_FAILURE:                     return (char *)"CL_BUILD_PROGRAM_FAILURE";
      case CL_MAP_FAILURE:                               return (char *)"CL_MAP_FAILURE";
      case CL_INVALID_VALUE:                             return (char *)"CL_INVALID_VALUE";
      case CL_INVALID_DEVICE_TYPE:                       return (char *)"CL_INVALID_DEVICE_TYPE";
      case CL_INVALID_PLATFORM:                          return (char *)"CL_INVALID_PLATFORM";
      case CL_INVALID_DEVICE:                            return (char *)"CL_INVALID_DEVICE";
      case CL_INVALID_CONTEXT:                           return (char *)"CL_INVALID_CONTEXT";
      case CL_INVALID_QUEUE_PROPERTIES:                  return (char *)"CL_INVALID_QUEUE_PROPERTIES";
      case CL_INVALID_COMMAND_QUEUE:                     return (char *)"CL_INVALID_COMMAND_QUEUE";
      case CL_INVALID_HOST_PTR:                          return (char *)"CL_INVALID_HOST_PTR";
      case CL_INVALID_MEM_OBJECT:                        return (char *)"CL_INVALID_MEM_OBJECT";
      case CL_INVALID_IMAGE_FORMAT_DESCRIPTOR:           return (char *)"CL_INVALID_IMAGE_FORMAT_DESCRIPTOR";
      case CL_INVALID_IMAGE_SIZE:                        return (char *)"CL_INVALID_IMAGE_SIZE";
      case CL_INVALID_SAMPLER:                           return (char *)"CL_INVALID_SAMPLER";
      case CL_INVALID_BINARY:                            return (char *)"CL_INVALID_BINARY";
      case CL_INVALID_BUILD_OPTIONS:                     return (char *)"CL_INVALID_BUILD_OPTIONS";
      case CL_INVALID_PROGRAM:                           return (char *)"CL_INVALID_PROGRAM";
      case CL_INVALID_PROGRAM_EXECUTABLE:                return (char *)"CL_INVALID_PROGRAM_EXECUTABLE";
      case CL_INVALID_KERNEL_NAME:                       return (char *)"CL_INVALID_KERNEL_NAME";
      case CL_INVALID_KERNEL_DEFINITION:                 return (char *)"CL_INVALID_KERNEL_DEFINITION";
      case CL_INVALID_KERNEL:                            return (char *)"CL_INVALID_KERNEL";
      case CL_INVALID_ARG_INDEX:                         return (char *)"CL_INVALID_ARG_INDEX";
      case CL_INVALID_ARG_VALUE:                         return (char *)"CL_INVALID_ARG_VALUE";
      case CL_INVALID_ARG_SIZE:                          return (char *)"CL_INVALID_ARG_SIZE";
      case CL_INVALID_KERNEL_ARGS:                       return (char *)"CL_INVALID_KERNEL_ARGS";
      case CL_INVALID_WORK_DIMENSION:                    return (char *)"CL_INVALID_WORK_DIMENSION";
      case CL_INVALID_WORK_GROUP_SIZE:                   return (char *)"CL_INVALID_WORK_GROUP_SIZE";
      case CL_INVALID_WORK_ITEM_SIZE:                    return (char *)"CL_INVALID_WORK_ITEM_SIZE";
      case CL_INVALID_GLOBAL_OFFSET:                     return (char *)"CL_INVALID_GLOBAL_OFFSET";
      case CL_INVALID_EVENT_WAIT_LIST:                   return (char *)"CL_INVALID_EVENT_WAIT_LIST";
      case CL_INVALID_EVENT:                             return (char *)"CL_INVALID_EVENT";
      case CL_INVALID_OPERATION:                         return (char *)"CL_INVALID_OPERATION";
      case CL_INVALID_GL_OBJECT:                         return (char *)"CL_INVALID_GL_OBJECT";
      case CL_INVALID_BUFFER_SIZE:                       return (char *)"CL_INVALID_BUFFER_SIZE";
      case CL_INVALID_MIP_LEVEL:                         return (char *)"CL_INVALID_MIP_LEVEL";
      case CL_INVALID_GLOBAL_WORK_SIZE:                  return (char *)"CL_INVALID_GLOBAL_WORK_SIZE";
#ifdef CL_VERSION_1_1
      case CL_MISALIGNED_SUB_BUFFER_OFFSET:              return (char *)"CL_MISALIGNED_SUB_BUFFER_OFFSET";
      case CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST: return (char *)"CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST";
      case CL_INVALID_PROPERTY:                          return (char *)"CL_INVALID_PROPERTY";
#endif
      default:                                           return (char *)"CL_UNKNOWN_ERROR_CODE";
    }
  return (char *)"CL_UNKNOWN_ERROR_CODE";
}

