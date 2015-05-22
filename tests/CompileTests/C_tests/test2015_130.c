
struct MemoryRegion { int x; };
typedef struct MemoryRegion MemoryRegion;

struct AddressSpace {
    char *name;
    MemoryRegion *root;
};
typedef struct AddressSpace AddressSpace;

typedef unsigned long int uint64_t;
typedef long int int64_t;
typedef uint64_t hwaddr;

typedef struct Int128 Int128;
struct Int128 {
    uint64_t lo;
    int64_t hi;
};

struct MemoryRegionSection {
    MemoryRegion *mr;
    AddressSpace *address_space;
    hwaddr offset_within_region;
    Int128 size;
    hwaddr offset_within_address_space;
};
typedef struct MemoryRegionSection MemoryRegionSection;


typedef struct AddrRange AddrRange;
struct AddrRange {
    Int128 start;
    Int128 size;
};
struct MemoryRegionIoeventfd {
    AddrRange addr;
};

typedef struct MemoryRegionIoeventfd MemoryRegionIoeventfd;

static __attribute__ (( always_inline )) __inline__ uint64_t int128_get64(Int128 a);

static void address_space_add_del_ioeventfds(AddressSpace *as)
   {
     MemoryRegionIoeventfd *fd;
     MemoryRegionSection section;
     section = (MemoryRegionSection)
        {
          .address_space = as,
          .offset_within_address_space = int128_get64(fd->addr.start),
          .size = fd->addr.size,
        };
   }

