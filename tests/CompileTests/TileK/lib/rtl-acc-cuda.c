
#include "RTL/Host/kernel.h"
#include "KLT/RTL/loop.h"
#include "KLT/RTL/tile.h"
#include "KLT/RTL/context.h"

#include <cuda.h>
#include <builtin_types.h>
#include <drvapi_error_string.h>

#include <stdlib.h>
#include <string.h>

#include <assert.h>

// Define PTX suffix for different platforms
#if defined(__x86_64) || defined(AMD64) || defined(_M_AMD64)
#  define PTX_SUFFIX "_64.ptx"
#else
#  define PTX_SUFFIX "_32.ptx"
#endif

extern char * cuda_kernel_file;

void launch(struct kernel_t * kernel, struct klt_loop_context_t * klt_loop_context) {
  CUdevice cu_device;
  CUcontext cu_context;
  CUmodule cu_module;
  CUfunction cu_kernel;

  CUresult err = cuInit(0);
  assert(err == CUDA_SUCCESS);

  int num_devices = 0;
  err = cuDeviceGetCount(&num_devices);
  assert(err == CUDA_SUCCESS);
  assert(num_devices > 0);

  err = cuDeviceGet(&cu_device, 0); // get first device
  assert(err == CUDA_SUCCESS);

  err = cuCtxCreate(&cu_context, 0, cu_device);
  assert(err == CUDA_SUCCESS);

  {
    char * ptx_source_file = ""; // TODO

    CUjit_option * jit_options = malloc(3 * sizeof(CUjit_option));
    void ** jit_opt_vals = malloc(3 * sizeof(void *));

    // Set up size of compilation log buffer
    jit_options[0] = CU_JIT_INFO_LOG_BUFFER_SIZE_BYTES;
    int jit_log_buffer_size = 1024;
    jit_opt_vals[0] = (void *)(size_t)jit_log_buffer_size;

    // Set up pointer to the compilation log buffer
    jit_options[1] = CU_JIT_INFO_LOG_BUFFER;
    char * jit_log_buffer = malloc(jit_log_buffer_size * sizeof(char));
    jit_opt_vals[1] = jit_log_buffer;

    // Set up pointer to set the Maximum # of registers for a particular kernel
    jit_options[2] = CU_JIT_MAX_REGISTERS;
    int jit_reg_count = 32;
    jit_opt_vals[2] = (void *)(size_t)jit_reg_count;

    err = cuModuleLoadDataEx(&cu_module, ptx_source_file, 3, jit_options, (void **)jit_opt_vals);
//  printf("> PTX JIT log:\n%s\n", jit_log_buffer);
    assert(err == CUDA_SUCCESS);
  }

  err = cuModuleGetFunction(&cu_kernel, cu_module, kernel->desc->kernel_name);
  assert(err == CUDA_SUCCESS);

  // Allocation

  assert(kernel->desc->data.num_priv == 0); // TODO handling of private

  CUdeviceptr * cu_data = malloc(kernel->desc->data.num_data * sizeof(CUdeviceptr));
  size_t * size_data = (cl_mem *)malloc(kernel->desc->data.num_data * sizeof(size_t));
  for (i = 0; i < kernel->desc->data.num_data; i++) {
    size_data[i] = kernel->desc->data.sizeof_data[i];
    for (j = 0; j < kernel->desc->data.ndims_data[i]; j++) {
      assert(kernel->data[i].sections[j].offset == 0);
      size_data[i] *= kernel->data[i].sections[j].length;
    }
    err = cuMemAlloc(&cu_data[i], size_data[i]);
    assert(err == CUDA_SUCCESS);
  }
/*CUdeviceptr * cu_priv = malloc(kernel->desc->data.num_priv * sizeof(CUdeviceptr));
  size_t * size_priv = (cl_mem *)malloc(kernel->desc->data.num_priv * sizeof(size_t));
  for (i = 0; i < kernel->desc->data.num_priv; i++) {
    size_priv[i] = kernel->desc->data.sizeof_priv[i];
    for (j = 0; j < kernel->desc->data.ndims_priv[i]; j++) {
      assert(kernel->priv[i].sections[j].offset == 0);
      size_priv[i] *= kernel->priv[i].sections[j].length;
    }
    err = cuMemAlloc(&cu_priv[i], size_priv[i]);
    assert(err == CUDA_SUCCESS);
  }*/
  size_t size_ctx = sizeof(struct klt_loop_context_t) + 3 * klt_loop_context->num_loops * sizeof(int) + 2 * klt_loop_context->num_tiles * sizeof(int);
  CUdeviceptr context;
  err = cuMemAlloc(&context, size_ctx);
  assert(err == CUDA_SUCCESS);

  // Move data to device (+ ctx)

  for (i = 0; i < kernel->desc->data.num_data; i++) {
    err = cuMemcpyHtoD(cl_data[i], kernel->data[i].ptr, size_data[i]);
    assert(err == CUDA_SUCCESS);
  }
/*for (i = 0; i < kernel->desc->data.num_priv; i++) {
    err = cuMemcpyHtoD(cl_priv[i], kernel->priv[i].ptr, size_priv[i]);
    assert(err == CUDA_SUCCESS);
  }*/
  err = cuMemcpyHtoD(context, klt_loop_context, size_ctx);
  assert(err == CUDA_SUCCESS);

  // Set kernel arguments

  void ** args = malloc((kernel->desc->data.num_param + kernel->desc->data.num_scalar + kernel->desc->data.num_data + kernel->desc->data.num_priv) * sizeof(void *));

  size_t arg_cnt = 0;
  for (i = 0; i < kernel->desc->data.num_param; i++) {
    args[arg_cnt++] = kernel->param[i];
  }
  for (i = 0; i < kernel->desc->data.num_scalar; i++) {
    args[arg_cnt++] = kernel->scalar[i];
  }
  for (i = 0; i < kernel->desc->data.num_data; i++) {
    args[arg_cnt++] = &cu_data[i];
  }
/*for (i = 0; i < kernel->desc->data.num_priv; i++) {
    args[arg_cnt++] = &cu_priv[i];
  }*/
    args[arg_cnt++] = &context;

  // Launch kernel

  err = cuCtxSynchronize();
  assert(err == CUDA_SUCCESS);

  int shared_mem_bytes = 0;
  err = cuLaunchKernel(cu_kernel, kernel->num_gangs[0], kernel->num_gangs[1], kernel->num_gangs[2],
                                  kernel->num_workers[0], kernel->num_workers[1], kernel->num_workers[2],
                                  shared_mem_bytes, NULL, args, NULL);
  assert(err == CUDA_SUCCESS);

  err = cuCtxSynchronize();
  assert(err == CUDA_SUCCESS);

  // Move data from device

    error = cuMemcpyDtoH(h_C, d_C, size);

  for (i = 0; i < kernel->desc->data.num_data; i++) {
    err = cuMemcpyDtoH(kernel->data[i].ptr, cl_data[i], size_data[i]);
    assert(err == CUDA_SUCCESS);
  }

  err = cuCtxSynchronize();
  assert(err == CUDA_SUCCESS);

  // Free

  assert(err == CUDA_SUCCESS);
}

