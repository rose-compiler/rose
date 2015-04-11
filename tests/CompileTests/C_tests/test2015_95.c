#define ALIGN(x,a)              __ALIGN_MASK(x,(typeof(x))(a)-1)
#define __ALIGN_MASK(x,mask)    (((x)+(mask))&~(mask))

extern unsigned long long RamSizeOver4G;

// Note that the use of a static function and the definition of align_mem in a nested basic block
// causes the type to expand to:
// unsigned long long x = 0x100000000LL + RamSizeOver4G + (((typeof(__uint128_t ) )align_mem) - 1) & ~(((typeof(__uint128_t ) )align_mem) - 1);
// else we get:
// unsigned long long x = 0x100000000LL + RamSizeOver4G + (((typeof(0x100000000LL + RamSizeOver4G) )align_mem) - 1) & ~(((typeof(0x100000000LL + RamSizeOver4G) )align_mem) - 1);

// The problem is that in -m32 mode, the __uint128_t is not defined.
static void foobar()
   {
        {
          unsigned long long align_mem;
          unsigned long long x = ALIGN(0x100000000LL + RamSizeOver4G, align_mem);
        }

     unsigned long long align_mem;
     unsigned long long x = ALIGN(0x100000000LL + RamSizeOver4G, align_mem);
   }
