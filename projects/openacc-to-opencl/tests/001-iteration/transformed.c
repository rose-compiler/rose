/*!
 * \addtogroup grp_libopenacc_tests
 * @{
 *
 * \file tests/test_01.c
 *
 */

#include <math.h>

#include <stdio.h>

#include <assert.h>

#include "OpenACC/openacc.h"
#include "OpenACC/private/debug.h"
#include "OpenACC/private/region.h"
#include "OpenACC/private/kernel.h"
#include "OpenACC/private/loop.h"
#include "OpenACC/private/data-env.h"

typedef struct acc_kernel_t_ * acc_kernel_t;
typedef struct acc_region_t_ * acc_region_t;

extern struct acc_kernel_desc_t_ kernel_0x00_desc;
extern struct acc_region_desc_t_ region_0x00_desc;

const unsigned long n = 8*64;

int main() {
  int a[n];
  int b[n];

  unsigned i;

  acc_set_device_type(acc_device_any);
  acc_set_device_num(0, acc_device_any);

  // Push a new data environment for region_0
  acc_push_data_environment();

  acc_create(a, n * sizeof(int));
  acc_create(b, n * sizeof(int));

  unsigned long region_0_num_gang = 8;     // clause num_gang(16)
  unsigned long region_0_num_worker = 64;   // clause num_worker(64)
  unsigned long region_0_vector_length = 1; // clause vector_length(1)

  // construct and start parallel region descriptor
  acc_region_t region_0 = acc_build_region(&region_0x00_desc, 1, &region_0_num_gang, &region_0_num_worker, region_0_vector_length);
  acc_region_start(region_0);

  // Create a kernel descriptor
  acc_kernel_t kernel_0 = acc_build_kernel(&kernel_0x00_desc);

  // Set data arguments
  kernel_0->data_ptrs[0] = acc_deviceptr(a);
  kernel_0->data_ptrs[1] = acc_deviceptr(b);

  // Configure the loop
  kernel_0->loops[0]->lower = 0;
  kernel_0->loops[0]->upper = n;
  kernel_0->loops[0]->stride = 1;

  // Enqueue the kernel for the current region
  acc_enqueue_kernel(region_0, kernel_0);

  acc_region_stop(region_0);

  acc_copyout(a, n * sizeof(int));
  acc_copyout(b, n * sizeof(int));

  acc_pop_data_environment();

  for (i = 0; i < n; i++)
    printf("%u : gang = %u , worker = %u\n", i, a[i], b[i]);

  return 0;
}

/*! @} */

