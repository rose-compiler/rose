typedef unsigned long int uint64_t;
typedef long int int64_t;
typedef uint64_t hwaddr;

typedef struct Int128 Int128;
struct Int128 {
    uint64_t lo;
    int64_t hi;
};

struct MemoryRegionSection 
   {
     Int128 size;
   };
typedef struct MemoryRegionSection MemoryRegionSection;

typedef struct AddrRange AddrRange;
struct AddrRange 
   {
     Int128 start;
     Int128 size;
   };

struct MemoryRegionIoeventfd 
   {
     AddrRange addr;
   };
typedef struct MemoryRegionIoeventfd MemoryRegionIoeventfd;

static void address_space_add_del_ioeventfds()
   {
     MemoryRegionIoeventfd *fd;
     MemoryRegionSection section;

  // This is an error when unparsed as:
  // section = ((MemoryRegionSection ){.size = {fd -> addr . size}});
  // Must be unparsed as:
  // section = ((MemoryRegionSection ){.size = fd -> addr . size});

     section = (MemoryRegionSection)
        {
        // .size = {fd->addr.size}
           .size = fd->addr.size
        };
   }

