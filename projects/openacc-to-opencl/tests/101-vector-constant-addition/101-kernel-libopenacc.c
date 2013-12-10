
#include "OpenACC/private/debug.h"
#include "OpenACC/private/region.h"
#include "OpenACC/private/kernel.h"
#include "OpenACC/private/loop.h"
#include "OpenACC/private/data-env.h"

typedef struct acc_kernel_t_ * acc_kernel_t;
typedef struct acc_region_t_ * acc_region_t;

extern struct acc_kernel_desc_t_ kernel_0x00_desc;
extern struct acc_region_desc_t_ region_0x00_desc;

void kernel_101(
  unsigned long n, float * a, float offset,
  unsigned long num_gang, unsigned long num_worker, unsigned long vector_length,
  acc_timer_t data_timer, acc_timer_t comp_timer
) {
  unsigned i;

  acc_timer_start(data_timer);

  acc_push_data_environment();  // construct data start

  acc_copyin(a, n * sizeof(float)); // clause copy(a[0:n])

  { // (1)

    acc_timer_start(comp_timer);

    // Initialize parallel region
    unsigned long region_0_num_gang = num_gang;           // clause num_gang(num_gang)
    unsigned long region_0_num_worker = num_worker;       // clause num_worker(num_worker)
    unsigned long region_0_vector_length = vector_length; // clause vector_length(vector_length)    
    acc_region_t region_0 = acc_build_region(&region_0x00_desc, 1, &region_0_num_gang, &region_0_num_worker, region_0_vector_length);

    acc_region_start(region_0); // construct parallel start

    { // (2)

      // Create a kernel descriptor
      acc_kernel_t kernel_0 = acc_build_kernel(&kernel_0x00_desc);

      // Set scalar arguments
      kernel_0->scalar_ptrs[0] = &offset;

      // Set data arguments
      kernel_0->data_ptrs[0] = acc_deviceptr(a);

      // Configure the loop
      kernel_0->loops[0]->lower = 0;
      kernel_0->loops[0]->upper = n;
      kernel_0->loops[0]->stride = 1;

      // Enqueue the kernel for the current region
      acc_enqueue_kernel(region_0, kernel_0);

    } // (2)

    acc_region_stop(region_0); // construct parallel stop

    acc_timer_stop(comp_timer);

  } // (1)

  acc_copyout(a, n * sizeof(float)); // clause copy(a[0:n])

  acc_pop_data_environment(); // construct data stop

  acc_timer_stop(data_timer);
}

