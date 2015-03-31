struct cpu_info {
//  struct cpu_user_regs guest_cpu_user_regs;
    unsigned int processor_id;
//  struct vcpu *current_vcpu;
    unsigned long per_cpu_offset;
    unsigned long __pad_for_stack_bottom;
};

// static __inline__ struct cpu_info *get_cpu_info(void);
struct cpu_info *get_cpu_info(void);

typedef struct { int counter; } atomic_t;

struct schedule_data {
//  spinlock_t *schedule_lock,_lock;
//  struct vcpu *curr;
    void *sched_priv;
//  struct timer s_timer;
    atomic_t urgent_count;
};

extern __typeof__(struct schedule_data) per_cpu__schedule_data;

static __inline__ int sched_has_urgent_vcpu(void)
   {
  // ({ typeof(*&((&(*({ unsigned long __ptr; __asm__ ("" : "=r"(__ptr) : "0"(&per_cpu__schedule_data)); (typeof(&per_cpu__schedule_data)) (__ptr + (get_cpu_info()->per_cpu_offset)); })).urgent_count)->counter)) __x; switch ( sizeof(*&((&(*({ unsigned long __ptr; __asm__ ("" : "=r"(__ptr) : "0"(&per_cpu__schedule_data)); (typeof(&per_cpu__schedule_data)) (__ptr + (get_cpu_info()->per_cpu_offset)); })).urgent_count)->counter)) ) { case 1: __x = (typeof(*&((&(*({ unsigned long __ptr; __asm__ ("" : "=r"(__ptr) : "0"(&per_cpu__schedule_data)); (typeof(&per_cpu__schedule_data)) (__ptr + (get_cpu_info()->per_cpu_offset)); })).urgent_count)->counter)))read_u8_atomic((uint8_t *)&((&(*({ unsigned long __ptr; __asm__ ("" : "=r"(__ptr) : "0"(&per_cpu__schedule_data)); (typeof(&per_cpu__schedule_data)) (__ptr + (get_cpu_info()->per_cpu_offset)); })).urgent_count)->counter)); break; case 2: __x = (typeof(*&((&(*({ unsigned long __ptr; __asm__ ("" : "=r"(__ptr) : "0"(&per_cpu__schedule_data)); (typeof(&per_cpu__schedule_data)) (__ptr + (get_cpu_info()->per_cpu_offset)); })).urgent_count)->counter)))read_u16_atomic((uint16_t *)&((&(*({ unsigned long __ptr; __asm__ ("" : "=r"(__ptr) : "0"(&per_cpu__schedule_data)); (typeof(&per_cpu__schedule_data)) (__ptr + (get_cpu_info()->per_cpu_offset)); })).urgent_count)->counter)); break; case 4: __x = (typeof(*&((&(*({ unsigned long __ptr; __asm__ ("" : "=r"(__ptr) : "0"(&per_cpu__schedule_data)); (typeof(&per_cpu__schedule_data)) (__ptr + (get_cpu_info()->per_cpu_offset)); })).urgent_count)->counter)))read_u32_atomic((uint32_t *)&((&(*({ unsigned long __ptr; __asm__ ("" : "=r"(__ptr) : "0"(&per_cpu__schedule_data)); (typeof(&per_cpu__schedule_data)) (__ptr + (get_cpu_info()->per_cpu_offset)); })).urgent_count)->counter)); break; case 8: __x = (typeof(*&((&(*({ unsigned long __ptr; __asm__ ("" : "=r"(__ptr) : "0"(&per_cpu__schedule_data)); (typeof(&per_cpu__schedule_data)) (__ptr + (get_cpu_info()->per_cpu_offset)); })).urgent_count)->counter)))read_u64_atomic((uint64_t *)&((&(*({ unsigned long __ptr; __asm__ ("" : "=r"(__ptr) : "0"(&per_cpu__schedule_data)); (typeof(&per_cpu__schedule_data)) (__ptr + (get_cpu_info()->per_cpu_offset)); })).urgent_count)->counter)); break; default: __x = 0; __bad_atomic_size(); break; } __x; });

     unsigned long __ptr;
  // __asm__ ("" : "=r"(__ptr) : "0"(&per_cpu__schedule_data)); 

     ({ 
        typeof(*&((&(*({ (typeof(&per_cpu__schedule_data)) (__ptr + (get_cpu_info()->per_cpu_offset)); })).urgent_count)->counter)) __x; 

//      switch ( sizeof(*&((&(*({ unsigned long __ptr; __asm__ ("" : "=r"(__ptr) : "0"(&per_cpu__schedule_data)); (typeof(&per_cpu__schedule_data)) (__ptr + (get_cpu_info()->per_cpu_offset)); })).urgent_count)->counter)) ) { case 1: __x = (typeof(*&((&(*({ unsigned long __ptr; __asm__ ("" : "=r"(__ptr) : "0"(&per_cpu__schedule_data)); (typeof(&per_cpu__schedule_data)) (__ptr + (get_cpu_info()->per_cpu_offset)); })).urgent_count)->counter)))read_u8_atomic((uint8_t *)&((&(*({ unsigned long __ptr; __asm__ ("" : "=r"(__ptr) : "0"(&per_cpu__schedule_data)); (typeof(&per_cpu__schedule_data)) (__ptr + (get_cpu_info()->per_cpu_offset)); })).urgent_count)->counter)); break; case 2: __x = (typeof(*&((&(*({ unsigned long __ptr; __asm__ ("" : "=r"(__ptr) : "0"(&per_cpu__schedule_data)); (typeof(&per_cpu__schedule_data)) (__ptr + (get_cpu_info()->per_cpu_offset)); })).urgent_count)->counter)))read_u16_atomic((uint16_t *)&((&(*({ unsigned long __ptr; __asm__ ("" : "=r"(__ptr) : "0"(&per_cpu__schedule_data)); (typeof(&per_cpu__schedule_data)) (__ptr + (get_cpu_info()->per_cpu_offset)); })).urgent_count)->counter)); break; case 4: __x = (typeof(*&((&(*({ unsigned long __ptr; __asm__ ("" : "=r"(__ptr) : "0"(&per_cpu__schedule_data)); (typeof(&per_cpu__schedule_data)) (__ptr + (get_cpu_info()->per_cpu_offset)); })).urgent_count)->counter)))read_u32_atomic((uint32_t *)&((&(*({ unsigned long __ptr; __asm__ ("" : "=r"(__ptr) : "0"(&per_cpu__schedule_data)); (typeof(&per_cpu__schedule_data)) (__ptr + (get_cpu_info()->per_cpu_offset)); })).urgent_count)->counter)); break; case 8: __x = (typeof(*&((&(*({ unsigned long __ptr; __asm__ ("" : "=r"(__ptr) : "0"(&per_cpu__schedule_data)); (typeof(&per_cpu__schedule_data)) (__ptr + (get_cpu_info()->per_cpu_offset)); })).urgent_count)->counter)))read_u64_atomic((uint64_t *)&((&(*({ unsigned long __ptr; __asm__ ("" : "=r"(__ptr) : "0"(&per_cpu__schedule_data)); (typeof(&per_cpu__schedule_data)) (__ptr + (get_cpu_info()->per_cpu_offset)); })).urgent_count)->counter)); break; default: __x = 0; __bad_atomic_size(); break; } __x; 
     });

     return 0;
   }
