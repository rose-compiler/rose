
#include "RTL/Host/kernel.h"
#include "KLT/RTL/loop.h"
#include "KLT/RTL/tile.h"
#include "KLT/RTL/context.h"

#include <CL/opencl.h>

#include <stdlib.h>
#include <string.h>

#include <assert.h>

void launch(struct kernel_t * kernel, struct klt_loop_context_t * klt_loop_context) {
  cl_int err;

  cl_platform_id platform;
  err = clGetPlatformIDs(1, &platform, NULL);

  cl_device_id device;
  err = clGetDeviceIDs(platform, CL_DEVICE_TYPE_ALL, 1, &device, NULL);

  cl_context context = clCreateContext(0, 1, &device, NULL, NULL, &err);
  cl_command_queue queue = clCreateCommandQueue(context, device, 0, &err);

  char * kernel_source = NULL; // TODO

  cl_program program = clCreateProgramWithSource(context, 1, (const char **)&kernel_source, NULL, &err);
  clBuildProgram(program, 0, NULL, NULL, NULL, NULL);
  cl_kernel ocl_kernel = clCreateKernel(program, kernel->desc->kernel_name, &err);

  // Move data to device (+ ctx)

  // Set kernel arguments

  // Launch kernel

  // Move data from device
  assert(0); // TODO
}

