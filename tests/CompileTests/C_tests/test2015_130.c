
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

// DQ (2/16/2016): This should fix ROSE-210 in JIRA bug reports.
#if ( (__GNUC__ == 4) && (__GNUC_MINOR__ < 9) )
// The inline declaration without a function definition is an error for the GNU 4.9.2 compiler.
static __attribute__ (( always_inline )) __inline__ uint64_t int128_get64(Int128 a);
#else
// The inline declaration is provided with a definition so as to not be an error for the GNU 4.9.2 compiler.
static __attribute__ (( always_inline )) __inline__ uint64_t int128_get64(Int128 a)
   {
     uint64_t x = 0L;
     return x;
   }
#endif

static void address_space_add_del_ioeventfds(AddressSpace *as)
   {
     MemoryRegionIoeventfd *fd = 0L;
     MemoryRegionSection section;
     section = (MemoryRegionSection)
        {
          .address_space = as,
          .offset_within_address_space = int128_get64(fd->addr.start),
          .size = fd->addr.size,
        };
   }

