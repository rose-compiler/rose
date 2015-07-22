
#include "RTL/Host/kernel.h"
#include "KLT/RTL/loop.h"
#include "KLT/RTL/tile.h"
#include "KLT/RTL/context.h"

#include <CL/opencl.h>

#include <stdlib.h>
#include <string.h>

#include <assert.h>

void launch(struct kernel_t * kernel, struct klt_loop_context_t * klt_loop_context) {
  int i;
  cl_int err;

  // Platform & Device

  cl_platform_id platform;
  err = clGetPlatformIDs(1, &platform, NULL);
  cl_device_id device;
  err = clGetDeviceIDs(platform, CL_DEVICE_TYPE_ALL, 1, &device, NULL);

  // Context & Queue

  cl_context cl_context = clCreateContext(0, 1, &device, NULL, NULL, &err);
  cl_command_queue queue = clCreateCommandQueue(cl_context, device, 0, &err);

  // Kernel

  char * kernel_source = NULL; // TODO
  cl_program program = clCreateProgramWithSource(cl_context, 1, (const char **)&kernel_source, NULL, &err);
  clBuildProgram(program, 0, NULL, NULL, NULL, NULL);
  cl_kernel ocl_kernel = clCreateKernel(program, kernel->desc->kernel_name, &err);

  // Allocation

  size_t bytes;
  cl_mem * cl_data = (cl_mem *)malloc(kernel->desc->data.num_data * sizeof(cl_mem));
  for (i = 0; i < kernel->desc->data.num_data; i++) {
    bytes = 1; // TODO
    cl_data[i] = clCreateBuffer(cl_context, CL_MEM_READ_WRITE, bytes, NULL, NULL);
  }
  cl_mem * cl_priv = (cl_mem *)malloc(kernel->desc->data.num_priv * sizeof(cl_mem));
  for (i = 0; i < kernel->desc->data.num_priv; i++) {
    bytes = 1; // TODO
    cl_priv[i] = clCreateBuffer(cl_context, CL_MEM_READ_WRITE, bytes, NULL, NULL);
  }
  bytes = sizeof(struct klt_loop_context_t) + 3 * klt_loop_context->num_loops * sizeof(int) + 2 * klt_loop_context->num_tiles * sizeof(int);
  cl_mem context = clCreateBuffer(cl_context, CL_MEM_READ_ONLY, bytes, NULL, NULL);

  // Move data to device (+ ctx)

  // Set kernel arguments

  // Launch kernel

  // Move data from device
  assert(0); // TODO
}

