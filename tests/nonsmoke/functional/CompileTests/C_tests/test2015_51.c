struct cpu_info {
    unsigned int processor_id;
    unsigned long per_cpu_offset;
    unsigned long __pad_for_stack_bottom;
};

struct cpu_info *get_cpu_info(void);

typedef struct { int counter; } atomic_t;

struct schedule_data {
    void *sched_priv;
    atomic_t urgent_count;
};

extern __typeof__(struct schedule_data) per_cpu__schedule_data;

static __inline__ int sched_has_urgent_vcpu(void)
   {
     unsigned long __ptr;

     ({ 
        typeof(*&((&(*({ (typeof(&per_cpu__schedule_data)) (__ptr + (get_cpu_info()->per_cpu_offset)); })).urgent_count)->counter)) __x; 
     });

     return 0;
   }
