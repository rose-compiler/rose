struct pending_eoi 
   {
     int vector;
   };

static __typeof__(struct pending_eoi) per_cpu__pending_eoi[2];

int foobar(void)
   {
  // typeof(&per_cpu__pending_eoi) x;

  // unsigned long __ptr;

#if 1
    return ((((*(
       {
         unsigned long __ptr;
//       __asm__ ("" : "=r"(__ptr) : "0"(&per_cpu__pending_eoi)); 
// Original code:
//       (typeof(&per_cpu__pending_eoi)) (__ptr + (get_cpu_info()->per_cpu_offset)); }))) [(0xdf - 0x20 + 1)-1].vector) != 0);
//       (typeof(&per_cpu__pending_eoi)) (__ptr + (get_cpu_info()->per_cpu_offset)); }))) [(0xdf - 0x20 + 1)-1].vector) != 0);
//       typeof(&per_cpu__pending_eoi) x })));
         (typeof(&per_cpu__pending_eoi)) __ptr; }))) [0].vector) != 0);
#endif
   }
