// typedef unsigned long int uint64_t;
// typedef long int int64_t;

typedef struct Int128 Int128;
struct Int128 {
    unsigned long int lo;
    long int hi;
};

struct MemoryRegionSection 
   {
     Int128 size;
   };
typedef struct MemoryRegionSection MemoryRegionSection;

static void foobar()
   {
     MemoryRegionSection section;

     Int128 tmp_size;

     section = (MemoryRegionSection)
        {
          .size = tmp_size
        };
   }

