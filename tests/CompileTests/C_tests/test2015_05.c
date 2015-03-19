static struct
   {
     unsigned long start, end;
     unsigned long size;
   } ranges[16] __attribute__((__section__(".init.data")));

void set_kexec_crash_area_size(unsigned long system_ram)
   {
     unsigned int idx;

  // Unpased code is: for (idx = 0; idx < sizeof(ranges) / sizeof(ranges[0]) + sizeof(struct {int :(-(!(!__types_compatible(struct {unsigned long start;unsigned long end;unsigned long size;},struct {unsigned long start;unsigned long end;unsigned long size;}*))));}); ++idx) { }{ }

     for ( idx = 0; idx < (sizeof(ranges) / sizeof((ranges)[0]) + sizeof(struct { int:-!!(__builtin_types_compatible_p(typeof(ranges), typeof(&ranges[0]))); })); ++idx )
        {
        }
   }
