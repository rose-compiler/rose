
#include "RTL/Host/tilek-rtl.h"

#include "KLT/RTL/kernel.h"
#include "KLT/RTL/loop.h"
#include "KLT/RTL/tile.h"
#include "KLT/RTL/data.h"
#include "KLT/RTL/context.h"

#include <cuda.h>
#include <builtin_types.h>

#include <stdlib.h>
#include <string.h>

#include <assert.h>

// Define PTX suffix for different platforms
//#if defined(__x86_64) || defined(AMD64) || defined(_M_AMD64)
//#  define PTX_SUFFIX "_64.ptx"
//#else
//#  define PTX_SUFFIX "_32.ptx"
//#endif
#define PTX_SUFFIX ".ptx"

extern char * cuda_kernel_file;

void klt_user_schedule(
  struct klt_kernel_t * kernel, struct klt_subkernel_desc_t * subkernel,
  struct klt_loop_context_t * klt_loop_context, struct klt_data_context_t * klt_data_context
) {
  int i, j;

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
    size_t filename_length = strlen(cuda_kernel_file) + strlen(PTX_SUFFIX) + 1;
    char * ptx_source_file = (char *)malloc(filename_length * sizeof(char));
    memset(ptx_source_file, 0, filename_length * sizeof(char));

    strcat(ptx_source_file, cuda_kernel_file);
    strcat(ptx_source_file, PTX_SUFFIX);

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

  err = cuModuleGetFunction(&cu_kernel, cu_module, subkernel->config->kernel_name);
  assert(err == CUDA_SUCCESS);

  // Allocation

  CUdeviceptr * cu_data = malloc(kernel->desc->data.num_data * sizeof(CUdeviceptr));
  size_t * size_data = (size_t *)malloc(kernel->desc->data.num_data * sizeof(size_t));
  for (i = 0; i < kernel->desc->data.num_data; i++) {
    size_data[i] = kernel->desc->data.sizeof_data[i];
    for (j = 0; j < kernel->desc->data.ndims_data[i]; j++) {
      assert(kernel->data[i].sections[j].offset == 0);
      size_data[i] *= kernel->data[i].sections[j].length;
    }
    err = cuMemAlloc(&cu_data[i], size_data[i]);
    assert(err == CUDA_SUCCESS);
  }

  size_t size_loop_ctx = sizeof(struct klt_loop_context_t) + 3 * klt_loop_context->num_loops * sizeof(int) + 2 * klt_loop_context->num_tiles * sizeof(int);
  CUdeviceptr loop_context;
  err = cuMemAlloc(&loop_context, size_loop_ctx);
  assert(err == CUDA_SUCCESS);

  size_t size_data_ctx = sizeof(struct klt_data_context_t);
  CUdeviceptr data_context;
  err = cuMemAlloc(&data_context, size_data_ctx);
  assert(err == CUDA_SUCCESS);

  // Move data to device (+ ctx)

  for (i = 0; i < kernel->desc->data.num_data; i++) {
    err = cuMemcpyHtoD(cu_data[i], kernel->data[i].ptr, size_data[i]);
    assert(err == CUDA_SUCCESS);
  }

  err = cuMemcpyHtoD(loop_context, klt_loop_context, size_loop_ctx);
  assert(err == CUDA_SUCCESS);

  err = cuMemcpyHtoD(data_context, klt_data_context, size_data_ctx);
  assert(err == CUDA_SUCCESS);

  // Set kernel arguments

  void ** args = malloc((kernel->desc->data.num_param + kernel->desc->data.num_data) * sizeof(void *));

  size_t arg_cnt = 0;
  for (i = 0; i < subkernel->num_params; i++)
    args[arg_cnt++] = kernel->param[i];
  for (i = 0; i < subkernel->num_data; i++)
    args[arg_cnt++] = &cu_data[i];
  args[arg_cnt++] = &loop_context;
  args[arg_cnt++] = &data_context;

  // Launch kernel

  err = cuCtxSynchronize();
  assert(err == CUDA_SUCCESS);

  int shared_mem_bytes = 0;
  err = cuLaunchKernel(cu_kernel, kernel->config->num_gangs[0], kernel->config->num_gangs[1], kernel->config->num_gangs[2],
                                  kernel->config->num_workers[0], kernel->config->num_workers[1], kernel->config->num_workers[2],
                                  shared_mem_bytes, NULL, args, NULL);
  assert(err == CUDA_SUCCESS);

  err = cuCtxSynchronize();
  assert(err == CUDA_SUCCESS);

  // Move data from device

  for (i = 0; i < kernel->desc->data.num_data; i++) {
    err = cuMemcpyDtoH(kernel->data[i].ptr, cu_data[i], size_data[i]);
    assert(err == CUDA_SUCCESS);
  }

  err = cuCtxSynchronize();
  assert(err == CUDA_SUCCESS);

  // Free

  assert(err == CUDA_SUCCESS);
}

void klt_user_wait(struct klt_kernel_t * kernel) { /* NOP */ }

