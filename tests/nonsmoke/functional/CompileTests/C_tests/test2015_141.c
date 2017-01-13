#pragma GCC visibility push(hidden)
extern unsigned long trampoline_phys;
extern char trampoline_start[], trampoline_end[];
extern char trampoline_realmode_entry[];
extern unsigned int trampoline_xen_phys_start;
extern unsigned char trampoline_cpu_started;
extern char wakeup_start[];
extern unsigned int video_mode, video_flags;
extern unsigned short boot_edid_caps;
extern unsigned char boot_edid_info[128];
extern unsigned long xen_phys_start;
int current_domain_id(void);
typedef __signed__ char __s8;
typedef unsigned char __u8;
typedef __signed__ short __s16;
typedef unsigned short __u16;
typedef __signed__ int __s32;
typedef unsigned int __u32;
typedef __signed__ long __s64;
typedef unsigned long __u64;

typedef signed char s8;
typedef unsigned char u8;

typedef signed short s16;
typedef unsigned short u16;

typedef signed int s32;
typedef unsigned int u32;

typedef signed long s64;
typedef unsigned long u64;
typedef unsigned long paddr_t;




typedef long unsigned int size_t;




typedef char bool_t;
typedef unsigned char u_char;
typedef unsigned short u_short;
typedef unsigned int u_int;
typedef unsigned long u_long;
typedef unsigned char unchar;
typedef unsigned short ushort;
typedef unsigned int uint;
typedef unsigned long ulong;
typedef __u8 uint8_t;
typedef __u8 u_int8_t;
typedef __s8 int8_t;
typedef __u16 uint16_t;
typedef __u16 u_int16_t;
typedef __s16 int16_t;
typedef __u32 uint32_t;
typedef __u32 u_int32_t;
typedef __s32 int32_t;
typedef __u64 uint64_t;
typedef __u64 u_int64_t;
typedef __s64 int64_t;
struct domain;
struct vcpu;
typedef __u16 __le16;
typedef __u16 __be16;
typedef __u32 __le32;
typedef __u32 __be32;
typedef __u64 __le64;
typedef __u64 __be64;
typedef unsigned long uintptr_t;
extern char _start[], _end[];
extern char _stext[], _etext[];





extern const char _srodata[], _erodata[];





extern char _sinittext[], _einittext[];





extern enum system_state {
    SYS_STATE_early_boot,
    SYS_STATE_boot,
    SYS_STATE_active,
    SYS_STATE_suspend,
    SYS_STATE_resume
} system_state;

bool_t is_active_kernel_text(unsigned long addr);
typedef __builtin_va_list va_list;
extern void xfree(void *);
extern void *_xmalloc(unsigned long size, unsigned long align);
extern void *_xzalloc(unsigned long size, unsigned long align);
static __inline__ void *_xmalloc_array(
    unsigned long size, unsigned long align, unsigned long num)
{
 if (size && num > (~0U) / size)
  return ((void*)0);
  return _xmalloc(size * num, align);
}
static __inline__ void *_xzalloc_array(
    unsigned long size, unsigned long align, unsigned long num)
{
    if (size && num > (~0U) / size)
        return ((void*)0);
    return _xzalloc(size * num, align);
}
struct xmem_pool;
typedef void *(xmem_pool_get_memory)(unsigned long bytes);
typedef void (xmem_pool_put_memory)(void *ptr);
struct xmem_pool *xmem_pool_create(
    const char *name,
    xmem_pool_get_memory get_mem,
    xmem_pool_put_memory put_mem,
    unsigned long init_size,
    unsigned long max_size,
    unsigned long grow_size);
void xmem_pool_destroy(struct xmem_pool *pool);
void *xmem_pool_alloc(unsigned long size, struct xmem_pool *pool);
int xmem_pool_maxalloc(struct xmem_pool *pool);
void xmem_pool_free(void *ptr, struct xmem_pool *pool);
unsigned long xmem_pool_get_used_size(struct xmem_pool *pool);
unsigned long xmem_pool_get_total_size(struct xmem_pool *pool);
extern char * strpbrk(const char *,const char *);
extern char * strsep(char **,const char *);
extern size_t strspn(const char *,const char *);
extern void *memmove(void *dest, const void *src, size_t n);
extern size_t strlcpy(char *,const char *, size_t);
extern size_t strlcat(char *,const char *, size_t);
extern int strcmp(const char *,const char *);
extern int strncmp(const char *,const char *,size_t);
extern int strnicmp(const char *, const char *, size_t);
extern int strcasecmp(const char *, const char *);
extern char * strchr(const char *,int);
extern char * strrchr(const char *,int);
extern char * strstr(const char *,const char *);
extern size_t strlen(const char *);
extern size_t strnlen(const char *,size_t);
extern void * memscan(void *,int,size_t);
extern int memcmp(const void *,const void *,size_t);
extern void * memchr(const void *,int,size_t);
struct bug_frame {
    signed int loc_disp:24;
    unsigned int line_hi:(31 - 24);
    signed int ptr_disp:24;
    unsigned int line_lo:(31 - 24);
    signed int msg_disp[];
};
extern const struct bug_frame __start_bug_frames[],
                              __stop_bug_frames_0[],
                              __stop_bug_frames_1[],
                              __stop_bug_frames_2[],
                              __stop_bug_frames_3[];
void __bug(char *file, int line) __attribute__((noreturn));
void __warn(char *file, int line);
struct domain;
void cmdline_parse(const char *cmdline);
int parse_bool(const char *s);
extern void printk(const char *format, ...)
    __attribute__ ((format (printf, 1, 2)));
extern void guest_printk(const struct domain *d, const char *format, ...)
    __attribute__ ((format (printf, 2, 3)));
extern void panic(const char *format, ...)
    __attribute__ ((format (printf, 1, 2)));
extern long vm_assist(struct domain *, unsigned int, unsigned int);
extern int __printk_ratelimit(int ratelimit_ms, int ratelimit_burst);
extern int printk_ratelimit(void);
extern int snprintf(char * buf, size_t size, const char * fmt, ...)
    __attribute__ ((format (printf, 3, 4)));
extern int vsnprintf(char *buf, size_t size, const char *fmt, va_list args)
    __attribute__ ((format (printf, 3, 0)));
extern int scnprintf(char * buf, size_t size, const char * fmt, ...)
    __attribute__ ((format (printf, 3, 4)));
extern int vscnprintf(char *buf, size_t size, const char *fmt, va_list args)
    __attribute__ ((format (printf, 3, 0)));
long simple_strtol(
    const char *cp,const char **endp, unsigned int base);
unsigned long simple_strtoul(
    const char *cp,const char **endp, unsigned int base);
long long simple_strtoll(
    const char *cp,const char **endp, unsigned int base);
unsigned long long simple_strtoull(
    const char *cp,const char **endp, unsigned int base);
unsigned long long parse_size_and_unit(const char *s, const char **ps);
uint64_t muldiv64(uint64_t a, uint32_t b, uint32_t c);
extern int tainted;
extern char *print_tainted(char *str);
extern void add_taint(unsigned);
struct cpu_user_regs;
void dump_execstate(struct cpu_user_regs *);
void init_constructors(void);
static __inline__ int generic_ffs(int x)
{
    int r = 1;
    if (!x)
        return 0;
    if (!(x & 0xffff)) {
        x >>= 16;
        r += 16;
    }
    if (!(x & 0xff)) {
        x >>= 8;
        r += 8;
    }
    if (!(x & 0xf)) {
        x >>= 4;
        r += 4;
    }
    if (!(x & 3)) {
        x >>= 2;
        r += 2;
    }
    if (!(x & 1)) {
        x >>= 1;
        r += 1;
    }
    return r;
}
static __inline__ int generic_fls(int x)
{
    int r = 32;
    if (!x)
        return 0;
    if (!(x & 0xffff0000u)) {
        x <<= 16;
        r -= 16;
    }
    if (!(x & 0xff000000u)) {
        x <<= 8;
        r -= 8;
    }
    if (!(x & 0xf0000000u)) {
        x <<= 4;
        r -= 4;
    }
    if (!(x & 0xc0000000u)) {
        x <<= 2;
        r -= 2;
    }
    if (!(x & 0x80000000u)) {
        x <<= 1;
        r -= 1;
    }
    return r;
}
extern void __bitop_bad_size(void);
static __inline__ void set_bit(int nr, volatile void *addr)
{
    asm volatile (
        "lock; btsl %1,%0"
        : "=m" ((*(volatile long *) addr))
        : "Ir" (nr), "m" ((*(volatile long *) addr)) : "memory");
}
static __inline__ void __set_bit(int nr, volatile void *addr)
{
    asm volatile (
        "btsl %1,%0"
        : "=m" ((*(volatile long *) addr))
        : "Ir" (nr), "m" ((*(volatile long *) addr)) : "memory");
}
static __inline__ void clear_bit(int nr, volatile void *addr)
{
    asm volatile (
        "lock; btrl %1,%0"
        : "=m" ((*(volatile long *) addr))
        : "Ir" (nr), "m" ((*(volatile long *) addr)) : "memory");
}
static __inline__ void __clear_bit(int nr, volatile void *addr)
{
    asm volatile (
        "btrl %1,%0"
        : "=m" ((*(volatile long *) addr))
        : "Ir" (nr), "m" ((*(volatile long *) addr)) : "memory");
}
static __inline__ void __change_bit(int nr, volatile void *addr)
{
    asm volatile (
        "btcl %1,%0"
        : "=m" ((*(volatile long *) addr))
        : "Ir" (nr), "m" ((*(volatile long *) addr)) : "memory");
}
static __inline__ void change_bit(int nr, volatile void *addr)
{
    asm volatile (
        "lock; btcl %1,%0"
        : "=m" ((*(volatile long *) addr))
        : "Ir" (nr), "m" ((*(volatile long *) addr)) : "memory");
}
static __inline__ int test_and_set_bit(int nr, volatile void *addr)
{
    int oldbit;
    asm volatile (
        "lock; btsl %2,%1\n\tsbbl %0,%0"
        : "=r" (oldbit), "=m" ((*(volatile long *) addr))
        : "Ir" (nr), "m" ((*(volatile long *) addr)) : "memory");
    return oldbit;
}
static __inline__ int __test_and_set_bit(int nr, volatile void *addr)
{
    int oldbit;
    asm volatile (
        "btsl %2,%1\n\tsbbl %0,%0"
        : "=r" (oldbit), "=m" ((*(volatile long *) addr))
        : "Ir" (nr), "m" ((*(volatile long *) addr)) : "memory");
    return oldbit;
}
static __inline__ int test_and_clear_bit(int nr, volatile void *addr)
{
    int oldbit;
    asm volatile (
        "lock; btrl %2,%1\n\tsbbl %0,%0"
        : "=r" (oldbit), "=m" ((*(volatile long *) addr))
        : "Ir" (nr), "m" ((*(volatile long *) addr)) : "memory");
    return oldbit;
}
static __inline__ int __test_and_clear_bit(int nr, volatile void *addr)
{
    int oldbit;
    asm volatile (
        "btrl %2,%1\n\tsbbl %0,%0"
        : "=r" (oldbit), "=m" ((*(volatile long *) addr))
        : "Ir" (nr), "m" ((*(volatile long *) addr)) : "memory");
    return oldbit;
}
static __inline__ int __test_and_change_bit(int nr, volatile void *addr)
{
    int oldbit;
    asm volatile (
        "btcl %2,%1\n\tsbbl %0,%0"
        : "=r" (oldbit), "=m" ((*(volatile long *) addr))
        : "Ir" (nr), "m" ((*(volatile long *) addr)) : "memory");
    return oldbit;
}
static __inline__ int test_and_change_bit(int nr, volatile void *addr)
{
    int oldbit;
    asm volatile (
        "lock; btcl %2,%1\n\tsbbl %0,%0"
        : "=r" (oldbit), "=m" ((*(volatile long *) addr))
        : "Ir" (nr), "m" ((*(volatile long *) addr)) : "memory");
    return oldbit;
}
static __inline__ int constant_test_bit(int nr, const volatile void *addr)
{
    return ((1U << (nr & 31)) &
            (((const volatile unsigned int *)addr)[nr >> 5])) != 0;
}
static __inline__ int variable_test_bit(int nr, const volatile void *addr)
{
    int oldbit;
    asm volatile (
        "btl %2,%1\n\tsbbl %0,%0"
        : "=r" (oldbit)
        : "m" ((*(const volatile long *) addr)), "Ir" (nr) : "memory" );
    return oldbit;
}
extern unsigned int __find_first_bit(
    const unsigned long *addr, unsigned int size);
extern unsigned int __find_next_bit(
    const unsigned long *addr, unsigned int size, unsigned int offset);
extern unsigned int __find_first_zero_bit(
    const unsigned long *addr, unsigned int size);
extern unsigned int __find_next_zero_bit(
    const unsigned long *addr, unsigned int size, unsigned int offset);
static __inline__ unsigned int __scanbit(unsigned long val, unsigned long max)
{
    asm ( "bsf %1,%0 ; cmovz %2,%0" : "=&r" (val) : "r" (val), "r" (max) );
    return (unsigned int)val;
}
static __inline__ unsigned int find_first_set_bit(unsigned long word)
{
    asm ( "bsf %1,%0" : "=r" (word) : "r" (word) );
    return (unsigned int)word;
}
static __inline__ int ffs(unsigned long x)
{
    long r;
    asm ( "bsf %1,%0\n\t"
          "jnz 1f\n\t"
          "mov $-1,%0\n"
          "1:" : "=r" (r) : "rm" (x));
    return (int)r+1;
}
static __inline__ int fls(unsigned long x)
{
    long r;
    asm ( "bsr %1,%0\n\t"
          "jnz 1f\n\t"
          "mov $-1,%0\n"
          "1:" : "=r" (r) : "rm" (x));
    return (int)r+1;
}
static __inline__ int generic_fls64(__u64 x)
{
    __u32 h = x >> 32;
    if (h)
        return fls(x) + 32;
    return fls(x);
}
static __inline__ int get_bitmask_order(unsigned int count)
{
    int order;
    order = fls(count);
    return order;
}
static __inline__ int get_count_order(unsigned int count)
{
    int order;
    order = fls(count) - 1;
    if (count & (count - 1))
        order++;
    return order;
}
static __inline__ unsigned int generic_hweight32(unsigned int w)
{
    unsigned int res = (w & 0x55555555) + ((w >> 1) & 0x55555555);
    res = (res & 0x33333333) + ((res >> 2) & 0x33333333);
    res = (res & 0x0F0F0F0F) + ((res >> 4) & 0x0F0F0F0F);
    res = (res & 0x00FF00FF) + ((res >> 8) & 0x00FF00FF);
    return (res & 0x0000FFFF) + ((res >> 16) & 0x0000FFFF);
}
static __inline__ unsigned int generic_hweight16(unsigned int w)
{
    unsigned int res = (w & 0x5555) + ((w >> 1) & 0x5555);
    res = (res & 0x3333) + ((res >> 2) & 0x3333);
    res = (res & 0x0F0F) + ((res >> 4) & 0x0F0F);
    return (res & 0x00FF) + ((res >> 8) & 0x00FF);
}
static __inline__ unsigned int generic_hweight8(unsigned int w)
{
    unsigned int res = (w & 0x55) + ((w >> 1) & 0x55);
    res = (res & 0x33) + ((res >> 2) & 0x33);
    return (res & 0x0F) + ((res >> 4) & 0x0F);
}
static __inline__ unsigned long generic_hweight64(__u64 w)
{
    u64 res;
    res = (w & 0x5555555555555555ul) + ((w >> 1) & 0x5555555555555555ul);
    res = (res & 0x3333333333333333ul) + ((res >> 2) & 0x3333333333333333ul);
    res = (res & 0x0F0F0F0F0F0F0F0Ful) + ((res >> 4) & 0x0F0F0F0F0F0F0F0Ful);
    res = (res & 0x00FF00FF00FF00FFul) + ((res >> 8) & 0x00FF00FF00FF00FFul);
    res = (res & 0x0000FFFF0000FFFFul) + ((res >> 16) & 0x0000FFFF0000FFFFul);
    return (res & 0x00000000FFFFFFFFul) + ((res >> 32) & 0x00000000FFFFFFFFul);
}
static __inline__ unsigned long hweight_long(unsigned long w)
{
    return sizeof(w) == 4 ? generic_hweight32(w) : generic_hweight64(w);
}
static __inline__ __u32 rol32(__u32 word, unsigned int shift)
{
    return (word << shift) | (word >> (32 - shift));
}
static __inline__ __u32 ror32(__u32 word, unsigned int shift)
{
    return (word >> shift) | (word << (32 - shift));
}
extern int __bitmap_empty(const unsigned long *bitmap, int bits);
extern int __bitmap_full(const unsigned long *bitmap, int bits);
extern int __bitmap_equal(const unsigned long *bitmap1,
                 const unsigned long *bitmap2, int bits);
extern void __bitmap_complement(unsigned long *dst, const unsigned long *src,
   int bits);
extern void __bitmap_shift_right(unsigned long *dst,
                        const unsigned long *src, int shift, int bits);
extern void __bitmap_shift_left(unsigned long *dst,
                        const unsigned long *src, int shift, int bits);
extern void __bitmap_and(unsigned long *dst, const unsigned long *bitmap1,
   const unsigned long *bitmap2, int bits);
extern void __bitmap_or(unsigned long *dst, const unsigned long *bitmap1,
   const unsigned long *bitmap2, int bits);
extern void __bitmap_xor(unsigned long *dst, const unsigned long *bitmap1,
   const unsigned long *bitmap2, int bits);
extern void __bitmap_andnot(unsigned long *dst, const unsigned long *bitmap1,
   const unsigned long *bitmap2, int bits);
extern int __bitmap_intersects(const unsigned long *bitmap1,
   const unsigned long *bitmap2, int bits);
extern int __bitmap_subset(const unsigned long *bitmap1,
   const unsigned long *bitmap2, int bits);
extern int __bitmap_weight(const unsigned long *bitmap, int bits);
extern int bitmap_scnprintf(char *buf, unsigned int len,
   const unsigned long *src, int nbits);
extern int bitmap_scnlistprintf(char *buf, unsigned int len,
   const unsigned long *src, int nbits);
extern int bitmap_find_free_region(unsigned long *bitmap, int bits, int order);
extern void bitmap_release_region(unsigned long *bitmap, int pos, int order);
extern int bitmap_allocate_region(unsigned long *bitmap, int pos, int order);
static __inline__ void bitmap_zero(unsigned long *dst, int nbits)
{
 switch (-!__builtin_constant_p(nbits) | (nbits)) { case 0: return ; case 1 ... ((1 << 3) << 3): *dst = 0UL; break; default: (__builtin_memset((dst),(0),(((((nbits)+((1 << 3) << 3)-1)/((1 << 3) << 3)) * sizeof(unsigned long))))); break; };
}
static __inline__ void bitmap_fill(unsigned long *dst, int nbits)
{
 size_t nlongs = (((nbits)+((1 << 3) << 3)-1)/((1 << 3) << 3));
 switch (nlongs) {
 default:
  (__builtin_memset((dst),(-1),((nlongs - 1) * sizeof(unsigned long))));
 case 1:
  dst[nlongs - 1] = ( ((nbits) % ((1 << 3) << 3)) ? (1UL<<((nbits) % ((1 << 3) << 3)))-1 : ~0UL );
  break;
 }
}
static __inline__ void bitmap_copy(unsigned long *dst, const unsigned long *src,
   int nbits)
{
 switch (-!__builtin_constant_p(nbits) | (nbits)) { case 0: return ; case 1 ... ((1 << 3) << 3): *dst = *src; break; default: (__builtin_memcpy((dst),(src),(((((nbits)+((1 << 3) << 3)-1)/((1 << 3) << 3)) * sizeof(unsigned long))))); break; };
}
static __inline__ void bitmap_and(unsigned long *dst, const unsigned long *src1,
   const unsigned long *src2, int nbits)
{
 switch (-!__builtin_constant_p(nbits) | (nbits)) { case 0: return ; case 1 ... ((1 << 3) << 3): *dst = *src1 & *src2; break; default: __bitmap_and(dst, src1, src2, nbits); break; };
}
static __inline__ void bitmap_or(unsigned long *dst, const unsigned long *src1,
   const unsigned long *src2, int nbits)
{
 switch (-!__builtin_constant_p(nbits) | (nbits)) { case 0: return ; case 1 ... ((1 << 3) << 3): *dst = *src1 | *src2; break; default: __bitmap_or(dst, src1, src2, nbits); break; };
}
static __inline__ void bitmap_xor(unsigned long *dst, const unsigned long *src1,
   const unsigned long *src2, int nbits)
{
 switch (-!__builtin_constant_p(nbits) | (nbits)) { case 0: return ; case 1 ... ((1 << 3) << 3): *dst = *src1 ^ *src2; break; default: __bitmap_xor(dst, src1, src2, nbits); break; };
}
static __inline__ void bitmap_andnot(unsigned long *dst, const unsigned long *src1,
   const unsigned long *src2, int nbits)
{
 switch (-!__builtin_constant_p(nbits) | (nbits)) { case 0: return ; case 1 ... ((1 << 3) << 3): *dst = *src1 & ~*src2; break; default: __bitmap_andnot(dst, src1, src2, nbits); break; };
}
static __inline__ void bitmap_complement(unsigned long *dst, const unsigned long *src,
   int nbits)
{
 switch (-!__builtin_constant_p(nbits) | (nbits)) { case 0: return ; case 1 ... ((1 << 3) << 3): *dst = ~*src & ( ((nbits) % ((1 << 3) << 3)) ? (1UL<<((nbits) % ((1 << 3) << 3)))-1 : ~0UL ); break; default: __bitmap_complement(dst, src, nbits); break; };
}
static __inline__ int bitmap_equal(const unsigned long *src1,
   const unsigned long *src2, int nbits)
{
 switch (-!__builtin_constant_p(nbits) | (nbits)) { case 0: return -1; case 1 ... ((1 << 3) << 3): return !((*src1 ^ *src2) & ( ((nbits) % ((1 << 3) << 3)) ? (1UL<<((nbits) % ((1 << 3) << 3)))-1 : ~0UL )); break; default: return __bitmap_equal(src1, src2, nbits); break; };
}
static __inline__ int bitmap_intersects(const unsigned long *src1,
   const unsigned long *src2, int nbits)
{
 switch (-!__builtin_constant_p(nbits) | (nbits)) { case 0: return -1; case 1 ... ((1 << 3) << 3): return ((*src1 & *src2) & ( ((nbits) % ((1 << 3) << 3)) ? (1UL<<((nbits) % ((1 << 3) << 3)))-1 : ~0UL )) != 0; break; default: return __bitmap_intersects(src1, src2, nbits); break; };
}
static __inline__ int bitmap_subset(const unsigned long *src1,
   const unsigned long *src2, int nbits)
{
 switch (-!__builtin_constant_p(nbits) | (nbits)) { case 0: return -1; case 1 ... ((1 << 3) << 3): return !((*src1 & ~*src2) & ( ((nbits) % ((1 << 3) << 3)) ? (1UL<<((nbits) % ((1 << 3) << 3)))-1 : ~0UL )); break; default: return __bitmap_subset(src1, src2, nbits); break; };
}
static __inline__ int bitmap_empty(const unsigned long *src, int nbits)
{
 switch (-!__builtin_constant_p(nbits) | (nbits)) { case 0: return -1; case 1 ... ((1 << 3) << 3): return !(*src & ( ((nbits) % ((1 << 3) << 3)) ? (1UL<<((nbits) % ((1 << 3) << 3)))-1 : ~0UL )); break; default: return __bitmap_empty(src, nbits); break; };
}
static __inline__ int bitmap_full(const unsigned long *src, int nbits)
{
 switch (-!__builtin_constant_p(nbits) | (nbits)) { case 0: return -1; case 1 ... ((1 << 3) << 3): return !(~*src & ( ((nbits) % ((1 << 3) << 3)) ? (1UL<<((nbits) % ((1 << 3) << 3)))-1 : ~0UL )); break; default: return __bitmap_full(src, nbits); break; };
}
static __inline__ int bitmap_weight(const unsigned long *src, int nbits)
{
 return __bitmap_weight(src, nbits);
}
static __inline__ void bitmap_shift_right(unsigned long *dst,
   const unsigned long *src, int n, int nbits)
{
 switch (-!__builtin_constant_p(nbits) | (nbits)) { case 0: return ; case 1 ... ((1 << 3) << 3): *dst = *src >> n; break; default: __bitmap_shift_right(dst, src, n, nbits); break; };
}
static __inline__ void bitmap_shift_left(unsigned long *dst,
   const unsigned long *src, int n, int nbits)
{
 switch (-!__builtin_constant_p(nbits) | (nbits)) { case 0: return ; case 1 ... ((1 << 3) << 3): *dst = (*src << n) & ( ((nbits) % ((1 << 3) << 3)) ? (1UL<<((nbits) % ((1 << 3) << 3)))-1 : ~0UL ); break; default: __bitmap_shift_left(dst, src, n, nbits); break; };
}
void bitmap_long_to_byte(uint8_t *bp, const unsigned long *lp, int nbits);
void bitmap_byte_to_long(unsigned long *lp, const uint8_t *bp, int nbits);
unsigned int get_random(void);
typedef struct cpumask{ unsigned long bits[(((256)+((1 << 3) << 3)-1)/((1 << 3) << 3))]; } cpumask_t;
extern unsigned int nr_cpu_ids;
static __inline__ unsigned int cpumask_check(unsigned int cpu)
{
 do { if ( 0 && (cpu < nr_cpu_ids) ); } while (0);
 return cpu;
}
static __inline__ void cpumask_set_cpu(int cpu, volatile cpumask_t *dstp)
{
 ({ if ( (sizeof(*(dstp->bits)) < 4) ) __bitop_bad_size(); set_bit(cpumask_check(cpu), dstp->bits); });
}
static __inline__ void cpumask_clear_cpu(int cpu, volatile cpumask_t *dstp)
{
 ({ if ( (sizeof(*(dstp->bits)) < 4) ) __bitop_bad_size(); clear_bit(cpumask_check(cpu), dstp->bits); });
}
static __inline__ void cpumask_setall(cpumask_t *dstp)
{
 bitmap_fill(dstp->bits, ((((256)+((1 << 3) << 3)-1)/((1 << 3) << 3)) * ((1 << 3) << 3)));
}
static __inline__ void cpumask_clear(cpumask_t *dstp)
{
 bitmap_zero(dstp->bits, ((((256)+((1 << 3) << 3)-1)/((1 << 3) << 3)) * ((1 << 3) << 3)));
}
static __inline__ int cpumask_test_and_set_cpu(int cpu, cpumask_t *addr)
{
 return ({ if ( (sizeof(*(addr->bits)) < 4) ) __bitop_bad_size(); test_and_set_bit(cpumask_check(cpu), addr->bits); });
}
static __inline__ int cpumask_test_and_clear_cpu(int cpu, cpumask_t *addr)
{
 return ({ if ( (sizeof(*(addr->bits)) < 4) ) __bitop_bad_size(); test_and_clear_bit(cpumask_check(cpu), addr->bits); });
}
static __inline__ void cpumask_and(cpumask_t *dstp, const cpumask_t *src1p,
          const cpumask_t *src2p)
{
 bitmap_and(dstp->bits, src1p->bits, src2p->bits, ((((256)+((1 << 3) << 3)-1)/((1 << 3) << 3)) * ((1 << 3) << 3)));
}
static __inline__ void cpumask_or(cpumask_t *dstp, const cpumask_t *src1p,
         const cpumask_t *src2p)
{
 bitmap_or(dstp->bits, src1p->bits, src2p->bits, ((((256)+((1 << 3) << 3)-1)/((1 << 3) << 3)) * ((1 << 3) << 3)));
}
static __inline__ void cpumask_xor(cpumask_t *dstp, const cpumask_t *src1p,
          const cpumask_t *src2p)
{
 bitmap_xor(dstp->bits, src1p->bits, src2p->bits, ((((256)+((1 << 3) << 3)-1)/((1 << 3) << 3)) * ((1 << 3) << 3)));
}
static __inline__ void cpumask_andnot(cpumask_t *dstp, const cpumask_t *src1p,
      const cpumask_t *src2p)
{
 bitmap_andnot(dstp->bits, src1p->bits, src2p->bits, ((((256)+((1 << 3) << 3)-1)/((1 << 3) << 3)) * ((1 << 3) << 3)));
}
static __inline__ void cpumask_complement(cpumask_t *dstp, const cpumask_t *srcp)
{
 bitmap_complement(dstp->bits, srcp->bits, ((((256)+((1 << 3) << 3)-1)/((1 << 3) << 3)) * ((1 << 3) << 3)));
}
static __inline__ int cpumask_equal(const cpumask_t *src1p,
    const cpumask_t *src2p)
{
 return bitmap_equal(src1p->bits, src2p->bits, nr_cpu_ids);
}
static __inline__ int cpumask_intersects(const cpumask_t *src1p,
         const cpumask_t *src2p)
{
 return bitmap_intersects(src1p->bits, src2p->bits, nr_cpu_ids);
}
static __inline__ int cpumask_subset(const cpumask_t *src1p,
     const cpumask_t *src2p)
{
 return bitmap_subset(src1p->bits, src2p->bits, nr_cpu_ids);
}
static __inline__ int cpumask_empty(const cpumask_t *srcp)
{
 return bitmap_empty(srcp->bits, nr_cpu_ids);
}
static __inline__ int cpumask_full(const cpumask_t *srcp)
{
 return bitmap_full(srcp->bits, nr_cpu_ids);
}
static __inline__ int cpumask_weight(const cpumask_t *srcp)
{
 return bitmap_weight(srcp->bits, nr_cpu_ids);
}
static __inline__ void cpumask_copy(cpumask_t *dstp, const cpumask_t *srcp)
{
 bitmap_copy(dstp->bits, srcp->bits, ((((256)+((1 << 3) << 3)-1)/((1 << 3) << 3)) * ((1 << 3) << 3)));
}
static __inline__ void cpumask_shift_right(cpumask_t *dstp,
           const cpumask_t *srcp, int n)
{
 bitmap_shift_right(dstp->bits, srcp->bits, n, ((((256)+((1 << 3) << 3)-1)/((1 << 3) << 3)) * ((1 << 3) << 3)));
}
static __inline__ void cpumask_shift_left(cpumask_t *dstp,
          const cpumask_t *srcp, int n)
{
 bitmap_shift_left(dstp->bits, srcp->bits, n, ((((256)+((1 << 3) << 3)-1)/((1 << 3) << 3)) * ((1 << 3) << 3)));
}
static __inline__ int cpumask_first(const cpumask_t *srcp)
{
 return ({ int __x = (nr_cpu_ids); int __y = (({ unsigned int r__ = (nr_cpu_ids); unsigned int o__ = (0); switch ( -!__builtin_constant_p(nr_cpu_ids) | r__ ) { case 0: (void)(srcp->bits); break; case 1 ... ((1 << 3) << 3): r__ = o__ + __scanbit(*(const unsigned long *)(srcp->bits) >> o__, r__); break; default: if ( __builtin_constant_p(0) && !o__ ) r__ = __find_first_bit(srcp->bits, r__); else r__ = __find_next_bit(srcp->bits, r__, o__); break; } r__; })); __x < __y ? __x: __y; });
}
static __inline__ int cpumask_next(int n, const cpumask_t *srcp)
{
 if (n != -1)
  cpumask_check(n);
 return ({ int __x = (nr_cpu_ids); int __y = (({ unsigned int r__ = (nr_cpu_ids); unsigned int o__ = (n + 1); switch ( -!__builtin_constant_p(nr_cpu_ids) | r__ ) { case 0: (void)(srcp->bits); break; case 1 ... ((1 << 3) << 3): r__ = o__ + __scanbit(*(const unsigned long *)(srcp->bits) >> o__, r__); break; default: if ( __builtin_constant_p(n + 1) && !o__ ) r__ = __find_first_bit(srcp->bits, r__); else r__ = __find_next_bit(srcp->bits, r__, o__); break; } r__; })); __x < __y ? __x: __y; });
}
static __inline__ int cpumask_last(const cpumask_t *srcp)
{
 int cpu, pcpu = nr_cpu_ids;
 for (cpu = cpumask_first(srcp);
      cpu < nr_cpu_ids;
      cpu = cpumask_next(cpu, srcp))
  pcpu = cpu;
 return pcpu;
}
static __inline__ int cpumask_cycle(int n, const cpumask_t *srcp)
{
    int nxt = cpumask_next(n, srcp);
    if (nxt == nr_cpu_ids)
        nxt = cpumask_first(srcp);
    return nxt;
}
static __inline__ unsigned int cpumask_any(const cpumask_t *srcp)
{
    unsigned int cpu = cpumask_first(srcp);
    unsigned int w = cpumask_weight(srcp);
    if ( w > 1 && cpu < nr_cpu_ids )
        for ( w = get_random() % w; w--; )
        {
            unsigned int next = cpumask_next(cpu, srcp);
            if ( next >= nr_cpu_ids )
                break;
            cpu = next;
        }
    return cpu;
}
extern const unsigned long
 cpu_bit_bitmap[((1 << 3) << 3)+1][(((256)+((1 << 3) << 3)-1)/((1 << 3) << 3))];
static __inline__ const cpumask_t *cpumask_of(unsigned int cpu)
{
 const unsigned long *p = cpu_bit_bitmap[1 + cpu % ((1 << 3) << 3)];
 return (const cpumask_t *)(p - cpu / ((1 << 3) << 3));
}
static __inline__ int cpumask_scnprintf(char *buf, int len,
        const cpumask_t *srcp)
{
 return bitmap_scnprintf(buf, len, srcp->bits, nr_cpu_ids);
}
static __inline__ int cpulist_scnprintf(char *buf, int len,
        const cpumask_t *srcp)
{
 return bitmap_scnlistprintf(buf, len, srcp->bits, nr_cpu_ids);
}
typedef cpumask_t *cpumask_var_t;
static __inline__ bool_t alloc_cpumask_var(cpumask_var_t *mask)
{
 *(void **)mask = _xmalloc(((((256)+((1 << 3) << 3)-1)/((1 << 3) << 3)) * ((1 << 3) << 3)) / 8, sizeof(long));
 return *mask != ((void*)0);
}
static __inline__ bool_t zalloc_cpumask_var(cpumask_var_t *mask)
{
 *(void **)mask = _xzalloc(((((256)+((1 << 3) << 3)-1)/((1 << 3) << 3)) * ((1 << 3) << 3)) / 8, sizeof(long));
 return *mask != ((void*)0);
}
static __inline__ void free_cpumask_var(cpumask_var_t mask)
{
 xfree(mask);
}
extern cpumask_t cpu_possible_map;
extern cpumask_t cpu_online_map;
extern cpumask_t cpu_present_map;
struct xenctl_bitmap;
int cpumask_to_xenctl_bitmap(struct xenctl_bitmap *, const cpumask_t *);
int xenctl_bitmap_to_cpumask(cpumask_var_t *, const struct xenctl_bitmap *);
extern char __per_cpu_start[], __per_cpu_data_end[];
extern unsigned long __per_cpu_offset[256];
void percpu_init_areas(void);
struct iret_context {
    uint64_t rax, r11, rcx, flags, rip, cs, rflags, rsp, ss;
};
struct cpu_user_regs {
    uint64_t r15;
    uint64_t r14;
    uint64_t r13;
    uint64_t r12;
    union { uint64_t rbp, ebp; uint32_t _ebp; };
    union { uint64_t rbx, ebx; uint32_t _ebx; };
    uint64_t r11;
    uint64_t r10;
    uint64_t r9;
    uint64_t r8;
    union { uint64_t rax, eax; uint32_t _eax; };
    union { uint64_t rcx, ecx; uint32_t _ecx; };
    union { uint64_t rdx, edx; uint32_t _edx; };
    union { uint64_t rsi, esi; uint32_t _esi; };
    union { uint64_t rdi, edi; uint32_t _edi; };
    uint32_t error_code;
    uint32_t entry_vector;
    union { uint64_t rip, eip; uint32_t _eip; };
    uint16_t cs, _pad0[1];
    uint8_t saved_upcall_mask;
    uint8_t _pad1[3];
    union { uint64_t rflags, eflags; uint32_t _eflags; };
    union { uint64_t rsp, esp; uint32_t _esp; };
    uint16_t ss, _pad2[3];
    uint16_t es, _pad3[3];
    uint16_t ds, _pad4[3];
    uint16_t fs, _pad5[3];
    uint16_t gs, _pad6[3];
};
typedef struct cpu_user_regs cpu_user_regs_t;
typedef struct { cpu_user_regs_t *p; } __guest_handle_cpu_user_regs_t; typedef struct { const cpu_user_regs_t *p; } __guest_handle_const_cpu_user_regs_t;
struct arch_vcpu_info {
    unsigned long cr2;
    unsigned long pad;
};
typedef struct arch_vcpu_info arch_vcpu_info_t;
typedef unsigned long xen_callback_t;
typedef unsigned long xen_pfn_t;
typedef unsigned long xen_ulong_t;
struct trap_info {
    uint8_t vector;
    uint8_t flags;
    uint16_t cs;
    unsigned long address;
};
typedef struct trap_info trap_info_t;
typedef struct { trap_info_t *p; } __guest_handle_trap_info_t; typedef struct { const trap_info_t *p; } __guest_handle_const_trap_info_t;
typedef uint64_t tsc_timestamp_t;
struct vcpu_guest_context {
    struct { char x[512]; } fpu_ctxt;
    unsigned long flags;
    struct cpu_user_regs user_regs;
    struct trap_info trap_ctxt[256];
    unsigned long ldt_base, ldt_ents;
    unsigned long gdt_frames[16], gdt_ents;
    unsigned long kernel_ss, kernel_sp;
    unsigned long ctrlreg[8];
    unsigned long debugreg[8];
    unsigned long event_callback_eip;
    unsigned long failsafe_callback_eip;
    union {
        unsigned long syscall_callback_eip;
        struct {
            unsigned int event_callback_cs;
            unsigned int failsafe_callback_cs;
        };
    };
    unsigned long vm_assist;
    uint64_t fs_base;
    uint64_t gs_base_kernel;
    uint64_t gs_base_user;
};
typedef struct vcpu_guest_context vcpu_guest_context_t;
typedef struct { vcpu_guest_context_t *p; } __guest_handle_vcpu_guest_context_t; typedef struct { const vcpu_guest_context_t *p; } __guest_handle_const_vcpu_guest_context_t;
struct arch_shared_info {
    unsigned long max_pfn;
    xen_pfn_t pfn_to_mfn_frame_list_list;
    unsigned long nmi_reason;
    uint64_t pad[32];
};
typedef struct arch_shared_info arch_shared_info_t;
typedef struct { char *p; } __guest_handle_char; typedef struct { const char *p; } __guest_handle_const_char;
typedef struct { unsigned char *p; } __guest_handle_uchar; typedef struct { const unsigned char *p; } __guest_handle_const_uchar;
typedef struct { int *p; } __guest_handle_int; typedef struct { const int *p; } __guest_handle_const_int;
typedef struct { unsigned int *p; } __guest_handle_uint; typedef struct { const unsigned int *p; } __guest_handle_const_uint;
typedef struct { void *p; } __guest_handle_void; typedef struct { const void *p; } __guest_handle_const_void;
typedef struct { uint64_t *p; } __guest_handle_uint64_t; typedef struct { const uint64_t *p; } __guest_handle_const_uint64_t;
typedef struct { xen_pfn_t *p; } __guest_handle_xen_pfn_t; typedef struct { const xen_pfn_t *p; } __guest_handle_const_xen_pfn_t;
typedef struct { xen_ulong_t *p; } __guest_handle_xen_ulong_t; typedef struct { const xen_ulong_t *p; } __guest_handle_const_xen_ulong_t;
struct mmuext_op {
    unsigned int cmd;
    union {
        xen_pfn_t mfn;
        unsigned long linear_addr;
    } arg1;
    union {
        unsigned int nr_ents;
        __guest_handle_const_void vcpumask;
        xen_pfn_t src_mfn;
    } arg2;
};
typedef struct mmuext_op mmuext_op_t;
typedef struct { mmuext_op_t *p; } __guest_handle_mmuext_op_t; typedef struct { const mmuext_op_t *p; } __guest_handle_const_mmuext_op_t;
typedef uint16_t domid_t;
struct mmu_update {
    uint64_t ptr;
    uint64_t val;
};
typedef struct mmu_update mmu_update_t;
typedef struct { mmu_update_t *p; } __guest_handle_mmu_update_t; typedef struct { const mmu_update_t *p; } __guest_handle_const_mmu_update_t;
struct multicall_entry {
    unsigned long op, result;
    unsigned long args[6];
};
typedef struct multicall_entry multicall_entry_t;
typedef struct { multicall_entry_t *p; } __guest_handle_multicall_entry_t; typedef struct { const multicall_entry_t *p; } __guest_handle_const_multicall_entry_t;
struct vcpu_time_info {
    uint32_t version;
    uint32_t pad0;
    uint64_t tsc_timestamp;
    uint64_t system_time;
    uint32_t tsc_to_system_mul;
    int8_t tsc_shift;
    int8_t pad1[3];
};
typedef struct vcpu_time_info vcpu_time_info_t;
struct vcpu_info {
    uint8_t evtchn_upcall_pending;
    uint8_t evtchn_upcall_mask;
    xen_ulong_t evtchn_pending_sel;
    struct arch_vcpu_info arch;
    struct vcpu_time_info time;
};
struct shared_info {
    struct vcpu_info vcpu_info[32];
    xen_ulong_t evtchn_pending[sizeof(xen_ulong_t) * 8];
    xen_ulong_t evtchn_mask[sizeof(xen_ulong_t) * 8];
    uint32_t wc_version;
    uint32_t wc_sec;
    uint32_t wc_nsec;
    struct arch_shared_info arch;
};
struct start_info {
    char magic[32];
    unsigned long nr_pages;
    unsigned long shared_info;
    uint32_t flags;
    xen_pfn_t store_mfn;
    uint32_t store_evtchn;
    union {
        struct {
            xen_pfn_t mfn;
            uint32_t evtchn;
        } domU;
        struct {
            uint32_t info_off;
            uint32_t info_size;
        } dom0;
    } console;
    unsigned long pt_base;
    unsigned long nr_pt_frames;
    unsigned long mfn_list;
    unsigned long mod_start;
    unsigned long mod_len;
    int8_t cmd_line[1024];
    unsigned long first_p2m_pfn;
    unsigned long nr_p2m_frames;
};
typedef struct start_info start_info_t;
struct xen_multiboot_mod_list
{
    uint32_t mod_start;
    uint32_t mod_end;
    uint32_t cmdline;
    uint32_t pad;
};
typedef struct dom0_vga_console_info {
    uint8_t video_type;
    union {
        struct {
            uint16_t font_height;
            uint16_t cursor_x, cursor_y;
            uint16_t rows, columns;
        } text_mode_3;
        struct {
            uint16_t width, height;
            uint16_t bytes_per_line;
            uint16_t bits_per_pixel;
            uint32_t lfb_base;
            uint32_t lfb_size;
            uint8_t red_pos, red_size;
            uint8_t green_pos, green_size;
            uint8_t blue_pos, blue_size;
            uint8_t rsvd_pos, rsvd_size;
            uint32_t gbl_caps;
            uint16_t mode_attrs;
        } vesa_lfb;
    } u;
} dom0_vga_console_info_t;
typedef uint8_t xen_domain_handle_t[16];
typedef struct { uint8_t *p; } __guest_handle_uint8; typedef struct { const uint8_t *p; } __guest_handle_const_uint8;
typedef struct { uint16_t *p; } __guest_handle_uint16; typedef struct { const uint16_t *p; } __guest_handle_const_uint16;
typedef struct { uint32_t *p; } __guest_handle_uint32; typedef struct { const uint32_t *p; } __guest_handle_const_uint32;
typedef struct { uint64_t *p; } __guest_handle_uint64; typedef struct { const uint64_t *p; } __guest_handle_const_uint64;
struct xenctl_bitmap {
    __guest_handle_uint8 bitmap;
    uint32_t nr_bits;
};
extern unsigned long xen_virt_end;
extern unsigned long max_pdx;
extern unsigned long pfn_pdx_bottom_mask, ma_va_bottom_mask;
extern unsigned int pfn_pdx_hole_shift;
extern unsigned long pfn_hole_mask;
extern unsigned long pfn_top_mask, ma_top_mask;
extern void pfn_pdx_hole_setup(unsigned long);
extern int __mfn_valid(unsigned long mfn);
static __inline__ unsigned long pfn_to_pdx(unsigned long pfn)
{
    return (pfn & pfn_pdx_bottom_mask) |
           ((pfn & pfn_top_mask) >> pfn_pdx_hole_shift);
}
static __inline__ unsigned long pdx_to_pfn(unsigned long pdx)
{
    return (pdx & pfn_pdx_bottom_mask) |
           ((pdx << pfn_pdx_hole_shift) & pfn_top_mask);
}
static __inline__ unsigned long pfn_to_sdx(unsigned long pfn)
{
    return pfn_to_pdx(pfn) >> (21 -12);
}
static __inline__ unsigned long sdx_to_pfn(unsigned long sdx)
{
    return pdx_to_pfn(sdx << (21 -12));
}
static __inline__ unsigned long __virt_to_maddr(unsigned long va)
{
    do { if ( 0 && (va >= ((((((((((261UL) >> 8) * (0xffff000000000000UL)) | ((261UL) << 39))) + (((1UL) << 39) / 2)) + ((64UL) << 30)) + ((1UL) << 30)) + ((1UL) << 30)))) ); } while (0);
    do { if ( 0 && (va < ((((((262UL) >> 8) * (0xffff000000000000UL)) | ((262UL) << 39))) + (((1UL) << 39) * (511 - 262)))) ); } while (0);
    if ( va >= (((((262UL) >> 8) * (0xffff000000000000UL)) | ((262UL) << 39))) )
        va -= (((((262UL) >> 8) * (0xffff000000000000UL)) | ((262UL) << 39)));
    else
    {
        do { if ( 0 && (va < (((((((((((261UL) >> 8) * (0xffff000000000000UL)) | ((261UL) << 39))) + (((1UL) << 39) / 2)) + ((64UL) << 30)) + ((1UL) << 30)) + ((1UL) << 30))) + ((1UL) << 30))) ); } while (0);
        va += xen_phys_start - ((((((((((261UL) >> 8) * (0xffff000000000000UL)) | ((261UL) << 39))) + (((1UL) << 39) / 2)) + ((64UL) << 30)) + ((1UL) << 30)) + ((1UL) << 30)));
    }
    return (va & ma_va_bottom_mask) |
           ((va << pfn_pdx_hole_shift) & ma_top_mask);
}
static __inline__ void *__maddr_to_virt(unsigned long ma)
{
    do { if ( 0 && (pfn_to_pdx(ma >> 12) < ((((1UL) << 39) * (511 - 262)) >> 12)) ); } while (0);
    return (void *)((((((262UL) >> 8) * (0xffff000000000000UL)) | ((262UL) << 39))) +
                    ((ma & ma_va_bottom_mask) |
                     ((ma & ma_top_mask) >> pfn_pdx_hole_shift)));
}
typedef u64 intpte_t;
typedef struct { intpte_t l1; } l1_pgentry_t;
typedef struct { intpte_t l2; } l2_pgentry_t;
typedef struct { intpte_t l3; } l3_pgentry_t;
typedef struct { intpte_t l4; } l4_pgentry_t;
typedef l4_pgentry_t root_pgentry_t;
static __inline__ l1_pgentry_t l1e_from_paddr(paddr_t pa, unsigned int flags)
{
    do { if ( 0 && ((pa & ~(((1UL << 52)-1) & (~(((1L) << 12)-1)))) == 0) ); } while (0);
    return (l1_pgentry_t) { pa | (((intpte_t)((flags) & ~0xFFF) << 40) | ((flags) & 0xFFF)) };
}
static __inline__ l2_pgentry_t l2e_from_paddr(paddr_t pa, unsigned int flags)
{
    do { if ( 0 && ((pa & ~(((1UL << 52)-1) & (~(((1L) << 12)-1)))) == 0) ); } while (0);
    return (l2_pgentry_t) { pa | (((intpte_t)((flags) & ~0xFFF) << 40) | ((flags) & 0xFFF)) };
}
static __inline__ l3_pgentry_t l3e_from_paddr(paddr_t pa, unsigned int flags)
{
    do { if ( 0 && ((pa & ~(((1UL << 52)-1) & (~(((1L) << 12)-1)))) == 0) ); } while (0);
    return (l3_pgentry_t) { pa | (((intpte_t)((flags) & ~0xFFF) << 40) | ((flags) & 0xFFF)) };
}
static __inline__ l4_pgentry_t l4e_from_paddr(paddr_t pa, unsigned int flags)
{
    do { if ( 0 && ((pa & ~(((1UL << 52)-1) & (~(((1L) << 12)-1)))) == 0) ); } while (0);
    return (l4_pgentry_t) { pa | (((intpte_t)((flags) & ~0xFFF) << 40) | ((flags) & 0xFFF)) };
}
typedef struct { u64 pfn; } pagetable_t;
void clear_page_sse2(void *);
void copy_page_sse2(void *, const void *);
extern root_pgentry_t idle_pg_table[(1<<9)];
extern l2_pgentry_t *compat_idle_pg_table_l2;
extern unsigned int m2p_compat_vstart;
extern l2_pgentry_t l2_xenmap[(1<<9)],
    l2_bootmap[(1<<9)];
extern l3_pgentry_t l3_bootmap[(1<<9)];
extern l2_pgentry_t l2_identmap[4*(1<<9)];
extern l1_pgentry_t l1_identmap[(1<<9)],
    l1_fixmap[(1<<9)];
void paging_init(void);
void *alloc_xen_pagetable(void);
void free_xen_pagetable(void *v);
l1_pgentry_t *virt_to_xen_l1e(unsigned long v);
extern void set_pdx_range(unsigned long smfn, unsigned long emfn);
static __inline__ uint32_t pte_flags_to_cacheattr(uint32_t flags)
{
    return ((flags >> 5) & 4) | ((flags >> 3) & 3);
}
static __inline__ uint32_t cacheattr_to_pte_flags(uint32_t cacheattr)
{
    return ((cacheattr & 4) << 5) | ((cacheattr & 3) << 3);
}
static __inline__ void flush_page_to_ram(unsigned long mfn) {}
static __inline__ bool_t
perms_strictly_increased(uint32_t old_flags, uint32_t new_flags)
{
    uint32_t of = old_flags & ((0x001U)|(0x002U)|(0x004U)|(1U<<23));
    uint32_t nf = new_flags & ((0x001U)|(0x002U)|(0x004U)|(1U<<23));
    of ^= (1U<<23);
    nf ^= (1U<<23);
    return ((of | (of ^ nf)) == nf);
}
struct vcpu;
struct cpu_info {
    struct cpu_user_regs guest_cpu_user_regs;
    unsigned int processor_id;
    struct vcpu *current_vcpu;
    unsigned long per_cpu_offset;
    unsigned long __pad_for_stack_bottom;
};
static __inline__ struct cpu_info *get_cpu_info(void)
{
    unsigned long tos;
    __asm__ ( "and %%rsp,%0" : "=r" (tos) : "0" (~((((1L) << 12) << 3)-1)) );
    return (struct cpu_info *)(tos + (((1L) << 12) << 3)) - 1;
}
extern __typeof__(struct vcpu *) per_cpu__curr_vcpu;
struct intel_mp_floating
{
 char mpf_signature[4];
 unsigned int mpf_physptr;
 unsigned char mpf_length;
 unsigned char mpf_specification;
 unsigned char mpf_checksum;
 unsigned char mpf_feature1;
 unsigned char mpf_feature2;
 unsigned char mpf_feature3;
 unsigned char mpf_feature4;
 unsigned char mpf_feature5;
};
struct mp_config_table
{
 char mpc_signature[4];
 unsigned short mpc_length;
 char mpc_spec;
 char mpc_checksum;
 char mpc_oem[8];
 char mpc_productid[12];
 unsigned int mpc_oemptr;
 unsigned short mpc_oemsize;
 unsigned short mpc_oemcount;
 unsigned int mpc_lapic;
 unsigned int reserved;
};
struct mpc_config_processor
{
 unsigned char mpc_type;
 unsigned char mpc_apicid;
 unsigned char mpc_apicver;
 unsigned char mpc_cpuflag;
 unsigned int mpc_cpufeature;
 unsigned int mpc_featureflag;
 unsigned int mpc_reserved[2];
};
struct mpc_config_bus
{
 unsigned char mpc_type;
 unsigned char mpc_busid;
 unsigned char mpc_bustype[6];
};
struct mpc_config_ioapic
{
 unsigned char mpc_type;
 unsigned char mpc_apicid;
 unsigned char mpc_apicver;
 unsigned char mpc_flags;
 unsigned int mpc_apicaddr;
};
struct mpc_config_intsrc
{
 unsigned char mpc_type;
 unsigned char mpc_irqtype;
 unsigned short mpc_irqflag;
 unsigned char mpc_srcbus;
 unsigned char mpc_srcbusirq;
 unsigned char mpc_dstapic;
 unsigned char mpc_dstirq;
};
enum mp_irq_source_types {
 mp_INT = 0,
 mp_NMI = 1,
 mp_SMI = 2,
 mp_ExtINT = 3
};
struct mpc_config_lintsrc
{
 unsigned char mpc_type;
 unsigned char mpc_irqtype;
 unsigned short mpc_irqflag;
 unsigned char mpc_srcbusid;
 unsigned char mpc_srcbusirq;
 unsigned char mpc_destapic;
 unsigned char mpc_destapiclint;
};
struct mp_config_oemtable
{
 char oem_signature[4];
 unsigned short oem_length;
 char oem_rev;
 char oem_checksum;
 char mpc_oem[8];
};
struct mpc_config_translation
{
        unsigned char mpc_type;
        unsigned char trans_len;
        unsigned char trans_type;
        unsigned char trans_quad;
        unsigned char trans_global;
        unsigned char trans_local;
        unsigned short trans_reserved;
};
enum mp_bustype {
 MP_BUS_ISA = 1,
 MP_BUS_EISA,
 MP_BUS_PCI,
 MP_BUS_MCA,
 MP_BUS_NEC98
};
extern unsigned char mp_bus_id_to_type[260];
extern bool_t def_to_bigsmp;
extern unsigned int boot_cpu_physical_apicid;
extern bool_t smp_found_config;
extern void find_smp_config (void);
extern void get_smp_config (void);
extern unsigned char apic_version [((256) > (4 * 256) ? (256) : (4 * 256))];
extern int mp_irq_entries;
extern struct mpc_config_intsrc mp_irqs [256];
extern int mpc_default_type;
extern unsigned long mp_lapic_addr;
extern bool_t pic_mode;
extern int mp_register_lapic(u32 id, bool_t enabled, bool_t hotplug);
extern void mp_unregister_lapic(uint32_t apic_id, uint32_t cpu);
extern void mp_register_lapic_address (u64 address);
extern void mp_register_ioapic (u8 id, u32 address, u32 gsi_base);
extern void mp_override_legacy_irq (u8 bus_irq, u8 polarity, u8 trigger, u32 gsi);
extern void mp_config_acpi_legacy_irqs (void);
extern int mp_register_gsi (u32 gsi, int edge_level, int active_high_low);
struct physid_mask
{
 unsigned long mask[(((((256) > (4 * 256) ? (256) : (4 * 256)))+((1 << 3) << 3)-1)/((1 << 3) << 3))];
};
typedef struct physid_mask physid_mask_t;
extern physid_mask_t phys_cpu_present_map;
extern void smp_alloc_memory(void);
extern __typeof__(cpumask_var_t) per_cpu__cpu_sibling_mask;
extern __typeof__(cpumask_var_t) per_cpu__cpu_core_mask;
void smp_send_nmi_allbutself(void);
void send_IPI_mask(const cpumask_t *, int vector);
void send_IPI_self(int vector);
extern void (*mtrr_hook) (void);
extern void zap_low_mappings(void);
extern u32 x86_cpu_to_apicid[];
extern void cpu_exit_clear(unsigned int cpu);
extern void cpu_uninit(unsigned int cpu);
int cpu_add(uint32_t apic_id, uint32_t acpi_id, uint32_t pxm);
int hard_smp_processor_id(void);
void __stop_this_cpu(void);
extern void smp_send_stop(void);
extern void smp_send_event_check_mask(const cpumask_t *mask);
extern void smp_send_state_dump(unsigned int cpu);
extern void smp_prepare_cpus(unsigned int max_cpus);
extern void smp_cpus_done(void);
extern void smp_call_function(
    void (*func) (void *info),
    void *info,
    int wait);
extern void on_selected_cpus(
    const cpumask_t *selected,
    void (*func) (void *info),
    void *info,
    int wait);
void smp_prepare_boot_cpu(void);
static __inline__ void on_each_cpu(
    void (*func) (void *info),
    void *info,
    int wait)
{
    on_selected_cpus(&cpu_online_map, func, info, wait);
}
void smp_call_function_interrupt(void);
void smp_send_call_function_mask(const cpumask_t *mask);
int alloc_cpu_id(void);
extern void *stack_base[256];
struct desc_struct {
    u32 a, b;
};
typedef struct {
    u64 a, b;
} idt_entry_t;
static __inline__ void _write_gate_lower(volatile idt_entry_t *gate,
                                     const idt_entry_t *new)
{
    do { if ( 0 && (gate->b == new->b) ); } while (0);
    gate->a = new->a;
}
static __inline__ void _set_gate_lower(idt_entry_t *gate, unsigned long type,
                                   unsigned long dpl, void *addr)
{
    idt_entry_t idte;
    idte.b = gate->b;
    idte.a =
        (((unsigned long)(addr) & 0xFFFF0000UL) << 32) |
        ((unsigned long)(dpl) << 45) |
        ((unsigned long)(type) << 40) |
        ((unsigned long)(addr) & 0xFFFFUL) |
        ((unsigned long)0xe008 << 16) |
        (1UL << 47);
    _write_gate_lower(gate, &idte);
}
static __inline__ void _update_gate_addr_lower(idt_entry_t *gate, void *addr)
{
    idt_entry_t idte;
    idte.a = gate->a;
    idte.b = ((unsigned long)(addr) >> 32);
    idte.a &= 0x0000FFFFFFFF0000ULL;
    idte.a |= (((unsigned long)(addr) & 0xFFFF0000UL) << 32) |
        ((unsigned long)(addr) & 0xFFFFUL);
    _write_gate_lower(gate, &idte);
}
struct desc_ptr {
 unsigned short limit;
 unsigned long base;
} __attribute__((__packed__)) ;
extern struct desc_struct boot_cpu_gdt_table[];
extern __typeof__(struct desc_struct *) per_cpu__gdt_table;
extern struct desc_struct boot_cpu_compat_gdt_table[];
extern __typeof__(struct desc_struct *) per_cpu__compat_gdt_table;
extern void set_intr_gate(unsigned int irq, void * addr);
extern void load_TR(void);
struct domain;
struct vcpu;
struct cpuinfo_x86 {
    __u8 x86;
    __u8 x86_vendor;
    __u8 x86_model;
    __u8 x86_mask;
    int cpuid_level;
    unsigned int x86_capability[8];
    char x86_vendor_id[16];
    char x86_model_id[64];
    int x86_cache_size;
    int x86_cache_alignment;
    int x86_power;
    __u32 x86_max_cores;
    __u32 booted_cores;
    __u32 x86_num_siblings;
    __u32 apicid;
    int phys_proc_id;
    int cpu_core_id;
    int compute_unit_id;
    unsigned short x86_clflush_size;
} __attribute__((__aligned__((1 << (7)))));
extern struct cpuinfo_x86 boot_cpu_data;
extern struct cpuinfo_x86 cpu_data[];
extern void set_cpuid_faulting(bool_t enable);
extern u64 host_pat;
extern bool_t opt_cpu_info;
extern unsigned int paddr_bits;
extern void identify_cpu(struct cpuinfo_x86 *);
extern void setup_clear_cpu_cap(unsigned int);
extern void print_cpu_info(unsigned int cpu);
extern unsigned int init_intel_cacheinfo(struct cpuinfo_x86 *c);
extern void dodgy_tsc(void);
extern void detect_extended_topology(struct cpuinfo_x86 *c);
extern void detect_ht(struct cpuinfo_x86 *c);
static __inline__ void cpuid_count(
    int op,
    int count,
    unsigned int *eax,
    unsigned int *ebx,
    unsigned int *ecx,
    unsigned int *edx)
{
    asm volatile ( "cpuid"
          : "=a" (*eax), "=b" (*ebx), "=c" (*ecx), "=d" (*edx)
          : "0" (op), "c" (count) );
}
static __inline__ __attribute__ ((always_inline)) unsigned int cpuid_eax(unsigned int op)
{
    unsigned int eax;
    asm volatile ( "cpuid"
          : "=a" (eax)
          : "0" (op)
          : "bx", "cx", "dx" );
    return eax;
}
static __inline__ __attribute__ ((always_inline)) unsigned int cpuid_ebx(unsigned int op)
{
    unsigned int eax, ebx;
    asm volatile ( "cpuid"
          : "=a" (eax), "=b" (ebx)
          : "0" (op)
          : "cx", "dx" );
    return ebx;
}
static __inline__ __attribute__ ((always_inline)) unsigned int cpuid_ecx(unsigned int op)
{
    unsigned int eax, ecx;
    asm volatile ( "cpuid"
          : "=a" (eax), "=c" (ecx)
          : "0" (op)
          : "bx", "dx" );
    return ecx;
}
static __inline__ __attribute__ ((always_inline)) unsigned int cpuid_edx(unsigned int op)
{
    unsigned int eax, edx;
    asm volatile ( "cpuid"
          : "=a" (eax), "=d" (edx)
          : "0" (op)
          : "bx", "cx" );
    return edx;
}
static __inline__ unsigned long read_cr0(void)
{
    unsigned long cr0;
    asm volatile ( "mov %%cr0,%0\n\t" : "=r" (cr0) );
    return cr0;
}
static __inline__ void write_cr0(unsigned long val)
{
    asm volatile ( "mov %0,%%cr0" : : "r" ((unsigned long)val) );
}
static __inline__ unsigned long read_cr2(void)
{
    unsigned long cr2;
    asm volatile ( "mov %%cr2,%0\n\t" : "=r" (cr2) );
    return cr2;
}
extern __typeof__(unsigned long) per_cpu__cr4;
static __inline__ unsigned long read_cr4(void)
{
    return (*({ unsigned long __ptr; __asm__ ("" : "=r"(__ptr) : "0"(&per_cpu__cr4)); (typeof(&per_cpu__cr4)) (__ptr + (get_cpu_info()->per_cpu_offset)); }));
}
static __inline__ void write_cr4(unsigned long val)
{
    (*({ unsigned long __ptr; __asm__ ("" : "=r"(__ptr) : "0"(&per_cpu__cr4)); (typeof(&per_cpu__cr4)) (__ptr + (get_cpu_info()->per_cpu_offset)); })) = val;
    asm volatile ( "mov %0,%%cr4" : : "r" (val) );
}
static __inline__ void clts(void)
{
    asm volatile ( "clts" );
}
static __inline__ void stts(void)
{
    write_cr0(0x00000008|read_cr0());
}
extern unsigned long mmu_cr4_features;
static __inline__ __attribute__ ((always_inline)) void set_in_cr4 (unsigned long mask)
{
    mmu_cr4_features |= mask;
    write_cr4(read_cr4() | mask);
}
static __inline__ __attribute__ ((always_inline)) void clear_in_cr4 (unsigned long mask)
{
    mmu_cr4_features &= ~mask;
    write_cr4(read_cr4() & ~mask);
}
static __inline__ void sync_core(void)
{
    int tmp;
    asm volatile (
        "cpuid"
        : "=a" (tmp)
        : "0" (1)
        : "ebx","ecx","edx","memory" );
}
static __inline__ __attribute__ ((always_inline)) void __monitor(const void *eax, unsigned long ecx,
                                    unsigned long edx)
{
    asm volatile (
        ".byte 0x0f,0x01,0xc8;"
        : : "a" (eax), "c" (ecx), "d"(edx) );
}
static __inline__ __attribute__ ((always_inline)) void __mwait(unsigned long eax, unsigned long ecx)
{
    asm volatile (
        ".byte 0x0f,0x01,0xc9;"
        : : "a" (eax), "c" (ecx) );
}
struct tss_struct {
    unsigned short back_link,__blh;
    union { u64 rsp0, esp0; };
    union { u64 rsp1, esp1; };
    union { u64 rsp2, esp2; };
    u64 reserved1;
    u64 ist[7];
    u64 reserved2;
    u16 reserved3;
    u16 bitmap;
    u8 __cacheline_filler[24];
} __attribute__((__aligned__((1 << (7))))) __attribute__((packed));
static __inline__ __attribute__ ((always_inline)) void set_ist(idt_entry_t *idt, unsigned long ist)
{
    do { if ( 0 && (ist <= 3UL) ); } while (0);
    idt->a = (idt->a & ~(7UL << 32)) | (ist << 32);
}
extern idt_entry_t idt_table[];
extern idt_entry_t *idt_tables[];
extern __typeof__(struct tss_struct) per_cpu__init_tss;
extern void init_int80_direct_trap(struct vcpu *v);
extern int gpf_emulate_4gb(struct cpu_user_regs *regs);
extern void write_ptbase(struct vcpu *v);
void destroy_gdt(struct vcpu *d);
long set_gdt(struct vcpu *d,
             unsigned long *frames,
             unsigned int entries);
long set_debugreg(struct vcpu *p, int reg, unsigned long value);
static __inline__ __attribute__ ((always_inline)) void rep_nop(void)
{
    asm volatile ( "rep;nop" : : : "memory" );
}
void show_stack(struct cpu_user_regs *regs);
void show_stack_overflow(unsigned int cpu, const struct cpu_user_regs *regs);
void show_registers(struct cpu_user_regs *regs);
void show_execution_state(struct cpu_user_regs *regs);
void show_page_walk(unsigned long addr);
void fatal_trap(int trapnr, struct cpu_user_regs *regs);
void compat_show_guest_stack(struct vcpu *, struct cpu_user_regs *, int lines);
extern void mtrr_ap_init(void);
extern void mtrr_bp_init(void);
void mcheck_init(struct cpuinfo_x86 *c, bool_t bsp);
void divide_error(void); void do_divide_error(struct cpu_user_regs *regs);
void debug(void); void do_debug(struct cpu_user_regs *regs);
void nmi(void); void do_nmi(struct cpu_user_regs *regs);
void nmi_crash(void); void do_nmi_crash(struct cpu_user_regs *regs);
void int3(void); void do_int3(struct cpu_user_regs *regs);
void overflow(void); void do_overflow(struct cpu_user_regs *regs);
void bounds(void); void do_bounds(struct cpu_user_regs *regs);
void invalid_op(void); void do_invalid_op(struct cpu_user_regs *regs);
void device_not_available(void); void do_device_not_available(struct cpu_user_regs *regs);
void coprocessor_segment_overrun(void); void do_coprocessor_segment_overrun(struct cpu_user_regs *regs);
void invalid_TSS(void); void do_invalid_TSS(struct cpu_user_regs *regs);
void segment_not_present(void); void do_segment_not_present(struct cpu_user_regs *regs);
void stack_segment(void); void do_stack_segment(struct cpu_user_regs *regs);
void general_protection(void); void do_general_protection(struct cpu_user_regs *regs);
void page_fault(void); void do_page_fault(struct cpu_user_regs *regs);
void coprocessor_error(void); void do_coprocessor_error(struct cpu_user_regs *regs);
void simd_coprocessor_error(void); void do_simd_coprocessor_error(struct cpu_user_regs *regs);
void machine_check(void); void do_machine_check(struct cpu_user_regs *regs);
void alignment_check(void); void do_alignment_check(struct cpu_user_regs *regs);
void spurious_interrupt_bug(void); void do_spurious_interrupt_bug(struct cpu_user_regs *regs);
void trap_nop(void);
void enable_nmis(void);
void syscall_enter(void);
void sysenter_entry(void);
void sysenter_eflags_saved(void);
void compat_hypercall(void);
void int80_direct_trap(void);
extern int hypercall(void);
int cpuid_hypervisor_leaves( uint32_t idx, uint32_t sub_idx,
          uint32_t *eax, uint32_t *ebx, uint32_t *ecx, uint32_t *edx);
int rdmsr_hypervisor_regs(uint32_t idx, uint64_t *val);
int wrmsr_hypervisor_regs(uint32_t idx, uint64_t val);
void microcode_set_module(unsigned int);
int microcode_update(__guest_handle_const_void, unsigned long len);
int microcode_resume_cpu(int cpu);
void pv_cpuid(struct cpu_user_regs *regs);
static __inline__ void prefetch(const void *x) {;}
static __inline__ void prefetchw(const void *x) {;}
void ret_from_intr(void);
struct vcpu;
int setup_compat_arg_xlat(struct vcpu *v);
void free_compat_arg_xlat(struct vcpu *v);
extern void *xlat_malloc(unsigned long *xlat_page_current, size_t size);
unsigned long copy_to_user(void *to, const void *from, unsigned len);
unsigned long clear_user(void *to, unsigned len);
unsigned long copy_from_user(void *to, const void *from, unsigned len);
unsigned long __copy_to_user_ll(void *to, const void *from, unsigned n);
unsigned long __copy_from_user_ll(void *to, const void *from, unsigned n);
extern long __get_user_bad(void);
extern void __put_user_bad(void);
struct __large_struct { unsigned long buf[100]; };
static __inline__ __attribute__ ((always_inline)) unsigned long
__copy_to_user(void *to, const void *from, unsigned long n)
{
    if (__builtin_constant_p(n)) {
        unsigned long ret;
        switch (n) {
        case 1:
            do { ret = 0; switch (1) { case 1: __asm__ __volatile__( "1:	mov""b"" %""b""1,%2\n" "2:\n" ".section .fixup,\"ax\"\n" "3:	mov %3,%0\n" "	jmp 2b\n" ".previous\n" " .section .ex_table" "" ",\"a\"\n" " .balign 4\n" " .long " "1b" "-." ", " "3b" "-." "\n" " .previous\n" : "=r"(ret) : "iq" (*(const u8 *)from), "m"((*(const struct __large_struct *)((u8 *)to))), "i"(1), "0"(ret));break; case 2: __asm__ __volatile__( "1:	mov""w"" %""w""1,%2\n" "2:\n" ".section .fixup,\"ax\"\n" "3:	mov %3,%0\n" "	jmp 2b\n" ".previous\n" " .section .ex_table" "" ",\"a\"\n" " .balign 4\n" " .long " "1b" "-." ", " "3b" "-." "\n" " .previous\n" : "=r"(ret) : "ir" (*(const u8 *)from), "m"((*(const struct __large_struct *)((u8 *)to))), "i"(1), "0"(ret));break; case 4: __asm__ __volatile__( "1:	mov""l"" %""k""1,%2\n" "2:\n" ".section .fixup,\"ax\"\n" "3:	mov %3,%0\n" "	jmp 2b\n" ".previous\n" " .section .ex_table" "" ",\"a\"\n" " .balign 4\n" " .long " "1b" "-." ", " "3b" "-." "\n" " .previous\n" : "=r"(ret) : "ir" (*(const u8 *)from), "m"((*(const struct __large_struct *)((u8 *)to))), "i"(1), "0"(ret));break; case 8: __asm__ __volatile__( "1:	mov""q"" %""""1,%2\n" "2:\n" ".section .fixup,\"ax\"\n" "3:	mov %3,%0\n" "	jmp 2b\n" ".previous\n" " .section .ex_table" "" ",\"a\"\n" " .balign 4\n" " .long " "1b" "-." ", " "3b" "-." "\n" " .previous\n" : "=r"(ret) : "ir" (*(const u8 *)from), "m"((*(const struct __large_struct *)((u8 *)to))), "i"(1), "0"(ret));break; default: __put_user_bad(); } } while (0);
            return ret;
        case 2:
            do { ret = 0; switch (2) { case 1: __asm__ __volatile__( "1:	mov""b"" %""b""1,%2\n" "2:\n" ".section .fixup,\"ax\"\n" "3:	mov %3,%0\n" "	jmp 2b\n" ".previous\n" " .section .ex_table" "" ",\"a\"\n" " .balign 4\n" " .long " "1b" "-." ", " "3b" "-." "\n" " .previous\n" : "=r"(ret) : "iq" (*(const u16 *)from), "m"((*(const struct __large_struct *)((u16 *)to))), "i"(2), "0"(ret));break; case 2: __asm__ __volatile__( "1:	mov""w"" %""w""1,%2\n" "2:\n" ".section .fixup,\"ax\"\n" "3:	mov %3,%0\n" "	jmp 2b\n" ".previous\n" " .section .ex_table" "" ",\"a\"\n" " .balign 4\n" " .long " "1b" "-." ", " "3b" "-." "\n" " .previous\n" : "=r"(ret) : "ir" (*(const u16 *)from), "m"((*(const struct __large_struct *)((u16 *)to))), "i"(2), "0"(ret));break; case 4: __asm__ __volatile__( "1:	mov""l"" %""k""1,%2\n" "2:\n" ".section .fixup,\"ax\"\n" "3:	mov %3,%0\n" "	jmp 2b\n" ".previous\n" " .section .ex_table" "" ",\"a\"\n" " .balign 4\n" " .long " "1b" "-." ", " "3b" "-." "\n" " .previous\n" : "=r"(ret) : "ir" (*(const u16 *)from), "m"((*(const struct __large_struct *)((u16 *)to))), "i"(2), "0"(ret));break; case 8: __asm__ __volatile__( "1:	mov""q"" %""""1,%2\n" "2:\n" ".section .fixup,\"ax\"\n" "3:	mov %3,%0\n" "	jmp 2b\n" ".previous\n" " .section .ex_table" "" ",\"a\"\n" " .balign 4\n" " .long " "1b" "-." ", " "3b" "-." "\n" " .previous\n" : "=r"(ret) : "ir" (*(const u16 *)from), "m"((*(const struct __large_struct *)((u16 *)to))), "i"(2), "0"(ret));break; default: __put_user_bad(); } } while (0);
            return ret;
        case 4:
            do { ret = 0; switch (4) { case 1: __asm__ __volatile__( "1:	mov""b"" %""b""1,%2\n" "2:\n" ".section .fixup,\"ax\"\n" "3:	mov %3,%0\n" "	jmp 2b\n" ".previous\n" " .section .ex_table" "" ",\"a\"\n" " .balign 4\n" " .long " "1b" "-." ", " "3b" "-." "\n" " .previous\n" : "=r"(ret) : "iq" (*(const u32 *)from), "m"((*(const struct __large_struct *)((u32 *)to))), "i"(4), "0"(ret));break; case 2: __asm__ __volatile__( "1:	mov""w"" %""w""1,%2\n" "2:\n" ".section .fixup,\"ax\"\n" "3:	mov %3,%0\n" "	jmp 2b\n" ".previous\n" " .section .ex_table" "" ",\"a\"\n" " .balign 4\n" " .long " "1b" "-." ", " "3b" "-." "\n" " .previous\n" : "=r"(ret) : "ir" (*(const u32 *)from), "m"((*(const struct __large_struct *)((u32 *)to))), "i"(4), "0"(ret));break; case 4: __asm__ __volatile__( "1:	mov""l"" %""k""1,%2\n" "2:\n" ".section .fixup,\"ax\"\n" "3:	mov %3,%0\n" "	jmp 2b\n" ".previous\n" " .section .ex_table" "" ",\"a\"\n" " .balign 4\n" " .long " "1b" "-." ", " "3b" "-." "\n" " .previous\n" : "=r"(ret) : "ir" (*(const u32 *)from), "m"((*(const struct __large_struct *)((u32 *)to))), "i"(4), "0"(ret));break; case 8: __asm__ __volatile__( "1:	mov""q"" %""""1,%2\n" "2:\n" ".section .fixup,\"ax\"\n" "3:	mov %3,%0\n" "	jmp 2b\n" ".previous\n" " .section .ex_table" "" ",\"a\"\n" " .balign 4\n" " .long " "1b" "-." ", " "3b" "-." "\n" " .previous\n" : "=r"(ret) : "ir" (*(const u32 *)from), "m"((*(const struct __large_struct *)((u32 *)to))), "i"(4), "0"(ret));break; default: __put_user_bad(); } } while (0);
            return ret;
        case 8:
            do { ret = 0; switch (8) { case 1: __asm__ __volatile__( "1:	mov""b"" %""b""1,%2\n" "2:\n" ".section .fixup,\"ax\"\n" "3:	mov %3,%0\n" "	jmp 2b\n" ".previous\n" " .section .ex_table" "" ",\"a\"\n" " .balign 4\n" " .long " "1b" "-." ", " "3b" "-." "\n" " .previous\n" : "=r"(ret) : "iq" (*(const u64 *)from), "m"((*(const struct __large_struct *)((u64 *)to))), "i"(8), "0"(ret));break; case 2: __asm__ __volatile__( "1:	mov""w"" %""w""1,%2\n" "2:\n" ".section .fixup,\"ax\"\n" "3:	mov %3,%0\n" "	jmp 2b\n" ".previous\n" " .section .ex_table" "" ",\"a\"\n" " .balign 4\n" " .long " "1b" "-." ", " "3b" "-." "\n" " .previous\n" : "=r"(ret) : "ir" (*(const u64 *)from), "m"((*(const struct __large_struct *)((u64 *)to))), "i"(8), "0"(ret));break; case 4: __asm__ __volatile__( "1:	mov""l"" %""k""1,%2\n" "2:\n" ".section .fixup,\"ax\"\n" "3:	mov %3,%0\n" "	jmp 2b\n" ".previous\n" " .section .ex_table" "" ",\"a\"\n" " .balign 4\n" " .long " "1b" "-." ", " "3b" "-." "\n" " .previous\n" : "=r"(ret) : "ir" (*(const u64 *)from), "m"((*(const struct __large_struct *)((u64 *)to))), "i"(8), "0"(ret));break; case 8: __asm__ __volatile__( "1:	mov""q"" %""""1,%2\n" "2:\n" ".section .fixup,\"ax\"\n" "3:	mov %3,%0\n" "	jmp 2b\n" ".previous\n" " .section .ex_table" "" ",\"a\"\n" " .balign 4\n" " .long " "1b" "-." ", " "3b" "-." "\n" " .previous\n" : "=r"(ret) : "ir" (*(const u64 *)from), "m"((*(const struct __large_struct *)((u64 *)to))), "i"(8), "0"(ret));break; default: __put_user_bad(); } } while (0);
            return ret;
        }
    }
    return __copy_to_user_ll(to, from, n);
}
static __inline__ __attribute__ ((always_inline)) unsigned long
__copy_from_user(void *to, const void *from, unsigned long n)
{
    if (__builtin_constant_p(n)) {
        unsigned long ret;
        switch (n) {
        case 1:
            do { ret = 0; switch (1) { case 1: __asm__ __volatile__( "1:	mov""b"" %2,%""b""1\n" "2:\n" ".section .fixup,\"ax\"\n" "3:	mov %3,%0\n" "	xor""b"" %""b""1,%""b""1\n" "	jmp 2b\n" ".previous\n" " .section .ex_table" "" ",\"a\"\n" " .balign 4\n" " .long " "1b" "-." ", " "3b" "-." "\n" " .previous\n" : "=r"(ret), "=q" (*(u8 *)to) : "m"((*(const struct __large_struct *)(from))), "i"(1), "0"(ret));break; case 2: __asm__ __volatile__( "1:	mov""w"" %2,%""w""1\n" "2:\n" ".section .fixup,\"ax\"\n" "3:	mov %3,%0\n" "	xor""w"" %""w""1,%""w""1\n" "	jmp 2b\n" ".previous\n" " .section .ex_table" "" ",\"a\"\n" " .balign 4\n" " .long " "1b" "-." ", " "3b" "-." "\n" " .previous\n" : "=r"(ret), "=r" (*(u8 *)to) : "m"((*(const struct __large_struct *)(from))), "i"(1), "0"(ret));break; case 4: __asm__ __volatile__( "1:	mov""l"" %2,%""k""1\n" "2:\n" ".section .fixup,\"ax\"\n" "3:	mov %3,%0\n" "	xor""l"" %""k""1,%""k""1\n" "	jmp 2b\n" ".previous\n" " .section .ex_table" "" ",\"a\"\n" " .balign 4\n" " .long " "1b" "-." ", " "3b" "-." "\n" " .previous\n" : "=r"(ret), "=r" (*(u8 *)to) : "m"((*(const struct __large_struct *)(from))), "i"(1), "0"(ret));break; case 8: __asm__ __volatile__( "1:	mov""q"" %2,%""""1\n" "2:\n" ".section .fixup,\"ax\"\n" "3:	mov %3,%0\n" "	xor""q"" %""""1,%""""1\n" "	jmp 2b\n" ".previous\n" " .section .ex_table" "" ",\"a\"\n" " .balign 4\n" " .long " "1b" "-." ", " "3b" "-." "\n" " .previous\n" : "=r"(ret), "=r" (*(u8 *)to) : "m"((*(const struct __large_struct *)(from))), "i"(1), "0"(ret)); break; default: __get_user_bad(); } } while (0);
            return ret;
        case 2:
            do { ret = 0; switch (2) { case 1: __asm__ __volatile__( "1:	mov""b"" %2,%""b""1\n" "2:\n" ".section .fixup,\"ax\"\n" "3:	mov %3,%0\n" "	xor""b"" %""b""1,%""b""1\n" "	jmp 2b\n" ".previous\n" " .section .ex_table" "" ",\"a\"\n" " .balign 4\n" " .long " "1b" "-." ", " "3b" "-." "\n" " .previous\n" : "=r"(ret), "=q" (*(u16 *)to) : "m"((*(const struct __large_struct *)(from))), "i"(2), "0"(ret));break; case 2: __asm__ __volatile__( "1:	mov""w"" %2,%""w""1\n" "2:\n" ".section .fixup,\"ax\"\n" "3:	mov %3,%0\n" "	xor""w"" %""w""1,%""w""1\n" "	jmp 2b\n" ".previous\n" " .section .ex_table" "" ",\"a\"\n" " .balign 4\n" " .long " "1b" "-." ", " "3b" "-." "\n" " .previous\n" : "=r"(ret), "=r" (*(u16 *)to) : "m"((*(const struct __large_struct *)(from))), "i"(2), "0"(ret));break; case 4: __asm__ __volatile__( "1:	mov""l"" %2,%""k""1\n" "2:\n" ".section .fixup,\"ax\"\n" "3:	mov %3,%0\n" "	xor""l"" %""k""1,%""k""1\n" "	jmp 2b\n" ".previous\n" " .section .ex_table" "" ",\"a\"\n" " .balign 4\n" " .long " "1b" "-." ", " "3b" "-." "\n" " .previous\n" : "=r"(ret), "=r" (*(u16 *)to) : "m"((*(const struct __large_struct *)(from))), "i"(2), "0"(ret));break; case 8: __asm__ __volatile__( "1:	mov""q"" %2,%""""1\n" "2:\n" ".section .fixup,\"ax\"\n" "3:	mov %3,%0\n" "	xor""q"" %""""1,%""""1\n" "	jmp 2b\n" ".previous\n" " .section .ex_table" "" ",\"a\"\n" " .balign 4\n" " .long " "1b" "-." ", " "3b" "-." "\n" " .previous\n" : "=r"(ret), "=r" (*(u16 *)to) : "m"((*(const struct __large_struct *)(from))), "i"(2), "0"(ret)); break; default: __get_user_bad(); } } while (0);
            return ret;
        case 4:
            do { ret = 0; switch (4) { case 1: __asm__ __volatile__( "1:	mov""b"" %2,%""b""1\n" "2:\n" ".section .fixup,\"ax\"\n" "3:	mov %3,%0\n" "	xor""b"" %""b""1,%""b""1\n" "	jmp 2b\n" ".previous\n" " .section .ex_table" "" ",\"a\"\n" " .balign 4\n" " .long " "1b" "-." ", " "3b" "-." "\n" " .previous\n" : "=r"(ret), "=q" (*(u32 *)to) : "m"((*(const struct __large_struct *)(from))), "i"(4), "0"(ret));break; case 2: __asm__ __volatile__( "1:	mov""w"" %2,%""w""1\n" "2:\n" ".section .fixup,\"ax\"\n" "3:	mov %3,%0\n" "	xor""w"" %""w""1,%""w""1\n" "	jmp 2b\n" ".previous\n" " .section .ex_table" "" ",\"a\"\n" " .balign 4\n" " .long " "1b" "-." ", " "3b" "-." "\n" " .previous\n" : "=r"(ret), "=r" (*(u32 *)to) : "m"((*(const struct __large_struct *)(from))), "i"(4), "0"(ret));break; case 4: __asm__ __volatile__( "1:	mov""l"" %2,%""k""1\n" "2:\n" ".section .fixup,\"ax\"\n" "3:	mov %3,%0\n" "	xor""l"" %""k""1,%""k""1\n" "	jmp 2b\n" ".previous\n" " .section .ex_table" "" ",\"a\"\n" " .balign 4\n" " .long " "1b" "-." ", " "3b" "-." "\n" " .previous\n" : "=r"(ret), "=r" (*(u32 *)to) : "m"((*(const struct __large_struct *)(from))), "i"(4), "0"(ret));break; case 8: __asm__ __volatile__( "1:	mov""q"" %2,%""""1\n" "2:\n" ".section .fixup,\"ax\"\n" "3:	mov %3,%0\n" "	xor""q"" %""""1,%""""1\n" "	jmp 2b\n" ".previous\n" " .section .ex_table" "" ",\"a\"\n" " .balign 4\n" " .long " "1b" "-." ", " "3b" "-." "\n" " .previous\n" : "=r"(ret), "=r" (*(u32 *)to) : "m"((*(const struct __large_struct *)(from))), "i"(4), "0"(ret)); break; default: __get_user_bad(); } } while (0);
            return ret;
        case 8:
            do { ret = 0; switch (8) { case 1: __asm__ __volatile__( "1:	mov""b"" %2,%""b""1\n" "2:\n" ".section .fixup,\"ax\"\n" "3:	mov %3,%0\n" "	xor""b"" %""b""1,%""b""1\n" "	jmp 2b\n" ".previous\n" " .section .ex_table" "" ",\"a\"\n" " .balign 4\n" " .long " "1b" "-." ", " "3b" "-." "\n" " .previous\n" : "=r"(ret), "=q" (*(u64*)to) : "m"((*(const struct __large_struct *)(from))), "i"(8), "0"(ret));break; case 2: __asm__ __volatile__( "1:	mov""w"" %2,%""w""1\n" "2:\n" ".section .fixup,\"ax\"\n" "3:	mov %3,%0\n" "	xor""w"" %""w""1,%""w""1\n" "	jmp 2b\n" ".previous\n" " .section .ex_table" "" ",\"a\"\n" " .balign 4\n" " .long " "1b" "-." ", " "3b" "-." "\n" " .previous\n" : "=r"(ret), "=r" (*(u64*)to) : "m"((*(const struct __large_struct *)(from))), "i"(8), "0"(ret));break; case 4: __asm__ __volatile__( "1:	mov""l"" %2,%""k""1\n" "2:\n" ".section .fixup,\"ax\"\n" "3:	mov %3,%0\n" "	xor""l"" %""k""1,%""k""1\n" "	jmp 2b\n" ".previous\n" " .section .ex_table" "" ",\"a\"\n" " .balign 4\n" " .long " "1b" "-." ", " "3b" "-." "\n" " .previous\n" : "=r"(ret), "=r" (*(u64*)to) : "m"((*(const struct __large_struct *)(from))), "i"(8), "0"(ret));break; case 8: __asm__ __volatile__( "1:	mov""q"" %2,%""""1\n" "2:\n" ".section .fixup,\"ax\"\n" "3:	mov %3,%0\n" "	xor""q"" %""""1,%""""1\n" "	jmp 2b\n" ".previous\n" " .section .ex_table" "" ",\"a\"\n" " .balign 4\n" " .long " "1b" "-." ", " "3b" "-." "\n" " .previous\n" : "=r"(ret), "=r" (*(u64*)to) : "m"((*(const struct __large_struct *)(from))), "i"(8), "0"(ret)); break; default: __get_user_bad(); } } while (0);
            return ret;
        }
    }
    return __copy_from_user_ll(to, from, n);
}
struct exception_table_entry
{
 s32 addr, cont;
};
extern struct exception_table_entry __start___ex_table[];
extern struct exception_table_entry __stop___ex_table[];
extern struct exception_table_entry __start___pre_ex_table[];
extern struct exception_table_entry __stop___pre_ex_table[];
extern unsigned long search_exception_table(unsigned long);
extern void sort_exception_tables(void);
struct __xchg_dummy { unsigned long a[100]; };
static __inline__ __attribute__ ((always_inline)) unsigned long __xchg(
    unsigned long x, volatile void *ptr, int size)
{
    switch ( size )
    {
    case 1:
        asm volatile ( "xchgb %b0,%1"
                       : "=q" (x)
                       : "m" (*((volatile struct __xchg_dummy *)((volatile void *)ptr))), "0" (x)
                       : "memory" );
        break;
    case 2:
        asm volatile ( "xchgw %w0,%1"
                       : "=r" (x)
                       : "m" (*((volatile struct __xchg_dummy *)((volatile void *)ptr))), "0" (x)
                       : "memory" );
        break;
    case 4:
        asm volatile ( "xchgl %k0,%1"
                       : "=r" (x)
                       : "m" (*((volatile struct __xchg_dummy *)((volatile void *)ptr))), "0" (x)
                       : "memory" );
        break;
    case 8:
        asm volatile ( "xchgq %0,%1"
                       : "=r" (x)
                       : "m" (*((volatile struct __xchg_dummy *)((volatile void *)ptr))), "0" (x)
                       : "memory" );
        break;
    }
    return x;
}
static __inline__ __attribute__ ((always_inline)) unsigned long __cmpxchg(
    volatile void *ptr, unsigned long old, unsigned long new, int size)
{
    unsigned long prev;
    switch ( size )
    {
    case 1:
        asm volatile ( "lock; cmpxchgb %b1,%2"
                       : "=a" (prev)
                       : "q" (new), "m" (*((volatile struct __xchg_dummy *)((volatile void *)ptr))),
                       "0" (old)
                       : "memory" );
        return prev;
    case 2:
        asm volatile ( "lock; cmpxchgw %w1,%2"
                       : "=a" (prev)
                       : "r" (new), "m" (*((volatile struct __xchg_dummy *)((volatile void *)ptr))),
                       "0" (old)
                       : "memory" );
        return prev;
    case 4:
        asm volatile ( "lock; cmpxchgl %k1,%2"
                       : "=a" (prev)
                       : "r" (new), "m" (*((volatile struct __xchg_dummy *)((volatile void *)ptr))),
                       "0" (old)
                       : "memory" );
        return prev;
    case 8:
        asm volatile ( "lock; cmpxchgq %1,%2"
                       : "=a" (prev)
                       : "r" (new), "m" (*((volatile struct __xchg_dummy *)((volatile void *)ptr))),
                       "0" (old)
                       : "memory" );
        return prev;
    }
    return old;
}
static __inline__ int local_irq_is_enabled(void)
{
    unsigned long flags;
    ({ ((void)sizeof(struct { int:-!!(sizeof(flags) != sizeof(long)); })); asm volatile ( "pushf" "q" " ; pop" "q" " %0" : "=g" (flags)); });
    return !!(flags & 0x00000200);
}
void trap_init(void);
void percpu_traps_init(void);
void subarch_percpu_traps_init(void);
struct list_head {
    struct list_head *next, *prev;
};
static __inline__ void INIT_LIST_HEAD(struct list_head *list)
{
    list->next = list;
    list->prev = list;
}
static __inline__ void __list_add(struct list_head *new,
                              struct list_head *prev,
                              struct list_head *next)
{
    next->prev = new;
    new->next = next;
    new->prev = prev;
    prev->next = new;
}
static __inline__ void list_add(struct list_head *new, struct list_head *head)
{
    __list_add(new, head, head->next);
}
static __inline__ void list_add_tail(struct list_head *new, struct list_head *head)
{
    __list_add(new, head->prev, head);
}
static __inline__ void __list_add_rcu(struct list_head *new,
                                  struct list_head *prev,
                                  struct list_head *next)
{
    new->next = next;
    new->prev = prev;
    __asm__ __volatile__("": : :"memory");
    next->prev = new;
    prev->next = new;
}
static __inline__ void list_add_rcu(struct list_head *new, struct list_head *head)
{
    __list_add_rcu(new, head, head->next);
}
static __inline__ void list_add_tail_rcu(struct list_head *new,
                                     struct list_head *head)
{
    __list_add_rcu(new, head->prev, head);
}
static __inline__ void __list_del(struct list_head *prev,
                              struct list_head *next)
{
    next->prev = prev;
    prev->next = next;
}
static __inline__ void list_del(struct list_head *entry)
{
    do { if ( 0 && (entry->next->prev == entry) ); } while (0);
    do { if ( 0 && (entry->prev->next == entry) ); } while (0);
    __list_del(entry->prev, entry->next);
    entry->next = ((void *) 0x00100100);
    entry->prev = ((void *) 0x00200200);
}
static __inline__ void list_del_rcu(struct list_head *entry)
{
    __list_del(entry->prev, entry->next);
    entry->prev = ((void *) 0x00200200);
}
static __inline__ void list_replace(struct list_head *old,
                                struct list_head *new)
{
    new->next = old->next;
    new->next->prev = new;
    new->prev = old->prev;
    new->prev->next = new;
}
static __inline__ void list_replace_init(struct list_head *old,
                                     struct list_head *new)
{
    list_replace(old, new);
    INIT_LIST_HEAD(old);
}
static __inline__ void list_replace_rcu(struct list_head *old,
                                    struct list_head *new)
{
    new->next = old->next;
    new->prev = old->prev;
    __asm__ __volatile__("": : :"memory");
    new->next->prev = new;
    new->prev->next = new;
    old->prev = ((void *) 0x00200200);
}
static __inline__ void list_del_init(struct list_head *entry)
{
    __list_del(entry->prev, entry->next);
    INIT_LIST_HEAD(entry);
}
static __inline__ void list_move(struct list_head *list, struct list_head *head)
{
    __list_del(list->prev, list->next);
    list_add(list, head);
}
static __inline__ void list_move_tail(struct list_head *list,
                                  struct list_head *head)
{
    __list_del(list->prev, list->next);
    list_add_tail(list, head);
}
static __inline__ int list_is_last(const struct list_head *list,
                               const struct list_head *head)
{
    return list->next == head;
}
static __inline__ int list_empty(const struct list_head *head)
{
    return head->next == head;
}
static __inline__ int list_empty_careful(const struct list_head *head)
{
    struct list_head *next = head->next;
    return (next == head) && (next == head->prev);
}
static __inline__ void __list_splice(struct list_head *list,
                                 struct list_head *head)
{
    struct list_head *first = list->next;
    struct list_head *last = list->prev;
    struct list_head *at = head->next;
    first->prev = head;
    head->next = first;
    last->next = at;
    at->prev = last;
}
static __inline__ void list_splice(struct list_head *list, struct list_head *head)
{
    if (!list_empty(list))
        __list_splice(list, head);
}
static __inline__ void list_splice_init(struct list_head *list,
                                    struct list_head *head)
{
    if (!list_empty(list)) {
        __list_splice(list, head);
        INIT_LIST_HEAD(list);
    }
}
struct hlist_head {
    struct hlist_node *first;
};
struct hlist_node {
    struct hlist_node *next, **pprev;
};
static __inline__ void INIT_HLIST_NODE(struct hlist_node *h)
{
    h->next = ((void*)0);
    h->pprev = ((void*)0);
}
static __inline__ int hlist_unhashed(const struct hlist_node *h)
{
    return !h->pprev;
}
static __inline__ int hlist_empty(const struct hlist_head *h)
{
    return !h->first;
}
static __inline__ void __hlist_del(struct hlist_node *n)
{
    struct hlist_node *next = n->next;
    struct hlist_node **pprev = n->pprev;
    *pprev = next;
    if (next)
        next->pprev = pprev;
}
static __inline__ void hlist_del(struct hlist_node *n)
{
    __hlist_del(n);
    n->next = ((void *) 0x00100100);
    n->pprev = ((void *) 0x00200200);
}
static __inline__ void hlist_del_rcu(struct hlist_node *n)
{
    __hlist_del(n);
    n->pprev = ((void *) 0x00200200);
}
static __inline__ void hlist_del_init(struct hlist_node *n)
{
    if (!hlist_unhashed(n)) {
        __hlist_del(n);
        INIT_HLIST_NODE(n);
    }
}
static __inline__ void hlist_replace_rcu(struct hlist_node *old,
                                     struct hlist_node *new)
{
    struct hlist_node *next = old->next;
    new->next = next;
    new->pprev = old->pprev;
    __asm__ __volatile__("": : :"memory");
    if (next)
        new->next->pprev = &new->next;
    *new->pprev = new;
    old->pprev = ((void *) 0x00200200);
}
static __inline__ void hlist_add_head(struct hlist_node *n, struct hlist_head *h)
{
    struct hlist_node *first = h->first;
    n->next = first;
    if (first)
        first->pprev = &n->next;
    h->first = n;
    n->pprev = &h->first;
}
static __inline__ void hlist_add_head_rcu(struct hlist_node *n,
                                      struct hlist_head *h)
{
    struct hlist_node *first = h->first;
    n->next = first;
    n->pprev = &h->first;
    __asm__ __volatile__("": : :"memory");
    if (first)
        first->pprev = &n->next;
    h->first = n;
}
static __inline__ void hlist_add_before(struct hlist_node *n,
                    struct hlist_node *next)
{
    n->pprev = next->pprev;
    n->next = next;
    next->pprev = &n->next;
    *(n->pprev) = n;
}
static __inline__ void hlist_add_after(struct hlist_node *n,
                    struct hlist_node *next)
{
    next->next = n->next;
    n->next = next;
    next->pprev = &n->next;
    if(next->next)
        next->next->pprev = &next->next;
}
static __inline__ void hlist_add_before_rcu(struct hlist_node *n,
                                        struct hlist_node *next)
{
    n->pprev = next->pprev;
    n->next = next;
    __asm__ __volatile__("": : :"memory");
    next->pprev = &n->next;
    *(n->pprev) = n;
}
static __inline__ void hlist_add_after_rcu(struct hlist_node *prev,
                                       struct hlist_node *n)
{
    n->next = prev->next;
    n->pprev = &prev->next;
    __asm__ __volatile__("": : :"memory");
    prev->next = n;
    if (n->next)
        n->next->pprev = &n->next;
}
static __inline__ uint8_t read_u8_atomic(const volatile uint8_t *addr) { uint8_t ret; asm volatile("mov" "b" " %1,%0":"=q" (ret) :"m" (*(volatile uint8_t *)addr) ); return ret; }
static __inline__ uint16_t read_u16_atomic(const volatile uint16_t *addr) { uint16_t ret; asm volatile("mov" "w" " %1,%0":"=r" (ret) :"m" (*(volatile uint16_t *)addr) ); return ret; }
static __inline__ uint32_t read_u32_atomic(const volatile uint32_t *addr) { uint32_t ret; asm volatile("mov" "l" " %1,%0":"=r" (ret) :"m" (*(volatile uint32_t *)addr) ); return ret; }
static __inline__ void write_u8_atomic(volatile uint8_t *addr, uint8_t val) { asm volatile("mov" "b" " %1,%0": "=m" (*(volatile uint8_t *)addr) :"q" (val) ); }
static __inline__ void write_u16_atomic(volatile uint16_t *addr, uint16_t val) { asm volatile("mov" "w" " %1,%0": "=m" (*(volatile uint16_t *)addr) :"r" (val) ); }
static __inline__ void write_u32_atomic(volatile uint32_t *addr, uint32_t val) { asm volatile("mov" "l" " %1,%0": "=m" (*(volatile uint32_t *)addr) :"r" (val) ); }
static __inline__ uint64_t read_u64_atomic(const volatile uint64_t *addr) { uint64_t ret; asm volatile("mov" "q" " %1,%0":"=r" (ret) :"m" (*(volatile uint64_t *)addr) ); return ret; }
static __inline__ void write_u64_atomic(volatile uint64_t *addr, uint64_t val) { asm volatile("mov" "q" " %1,%0": "=m" (*(volatile uint64_t *)addr) :"r" (val) ); }
void __bad_atomic_size(void);
typedef struct { int counter; } atomic_t;
static __inline__ void atomic_add(int i, atomic_t *v)
{
    asm volatile (
        "lock; addl %1,%0"
        : "=m" (*(volatile int *)&v->counter)
        : "ir" (i), "m" (*(volatile int *)&v->counter) );
}
static __inline__ void atomic_sub(int i, atomic_t *v)
{
    asm volatile (
        "lock; subl %1,%0"
        : "=m" (*(volatile int *)&v->counter)
        : "ir" (i), "m" (*(volatile int *)&v->counter) );
}
static __inline__ int atomic_sub_and_test(int i, atomic_t *v)
{
    unsigned char c;
    asm volatile (
        "lock; subl %2,%0; sete %1"
        : "=m" (*(volatile int *)&v->counter), "=qm" (c)
        : "ir" (i), "m" (*(volatile int *)&v->counter) : "memory" );
    return c;
}
static __inline__ void atomic_inc(atomic_t *v)
{
    asm volatile (
        "lock; incl %0"
        : "=m" (*(volatile int *)&v->counter)
        : "m" (*(volatile int *)&v->counter) );
}
static __inline__ void atomic_dec(atomic_t *v)
{
    asm volatile (
        "lock; decl %0"
        : "=m" (*(volatile int *)&v->counter)
        : "m" (*(volatile int *)&v->counter) );
}
static __inline__ int atomic_dec_and_test(atomic_t *v)
{
    unsigned char c;
    asm volatile (
        "lock; decl %0; sete %1"
        : "=m" (*(volatile int *)&v->counter), "=qm" (c)
        : "m" (*(volatile int *)&v->counter) : "memory" );
    return c != 0;
}
static __inline__ int atomic_inc_and_test(atomic_t *v)
{
    unsigned char c;
    asm volatile (
        "lock; incl %0; sete %1"
        : "=m" (*(volatile int *)&v->counter), "=qm" (c)
        : "m" (*(volatile int *)&v->counter) : "memory" );
    return c != 0;
}
static __inline__ int atomic_add_negative(int i, atomic_t *v)
{
    unsigned char c;
    asm volatile (
        "lock; addl %2,%0; sets %1"
        : "=m" (*(volatile int *)&v->counter), "=qm" (c)
        : "ir" (i), "m" (*(volatile int *)&v->counter) : "memory" );
    return c;
}
static __inline__ atomic_t atomic_compareandswap(
    atomic_t old, atomic_t new, atomic_t *v)
{
    atomic_t rc;
    rc.counter = __cmpxchg(&v->counter, old.counter, new.counter, sizeof(int));
    return rc;
}
typedef struct {
    volatile s16 lock;
} raw_spinlock_t;
static __inline__ __attribute__ ((always_inline)) void _raw_spin_unlock(raw_spinlock_t *lock)
{
    do { if ( 0 && (((lock)->lock <= 0)) ); } while (0);
    asm volatile (
        "movw $1,%0"
        : "=m" (lock->lock) : : "memory" );
}
static __inline__ __attribute__ ((always_inline)) int _raw_spin_trylock(raw_spinlock_t *lock)
{
    s16 oldval;
    asm volatile (
        "xchgw %w0,%1"
        :"=r" (oldval), "=m" (lock->lock)
        :"0" ((s16)0) : "memory" );
    return (oldval > 0);
}
typedef struct {
    volatile int lock;
} raw_rwlock_t;
static __inline__ __attribute__ ((always_inline)) int _raw_read_trylock(raw_rwlock_t *rw)
{
    int acquired;
    asm volatile (
        "    lock; decl %0         \n"
        "    jns 2f                \n"
        "1:  .subsection 1         \n"
        "2:  lock; incl %0         \n"
        "    decl %1               \n"
        "    jmp 1b                \n"
        "    .subsection 0         \n"
        : "=m" (rw->lock), "=r" (acquired) : "1" (1) : "memory" );
    return acquired;
}
static __inline__ __attribute__ ((always_inline)) int _raw_write_trylock(raw_rwlock_t *rw)
{
    return (((__typeof__(*(&rw->lock)))__cmpxchg((&rw->lock),(unsigned long)(0), (unsigned long)(0x7fffffff),sizeof(*(&rw->lock)))) == 0);
}
static __inline__ __attribute__ ((always_inline)) void _raw_read_unlock(raw_rwlock_t *rw)
{
    asm volatile (
        "lock ; incl %0"
        : "=m" ((rw)->lock) : : "memory" );
}
static __inline__ __attribute__ ((always_inline)) void _raw_write_unlock(raw_rwlock_t *rw)
{
    asm volatile (
        "lock ; subl %1,%0"
        : "=m" ((rw)->lock) : "i" (0x7fffffff) : "memory" );
}
struct lock_debug { };
struct lock_profile_qhead { };
typedef struct spinlock {
    raw_spinlock_t raw;
    u16 recurse_cpu:12;
    u16 recurse_cnt:4;
    struct lock_debug debug;
} spinlock_t;
typedef struct {
    raw_rwlock_t raw;
    struct lock_debug debug;
} rwlock_t;
void _spin_lock(spinlock_t *lock);
void _spin_lock_irq(spinlock_t *lock);
unsigned long _spin_lock_irqsave(spinlock_t *lock);
void _spin_unlock(spinlock_t *lock);
void _spin_unlock_irq(spinlock_t *lock);
void _spin_unlock_irqrestore(spinlock_t *lock, unsigned long flags);
int _spin_is_locked(spinlock_t *lock);
int _spin_trylock(spinlock_t *lock);
void _spin_barrier(spinlock_t *lock);
int _spin_trylock_recursive(spinlock_t *lock);
void _spin_lock_recursive(spinlock_t *lock);
void _spin_unlock_recursive(spinlock_t *lock);
void _read_lock(rwlock_t *lock);
void _read_lock_irq(rwlock_t *lock);
unsigned long _read_lock_irqsave(rwlock_t *lock);
void _read_unlock(rwlock_t *lock);
void _read_unlock_irq(rwlock_t *lock);
void _read_unlock_irqrestore(rwlock_t *lock, unsigned long flags);
int _read_trylock(rwlock_t *lock);
void _write_lock(rwlock_t *lock);
void _write_lock_irq(rwlock_t *lock);
unsigned long _write_lock_irqsave(rwlock_t *lock);
int _write_trylock(rwlock_t *lock);
void _write_unlock(rwlock_t *lock);
void _write_unlock_irq(rwlock_t *lock);
void _write_unlock_irqrestore(rwlock_t *lock, unsigned long flags);
int _rw_is_locked(rwlock_t *lock);
int _rw_is_write_locked(rwlock_t *lock);
struct domain;
struct page_info;
void init_boot_pages(paddr_t ps, paddr_t pe);
unsigned long alloc_boot_pages(
    unsigned long nr_pfns, unsigned long pfn_align);
void end_boot_allocator(void);
void init_xenheap_pages(paddr_t ps, paddr_t pe);
void xenheap_max_mfn(unsigned long mfn);
void *alloc_xenheap_pages(unsigned int order, unsigned int memflags);
void free_xenheap_pages(void *v, unsigned int order);
int map_pages_to_xen(
    unsigned long virt,
    unsigned long mfn,
    unsigned long nr_mfns,
    unsigned int flags);
void destroy_xen_mappings(unsigned long v, unsigned long e);
unsigned long domain_adjust_tot_pages(struct domain *d, long pages);
int domain_set_outstanding_pages(struct domain *d, unsigned long pages);
void get_outstanding_claims(uint64_t *free_pages, uint64_t *outstanding_pages);
void init_domheap_pages(paddr_t ps, paddr_t pe);
struct page_info *alloc_domheap_pages(
    struct domain *d, unsigned int order, unsigned int memflags);
void free_domheap_pages(struct page_info *pg, unsigned int order);
unsigned long avail_domheap_pages_region(
    unsigned int node, unsigned int min_width, unsigned int max_width);
unsigned long avail_domheap_pages(void);
unsigned long avail_node_heap_pages(unsigned int);
unsigned int online_page(unsigned long mfn, uint32_t *status);
int offline_page(unsigned long mfn, int broken, uint32_t *status);
int query_page_offline(unsigned long mfn, uint32_t *status);
unsigned long total_free_pages(void);
void scrub_heap_pages(void);
int assign_pages(
    struct domain *d,
    struct page_info *pg,
    unsigned int order,
    unsigned int memflags);
void arch_dump_shared_mem_info(void);
void *vm_alloc(unsigned int nr, unsigned int align);
void vm_free(const void *);
void *__vmap(const unsigned long *mfn, unsigned int granularity,
             unsigned int nr, unsigned int align, unsigned int flags);
void *vmap(const unsigned long *mfn, unsigned int nr);
void vunmap(const void *);
void *ioremap(paddr_t, size_t);
static __inline__ void iounmap(void *va)
{
    unsigned long addr = (unsigned long)(void *)va;
    vunmap((void *)(addr & (~(((1L) << 12)-1))));
}
void vm_init(void);
void *arch_vmap_virt_end(void);
static __inline__ unsigned char inb(unsigned short port) { unsigned char _v; __asm__ __volatile__ ("in" "b" " %" "w" "1,%" "" "0" : "=a" (_v) : "Nd" (port) ); return _v; } static __inline__ unsigned char inb_p(unsigned short port) { unsigned char _v; __asm__ __volatile__ ("in" "b" " %" "w" "1,%" "" "0" : "=a" (_v) : "Nd" (port) ); return _v; }
static __inline__ unsigned short inw(unsigned short port) { unsigned short _v; __asm__ __volatile__ ("in" "w" " %" "w" "1,%" "" "0" : "=a" (_v) : "Nd" (port) ); return _v; } static __inline__ unsigned short inw_p(unsigned short port) { unsigned short _v; __asm__ __volatile__ ("in" "w" " %" "w" "1,%" "" "0" : "=a" (_v) : "Nd" (port) ); return _v; }
static __inline__ unsigned int inl(unsigned short port) { unsigned int _v; __asm__ __volatile__ ("in" "l" " %" "w" "1,%" "" "0" : "=a" (_v) : "Nd" (port) ); return _v; } static __inline__ unsigned int inl_p(unsigned short port) { unsigned int _v; __asm__ __volatile__ ("in" "l" " %" "w" "1,%" "" "0" : "=a" (_v) : "Nd" (port) ); return _v; }
static __inline__ void outb(unsigned char value, unsigned short port) { __asm__ __volatile__ ("out" "b" " %" "b" "0,%" "w" "1" : : "a" (value), "Nd" (port)); } static __inline__ void outb_p(unsigned char value, unsigned short port) { __asm__ __volatile__ ("out" "b" " %" "b" "0,%" "w" "1" : : "a" (value), "Nd" (port));}
static __inline__ void outw(unsigned short value, unsigned short port) { __asm__ __volatile__ ("out" "w" " %" "w" "0,%" "w" "1" : : "a" (value), "Nd" (port)); } static __inline__ void outw_p(unsigned short value, unsigned short port) { __asm__ __volatile__ ("out" "w" " %" "w" "0,%" "w" "1" : : "a" (value), "Nd" (port));}
static __inline__ void outl(unsigned int value, unsigned short port) { __asm__ __volatile__ ("out" "l" " %" "0,%" "w" "1" : : "a" (value), "Nd" (port)); } static __inline__ void outl_p(unsigned int value, unsigned short port) { __asm__ __volatile__ ("out" "l" " %" "0,%" "w" "1" : : "a" (value), "Nd" (port));}
extern void (*pv_post_outb_hook)(unsigned int port, u8 value);
extern void (*ioemul_handle_quirk)(
    u8 opcode, char *io_emul_stub, struct cpu_user_regs *regs);
struct page_list_entry
{
    unsigned int next, prev;
};
struct page_sharing_info;
struct page_info
{
    union {
        struct page_list_entry list;
        paddr_t up;
        struct page_sharing_info *sharing;
    };
    unsigned long count_info;
    union {
        struct {
            unsigned long type_info;
        } inuse;
        struct {
            unsigned long type:5;
            unsigned long pinned:1;
            unsigned long head:1;
            unsigned long count:25;
        } sh;
        struct {
            bool_t need_tlbflush;
        } free;
    } u;
    union {
        struct {
            unsigned int _domain;
        } inuse;
        struct {
            unsigned int back;
        } sh;
        struct {
            unsigned int order;
        } free;
    } v;
    union {
        u32 tlbflush_timestamp;
        struct {
            u16 nr_validated_ptes;
            s8 partial_pte;
        };
        u32 shadow_flags;
        unsigned int next_shadow;
    };
};
struct spage_info
{
       unsigned long type_info;
};
extern void share_xen_page_with_guest(
    struct page_info *page, struct domain *d, int readonly);
extern void share_xen_page_with_privileged_guests(
    struct page_info *page, int readonly);
int get_superpage(unsigned long mfn, struct domain *d);
extern unsigned long max_page;
extern unsigned long total_pages;
void init_frametable(void);
extern unsigned long pdx_group_valid[];
static __inline__ struct page_info *__virt_to_page(const void *v)
{
    unsigned long va = (unsigned long)v;
    do { if ( 0 && (va >= ((((((((((261UL) >> 8) * (0xffff000000000000UL)) | ((261UL) << 39))) + (((1UL) << 39) / 2)) + ((64UL) << 30)) + ((1UL) << 30)) + ((1UL) << 30)))) ); } while (0);
    do { if ( 0 && (va < ((((((262UL) >> 8) * (0xffff000000000000UL)) | ((262UL) << 39))) + (((1UL) << 39) * (511 - 262)))) ); } while (0);
    if ( va < (((((((((((261UL) >> 8) * (0xffff000000000000UL)) | ((261UL) << 39))) + (((1UL) << 39) / 2)) + ((64UL) << 30)) + ((1UL) << 30)) + ((1UL) << 30))) + ((1UL) << 30)) )
        va += (((((262UL) >> 8) * (0xffff000000000000UL)) | ((262UL) << 39))) - ((((((((((261UL) >> 8) * (0xffff000000000000UL)) | ((261UL) << 39))) + (((1UL) << 39) / 2)) + ((64UL) << 30)) + ((1UL) << 30)) + ((1UL) << 30))) + xen_phys_start;
    else
        do { if ( 0 && (va >= (((((262UL) >> 8) * (0xffff000000000000UL)) | ((262UL) << 39)))) ); } while (0);
    return ((struct page_info *)((((((262UL) >> 8) * (0xffff000000000000UL)) | ((262UL) << 39))) - ((128UL) << 30))) + ((va - (((((262UL) >> 8) * (0xffff000000000000UL)) | ((262UL) << 39)))) >> 12);
}
static __inline__ void *__page_to_virt(const struct page_info *pg)
{
    do { if ( 0 && ((unsigned long)pg - ((((((262UL) >> 8) * (0xffff000000000000UL)) | ((262UL) << 39))) - ((128UL) << 30)) < ((128UL) << 30)) ); } while (0);
    return (void *)((((((262UL) >> 8) * (0xffff000000000000UL)) | ((262UL) << 39))) +
                    ((unsigned long)pg - ((((((262UL) >> 8) * (0xffff000000000000UL)) | ((262UL) << 39))) - ((128UL) << 30))) /
                    (sizeof(*pg) / (sizeof(*pg) & -sizeof(*pg))) *
                    (((1L) << 12) / (sizeof(*pg) & -sizeof(*pg))));
}
int free_page_type(struct page_info *page, unsigned long type,
                   int preemptible);
void init_guest_l4_table(l4_pgentry_t[], const struct domain *);
int is_iomem_page(unsigned long mfn);
void clear_superpage_mark(struct page_info *page);
const unsigned long *get_platform_badpages(unsigned int *array_size);
int page_lock(struct page_info *page);
void page_unlock(struct page_info *page);
struct domain *page_get_owner_and_reference(struct page_info *page);
void put_page(struct page_info *page);
int get_page(struct page_info *page, struct domain *domain);
void put_page_type(struct page_info *page);
int get_page_type(struct page_info *page, unsigned long type);
int put_page_type_preemptible(struct page_info *page);
int get_page_type_preemptible(struct page_info *page, unsigned long type);
int put_old_guest_table(struct vcpu *);
int get_page_from_l1e(
    l1_pgentry_t l1e, struct domain *l1e_owner, struct domain *pg_owner);
void put_page_from_l1e(l1_pgentry_t l1e, struct domain *l1e_owner);
static __inline__ void put_page_and_type(struct page_info *page)
{
    put_page_type(page);
    put_page(page);
}
static __inline__ int put_page_and_type_preemptible(struct page_info *page)
{
    int rc = put_page_type_preemptible(page);
    if ( __builtin_expect((rc == 0),1) )
        put_page(page);
    return rc;
}
static __inline__ int get_page_and_type(struct page_info *page,
                                    struct domain *domain,
                                    unsigned long type)
{
    int rc = get_page(page, domain);
    if ( __builtin_expect((rc),1) && __builtin_expect((!get_page_type(page, type)),0) )
    {
        put_page(page);
        rc = 0;
    }
    return rc;
}
int check_descriptor(const struct domain *, struct desc_struct *d);
extern bool_t opt_allow_superpage;
extern paddr_t mem_hotplug;
typedef unsigned long mfn_t; static __inline__ mfn_t _mfn(unsigned long n) { return n; } static __inline__ unsigned long mfn_x(mfn_t n) { return n; };
extern bool_t machine_to_phys_mapping_valid;
extern struct rangeset *mmio_ro_ranges;
void memguard_guard_stack(void *p);
void memguard_unguard_stack(void *p);
int ptwr_do_page_fault(struct vcpu *, unsigned long,
                        struct cpu_user_regs *);
int mmio_ro_do_page_fault(struct vcpu *, unsigned long,
                           struct cpu_user_regs *);
int audit_adjust_pgtables(struct domain *d, int dir, int noisy);
extern int pagefault_by_memadd(unsigned long addr, struct cpu_user_regs *regs);
extern int handle_memadd_fault(unsigned long addr, struct cpu_user_regs *regs);
int new_guest_cr3(unsigned long pfn);
void make_cr3(struct vcpu *v, unsigned long mfn);
void update_cr3(struct vcpu *v);
int vcpu_destroy_pagetables(struct vcpu *);
struct trap_bounce *propagate_page_fault(unsigned long addr, u16 error_code);
void *do_page_walk(struct vcpu *v, unsigned long addr);
int __sync_local_execstate(void);
long arch_memory_op(int op, __guest_handle_void arg);
long subarch_memory_op(int op, __guest_handle_void arg);
int compat_arch_memory_op(int op, __guest_handle_void);
int compat_subarch_memory_op(int op, __guest_handle_void);
int steal_page(
    struct domain *d, struct page_info *page, unsigned int memflags);
int donate_page(
    struct domain *d, struct page_info *page, unsigned int memflags);
int map_ldt_shadow_page(unsigned int);
int create_perdomain_mapping(struct domain *, unsigned long va,
                             unsigned int nr, l1_pgentry_t **,
                             struct page_info **);
void destroy_perdomain_mapping(struct domain *, unsigned long va,
                               unsigned int nr);
void free_perdomain_mappings(struct domain *);
extern int memory_add(unsigned long spfn, unsigned long epfn, unsigned int pxm);
void domain_set_alloc_bitsize(struct domain *d);
unsigned int domain_clamp_alloc_bitsize(struct domain *d, unsigned int bits);
unsigned long domain_get_maximum_gpfn(struct domain *d);
void mem_event_cleanup(struct domain *d);
extern struct domain *dom_xen, *dom_io, *dom_cow;
typedef struct mm_lock {
    spinlock_t lock;
    int unlock_level;
    int locker;
    const char *locker_function;
} mm_lock_t;
typedef struct mm_rwlock {
    rwlock_t lock;
    int unlock_level;
    int recurse_count;
    int locker;
    const char *locker_function;
} mm_rwlock_t;
struct page_list_head
{
    struct page_info *next, *tail;
};
static __inline__ int
page_list_empty(const struct page_list_head *head)
{
    return !head->next;
}
static __inline__ struct page_info *
page_list_first(const struct page_list_head *head)
{
    return head->next;
}
static __inline__ struct page_info *
page_list_next(const struct page_info *page,
               const struct page_list_head *head)
{
    return page != head->tail ? (((struct page_info *)((((((262UL) >> 8) * (0xffff000000000000UL)) | ((262UL) << 39))) - ((128UL) << 30))) + (page->list.next)) : ((void*)0);
}
static __inline__ struct page_info *
page_list_prev(const struct page_info *page,
               const struct page_list_head *head)
{
    return page != head->next ? (((struct page_info *)((((((262UL) >> 8) * (0xffff000000000000UL)) | ((262UL) << 39))) - ((128UL) << 30))) + (page->list.prev)) : ((void*)0);
}
static __inline__ void
page_list_add(struct page_info *page, struct page_list_head *head)
{
    if ( head->next )
    {
        page->list.next = ((head->next) - ((struct page_info *)((((((262UL) >> 8) * (0xffff000000000000UL)) | ((262UL) << 39))) - ((128UL) << 30))));
        head->next->list.prev = ((page) - ((struct page_info *)((((((262UL) >> 8) * (0xffff000000000000UL)) | ((262UL) << 39))) - ((128UL) << 30))));
    }
    else
    {
        head->tail = page;
        page->list.next = ((typeof(((struct page_info){}).list.next))~0);
    }
    page->list.prev = ((typeof(((struct page_info){}).list.next))~0);
    head->next = page;
}
static __inline__ void
page_list_add_tail(struct page_info *page, struct page_list_head *head)
{
    page->list.next = ((typeof(((struct page_info){}).list.next))~0);
    if ( head->next )
    {
        page->list.prev = ((head->tail) - ((struct page_info *)((((((262UL) >> 8) * (0xffff000000000000UL)) | ((262UL) << 39))) - ((128UL) << 30))));
        head->tail->list.next = ((page) - ((struct page_info *)((((((262UL) >> 8) * (0xffff000000000000UL)) | ((262UL) << 39))) - ((128UL) << 30))));
    }
    else
    {
        page->list.prev = ((typeof(((struct page_info){}).list.next))~0);
        head->next = page;
    }
    head->tail = page;
}
static __inline__ bool_t
__page_list_del_head(struct page_info *page, struct page_list_head *head,
                     struct page_info *next, struct page_info *prev)
{
    if ( head->next == page )
    {
        if ( head->tail != page )
        {
            next->list.prev = ((typeof(((struct page_info){}).list.next))~0);
            head->next = next;
        }
        else
            head->tail = head->next = ((void*)0);
        return 1;
    }
    if ( head->tail == page )
    {
        prev->list.next = ((typeof(((struct page_info){}).list.next))~0);
        head->tail = prev;
        return 1;
    }
    return 0;
}
static __inline__ void
page_list_del(struct page_info *page, struct page_list_head *head)
{
    struct page_info *next = (((struct page_info *)((((((262UL) >> 8) * (0xffff000000000000UL)) | ((262UL) << 39))) - ((128UL) << 30))) + (page->list.next));
    struct page_info *prev = (((struct page_info *)((((((262UL) >> 8) * (0xffff000000000000UL)) | ((262UL) << 39))) - ((128UL) << 30))) + (page->list.prev));
    if ( !__page_list_del_head(page, head, next, prev) )
    {
        next->list.prev = page->list.prev;
        prev->list.next = page->list.next;
    }
}
static __inline__ void
page_list_del2(struct page_info *page, struct page_list_head *head1,
               struct page_list_head *head2)
{
    struct page_info *next = (((struct page_info *)((((((262UL) >> 8) * (0xffff000000000000UL)) | ((262UL) << 39))) - ((128UL) << 30))) + (page->list.next));
    struct page_info *prev = (((struct page_info *)((((((262UL) >> 8) * (0xffff000000000000UL)) | ((262UL) << 39))) - ((128UL) << 30))) + (page->list.prev));
    if ( !__page_list_del_head(page, head1, next, prev) &&
         !__page_list_del_head(page, head2, next, prev) )
    {
        next->list.prev = page->list.prev;
        prev->list.next = page->list.next;
    }
}
static __inline__ struct page_info *
page_list_remove_head(struct page_list_head *head)
{
    struct page_info *page = head->next;
    if ( page )
        page_list_del(page, head);
    return page;
}
static __inline__ void
page_list_move(struct page_list_head *dst, struct page_list_head *src)
{
    if ( !page_list_empty(src) )
    {
        *dst = *src;
        ((src)->tail = (src)->next = ((void*)0));
    }
}
static __inline__ void
page_list_splice(struct page_list_head *list, struct page_list_head *head)
{
    struct page_info *first, *last, *at;
    if ( page_list_empty(list) )
        return;
    if ( page_list_empty(head) )
    {
        head->next = list->next;
        head->tail = list->tail;
        return;
    }
    first = list->next;
    last = list->tail;
    at = head->next;
    do { if ( 0 && (first->list.prev == ((typeof(((struct page_info){}).list.next))~0)) ); } while (0);
    do { if ( 0 && (first->list.prev == at->list.prev) ); } while (0);
    head->next = first;
    last->list.next = ((at) - ((struct page_info *)((((((262UL) >> 8) * (0xffff000000000000UL)) | ((262UL) << 39))) - ((128UL) << 30))));
    at->list.prev = ((last) - ((struct page_info *)((((((262UL) >> 8) * (0xffff000000000000UL)) | ((262UL) << 39))) - ((128UL) << 30))));
}
static __inline__ unsigned int get_order_from_bytes(paddr_t size)
{
    unsigned int order;
    size = (size - 1) >> 12;
    for ( order = 0; size; order++ )
        size >>= 1;
    return order;
}
static __inline__ unsigned int get_order_from_pages(unsigned long nr_pages)
{
    unsigned int order;
    nr_pages--;
    for ( order = 0; nr_pages; order++ )
        nr_pages >>= 1;
    return order;
}
void scrub_one_page(struct page_info *);
int xenmem_add_to_physmap_one(struct domain *d, unsigned int space,
                              domid_t foreign_domid,
                              unsigned long idx, xen_pfn_t gpfn);
int guest_remove_page(struct domain *d, unsigned long gmfn);
int page_is_ram_type(unsigned long mfn, unsigned long mem_type);
typedef uint32_t grant_ref_t;
struct grant_entry_v1 {
    uint16_t flags;
    domid_t domid;
    uint32_t frame;
};
typedef struct grant_entry_v1 grant_entry_v1_t;
struct grant_entry_header {
    uint16_t flags;
    domid_t domid;
};
typedef struct grant_entry_header grant_entry_header_t;
union grant_entry_v2 {
    grant_entry_header_t hdr;
    struct {
        grant_entry_header_t hdr;
        uint32_t pad0;
        uint64_t frame;
    } full_page;
    struct {
        grant_entry_header_t hdr;
        uint16_t page_off;
        uint16_t length;
        uint64_t frame;
    } sub_page;
    struct {
        grant_entry_header_t hdr;
        domid_t trans_domid;
        uint16_t pad0;
        grant_ref_t gref;
    } transitive;
    uint32_t __spacer[4];
};
typedef union grant_entry_v2 grant_entry_v2_t;
typedef uint16_t grant_status_t;
typedef uint32_t grant_handle_t;
struct gnttab_map_grant_ref {
    uint64_t host_addr;
    uint32_t flags;
    grant_ref_t ref;
    domid_t dom;
    int16_t status;
    grant_handle_t handle;
    uint64_t dev_bus_addr;
};
typedef struct gnttab_map_grant_ref gnttab_map_grant_ref_t;
typedef struct { gnttab_map_grant_ref_t *p; } __guest_handle_gnttab_map_grant_ref_t; typedef struct { const gnttab_map_grant_ref_t *p; } __guest_handle_const_gnttab_map_grant_ref_t;
struct gnttab_unmap_grant_ref {
    uint64_t host_addr;
    uint64_t dev_bus_addr;
    grant_handle_t handle;
    int16_t status;
};
typedef struct gnttab_unmap_grant_ref gnttab_unmap_grant_ref_t;
typedef struct { gnttab_unmap_grant_ref_t *p; } __guest_handle_gnttab_unmap_grant_ref_t; typedef struct { const gnttab_unmap_grant_ref_t *p; } __guest_handle_const_gnttab_unmap_grant_ref_t;
struct gnttab_setup_table {
    domid_t dom;
    uint32_t nr_frames;
    int16_t status;
    __guest_handle_xen_pfn_t frame_list;
};
typedef struct gnttab_setup_table gnttab_setup_table_t;
typedef struct { gnttab_setup_table_t *p; } __guest_handle_gnttab_setup_table_t; typedef struct { const gnttab_setup_table_t *p; } __guest_handle_const_gnttab_setup_table_t;
struct gnttab_dump_table {
    domid_t dom;
    int16_t status;
};
typedef struct gnttab_dump_table gnttab_dump_table_t;
typedef struct { gnttab_dump_table_t *p; } __guest_handle_gnttab_dump_table_t; typedef struct { const gnttab_dump_table_t *p; } __guest_handle_const_gnttab_dump_table_t;
struct gnttab_transfer {
    xen_pfn_t mfn;
    domid_t domid;
    grant_ref_t ref;
    int16_t status;
};
typedef struct gnttab_transfer gnttab_transfer_t;
typedef struct { gnttab_transfer_t *p; } __guest_handle_gnttab_transfer_t; typedef struct { const gnttab_transfer_t *p; } __guest_handle_const_gnttab_transfer_t;
struct gnttab_copy {
    struct {
        union {
            grant_ref_t ref;
            xen_pfn_t gmfn;
        } u;
        domid_t domid;
        uint16_t offset;
    } source, dest;
    uint16_t len;
    uint16_t flags;
    int16_t status;
};
typedef struct gnttab_copy gnttab_copy_t;
typedef struct { gnttab_copy_t *p; } __guest_handle_gnttab_copy_t; typedef struct { const gnttab_copy_t *p; } __guest_handle_const_gnttab_copy_t;
struct gnttab_query_size {
    domid_t dom;
    uint32_t nr_frames;
    uint32_t max_nr_frames;
    int16_t status;
};
typedef struct gnttab_query_size gnttab_query_size_t;
typedef struct { gnttab_query_size_t *p; } __guest_handle_gnttab_query_size_t; typedef struct { const gnttab_query_size_t *p; } __guest_handle_const_gnttab_query_size_t;
struct gnttab_unmap_and_replace {
    uint64_t host_addr;
    uint64_t new_addr;
    grant_handle_t handle;
    int16_t status;
};
typedef struct gnttab_unmap_and_replace gnttab_unmap_and_replace_t;
typedef struct { gnttab_unmap_and_replace_t *p; } __guest_handle_gnttab_unmap_and_replace_t; typedef struct { const gnttab_unmap_and_replace_t *p; } __guest_handle_const_gnttab_unmap_and_replace_t;
struct gnttab_set_version {
    uint32_t version;
};
typedef struct gnttab_set_version gnttab_set_version_t;
typedef struct { gnttab_set_version_t *p; } __guest_handle_gnttab_set_version_t; typedef struct { const gnttab_set_version_t *p; } __guest_handle_const_gnttab_set_version_t;
struct gnttab_get_status_frames {
    uint32_t nr_frames;
    domid_t dom;
    int16_t status;
    __guest_handle_uint64_t frame_list;
};
typedef struct gnttab_get_status_frames gnttab_get_status_frames_t;
typedef struct { gnttab_get_status_frames_t *p; } __guest_handle_gnttab_get_status_frames_t; typedef struct { const gnttab_get_status_frames_t *p; } __guest_handle_const_gnttab_get_status_frames_t;
struct gnttab_get_version {
    domid_t dom;
    uint16_t pad;
    uint32_t version;
};
typedef struct gnttab_get_version gnttab_get_version_t;
typedef struct { gnttab_get_version_t *p; } __guest_handle_gnttab_get_version_t; typedef struct { const gnttab_get_version_t *p; } __guest_handle_const_gnttab_get_version_t;
struct gnttab_swap_grant_ref {
    grant_ref_t ref_a;
    grant_ref_t ref_b;
    int16_t status;
};
typedef struct gnttab_swap_grant_ref gnttab_swap_grant_ref_t;
typedef struct { gnttab_swap_grant_ref_t *p; } __guest_handle_gnttab_swap_grant_ref_t; typedef struct { const gnttab_swap_grant_ref_t *p; } __guest_handle_const_gnttab_swap_grant_ref_t;
struct hvm_save_descriptor {
    uint16_t typecode;
    uint16_t instance;
    uint32_t length;
};
struct hvm_save_end {};
static __inline__ int __HVM_SAVE_FIX_COMPAT_END(void *h) { do { do { ((void)sizeof(struct { int:-!!((101) >> ((31 - 24) + (31 - 24))); })); asm volatile ( ".Lbug%=: ud2\n" ".pushsection .bug_frames.%c0, \"a\", @progbits\n" ".p2align 2\n" ".Lfrm%=:\n" ".long (.Lbug%= - .Lfrm%=) + %c4\n" ".long (%c1 - .Lfrm%=) + %c3\n" ".if " "0" "\n" ".long 0, %c2 - .Lfrm%=\n" ".endif\n" ".popsection" : : "i" (2), "i" ("/home/dquinlan/ROSE/Zen/rose-sh/workspace/xen/phase_1/xen-src/xen/include/public/hvm/save.h"), "i" (((void*)0)), "i" ((101 & ((1 << (31 - 24)) - 1)) << 24), "i" (((101) >> (31 - 24)) << 24)); } while (0); do {} while (1); } while (0); return -1; } struct __HVM_SAVE_TYPE_END { struct hvm_save_end t; char c[0]; char cpt[1];}; struct __HVM_SAVE_TYPE_COMPAT_END { struct hvm_save_end t; };
struct hvm_save_header {
    uint32_t magic;
    uint32_t version;
    uint64_t changeset;
    uint32_t cpuid;
    uint32_t gtsc_khz;
};
static __inline__ int __HVM_SAVE_FIX_COMPAT_HEADER(void *h) { do { do { ((void)sizeof(struct { int:-!!((44) >> ((31 - 24) + (31 - 24))); })); asm volatile ( ".Lbug%=: ud2\n" ".pushsection .bug_frames.%c0, \"a\", @progbits\n" ".p2align 2\n" ".Lfrm%=:\n" ".long (.Lbug%= - .Lfrm%=) + %c4\n" ".long (%c1 - .Lfrm%=) + %c3\n" ".if " "0" "\n" ".long 0, %c2 - .Lfrm%=\n" ".endif\n" ".popsection" : : "i" (2), "i" ("/home/dquinlan/ROSE/Zen/rose-sh/workspace/xen/phase_1/xen-src/xen/include/public/hvm/../arch-x86/hvm/save.h"), "i" (((void*)0)), "i" ((44 & ((1 << (31 - 24)) - 1)) << 24), "i" (((44) >> (31 - 24)) << 24)); } while (0); do {} while (1); } while (0); return -1; } struct __HVM_SAVE_TYPE_HEADER { struct hvm_save_header t; char c[1]; char cpt[1];}; struct __HVM_SAVE_TYPE_COMPAT_HEADER { struct hvm_save_header t; };
struct hvm_hw_cpu {
    uint8_t fpu_regs[512];
    uint64_t rax;
    uint64_t rbx;
    uint64_t rcx;
    uint64_t rdx;
    uint64_t rbp;
    uint64_t rsi;
    uint64_t rdi;
    uint64_t rsp;
    uint64_t r8;
    uint64_t r9;
    uint64_t r10;
    uint64_t r11;
    uint64_t r12;
    uint64_t r13;
    uint64_t r14;
    uint64_t r15;
    uint64_t rip;
    uint64_t rflags;
    uint64_t cr0;
    uint64_t cr2;
    uint64_t cr3;
    uint64_t cr4;
    uint64_t dr0;
    uint64_t dr1;
    uint64_t dr2;
    uint64_t dr3;
    uint64_t dr6;
    uint64_t dr7;
    uint32_t cs_sel;
    uint32_t ds_sel;
    uint32_t es_sel;
    uint32_t fs_sel;
    uint32_t gs_sel;
    uint32_t ss_sel;
    uint32_t tr_sel;
    uint32_t ldtr_sel;
    uint32_t cs_limit;
    uint32_t ds_limit;
    uint32_t es_limit;
    uint32_t fs_limit;
    uint32_t gs_limit;
    uint32_t ss_limit;
    uint32_t tr_limit;
    uint32_t ldtr_limit;
    uint32_t idtr_limit;
    uint32_t gdtr_limit;
    uint64_t cs_base;
    uint64_t ds_base;
    uint64_t es_base;
    uint64_t fs_base;
    uint64_t gs_base;
    uint64_t ss_base;
    uint64_t tr_base;
    uint64_t ldtr_base;
    uint64_t idtr_base;
    uint64_t gdtr_base;
    uint32_t cs_arbytes;
    uint32_t ds_arbytes;
    uint32_t es_arbytes;
    uint32_t fs_arbytes;
    uint32_t gs_arbytes;
    uint32_t ss_arbytes;
    uint32_t tr_arbytes;
    uint32_t ldtr_arbytes;
    uint64_t sysenter_cs;
    uint64_t sysenter_esp;
    uint64_t sysenter_eip;
    uint64_t shadow_gs;
    uint64_t msr_flags;
    uint64_t msr_lstar;
    uint64_t msr_star;
    uint64_t msr_cstar;
    uint64_t msr_syscall_mask;
    uint64_t msr_efer;
    uint64_t msr_tsc_aux;
    uint64_t tsc;
    union {
        uint32_t pending_event;
        struct {
            uint8_t pending_vector:8;
            uint8_t pending_type:3;
            uint8_t pending_error_valid:1;
            uint32_t pending_reserved:19;
            uint8_t pending_valid:1;
        };
    };
    uint32_t error_code;
};
struct hvm_hw_cpu_compat {
    uint8_t fpu_regs[512];
    uint64_t rax;
    uint64_t rbx;
    uint64_t rcx;
    uint64_t rdx;
    uint64_t rbp;
    uint64_t rsi;
    uint64_t rdi;
    uint64_t rsp;
    uint64_t r8;
    uint64_t r9;
    uint64_t r10;
    uint64_t r11;
    uint64_t r12;
    uint64_t r13;
    uint64_t r14;
    uint64_t r15;
    uint64_t rip;
    uint64_t rflags;
    uint64_t cr0;
    uint64_t cr2;
    uint64_t cr3;
    uint64_t cr4;
    uint64_t dr0;
    uint64_t dr1;
    uint64_t dr2;
    uint64_t dr3;
    uint64_t dr6;
    uint64_t dr7;
    uint32_t cs_sel;
    uint32_t ds_sel;
    uint32_t es_sel;
    uint32_t fs_sel;
    uint32_t gs_sel;
    uint32_t ss_sel;
    uint32_t tr_sel;
    uint32_t ldtr_sel;
    uint32_t cs_limit;
    uint32_t ds_limit;
    uint32_t es_limit;
    uint32_t fs_limit;
    uint32_t gs_limit;
    uint32_t ss_limit;
    uint32_t tr_limit;
    uint32_t ldtr_limit;
    uint32_t idtr_limit;
    uint32_t gdtr_limit;
    uint64_t cs_base;
    uint64_t ds_base;
    uint64_t es_base;
    uint64_t fs_base;
    uint64_t gs_base;
    uint64_t ss_base;
    uint64_t tr_base;
    uint64_t ldtr_base;
    uint64_t idtr_base;
    uint64_t gdtr_base;
    uint32_t cs_arbytes;
    uint32_t ds_arbytes;
    uint32_t es_arbytes;
    uint32_t fs_arbytes;
    uint32_t gs_arbytes;
    uint32_t ss_arbytes;
    uint32_t tr_arbytes;
    uint32_t ldtr_arbytes;
    uint64_t sysenter_cs;
    uint64_t sysenter_esp;
    uint64_t sysenter_eip;
    uint64_t shadow_gs;
    uint64_t msr_flags;
    uint64_t msr_lstar;
    uint64_t msr_star;
    uint64_t msr_cstar;
    uint64_t msr_syscall_mask;
    uint64_t msr_efer;
    uint64_t tsc;
    union {
        uint32_t pending_event;
        struct {
            uint8_t pending_vector:8;
            uint8_t pending_type:3;
            uint8_t pending_error_valid:1;
            uint32_t pending_reserved:19;
            uint8_t pending_valid:1;
        };
    };
    uint32_t error_code;
};
static __inline__ int _hvm_hw_fix_cpu(void *h) {
    union hvm_hw_cpu_union {
        struct hvm_hw_cpu nat;
        struct hvm_hw_cpu_compat cmp;
    } *ucpu = (union hvm_hw_cpu_union *)h;
    ucpu->nat.error_code = ucpu->cmp.error_code;
    ucpu->nat.pending_event = ucpu->cmp.pending_event;
    ucpu->nat.tsc = ucpu->cmp.tsc;
    ucpu->nat.msr_tsc_aux = 0;
    return 0;
}
static __inline__ int __HVM_SAVE_FIX_COMPAT_CPU(void *h) { return _hvm_hw_fix_cpu(h); } struct __HVM_SAVE_TYPE_CPU { struct hvm_hw_cpu t; char c[2]; char cpt[2];}; struct __HVM_SAVE_TYPE_COMPAT_CPU { struct hvm_hw_cpu_compat t; };
struct hvm_hw_vpic {
    uint8_t irr;
    uint8_t imr;
    uint8_t isr;
    uint8_t irq_base;
    uint8_t init_state:4;
    uint8_t priority_add:4;
    uint8_t readsel_isr:1;
    uint8_t poll:1;
    uint8_t auto_eoi:1;
    uint8_t rotate_on_auto_eoi:1;
    uint8_t special_fully_nested_mode:1;
    uint8_t special_mask_mode:1;
    uint8_t is_master:1;
    uint8_t elcr;
    uint8_t int_output;
};
static __inline__ int __HVM_SAVE_FIX_COMPAT_PIC(void *h) { do { do { ((void)sizeof(struct { int:-!!((343) >> ((31 - 24) + (31 - 24))); })); asm volatile ( ".Lbug%=: ud2\n" ".pushsection .bug_frames.%c0, \"a\", @progbits\n" ".p2align 2\n" ".Lfrm%=:\n" ".long (.Lbug%= - .Lfrm%=) + %c4\n" ".long (%c1 - .Lfrm%=) + %c3\n" ".if " "0" "\n" ".long 0, %c2 - .Lfrm%=\n" ".endif\n" ".popsection" : : "i" (2), "i" ("/home/dquinlan/ROSE/Zen/rose-sh/workspace/xen/phase_1/xen-src/xen/include/public/hvm/../arch-x86/hvm/save.h"), "i" (((void*)0)), "i" ((343 & ((1 << (31 - 24)) - 1)) << 24), "i" (((343) >> (31 - 24)) << 24)); } while (0); do {} while (1); } while (0); return -1; } struct __HVM_SAVE_TYPE_PIC { struct hvm_hw_vpic t; char c[3]; char cpt[1];}; struct __HVM_SAVE_TYPE_COMPAT_PIC { struct hvm_hw_vpic t; };
struct hvm_hw_vioapic {
    uint64_t base_address;
    uint32_t ioregsel;
    uint32_t id;
    union vioapic_redir_entry
    {
        uint64_t bits;
        struct {
            uint8_t vector;
            uint8_t delivery_mode:3;
            uint8_t dest_mode:1;
            uint8_t delivery_status:1;
            uint8_t polarity:1;
            uint8_t remote_irr:1;
            uint8_t trig_mode:1;
            uint8_t mask:1;
            uint8_t reserve:7;
            uint8_t reserved[4];
            uint8_t dest_id;
        } fields;
    } redirtbl[48];
};
static __inline__ int __HVM_SAVE_FIX_COMPAT_IOAPIC(void *h) { do { do { ((void)sizeof(struct { int:-!!((375) >> ((31 - 24) + (31 - 24))); })); asm volatile ( ".Lbug%=: ud2\n" ".pushsection .bug_frames.%c0, \"a\", @progbits\n" ".p2align 2\n" ".Lfrm%=:\n" ".long (.Lbug%= - .Lfrm%=) + %c4\n" ".long (%c1 - .Lfrm%=) + %c3\n" ".if " "0" "\n" ".long 0, %c2 - .Lfrm%=\n" ".endif\n" ".popsection" : : "i" (2), "i" ("/home/dquinlan/ROSE/Zen/rose-sh/workspace/xen/phase_1/xen-src/xen/include/public/hvm/../arch-x86/hvm/save.h"), "i" (((void*)0)), "i" ((375 & ((1 << (31 - 24)) - 1)) << 24), "i" (((375) >> (31 - 24)) << 24)); } while (0); do {} while (1); } while (0); return -1; } struct __HVM_SAVE_TYPE_IOAPIC { struct hvm_hw_vioapic t; char c[4]; char cpt[1];}; struct __HVM_SAVE_TYPE_COMPAT_IOAPIC { struct hvm_hw_vioapic t; };
struct hvm_hw_lapic {
    uint64_t apic_base_msr;
    uint32_t disabled;
    uint32_t timer_divisor;
    uint64_t tdt_msr;
};
static __inline__ int __HVM_SAVE_FIX_COMPAT_LAPIC(void *h) { do { do { ((void)sizeof(struct { int:-!!((389) >> ((31 - 24) + (31 - 24))); })); asm volatile ( ".Lbug%=: ud2\n" ".pushsection .bug_frames.%c0, \"a\", @progbits\n" ".p2align 2\n" ".Lfrm%=:\n" ".long (.Lbug%= - .Lfrm%=) + %c4\n" ".long (%c1 - .Lfrm%=) + %c3\n" ".if " "0" "\n" ".long 0, %c2 - .Lfrm%=\n" ".endif\n" ".popsection" : : "i" (2), "i" ("/home/dquinlan/ROSE/Zen/rose-sh/workspace/xen/phase_1/xen-src/xen/include/public/hvm/../arch-x86/hvm/save.h"), "i" (((void*)0)), "i" ((389 & ((1 << (31 - 24)) - 1)) << 24), "i" (((389) >> (31 - 24)) << 24)); } while (0); do {} while (1); } while (0); return -1; } struct __HVM_SAVE_TYPE_LAPIC { struct hvm_hw_lapic t; char c[5]; char cpt[1];}; struct __HVM_SAVE_TYPE_COMPAT_LAPIC { struct hvm_hw_lapic t; };
struct hvm_hw_lapic_regs {
    uint8_t data[1024];
};
static __inline__ int __HVM_SAVE_FIX_COMPAT_LAPIC_REGS(void *h) { do { do { ((void)sizeof(struct { int:-!!((395) >> ((31 - 24) + (31 - 24))); })); asm volatile ( ".Lbug%=: ud2\n" ".pushsection .bug_frames.%c0, \"a\", @progbits\n" ".p2align 2\n" ".Lfrm%=:\n" ".long (.Lbug%= - .Lfrm%=) + %c4\n" ".long (%c1 - .Lfrm%=) + %c3\n" ".if " "0" "\n" ".long 0, %c2 - .Lfrm%=\n" ".endif\n" ".popsection" : : "i" (2), "i" ("/home/dquinlan/ROSE/Zen/rose-sh/workspace/xen/phase_1/xen-src/xen/include/public/hvm/../arch-x86/hvm/save.h"), "i" (((void*)0)), "i" ((395 & ((1 << (31 - 24)) - 1)) << 24), "i" (((395) >> (31 - 24)) << 24)); } while (0); do {} while (1); } while (0); return -1; } struct __HVM_SAVE_TYPE_LAPIC_REGS { struct hvm_hw_lapic_regs t; char c[6]; char cpt[1];}; struct __HVM_SAVE_TYPE_COMPAT_LAPIC_REGS { struct hvm_hw_lapic_regs t; };
struct hvm_hw_pci_irqs {
    union {
        unsigned long i[16 / sizeof (unsigned long)];
        uint64_t pad[2];
    };
};
static __inline__ int __HVM_SAVE_FIX_COMPAT_PCI_IRQ(void *h) { do { do { ((void)sizeof(struct { int:-!!((413) >> ((31 - 24) + (31 - 24))); })); asm volatile ( ".Lbug%=: ud2\n" ".pushsection .bug_frames.%c0, \"a\", @progbits\n" ".p2align 2\n" ".Lfrm%=:\n" ".long (.Lbug%= - .Lfrm%=) + %c4\n" ".long (%c1 - .Lfrm%=) + %c3\n" ".if " "0" "\n" ".long 0, %c2 - .Lfrm%=\n" ".endif\n" ".popsection" : : "i" (2), "i" ("/home/dquinlan/ROSE/Zen/rose-sh/workspace/xen/phase_1/xen-src/xen/include/public/hvm/../arch-x86/hvm/save.h"), "i" (((void*)0)), "i" ((413 & ((1 << (31 - 24)) - 1)) << 24), "i" (((413) >> (31 - 24)) << 24)); } while (0); do {} while (1); } while (0); return -1; } struct __HVM_SAVE_TYPE_PCI_IRQ { struct hvm_hw_pci_irqs t; char c[7]; char cpt[1];}; struct __HVM_SAVE_TYPE_COMPAT_PCI_IRQ { struct hvm_hw_pci_irqs t; };
struct hvm_hw_isa_irqs {
    union {
        unsigned long i[1];
        uint64_t pad[1];
    };
};
static __inline__ int __HVM_SAVE_FIX_COMPAT_ISA_IRQ(void *h) { do { do { ((void)sizeof(struct { int:-!!((426) >> ((31 - 24) + (31 - 24))); })); asm volatile ( ".Lbug%=: ud2\n" ".pushsection .bug_frames.%c0, \"a\", @progbits\n" ".p2align 2\n" ".Lfrm%=:\n" ".long (.Lbug%= - .Lfrm%=) + %c4\n" ".long (%c1 - .Lfrm%=) + %c3\n" ".if " "0" "\n" ".long 0, %c2 - .Lfrm%=\n" ".endif\n" ".popsection" : : "i" (2), "i" ("/home/dquinlan/ROSE/Zen/rose-sh/workspace/xen/phase_1/xen-src/xen/include/public/hvm/../arch-x86/hvm/save.h"), "i" (((void*)0)), "i" ((426 & ((1 << (31 - 24)) - 1)) << 24), "i" (((426) >> (31 - 24)) << 24)); } while (0); do {} while (1); } while (0); return -1; } struct __HVM_SAVE_TYPE_ISA_IRQ { struct hvm_hw_isa_irqs t; char c[8]; char cpt[1];}; struct __HVM_SAVE_TYPE_COMPAT_ISA_IRQ { struct hvm_hw_isa_irqs t; };
struct hvm_hw_pci_link {
    uint8_t route[4];
    uint8_t pad0[4];
};
static __inline__ int __HVM_SAVE_FIX_COMPAT_PCI_LINK(void *h) { do { do { ((void)sizeof(struct { int:-!!((439) >> ((31 - 24) + (31 - 24))); })); asm volatile ( ".Lbug%=: ud2\n" ".pushsection .bug_frames.%c0, \"a\", @progbits\n" ".p2align 2\n" ".Lfrm%=:\n" ".long (.Lbug%= - .Lfrm%=) + %c4\n" ".long (%c1 - .Lfrm%=) + %c3\n" ".if " "0" "\n" ".long 0, %c2 - .Lfrm%=\n" ".endif\n" ".popsection" : : "i" (2), "i" ("/home/dquinlan/ROSE/Zen/rose-sh/workspace/xen/phase_1/xen-src/xen/include/public/hvm/../arch-x86/hvm/save.h"), "i" (((void*)0)), "i" ((439 & ((1 << (31 - 24)) - 1)) << 24), "i" (((439) >> (31 - 24)) << 24)); } while (0); do {} while (1); } while (0); return -1; } struct __HVM_SAVE_TYPE_PCI_LINK { struct hvm_hw_pci_link t; char c[9]; char cpt[1];}; struct __HVM_SAVE_TYPE_COMPAT_PCI_LINK { struct hvm_hw_pci_link t; };
struct hvm_hw_pit {
    struct hvm_hw_pit_channel {
        uint32_t count;
        uint16_t latched_count;
        uint8_t count_latched;
        uint8_t status_latched;
        uint8_t status;
        uint8_t read_state;
        uint8_t write_state;
        uint8_t write_latch;
        uint8_t rw_mode;
        uint8_t mode;
        uint8_t bcd;
        uint8_t gate;
    } channels[3];
    uint32_t speaker_data_on;
    uint32_t pad0;
};
static __inline__ int __HVM_SAVE_FIX_COMPAT_PIT(void *h) { do { do { ((void)sizeof(struct { int:-!!((464) >> ((31 - 24) + (31 - 24))); })); asm volatile ( ".Lbug%=: ud2\n" ".pushsection .bug_frames.%c0, \"a\", @progbits\n" ".p2align 2\n" ".Lfrm%=:\n" ".long (.Lbug%= - .Lfrm%=) + %c4\n" ".long (%c1 - .Lfrm%=) + %c3\n" ".if " "0" "\n" ".long 0, %c2 - .Lfrm%=\n" ".endif\n" ".popsection" : : "i" (2), "i" ("/home/dquinlan/ROSE/Zen/rose-sh/workspace/xen/phase_1/xen-src/xen/include/public/hvm/../arch-x86/hvm/save.h"), "i" (((void*)0)), "i" ((464 & ((1 << (31 - 24)) - 1)) << 24), "i" (((464) >> (31 - 24)) << 24)); } while (0); do {} while (1); } while (0); return -1; } struct __HVM_SAVE_TYPE_PIT { struct hvm_hw_pit t; char c[10]; char cpt[1];}; struct __HVM_SAVE_TYPE_COMPAT_PIT { struct hvm_hw_pit t; };
struct hvm_hw_rtc {
    uint8_t cmos_data[14];
    uint8_t cmos_index;
    uint8_t pad0;
};
static __inline__ int __HVM_SAVE_FIX_COMPAT_RTC(void *h) { do { do { ((void)sizeof(struct { int:-!!((480) >> ((31 - 24) + (31 - 24))); })); asm volatile ( ".Lbug%=: ud2\n" ".pushsection .bug_frames.%c0, \"a\", @progbits\n" ".p2align 2\n" ".Lfrm%=:\n" ".long (.Lbug%= - .Lfrm%=) + %c4\n" ".long (%c1 - .Lfrm%=) + %c3\n" ".if " "0" "\n" ".long 0, %c2 - .Lfrm%=\n" ".endif\n" ".popsection" : : "i" (2), "i" ("/home/dquinlan/ROSE/Zen/rose-sh/workspace/xen/phase_1/xen-src/xen/include/public/hvm/../arch-x86/hvm/save.h"), "i" (((void*)0)), "i" ((480 & ((1 << (31 - 24)) - 1)) << 24), "i" (((480) >> (31 - 24)) << 24)); } while (0); do {} while (1); } while (0); return -1; } struct __HVM_SAVE_TYPE_RTC { struct hvm_hw_rtc t; char c[11]; char cpt[1];}; struct __HVM_SAVE_TYPE_COMPAT_RTC { struct hvm_hw_rtc t; };
struct hvm_hw_hpet {
    uint64_t capability;
    uint64_t res0;
    uint64_t config;
    uint64_t res1;
    uint64_t isr;
    uint64_t res2[25];
    uint64_t mc64;
    uint64_t res3;
    struct {
        uint64_t config;
        uint64_t cmp;
        uint64_t fsb;
        uint64_t res4;
    } timers[3];
    uint64_t res5[4*(24-3)];
    uint64_t period[3];
};
static __inline__ int __HVM_SAVE_FIX_COMPAT_HPET(void *h) { do { do { ((void)sizeof(struct { int:-!!((510) >> ((31 - 24) + (31 - 24))); })); asm volatile ( ".Lbug%=: ud2\n" ".pushsection .bug_frames.%c0, \"a\", @progbits\n" ".p2align 2\n" ".Lfrm%=:\n" ".long (.Lbug%= - .Lfrm%=) + %c4\n" ".long (%c1 - .Lfrm%=) + %c3\n" ".if " "0" "\n" ".long 0, %c2 - .Lfrm%=\n" ".endif\n" ".popsection" : : "i" (2), "i" ("/home/dquinlan/ROSE/Zen/rose-sh/workspace/xen/phase_1/xen-src/xen/include/public/hvm/../arch-x86/hvm/save.h"), "i" (((void*)0)), "i" ((510 & ((1 << (31 - 24)) - 1)) << 24), "i" (((510) >> (31 - 24)) << 24)); } while (0); do {} while (1); } while (0); return -1; } struct __HVM_SAVE_TYPE_HPET { struct hvm_hw_hpet t; char c[12]; char cpt[1];}; struct __HVM_SAVE_TYPE_COMPAT_HPET { struct hvm_hw_hpet t; };
struct hvm_hw_pmtimer {
    uint32_t tmr_val;
    uint16_t pm1a_sts;
    uint16_t pm1a_en;
};
static __inline__ int __HVM_SAVE_FIX_COMPAT_PMTIMER(void *h) { do { do { ((void)sizeof(struct { int:-!!((523) >> ((31 - 24) + (31 - 24))); })); asm volatile ( ".Lbug%=: ud2\n" ".pushsection .bug_frames.%c0, \"a\", @progbits\n" ".p2align 2\n" ".Lfrm%=:\n" ".long (.Lbug%= - .Lfrm%=) + %c4\n" ".long (%c1 - .Lfrm%=) + %c3\n" ".if " "0" "\n" ".long 0, %c2 - .Lfrm%=\n" ".endif\n" ".popsection" : : "i" (2), "i" ("/home/dquinlan/ROSE/Zen/rose-sh/workspace/xen/phase_1/xen-src/xen/include/public/hvm/../arch-x86/hvm/save.h"), "i" (((void*)0)), "i" ((523 & ((1 << (31 - 24)) - 1)) << 24), "i" (((523) >> (31 - 24)) << 24)); } while (0); do {} while (1); } while (0); return -1; } struct __HVM_SAVE_TYPE_PMTIMER { struct hvm_hw_pmtimer t; char c[13]; char cpt[1];}; struct __HVM_SAVE_TYPE_COMPAT_PMTIMER { struct hvm_hw_pmtimer t; };
struct hvm_hw_mtrr {
    uint64_t msr_pat_cr;
    uint64_t msr_mtrr_var[8*2];
    uint64_t msr_mtrr_fixed[11];
    uint64_t msr_mtrr_cap;
    uint64_t msr_mtrr_def_type;
};
static __inline__ int __HVM_SAVE_FIX_COMPAT_MTRR(void *h) { do { do { ((void)sizeof(struct { int:-!!((540) >> ((31 - 24) + (31 - 24))); })); asm volatile ( ".Lbug%=: ud2\n" ".pushsection .bug_frames.%c0, \"a\", @progbits\n" ".p2align 2\n" ".Lfrm%=:\n" ".long (.Lbug%= - .Lfrm%=) + %c4\n" ".long (%c1 - .Lfrm%=) + %c3\n" ".if " "0" "\n" ".long 0, %c2 - .Lfrm%=\n" ".endif\n" ".popsection" : : "i" (2), "i" ("/home/dquinlan/ROSE/Zen/rose-sh/workspace/xen/phase_1/xen-src/xen/include/public/hvm/../arch-x86/hvm/save.h"), "i" (((void*)0)), "i" ((540 & ((1 << (31 - 24)) - 1)) << 24), "i" (((540) >> (31 - 24)) << 24)); } while (0); do {} while (1); } while (0); return -1; } struct __HVM_SAVE_TYPE_MTRR { struct hvm_hw_mtrr t; char c[14]; char cpt[1];}; struct __HVM_SAVE_TYPE_COMPAT_MTRR { struct hvm_hw_mtrr t; };
struct hvm_hw_cpu_xsave {
    uint64_t xfeature_mask;
    uint64_t xcr0;
    uint64_t xcr0_accum;
    struct {
        struct { char x[512]; } fpu_sse;
        struct {
            uint64_t xstate_bv;
            uint64_t reserved[7];
        } xsave_hdr;
        struct { char x[0]; } ymm;
    } save_area;
};
struct hvm_viridian_domain_context {
    uint64_t hypercall_gpa;
    uint64_t guest_os_id;
};
static __inline__ int __HVM_SAVE_FIX_COMPAT_VIRIDIAN_DOMAIN(void *h) { do { do { ((void)sizeof(struct { int:-!!((573) >> ((31 - 24) + (31 - 24))); })); asm volatile ( ".Lbug%=: ud2\n" ".pushsection .bug_frames.%c0, \"a\", @progbits\n" ".p2align 2\n" ".Lfrm%=:\n" ".long (.Lbug%= - .Lfrm%=) + %c4\n" ".long (%c1 - .Lfrm%=) + %c3\n" ".if " "0" "\n" ".long 0, %c2 - .Lfrm%=\n" ".endif\n" ".popsection" : : "i" (2), "i" ("/home/dquinlan/ROSE/Zen/rose-sh/workspace/xen/phase_1/xen-src/xen/include/public/hvm/../arch-x86/hvm/save.h"), "i" (((void*)0)), "i" ((573 & ((1 << (31 - 24)) - 1)) << 24), "i" (((573) >> (31 - 24)) << 24)); } while (0); do {} while (1); } while (0); return -1; } struct __HVM_SAVE_TYPE_VIRIDIAN_DOMAIN { struct hvm_viridian_domain_context t; char c[15]; char cpt[1];}; struct __HVM_SAVE_TYPE_COMPAT_VIRIDIAN_DOMAIN { struct hvm_viridian_domain_context t; };
struct hvm_viridian_vcpu_context {
    uint64_t apic_assist;
};
static __inline__ int __HVM_SAVE_FIX_COMPAT_VIRIDIAN_VCPU(void *h) { do { do { ((void)sizeof(struct { int:-!!((579) >> ((31 - 24) + (31 - 24))); })); asm volatile ( ".Lbug%=: ud2\n" ".pushsection .bug_frames.%c0, \"a\", @progbits\n" ".p2align 2\n" ".Lfrm%=:\n" ".long (.Lbug%= - .Lfrm%=) + %c4\n" ".long (%c1 - .Lfrm%=) + %c3\n" ".if " "0" "\n" ".long 0, %c2 - .Lfrm%=\n" ".endif\n" ".popsection" : : "i" (2), "i" ("/home/dquinlan/ROSE/Zen/rose-sh/workspace/xen/phase_1/xen-src/xen/include/public/hvm/../arch-x86/hvm/save.h"), "i" (((void*)0)), "i" ((579 & ((1 << (31 - 24)) - 1)) << 24), "i" (((579) >> (31 - 24)) << 24)); } while (0); do {} while (1); } while (0); return -1; } struct __HVM_SAVE_TYPE_VIRIDIAN_VCPU { struct hvm_viridian_vcpu_context t; char c[17]; char cpt[1];}; struct __HVM_SAVE_TYPE_COMPAT_VIRIDIAN_VCPU { struct hvm_viridian_vcpu_context t; };
struct hvm_vmce_vcpu {
    uint64_t caps;
    uint64_t mci_ctl2_bank0;
    uint64_t mci_ctl2_bank1;
};
static __inline__ int __HVM_SAVE_FIX_COMPAT_VMCE_VCPU(void *h) { do { do { ((void)sizeof(struct { int:-!!((587) >> ((31 - 24) + (31 - 24))); })); asm volatile ( ".Lbug%=: ud2\n" ".pushsection .bug_frames.%c0, \"a\", @progbits\n" ".p2align 2\n" ".Lfrm%=:\n" ".long (.Lbug%= - .Lfrm%=) + %c4\n" ".long (%c1 - .Lfrm%=) + %c3\n" ".if " "0" "\n" ".long 0, %c2 - .Lfrm%=\n" ".endif\n" ".popsection" : : "i" (2), "i" ("/home/dquinlan/ROSE/Zen/rose-sh/workspace/xen/phase_1/xen-src/xen/include/public/hvm/../arch-x86/hvm/save.h"), "i" (((void*)0)), "i" ((587 & ((1 << (31 - 24)) - 1)) << 24), "i" (((587) >> (31 - 24)) << 24)); } while (0); do {} while (1); } while (0); return -1; } struct __HVM_SAVE_TYPE_VMCE_VCPU { struct hvm_vmce_vcpu t; char c[18]; char cpt[1];}; struct __HVM_SAVE_TYPE_COMPAT_VMCE_VCPU { struct hvm_vmce_vcpu t; };
struct hvm_tsc_adjust {
    uint64_t tsc_adjust;
};
static __inline__ int __HVM_SAVE_FIX_COMPAT_TSC_ADJUST(void *h) { do { do { ((void)sizeof(struct { int:-!!((593) >> ((31 - 24) + (31 - 24))); })); asm volatile ( ".Lbug%=: ud2\n" ".pushsection .bug_frames.%c0, \"a\", @progbits\n" ".p2align 2\n" ".Lfrm%=:\n" ".long (.Lbug%= - .Lfrm%=) + %c4\n" ".long (%c1 - .Lfrm%=) + %c3\n" ".if " "0" "\n" ".long 0, %c2 - .Lfrm%=\n" ".endif\n" ".popsection" : : "i" (2), "i" ("/home/dquinlan/ROSE/Zen/rose-sh/workspace/xen/phase_1/xen-src/xen/include/public/hvm/../arch-x86/hvm/save.h"), "i" (((void*)0)), "i" ((593 & ((1 << (31 - 24)) - 1)) << 24), "i" (((593) >> (31 - 24)) << 24)); } while (0); do {} while (1); } while (0); return -1; } struct __HVM_SAVE_TYPE_TSC_ADJUST { struct hvm_tsc_adjust t; char c[19]; char cpt[1];}; struct __HVM_SAVE_TYPE_COMPAT_TSC_ADJUST { struct hvm_tsc_adjust t; };
struct xen_domctl_createdomain {
    uint32_t ssidref;
    xen_domain_handle_t handle;
    uint32_t flags;
};
typedef struct xen_domctl_createdomain xen_domctl_createdomain_t;
typedef struct { xen_domctl_createdomain_t *p; } __guest_handle_xen_domctl_createdomain_t; typedef struct { const xen_domctl_createdomain_t *p; } __guest_handle_const_xen_domctl_createdomain_t;
struct xen_domctl_getdomaininfo {
    domid_t domain;
    uint32_t flags;
    uint64_t tot_pages;
    uint64_t max_pages;
    uint64_t outstanding_pages;
    uint64_t shr_pages;
    uint64_t paged_pages;
    uint64_t shared_info_frame;
    uint64_t cpu_time;
    uint32_t nr_online_vcpus;
    uint32_t max_vcpu_id;
    uint32_t ssidref;
    xen_domain_handle_t handle;
    uint32_t cpupool;
};
typedef struct xen_domctl_getdomaininfo xen_domctl_getdomaininfo_t;
typedef struct { xen_domctl_getdomaininfo_t *p; } __guest_handle_xen_domctl_getdomaininfo_t; typedef struct { const xen_domctl_getdomaininfo_t *p; } __guest_handle_const_xen_domctl_getdomaininfo_t;
struct xen_domctl_getmemlist {
    uint64_t max_pfns;
    uint64_t start_pfn;
    __guest_handle_uint64 buffer;
    uint64_t num_pfns;
};
typedef struct xen_domctl_getmemlist xen_domctl_getmemlist_t;
typedef struct { xen_domctl_getmemlist_t *p; } __guest_handle_xen_domctl_getmemlist_t; typedef struct { const xen_domctl_getmemlist_t *p; } __guest_handle_const_xen_domctl_getmemlist_t;
struct xen_domctl_getpageframeinfo {
    uint64_t gmfn;
    uint32_t type;
};
typedef struct xen_domctl_getpageframeinfo xen_domctl_getpageframeinfo_t;
typedef struct { xen_domctl_getpageframeinfo_t *p; } __guest_handle_xen_domctl_getpageframeinfo_t; typedef struct { const xen_domctl_getpageframeinfo_t *p; } __guest_handle_const_xen_domctl_getpageframeinfo_t;
struct xen_domctl_getpageframeinfo2 {
    uint64_t num;
    __guest_handle_uint32 array;
};
typedef struct xen_domctl_getpageframeinfo2 xen_domctl_getpageframeinfo2_t;
typedef struct { xen_domctl_getpageframeinfo2_t *p; } __guest_handle_xen_domctl_getpageframeinfo2_t; typedef struct { const xen_domctl_getpageframeinfo2_t *p; } __guest_handle_const_xen_domctl_getpageframeinfo2_t;
struct xen_domctl_getpageframeinfo3 {
    uint64_t num;
    __guest_handle_xen_pfn_t array;
};
struct xen_domctl_shadow_op_stats {
    uint32_t fault_count;
    uint32_t dirty_count;
};
typedef struct xen_domctl_shadow_op_stats xen_domctl_shadow_op_stats_t;
typedef struct { xen_domctl_shadow_op_stats_t *p; } __guest_handle_xen_domctl_shadow_op_stats_t; typedef struct { const xen_domctl_shadow_op_stats_t *p; } __guest_handle_const_xen_domctl_shadow_op_stats_t;
struct xen_domctl_shadow_op {
    uint32_t op;
    uint32_t mode;
    uint32_t mb;
    __guest_handle_uint8 dirty_bitmap;
    uint64_t pages;
    struct xen_domctl_shadow_op_stats stats;
};
typedef struct xen_domctl_shadow_op xen_domctl_shadow_op_t;
typedef struct { xen_domctl_shadow_op_t *p; } __guest_handle_xen_domctl_shadow_op_t; typedef struct { const xen_domctl_shadow_op_t *p; } __guest_handle_const_xen_domctl_shadow_op_t;
struct xen_domctl_max_mem {
    uint64_t max_memkb;
};
typedef struct xen_domctl_max_mem xen_domctl_max_mem_t;
typedef struct { xen_domctl_max_mem_t *p; } __guest_handle_xen_domctl_max_mem_t; typedef struct { const xen_domctl_max_mem_t *p; } __guest_handle_const_xen_domctl_max_mem_t;
struct xen_domctl_vcpucontext {
    uint32_t vcpu;
    __guest_handle_vcpu_guest_context_t ctxt;
};
typedef struct xen_domctl_vcpucontext xen_domctl_vcpucontext_t;
typedef struct { xen_domctl_vcpucontext_t *p; } __guest_handle_xen_domctl_vcpucontext_t; typedef struct { const xen_domctl_vcpucontext_t *p; } __guest_handle_const_xen_domctl_vcpucontext_t;
struct xen_domctl_getvcpuinfo {
    uint32_t vcpu;
    uint8_t online;
    uint8_t blocked;
    uint8_t running;
    uint64_t cpu_time;
    uint32_t cpu;
};
typedef struct xen_domctl_getvcpuinfo xen_domctl_getvcpuinfo_t;
typedef struct { xen_domctl_getvcpuinfo_t *p; } __guest_handle_xen_domctl_getvcpuinfo_t; typedef struct { const xen_domctl_getvcpuinfo_t *p; } __guest_handle_const_xen_domctl_getvcpuinfo_t;
struct xen_domctl_nodeaffinity {
    struct xenctl_bitmap nodemap;
};
typedef struct xen_domctl_nodeaffinity xen_domctl_nodeaffinity_t;
typedef struct { xen_domctl_nodeaffinity_t *p; } __guest_handle_xen_domctl_nodeaffinity_t; typedef struct { const xen_domctl_nodeaffinity_t *p; } __guest_handle_const_xen_domctl_nodeaffinity_t;
struct xen_domctl_vcpuaffinity {
    uint32_t vcpu;
    struct xenctl_bitmap cpumap;
};
typedef struct xen_domctl_vcpuaffinity xen_domctl_vcpuaffinity_t;
typedef struct { xen_domctl_vcpuaffinity_t *p; } __guest_handle_xen_domctl_vcpuaffinity_t; typedef struct { const xen_domctl_vcpuaffinity_t *p; } __guest_handle_const_xen_domctl_vcpuaffinity_t;
struct xen_domctl_max_vcpus {
    uint32_t max;
};
typedef struct xen_domctl_max_vcpus xen_domctl_max_vcpus_t;
typedef struct { xen_domctl_max_vcpus_t *p; } __guest_handle_xen_domctl_max_vcpus_t; typedef struct { const xen_domctl_max_vcpus_t *p; } __guest_handle_const_xen_domctl_max_vcpus_t;
struct xen_domctl_scheduler_op {
    uint32_t sched_id;
    uint32_t cmd;
    union {
        struct xen_domctl_sched_sedf {
            uint64_t period;
            uint64_t slice;
            uint64_t latency;
            uint32_t extratime;
            uint32_t weight;
        } sedf;
        struct xen_domctl_sched_credit {
            uint16_t weight;
            uint16_t cap;
        } credit;
        struct xen_domctl_sched_credit2 {
            uint16_t weight;
        } credit2;
    } u;
};
typedef struct xen_domctl_scheduler_op xen_domctl_scheduler_op_t;
typedef struct { xen_domctl_scheduler_op_t *p; } __guest_handle_xen_domctl_scheduler_op_t; typedef struct { const xen_domctl_scheduler_op_t *p; } __guest_handle_const_xen_domctl_scheduler_op_t;
struct xen_domctl_setdomainhandle {
    xen_domain_handle_t handle;
};
typedef struct xen_domctl_setdomainhandle xen_domctl_setdomainhandle_t;
typedef struct { xen_domctl_setdomainhandle_t *p; } __guest_handle_xen_domctl_setdomainhandle_t; typedef struct { const xen_domctl_setdomainhandle_t *p; } __guest_handle_const_xen_domctl_setdomainhandle_t;
struct xen_domctl_setdebugging {
    uint8_t enable;
};
typedef struct xen_domctl_setdebugging xen_domctl_setdebugging_t;
typedef struct { xen_domctl_setdebugging_t *p; } __guest_handle_xen_domctl_setdebugging_t; typedef struct { const xen_domctl_setdebugging_t *p; } __guest_handle_const_xen_domctl_setdebugging_t;
struct xen_domctl_irq_permission {
    uint8_t pirq;
    uint8_t allow_access;
};
typedef struct xen_domctl_irq_permission xen_domctl_irq_permission_t;
typedef struct { xen_domctl_irq_permission_t *p; } __guest_handle_xen_domctl_irq_permission_t; typedef struct { const xen_domctl_irq_permission_t *p; } __guest_handle_const_xen_domctl_irq_permission_t;
struct xen_domctl_iomem_permission {
    uint64_t first_mfn;
    uint64_t nr_mfns;
    uint8_t allow_access;
};
typedef struct xen_domctl_iomem_permission xen_domctl_iomem_permission_t;
typedef struct { xen_domctl_iomem_permission_t *p; } __guest_handle_xen_domctl_iomem_permission_t; typedef struct { const xen_domctl_iomem_permission_t *p; } __guest_handle_const_xen_domctl_iomem_permission_t;
struct xen_domctl_ioport_permission {
    uint32_t first_port;
    uint32_t nr_ports;
    uint8_t allow_access;
};
typedef struct xen_domctl_ioport_permission xen_domctl_ioport_permission_t;
typedef struct { xen_domctl_ioport_permission_t *p; } __guest_handle_xen_domctl_ioport_permission_t; typedef struct { const xen_domctl_ioport_permission_t *p; } __guest_handle_const_xen_domctl_ioport_permission_t;
struct xen_domctl_hypercall_init {
    uint64_t gmfn;
};
typedef struct xen_domctl_hypercall_init xen_domctl_hypercall_init_t;
typedef struct { xen_domctl_hypercall_init_t *p; } __guest_handle_xen_domctl_hypercall_init_t; typedef struct { const xen_domctl_hypercall_init_t *p; } __guest_handle_const_xen_domctl_hypercall_init_t;
typedef struct xen_domctl_arch_setup {
    uint64_t flags;
} xen_domctl_arch_setup_t;
typedef struct { xen_domctl_arch_setup_t *p; } __guest_handle_xen_domctl_arch_setup_t; typedef struct { const xen_domctl_arch_setup_t *p; } __guest_handle_const_xen_domctl_arch_setup_t;
struct xen_domctl_settimeoffset {
    int32_t time_offset_seconds;
};
typedef struct xen_domctl_settimeoffset xen_domctl_settimeoffset_t;
typedef struct { xen_domctl_settimeoffset_t *p; } __guest_handle_xen_domctl_settimeoffset_t; typedef struct { const xen_domctl_settimeoffset_t *p; } __guest_handle_const_xen_domctl_settimeoffset_t;
typedef struct xen_domctl_hvmcontext {
    uint32_t size;
    __guest_handle_uint8 buffer;
} xen_domctl_hvmcontext_t;
typedef struct { xen_domctl_hvmcontext_t *p; } __guest_handle_xen_domctl_hvmcontext_t; typedef struct { const xen_domctl_hvmcontext_t *p; } __guest_handle_const_xen_domctl_hvmcontext_t;
typedef struct xen_domctl_address_size {
    uint32_t size;
} xen_domctl_address_size_t;
typedef struct { xen_domctl_address_size_t *p; } __guest_handle_xen_domctl_address_size_t; typedef struct { const xen_domctl_address_size_t *p; } __guest_handle_const_xen_domctl_address_size_t;
struct xen_domctl_real_mode_area {
    uint32_t log;
};
typedef struct xen_domctl_real_mode_area xen_domctl_real_mode_area_t;
typedef struct { xen_domctl_real_mode_area_t *p; } __guest_handle_xen_domctl_real_mode_area_t; typedef struct { const xen_domctl_real_mode_area_t *p; } __guest_handle_const_xen_domctl_real_mode_area_t;
struct xen_domctl_sendtrigger {
    uint32_t trigger;
    uint32_t vcpu;
};
typedef struct xen_domctl_sendtrigger xen_domctl_sendtrigger_t;
typedef struct { xen_domctl_sendtrigger_t *p; } __guest_handle_xen_domctl_sendtrigger_t; typedef struct { const xen_domctl_sendtrigger_t *p; } __guest_handle_const_xen_domctl_sendtrigger_t;
struct xen_domctl_assign_device {
    uint32_t machine_sbdf;
};
typedef struct xen_domctl_assign_device xen_domctl_assign_device_t;
typedef struct { xen_domctl_assign_device_t *p; } __guest_handle_xen_domctl_assign_device_t; typedef struct { const xen_domctl_assign_device_t *p; } __guest_handle_const_xen_domctl_assign_device_t;
struct xen_domctl_get_device_group {
    uint32_t machine_sbdf;
    uint32_t max_sdevs;
    uint32_t num_sdevs;
    __guest_handle_uint32 sdev_array;
};
typedef struct xen_domctl_get_device_group xen_domctl_get_device_group_t;
typedef struct { xen_domctl_get_device_group_t *p; } __guest_handle_xen_domctl_get_device_group_t; typedef struct { const xen_domctl_get_device_group_t *p; } __guest_handle_const_xen_domctl_get_device_group_t;
typedef enum pt_irq_type_e {
    PT_IRQ_TYPE_PCI,
    PT_IRQ_TYPE_ISA,
    PT_IRQ_TYPE_MSI,
    PT_IRQ_TYPE_MSI_TRANSLATE,
} pt_irq_type_t;
struct xen_domctl_bind_pt_irq {
    uint32_t machine_irq;
    pt_irq_type_t irq_type;
    uint32_t hvm_domid;
    union {
        struct {
            uint8_t isa_irq;
        } isa;
        struct {
            uint8_t bus;
            uint8_t device;
            uint8_t intx;
        } pci;
        struct {
            uint8_t gvec;
            uint32_t gflags;
            uint64_t gtable;
        } msi;
    } u;
};
typedef struct xen_domctl_bind_pt_irq xen_domctl_bind_pt_irq_t;
typedef struct { xen_domctl_bind_pt_irq_t *p; } __guest_handle_xen_domctl_bind_pt_irq_t; typedef struct { const xen_domctl_bind_pt_irq_t *p; } __guest_handle_const_xen_domctl_bind_pt_irq_t;
struct xen_domctl_memory_mapping {
    uint64_t first_gfn;
    uint64_t first_mfn;
    uint64_t nr_mfns;
    uint32_t add_mapping;
    uint32_t padding;
};
typedef struct xen_domctl_memory_mapping xen_domctl_memory_mapping_t;
typedef struct { xen_domctl_memory_mapping_t *p; } __guest_handle_xen_domctl_memory_mapping_t; typedef struct { const xen_domctl_memory_mapping_t *p; } __guest_handle_const_xen_domctl_memory_mapping_t;
struct xen_domctl_ioport_mapping {
    uint32_t first_gport;
    uint32_t first_mport;
    uint32_t nr_ports;
    uint32_t add_mapping;
};
typedef struct xen_domctl_ioport_mapping xen_domctl_ioport_mapping_t;
typedef struct { xen_domctl_ioport_mapping_t *p; } __guest_handle_xen_domctl_ioport_mapping_t; typedef struct { const xen_domctl_ioport_mapping_t *p; } __guest_handle_const_xen_domctl_ioport_mapping_t;
struct xen_domctl_pin_mem_cacheattr {
    uint64_t start, end;
    uint32_t type;
};
typedef struct xen_domctl_pin_mem_cacheattr xen_domctl_pin_mem_cacheattr_t;
typedef struct { xen_domctl_pin_mem_cacheattr_t *p; } __guest_handle_xen_domctl_pin_mem_cacheattr_t; typedef struct { const xen_domctl_pin_mem_cacheattr_t *p; } __guest_handle_const_xen_domctl_pin_mem_cacheattr_t;
struct xen_domctl_ext_vcpucontext {
    uint32_t vcpu;
    uint32_t size;
    uint64_t syscall32_callback_eip;
    uint64_t sysenter_callback_eip;
    uint16_t syscall32_callback_cs;
    uint16_t sysenter_callback_cs;
    uint8_t syscall32_disables_events;
    uint8_t sysenter_disables_events;
    union {
        uint64_t mcg_cap;
        struct hvm_vmce_vcpu vmce;
    };
};
typedef struct xen_domctl_ext_vcpucontext xen_domctl_ext_vcpucontext_t;
typedef struct { xen_domctl_ext_vcpucontext_t *p; } __guest_handle_xen_domctl_ext_vcpucontext_t; typedef struct { const xen_domctl_ext_vcpucontext_t *p; } __guest_handle_const_xen_domctl_ext_vcpucontext_t;
struct xen_domctl_set_target {
    domid_t target;
};
typedef struct xen_domctl_set_target xen_domctl_set_target_t;
typedef struct { xen_domctl_set_target_t *p; } __guest_handle_xen_domctl_set_target_t; typedef struct { const xen_domctl_set_target_t *p; } __guest_handle_const_xen_domctl_set_target_t;
struct xen_domctl_cpuid {
  uint32_t input[2];
  uint32_t eax;
  uint32_t ebx;
  uint32_t ecx;
  uint32_t edx;
};
typedef struct xen_domctl_cpuid xen_domctl_cpuid_t;
typedef struct { xen_domctl_cpuid_t *p; } __guest_handle_xen_domctl_cpuid_t; typedef struct { const xen_domctl_cpuid_t *p; } __guest_handle_const_xen_domctl_cpuid_t;
struct xen_domctl_subscribe {
    uint32_t port;
};
typedef struct xen_domctl_subscribe xen_domctl_subscribe_t;
typedef struct { xen_domctl_subscribe_t *p; } __guest_handle_xen_domctl_subscribe_t; typedef struct { const xen_domctl_subscribe_t *p; } __guest_handle_const_xen_domctl_subscribe_t;
struct xen_domctl_debug_op {
    uint32_t op;
    uint32_t vcpu;
};
typedef struct xen_domctl_debug_op xen_domctl_debug_op_t;
typedef struct { xen_domctl_debug_op_t *p; } __guest_handle_xen_domctl_debug_op_t; typedef struct { const xen_domctl_debug_op_t *p; } __guest_handle_const_xen_domctl_debug_op_t;
typedef struct xen_domctl_hvmcontext_partial {
    uint32_t type;
    uint32_t instance;
    __guest_handle_uint8 buffer;
} xen_domctl_hvmcontext_partial_t;
typedef struct { xen_domctl_hvmcontext_partial_t *p; } __guest_handle_xen_domctl_hvmcontext_partial_t; typedef struct { const xen_domctl_hvmcontext_partial_t *p; } __guest_handle_const_xen_domctl_hvmcontext_partial_t;
typedef struct xen_domctl_disable_migrate {
    uint32_t disable;
} xen_domctl_disable_migrate_t;
struct xen_guest_tsc_info {
    uint32_t tsc_mode;
    uint32_t gtsc_khz;
    uint32_t incarnation;
    uint32_t pad;
    uint64_t elapsed_nsec;
};
typedef struct xen_guest_tsc_info xen_guest_tsc_info_t;
typedef struct { xen_guest_tsc_info_t *p; } __guest_handle_xen_guest_tsc_info_t; typedef struct { const xen_guest_tsc_info_t *p; } __guest_handle_const_xen_guest_tsc_info_t;
typedef struct xen_domctl_tsc_info {
    __guest_handle_xen_guest_tsc_info_t out_info;
    xen_guest_tsc_info_t info;
} xen_domctl_tsc_info_t;
struct xen_domctl_gdbsx_memio {
    uint64_t pgd3val;
    uint64_t gva;
    uint64_t uva;
    uint32_t len;
    uint8_t gwr;
    uint32_t remain;
};
struct xen_domctl_gdbsx_pauseunp_vcpu {
    uint32_t vcpu;
};
struct xen_domctl_gdbsx_domstatus {
    uint8_t paused;
    uint32_t vcpu_id;
    uint32_t vcpu_ev;
};
struct xen_domctl_mem_event_op {
    uint32_t op;
    uint32_t mode;
    uint32_t port;
};
typedef struct xen_domctl_mem_event_op xen_domctl_mem_event_op_t;
typedef struct { xen_domctl_mem_event_op_t *p; } __guest_handle_xen_domctl_mem_event_op_t; typedef struct { const xen_domctl_mem_event_op_t *p; } __guest_handle_const_xen_domctl_mem_event_op_t;
struct xen_domctl_mem_sharing_op {
    uint8_t op;
    union {
        uint8_t enable;
    } u;
};
typedef struct xen_domctl_mem_sharing_op xen_domctl_mem_sharing_op_t;
typedef struct { xen_domctl_mem_sharing_op_t *p; } __guest_handle_xen_domctl_mem_sharing_op_t; typedef struct { const xen_domctl_mem_sharing_op_t *p; } __guest_handle_const_xen_domctl_mem_sharing_op_t;
struct xen_domctl_audit_p2m {
    uint64_t orphans;
    uint64_t m2p_bad;
    uint64_t p2m_bad;
};
typedef struct xen_domctl_audit_p2m xen_domctl_audit_p2m_t;
typedef struct { xen_domctl_audit_p2m_t *p; } __guest_handle_xen_domctl_audit_p2m_t; typedef struct { const xen_domctl_audit_p2m_t *p; } __guest_handle_const_xen_domctl_audit_p2m_t;
struct xen_domctl_set_virq_handler {
    uint32_t virq;
};
typedef struct xen_domctl_set_virq_handler xen_domctl_set_virq_handler_t;
typedef struct { xen_domctl_set_virq_handler_t *p; } __guest_handle_xen_domctl_set_virq_handler_t; typedef struct { const xen_domctl_set_virq_handler_t *p; } __guest_handle_const_xen_domctl_set_virq_handler_t;
struct xen_domctl_vcpuextstate {
    uint32_t vcpu;
    uint64_t xfeature_mask;
    uint64_t size;
    __guest_handle_uint64 buffer;
};
typedef struct xen_domctl_vcpuextstate xen_domctl_vcpuextstate_t;
typedef struct { xen_domctl_vcpuextstate_t *p; } __guest_handle_xen_domctl_vcpuextstate_t; typedef struct { const xen_domctl_vcpuextstate_t *p; } __guest_handle_const_xen_domctl_vcpuextstate_t;
struct xen_domctl_set_access_required {
    uint8_t access_required;
};
typedef struct xen_domctl_set_access_required xen_domctl_set_access_required_t;
typedef struct { xen_domctl_set_access_required_t *p; } __guest_handle_xen_domctl_set_access_required_t; typedef struct { const xen_domctl_set_access_required_t *p; } __guest_handle_const_xen_domctl_set_access_required_t;
struct xen_domctl_set_broken_page_p2m {
    uint64_t pfn;
};
typedef struct xen_domctl_set_broken_page_p2m xen_domctl_set_broken_page_p2m_t;
typedef struct { xen_domctl_set_broken_page_p2m_t *p; } __guest_handle_xen_domctl_set_broken_page_p2m_t; typedef struct { const xen_domctl_set_broken_page_p2m_t *p; } __guest_handle_const_xen_domctl_set_broken_page_p2m_t;
struct xen_domctl_set_max_evtchn {
    uint32_t max_port;
};
typedef struct xen_domctl_set_max_evtchn xen_domctl_set_max_evtchn_t;
typedef struct { xen_domctl_set_max_evtchn_t *p; } __guest_handle_xen_domctl_set_max_evtchn_t; typedef struct { const xen_domctl_set_max_evtchn_t *p; } __guest_handle_const_xen_domctl_set_max_evtchn_t;
struct xen_domctl_cacheflush {
    xen_pfn_t start_pfn, nr_pfns;
};
typedef struct xen_domctl_cacheflush xen_domctl_cacheflush_t;
typedef struct { xen_domctl_cacheflush_t *p; } __guest_handle_xen_domctl_cacheflush_t; typedef struct { const xen_domctl_cacheflush_t *p; } __guest_handle_const_xen_domctl_cacheflush_t;
struct xen_domctl {
    uint32_t cmd;
    uint32_t interface_version;
    domid_t domain;
    union {
        struct xen_domctl_createdomain createdomain;
        struct xen_domctl_getdomaininfo getdomaininfo;
        struct xen_domctl_getmemlist getmemlist;
        struct xen_domctl_getpageframeinfo getpageframeinfo;
        struct xen_domctl_getpageframeinfo2 getpageframeinfo2;
        struct xen_domctl_getpageframeinfo3 getpageframeinfo3;
        struct xen_domctl_nodeaffinity nodeaffinity;
        struct xen_domctl_vcpuaffinity vcpuaffinity;
        struct xen_domctl_shadow_op shadow_op;
        struct xen_domctl_max_mem max_mem;
        struct xen_domctl_vcpucontext vcpucontext;
        struct xen_domctl_getvcpuinfo getvcpuinfo;
        struct xen_domctl_max_vcpus max_vcpus;
        struct xen_domctl_scheduler_op scheduler_op;
        struct xen_domctl_setdomainhandle setdomainhandle;
        struct xen_domctl_setdebugging setdebugging;
        struct xen_domctl_irq_permission irq_permission;
        struct xen_domctl_iomem_permission iomem_permission;
        struct xen_domctl_ioport_permission ioport_permission;
        struct xen_domctl_hypercall_init hypercall_init;
        struct xen_domctl_arch_setup arch_setup;
        struct xen_domctl_settimeoffset settimeoffset;
        struct xen_domctl_disable_migrate disable_migrate;
        struct xen_domctl_tsc_info tsc_info;
        struct xen_domctl_real_mode_area real_mode_area;
        struct xen_domctl_hvmcontext hvmcontext;
        struct xen_domctl_hvmcontext_partial hvmcontext_partial;
        struct xen_domctl_address_size address_size;
        struct xen_domctl_sendtrigger sendtrigger;
        struct xen_domctl_get_device_group get_device_group;
        struct xen_domctl_assign_device assign_device;
        struct xen_domctl_bind_pt_irq bind_pt_irq;
        struct xen_domctl_memory_mapping memory_mapping;
        struct xen_domctl_ioport_mapping ioport_mapping;
        struct xen_domctl_pin_mem_cacheattr pin_mem_cacheattr;
        struct xen_domctl_ext_vcpucontext ext_vcpucontext;
        struct xen_domctl_set_target set_target;
        struct xen_domctl_subscribe subscribe;
        struct xen_domctl_debug_op debug_op;
        struct xen_domctl_mem_event_op mem_event_op;
        struct xen_domctl_mem_sharing_op mem_sharing_op;
        struct xen_domctl_cpuid cpuid;
        struct xen_domctl_vcpuextstate vcpuextstate;
        struct xen_domctl_set_access_required access_required;
        struct xen_domctl_audit_p2m audit_p2m;
        struct xen_domctl_set_virq_handler set_virq_handler;
        struct xen_domctl_set_max_evtchn set_max_evtchn;
        struct xen_domctl_gdbsx_memio gdbsx_guest_memio;
        struct xen_domctl_set_broken_page_p2m set_broken_page_p2m;
        struct xen_domctl_cacheflush cacheflush;
        struct xen_domctl_gdbsx_pauseunp_vcpu gdbsx_pauseunp_vcpu;
        struct xen_domctl_gdbsx_domstatus gdbsx_domstatus;
        uint8_t pad[128];
    } u;
};
typedef struct xen_domctl xen_domctl_t;
typedef struct { xen_domctl_t *p; } __guest_handle_xen_domctl_t; typedef struct { const xen_domctl_t *p; } __guest_handle_const_xen_domctl_t;
typedef uint32_t compat_ptr_t;
typedef unsigned long full_ptr_t;
enum XLAT_mmuext_op_arg1 {
    XLAT_mmuext_op_arg1_mfn,
    XLAT_mmuext_op_arg1_linear_addr,
};
enum XLAT_mmuext_op_arg2 {
    XLAT_mmuext_op_arg2_nr_ents,
    XLAT_mmuext_op_arg2_vcpumask,
    XLAT_mmuext_op_arg2_src_mfn,
};
enum XLAT_start_info_console {
    XLAT_start_info_console_domU,
    XLAT_start_info_console_dom0,
};
enum XLAT_gnttab_copy_source_u {
    XLAT_gnttab_copy_source_u_ref,
    XLAT_gnttab_copy_source_u_gmfn,
};
enum XLAT_gnttab_copy_dest_u {
    XLAT_gnttab_copy_dest_u_ref,
    XLAT_gnttab_copy_dest_u_gmfn,
};
enum XLAT_tmem_op_u {
    XLAT_tmem_op_u_creat,
    XLAT_tmem_op_u_ctrl,
    XLAT_tmem_op_u_gen,
};
int hypercall_xlat_continuation(unsigned int *id, unsigned int mask, ...);
struct start_info;
void xlat_start_info(struct start_info *, enum XLAT_start_info_console);
struct vcpu_runstate_info;
void xlat_vcpu_runstate_info(struct vcpu_runstate_info *);
int switch_compat(struct domain *);
int switch_native(struct domain *);
#pragma pack(4)
#pragma pack(4)
#pragma pack(4)
#pragma pack(4)
#pragma pack(4)
struct compat_cpu_user_regs {
    uint32_t ebx;
    uint32_t ecx;
    uint32_t edx;
    uint32_t esi;
    uint32_t edi;
    uint32_t ebp;
    uint32_t eax;
    uint16_t error_code;
    uint16_t entry_vector;
    uint32_t eip;
    uint16_t cs;
    uint8_t saved_upcall_mask;
    uint8_t _pad0;
    uint32_t eflags;
    uint32_t esp;
    uint16_t ss, _pad1;
    uint16_t es, _pad2;
    uint16_t ds, _pad3;
    uint16_t fs, _pad4;
    uint16_t gs, _pad5;
};
typedef struct compat_cpu_user_regs cpu_user_regs_compat_t;
typedef struct { compat_ptr_t c; cpu_user_regs_compat_t *_[0] __attribute__((__packed__)); } __compat_handle_cpu_user_regs_compat_t; typedef struct { compat_ptr_t c; const cpu_user_regs_compat_t *_[0] __attribute__((__packed__)); } __compat_handle_const_cpu_user_regs_compat_t;
struct compat_arch_vcpu_info {
    unsigned int cr2;
    unsigned int pad[5];
};
typedef struct compat_arch_vcpu_info arch_vcpu_info_compat_t;
struct compat_callback {
    unsigned int cs;
    unsigned int eip;
};
typedef struct compat_callback compat_callback_t;
#pragma pack()
#pragma pack(4)
typedef unsigned int compat_pfn_t;
typedef unsigned int compat_ulong_t;
struct compat_trap_info {
    uint8_t vector;
    uint8_t flags;
    uint16_t cs;
    unsigned int address;
};
typedef struct compat_trap_info trap_info_compat_t;
typedef struct { compat_ptr_t c; trap_info_compat_t *_[0] __attribute__((__packed__)); } __compat_handle_trap_info_compat_t; typedef struct { compat_ptr_t c; const trap_info_compat_t *_[0] __attribute__((__packed__)); } __compat_handle_const_trap_info_compat_t;
typedef uint64_t tsc_timestamp_compat_t;
struct compat_vcpu_guest_context {
    struct { char x[512]; } fpu_ctxt;
    unsigned int flags;
    struct compat_cpu_user_regs user_regs;
    struct compat_trap_info trap_ctxt[256];
    unsigned int ldt_base, ldt_ents;
    unsigned int gdt_frames[16], gdt_ents;
    unsigned int kernel_ss, kernel_sp;
    unsigned int ctrlreg[8];
    unsigned int debugreg[8];
    unsigned int event_callback_cs;
    unsigned int event_callback_eip;
    unsigned int failsafe_callback_cs;
    unsigned int failsafe_callback_eip;
    unsigned int vm_assist;
};
typedef struct compat_vcpu_guest_context vcpu_guest_context_compat_t;
typedef struct { compat_ptr_t c; vcpu_guest_context_compat_t *_[0] __attribute__((__packed__)); } __compat_handle_vcpu_guest_context_compat_t; typedef struct { compat_ptr_t c; const vcpu_guest_context_compat_t *_[0] __attribute__((__packed__)); } __compat_handle_const_vcpu_guest_context_compat_t;
struct compat_arch_shared_info {
    unsigned int max_pfn;
    compat_pfn_t pfn_to_mfn_frame_list_list;
    unsigned int nmi_reason;
    uint64_t pad[32];
};
typedef struct compat_arch_shared_info arch_shared_info_compat_t;
#pragma pack()
#pragma pack(4)
typedef struct { compat_ptr_t c; char *_[0] __attribute__((__packed__)); } __compat_handle_char; typedef struct { compat_ptr_t c; const char *_[0] __attribute__((__packed__)); } __compat_handle_const_char;
typedef struct { compat_ptr_t c; unsigned char *_[0] __attribute__((__packed__)); } __compat_handle_uchar;
typedef struct { compat_ptr_t c; int *_[0] __attribute__((__packed__)); } __compat_handle_int; typedef struct { compat_ptr_t c; const int *_[0] __attribute__((__packed__)); } __compat_handle_const_int;
typedef struct { compat_ptr_t c; unsigned int *_[0] __attribute__((__packed__)); } __compat_handle_uint;
typedef struct { compat_ptr_t c; void *_[0] __attribute__((__packed__)); } __compat_handle_void; typedef struct { compat_ptr_t c; const void *_[0] __attribute__((__packed__)); } __compat_handle_const_void;
typedef struct { compat_ptr_t c; uint64_t *_[0] __attribute__((__packed__)); } __compat_handle_uint64_t; typedef struct { compat_ptr_t c; const uint64_t *_[0] __attribute__((__packed__)); } __compat_handle_const_uint64_t;
typedef struct { compat_ptr_t c; compat_pfn_t *_[0] __attribute__((__packed__)); } __compat_handle_compat_pfn_t; typedef struct { compat_ptr_t c; const compat_pfn_t *_[0] __attribute__((__packed__)); } __compat_handle_const_compat_pfn_t;
typedef struct { compat_ptr_t c; compat_ulong_t *_[0] __attribute__((__packed__)); } __compat_handle_compat_ulong_t; typedef struct { compat_ptr_t c; const compat_ulong_t *_[0] __attribute__((__packed__)); } __compat_handle_const_compat_ulong_t;
struct compat_mmuext_op {
    unsigned int cmd;
    union {
        compat_pfn_t mfn;
        unsigned int linear_addr;
    } arg1;
    union {
        unsigned int nr_ents;
        __compat_handle_const_void vcpumask;
        compat_pfn_t src_mfn;
    } arg2;
};
typedef struct compat_mmuext_op mmuext_op_compat_t;
typedef struct { compat_ptr_t c; mmuext_op_compat_t *_[0] __attribute__((__packed__)); } __compat_handle_mmuext_op_compat_t; typedef struct { compat_ptr_t c; const mmuext_op_compat_t *_[0] __attribute__((__packed__)); } __compat_handle_const_mmuext_op_compat_t;
typedef uint16_t domid_compat_t;
struct compat_mmu_update {
    uint64_t ptr;
    uint64_t val;
};
typedef struct mmu_update mmu_update_compat_t;
typedef struct { compat_ptr_t c; mmu_update_compat_t *_[0] __attribute__((__packed__)); } __compat_handle_mmu_update_compat_t; typedef struct { compat_ptr_t c; const mmu_update_compat_t *_[0] __attribute__((__packed__)); } __compat_handle_const_mmu_update_compat_t;
struct compat_multicall_entry {
    unsigned int op, result;
    unsigned int args[6];
};
typedef struct compat_multicall_entry multicall_entry_compat_t;
typedef struct { compat_ptr_t c; multicall_entry_compat_t *_[0] __attribute__((__packed__)); } __compat_handle_multicall_entry_compat_t; typedef struct { compat_ptr_t c; const multicall_entry_compat_t *_[0] __attribute__((__packed__)); } __compat_handle_const_multicall_entry_compat_t;
struct compat_vcpu_time_info {
    uint32_t version;
    uint32_t pad0;
    uint64_t tsc_timestamp;
    uint64_t system_time;
    uint32_t tsc_to_system_mul;
    int8_t tsc_shift;
    int8_t pad1[3];
};
typedef struct vcpu_time_info vcpu_time_info_compat_t;
struct compat_vcpu_info {
    uint8_t evtchn_upcall_pending;
    uint8_t evtchn_upcall_mask;
    compat_ulong_t evtchn_pending_sel;
    struct compat_arch_vcpu_info arch;
    struct vcpu_time_info time;
};
struct compat_shared_info {
    struct vcpu_info vcpu_info[32];
    compat_ulong_t evtchn_pending[sizeof(compat_ulong_t) * 8];
    compat_ulong_t evtchn_mask[sizeof(compat_ulong_t) * 8];
    uint32_t wc_version;
    uint32_t wc_sec;
    uint32_t wc_nsec;
    struct compat_arch_shared_info arch;
};
struct compat_start_info {
    char magic[32];
    unsigned int nr_pages;
    unsigned int shared_info;
    uint32_t flags;
    compat_pfn_t store_mfn;
    uint32_t store_evtchn;
    union {
        struct {
            compat_pfn_t mfn;
            uint32_t evtchn;
        } domU;
        struct {
            uint32_t info_off;
            uint32_t info_size;
        } dom0;
    } console;
    unsigned int pt_base;
    unsigned int nr_pt_frames;
    unsigned int mfn_list;
    unsigned int mod_start;
    unsigned int mod_len;
    int8_t cmd_line[1024];
    unsigned int first_p2m_pfn;
    unsigned int nr_p2m_frames;
};
typedef struct compat_start_info start_info_compat_t;
struct compat_multiboot_mod_list
{
    uint32_t mod_start;
    uint32_t mod_end;
    uint32_t cmdline;
    uint32_t pad;
};
typedef struct compat_dom0_vga_console_info {
    uint8_t video_type;
    union {
        struct {
            uint16_t font_height;
            uint16_t cursor_x, cursor_y;
            uint16_t rows, columns;
        } text_mode_3;
        struct {
            uint16_t width, height;
            uint16_t bytes_per_line;
            uint16_t bits_per_pixel;
            uint32_t lfb_base;
            uint32_t lfb_size;
            uint8_t red_pos, red_size;
            uint8_t green_pos, green_size;
            uint8_t blue_pos, blue_size;
            uint8_t rsvd_pos, rsvd_size;
            uint32_t gbl_caps;
            uint16_t mode_attrs;
        } vesa_lfb;
    } u;
} dom0_vga_console_info_compat_t;
typedef uint8_t compat_domain_handle_t[16];
typedef struct { compat_ptr_t c; uint8_t *_[0] __attribute__((__packed__)); } __compat_handle_uint8;
typedef struct { compat_ptr_t c; uint16_t *_[0] __attribute__((__packed__)); } __compat_handle_uint16;
typedef struct { compat_ptr_t c; uint32_t *_[0] __attribute__((__packed__)); } __compat_handle_uint32;
typedef struct { compat_ptr_t c; uint64_t *_[0] __attribute__((__packed__)); } __compat_handle_uint64;
struct compat_ctl_bitmap {
    __compat_handle_uint8 bitmap;
    uint32_t nr_bits;
};
#pragma pack()
typedef union {
    struct shared_info native;
    struct compat_shared_info compat;
} shared_info_t;
typedef union {
    struct vcpu_info native;
    struct compat_vcpu_info compat;
} vcpu_info_t;
extern vcpu_info_t dummy_vcpu_info;
extern int init_xen_time(void);
extern void cstate_restore_tsc(void);
extern unsigned long cpu_khz;
extern unsigned long pit0_ticks;
struct domain;
struct vcpu;
typedef s64 s_time_t;
s_time_t get_s_time(void);
unsigned long get_localtime(struct domain *d);
uint64_t get_localtime_us(struct domain *d);
struct tm {
    int tm_sec;
    int tm_min;
    int tm_hour;
    int tm_mday;
    int tm_mon;
    int tm_year;
    int tm_wday;
    int tm_yday;
    int tm_isdst;
};
struct tm gmtime(unsigned long t);
extern void update_vcpu_system_time(struct vcpu *v);
extern void update_domain_wallclock_time(struct domain *d);
extern void do_settime(
    unsigned long secs, unsigned long nsecs, u64 system_time_base);
extern void send_timer_event(struct vcpu *v);
void domain_set_time_offset(struct domain *d, int32_t time_offset_seconds);
static __inline__ void wrmsrl(unsigned int msr, __u64 val)
{
        __u32 lo, hi;
        lo = (__u32)val;
        hi = (__u32)(val >> 32);
        __asm__ __volatile__("wrmsr" : : "c" (msr), "a" (lo), "d" (hi));
}
static __inline__ int wrmsr_safe(unsigned int msr, uint64_t val)
{
    int _rc;
    uint32_t lo, hi;
    lo = (uint32_t)val;
    hi = (uint32_t)(val >> 32);
    __asm__ __volatile__(
        "1: wrmsr\n2:\n"
        ".section .fixup,\"ax\"\n"
        "3: movl %5,%0\n; jmp 2b\n"
        ".previous\n"
        " .section .ex_table" "" ",\"a\"\n" " .balign 4\n" " .long " "1b" "-." ", " "3b" "-." "\n" " .previous\n"
        : "=&r" (_rc)
        : "c" (msr), "a" (lo), "d" (hi), "0" (0), "i" (-14));
    return _rc;
}
static __inline__ unsigned long __rdfsbase(void)
{
    unsigned long base;
    asm volatile ( "rdfsbase %0" : "=r" (base) );
    return base;
}
static __inline__ unsigned long __rdgsbase(void)
{
    unsigned long base;
    asm volatile ( "rdgsbase %0" : "=r" (base) );
    return base;
}
static __inline__ unsigned long rdfsbase(void)
{
    unsigned long base;
    if ( ({ if ( (sizeof(*(boot_cpu_data.x86_capability)) < 4) ) __bitop_bad_size(); (__builtin_constant_p((7*32+ 0)) ? constant_test_bit(((7*32+ 0)),(boot_cpu_data.x86_capability)) : variable_test_bit(((7*32+ 0)),(boot_cpu_data.x86_capability))); }) )
        return __rdfsbase();
    do { unsigned long a__,b__; __asm__ __volatile__("rdmsr" : "=a" (a__), "=d" (b__) : "c" (0xc0000100)); base = a__ | ((u64)b__<<32); } while(0);
    return base;
}
static __inline__ unsigned long rdgsbase(void)
{
    unsigned long base;
    if ( ({ if ( (sizeof(*(boot_cpu_data.x86_capability)) < 4) ) __bitop_bad_size(); (__builtin_constant_p((7*32+ 0)) ? constant_test_bit(((7*32+ 0)),(boot_cpu_data.x86_capability)) : variable_test_bit(((7*32+ 0)),(boot_cpu_data.x86_capability))); }) )
        return __rdgsbase();
    do { unsigned long a__,b__; __asm__ __volatile__("rdmsr" : "=a" (a__), "=d" (b__) : "c" (0xc0000101)); base = a__ | ((u64)b__<<32); } while(0);
    return base;
}
static __inline__ void wrfsbase(unsigned long base)
{
    if ( ({ if ( (sizeof(*(boot_cpu_data.x86_capability)) < 4) ) __bitop_bad_size(); (__builtin_constant_p((7*32+ 0)) ? constant_test_bit(((7*32+ 0)),(boot_cpu_data.x86_capability)) : variable_test_bit(((7*32+ 0)),(boot_cpu_data.x86_capability))); }) )
        asm volatile ( "wrfsbase %0" :: "r" (base) );
    else
        wrmsrl(0xc0000100, base);
}
static __inline__ void wrgsbase(unsigned long base)
{
    if ( ({ if ( (sizeof(*(boot_cpu_data.x86_capability)) < 4) ) __bitop_bad_size(); (__builtin_constant_p((7*32+ 0)) ? constant_test_bit(((7*32+ 0)),(boot_cpu_data.x86_capability)) : variable_test_bit(((7*32+ 0)),(boot_cpu_data.x86_capability))); }) )
        asm volatile ( "wrgsbase %0" :: "r" (base) );
    else
        wrmsrl(0xc0000101, base);
}
extern __typeof__(u64) per_cpu__efer;
u64 read_efer(void);
void write_efer(u64 val);
extern __typeof__(u32) per_cpu__ler_msr;
typedef u64 cycles_t;
extern bool_t disable_tsc_sync;
static __inline__ cycles_t get_cycles(void)
{
    cycles_t c;
    do { unsigned int a,d; asm volatile("rdtsc" : "=a" (a), "=d" (d)); (c) = ((unsigned long)a) | (((unsigned long)d)<<32); } while(0);
    return c;
}
unsigned long
mktime (unsigned int year, unsigned int mon,
        unsigned int day, unsigned int hour,
        unsigned int min, unsigned int sec);
int time_suspend(void);
int time_resume(void);
void init_percpu_time(void);
struct ioreq;
int dom0_pit_access(struct ioreq *ioreq);
int cpu_frequency_change(u64 freq);
struct tm;
struct tm wallclock_time(void);
void pit_broadcast_enter(void);
void pit_broadcast_exit(void);
int pit_broadcast_is_available(void);
uint64_t acpi_pm_tick_to_ns(uint64_t ticks);
uint64_t ns_to_acpi_pm_tick(uint64_t ns);
uint64_t tsc_ticks2ns(uint64_t ticks);
void pv_soft_rdtsc(struct vcpu *v, struct cpu_user_regs *regs, int rdtscp);
u64 gtime_to_gtsc(struct domain *d, u64 time);
u64 gtsc_to_gtime(struct domain *d, u64 tsc);
void tsc_set_info(struct domain *d, uint32_t tsc_mode, uint64_t elapsed_nsec,
                  uint32_t gtsc_khz, uint32_t incarnation);
void tsc_get_info(struct domain *d, uint32_t *tsc_mode, uint64_t *elapsed_nsec,
                  uint32_t *gtsc_khz, uint32_t *incarnation);
void force_update_vcpu_system_time(struct vcpu *v);
int host_tsc_is_safe(void);
void cpuid_time_leaf(uint32_t sub_idx, unsigned int *eax, unsigned int *ebx,
                      unsigned int *ecx, unsigned int *edx);
u64 stime2tsc(s_time_t stime);
struct timer {
    s_time_t expires;
    union {
        unsigned int heap_offset;
        struct timer *list_next;
        struct list_head inactive;
    };
    void (*function)(void *);
    void *data;
    uint16_t cpu;
    uint8_t status;
};
void init_timer(
    struct timer *timer,
    void (*function)(void *),
    void *data,
    unsigned int cpu);
void set_timer(struct timer *timer, s_time_t expires);
void stop_timer(struct timer *timer);
void migrate_timer(struct timer *timer, unsigned int new_cpu);
void kill_timer(struct timer *timer);
void timer_init(void);
extern __typeof__(s_time_t) per_cpu__timer_deadline;
int reprogram_timer(s_time_t timeout);
s_time_t align_timer(s_time_t firsttick, uint64_t period);
struct domain;
struct rangeset;
void rangeset_domain_initialise(
    struct domain *d);
void rangeset_domain_destroy(
    struct domain *d);
struct rangeset *rangeset_new(
    struct domain *d, char *name, unsigned int flags);
void rangeset_destroy(
    struct rangeset *r);
int __attribute__((warn_unused_result)) rangeset_is_empty(
    struct rangeset *r);
int __attribute__((warn_unused_result)) rangeset_add_range(
    struct rangeset *r, unsigned long s, unsigned long e);
int __attribute__((warn_unused_result)) rangeset_remove_range(
    struct rangeset *r, unsigned long s, unsigned long e);
int __attribute__((warn_unused_result)) rangeset_contains_range(
    struct rangeset *r, unsigned long s, unsigned long e);
int __attribute__((warn_unused_result)) rangeset_overlaps_range(
    struct rangeset *r, unsigned long s, unsigned long e);
int rangeset_report_ranges(
    struct rangeset *r, unsigned long s, unsigned long e,
    int (*cb)(unsigned long s, unsigned long e, void *), void *ctxt);
int __attribute__((warn_unused_result)) rangeset_add_singleton(
    struct rangeset *r, unsigned long s);
int __attribute__((warn_unused_result)) rangeset_remove_singleton(
    struct rangeset *r, unsigned long s);
int __attribute__((warn_unused_result)) rangeset_contains_singleton(
    struct rangeset *r, unsigned long s);
void rangeset_printk(
    struct rangeset *r);
void rangeset_domain_printk(
    struct domain *d);
extern __typeof__(unsigned int) per_cpu____preempt_count;
bool_t in_atomic(void);
struct rcu_head {
    struct rcu_head *next;
    void (*func)(struct rcu_head *head);
};
int rcu_pending(int cpu);
int rcu_needs_cpu(int cpu);
struct _rcu_read_lock {};
typedef struct _rcu_read_lock rcu_read_lock_t;
void rcu_init(void);
void rcu_check_callbacks(int cpu);
void call_rcu(struct rcu_head *head,
              void (*func)(struct rcu_head *head));
int rcu_barrier(void);
static __inline__ int radix_tree_is_indirect_ptr(void *ptr)
{
 return (int)((unsigned long)ptr & 1);
}
struct radix_tree_node {
 unsigned int height;
 unsigned int count;
 void *slots[(1UL << 6)];
};
typedef struct radix_tree_node *radix_tree_alloc_fn_t(void *);
typedef void radix_tree_free_fn_t(struct radix_tree_node *, void *);
struct radix_tree_root {
 unsigned int height;
 struct radix_tree_node *rnode;
 radix_tree_alloc_fn_t *node_alloc;
 radix_tree_free_fn_t *node_free;
 void *node_alloc_free_arg;
};
void radix_tree_init(struct radix_tree_root *root);
void radix_tree_set_alloc_callbacks(
 struct radix_tree_root *root,
 radix_tree_alloc_fn_t *node_alloc,
 radix_tree_free_fn_t *node_free,
 void *node_alloc_free_arg);
void radix_tree_destroy(
 struct radix_tree_root *root,
 void (*slot_free)(void *));
static __inline__ void *radix_tree_deref_slot(void **pslot)
{
 return (*pslot);
}
static __inline__ int radix_tree_deref_retry(void *arg)
{
 return __builtin_expect(((unsigned long)arg & 1),0);
}
static __inline__ void radix_tree_replace_slot(void **pslot, void *item)
{
 do { if (__builtin_expect((radix_tree_is_indirect_ptr(item)),0)) do { do { ((void)sizeof(struct { int:-!!((168) >> ((31 - 24) + (31 - 24))); })); asm volatile ( ".Lbug%=: ud2\n" ".pushsection .bug_frames.%c0, \"a\", @progbits\n" ".p2align 2\n" ".Lfrm%=:\n" ".long (.Lbug%= - .Lfrm%=) + %c4\n" ".long (%c1 - .Lfrm%=) + %c3\n" ".if " "0" "\n" ".long 0, %c2 - .Lfrm%=\n" ".endif\n" ".popsection" : : "i" (2), "i" ("/home/dquinlan/ROSE/Zen/rose-sh/workspace/xen/phase_1/xen-src/xen/include/xen/radix-tree.h"), "i" (((void*)0)), "i" ((168 & ((1 << (31 - 24)) - 1)) << 24), "i" (((168) >> (31 - 24)) << 24)); } while (0); do {} while (1); } while (0); } while (0);
 ({ __asm__ __volatile__("": : :"memory"); (*pslot) = (item); });
}
static __inline__ void *radix_tree_int_to_ptr(int val)
{
    long _ptr = ((long)val << 2) | 0x2l;
    do { if ( 0 && ((_ptr >> 2) == val) ); } while (0);
    return (void *)_ptr;
}
static __inline__ int radix_tree_ptr_to_int(void *ptr)
{
    do { if ( 0 && (((long)ptr & 0x3) == 0x2) ); } while (0);
    return (int)((long)ptr >> 2);
}
int radix_tree_insert(struct radix_tree_root *, unsigned long, void *);
void *radix_tree_lookup(struct radix_tree_root *, unsigned long);
void **radix_tree_lookup_slot(struct radix_tree_root *, unsigned long);
void *radix_tree_delete(struct radix_tree_root *, unsigned long);
unsigned int
radix_tree_gang_lookup(struct radix_tree_root *root, void **results,
   unsigned long first_index, unsigned int max_items);
unsigned int
radix_tree_gang_lookup_slot(struct radix_tree_root *root, void ***results,
   unsigned long first_index, unsigned int max_items);
unsigned long radix_tree_next_hole(struct radix_tree_root *root,
    unsigned long index, unsigned long max_scan);
unsigned long radix_tree_prev_hole(struct radix_tree_root *root,
    unsigned long index, unsigned long max_scan);
struct tasklet
{
    struct list_head list;
    int scheduled_on;
    bool_t is_softirq;
    bool_t is_running;
    bool_t is_dead;
    void (*func)(unsigned long);
    unsigned long data;
};
extern __typeof__(unsigned long) per_cpu__tasklet_work_to_do;
void tasklet_schedule_on_cpu(struct tasklet *t, unsigned int cpu);
void tasklet_schedule(struct tasklet *t);
void do_tasklet(void);
void tasklet_kill(struct tasklet *t);
void tasklet_init(
    struct tasklet *t, void (*func)(unsigned long), unsigned long data);
void softirq_tasklet_init(
    struct tasklet *t, void (*func)(unsigned long), unsigned long data);
void tasklet_subsys_init(void);
void vpic_irq_positive_edge(struct domain *d, int irq);
void vpic_irq_negative_edge(struct domain *d, int irq);
void vpic_init(struct domain *d);
void vpic_reset(struct domain *d);
int vpic_ack_pending_irq(struct vcpu *v);
int is_periodic_irq(struct vcpu *v, int irq, int type);
struct hvm_vioapic {
    struct hvm_hw_vioapic hvm_hw_vioapic;
    struct domain *domain;
};
int vioapic_init(struct domain *d);
void vioapic_deinit(struct domain *d);
void vioapic_reset(struct domain *d);
void vioapic_irq_positive_edge(struct domain *d, unsigned int irq);
void vioapic_update_EOI(struct domain *d, int vector);
struct ioreq {
    uint64_t addr;
    uint64_t data;
    uint32_t count;
    uint32_t size;
    uint32_t vp_eport;
    uint16_t _pad0;
    uint8_t state:4;
    uint8_t data_is_ptr:1;
    uint8_t dir:1;
    uint8_t df:1;
    uint8_t _pad1:1;
    uint8_t type;
};
typedef struct ioreq ioreq_t;
struct shared_iopage {
    struct ioreq vcpu_ioreq[1];
};
typedef struct shared_iopage shared_iopage_t;
struct buf_ioreq {
    uint8_t type;
    uint8_t pad:1;
    uint8_t dir:1;
    uint8_t size:2;
    uint32_t addr:20;
    uint32_t data;
};
typedef struct buf_ioreq buf_ioreq_t;
struct buffered_iopage {
    unsigned int read_pointer;
    unsigned int write_pointer;
    buf_ioreq_t buf_ioreq[511];
};
typedef struct buffered_iopage buffered_iopage_t;
typedef uint32_t evtchn_port_t;
typedef struct { evtchn_port_t *p; } __guest_handle_evtchn_port_t; typedef struct { const evtchn_port_t *p; } __guest_handle_const_evtchn_port_t;
struct evtchn_alloc_unbound {
    domid_t dom, remote_dom;
    evtchn_port_t port;
};
typedef struct evtchn_alloc_unbound evtchn_alloc_unbound_t;
struct evtchn_bind_interdomain {
    domid_t remote_dom;
    evtchn_port_t remote_port;
    evtchn_port_t local_port;
};
typedef struct evtchn_bind_interdomain evtchn_bind_interdomain_t;
struct evtchn_bind_virq {
    uint32_t virq;
    uint32_t vcpu;
    evtchn_port_t port;
};
typedef struct evtchn_bind_virq evtchn_bind_virq_t;
struct evtchn_bind_pirq {
    uint32_t pirq;
    uint32_t flags;
    evtchn_port_t port;
};
typedef struct evtchn_bind_pirq evtchn_bind_pirq_t;
struct evtchn_bind_ipi {
    uint32_t vcpu;
    evtchn_port_t port;
};
typedef struct evtchn_bind_ipi evtchn_bind_ipi_t;
struct evtchn_close {
    evtchn_port_t port;
};
typedef struct evtchn_close evtchn_close_t;
struct evtchn_send {
    evtchn_port_t port;
};
typedef struct evtchn_send evtchn_send_t;
struct evtchn_status {
    domid_t dom;
    evtchn_port_t port;
    uint32_t status;
    uint32_t vcpu;
    union {
        struct {
            domid_t dom;
        } unbound;
        struct {
            domid_t dom;
            evtchn_port_t port;
        } interdomain;
        uint32_t pirq;
        uint32_t virq;
    } u;
};
typedef struct evtchn_status evtchn_status_t;
struct evtchn_bind_vcpu {
    evtchn_port_t port;
    uint32_t vcpu;
};
typedef struct evtchn_bind_vcpu evtchn_bind_vcpu_t;
struct evtchn_unmask {
    evtchn_port_t port;
};
typedef struct evtchn_unmask evtchn_unmask_t;
struct evtchn_reset {
    domid_t dom;
};
typedef struct evtchn_reset evtchn_reset_t;
struct evtchn_init_control {
    uint64_t control_gfn;
    uint32_t offset;
    uint32_t vcpu;
    uint8_t link_bits;
    uint8_t _pad[7];
};
typedef struct evtchn_init_control evtchn_init_control_t;
struct evtchn_expand_array {
    uint64_t array_gfn;
};
typedef struct evtchn_expand_array evtchn_expand_array_t;
struct evtchn_set_priority {
    uint32_t port;
    uint32_t priority;
};
typedef struct evtchn_set_priority evtchn_set_priority_t;
struct evtchn_op {
    uint32_t cmd;
    union {
        struct evtchn_alloc_unbound alloc_unbound;
        struct evtchn_bind_interdomain bind_interdomain;
        struct evtchn_bind_virq bind_virq;
        struct evtchn_bind_pirq bind_pirq;
        struct evtchn_bind_ipi bind_ipi;
        struct evtchn_close close;
        struct evtchn_send send;
        struct evtchn_status status;
        struct evtchn_bind_vcpu bind_vcpu;
        struct evtchn_unmask unmask;
    } u;
};
typedef struct evtchn_op evtchn_op_t;
typedef struct { evtchn_op_t *p; } __guest_handle_evtchn_op_t; typedef struct { const evtchn_op_t *p; } __guest_handle_const_evtchn_op_t;
typedef uint32_t event_word_t;
struct evtchn_fifo_control_block {
    uint32_t ready;
    uint32_t _rsvd;
    uint32_t head[(15 + 1)];
};
typedef struct evtchn_fifo_control_block evtchn_fifo_control_block_t;
typedef int (*hvm_mmio_read_t)(struct vcpu *v,
                               unsigned long addr,
                               unsigned long length,
                               unsigned long *val);
typedef int (*hvm_mmio_write_t)(struct vcpu *v,
                                unsigned long addr,
                                unsigned long length,
                                unsigned long val);
typedef int (*hvm_mmio_check_t)(struct vcpu *v, unsigned long addr);
typedef int (*portio_action_t)(
    int dir, uint32_t port, uint32_t bytes, uint32_t *val);
typedef int (*mmio_action_t)(ioreq_t *);
struct io_handler {
    int type;
    unsigned long addr;
    unsigned long size;
    union {
        portio_action_t portio;
        mmio_action_t mmio;
        void *ptr;
    } action;
};
struct hvm_io_handler {
    int num_slot;
    struct io_handler hdl_list[16];
};
struct hvm_mmio_handler {
    hvm_mmio_check_t check_handler;
    hvm_mmio_read_t read_handler;
    hvm_mmio_write_t write_handler;
};
extern const struct hvm_mmio_handler hpet_mmio_handler;
extern const struct hvm_mmio_handler vlapic_mmio_handler;
extern const struct hvm_mmio_handler vioapic_mmio_handler;
extern const struct hvm_mmio_handler msixtbl_mmio_handler;
extern const struct hvm_mmio_handler iommu_mmio_handler;
int hvm_io_intercept(ioreq_t *p, int type);
void register_io_handler(
    struct domain *d, unsigned long addr, unsigned long size,
    void *action, int type);
void relocate_io_handler(
    struct domain *d, unsigned long old_addr, unsigned long new_addr,
    unsigned long size, int type);
static __inline__ int hvm_portio_intercept(ioreq_t *p)
{
    return hvm_io_intercept(p, 0);
}
static __inline__ int hvm_buffered_io_intercept(ioreq_t *p)
{
    return hvm_io_intercept(p, 2);
}
int hvm_mmio_intercept(ioreq_t *p);
int hvm_buffered_io_send(ioreq_t *p);
static __inline__ void register_portio_handler(
    struct domain *d, unsigned long addr,
    unsigned long size, portio_action_t action)
{
    register_io_handler(d, addr, size, action, 0);
}
static __inline__ void relocate_portio_handler(
    struct domain *d, unsigned long old_addr, unsigned long new_addr,
    unsigned long size)
{
    relocate_io_handler(d, old_addr, new_addr, size, 0);
}
static __inline__ void register_buffered_io_handler(
    struct domain *d, unsigned long addr,
    unsigned long size, mmio_action_t action)
{
    register_io_handler(d, addr, size, action, 2);
}
void send_timeoffset_req(unsigned long timeoff);
void send_invalidate_req(void);
int handle_mmio(void);
int handle_mmio_with_translation(unsigned long gva, unsigned long gpfn);
int handle_pio(uint16_t port, unsigned int size, int dir);
void hvm_interrupt_post(struct vcpu *v, int vector, int type);
void hvm_io_assist(ioreq_t *p);
void hvm_dpci_eoi(struct domain *d, unsigned int guest_irq,
                  union vioapic_redir_entry *ent);
void msix_write_completion(struct vcpu *);
struct hvm_hw_stdvga {
    uint8_t sr_index;
    uint8_t sr[8];
    uint8_t gr_index;
    uint8_t gr[9];
    bool_t stdvga;
    bool_t cache;
    uint32_t latch;
    struct page_info *vram_page[64];
    spinlock_t lock;
};
void stdvga_init(struct domain *d);
void stdvga_deinit(struct domain *d);
extern void hvm_dpci_msi_eoi(struct domain *d, int vector);
typedef int (*initcall_t)(void);
typedef void (*exitcall_t)(void);
void do_presmp_initcalls(void);
void do_initcalls(void);
struct kernel_param {
    const char *name;
    enum {
        OPT_STR,
        OPT_UINT,
        OPT_BOOL,
        OPT_INVBOOL,
        OPT_SIZE,
        OPT_CUSTOM
    } type;
    void *var;
    unsigned int len;
};
extern struct kernel_param __setup_start, __setup_end;
struct dev_intx_gsi_link {
    struct list_head list;
    uint8_t device;
    uint8_t intx;
    uint8_t gsi;
    uint8_t link;
};
struct hvm_gmsi_info {
    uint32_t gvec;
    uint32_t gflags;
    int dest_vcpu_id;
};
struct hvm_girq_dpci_mapping {
    struct list_head list;
    uint8_t device;
    uint8_t intx;
    uint8_t machine_gsi;
};
struct hvm_irq_dpci {
    struct list_head girq[48];
    unsigned long isairq_map[(((16)+((1 << 3) << 3)-1)/((1 << 3) << 3))];
    uint8_t link_cnt[4];
    struct tasklet dirq_tasklet;
};
struct hvm_pirq_dpci {
    uint32_t flags;
    bool_t masked;
    uint16_t pending;
    struct list_head digl_list;
    struct domain *dom;
    struct hvm_gmsi_info gmsi;
    struct timer timer;
};
void pt_pirq_init(struct domain *, struct hvm_pirq_dpci *);
bool_t pt_pirq_cleanup_check(struct hvm_pirq_dpci *);
int pt_pirq_iterate(struct domain *d,
                    int (*cb)(struct domain *,
                              struct hvm_pirq_dpci *, void *arg),
                    void *arg);
void hvm_pci_intx_assert(
    struct domain *d, unsigned int device, unsigned int intx);
void hvm_pci_intx_deassert(
    struct domain *d, unsigned int device, unsigned int intx);
void hvm_isa_irq_assert(
    struct domain *d, unsigned int isa_irq);
void hvm_isa_irq_deassert(
    struct domain *d, unsigned int isa_irq);
void hvm_set_pci_link_route(struct domain *d, u8 link, u8 isa_irq);
void hvm_inject_msi(struct domain *d, uint64_t addr, uint32_t data);
void hvm_maybe_deassert_evtchn_irq(void);
void hvm_assert_evtchn_irq(struct vcpu *v);
void hvm_set_callback_via(struct domain *d, uint64_t via);
struct x86_emulate_ctxt;
enum x86_segment {
    x86_seg_cs,
    x86_seg_ss,
    x86_seg_ds,
    x86_seg_es,
    x86_seg_fs,
    x86_seg_gs,
    x86_seg_tr,
    x86_seg_ldtr,
    x86_seg_gdtr,
    x86_seg_idtr,
    x86_seg_none
};
typedef union segment_attributes {
    uint16_t bytes;
    struct
    {
        uint16_t type:4;
        uint16_t s: 1;
        uint16_t dpl: 2;
        uint16_t p: 1;
        uint16_t avl: 1;
        uint16_t l: 1;
        uint16_t db: 1;
        uint16_t g: 1;
        uint16_t pad: 4;
    } fields;
} __attribute__ ((packed)) segment_attributes_t;
struct segment_register {
    uint16_t sel;
    segment_attributes_t attr;
    uint32_t limit;
    uint64_t base;
} __attribute__ ((packed));
enum x86_emulate_fpu_type {
    X86EMUL_FPU_fpu,
    X86EMUL_FPU_mmx,
    X86EMUL_FPU_xmm,
    X86EMUL_FPU_ymm
};
struct x86_emulate_ops
{
    int (*read)(
        enum x86_segment seg,
        unsigned long offset,
        void *p_data,
        unsigned int bytes,
        struct x86_emulate_ctxt *ctxt);
    int (*insn_fetch)(
        enum x86_segment seg,
        unsigned long offset,
        void *p_data,
        unsigned int bytes,
        struct x86_emulate_ctxt *ctxt);
    int (*write)(
        enum x86_segment seg,
        unsigned long offset,
        void *p_data,
        unsigned int bytes,
        struct x86_emulate_ctxt *ctxt);
    int (*cmpxchg)(
        enum x86_segment seg,
        unsigned long offset,
        void *p_old,
        void *p_new,
        unsigned int bytes,
        struct x86_emulate_ctxt *ctxt);
    int (*rep_ins)(
        uint16_t src_port,
        enum x86_segment dst_seg,
        unsigned long dst_offset,
        unsigned int bytes_per_rep,
        unsigned long *reps,
        struct x86_emulate_ctxt *ctxt);
    int (*rep_outs)(
        enum x86_segment src_seg,
        unsigned long src_offset,
        uint16_t dst_port,
        unsigned int bytes_per_rep,
        unsigned long *reps,
        struct x86_emulate_ctxt *ctxt);
    int (*rep_movs)(
        enum x86_segment src_seg,
        unsigned long src_offset,
        enum x86_segment dst_seg,
        unsigned long dst_offset,
        unsigned int bytes_per_rep,
        unsigned long *reps,
        struct x86_emulate_ctxt *ctxt);
    int (*read_segment)(
        enum x86_segment seg,
        struct segment_register *reg,
        struct x86_emulate_ctxt *ctxt);
    int (*write_segment)(
        enum x86_segment seg,
        struct segment_register *reg,
        struct x86_emulate_ctxt *ctxt);
    int (*read_io)(
        unsigned int port,
        unsigned int bytes,
        unsigned long *val,
        struct x86_emulate_ctxt *ctxt);
    int (*write_io)(
        unsigned int port,
        unsigned int bytes,
        unsigned long val,
        struct x86_emulate_ctxt *ctxt);
    int (*read_cr)(
        unsigned int reg,
        unsigned long *val,
        struct x86_emulate_ctxt *ctxt);
    int (*write_cr)(
        unsigned int reg,
        unsigned long val,
        struct x86_emulate_ctxt *ctxt);
    int (*read_dr)(
        unsigned int reg,
        unsigned long *val,
        struct x86_emulate_ctxt *ctxt);
    int (*write_dr)(
        unsigned int reg,
        unsigned long val,
        struct x86_emulate_ctxt *ctxt);
    int (*read_msr)(
        unsigned long reg,
        uint64_t *val,
        struct x86_emulate_ctxt *ctxt);
    int (*write_msr)(
        unsigned long reg,
        uint64_t val,
        struct x86_emulate_ctxt *ctxt);
    int (*wbinvd)(
        struct x86_emulate_ctxt *ctxt);
    int (*cpuid)(
        unsigned int *eax,
        unsigned int *ebx,
        unsigned int *ecx,
        unsigned int *edx,
        struct x86_emulate_ctxt *ctxt);
    int (*inject_hw_exception)(
        uint8_t vector,
        int32_t error_code,
        struct x86_emulate_ctxt *ctxt);
    int (*inject_sw_interrupt)(
        uint8_t vector,
        uint8_t insn_len,
        struct x86_emulate_ctxt *ctxt);
    int (*get_fpu)(
        void (*exception_callback)(void *, struct cpu_user_regs *),
        void *exception_callback_arg,
        enum x86_emulate_fpu_type type,
        struct x86_emulate_ctxt *ctxt);
    void (*put_fpu)(
        struct x86_emulate_ctxt *ctxt);
    int (*invlpg)(
        enum x86_segment seg,
        unsigned long offset,
        struct x86_emulate_ctxt *ctxt);
};
struct cpu_user_regs;
struct x86_emulate_ctxt
{
    struct cpu_user_regs *regs;
    unsigned int addr_size;
    unsigned int sp_size;
    uint8_t force_writeback;
    union {
        struct {
            uint8_t hlt:1;
            uint8_t mov_ss:1;
            uint8_t sti:1;
        } flags;
        uint8_t byte;
    } retire;
};
int
x86_emulate(
    struct x86_emulate_ctxt *ctxt,
    const struct x86_emulate_ops *ops);
void *
decode_register(
    uint8_t modrm_reg, struct cpu_user_regs *regs, int highbyte_regs);
struct vcpu;
struct hvm_vcpu_asid;
void hvm_asid_init(int nasids);
void hvm_asid_flush_vcpu_asid(struct hvm_vcpu_asid *asid);
void hvm_asid_flush_vcpu(struct vcpu *v);
void hvm_asid_flush_core(void);
bool_t hvm_asid_handle_vmenter(struct hvm_vcpu_asid *asid);
enum hvm_intsrc {
    hvm_intsrc_none,
    hvm_intsrc_pic,
    hvm_intsrc_lapic,
    hvm_intsrc_nmi,
    hvm_intsrc_mce,
    hvm_intsrc_vector
};
struct hvm_intack {
    uint8_t source;
    uint8_t vector;
};
enum hvm_intblk {
    hvm_intblk_none,
    hvm_intblk_shadow,
    hvm_intblk_rflags_ie,
    hvm_intblk_tpr,
    hvm_intblk_nmi_iret,
    hvm_intblk_arch,
};
struct hvm_trap {
    int vector;
    unsigned int type;
    int error_code;
    int insn_len;
    unsigned long cr2;
};
struct hvm_function_table {
    char *name;
    int hap_supported;
    int pvh_supported;
    int hap_capabilities;
    int (*domain_initialise)(struct domain *d);
    void (*domain_destroy)(struct domain *d);
    int (*vcpu_initialise)(struct vcpu *v);
    void (*vcpu_destroy)(struct vcpu *v);
    void (*save_cpu_ctxt)(struct vcpu *v, struct hvm_hw_cpu *ctxt);
    int (*load_cpu_ctxt)(struct vcpu *v, struct hvm_hw_cpu *ctxt);
    unsigned int (*get_interrupt_shadow)(struct vcpu *v);
    void (*set_interrupt_shadow)(struct vcpu *v, unsigned int intr_shadow);
    int (*guest_x86_mode)(struct vcpu *v);
    void (*get_segment_register)(struct vcpu *v, enum x86_segment seg,
                                 struct segment_register *reg);
    void (*set_segment_register)(struct vcpu *v, enum x86_segment seg,
                                 struct segment_register *reg);
    unsigned long (*get_shadow_gs_base)(struct vcpu *v);
    void (*update_host_cr3)(struct vcpu *v);
    void (*update_guest_cr)(struct vcpu *v, unsigned int cr);
    void (*update_guest_efer)(struct vcpu *v);
    int (*get_guest_pat)(struct vcpu *v, u64 *);
    int (*set_guest_pat)(struct vcpu *v, u64);
    void (*set_tsc_offset)(struct vcpu *v, u64 offset);
    void (*inject_trap)(struct hvm_trap *trap);
    void (*init_hypercall_page)(struct domain *d, void *hypercall_page);
    int (*event_pending)(struct vcpu *v);
    int (*cpu_up_prepare)(unsigned int cpu);
    void (*cpu_dead)(unsigned int cpu);
    int (*cpu_up)(void);
    void (*cpu_down)(void);
    unsigned int (*get_insn_bytes)(struct vcpu *v, uint8_t *buf);
    void (*cpuid_intercept)(
        unsigned int *eax, unsigned int *ebx,
        unsigned int *ecx, unsigned int *edx);
    void (*wbinvd_intercept)(void);
    void (*fpu_dirty_intercept)(void);
    int (*msr_read_intercept)(unsigned int msr, uint64_t *msr_content);
    int (*msr_write_intercept)(unsigned int msr, uint64_t msr_content);
    void (*invlpg_intercept)(unsigned long vaddr);
    void (*handle_cd)(struct vcpu *v, unsigned long value);
    void (*set_info_guest)(struct vcpu *v);
    void (*set_rdtsc_exiting)(struct vcpu *v, bool_t);
    int (*nhvm_vcpu_initialise)(struct vcpu *v);
    void (*nhvm_vcpu_destroy)(struct vcpu *v);
    int (*nhvm_vcpu_reset)(struct vcpu *v);
    int (*nhvm_vcpu_hostrestore)(struct vcpu *v,
                                struct cpu_user_regs *regs);
    int (*nhvm_vcpu_vmexit)(struct vcpu *v, struct cpu_user_regs *regs,
                                uint64_t exitcode);
    int (*nhvm_vcpu_vmexit_trap)(struct vcpu *v, struct hvm_trap *trap);
    uint64_t (*nhvm_vcpu_guestcr3)(struct vcpu *v);
    uint64_t (*nhvm_vcpu_p2m_base)(struct vcpu *v);
    uint32_t (*nhvm_vcpu_asid)(struct vcpu *v);
    int (*nhvm_vmcx_guest_intercepts_trap)(struct vcpu *v,
                               unsigned int trapnr, int errcode);
    bool_t (*nhvm_vmcx_hap_enabled)(struct vcpu *v);
    enum hvm_intblk (*nhvm_intr_blocked)(struct vcpu *v);
    void (*nhvm_domain_relinquish_resources)(struct domain *d);
    void (*update_eoi_exit_bitmap)(struct vcpu *v, u8 vector, u8 trig);
    int (*virtual_intr_delivery_enabled)(void);
    void (*process_isr)(int isr, struct vcpu *v);
    void (*deliver_posted_intr)(struct vcpu *v, u8 vector);
    void (*sync_pir_to_irr)(struct vcpu *v);
    void (*handle_eoi)(u8 vector);
    int (*nhvm_hap_walk_L1_p2m)(struct vcpu *v, paddr_t L2_gpa,
                                paddr_t *L1_gpa, unsigned int *page_order,
                                uint8_t *p2m_acc, bool_t access_r,
                                bool_t access_w, bool_t access_x);
};
extern struct hvm_function_table hvm_funcs;
extern bool_t hvm_enabled;
extern bool_t cpu_has_lmsl;
extern s8 hvm_port80_allowed;
extern const struct hvm_function_table *start_svm(void);
extern const struct hvm_function_table *start_vmx(void);
int hvm_domain_initialise(struct domain *d);
void hvm_domain_relinquish_resources(struct domain *d);
void hvm_domain_destroy(struct domain *d);
int hvm_vcpu_initialise(struct vcpu *v);
void hvm_vcpu_destroy(struct vcpu *v);
void hvm_vcpu_down(struct vcpu *v);
int hvm_vcpu_cacheattr_init(struct vcpu *v);
void hvm_vcpu_cacheattr_destroy(struct vcpu *v);
void hvm_vcpu_reset_state(struct vcpu *v, uint16_t cs, uint16_t ip);
int prepare_ring_for_helper(struct domain *d, unsigned long gmfn,
                            struct page_info **_page, void **_va);
void destroy_ring_for_helper(void **_va, struct page_info *page);
bool_t hvm_send_assist_req(struct vcpu *v);
void hvm_get_guest_pat(struct vcpu *v, u64 *guest_pat);
int hvm_set_guest_pat(struct vcpu *v, u64 guest_pat);
void hvm_set_guest_tsc(struct vcpu *v, u64 guest_tsc);
u64 hvm_get_guest_tsc(struct vcpu *v);
void hvm_init_guest_time(struct domain *d);
void hvm_set_guest_time(struct vcpu *v, u64 guest_time);
u64 hvm_get_guest_time(struct vcpu *v);
int vmsi_deliver(
    struct domain *d, int vector,
    uint8_t dest, uint8_t dest_mode,
    uint8_t delivery_mode, uint8_t trig_mode);
struct hvm_pirq_dpci;
void vmsi_deliver_pirq(struct domain *d, const struct hvm_pirq_dpci *);
int hvm_girq_dest_2_vcpu_id(struct domain *d, uint8_t dest, uint8_t dest_mode);
enum hvm_intblk
hvm_interrupt_blocked(struct vcpu *v, struct hvm_intack intack);
static __inline__ int
hvm_guest_x86_mode(struct vcpu *v)
{
    do { if ( 0 && (v == ((get_cpu_info()->current_vcpu))) ); } while (0);
    return hvm_funcs.guest_x86_mode(v);
}
static __inline__ void
hvm_update_host_cr3(struct vcpu *v)
{
    if ( hvm_funcs.update_host_cr3 )
        hvm_funcs.update_host_cr3(v);
}
static __inline__ void hvm_update_guest_cr(struct vcpu *v, unsigned int cr)
{
    hvm_funcs.update_guest_cr(v, cr);
}
static __inline__ void hvm_update_guest_efer(struct vcpu *v)
{
    hvm_funcs.update_guest_efer(v);
}
static __inline__ void hvm_flush_guest_tlbs(void)
{
    if ( hvm_enabled )
        hvm_asid_flush_core();
}
void hvm_hypercall_page_initialise(struct domain *d,
                                   void *hypercall_page);
static __inline__ void
hvm_get_segment_register(struct vcpu *v, enum x86_segment seg,
                         struct segment_register *reg)
{
    hvm_funcs.get_segment_register(v, seg, reg);
}
static __inline__ void
hvm_set_segment_register(struct vcpu *v, enum x86_segment seg,
                         struct segment_register *reg)
{
    hvm_funcs.set_segment_register(v, seg, reg);
}
static __inline__ unsigned long hvm_get_shadow_gs_base(struct vcpu *v)
{
    return hvm_funcs.get_shadow_gs_base(v);
}
void hvm_cpuid(unsigned int input, unsigned int *eax, unsigned int *ebx,
                                   unsigned int *ecx, unsigned int *edx);
void hvm_migrate_timers(struct vcpu *v);
void hvm_do_resume(struct vcpu *v);
void hvm_migrate_pirqs(struct vcpu *v);
void hvm_inject_trap(struct hvm_trap *trap);
void hvm_inject_hw_exception(unsigned int trapnr, int errcode);
void hvm_inject_page_fault(int errcode, unsigned long cr2);
static __inline__ int hvm_event_pending(struct vcpu *v)
{
    return hvm_funcs.event_pending(v);
}
int hvm_event_needs_reinjection(uint8_t type, uint8_t vector);
uint8_t hvm_combine_hw_exceptions(uint8_t vec1, uint8_t vec2);
void hvm_set_rdtsc_exiting(struct domain *d, bool_t enable);
static __inline__ int hvm_cpu_up(void)
{
    return (hvm_funcs.cpu_up ? hvm_funcs.cpu_up() : 0);
}
static __inline__ void hvm_cpu_down(void)
{
    if ( hvm_funcs.cpu_down )
        hvm_funcs.cpu_down();
}
static __inline__ unsigned int hvm_get_insn_bytes(struct vcpu *v, uint8_t *buf)
{
    return (hvm_funcs.get_insn_bytes ? hvm_funcs.get_insn_bytes(v, buf) : 0);
}
enum hvm_task_switch_reason { TSW_jmp, TSW_iret, TSW_call_or_int };
void hvm_task_switch(
    uint16_t tss_sel, enum hvm_task_switch_reason taskswitch_reason,
    int32_t errcode);
enum hvm_access_type {
    hvm_access_insn_fetch,
    hvm_access_none,
    hvm_access_read,
    hvm_access_write
};
int hvm_virtual_to_linear_addr(
    enum x86_segment seg,
    struct segment_register *reg,
    unsigned long offset,
    unsigned int bytes,
    enum hvm_access_type access_type,
    unsigned int addr_size,
    unsigned long *linear_addr);
void *hvm_map_guest_frame_rw(unsigned long gfn, bool_t permanent);
void *hvm_map_guest_frame_ro(unsigned long gfn, bool_t permanent);
void hvm_unmap_guest_frame(void *p, bool_t permanent);
static __inline__ void hvm_set_info_guest(struct vcpu *v)
{
    if ( hvm_funcs.set_info_guest )
        return hvm_funcs.set_info_guest(v);
}
int hvm_debug_op(struct vcpu *v, int32_t op);
static __inline__ void hvm_invalidate_regs_fields(struct cpu_user_regs *regs)
{
}
int hvm_hap_nested_page_fault(paddr_t gpa,
                              bool_t gla_valid, unsigned long gla,
                              bool_t access_r,
                              bool_t access_w,
                              bool_t access_x);
int hvm_x2apic_msr_read(struct vcpu *v, unsigned int msr, uint64_t *msr_content);
int hvm_x2apic_msr_write(struct vcpu *v, unsigned int msr, uint64_t msr_content);
void hvm_memory_event_cr0(unsigned long value, unsigned long old);
void hvm_memory_event_cr3(unsigned long value, unsigned long old);
void hvm_memory_event_cr4(unsigned long value, unsigned long old);
void hvm_memory_event_msr(unsigned long msr, unsigned long value);
int hvm_memory_event_int3(unsigned long gla);
int hvm_memory_event_single_step(unsigned long gla);
int nhvm_vcpu_hostrestore(struct vcpu *v, struct cpu_user_regs *regs);
int nhvm_vcpu_vmexit(struct vcpu *v, struct cpu_user_regs *regs,
                     uint64_t exitcode);
int nhvm_vcpu_vmexit_trap(struct vcpu *v, struct hvm_trap *trap);
uint64_t nhvm_vcpu_guestcr3(struct vcpu *v);
uint64_t nhvm_vcpu_p2m_base(struct vcpu *v);
uint32_t nhvm_vcpu_asid(struct vcpu *v);
int nhvm_vmcx_guest_intercepts_trap(struct vcpu *v,
                                    unsigned int trapnr, int errcode);
bool_t nhvm_vmcx_hap_enabled(struct vcpu *v);
enum hvm_intblk nhvm_interrupt_blocked(struct vcpu *v);
struct hvm_irq {
    struct hvm_hw_pci_irqs pci_intx;
    struct hvm_hw_isa_irqs isa_irq;
    struct hvm_hw_pci_link pci_link;
    uint32_t callback_via_asserted;
    union {
        enum {
            HVMIRQ_callback_none,
            HVMIRQ_callback_gsi,
            HVMIRQ_callback_pci_intx,
            HVMIRQ_callback_vector
        } callback_via_type;
    };
    union {
        uint32_t gsi;
        struct { uint8_t dev, intx; } pci;
        uint32_t vector;
    } callback_via;
    u8 pci_link_assert_count[4];
    u8 gsi_assert_count[48];
    u8 round_robin_prev_vcpu;
    struct hvm_irq_dpci *dpci;
};
struct hvm_intack hvm_vcpu_has_pending_irq(struct vcpu *v);
struct hvm_intack hvm_vcpu_ack_pending_irq(struct vcpu *v,
                                           struct hvm_intack intack);
typedef void time_cb(struct vcpu *v, void *opaque);
struct periodic_time {
    struct list_head list;
    bool_t on_list;
    bool_t one_shot;
    bool_t do_not_freeze;
    bool_t irq_issued;
    bool_t warned_timeout_too_short;
    u8 source;
    u8 irq;
    struct vcpu *vcpu;
    u32 pending_intr_nr;
    u64 period;
    s_time_t scheduled;
    u64 last_plt_gtime;
    struct timer timer;
    time_cb *cb;
    void *priv;
};
typedef struct PITState {
    struct hvm_hw_pit hw;
    int64_t count_load_time[3];
    struct periodic_time pt0;
    spinlock_t lock;
} PITState;
struct hpet_registers {
    uint64_t capability;
    uint64_t config;
    uint64_t isr;
    uint64_t mc64;
    struct {
        uint64_t config;
        uint64_t cmp;
        uint64_t fsb;
    } timers[3];
    uint64_t period[3];
    uint64_t comparator64[3];
};
typedef struct HPETState {
    struct hpet_registers hpet;
    uint64_t stime_freq;
    uint64_t hpet_to_ns_scale;
    uint64_t hpet_to_ns_limit;
    uint64_t mc_offset;
    struct periodic_time pt[3];
    spinlock_t lock;
} HPETState;
typedef struct RTCState {
    struct hvm_hw_rtc hw;
    struct tm current_tm;
    struct timer update_timer;
    struct timer update_timer2;
    uint64_t next_update_time;
    struct timer alarm_timer;
    struct periodic_time pt;
    s_time_t start_time;
    int pt_code;
    uint8_t pt_dead_ticks;
    uint32_t use_timer;
    spinlock_t lock;
} RTCState;
typedef struct PMTState {
    struct hvm_hw_pmtimer pm;
    struct vcpu *vcpu;
    uint64_t last_gtime;
    uint32_t not_accounted;
    uint64_t scale;
    struct timer timer;
    spinlock_t lock;
} PMTState;
struct pl_time {
    struct RTCState vrtc;
    struct HPETState vhpet;
    struct PMTState vpmt;
    int64_t stime_offset;
    uint64_t last_guest_time;
    spinlock_t pl_time_lock;
};
void pt_save_timer(struct vcpu *v);
void pt_restore_timer(struct vcpu *v);
int pt_update_irq(struct vcpu *v);
void pt_intr_post(struct vcpu *v, struct hvm_intack intack);
void pt_migrate(struct vcpu *v);
void pt_adjust_global_vcpu_target(struct vcpu *v);
void pt_may_unmask_irq(struct domain *d, struct periodic_time *vlapic_pt);
void create_periodic_time(
    struct vcpu *v, struct periodic_time *pt, uint64_t delta,
    uint64_t period, uint8_t irq, time_cb *cb, void *data);
void destroy_periodic_time(struct periodic_time *pt);
int pv_pit_handler(int port, int data, int write);
void pit_reset(struct domain *d);
void pit_init(struct vcpu *v, unsigned long cpu_khz);
void pit_stop_channel0_irq(PITState * pit);
void pit_deinit(struct domain *d);
void rtc_init(struct domain *d);
void rtc_migrate_timers(struct vcpu *v);
void rtc_deinit(struct domain *d);
void rtc_reset(struct domain *d);
void rtc_update_clock(struct domain *d);
bool_t rtc_periodic_interrupt(void *);
void pmtimer_init(struct vcpu *v);
void pmtimer_deinit(struct domain *d);
void pmtimer_reset(struct domain *d);
int pmtimer_change_ioport(struct domain *d, unsigned int version);
void hpet_init(struct vcpu *v);
void hpet_deinit(struct domain *d);
void hpet_reset(struct domain *d);
struct vlapic {
    struct hvm_hw_lapic hw;
    struct hvm_hw_lapic_regs *regs;
    struct periodic_time pt;
    s_time_t timer_last_update;
    struct page_info *regs_page;
    struct {
        uint32_t icr, dest;
        struct tasklet tasklet;
    } init_sipi;
};
static __inline__ uint32_t vlapic_get_reg(struct vlapic *vlapic, uint32_t reg)
{
    return *((uint32_t *)(&vlapic->regs->data[reg]));
}
static __inline__ void vlapic_set_reg(
    struct vlapic *vlapic, uint32_t reg, uint32_t val)
{
    *((uint32_t *)(&vlapic->regs->data[reg])) = val;
}
bool_t is_vlapic_lvtpc_enabled(struct vlapic *vlapic);
void vlapic_set_irq(struct vlapic *vlapic, uint8_t vec, uint8_t trig);
int vlapic_has_pending_irq(struct vcpu *v);
int vlapic_ack_pending_irq(struct vcpu *v, int vector, bool_t force_ack);
int vlapic_init(struct vcpu *v);
void vlapic_destroy(struct vcpu *v);
void vlapic_reset(struct vlapic *vlapic);
void vlapic_msr_set(struct vlapic *vlapic, uint64_t value);
void vlapic_tdt_msr_set(struct vlapic *vlapic, uint64_t value);
uint64_t vlapic_tdt_msr_get(struct vlapic *vlapic);
int vlapic_accept_pic_intr(struct vcpu *v);
uint32_t vlapic_set_ppr(struct vlapic *vlapic);
void vlapic_adjust_i8259_target(struct domain *d);
void vlapic_EOI_set(struct vlapic *vlapic);
void vlapic_handle_EOI_induced_exit(struct vlapic *vlapic, int vector);
void vlapic_ipi(struct vlapic *vlapic, uint32_t icr_low, uint32_t icr_high);
int vlapic_apicv_write(struct vcpu *v, unsigned int offset);
struct vlapic *vlapic_lowest_prio(
    struct domain *d, struct vlapic *source,
    int short_hand, uint8_t dest, uint8_t dest_mode);
bool_t vlapic_match_dest(
    struct vlapic *target, struct vlapic *source,
    int short_hand, uint8_t dest, uint8_t dest_mode);
union viridian_apic_assist
{ uint64_t raw;
    struct
    {
        uint64_t enabled:1;
        uint64_t reserved_preserved:11;
        uint64_t pfn:48;
    } fields;
};
struct viridian_vcpu
{
    union viridian_apic_assist apic_assist;
};
union viridian_guest_os_id
{
    uint64_t raw;
    struct
    {
        uint64_t build_number:16;
        uint64_t service_pack:8;
        uint64_t minor:8;
        uint64_t major:8;
        uint64_t os:8;
        uint64_t vendor:16;
    } fields;
};
union viridian_hypercall_gpa
{ uint64_t raw;
    struct
    {
        uint64_t enabled:1;
        uint64_t reserved_preserved:11;
        uint64_t pfn:48;
    } fields;
};
struct viridian_domain
{
    union viridian_guest_os_id guest_os_id;
    union viridian_hypercall_gpa hypercall_gpa;
};
int
cpuid_viridian_leaves(
    unsigned int leaf,
    unsigned int *eax,
    unsigned int *ebx,
    unsigned int *ecx,
    unsigned int *edx);
int
wrmsr_viridian_regs(
    uint32_t idx,
    uint64_t val);
int
rdmsr_viridian_regs(
    uint32_t idx,
    uint64_t *val);
int
viridian_hypercall(struct cpu_user_regs *regs);
struct arch_vpmu_ops {
    int (*do_wrmsr)(unsigned int msr, uint64_t msr_content);
    int (*do_rdmsr)(unsigned int msr, uint64_t *msr_content);
    int (*do_interrupt)(struct cpu_user_regs *regs);
    void (*do_cpuid)(unsigned int input,
                     unsigned int *eax, unsigned int *ebx,
                     unsigned int *ecx, unsigned int *edx);
    void (*arch_vpmu_destroy)(struct vcpu *v);
    int (*arch_vpmu_save)(struct vcpu *v);
    void (*arch_vpmu_load)(struct vcpu *v);
    void (*arch_vpmu_dump)(const struct vcpu *);
};
int vmx_vpmu_initialise(struct vcpu *, unsigned int flags);
int svm_vpmu_initialise(struct vcpu *, unsigned int flags);
struct vpmu_struct {
    u32 flags;
    u32 last_pcpu;
    u32 hw_lapic_lvtpc;
    void *context;
    struct arch_vpmu_ops *arch_vpmu_ops;
};
int vpmu_do_wrmsr(unsigned int msr, uint64_t msr_content);
int vpmu_do_rdmsr(unsigned int msr, uint64_t *msr_content);
int vpmu_do_interrupt(struct cpu_user_regs *regs);
void vpmu_do_cpuid(unsigned int input, unsigned int *eax, unsigned int *ebx,
                                       unsigned int *ecx, unsigned int *edx);
void vpmu_initialise(struct vcpu *v);
void vpmu_destroy(struct vcpu *v);
void vpmu_save(struct vcpu *v);
void vpmu_load(struct vcpu *v);
void vpmu_dump(struct vcpu *v);
extern int acquire_pmu_ownership(int pmu_ownership);
extern void release_pmu_ownership(int pmu_ownership);
extern void vmcs_dump_vcpu(struct vcpu *v);
extern void setup_vmcs_dump(void);
extern int vmx_cpu_up_prepare(unsigned int cpu);
extern void vmx_cpu_dead(unsigned int cpu);
extern int vmx_cpu_up(void);
extern void vmx_cpu_down(void);
extern void vmx_save_host_msrs(void);
struct vmcs_struct {
    u32 vmcs_revision_id;
    unsigned char data [0];
};
struct vmx_msr_entry {
    u32 index;
    u32 mbz;
    u64 data;
};
enum {
    VMX_INDEX_MSR_LSTAR = 0,
    VMX_INDEX_MSR_STAR,
    VMX_INDEX_MSR_SYSCALL_MASK,
    VMX_MSR_COUNT
};
struct vmx_msr_state {
    unsigned long flags;
    unsigned long msrs[VMX_MSR_COUNT];
};
struct ept_data {
    union {
    struct {
            u64 ept_mt :3,
                ept_wl :3,
                rsvd :6,
                asr :52;
        };
        u64 eptp;
    };
    cpumask_var_t synced_mask;
};
struct vmx_domain {
    unsigned long apic_access_mfn;
};
struct pi_desc {
    unsigned long pir[(((256)+((1 << 3) << 3)-1)/((1 << 3) << 3))];
    u32 control;
    u32 rsvd[7];
} __attribute__ ((aligned (64)));
struct arch_vmx_struct {
    struct vmcs_struct *vmcs;
    paddr_t vmcs_shadow_maddr;
    spinlock_t vmcs_lock;
    struct list_head active_list;
    int active_cpu;
    int launched;
    u32 exec_control;
    u32 secondary_exec_control;
    u32 exception_bitmap;
    struct vmx_msr_state msr_state;
    unsigned long shadow_gs;
    unsigned long cstar;
    unsigned long *msr_bitmap;
    unsigned int msr_count;
    struct vmx_msr_entry *msr_area;
    unsigned int host_msr_count;
    struct vmx_msr_entry *host_msr_area;
    unsigned long eoi_exitmap_changed;
    unsigned long eoi_exit_bitmap[(((256)+((1 << 3) << 3)-1)/((1 << 3) << 3))];
    struct pi_desc pi_desc;
    unsigned long host_cr0;
    uint8_t vmx_realmode;
    uint8_t vmx_emulate;
    uint16_t vm86_segment_mask;
    struct segment_register vm86_saved_seg[x86_seg_tr + 1];
    uint32_t vm86_saved_eflags;
    int hostenv_migrated;
    struct page_info *vmread_bitmap;
    struct page_info *vmwrite_bitmap;
};
int vmx_create_vmcs(struct vcpu *v);
void vmx_destroy_vmcs(struct vcpu *v);
void vmx_vmcs_enter(struct vcpu *v);
bool_t __attribute__((warn_unused_result)) vmx_vmcs_try_enter(struct vcpu *v);
void vmx_vmcs_exit(struct vcpu *v);
extern u32 vmx_cpu_based_exec_control;
extern u32 vmx_pin_based_exec_control;
extern u32 vmx_vmexit_control;
extern u32 vmx_vmentry_control;
extern u32 vmx_secondary_exec_control;
extern u64 vmx_basic_msr;
enum vmcs_field {
    VIRTUAL_PROCESSOR_ID = 0x00000000,
    POSTED_INTR_NOTIFICATION_VECTOR = 0x00000002,
    GUEST_ES_SELECTOR = 0x00000800,
    GUEST_CS_SELECTOR = 0x00000802,
    GUEST_SS_SELECTOR = 0x00000804,
    GUEST_DS_SELECTOR = 0x00000806,
    GUEST_FS_SELECTOR = 0x00000808,
    GUEST_GS_SELECTOR = 0x0000080a,
    GUEST_LDTR_SELECTOR = 0x0000080c,
    GUEST_TR_SELECTOR = 0x0000080e,
    GUEST_INTR_STATUS = 0x00000810,
    HOST_ES_SELECTOR = 0x00000c00,
    HOST_CS_SELECTOR = 0x00000c02,
    HOST_SS_SELECTOR = 0x00000c04,
    HOST_DS_SELECTOR = 0x00000c06,
    HOST_FS_SELECTOR = 0x00000c08,
    HOST_GS_SELECTOR = 0x00000c0a,
    HOST_TR_SELECTOR = 0x00000c0c,
    IO_BITMAP_A = 0x00002000,
    IO_BITMAP_A_HIGH = 0x00002001,
    IO_BITMAP_B = 0x00002002,
    IO_BITMAP_B_HIGH = 0x00002003,
    MSR_BITMAP = 0x00002004,
    MSR_BITMAP_HIGH = 0x00002005,
    VM_EXIT_MSR_STORE_ADDR = 0x00002006,
    VM_EXIT_MSR_STORE_ADDR_HIGH = 0x00002007,
    VM_EXIT_MSR_LOAD_ADDR = 0x00002008,
    VM_EXIT_MSR_LOAD_ADDR_HIGH = 0x00002009,
    VM_ENTRY_MSR_LOAD_ADDR = 0x0000200a,
    VM_ENTRY_MSR_LOAD_ADDR_HIGH = 0x0000200b,
    TSC_OFFSET = 0x00002010,
    TSC_OFFSET_HIGH = 0x00002011,
    VIRTUAL_APIC_PAGE_ADDR = 0x00002012,
    VIRTUAL_APIC_PAGE_ADDR_HIGH = 0x00002013,
    APIC_ACCESS_ADDR = 0x00002014,
    APIC_ACCESS_ADDR_HIGH = 0x00002015,
    PI_DESC_ADDR = 0x00002016,
    PI_DESC_ADDR_HIGH = 0x00002017,
    EPT_POINTER = 0x0000201a,
    EPT_POINTER_HIGH = 0x0000201b,
    EOI_EXIT_BITMAP0 = 0x0000201c,
    VMREAD_BITMAP = 0x00002026,
    VMREAD_BITMAP_HIGH = 0x00002027,
    VMWRITE_BITMAP = 0x00002028,
    VMWRITE_BITMAP_HIGH = 0x00002029,
    GUEST_PHYSICAL_ADDRESS = 0x00002400,
    GUEST_PHYSICAL_ADDRESS_HIGH = 0x00002401,
    VMCS_LINK_POINTER = 0x00002800,
    VMCS_LINK_POINTER_HIGH = 0x00002801,
    GUEST_IA32_DEBUGCTL = 0x00002802,
    GUEST_IA32_DEBUGCTL_HIGH = 0x00002803,
    GUEST_PAT = 0x00002804,
    GUEST_PAT_HIGH = 0x00002805,
    GUEST_EFER = 0x00002806,
    GUEST_EFER_HIGH = 0x00002807,
    GUEST_PERF_GLOBAL_CTRL = 0x00002808,
    GUEST_PERF_GLOBAL_CTRL_HIGH = 0x00002809,
    GUEST_PDPTR0 = 0x0000280a,
    GUEST_PDPTR0_HIGH = 0x0000280b,
    GUEST_PDPTR1 = 0x0000280c,
    GUEST_PDPTR1_HIGH = 0x0000280d,
    GUEST_PDPTR2 = 0x0000280e,
    GUEST_PDPTR2_HIGH = 0x0000280f,
    GUEST_PDPTR3 = 0x00002810,
    GUEST_PDPTR3_HIGH = 0x00002811,
    HOST_PAT = 0x00002c00,
    HOST_PAT_HIGH = 0x00002c01,
    HOST_EFER = 0x00002c02,
    HOST_EFER_HIGH = 0x00002c03,
    HOST_PERF_GLOBAL_CTRL = 0x00002c04,
    HOST_PERF_GLOBAL_CTRL_HIGH = 0x00002c05,
    PIN_BASED_VM_EXEC_CONTROL = 0x00004000,
    CPU_BASED_VM_EXEC_CONTROL = 0x00004002,
    EXCEPTION_BITMAP = 0x00004004,
    PAGE_FAULT_ERROR_CODE_MASK = 0x00004006,
    PAGE_FAULT_ERROR_CODE_MATCH = 0x00004008,
    CR3_TARGET_COUNT = 0x0000400a,
    VM_EXIT_CONTROLS = 0x0000400c,
    VM_EXIT_MSR_STORE_COUNT = 0x0000400e,
    VM_EXIT_MSR_LOAD_COUNT = 0x00004010,
    VM_ENTRY_CONTROLS = 0x00004012,
    VM_ENTRY_MSR_LOAD_COUNT = 0x00004014,
    VM_ENTRY_INTR_INFO = 0x00004016,
    VM_ENTRY_EXCEPTION_ERROR_CODE = 0x00004018,
    VM_ENTRY_INSTRUCTION_LEN = 0x0000401a,
    TPR_THRESHOLD = 0x0000401c,
    SECONDARY_VM_EXEC_CONTROL = 0x0000401e,
    PLE_GAP = 0x00004020,
    PLE_WINDOW = 0x00004022,
    VM_INSTRUCTION_ERROR = 0x00004400,
    VM_EXIT_REASON = 0x00004402,
    VM_EXIT_INTR_INFO = 0x00004404,
    VM_EXIT_INTR_ERROR_CODE = 0x00004406,
    IDT_VECTORING_INFO = 0x00004408,
    IDT_VECTORING_ERROR_CODE = 0x0000440a,
    VM_EXIT_INSTRUCTION_LEN = 0x0000440c,
    VMX_INSTRUCTION_INFO = 0x0000440e,
    GUEST_ES_LIMIT = 0x00004800,
    GUEST_CS_LIMIT = 0x00004802,
    GUEST_SS_LIMIT = 0x00004804,
    GUEST_DS_LIMIT = 0x00004806,
    GUEST_FS_LIMIT = 0x00004808,
    GUEST_GS_LIMIT = 0x0000480a,
    GUEST_LDTR_LIMIT = 0x0000480c,
    GUEST_TR_LIMIT = 0x0000480e,
    GUEST_GDTR_LIMIT = 0x00004810,
    GUEST_IDTR_LIMIT = 0x00004812,
    GUEST_ES_AR_BYTES = 0x00004814,
    GUEST_CS_AR_BYTES = 0x00004816,
    GUEST_SS_AR_BYTES = 0x00004818,
    GUEST_DS_AR_BYTES = 0x0000481a,
    GUEST_FS_AR_BYTES = 0x0000481c,
    GUEST_GS_AR_BYTES = 0x0000481e,
    GUEST_LDTR_AR_BYTES = 0x00004820,
    GUEST_TR_AR_BYTES = 0x00004822,
    GUEST_INTERRUPTIBILITY_INFO = 0x00004824,
    GUEST_ACTIVITY_STATE = 0x00004826,
    GUEST_SYSENTER_CS = 0x0000482A,
    GUEST_PREEMPTION_TIMER = 0x0000482e,
    HOST_SYSENTER_CS = 0x00004c00,
    CR0_GUEST_HOST_MASK = 0x00006000,
    CR4_GUEST_HOST_MASK = 0x00006002,
    CR0_READ_SHADOW = 0x00006004,
    CR4_READ_SHADOW = 0x00006006,
    CR3_TARGET_VALUE0 = 0x00006008,
    CR3_TARGET_VALUE1 = 0x0000600a,
    CR3_TARGET_VALUE2 = 0x0000600c,
    CR3_TARGET_VALUE3 = 0x0000600e,
    EXIT_QUALIFICATION = 0x00006400,
    GUEST_LINEAR_ADDRESS = 0x0000640a,
    GUEST_CR0 = 0x00006800,
    GUEST_CR3 = 0x00006802,
    GUEST_CR4 = 0x00006804,
    GUEST_ES_BASE = 0x00006806,
    GUEST_CS_BASE = 0x00006808,
    GUEST_SS_BASE = 0x0000680a,
    GUEST_DS_BASE = 0x0000680c,
    GUEST_FS_BASE = 0x0000680e,
    GUEST_GS_BASE = 0x00006810,
    GUEST_LDTR_BASE = 0x00006812,
    GUEST_TR_BASE = 0x00006814,
    GUEST_GDTR_BASE = 0x00006816,
    GUEST_IDTR_BASE = 0x00006818,
    GUEST_DR7 = 0x0000681a,
    GUEST_RSP = 0x0000681c,
    GUEST_RIP = 0x0000681e,
    GUEST_RFLAGS = 0x00006820,
    GUEST_PENDING_DBG_EXCEPTIONS = 0x00006822,
    GUEST_SYSENTER_ESP = 0x00006824,
    GUEST_SYSENTER_EIP = 0x00006826,
    HOST_CR0 = 0x00006c00,
    HOST_CR3 = 0x00006c02,
    HOST_CR4 = 0x00006c04,
    HOST_FS_BASE = 0x00006c06,
    HOST_GS_BASE = 0x00006c08,
    HOST_TR_BASE = 0x00006c0a,
    HOST_GDTR_BASE = 0x00006c0c,
    HOST_IDTR_BASE = 0x00006c0e,
    HOST_SYSENTER_ESP = 0x00006c10,
    HOST_SYSENTER_EIP = 0x00006c12,
    HOST_RSP = 0x00006c14,
    HOST_RIP = 0x00006c16,
};
void vmx_disable_intercept_for_msr(struct vcpu *v, u32 msr, int type);
void vmx_enable_intercept_for_msr(struct vcpu *v, u32 msr, int type);
int vmx_read_guest_msr(u32 msr, u64 *val);
int vmx_write_guest_msr(u32 msr, u64 val);
int vmx_add_guest_msr(u32 msr);
int vmx_add_host_load_msr(u32 msr);
void vmx_vmcs_switch(struct vmcs_struct *from, struct vmcs_struct *to);
void vmx_set_eoi_exit_bitmap(struct vcpu *v, u8 vector);
void vmx_clear_eoi_exit_bitmap(struct vcpu *v, u8 vector);
int vmx_check_msr_bitmap(unsigned long *msr_bitmap, u32 msr, int access_type);
void virtual_vmcs_enter(void *vvmcs);
void virtual_vmcs_exit(void *vvmcs);
u64 virtual_vmcs_vmread(void *vvmcs, u32 vmcs_encoding);
void virtual_vmcs_vmwrite(void *vvmcs, u32 vmcs_encoding, u64 val);
struct vvmcs_list {
    unsigned long vvmcs_mfn;
    struct list_head node;
};
struct nestedvmx {
    paddr_t vmxon_region_pa;
    void *iobitmap[2];
    void *msrbitmap;
    struct {
        unsigned long intr_info;
        u32 error_code;
        u8 source;
    } intr;
    struct {
        bool_t enabled;
        uint32_t exit_reason;
        uint32_t exit_qual;
    } ept;
    uint32_t guest_vpid;
    struct list_head launched_list;
};
enum vmx_regs_enc {
    VMX_REG_RAX,
    VMX_REG_RCX,
    VMX_REG_RDX,
    VMX_REG_RBX,
    VMX_REG_RSP,
    VMX_REG_RBP,
    VMX_REG_RSI,
    VMX_REG_RDI,
    VMX_REG_R8,
    VMX_REG_R9,
    VMX_REG_R10,
    VMX_REG_R11,
    VMX_REG_R12,
    VMX_REG_R13,
    VMX_REG_R14,
    VMX_REG_R15,
};
enum vmx_sregs_enc {
    VMX_SREG_ES,
    VMX_SREG_CS,
    VMX_SREG_SS,
    VMX_SREG_DS,
    VMX_SREG_FS,
    VMX_SREG_GS,
};
union vmx_inst_info {
    struct {
        unsigned int scaling :2;
        unsigned int __rsvd0 :1;
        unsigned int reg1 :4;
        unsigned int addr_size :3;
        unsigned int memreg :1;
        unsigned int __rsvd1 :4;
        unsigned int segment :3;
        unsigned int index_reg :4;
        unsigned int index_reg_invalid :1;
        unsigned int base_reg :4;
        unsigned int base_reg_invalid :1;
        unsigned int reg2 :4;
    } fields;
    u32 word;
};
int nvmx_vcpu_initialise(struct vcpu *v);
void nvmx_vcpu_destroy(struct vcpu *v);
int nvmx_vcpu_reset(struct vcpu *v);
uint64_t nvmx_vcpu_guestcr3(struct vcpu *v);
uint64_t nvmx_vcpu_eptp_base(struct vcpu *v);
uint32_t nvmx_vcpu_asid(struct vcpu *v);
enum hvm_intblk nvmx_intr_blocked(struct vcpu *v);
int nvmx_intercepts_exception(struct vcpu *v,
                              unsigned int trap, int error_code);
void nvmx_domain_relinquish_resources(struct domain *d);
bool_t nvmx_ept_enabled(struct vcpu *v);
int nvmx_handle_vmxon(struct cpu_user_regs *regs);
int nvmx_handle_vmxoff(struct cpu_user_regs *regs);
int
nvmx_hap_walk_L1_p2m(struct vcpu *v, paddr_t L2_gpa, paddr_t *L1_gpa,
                     unsigned int *page_order, uint8_t *p2m_acc,
                     bool_t access_r, bool_t access_w, bool_t access_x);
struct vvmcs_header {
    u32 revision;
    u32 abort;
};
union vmcs_encoding {
    struct {
        u32 access_type : 1;
        u32 index : 9;
        u32 type : 2;
        u32 rsv1 : 1;
        u32 width : 2;
        u32 rsv2 : 17;
    };
    u32 word;
};
enum vvmcs_encoding_width {
    VVMCS_WIDTH_16 = 0,
    VVMCS_WIDTH_64,
    VVMCS_WIDTH_32,
    VVMCS_WIDTH_NATURAL,
};
enum vvmcs_encoding_type {
    VVMCS_TYPE_CONTROL = 0,
    VVMCS_TYPE_RO,
    VVMCS_TYPE_GSTATE,
    VVMCS_TYPE_HSTATE,
};
u64 __get_vvmcs_virtual(void *vvmcs, u32 vmcs_encoding);
u64 __get_vvmcs_real(void *vvmcs, u32 vmcs_encoding);
void __set_vvmcs_virtual(void *vvmcs, u32 vmcs_encoding, u64 val);
void __set_vvmcs_real(void *vvmcs, u32 vmcs_encoding, u64 val);
uint64_t get_shadow_eptp(struct vcpu *v);
void nvmx_destroy_vmcs(struct vcpu *v);
int nvmx_handle_vmptrld(struct cpu_user_regs *regs);
int nvmx_handle_vmptrst(struct cpu_user_regs *regs);
int nvmx_handle_vmclear(struct cpu_user_regs *regs);
int nvmx_handle_vmread(struct cpu_user_regs *regs);
int nvmx_handle_vmwrite(struct cpu_user_regs *regs);
int nvmx_handle_vmresume(struct cpu_user_regs *regs);
int nvmx_handle_vmlaunch(struct cpu_user_regs *regs);
int nvmx_handle_invept(struct cpu_user_regs *regs);
int nvmx_handle_invvpid(struct cpu_user_regs *regs);
int nvmx_msr_read_intercept(unsigned int msr,
                                u64 *msr_content);
int nvmx_msr_write_intercept(unsigned int msr,
                                 u64 msr_content);
void nvmx_update_exec_control(struct vcpu *v, u32 value);
void nvmx_update_secondary_exec_control(struct vcpu *v,
                                        unsigned long value);
void nvmx_update_exception_bitmap(struct vcpu *v, unsigned long value);
void nvmx_switch_guest(void);
void nvmx_idtv_handling(void);
u64 nvmx_get_tsc_offset(struct vcpu *v);
int nvmx_n2_vmexit_handler(struct cpu_user_regs *regs,
                          unsigned int exit_reason);
void nvmx_set_cr_read_shadow(struct vcpu *v, unsigned int cr);
uint64_t nept_get_ept_vpid_cap(void);
int nept_translate_l2ga(struct vcpu *v, paddr_t l2ga,
                        unsigned int *page_order, uint32_t rwx_acc,
                        unsigned long *l1gfn, uint8_t *p2m_acc,
                        uint64_t *exit_qual, uint32_t *exit_reason);
int nvmx_cpu_up_prepare(unsigned int cpu);
void nvmx_cpu_dead(unsigned int cpu);
struct hvm_emulate_ctxt {
    struct x86_emulate_ctxt ctxt;
    uint8_t insn_buf[16];
    unsigned long insn_buf_eip;
    unsigned int insn_buf_bytes;
    struct segment_register seg_reg[10];
    unsigned long seg_reg_accessed;
    unsigned long seg_reg_dirty;
    bool_t exn_pending;
    uint8_t exn_vector;
    uint8_t exn_insn_len;
    int32_t exn_error_code;
    uint32_t intr_shadow;
};
int hvm_emulate_one(
    struct hvm_emulate_ctxt *hvmemul_ctxt);
void hvm_emulate_prepare(
    struct hvm_emulate_ctxt *hvmemul_ctxt,
    struct cpu_user_regs *regs);
void hvm_emulate_writeback(
    struct hvm_emulate_ctxt *hvmemul_ctxt);
struct segment_register *hvmemul_get_seg_reg(
    enum x86_segment seg,
    struct hvm_emulate_ctxt *hvmemul_ctxt);
int hvmemul_do_pio(
    unsigned long port, unsigned long *reps, int size,
    paddr_t ram_gpa, int dir, int df, void *p_data);
enum GenericIntercept1bits
{
    GENERAL1_INTERCEPT_INTR = 1 << 0,
    GENERAL1_INTERCEPT_NMI = 1 << 1,
    GENERAL1_INTERCEPT_SMI = 1 << 2,
    GENERAL1_INTERCEPT_INIT = 1 << 3,
    GENERAL1_INTERCEPT_VINTR = 1 << 4,
    GENERAL1_INTERCEPT_CR0_SEL_WRITE = 1 << 5,
    GENERAL1_INTERCEPT_IDTR_READ = 1 << 6,
    GENERAL1_INTERCEPT_GDTR_READ = 1 << 7,
    GENERAL1_INTERCEPT_LDTR_READ = 1 << 8,
    GENERAL1_INTERCEPT_TR_READ = 1 << 9,
    GENERAL1_INTERCEPT_IDTR_WRITE = 1 << 10,
    GENERAL1_INTERCEPT_GDTR_WRITE = 1 << 11,
    GENERAL1_INTERCEPT_LDTR_WRITE = 1 << 12,
    GENERAL1_INTERCEPT_TR_WRITE = 1 << 13,
    GENERAL1_INTERCEPT_RDTSC = 1 << 14,
    GENERAL1_INTERCEPT_RDPMC = 1 << 15,
    GENERAL1_INTERCEPT_PUSHF = 1 << 16,
    GENERAL1_INTERCEPT_POPF = 1 << 17,
    GENERAL1_INTERCEPT_CPUID = 1 << 18,
    GENERAL1_INTERCEPT_RSM = 1 << 19,
    GENERAL1_INTERCEPT_IRET = 1 << 20,
    GENERAL1_INTERCEPT_SWINT = 1 << 21,
    GENERAL1_INTERCEPT_INVD = 1 << 22,
    GENERAL1_INTERCEPT_PAUSE = 1 << 23,
    GENERAL1_INTERCEPT_HLT = 1 << 24,
    GENERAL1_INTERCEPT_INVLPG = 1 << 25,
    GENERAL1_INTERCEPT_INVLPGA = 1 << 26,
    GENERAL1_INTERCEPT_IOIO_PROT = 1 << 27,
    GENERAL1_INTERCEPT_MSR_PROT = 1 << 28,
    GENERAL1_INTERCEPT_TASK_SWITCH = 1 << 29,
    GENERAL1_INTERCEPT_FERR_FREEZE = 1 << 30,
    GENERAL1_INTERCEPT_SHUTDOWN_EVT = 1 << 31
};
enum GenericIntercept2bits
{
    GENERAL2_INTERCEPT_VMRUN = 1 << 0,
    GENERAL2_INTERCEPT_VMMCALL = 1 << 1,
    GENERAL2_INTERCEPT_VMLOAD = 1 << 2,
    GENERAL2_INTERCEPT_VMSAVE = 1 << 3,
    GENERAL2_INTERCEPT_STGI = 1 << 4,
    GENERAL2_INTERCEPT_CLGI = 1 << 5,
    GENERAL2_INTERCEPT_SKINIT = 1 << 6,
    GENERAL2_INTERCEPT_RDTSCP = 1 << 7,
    GENERAL2_INTERCEPT_ICEBP = 1 << 8,
    GENERAL2_INTERCEPT_WBINVD = 1 << 9,
    GENERAL2_INTERCEPT_MONITOR = 1 << 10,
    GENERAL2_INTERCEPT_MWAIT = 1 << 11,
    GENERAL2_INTERCEPT_MWAIT_CONDITIONAL = 1 << 12,
    GENERAL2_INTERCEPT_XSETBV = 1 << 13
};
enum CRInterceptBits
{
    CR_INTERCEPT_CR0_READ = 1 << 0,
    CR_INTERCEPT_CR1_READ = 1 << 1,
    CR_INTERCEPT_CR2_READ = 1 << 2,
    CR_INTERCEPT_CR3_READ = 1 << 3,
    CR_INTERCEPT_CR4_READ = 1 << 4,
    CR_INTERCEPT_CR5_READ = 1 << 5,
    CR_INTERCEPT_CR6_READ = 1 << 6,
    CR_INTERCEPT_CR7_READ = 1 << 7,
    CR_INTERCEPT_CR8_READ = 1 << 8,
    CR_INTERCEPT_CR9_READ = 1 << 9,
    CR_INTERCEPT_CR10_READ = 1 << 10,
    CR_INTERCEPT_CR11_READ = 1 << 11,
    CR_INTERCEPT_CR12_READ = 1 << 12,
    CR_INTERCEPT_CR13_READ = 1 << 13,
    CR_INTERCEPT_CR14_READ = 1 << 14,
    CR_INTERCEPT_CR15_READ = 1 << 15,
    CR_INTERCEPT_CR0_WRITE = 1 << 16,
    CR_INTERCEPT_CR1_WRITE = 1 << 17,
    CR_INTERCEPT_CR2_WRITE = 1 << 18,
    CR_INTERCEPT_CR3_WRITE = 1 << 19,
    CR_INTERCEPT_CR4_WRITE = 1 << 20,
    CR_INTERCEPT_CR5_WRITE = 1 << 21,
    CR_INTERCEPT_CR6_WRITE = 1 << 22,
    CR_INTERCEPT_CR7_WRITE = 1 << 23,
    CR_INTERCEPT_CR8_WRITE = 1 << 24,
    CR_INTERCEPT_CR9_WRITE = 1 << 25,
    CR_INTERCEPT_CR10_WRITE = 1 << 26,
    CR_INTERCEPT_CR11_WRITE = 1 << 27,
    CR_INTERCEPT_CR12_WRITE = 1 << 28,
    CR_INTERCEPT_CR13_WRITE = 1 << 29,
    CR_INTERCEPT_CR14_WRITE = 1 << 30,
    CR_INTERCEPT_CR15_WRITE = 1 << 31,
};
enum DRInterceptBits
{
    DR_INTERCEPT_DR0_READ = 1 << 0,
    DR_INTERCEPT_DR1_READ = 1 << 1,
    DR_INTERCEPT_DR2_READ = 1 << 2,
    DR_INTERCEPT_DR3_READ = 1 << 3,
    DR_INTERCEPT_DR4_READ = 1 << 4,
    DR_INTERCEPT_DR5_READ = 1 << 5,
    DR_INTERCEPT_DR6_READ = 1 << 6,
    DR_INTERCEPT_DR7_READ = 1 << 7,
    DR_INTERCEPT_DR8_READ = 1 << 8,
    DR_INTERCEPT_DR9_READ = 1 << 9,
    DR_INTERCEPT_DR10_READ = 1 << 10,
    DR_INTERCEPT_DR11_READ = 1 << 11,
    DR_INTERCEPT_DR12_READ = 1 << 12,
    DR_INTERCEPT_DR13_READ = 1 << 13,
    DR_INTERCEPT_DR14_READ = 1 << 14,
    DR_INTERCEPT_DR15_READ = 1 << 15,
    DR_INTERCEPT_DR0_WRITE = 1 << 16,
    DR_INTERCEPT_DR1_WRITE = 1 << 17,
    DR_INTERCEPT_DR2_WRITE = 1 << 18,
    DR_INTERCEPT_DR3_WRITE = 1 << 19,
    DR_INTERCEPT_DR4_WRITE = 1 << 20,
    DR_INTERCEPT_DR5_WRITE = 1 << 21,
    DR_INTERCEPT_DR6_WRITE = 1 << 22,
    DR_INTERCEPT_DR7_WRITE = 1 << 23,
    DR_INTERCEPT_DR8_WRITE = 1 << 24,
    DR_INTERCEPT_DR9_WRITE = 1 << 25,
    DR_INTERCEPT_DR10_WRITE = 1 << 26,
    DR_INTERCEPT_DR11_WRITE = 1 << 27,
    DR_INTERCEPT_DR12_WRITE = 1 << 28,
    DR_INTERCEPT_DR13_WRITE = 1 << 29,
    DR_INTERCEPT_DR14_WRITE = 1 << 30,
    DR_INTERCEPT_DR15_WRITE = 1 << 31,
};
enum VMEXIT_EXITCODE
{
    VMEXIT_CR0_READ = 0,
    VMEXIT_CR1_READ = 1,
    VMEXIT_CR2_READ = 2,
    VMEXIT_CR3_READ = 3,
    VMEXIT_CR4_READ = 4,
    VMEXIT_CR5_READ = 5,
    VMEXIT_CR6_READ = 6,
    VMEXIT_CR7_READ = 7,
    VMEXIT_CR8_READ = 8,
    VMEXIT_CR9_READ = 9,
    VMEXIT_CR10_READ = 10,
    VMEXIT_CR11_READ = 11,
    VMEXIT_CR12_READ = 12,
    VMEXIT_CR13_READ = 13,
    VMEXIT_CR14_READ = 14,
    VMEXIT_CR15_READ = 15,
    VMEXIT_CR0_WRITE = 16,
    VMEXIT_CR1_WRITE = 17,
    VMEXIT_CR2_WRITE = 18,
    VMEXIT_CR3_WRITE = 19,
    VMEXIT_CR4_WRITE = 20,
    VMEXIT_CR5_WRITE = 21,
    VMEXIT_CR6_WRITE = 22,
    VMEXIT_CR7_WRITE = 23,
    VMEXIT_CR8_WRITE = 24,
    VMEXIT_CR9_WRITE = 25,
    VMEXIT_CR10_WRITE = 26,
    VMEXIT_CR11_WRITE = 27,
    VMEXIT_CR12_WRITE = 28,
    VMEXIT_CR13_WRITE = 29,
    VMEXIT_CR14_WRITE = 30,
    VMEXIT_CR15_WRITE = 31,
    VMEXIT_DR0_READ = 32,
    VMEXIT_DR1_READ = 33,
    VMEXIT_DR2_READ = 34,
    VMEXIT_DR3_READ = 35,
    VMEXIT_DR4_READ = 36,
    VMEXIT_DR5_READ = 37,
    VMEXIT_DR6_READ = 38,
    VMEXIT_DR7_READ = 39,
    VMEXIT_DR8_READ = 40,
    VMEXIT_DR9_READ = 41,
    VMEXIT_DR10_READ = 42,
    VMEXIT_DR11_READ = 43,
    VMEXIT_DR12_READ = 44,
    VMEXIT_DR13_READ = 45,
    VMEXIT_DR14_READ = 46,
    VMEXIT_DR15_READ = 47,
    VMEXIT_DR0_WRITE = 48,
    VMEXIT_DR1_WRITE = 49,
    VMEXIT_DR2_WRITE = 50,
    VMEXIT_DR3_WRITE = 51,
    VMEXIT_DR4_WRITE = 52,
    VMEXIT_DR5_WRITE = 53,
    VMEXIT_DR6_WRITE = 54,
    VMEXIT_DR7_WRITE = 55,
    VMEXIT_DR8_WRITE = 56,
    VMEXIT_DR9_WRITE = 57,
    VMEXIT_DR10_WRITE = 58,
    VMEXIT_DR11_WRITE = 59,
    VMEXIT_DR12_WRITE = 60,
    VMEXIT_DR13_WRITE = 61,
    VMEXIT_DR14_WRITE = 62,
    VMEXIT_DR15_WRITE = 63,
    VMEXIT_EXCEPTION_DE = 64,
    VMEXIT_EXCEPTION_DB = 65,
    VMEXIT_EXCEPTION_NMI = 66,
    VMEXIT_EXCEPTION_BP = 67,
    VMEXIT_EXCEPTION_OF = 68,
    VMEXIT_EXCEPTION_BR = 69,
    VMEXIT_EXCEPTION_UD = 70,
    VMEXIT_EXCEPTION_NM = 71,
    VMEXIT_EXCEPTION_DF = 72,
    VMEXIT_EXCEPTION_09 = 73,
    VMEXIT_EXCEPTION_TS = 74,
    VMEXIT_EXCEPTION_NP = 75,
    VMEXIT_EXCEPTION_SS = 76,
    VMEXIT_EXCEPTION_GP = 77,
    VMEXIT_EXCEPTION_PF = 78,
    VMEXIT_EXCEPTION_15 = 79,
    VMEXIT_EXCEPTION_MF = 80,
    VMEXIT_EXCEPTION_AC = 81,
    VMEXIT_EXCEPTION_MC = 82,
    VMEXIT_EXCEPTION_XF = 83,
    VMEXIT_INTR = 96,
    VMEXIT_NMI = 97,
    VMEXIT_SMI = 98,
    VMEXIT_INIT = 99,
    VMEXIT_VINTR = 100,
    VMEXIT_CR0_SEL_WRITE = 101,
    VMEXIT_IDTR_READ = 102,
    VMEXIT_GDTR_READ = 103,
    VMEXIT_LDTR_READ = 104,
    VMEXIT_TR_READ = 105,
    VMEXIT_IDTR_WRITE = 106,
    VMEXIT_GDTR_WRITE = 107,
    VMEXIT_LDTR_WRITE = 108,
    VMEXIT_TR_WRITE = 109,
    VMEXIT_RDTSC = 110,
    VMEXIT_RDPMC = 111,
    VMEXIT_PUSHF = 112,
    VMEXIT_POPF = 113,
    VMEXIT_CPUID = 114,
    VMEXIT_RSM = 115,
    VMEXIT_IRET = 116,
    VMEXIT_SWINT = 117,
    VMEXIT_INVD = 118,
    VMEXIT_PAUSE = 119,
    VMEXIT_HLT = 120,
    VMEXIT_INVLPG = 121,
    VMEXIT_INVLPGA = 122,
    VMEXIT_IOIO = 123,
    VMEXIT_MSR = 124,
    VMEXIT_TASK_SWITCH = 125,
    VMEXIT_FERR_FREEZE = 126,
    VMEXIT_SHUTDOWN = 127,
    VMEXIT_VMRUN = 128,
    VMEXIT_VMMCALL = 129,
    VMEXIT_VMLOAD = 130,
    VMEXIT_VMSAVE = 131,
    VMEXIT_STGI = 132,
    VMEXIT_CLGI = 133,
    VMEXIT_SKINIT = 134,
    VMEXIT_RDTSCP = 135,
    VMEXIT_ICEBP = 136,
    VMEXIT_WBINVD = 137,
    VMEXIT_MONITOR = 138,
    VMEXIT_MWAIT = 139,
    VMEXIT_MWAIT_CONDITIONAL= 140,
    VMEXIT_XSETBV = 141,
    VMEXIT_NPF = 1024,
    VMEXIT_INVALID = -1
};
typedef struct segment_register svm_segment_register_t;
typedef union
{
    u64 bytes;
    struct
    {
        u64 vector: 8;
        u64 type: 3;
        u64 ev: 1;
        u64 resvd1: 19;
        u64 v: 1;
        u64 errorcode:32;
    } fields;
} __attribute__ ((packed)) eventinj_t;
typedef union
{
    u64 bytes;
    struct
    {
        u64 tpr: 8;
        u64 irq: 1;
        u64 rsvd0: 7;
        u64 prio: 4;
        u64 ign_tpr: 1;
        u64 rsvd1: 3;
        u64 intr_masking: 1;
        u64 rsvd2: 7;
        u64 vector: 8;
        u64 rsvd3: 24;
    } fields;
} __attribute__ ((packed)) vintr_t;
typedef union
{
    u64 bytes;
    struct
    {
        u64 type: 1;
        u64 rsv0: 1;
        u64 str: 1;
        u64 rep: 1;
        u64 sz8: 1;
        u64 sz16: 1;
        u64 sz32: 1;
        u64 rsv1: 9;
        u64 port: 16;
    } fields;
} __attribute__ ((packed)) ioio_info_t;
typedef union
{
    u64 bytes;
    struct
    {
        u64 enable:1;
    } fields;
} __attribute__ ((packed)) lbrctrl_t;
typedef union
{
    uint32_t bytes;
    struct
    {
        uint32_t intercepts: 1;
        uint32_t iopm: 1;
        uint32_t asid: 1;
        uint32_t tpr: 1;
        uint32_t np: 1;
        uint32_t cr: 1;
        uint32_t dr: 1;
        uint32_t dt: 1;
        uint32_t seg: 1;
        uint32_t cr2: 1;
        uint32_t lbr: 1;
        uint32_t resv: 21;
    } fields;
} __attribute__ ((packed)) vmcbcleanbits_t;
struct vmcb_struct {
    u32 _cr_intercepts;
    u32 _dr_intercepts;
    u32 _exception_intercepts;
    u32 _general1_intercepts;
    u32 _general2_intercepts;
    u32 res01;
    u64 res02;
    u64 res03;
    u64 res04;
    u64 res05;
    u32 res06;
    u16 res06a;
    u16 _pause_filter_count;
    u64 _iopm_base_pa;
    u64 _msrpm_base_pa;
    u64 _tsc_offset;
    u32 _guest_asid;
    u8 tlb_control;
    u8 res07[3];
    vintr_t _vintr;
    u64 interrupt_shadow;
    u64 exitcode;
    u64 exitinfo1;
    u64 exitinfo2;
    eventinj_t exitintinfo;
    u64 _np_enable;
    u64 res08[2];
    eventinj_t eventinj;
    u64 _h_cr3;
    lbrctrl_t lbr_control;
    vmcbcleanbits_t cleanbits;
    u32 res09;
    u64 nextrip;
    u8 guest_ins_len;
    u8 guest_ins[15];
    u64 res10a[100];
    svm_segment_register_t es;
    svm_segment_register_t cs;
    svm_segment_register_t ss;
    svm_segment_register_t ds;
    svm_segment_register_t fs;
    svm_segment_register_t gs;
    svm_segment_register_t gdtr;
    svm_segment_register_t ldtr;
    svm_segment_register_t idtr;
    svm_segment_register_t tr;
    u64 res10[5];
    u8 res11[3];
    u8 _cpl;
    u32 res12;
    u64 _efer;
    u64 res13[14];
    u64 _cr4;
    u64 _cr3;
    u64 _cr0;
    u64 _dr7;
    u64 _dr6;
    u64 rflags;
    u64 rip;
    u64 res14[11];
    u64 rsp;
    u64 res15[3];
    u64 rax;
    u64 star;
    u64 lstar;
    u64 cstar;
    u64 sfmask;
    u64 kerngsbase;
    u64 sysenter_cs;
    u64 sysenter_esp;
    u64 sysenter_eip;
    u64 _cr2;
    u64 pdpe0;
    u64 pdpe1;
    u64 pdpe2;
    u64 pdpe3;
    u64 _g_pat;
    u64 _debugctlmsr;
    u64 _lastbranchfromip;
    u64 _lastbranchtoip;
    u64 _lastintfromip;
    u64 _lastinttoip;
    u64 res16[301];
} __attribute__ ((packed));
struct svm_domain {
};
struct arch_svm_struct {
    struct vmcb_struct *vmcb;
    u64 vmcb_pa;
    unsigned long *msrpm;
    int launch_core;
    bool_t vmcb_in_sync;
    uint8_t cached_insn_len;
    uint64_t guest_sysenter_cs;
    uint64_t guest_sysenter_esp;
    uint64_t guest_sysenter_eip;
    uint64_t guest_lwp_cfg;
    uint64_t cpu_lwp_cfg;
    struct {
        u64 length;
        u64 status;
    } osvw;
};
struct vmcb_struct *alloc_vmcb(void);
struct host_save_area *alloc_host_save_area(void);
void free_vmcb(struct vmcb_struct *vmcb);
int svm_create_vmcb(struct vcpu *v);
void svm_destroy_vmcb(struct vcpu *v);
void setup_vmcb_dump(void);
void svm_intercept_msr(struct vcpu *v, uint32_t msr, int enable);
static __inline__ void vmcb_set_cr_intercepts(struct vmcb_struct *vmcb, u32 value) { vmcb->_cr_intercepts = value; vmcb->cleanbits.fields.intercepts = 0; } static __inline__ u32 vmcb_get_cr_intercepts(struct vmcb_struct *vmcb) { return vmcb->_cr_intercepts; }
static __inline__ void vmcb_set_dr_intercepts(struct vmcb_struct *vmcb, u32 value) { vmcb->_dr_intercepts = value; vmcb->cleanbits.fields.intercepts = 0; } static __inline__ u32 vmcb_get_dr_intercepts(struct vmcb_struct *vmcb) { return vmcb->_dr_intercepts; }
static __inline__ void vmcb_set_exception_intercepts(struct vmcb_struct *vmcb, u32 value) { vmcb->_exception_intercepts = value; vmcb->cleanbits.fields.intercepts = 0; } static __inline__ u32 vmcb_get_exception_intercepts(struct vmcb_struct *vmcb) { return vmcb->_exception_intercepts; }
static __inline__ void vmcb_set_general1_intercepts(struct vmcb_struct *vmcb, u32 value) { vmcb->_general1_intercepts = value; vmcb->cleanbits.fields.intercepts = 0; } static __inline__ u32 vmcb_get_general1_intercepts(struct vmcb_struct *vmcb) { return vmcb->_general1_intercepts; }
static __inline__ void vmcb_set_general2_intercepts(struct vmcb_struct *vmcb, u32 value) { vmcb->_general2_intercepts = value; vmcb->cleanbits.fields.intercepts = 0; } static __inline__ u32 vmcb_get_general2_intercepts(struct vmcb_struct *vmcb) { return vmcb->_general2_intercepts; }
static __inline__ void vmcb_set_pause_filter_count(struct vmcb_struct *vmcb, u16 value) { vmcb->_pause_filter_count = value; vmcb->cleanbits.fields.intercepts = 0; } static __inline__ u16 vmcb_get_pause_filter_count(struct vmcb_struct *vmcb) { return vmcb->_pause_filter_count; }
static __inline__ void vmcb_set_tsc_offset(struct vmcb_struct *vmcb, u64 value) { vmcb->_tsc_offset = value; vmcb->cleanbits.fields.intercepts = 0; } static __inline__ u64 vmcb_get_tsc_offset(struct vmcb_struct *vmcb) { return vmcb->_tsc_offset; }
static __inline__ void vmcb_set_iopm_base_pa(struct vmcb_struct *vmcb, u64 value) { vmcb->_iopm_base_pa = value; vmcb->cleanbits.fields.iopm = 0; } static __inline__ u64 vmcb_get_iopm_base_pa(struct vmcb_struct *vmcb) { return vmcb->_iopm_base_pa; }
static __inline__ void vmcb_set_msrpm_base_pa(struct vmcb_struct *vmcb, u64 value) { vmcb->_msrpm_base_pa = value; vmcb->cleanbits.fields.iopm = 0; } static __inline__ u64 vmcb_get_msrpm_base_pa(struct vmcb_struct *vmcb) { return vmcb->_msrpm_base_pa; }
static __inline__ void vmcb_set_guest_asid(struct vmcb_struct *vmcb, u32 value) { vmcb->_guest_asid = value; vmcb->cleanbits.fields.asid = 0; } static __inline__ u32 vmcb_get_guest_asid(struct vmcb_struct *vmcb) { return vmcb->_guest_asid; }
static __inline__ void vmcb_set_vintr(struct vmcb_struct *vmcb, vintr_t value) { vmcb->_vintr = value; vmcb->cleanbits.fields.tpr = 0; } static __inline__ vintr_t vmcb_get_vintr(struct vmcb_struct *vmcb) { return vmcb->_vintr; }
static __inline__ void vmcb_set_np_enable(struct vmcb_struct *vmcb, u64 value) { vmcb->_np_enable = value; vmcb->cleanbits.fields.np = 0; } static __inline__ u64 vmcb_get_np_enable(struct vmcb_struct *vmcb) { return vmcb->_np_enable; }
static __inline__ void vmcb_set_h_cr3(struct vmcb_struct *vmcb, u64 value) { vmcb->_h_cr3 = value; vmcb->cleanbits.fields.np = 0; } static __inline__ u64 vmcb_get_h_cr3(struct vmcb_struct *vmcb) { return vmcb->_h_cr3; }
static __inline__ void vmcb_set_g_pat(struct vmcb_struct *vmcb, u64 value) { vmcb->_g_pat = value; vmcb->cleanbits.fields.np = 0; } static __inline__ u64 vmcb_get_g_pat(struct vmcb_struct *vmcb) { return vmcb->_g_pat; }
static __inline__ void vmcb_set_cr0(struct vmcb_struct *vmcb, u64 value) { vmcb->_cr0 = value; vmcb->cleanbits.fields.cr = 0; } static __inline__ u64 vmcb_get_cr0(struct vmcb_struct *vmcb) { return vmcb->_cr0; }
static __inline__ void vmcb_set_cr3(struct vmcb_struct *vmcb, u64 value) { vmcb->_cr3 = value; vmcb->cleanbits.fields.cr = 0; } static __inline__ u64 vmcb_get_cr3(struct vmcb_struct *vmcb) { return vmcb->_cr3; }
static __inline__ void vmcb_set_cr4(struct vmcb_struct *vmcb, u64 value) { vmcb->_cr4 = value; vmcb->cleanbits.fields.cr = 0; } static __inline__ u64 vmcb_get_cr4(struct vmcb_struct *vmcb) { return vmcb->_cr4; }
static __inline__ void vmcb_set_efer(struct vmcb_struct *vmcb, u64 value) { vmcb->_efer = value; vmcb->cleanbits.fields.cr = 0; } static __inline__ u64 vmcb_get_efer(struct vmcb_struct *vmcb) { return vmcb->_efer; }
static __inline__ void vmcb_set_dr6(struct vmcb_struct *vmcb, u64 value) { vmcb->_dr6 = value; vmcb->cleanbits.fields.dr = 0; } static __inline__ u64 vmcb_get_dr6(struct vmcb_struct *vmcb) { return vmcb->_dr6; }
static __inline__ void vmcb_set_dr7(struct vmcb_struct *vmcb, u64 value) { vmcb->_dr7 = value; vmcb->cleanbits.fields.dr = 0; } static __inline__ u64 vmcb_get_dr7(struct vmcb_struct *vmcb) { return vmcb->_dr7; }
static __inline__ void vmcb_set_cpl(struct vmcb_struct *vmcb, u8 value) { vmcb->_cpl = value; vmcb->cleanbits.fields.seg = 0; } static __inline__ u8 vmcb_get_cpl(struct vmcb_struct *vmcb) { return vmcb->_cpl; }
static __inline__ void vmcb_set_cr2(struct vmcb_struct *vmcb, u64 value) { vmcb->_cr2 = value; vmcb->cleanbits.fields.cr2 = 0; } static __inline__ u64 vmcb_get_cr2(struct vmcb_struct *vmcb) { return vmcb->_cr2; }
static __inline__ void vmcb_set_debugctlmsr(struct vmcb_struct *vmcb, u64 value) { vmcb->_debugctlmsr = value; vmcb->cleanbits.fields.lbr = 0; } static __inline__ u64 vmcb_get_debugctlmsr(struct vmcb_struct *vmcb) { return vmcb->_debugctlmsr; }
static __inline__ void vmcb_set_lastbranchfromip(struct vmcb_struct *vmcb, u64 value) { vmcb->_lastbranchfromip = value; vmcb->cleanbits.fields.lbr = 0; } static __inline__ u64 vmcb_get_lastbranchfromip(struct vmcb_struct *vmcb) { return vmcb->_lastbranchfromip; }
static __inline__ void vmcb_set_lastbranchtoip(struct vmcb_struct *vmcb, u64 value) { vmcb->_lastbranchtoip = value; vmcb->cleanbits.fields.lbr = 0; } static __inline__ u64 vmcb_get_lastbranchtoip(struct vmcb_struct *vmcb) { return vmcb->_lastbranchtoip; }
static __inline__ void vmcb_set_lastintfromip(struct vmcb_struct *vmcb, u64 value) { vmcb->_lastintfromip = value; vmcb->cleanbits.fields.lbr = 0; } static __inline__ u64 vmcb_get_lastintfromip(struct vmcb_struct *vmcb) { return vmcb->_lastintfromip; }
static __inline__ void vmcb_set_lastinttoip(struct vmcb_struct *vmcb, u64 value) { vmcb->_lastinttoip = value; vmcb->cleanbits.fields.lbr = 0; } static __inline__ u64 vmcb_get_lastinttoip(struct vmcb_struct *vmcb) { return vmcb->_lastinttoip; }
struct nestedsvm {
    bool_t ns_gif;
    uint64_t ns_msr_hsavepa;
    uint64_t ns_ovvmcb_pa;
    uint64_t ns_tscratio;
    uint32_t ns_cr_intercepts;
    uint32_t ns_dr_intercepts;
    uint32_t ns_exception_intercepts;
    uint32_t ns_general1_intercepts;
    uint32_t ns_general2_intercepts;
    lbrctrl_t ns_lbr_control;
    unsigned long *ns_cached_msrpm;
    unsigned long *ns_merged_msrpm;
    paddr_t ns_iomap_pa, ns_oiomap_pa;
    unsigned long *ns_iomap;
    uint64_t ns_cr0;
    uint64_t ns_vmcb_guestcr3, ns_vmcb_hostcr3;
    uint32_t ns_guest_asid;
    bool_t ns_hap_enabled;
    struct {
        uint64_t exitcode;
        uint64_t exitinfo1;
        uint64_t exitinfo2;
    } ns_vmexit;
    union {
        uint32_t bytes;
        struct {
            uint32_t rflagsif: 1;
            uint32_t vintrmask: 1;
            uint32_t reserved: 30;
        } fields;
    } ns_hostflags;
};
int nestedsvm_vmcb_map(struct vcpu *v, uint64_t vmcbaddr);
void nestedsvm_vmexit_defer(struct vcpu *v,
    uint64_t exitcode, uint64_t exitinfo1, uint64_t exitinfo2);
enum nestedhvm_vmexits
nestedsvm_vmexit_n2n1(struct vcpu *v, struct cpu_user_regs *regs);
enum nestedhvm_vmexits
nestedsvm_check_intercepts(struct vcpu *v, struct cpu_user_regs *regs,
    uint64_t exitcode);
void nsvm_vcpu_destroy(struct vcpu *v);
int nsvm_vcpu_initialise(struct vcpu *v);
int nsvm_vcpu_reset(struct vcpu *v);
int nsvm_vcpu_hostrestore(struct vcpu *v, struct cpu_user_regs *regs);
int nsvm_vcpu_vmrun(struct vcpu *v, struct cpu_user_regs *regs);
int nsvm_vcpu_vmexit_inject(struct vcpu *v, struct cpu_user_regs *regs,
    uint64_t exitcode);
int nsvm_vcpu_vmexit_trap(struct vcpu *v, struct hvm_trap *trap);
uint64_t nsvm_vcpu_guestcr3(struct vcpu *v);
uint64_t nsvm_vcpu_hostcr3(struct vcpu *v);
uint32_t nsvm_vcpu_asid(struct vcpu *v);
int nsvm_vmcb_guest_intercepts_exitcode(struct vcpu *v,
    struct cpu_user_regs *regs, uint64_t exitcode);
int nsvm_vmcb_guest_intercepts_trap(struct vcpu *v, unsigned int trapnr,
                                    int errcode);
bool_t nsvm_vmcb_hap_enabled(struct vcpu *v);
enum hvm_intblk nsvm_intr_blocked(struct vcpu *v);
int nsvm_rdmsr(struct vcpu *v, unsigned int msr, uint64_t *msr_content);
int nsvm_wrmsr(struct vcpu *v, unsigned int msr, uint64_t msr_content);
void svm_vmexit_do_clgi(struct cpu_user_regs *regs, struct vcpu *v);
void svm_vmexit_do_stgi(struct cpu_user_regs *regs, struct vcpu *v);
bool_t nestedsvm_gif_isset(struct vcpu *v);
int nsvm_hap_walk_L1_p2m(struct vcpu *v, paddr_t L2_gpa, paddr_t *L1_gpa,
                         unsigned int *page_order, uint8_t *p2m_acc,
                         bool_t access_r, bool_t access_w, bool_t access_x);
int nestedsvm_vcpu_interrupt(struct vcpu *v, const struct hvm_intack intack);
enum {
    PAT_TYPE_UNCACHABLE=0,
    PAT_TYPE_WRCOMB=1,
    PAT_TYPE_RESERVED=2,
    PAT_TYPE_WRTHROUGH=4,
    PAT_TYPE_WRPROT=5,
    PAT_TYPE_WRBACK=6,
    PAT_TYPE_UC_MINUS=7,
    PAT_TYPE_NUMS
};
typedef u8 mtrr_type;
struct mtrr_var_range {
 uint64_t base;
 uint64_t mask;
};
struct mtrr_state {
 struct mtrr_var_range *var_ranges;
 mtrr_type fixed_ranges[88];
 unsigned char enabled;
 unsigned char have_fixed;
 mtrr_type def_type;
 u64 mtrr_cap;
 bool_t overlapped;
};
extern struct mtrr_state mtrr_state;
extern void mtrr_save_fixed_ranges(void *);
extern void mtrr_save_state(void);
extern int mtrr_add(unsigned long base, unsigned long size,
                    unsigned int type, char increment);
extern int mtrr_add_page(unsigned long base, unsigned long size,
                         unsigned int type, char increment);
extern int mtrr_del(int reg, unsigned long base, unsigned long size);
extern int mtrr_del_page(int reg, unsigned long base, unsigned long size);
extern void mtrr_centaur_report_mcr(int mcr, u32 lo, u32 hi);
extern u32 get_pat_flags(struct vcpu *v, u32 gl1e_flags, paddr_t gpaddr,
                  paddr_t spaddr, uint8_t gmtrr_mtype);
extern uint8_t epte_get_entry_emt(struct domain *d, unsigned long gfn,
                                  mfn_t mfn, uint8_t *ipat, bool_t direct_mmio);
extern void ept_change_entry_emt_with_range(
    struct domain *d, unsigned long start_gfn, unsigned long end_gfn);
extern unsigned char pat_type_2_pte_flags(unsigned char pat_type);
extern int hold_mtrr_updates_on_aps;
extern void mtrr_aps_sync_begin(void);
extern void mtrr_aps_sync_end(void);
extern void mtrr_bp_restore(void);
extern bool_t mtrr_var_range_msr_set(
    struct domain *d, struct mtrr_state *m,
    uint32_t msr, uint64_t msr_content);
extern bool_t mtrr_fix_range_msr_set(struct mtrr_state *v,
    uint32_t row, uint64_t msr_content);
extern bool_t mtrr_def_type_msr_set(struct mtrr_state *v, uint64_t msr_content);
extern bool_t pat_msr_set(uint64_t *pat, uint64_t msr);
bool_t is_var_mtrr_overlapped(struct mtrr_state *m);
bool_t mtrr_pat_not_equal(struct vcpu *vd, struct vcpu *vs);
enum hvm_io_state {
    HVMIO_none = 0,
    HVMIO_dispatched,
    HVMIO_awaiting_completion,
    HVMIO_handle_mmio_awaiting_completion,
    HVMIO_handle_pio_awaiting_completion,
    HVMIO_completed
};
struct hvm_vcpu_asid {
    uint64_t generation;
    uint32_t asid;
};
struct hvm_vcpu_io {
    enum hvm_io_state io_state;
    unsigned long io_data;
    int io_size;
    unsigned long mmio_gva;
    unsigned long mmio_gpfn;
    paddr_t mmio_large_read_pa;
    uint8_t mmio_large_read[32];
    unsigned int mmio_large_read_bytes;
    unsigned int mmio_large_write_bytes;
    paddr_t mmio_large_write_pa;
    unsigned int mmio_insn_bytes;
    unsigned char mmio_insn[16];
    bool_t mmio_retry, mmio_retrying;
    unsigned long msix_unmask_address;
};
struct nestedvcpu {
    bool_t nv_guestmode;
    void *nv_vvmcx;
    void *nv_n1vmcx;
    void *nv_n2vmcx;
    uint64_t nv_vvmcxaddr;
    uint64_t nv_n1vmcx_pa;
    uint64_t nv_n2vmcx_pa;
    union {
        struct nestedsvm nsvm;
        struct nestedvmx nvmx;
    } u;
    bool_t nv_flushp2m;
    struct p2m_domain *nv_p2m;
    struct hvm_vcpu_asid nv_n2asid;
    bool_t nv_vmentry_pending;
    bool_t nv_vmexit_pending;
    bool_t nv_vmswitch_in_progress;
    bool_t nv_ioport80;
    bool_t nv_ioportED;
    unsigned long guest_cr[5];
};
struct hvm_vcpu {
    unsigned long guest_cr[5];
    unsigned long guest_efer;
    unsigned long hw_cr[5];
    struct vlapic vlapic;
    s64 cache_tsc_offset;
    u64 guest_time;
    spinlock_t tm_lock;
    struct list_head tm_list;
    int xen_port;
    bool_t flag_dr_dirty;
    bool_t debug_state_latch;
    bool_t single_step;
    bool_t hcall_preempted;
    bool_t hcall_64bit;
    struct hvm_vcpu_asid n1asid;
    u32 msr_tsc_aux;
    u64 msr_tsc_adjust;
    struct vpmu_struct vpmu;
    union {
        struct arch_vmx_struct vmx;
        struct arch_svm_struct svm;
    } u;
    struct tasklet assert_evtchn_irq_tasklet;
    struct nestedvcpu nvcpu;
    struct mtrr_state mtrr;
    u64 pat_cr;
    int64_t stime_offset;
    u8 cache_mode;
    struct hvm_vcpu_io hvm_io;
    void (*fpu_exception_callback)(void *, struct cpu_user_regs *);
    void *fpu_exception_callback_arg;
    struct hvm_trap inject_trap;
    struct viridian_vcpu viridian;
};
typedef struct {
 unsigned int __softirq_pending;
 unsigned int __local_irq_count;
 unsigned int __nmi_count;
 bool_t __mwait_wakeup;
} __attribute__((__aligned__((1 << (7))))) irq_cpustat_t;
extern irq_cpustat_t irq_stat[];
void ack_bad_irq(unsigned int irq);
extern void apic_intr_init(void);
extern void smp_intr_init(void);
struct irqaction {
    void (*handler)(int, void *, struct cpu_user_regs *);
    const char *name;
    void *dev_id;
    bool_t free_on_release;
};
struct irq_desc;
struct hw_interrupt_type {
    const char *typename;
    unsigned int (*startup)(struct irq_desc *);
    void (*shutdown)(struct irq_desc *);
    void (*enable)(struct irq_desc *);
    void (*disable)(struct irq_desc *);
    void (*ack)(struct irq_desc *);
    void (*end)(struct irq_desc *, u8 vector);
    void (*set_affinity)(struct irq_desc *, const cpumask_t *);
};
typedef const struct hw_interrupt_type hw_irq_controller;
extern unsigned int nr_irqs_gsi;
extern unsigned int nr_irqs;
typedef struct {
    unsigned long _bits[(((256)+((1 << 3) << 3)-1)/((1 << 3) << 3))];
} vmask_t;
struct irq_desc;
struct arch_irq_desc {
        s16 vector;
        s16 old_vector;
        cpumask_var_t cpu_mask;
        cpumask_var_t old_cpu_mask;
        cpumask_var_t pending_mask;
        unsigned move_cleanup_count;
        vmask_t *used_vectors;
        u8 move_in_progress : 1;
        s8 used;
};
typedef int vector_irq_t[256];
extern __typeof__(vector_irq_t) per_cpu__vector_irq;
extern bool_t opt_noirqbalance;
extern int opt_irq_vector_map;
extern __typeof__(struct cpu_user_regs *) per_cpu____irq_regs;
static __inline__ struct cpu_user_regs *get_irq_regs(void)
{
 return (*({ unsigned long __ptr; __asm__ ("" : "=r"(__ptr) : "0"(&per_cpu____irq_regs)); (typeof(&per_cpu____irq_regs)) (__ptr + (get_cpu_info()->per_cpu_offset)); }));
}
static __inline__ struct cpu_user_regs *set_irq_regs(struct cpu_user_regs *new_regs)
{
 struct cpu_user_regs *old_regs, **pp_regs = &(*({ unsigned long __ptr; __asm__ ("" : "=r"(__ptr) : "0"(&per_cpu____irq_regs)); (typeof(&per_cpu____irq_regs)) (__ptr + (get_cpu_info()->per_cpu_offset)); }));
 old_regs = *pp_regs;
 *pp_regs = new_regs;
 return old_regs;
}
void event_check_interrupt(struct cpu_user_regs *regs);
void invalidate_interrupt(struct cpu_user_regs *regs);
void call_function_interrupt(struct cpu_user_regs *regs);
void apic_timer_interrupt(struct cpu_user_regs *regs);
void error_interrupt(struct cpu_user_regs *regs);
void pmu_apic_interrupt(struct cpu_user_regs *regs);
void spurious_interrupt(struct cpu_user_regs *regs);
void irq_move_cleanup_interrupt(struct cpu_user_regs *regs);
uint8_t alloc_hipriority_vector(void);
void set_direct_apic_vector(
    uint8_t vector, void (*handler)(struct cpu_user_regs *));
void alloc_direct_apic_vector(
    uint8_t *vector, void (*handler)(struct cpu_user_regs *));
void do_IRQ(struct cpu_user_regs *regs);
void disable_8259A_irq(struct irq_desc *);
void enable_8259A_irq(struct irq_desc *);
int i8259A_irq_pending(unsigned int irq);
void mask_8259A(void);
void unmask_8259A(void);
void init_8259A(int aeoi);
void make_8259A_irq(unsigned int irq);
bool_t bogus_8259A_irq(unsigned int irq);
int i8259A_suspend(void);
int i8259A_resume(void);
void setup_IO_APIC(void);
void disable_IO_APIC(void);
void setup_ioapic_dest(void);
vmask_t *io_apic_get_used_vector_map(unsigned int irq);
extern unsigned int io_apic_irqs;
extern __typeof__(unsigned int) per_cpu__irq_count;
struct pirq;
struct arch_pirq {
    int irq;
    union {
        struct hvm_pirq {
            int emuirq;
            struct hvm_pirq_dpci dpci;
        } hvm;
    };
};
int pirq_shared(struct domain *d , int irq);
int map_domain_pirq(struct domain *d, int pirq, int irq, int type,
                           void *data);
int unmap_domain_pirq(struct domain *d, int pirq);
int get_free_pirq(struct domain *d, int type);
int get_free_pirqs(struct domain *, unsigned int nr);
void free_domain_pirqs(struct domain *d);
int map_domain_emuirq_pirq(struct domain *d, int pirq, int irq);
int unmap_domain_pirq_emuirq(struct domain *d, int pirq);
bool_t hvm_domain_use_pirq(const struct domain *, const struct pirq *);
void fixup_irqs(void);
int init_irq_data(void);
void clear_irq_vector(int irq);
int irq_to_vector(int irq);
int create_irq(int node);
void destroy_irq(unsigned int irq);
int assign_irq_vector(int irq, const cpumask_t *);
extern void irq_complete_move(struct irq_desc *);
extern struct irq_desc *irq_desc;
void lock_vector_lock(void);
void unlock_vector_lock(void);
void __setup_vector_irq(int cpu);
void move_native_irq(struct irq_desc *);
void move_masked_irq(struct irq_desc *);
int bind_irq_vector(int irq, int vector, const cpumask_t *);
void irq_set_affinity(struct irq_desc *, const cpumask_t *mask);
int init_domain_irq_mapping(struct domain *);
void cleanup_domain_irq_mapping(struct domain *);
bool_t cpu_has_pending_apic_eoi(void);
struct msi_desc;
typedef struct irq_desc {
    unsigned int status;
    hw_irq_controller *handler;
    struct msi_desc *msi_desc;
    struct irqaction *action;
    int irq;
    spinlock_t lock;
    struct arch_irq_desc arch;
    cpumask_var_t affinity;
    s_time_t rl_quantum_start;
    unsigned int rl_cnt;
    struct list_head rl_link;
} __attribute__((__aligned__((1 << (7))))) irq_desc_t;
int init_one_irq_desc(struct irq_desc *);
int arch_init_one_irq_desc(struct irq_desc *);
extern int setup_irq(unsigned int irq, struct irqaction *);
extern void release_irq(unsigned int irq);
extern int request_irq(unsigned int irq,
               void (*handler)(int, void *, struct cpu_user_regs *),
               const char * devname, void *dev_id);
extern hw_irq_controller no_irq_type;
extern void no_action(int cpl, void *dev_id, struct cpu_user_regs *regs);
extern unsigned int irq_startup_none(struct irq_desc *);
extern void irq_actor_none(struct irq_desc *);
struct domain;
struct vcpu;
struct pirq {
    int pirq;
    u16 evtchn;
    bool_t masked;
    struct rcu_head rcu_head;
    struct arch_pirq arch;
};
extern struct pirq *pirq_get_info(struct domain *, int pirq);
void pirq_cleanup_check(struct pirq *, struct domain *);
extern void pirq_guest_eoi(struct pirq *);
extern void desc_guest_eoi(struct irq_desc *, struct pirq *);
extern int pirq_guest_unmask(struct domain *d);
extern int pirq_guest_bind(struct vcpu *, struct pirq *, int will_share);
extern void pirq_guest_unbind(struct domain *d, struct pirq *);
extern void pirq_set_affinity(struct domain *d, int irq, const cpumask_t *);
extern irq_desc_t *domain_spin_lock_irq_desc(
    struct domain *d, int irq, unsigned long *pflags);
extern irq_desc_t *pirq_spin_lock_irq_desc(
    const struct pirq *, unsigned long *pflags);
static __inline__ void set_native_irq_info(unsigned int irq, const cpumask_t *mask)
{
    cpumask_copy((&irq_desc[irq])->affinity, mask);
}
unsigned int set_desc_affinity(struct irq_desc *, const cpumask_t *);
struct arch_pci_dev {
    vmask_t used_vectors;
};
struct pci_dev_info {
    bool_t is_extfn;
    bool_t is_virtfn;
    struct {
        u8 bus;
        u8 devfn;
    } physfn;
};
struct pci_dev {
    struct list_head alldevs_list;
    struct list_head domain_list;
    struct list_head msi_list;
    struct arch_msix *msix;
    struct domain *domain;
    const u16 seg;
    const u8 bus;
    const u8 devfn;
    u8 phantom_stride;
    enum pdev_type {
        DEV_TYPE_PCI_UNKNOWN,
        DEV_TYPE_PCIe_ENDPOINT,
        DEV_TYPE_PCIe_BRIDGE,
        DEV_TYPE_PCIe2PCI_BRIDGE,
        DEV_TYPE_PCI2PCIe_BRIDGE,
        DEV_TYPE_LEGACY_PCI_BRIDGE,
        DEV_TYPE_PCI_HOST_BRIDGE,
        DEV_TYPE_PCI,
    } type;
    struct pci_dev_info info;
    struct arch_pci_dev arch;
    struct {
        s_time_t time;
        unsigned int count;
    } fault;
    u64 vf_rlen[6];
};
extern spinlock_t pcidevs_lock;
bool_t pci_known_segment(u16 seg);
int pci_device_detect(u16 seg, u8 bus, u8 dev, u8 func);
int scan_pci_devices(void);
enum pdev_type pdev_type(u16 seg, u8 bus, u8 devfn);
int find_upstream_bridge(u16 seg, u8 *bus, u8 *devfn, u8 *secbus);
struct pci_dev *pci_lock_pdev(int seg, int bus, int devfn);
struct pci_dev *pci_lock_domain_pdev(
    struct domain *, int seg, int bus, int devfn);
void setup_dom0_pci_devices(struct domain *,
                            int (*)(u8 devfn, struct pci_dev *));
void pci_release_devices(struct domain *d);
int pci_add_segment(u16 seg);
const unsigned long *pci_get_ro_map(u16 seg);
int pci_add_device(u16 seg, u8 bus, u8 devfn, const struct pci_dev_info *);
int pci_remove_device(u16 seg, u8 bus, u8 devfn);
int pci_ro_device(int seg, int bus, int devfn);
void arch_pci_ro_device(int seg, int bdf);
int pci_hide_device(int bus, int devfn);
struct pci_dev *pci_get_pdev(int seg, int bus, int devfn);
struct pci_dev *pci_get_real_pdev(int seg, int bus, int devfn);
struct pci_dev *pci_get_pdev_by_domain(
    struct domain *, int seg, int bus, int devfn);
void pci_check_disable_device(u16 seg, u8 bus, u8 devfn);
uint8_t pci_conf_read8(
    unsigned int seg, unsigned int bus, unsigned int dev, unsigned int func,
    unsigned int reg);
uint16_t pci_conf_read16(
    unsigned int seg, unsigned int bus, unsigned int dev, unsigned int func,
    unsigned int reg);
uint32_t pci_conf_read32(
    unsigned int seg, unsigned int bus, unsigned int dev, unsigned int func,
    unsigned int reg);
void pci_conf_write8(
    unsigned int seg, unsigned int bus, unsigned int dev, unsigned int func,
    unsigned int reg, uint8_t data);
void pci_conf_write16(
    unsigned int seg, unsigned int bus, unsigned int dev, unsigned int func,
    unsigned int reg, uint16_t data);
void pci_conf_write32(
    unsigned int seg, unsigned int bus, unsigned int dev, unsigned int func,
    unsigned int reg, uint32_t data);
uint32_t pci_conf_read(uint32_t cf8, uint8_t offset, uint8_t bytes);
void pci_conf_write(uint32_t cf8, uint8_t offset, uint8_t bytes, uint32_t data);
int pci_mmcfg_read(unsigned int seg, unsigned int bus,
                   unsigned int devfn, int reg, int len, u32 *value);
int pci_mmcfg_write(unsigned int seg, unsigned int bus,
                    unsigned int devfn, int reg, int len, u32 value);
int pci_find_cap_offset(u16 seg, u8 bus, u8 dev, u8 func, u8 cap);
int pci_find_next_cap(u16 seg, u8 bus, unsigned int devfn, u8 pos, int cap);
int pci_find_ext_capability(int seg, int bus, int devfn, int cap);
const char *parse_pci(const char *, unsigned int *seg, unsigned int *bus,
                      unsigned int *dev, unsigned int *func);
struct pirq;
int msixtbl_pt_register(struct domain *, struct pirq *, uint64_t gtable);
void msixtbl_pt_unregister(struct domain *, struct pirq *);
void msixtbl_pt_cleanup(struct domain *d);
extern bool_t iommu_enable, iommu_enabled;
extern bool_t force_iommu, iommu_verbose;
extern bool_t iommu_workaround_bios_bug, iommu_passthrough;
extern bool_t iommu_snoop, iommu_qinval, iommu_intremap;
extern bool_t iommu_hap_pt_share;
extern bool_t iommu_debug;
extern bool_t amd_iommu_perdev_intremap;
int iommu_setup(void);
int iommu_supports_eim(void);
int iommu_enable_x2apic_IR(void);
void iommu_disable_x2apic_IR(void);
int iommu_add_device(struct pci_dev *pdev);
int iommu_enable_device(struct pci_dev *pdev);
int iommu_remove_device(struct pci_dev *pdev);
int iommu_domain_init(struct domain *d);
void iommu_dom0_init(struct domain *d);
void iommu_domain_destroy(struct domain *d);
int deassign_device(struct domain *d, u16 seg, u8 bus, u8 devfn);
int iommu_map_page(struct domain *d, unsigned long gfn, unsigned long mfn,
                   unsigned int flags);
int iommu_unmap_page(struct domain *d, unsigned long gfn);
void iommu_pte_flush(struct domain *d, u64 gfn, u64 *pte, int order, int present);
void iommu_set_pgd(struct domain *d);
void iommu_domain_teardown(struct domain *d);
void pt_pci_init(void);
struct pirq;
int hvm_do_IRQ_dpci(struct domain *, struct pirq *);
int dpci_ioport_intercept(ioreq_t *p);
int pt_irq_create_bind(struct domain *, xen_domctl_bind_pt_irq_t *);
int pt_irq_destroy_bind(struct domain *, xen_domctl_bind_pt_irq_t *);
void hvm_dpci_isairq_eoi(struct domain *d, unsigned int isairq);
struct hvm_irq_dpci *domain_get_irq_dpci(const struct domain *);
void free_hvm_irq_dpci(struct hvm_irq_dpci *dpci);
bool_t pt_irq_need_timer(uint32_t flags);
struct msi_desc;
struct msi_msg;
struct page_info;
struct iommu_ops {
    int (*init)(struct domain *d);
    void (*dom0_init)(struct domain *d);
    int (*add_device)(u8 devfn, struct pci_dev *);
    int (*enable_device)(struct pci_dev *pdev);
    int (*remove_device)(u8 devfn, struct pci_dev *);
    int (*assign_device)(struct domain *, u8 devfn, struct pci_dev *);
    void (*teardown)(struct domain *d);
    int (*map_page)(struct domain *d, unsigned long gfn, unsigned long mfn,
                    unsigned int flags);
    int (*unmap_page)(struct domain *d, unsigned long gfn);
    void (*free_page_table)(struct page_info *);
    int (*reassign_device)(struct domain *s, struct domain *t,
      u8 devfn, struct pci_dev *);
    int (*get_device_group_id)(u16 seg, u8 bus, u8 devfn);
    void (*update_ire_from_apic)(unsigned int apic, unsigned int reg, unsigned int value);
    int (*update_ire_from_msi)(struct msi_desc *msi_desc, struct msi_msg *msg);
    void (*read_msi_from_ire)(struct msi_desc *msi_desc, struct msi_msg *msg);
    unsigned int (*read_apic_from_ire)(unsigned int apic, unsigned int reg);
    int (*setup_hpet_msi)(struct msi_desc *);
    void (*suspend)(void);
    void (*resume)(void);
    void (*share_p2m)(struct domain *d);
    void (*crash_shutdown)(void);
    void (*iotlb_flush)(struct domain *d, unsigned long gfn, unsigned int page_count);
    void (*iotlb_flush_all)(struct domain *d);
    void (*dump_p2m_table)(struct domain *d);
};
void iommu_update_ire_from_apic(unsigned int apic, unsigned int reg, unsigned int value);
int iommu_update_ire_from_msi(struct msi_desc *msi_desc, struct msi_msg *msg);
void iommu_read_msi_from_ire(struct msi_desc *msi_desc, struct msi_msg *msg);
unsigned int iommu_read_apic_from_ire(unsigned int apic, unsigned int reg);
int iommu_setup_hpet_msi(struct msi_desc *);
void iommu_suspend(void);
void iommu_resume(void);
void iommu_crash_shutdown(void);
void iommu_set_dom0_mapping(struct domain *d);
void iommu_share_p2m_table(struct domain *d);
int iommu_do_domctl(struct xen_domctl *, struct domain *d,
                    __guest_handle_xen_domctl_t);
void iommu_iotlb_flush(struct domain *d, unsigned long gfn, unsigned int page_count);
void iommu_iotlb_flush_all(struct domain *d);
int adjust_vtd_irq_affinities(void);
extern __typeof__(bool_t) per_cpu__iommu_dont_flush_iotlb;
extern struct spinlock iommu_pt_cleanup_lock;
extern struct page_list_head iommu_pt_cleanup_list;
struct g2m_ioport {
    struct list_head list;
    unsigned int gport;
    unsigned int mport;
    unsigned int np;
};
struct mapped_rmrr {
    struct list_head list;
    u64 base;
    u64 end;
};
struct hvm_iommu {
    u64 pgd_maddr;
    spinlock_t mapping_lock;
    int agaw;
    struct list_head g2m_ioport_list;
    u64 iommu_bitmap;
    struct list_head mapped_rmrrs;
    int domain_id;
    int paging_mode;
    struct page_info *root_table;
    struct guest_iommu *g_iommu;
    const struct iommu_ops *platform_ops;
};
struct t_rec {
    uint32_t event:28;
    uint32_t extra_u32:3;
    uint32_t cycles_included:1;
    union {
        struct {
            uint32_t cycles_lo, cycles_hi;
            uint32_t extra_u32[7];
        } cycles;
        struct {
            uint32_t extra_u32[7];
        } nocycles;
    } u;
};
struct t_buf {
    uint32_t cons;
    uint32_t prod;
};
struct t_info {
    uint16_t tbuf_size;
    uint16_t mfn_offset[];
};
struct xen_hvm_param {
    domid_t domid;
    uint32_t index;
    uint64_t value;
};
typedef struct xen_hvm_param xen_hvm_param_t;
typedef struct { xen_hvm_param_t *p; } __guest_handle_xen_hvm_param_t; typedef struct { const xen_hvm_param_t *p; } __guest_handle_const_xen_hvm_param_t;
struct xen_hvm_set_pci_intx_level {
    domid_t domid;
    uint8_t domain, bus, device, intx;
    uint8_t level;
};
typedef struct xen_hvm_set_pci_intx_level xen_hvm_set_pci_intx_level_t;
typedef struct { xen_hvm_set_pci_intx_level_t *p; } __guest_handle_xen_hvm_set_pci_intx_level_t; typedef struct { const xen_hvm_set_pci_intx_level_t *p; } __guest_handle_const_xen_hvm_set_pci_intx_level_t;
struct xen_hvm_set_isa_irq_level {
    domid_t domid;
    uint8_t isa_irq;
    uint8_t level;
};
typedef struct xen_hvm_set_isa_irq_level xen_hvm_set_isa_irq_level_t;
typedef struct { xen_hvm_set_isa_irq_level_t *p; } __guest_handle_xen_hvm_set_isa_irq_level_t; typedef struct { const xen_hvm_set_isa_irq_level_t *p; } __guest_handle_const_xen_hvm_set_isa_irq_level_t;
struct xen_hvm_set_pci_link_route {
    domid_t domid;
    uint8_t link;
    uint8_t isa_irq;
};
typedef struct xen_hvm_set_pci_link_route xen_hvm_set_pci_link_route_t;
typedef struct { xen_hvm_set_pci_link_route_t *p; } __guest_handle_xen_hvm_set_pci_link_route_t; typedef struct { const xen_hvm_set_pci_link_route_t *p; } __guest_handle_const_xen_hvm_set_pci_link_route_t;
typedef enum {
    HVMMEM_ram_rw,
    HVMMEM_ram_ro,
    HVMMEM_mmio_dm,
} hvmmem_type_t;
struct xen_hvm_track_dirty_vram {
    domid_t domid;
    uint64_t first_pfn;
    uint64_t nr;
    __guest_handle_uint8 dirty_bitmap;
};
typedef struct xen_hvm_track_dirty_vram xen_hvm_track_dirty_vram_t;
typedef struct { xen_hvm_track_dirty_vram_t *p; } __guest_handle_xen_hvm_track_dirty_vram_t; typedef struct { const xen_hvm_track_dirty_vram_t *p; } __guest_handle_const_xen_hvm_track_dirty_vram_t;
struct xen_hvm_modified_memory {
    domid_t domid;
    uint64_t first_pfn;
    uint64_t nr;
};
typedef struct xen_hvm_modified_memory xen_hvm_modified_memory_t;
typedef struct { xen_hvm_modified_memory_t *p; } __guest_handle_xen_hvm_modified_memory_t; typedef struct { const xen_hvm_modified_memory_t *p; } __guest_handle_const_xen_hvm_modified_memory_t;
struct xen_hvm_set_mem_type {
    domid_t domid;
    uint16_t hvmmem_type;
    uint32_t nr;
    uint64_t first_pfn;
};
typedef struct xen_hvm_set_mem_type xen_hvm_set_mem_type_t;
typedef struct { xen_hvm_set_mem_type_t *p; } __guest_handle_xen_hvm_set_mem_type_t; typedef struct { const xen_hvm_set_mem_type_t *p; } __guest_handle_const_xen_hvm_set_mem_type_t;
struct xen_hvm_pagetable_dying {
    domid_t domid;
    uint16_t pad[3];
    uint64_t gpa;
};
typedef struct xen_hvm_pagetable_dying xen_hvm_pagetable_dying_t;
typedef struct { xen_hvm_pagetable_dying_t *p; } __guest_handle_xen_hvm_pagetable_dying_t; typedef struct { const xen_hvm_pagetable_dying_t *p; } __guest_handle_const_xen_hvm_pagetable_dying_t;
struct xen_hvm_get_time {
    uint64_t now;
};
typedef struct xen_hvm_get_time xen_hvm_get_time_t;
typedef struct { xen_hvm_get_time_t *p; } __guest_handle_xen_hvm_get_time_t; typedef struct { const xen_hvm_get_time_t *p; } __guest_handle_const_xen_hvm_get_time_t;
struct xen_hvm_xentrace {
    uint16_t event, extra_bytes;
    uint8_t extra[7 * sizeof(uint32_t)];
};
typedef struct xen_hvm_xentrace xen_hvm_xentrace_t;
typedef struct { xen_hvm_xentrace_t *p; } __guest_handle_xen_hvm_xentrace_t; typedef struct { const xen_hvm_xentrace_t *p; } __guest_handle_const_xen_hvm_xentrace_t;
typedef enum {
    HVMMEM_access_n,
    HVMMEM_access_r,
    HVMMEM_access_w,
    HVMMEM_access_rw,
    HVMMEM_access_x,
    HVMMEM_access_rx,
    HVMMEM_access_wx,
    HVMMEM_access_rwx,
    HVMMEM_access_rx2rw,
    HVMMEM_access_n2rwx,
    HVMMEM_access_default
} hvmmem_access_t;
struct xen_hvm_set_mem_access {
    domid_t domid;
    uint16_t hvmmem_access;
    uint32_t nr;
    uint64_t first_pfn;
};
typedef struct xen_hvm_set_mem_access xen_hvm_set_mem_access_t;
typedef struct { xen_hvm_set_mem_access_t *p; } __guest_handle_xen_hvm_set_mem_access_t; typedef struct { const xen_hvm_set_mem_access_t *p; } __guest_handle_const_xen_hvm_set_mem_access_t;
struct xen_hvm_get_mem_access {
    domid_t domid;
    uint16_t hvmmem_access;
    uint64_t pfn;
};
typedef struct xen_hvm_get_mem_access xen_hvm_get_mem_access_t;
typedef struct { xen_hvm_get_mem_access_t *p; } __guest_handle_xen_hvm_get_mem_access_t; typedef struct { const xen_hvm_get_mem_access_t *p; } __guest_handle_const_xen_hvm_get_mem_access_t;
struct xen_hvm_inject_trap {
    domid_t domid;
    uint32_t vcpuid;
    uint32_t vector;
    uint32_t type;
    uint32_t error_code;
    uint32_t insn_len;
    uint64_t cr2;
};
typedef struct xen_hvm_inject_trap xen_hvm_inject_trap_t;
typedef struct { xen_hvm_inject_trap_t *p; } __guest_handle_xen_hvm_inject_trap_t; typedef struct { const xen_hvm_inject_trap_t *p; } __guest_handle_const_xen_hvm_inject_trap_t;
struct xen_hvm_get_mem_type {
    domid_t domid;
    uint16_t mem_type;
    uint16_t pad[2];
    uint64_t pfn;
};
typedef struct xen_hvm_get_mem_type xen_hvm_get_mem_type_t;
typedef struct { xen_hvm_get_mem_type_t *p; } __guest_handle_xen_hvm_get_mem_type_t; typedef struct { const xen_hvm_get_mem_type_t *p; } __guest_handle_const_xen_hvm_get_mem_type_t;
struct xen_hvm_inject_msi {
    domid_t domid;
    uint32_t data;
    uint64_t addr;
};
typedef struct xen_hvm_inject_msi xen_hvm_inject_msi_t;
typedef struct { xen_hvm_inject_msi_t *p; } __guest_handle_xen_hvm_inject_msi_t; typedef struct { const xen_hvm_inject_msi_t *p; } __guest_handle_const_xen_hvm_inject_msi_t;
struct hvm_ioreq_page {
    spinlock_t lock;
    struct page_info *page;
    void *va;
};
struct hvm_domain {
    struct hvm_ioreq_page ioreq;
    struct hvm_ioreq_page buf_ioreq;
    struct pl_time pl_time;
    struct hvm_io_handler *io_handler;
    spinlock_t irq_lock;
    struct hvm_irq irq;
    struct hvm_hw_vpic vpic[2];
    struct hvm_vioapic *vioapic;
    struct hvm_hw_stdvga stdvga;
    struct vcpu *i8259_target;
    struct radix_tree_root emuirq_pirq;
    uint64_t *params;
    struct list_head pinned_cacheattr_ranges;
    struct sh_dirty_vram *dirty_vram;
    spinlock_t uc_lock;
    bool_t is_in_uc_mode;
    struct hvm_iommu hvm_iommu;
    struct list_head msixtbl_list;
    spinlock_t msixtbl_list_lock;
    struct viridian_domain viridian;
    bool_t hap_enabled;
    bool_t mem_sharing_enabled;
    bool_t qemu_mapcache_invalidate;
    bool_t is_s3_suspended;
    union {
        struct vmx_domain vmx;
        struct svm_domain svm;
    };
};
struct e820entry {
    uint64_t addr;
    uint64_t size;
    uint32_t type;
} __attribute__((packed));
struct e820map {
    int nr_map;
    struct e820entry map[128];
};
extern int e820_all_mapped(u64 start, u64 end, unsigned type);
extern int reserve_e820_ram(struct e820map *e820, uint64_t s, uint64_t e);
extern int e820_change_range_type(
    struct e820map *e820, uint64_t s, uint64_t e,
    uint32_t orig_type, uint32_t new_type);
extern int e820_add_range(
    struct e820map *, uint64_t s, uint64_t e, uint32_t type);
extern unsigned long init_e820(const char *, struct e820entry *, int *);
extern struct e820map e820;
extern struct e820entry e820map[];
extern int e820nr;
extern unsigned int lowmem_kb, highmem_kb;
struct mcinfo_common {
    uint16_t type;
    uint16_t size;
};
struct mcinfo_global {
    struct mcinfo_common common;
    uint16_t mc_domid;
    uint16_t mc_vcpuid;
    uint32_t mc_socketid;
    uint16_t mc_coreid;
    uint16_t mc_core_threadid;
    uint32_t mc_apicid;
    uint32_t mc_flags;
    uint64_t mc_gstatus;
};
struct mcinfo_bank {
    struct mcinfo_common common;
    uint16_t mc_bank;
    uint16_t mc_domid;
    uint64_t mc_status;
    uint64_t mc_addr;
    uint64_t mc_misc;
    uint64_t mc_ctrl2;
    uint64_t mc_tsc;
};
struct mcinfo_msr {
    uint64_t reg;
    uint64_t value;
};
struct mcinfo_extended {
    struct mcinfo_common common;
    uint32_t mc_msrs;
    struct mcinfo_msr mc_msr[sizeof(void *) * 4];
};
struct page_offline_action
{
    uint64_t mfn;
    uint64_t status;
};
struct cpu_offline_action
{
    uint32_t mc_socketid;
    uint16_t mc_coreid;
    uint16_t mc_core_threadid;
};
struct mcinfo_recovery
{
    struct mcinfo_common common;
    uint16_t mc_bank;
    uint8_t action_flags;
    uint8_t action_types;
    union {
        struct page_offline_action page_retire;
        struct cpu_offline_action cpu_offline;
        uint8_t pad[16];
    } action_info;
};
struct mc_info {
    uint32_t mi_nentries;
    uint32_t flags;
    uint64_t mi_data[(768 - 1) / 8];
};
typedef struct mc_info mc_info_t;
typedef struct { mc_info_t *p; } __guest_handle_mc_info_t; typedef struct { const mc_info_t *p; } __guest_handle_const_mc_info_t;
struct mcinfo_logical_cpu {
    uint32_t mc_cpunr;
    uint32_t mc_chipid;
    uint16_t mc_coreid;
    uint16_t mc_threadid;
    uint32_t mc_apicid;
    uint32_t mc_clusterid;
    uint32_t mc_ncores;
    uint32_t mc_ncores_active;
    uint32_t mc_nthreads;
    int32_t mc_cpuid_level;
    uint32_t mc_family;
    uint32_t mc_vendor;
    uint32_t mc_model;
    uint32_t mc_step;
    char mc_vendorid[16];
    char mc_brandid[64];
    uint32_t mc_cpu_caps[7];
    uint32_t mc_cache_size;
    uint32_t mc_cache_alignment;
    int32_t mc_nmsrvals;
    struct mcinfo_msr mc_msrvalues[8];
};
typedef struct mcinfo_logical_cpu xen_mc_logical_cpu_t;
typedef struct { xen_mc_logical_cpu_t *p; } __guest_handle_xen_mc_logical_cpu_t; typedef struct { const xen_mc_logical_cpu_t *p; } __guest_handle_const_xen_mc_logical_cpu_t;
struct xen_mc_fetch {
    uint32_t flags;
    uint32_t _pad0;
    uint64_t fetch_id;
    __guest_handle_mc_info_t data;
};
typedef struct xen_mc_fetch xen_mc_fetch_t;
typedef struct { xen_mc_fetch_t *p; } __guest_handle_xen_mc_fetch_t; typedef struct { const xen_mc_fetch_t *p; } __guest_handle_const_xen_mc_fetch_t;
struct xen_mc_notifydomain {
    uint16_t mc_domid;
    uint16_t mc_vcpuid;
    uint32_t flags;
};
typedef struct xen_mc_notifydomain xen_mc_notifydomain_t;
typedef struct { xen_mc_notifydomain_t *p; } __guest_handle_xen_mc_notifydomain_t; typedef struct { const xen_mc_notifydomain_t *p; } __guest_handle_const_xen_mc_notifydomain_t;
struct xen_mc_physcpuinfo {
 uint32_t ncpus;
 uint32_t _pad0;
 __guest_handle_xen_mc_logical_cpu_t info;
};
struct xen_mc_msrinject {
 uint32_t mcinj_cpunr;
 uint32_t mcinj_flags;
 uint32_t mcinj_count;
 uint32_t _pad0;
 struct mcinfo_msr mcinj_msr[8];
};
struct xen_mc_mceinject {
 unsigned int mceinj_cpunr;
};
struct xen_mc_inject_v2 {
 uint32_t flags;
 struct xenctl_bitmap cpumap;
};
struct xen_mc {
    uint32_t cmd;
    uint32_t interface_version;
    union {
        struct xen_mc_fetch mc_fetch;
        struct xen_mc_notifydomain mc_notifydomain;
        struct xen_mc_physcpuinfo mc_physcpuinfo;
        struct xen_mc_msrinject mc_msrinject;
        struct xen_mc_mceinject mc_mceinject;
        struct xen_mc_inject_v2 mc_inject_v2;
    } u;
};
typedef struct xen_mc xen_mc_t;
typedef struct { xen_mc_t *p; } __guest_handle_xen_mc_t; typedef struct { const xen_mc_t *p; } __guest_handle_const_xen_mc_t;
struct vmce_bank {
    uint64_t mci_status;
    uint64_t mci_addr;
    uint64_t mci_misc;
    uint64_t mci_ctl2;
};
struct vmce {
    uint64_t mcg_cap;
    uint64_t mcg_status;
    spinlock_t lock;
    struct vmce_bank bank[2];
};
extern void vmce_init_vcpu(struct vcpu *);
extern int vmce_restore_vcpu(struct vcpu *, const struct hvm_vmce_vcpu *);
extern int vmce_wrmsr(uint32_t msr, uint64_t val);
extern int vmce_rdmsr(uint32_t msr, uint64_t *val);
extern unsigned int nr_mce_banks;
struct vcpu_runstate_info {
    int state;
    uint64_t state_entry_time;
    uint64_t time[4];
};
typedef struct vcpu_runstate_info vcpu_runstate_info_t;
typedef struct { vcpu_runstate_info_t *p; } __guest_handle_vcpu_runstate_info_t; typedef struct { const vcpu_runstate_info_t *p; } __guest_handle_const_vcpu_runstate_info_t;
struct vcpu_register_runstate_memory_area {
    union {
        __guest_handle_vcpu_runstate_info_t h;
        struct vcpu_runstate_info *v;
        uint64_t p;
    } addr;
};
typedef struct vcpu_register_runstate_memory_area vcpu_register_runstate_memory_area_t;
typedef struct { vcpu_register_runstate_memory_area_t *p; } __guest_handle_vcpu_register_runstate_memory_area_t; typedef struct { const vcpu_register_runstate_memory_area_t *p; } __guest_handle_const_vcpu_register_runstate_memory_area_t;
struct vcpu_set_periodic_timer {
    uint64_t period_ns;
};
typedef struct vcpu_set_periodic_timer vcpu_set_periodic_timer_t;
typedef struct { vcpu_set_periodic_timer_t *p; } __guest_handle_vcpu_set_periodic_timer_t; typedef struct { const vcpu_set_periodic_timer_t *p; } __guest_handle_const_vcpu_set_periodic_timer_t;
struct vcpu_set_singleshot_timer {
    uint64_t timeout_abs_ns;
    uint32_t flags;
};
typedef struct vcpu_set_singleshot_timer vcpu_set_singleshot_timer_t;
typedef struct { vcpu_set_singleshot_timer_t *p; } __guest_handle_vcpu_set_singleshot_timer_t; typedef struct { const vcpu_set_singleshot_timer_t *p; } __guest_handle_const_vcpu_set_singleshot_timer_t;
struct vcpu_register_vcpu_info {
    uint64_t mfn;
    uint32_t offset;
    uint32_t rsvd;
};
typedef struct vcpu_register_vcpu_info vcpu_register_vcpu_info_t;
typedef struct { vcpu_register_vcpu_info_t *p; } __guest_handle_vcpu_register_vcpu_info_t; typedef struct { const vcpu_register_vcpu_info_t *p; } __guest_handle_const_vcpu_register_vcpu_info_t;
struct vcpu_get_physid {
    uint64_t phys_id;
};
typedef struct vcpu_get_physid vcpu_get_physid_t;
typedef struct { vcpu_get_physid_t *p; } __guest_handle_vcpu_get_physid_t; typedef struct { const vcpu_get_physid_t *p; } __guest_handle_const_vcpu_get_physid_t;
typedef struct { vcpu_time_info_t *p; } __guest_handle_vcpu_time_info_t; typedef struct { const vcpu_time_info_t *p; } __guest_handle_const_vcpu_time_info_t;
struct vcpu_register_time_memory_area {
    union {
        __guest_handle_vcpu_time_info_t h;
        struct vcpu_time_info *v;
        uint64_t p;
    } addr;
};
typedef struct vcpu_register_time_memory_area vcpu_register_time_memory_area_t;
typedef struct { vcpu_register_time_memory_area_t *p; } __guest_handle_vcpu_register_time_memory_area_t; typedef struct { const vcpu_register_time_memory_area_t *p; } __guest_handle_const_vcpu_register_time_memory_area_t;
struct trap_bounce {
    uint32_t error_code;
    uint8_t flags;
    uint16_t cs;
    unsigned long eip;
};
struct mapcache_vcpu {
    unsigned int shadow_epoch;
    struct vcpu_maphash_entry {
        unsigned long mfn;
        uint32_t idx;
        uint32_t refcnt;
    } hash[8];
};
struct mapcache_domain {
    unsigned int entries;
    unsigned int cursor;
    spinlock_t lock;
    unsigned int epoch;
    u32 tlbflush_timestamp;
    unsigned long *inuse;
    unsigned long *garbage;
};
int mapcache_domain_init(struct domain *);
int mapcache_vcpu_init(struct vcpu *);
void mapcache_override_current(struct vcpu *);
void toggle_guest_mode(struct vcpu *);
void hypercall_page_initialise(struct domain *d, void *);
struct shadow_domain {
    unsigned int opt_flags;
    struct page_list_head pinned_shadows;
    struct page_list_head freelist;
    unsigned int total_pages;
    unsigned int free_pages;
    unsigned int p2m_pages;
    pagetable_t unpaged_pagetable;
    atomic_t gtable_dirty_version;
    struct page_info **hash_table;
    bool_t hash_walking;
    bool_t has_fast_mmio_entries;
    bool_t oos_active;
    bool_t oos_off;
    bool_t pagetable_dying_op;
};
struct shadow_vcpu {
    l3_pgentry_t l3table[4] __attribute__((__aligned__(32)));
    l3_pgentry_t gl3e[4] __attribute__((__aligned__(32)));
    void *guest_vtable;
    unsigned long last_emulated_mfn_for_unshadow;
    unsigned long last_writeable_pte_smfn;
    unsigned long last_emulated_frame;
    unsigned long last_emulated_mfn;
    mfn_t oos[3];
    mfn_t oos_snapshot[3];
    struct oos_fixup {
        int next;
        mfn_t smfn[2];
        unsigned long off[2];
    } oos_fixup[3];
    bool_t pagetable_dying;
};
struct hap_domain {
    struct page_list_head freelist;
    unsigned int total_pages;
    unsigned int free_pages;
    unsigned int p2m_pages;
};
struct log_dirty_domain {
    mfn_t top;
    unsigned int allocs;
    unsigned int failed_allocs;
    unsigned int fault_count;
    unsigned int dirty_count;
    int (*enable_log_dirty )(struct domain *d, bool_t log_global);
    int (*disable_log_dirty )(struct domain *d);
    void (*clean_dirty_bitmap )(struct domain *d);
};
struct paging_domain {
    mm_lock_t lock;
    u32 mode;
    struct shadow_domain shadow;
    struct hap_domain hap;
    struct log_dirty_domain log_dirty;
    struct page_info * (*alloc_page)(struct domain *d);
    void (*free_page)(struct domain *d, struct page_info *pg);
    bool_t p2m_alloc_failed;
};
struct paging_vcpu {
    const struct paging_mode *mode;
    const struct paging_mode *nestedmode;
    unsigned int last_write_was_pt:1;
    unsigned int last_write_emul_ok:1;
    struct shadow_vtlb *vtlb;
    spinlock_t vtlb_lock;
    struct shadow_vcpu shadow;
};
typedef xen_domctl_cpuid_t cpuid_input_t;
struct p2m_domain;
struct time_scale {
    int shift;
    u32 mul_frac;
};
struct pv_domain
{
    l1_pgentry_t **gdt_ldt_l1tab;
    unsigned long *pirq_eoi_map;
    unsigned long pirq_eoi_map_mfn;
    bool_t auto_unmask;
    struct mapcache_domain mapcache;
};
struct arch_domain
{
    struct page_info *perdomain_l3_pg;
    unsigned int hv_compat_vstart;
    bool_t s3_integrity;
    struct rangeset *ioport_caps;
    uint32_t pci_cf8;
    uint8_t cmos_idx;
    struct list_head pdev_list;
    union {
        struct pv_domain pv_domain;
        struct hvm_domain hvm_domain;
    };
    struct paging_domain paging;
    struct p2m_domain *p2m;
    int page_alloc_unlock_level;
    struct p2m_domain *nested_p2m[10];
    mm_lock_t nested_p2m_lock;
    struct radix_tree_root irq_pirq;
    unsigned int physaddr_bitsize;
    bool_t is_32bit_pv;
    bool_t has_32bit_shinfo;
    bool_t suppress_spurious_page_faults;
    enum {
        RELMEM_not_started,
        RELMEM_shared,
        RELMEM_xen,
        RELMEM_l4,
        RELMEM_l3,
        RELMEM_l2,
        RELMEM_done,
    } relmem;
    struct page_list_head relmem_list;
    cpuid_input_t *cpuids;
    struct PITState vpit;
    int tsc_mode;
    bool_t vtsc;
    s_time_t vtsc_last;
    spinlock_t vtsc_lock;
    uint64_t vtsc_offset;
    uint32_t tsc_khz;
    struct time_scale vtsc_to_ns;
    struct time_scale ns_to_vtsc;
    uint32_t incarnation;
    uint64_t vtsc_kerncount;
    uint64_t vtsc_usercount;
    spinlock_t e820_lock;
    struct e820entry *e820;
    unsigned int nr_e820;
} __attribute__((__aligned__((1 << (7)))));
struct pv_vcpu
{
    struct mapcache_vcpu mapcache;
    struct trap_info *trap_ctxt;
    unsigned long gdt_frames[14];
    unsigned long ldt_base;
    unsigned int gdt_ents, ldt_ents;
    unsigned long kernel_ss, kernel_sp;
    unsigned long ctrlreg[8];
    unsigned long event_callback_eip;
    unsigned long failsafe_callback_eip;
    union {
        unsigned long syscall_callback_eip;
        struct {
            unsigned int event_callback_cs;
            unsigned int failsafe_callback_cs;
        };
    };
    unsigned long vm_assist;
    unsigned long syscall32_callback_eip;
    unsigned long sysenter_callback_eip;
    unsigned short syscall32_callback_cs;
    unsigned short sysenter_callback_cs;
    bool_t syscall32_disables_events;
    bool_t sysenter_disables_events;
    unsigned long fs_base;
    unsigned long gs_base_kernel;
    unsigned long gs_base_user;
    struct trap_bounce trap_bounce;
    struct trap_bounce int80_bounce;
    __guest_handle_uint8 iobmp;
    unsigned int iobmp_limit;
    unsigned int iopl;
    unsigned long shadow_ldt_mapcnt;
    spinlock_t shadow_ldt_lock;
    bool_t need_update_runstate_area;
    struct vcpu_time_info pending_system_time;
};
struct arch_vcpu
{
    void *fpu_ctxt;
    unsigned long vgc_flags;
    struct cpu_user_regs user_regs;
    unsigned long debugreg[8];
    unsigned long flags;
    void (*schedule_tail) (struct vcpu *);
    void (*ctxt_switch_from) (struct vcpu *);
    void (*ctxt_switch_to) (struct vcpu *);
    union {
        struct pv_vcpu pv_vcpu;
        struct hvm_vcpu hvm_vcpu;
    };
    pagetable_t guest_table_user;
    pagetable_t guest_table;
    struct page_info *old_guest_table;
    pagetable_t shadow_table[4];
    pagetable_t monitor_table;
    unsigned long cr3;
    struct xsave_struct *xsave_area;
    uint64_t xcr0;
    uint64_t xcr0_accum;
    bool_t nonlazy_xstate_used;
    struct vmce vmce;
    struct paging_vcpu paging;
    uint32_t gdbsx_vcpu_event;
    __guest_handle_vcpu_time_info_t time_info_guest;
} __attribute__((__aligned__((1 << (7)))));
bool_t update_runstate_area(const struct vcpu *);
bool_t update_secondary_system_time(const struct vcpu *,
                                    struct vcpu_time_info *);
void vcpu_show_execution_state(struct vcpu *);
void vcpu_show_registers(const struct vcpu *);
unsigned long pv_guest_cr4_fixup(const struct vcpu *, unsigned long guest_cr4);
void domain_cpuid(struct domain *d,
                  unsigned int input,
                  unsigned int sub_input,
                  unsigned int *eax,
                  unsigned int *ebx,
                  unsigned int *ecx,
                  unsigned int *edx);
typedef union {
    struct vcpu_guest_context *nat;
    struct compat_vcpu_guest_context *cmp;
} vcpu_guest_context_u __attribute__((__transparent_union__));
struct vcpu *alloc_vcpu(
    struct domain *d, unsigned int vcpu_id, unsigned int cpu_id);
struct vcpu *alloc_dom0_vcpu0(void);
int vcpu_reset(struct vcpu *);
struct xen_domctl_getdomaininfo;
void getdomaininfo(struct domain *d, struct xen_domctl_getdomaininfo *info);
struct domain *alloc_domain_struct(void);
void free_domain_struct(struct domain *d);
struct vcpu *alloc_vcpu_struct(void);
void free_vcpu_struct(struct vcpu *v);
struct vcpu_guest_context *alloc_vcpu_guest_context(void);
void free_vcpu_guest_context(struct vcpu_guest_context *);
struct pirq *alloc_pirq_struct(struct domain *);
void free_pirq_struct(void *);
int vcpu_initialise(struct vcpu *v);
void vcpu_destroy(struct vcpu *v);
int map_vcpu_info(struct vcpu *v, unsigned long gfn, unsigned offset);
void unmap_vcpu_info(struct vcpu *v);
int arch_domain_create(struct domain *d, unsigned int domcr_flags);
void arch_domain_destroy(struct domain *d);
int arch_set_info_guest(struct vcpu *, vcpu_guest_context_u);
void arch_get_info_guest(struct vcpu *, vcpu_guest_context_u);
int domain_relinquish_resources(struct domain *d);
void dump_pageframe_info(struct domain *d);
void arch_dump_vcpu_info(struct vcpu *v);
void arch_dump_domain_info(struct domain *d);
int arch_vcpu_reset(struct vcpu *);
extern spinlock_t vcpu_alloc_lock;
bool_t domctl_lock_acquire(void);
void domctl_lock_release(void);
int continue_hypercall_on_cpu(
    unsigned int cpu, long (*func)(void *data), void *data);
extern unsigned int xen_processor_pmbits;
extern bool_t opt_dom0_vcpus_pin;
extern int srat_rev;
extern unsigned char cpu_to_node[];
extern cpumask_t node_to_cpumask[];
struct node {
 u64 start,end;
};
extern int compute_hash_shift(struct node *nodes, int numnodes,
         int *nodeids);
extern int pxm_to_node(int nid);
extern void numa_add_cpu(int cpu);
extern void numa_init_array(void);
extern int numa_off;
extern int srat_disabled(void);
extern void numa_set_node(int cpu, int node);
extern int setup_node(int pxm);
extern void srat_detect_node(int cpu);
extern void setup_node_bootmem(int nodeid, u64 start, u64 end);
extern unsigned char apicid_to_node[];
extern void init_cpu_to_node(void);
static __inline__ void clear_node_cpumask(int cpu)
{
 cpumask_clear_cpu(cpu, &node_to_cpumask[(cpu_to_node[cpu])]);
}
extern int memnode_shift;
extern unsigned long memnodemapsize;
extern u8 *memnodemap;
struct node_data {
    unsigned long node_start_pfn;
    unsigned long node_spanned_pages;
    unsigned int node_id;
};
extern struct node_data node_data[];
static __inline__ __attribute__((pure)) int phys_to_nid(paddr_t addr)
{
 unsigned nid;
 ;
 nid = memnodemap[pfn_to_pdx(((unsigned long)((addr) >> 12))) >> memnode_shift];
 ;
 return nid;
}
extern int valid_numa_range(u64 start, u64 end, int node);
void srat_parse_regions(u64 addr);
extern int __node_distance(int a, int b);
typedef struct { unsigned long bits[((((1 << 6))+((1 << 3) << 3)-1)/((1 << 3) << 3))]; } nodemask_t;
extern nodemask_t _unused_nodemask_arg_;
static __inline__ void __node_set(int node, volatile nodemask_t *dstp)
{
 ({ if ( (sizeof(*(dstp->bits)) < 4) ) __bitop_bad_size(); set_bit(node, dstp->bits); });
}
static __inline__ void __node_clear(int node, volatile nodemask_t *dstp)
{
 ({ if ( (sizeof(*(dstp->bits)) < 4) ) __bitop_bad_size(); clear_bit(node, dstp->bits); });
}
static __inline__ void __nodes_setall(nodemask_t *dstp, int nbits)
{
 bitmap_fill(dstp->bits, nbits);
}
static __inline__ void __nodes_clear(nodemask_t *dstp, int nbits)
{
 bitmap_zero(dstp->bits, nbits);
}
static __inline__ int __node_test_and_set(int node, nodemask_t *addr)
{
 return ({ if ( (sizeof(*(addr->bits)) < 4) ) __bitop_bad_size(); test_and_set_bit(node, addr->bits); });
}
static __inline__ void __nodes_and(nodemask_t *dstp, const nodemask_t *src1p,
     const nodemask_t *src2p, int nbits)
{
 bitmap_and(dstp->bits, src1p->bits, src2p->bits, nbits);
}
static __inline__ void __nodes_or(nodemask_t *dstp, const nodemask_t *src1p,
     const nodemask_t *src2p, int nbits)
{
 bitmap_or(dstp->bits, src1p->bits, src2p->bits, nbits);
}
static __inline__ void __nodes_xor(nodemask_t *dstp, const nodemask_t *src1p,
     const nodemask_t *src2p, int nbits)
{
 bitmap_xor(dstp->bits, src1p->bits, src2p->bits, nbits);
}
static __inline__ void __nodes_andnot(nodemask_t *dstp, const nodemask_t *src1p,
     const nodemask_t *src2p, int nbits)
{
 bitmap_andnot(dstp->bits, src1p->bits, src2p->bits, nbits);
}
static __inline__ void __nodes_complement(nodemask_t *dstp,
     const nodemask_t *srcp, int nbits)
{
 bitmap_complement(dstp->bits, srcp->bits, nbits);
}
static __inline__ int __nodes_equal(const nodemask_t *src1p,
     const nodemask_t *src2p, int nbits)
{
 return bitmap_equal(src1p->bits, src2p->bits, nbits);
}
static __inline__ int __nodes_intersects(const nodemask_t *src1p,
     const nodemask_t *src2p, int nbits)
{
 return bitmap_intersects(src1p->bits, src2p->bits, nbits);
}
static __inline__ int __nodes_subset(const nodemask_t *src1p,
     const nodemask_t *src2p, int nbits)
{
 return bitmap_subset(src1p->bits, src2p->bits, nbits);
}
static __inline__ int __nodes_empty(const nodemask_t *srcp, int nbits)
{
 return bitmap_empty(srcp->bits, nbits);
}
static __inline__ int __nodes_full(const nodemask_t *srcp, int nbits)
{
 return bitmap_full(srcp->bits, nbits);
}
static __inline__ int __nodes_weight(const nodemask_t *srcp, int nbits)
{
 return bitmap_weight(srcp->bits, nbits);
}
static __inline__ void __nodes_shift_right(nodemask_t *dstp,
     const nodemask_t *srcp, int n, int nbits)
{
 bitmap_shift_right(dstp->bits, srcp->bits, n, nbits);
}
static __inline__ void __nodes_shift_left(nodemask_t *dstp,
     const nodemask_t *srcp, int n, int nbits)
{
 bitmap_shift_left(dstp->bits, srcp->bits, n, nbits);
}
static __inline__ int __first_node(const nodemask_t *srcp, int nbits)
{
 return ({ int __x = (nbits); int __y = (({ unsigned int r__ = (nbits); unsigned int o__ = (0); switch ( -!__builtin_constant_p(nbits) | r__ ) { case 0: (void)(srcp->bits); break; case 1 ... ((1 << 3) << 3): r__ = o__ + __scanbit(*(const unsigned long *)(srcp->bits) >> o__, r__); break; default: if ( __builtin_constant_p(0) && !o__ ) r__ = __find_first_bit(srcp->bits, r__); else r__ = __find_next_bit(srcp->bits, r__, o__); break; } r__; })); __x < __y ? __x: __y; });
}
static __inline__ int __next_node(int n, const nodemask_t *srcp, int nbits)
{
 return ({ int __x = (nbits); int __y = (({ unsigned int r__ = (nbits); unsigned int o__ = (n+1); switch ( -!__builtin_constant_p(nbits) | r__ ) { case 0: (void)(srcp->bits); break; case 1 ... ((1 << 3) << 3): r__ = o__ + __scanbit(*(const unsigned long *)(srcp->bits) >> o__, r__); break; default: if ( __builtin_constant_p(n+1) && !o__ ) r__ = __find_first_bit(srcp->bits, r__); else r__ = __find_next_bit(srcp->bits, r__, o__); break; } r__; })); __x < __y ? __x: __y; });
}
static __inline__ int __last_node(const nodemask_t *srcp, int nbits)
{
 int node, pnode = nbits;
 for (node = __first_node(srcp, nbits);
      node < nbits;
      node = __next_node(node, srcp, nbits))
  pnode = node;
 return pnode;
}
static __inline__ int __first_unset_node(const nodemask_t *maskp)
{
 return ({ int __x = ((1 << 6)); int __y = (({ unsigned int r__ = ((1 << 6)); unsigned int o__ = (0); switch ( -!__builtin_constant_p((1 << 6)) | r__ ) { case 0: (void)(maskp->bits); break; case 1 ... ((1 << 3) << 3): r__ = o__ + __scanbit(~*(const unsigned long *)(maskp->bits) >> o__, r__); break; default: if ( __builtin_constant_p(0) && !o__ ) r__ = __find_first_zero_bit(maskp->bits, r__); else r__ = __find_next_zero_bit(maskp->bits, r__, o__); break; } r__; })); __x < __y ? __x: __y; });
}
static __inline__ int __cycle_node(int n, const nodemask_t *maskp, int nbits)
{
    int nxt = __next_node(n, maskp, nbits);
    if (nxt == nbits)
        nxt = __first_node(maskp, nbits);
    return nxt;
}
static __inline__ int __nodelist_scnprintf(char *buf, int len,
     const nodemask_t *srcp, int nbits)
{
 return bitmap_scnlistprintf(buf, len, srcp->bits, nbits);
}
extern nodemask_t node_online_map;
struct mc_state {
    unsigned long flags;
    union {
        struct multicall_entry call;
        struct compat_multicall_entry compat_call;
    };
};
struct waitqueue_head {
    spinlock_t lock;
    struct list_head list;
};
void init_waitqueue_head(struct waitqueue_head *wq);
void destroy_waitqueue_head(struct waitqueue_head *wq);
void wake_up_nr(struct waitqueue_head *wq, unsigned int nr);
void wake_up_one(struct waitqueue_head *wq);
void wake_up_all(struct waitqueue_head *wq);
int init_waitqueue_vcpu(struct vcpu *v);
void destroy_waitqueue_vcpu(struct vcpu *v);
void prepare_to_wait(struct waitqueue_head *wq);
void wait(void);
void finish_wait(struct waitqueue_head *wq);
void check_wakeup_from_wait(void);
struct xen_sysctl_readconsole {
    uint8_t clear;
    uint8_t incremental;
    uint8_t pad0, pad1;
    uint32_t index;
    __guest_handle_char buffer;
    uint32_t count;
};
typedef struct xen_sysctl_readconsole xen_sysctl_readconsole_t;
typedef struct { xen_sysctl_readconsole_t *p; } __guest_handle_xen_sysctl_readconsole_t; typedef struct { const xen_sysctl_readconsole_t *p; } __guest_handle_const_xen_sysctl_readconsole_t;
struct xen_sysctl_tbuf_op {
    uint32_t cmd;
    struct xenctl_bitmap cpu_mask;
    uint32_t evt_mask;
    uint64_t buffer_mfn;
    uint32_t size;
};
typedef struct xen_sysctl_tbuf_op xen_sysctl_tbuf_op_t;
typedef struct { xen_sysctl_tbuf_op_t *p; } __guest_handle_xen_sysctl_tbuf_op_t; typedef struct { const xen_sysctl_tbuf_op_t *p; } __guest_handle_const_xen_sysctl_tbuf_op_t;
struct xen_sysctl_physinfo {
    uint32_t threads_per_core;
    uint32_t cores_per_socket;
    uint32_t nr_cpus;
    uint32_t max_cpu_id;
    uint32_t nr_nodes;
    uint32_t max_node_id;
    uint32_t cpu_khz;
    uint64_t total_pages;
    uint64_t free_pages;
    uint64_t scrub_pages;
    uint64_t outstanding_pages;
    uint32_t hw_cap[8];
    uint32_t capabilities;
};
typedef struct xen_sysctl_physinfo xen_sysctl_physinfo_t;
typedef struct { xen_sysctl_physinfo_t *p; } __guest_handle_xen_sysctl_physinfo_t; typedef struct { const xen_sysctl_physinfo_t *p; } __guest_handle_const_xen_sysctl_physinfo_t;
struct xen_sysctl_sched_id {
    uint32_t sched_id;
};
typedef struct xen_sysctl_sched_id xen_sysctl_sched_id_t;
typedef struct { xen_sysctl_sched_id_t *p; } __guest_handle_xen_sysctl_sched_id_t; typedef struct { const xen_sysctl_sched_id_t *p; } __guest_handle_const_xen_sysctl_sched_id_t;
struct xen_sysctl_perfc_desc {
    char name[80];
    uint32_t nr_vals;
};
typedef struct xen_sysctl_perfc_desc xen_sysctl_perfc_desc_t;
typedef struct { xen_sysctl_perfc_desc_t *p; } __guest_handle_xen_sysctl_perfc_desc_t; typedef struct { const xen_sysctl_perfc_desc_t *p; } __guest_handle_const_xen_sysctl_perfc_desc_t;
typedef uint32_t xen_sysctl_perfc_val_t;
typedef struct { xen_sysctl_perfc_val_t *p; } __guest_handle_xen_sysctl_perfc_val_t; typedef struct { const xen_sysctl_perfc_val_t *p; } __guest_handle_const_xen_sysctl_perfc_val_t;
struct xen_sysctl_perfc_op {
    uint32_t cmd;
    uint32_t nr_counters;
    uint32_t nr_vals;
    __guest_handle_xen_sysctl_perfc_desc_t desc;
    __guest_handle_xen_sysctl_perfc_val_t val;
};
typedef struct xen_sysctl_perfc_op xen_sysctl_perfc_op_t;
typedef struct { xen_sysctl_perfc_op_t *p; } __guest_handle_xen_sysctl_perfc_op_t; typedef struct { const xen_sysctl_perfc_op_t *p; } __guest_handle_const_xen_sysctl_perfc_op_t;
struct xen_sysctl_getdomaininfolist {
    domid_t first_domain;
    uint32_t max_domains;
    __guest_handle_xen_domctl_getdomaininfo_t buffer;
    uint32_t num_domains;
};
typedef struct xen_sysctl_getdomaininfolist xen_sysctl_getdomaininfolist_t;
typedef struct { xen_sysctl_getdomaininfolist_t *p; } __guest_handle_xen_sysctl_getdomaininfolist_t; typedef struct { const xen_sysctl_getdomaininfolist_t *p; } __guest_handle_const_xen_sysctl_getdomaininfolist_t;
struct xen_sysctl_debug_keys {
    __guest_handle_char keys;
    uint32_t nr_keys;
};
typedef struct xen_sysctl_debug_keys xen_sysctl_debug_keys_t;
typedef struct { xen_sysctl_debug_keys_t *p; } __guest_handle_xen_sysctl_debug_keys_t; typedef struct { const xen_sysctl_debug_keys_t *p; } __guest_handle_const_xen_sysctl_debug_keys_t;
struct xen_sysctl_cpuinfo {
    uint64_t idletime;
};
typedef struct xen_sysctl_cpuinfo xen_sysctl_cpuinfo_t;
typedef struct { xen_sysctl_cpuinfo_t *p; } __guest_handle_xen_sysctl_cpuinfo_t; typedef struct { const xen_sysctl_cpuinfo_t *p; } __guest_handle_const_xen_sysctl_cpuinfo_t;
struct xen_sysctl_getcpuinfo {
    uint32_t max_cpus;
    __guest_handle_xen_sysctl_cpuinfo_t info;
    uint32_t nr_cpus;
};
typedef struct xen_sysctl_getcpuinfo xen_sysctl_getcpuinfo_t;
typedef struct { xen_sysctl_getcpuinfo_t *p; } __guest_handle_xen_sysctl_getcpuinfo_t; typedef struct { const xen_sysctl_getcpuinfo_t *p; } __guest_handle_const_xen_sysctl_getcpuinfo_t;
struct xen_sysctl_availheap {
    uint32_t min_bitwidth;
    uint32_t max_bitwidth;
    int32_t node;
    uint64_t avail_bytes;
};
typedef struct xen_sysctl_availheap xen_sysctl_availheap_t;
typedef struct { xen_sysctl_availheap_t *p; } __guest_handle_xen_sysctl_availheap_t; typedef struct { const xen_sysctl_availheap_t *p; } __guest_handle_const_xen_sysctl_availheap_t;
struct pm_px_val {
    uint64_t freq;
    uint64_t residency;
    uint64_t count;
};
typedef struct pm_px_val pm_px_val_t;
typedef struct { pm_px_val_t *p; } __guest_handle_pm_px_val_t; typedef struct { const pm_px_val_t *p; } __guest_handle_const_pm_px_val_t;
struct pm_px_stat {
    uint8_t total;
    uint8_t usable;
    uint8_t last;
    uint8_t cur;
    __guest_handle_uint64 trans_pt;
    __guest_handle_pm_px_val_t pt;
};
typedef struct pm_px_stat pm_px_stat_t;
typedef struct { pm_px_stat_t *p; } __guest_handle_pm_px_stat_t; typedef struct { const pm_px_stat_t *p; } __guest_handle_const_pm_px_stat_t;
struct pm_cx_stat {
    uint32_t nr;
    uint32_t last;
    uint64_t idle_time;
    __guest_handle_uint64 triggers;
    __guest_handle_uint64 residencies;
    uint64_t pc2;
    uint64_t pc3;
    uint64_t pc6;
    uint64_t pc7;
    uint64_t cc3;
    uint64_t cc6;
    uint64_t cc7;
};
struct xen_sysctl_get_pmstat {
    uint32_t type;
    uint32_t cpuid;
    union {
        struct pm_px_stat getpx;
        struct pm_cx_stat getcx;
    } u;
};
typedef struct xen_sysctl_get_pmstat xen_sysctl_get_pmstat_t;
typedef struct { xen_sysctl_get_pmstat_t *p; } __guest_handle_xen_sysctl_get_pmstat_t; typedef struct { const xen_sysctl_get_pmstat_t *p; } __guest_handle_const_xen_sysctl_get_pmstat_t;
struct xen_sysctl_cpu_hotplug {
    uint32_t cpu;
    uint32_t op;
};
typedef struct xen_sysctl_cpu_hotplug xen_sysctl_cpu_hotplug_t;
typedef struct { xen_sysctl_cpu_hotplug_t *p; } __guest_handle_xen_sysctl_cpu_hotplug_t; typedef struct { const xen_sysctl_cpu_hotplug_t *p; } __guest_handle_const_xen_sysctl_cpu_hotplug_t;
struct xen_userspace {
    uint32_t scaling_setspeed;
};
typedef struct xen_userspace xen_userspace_t;
struct xen_ondemand {
    uint32_t sampling_rate_max;
    uint32_t sampling_rate_min;
    uint32_t sampling_rate;
    uint32_t up_threshold;
};
typedef struct xen_ondemand xen_ondemand_t;
struct xen_get_cpufreq_para {
    uint32_t cpu_num;
    uint32_t freq_num;
    uint32_t gov_num;
    __guest_handle_uint32 affected_cpus;
    __guest_handle_uint32 scaling_available_frequencies;
    __guest_handle_char scaling_available_governors;
    char scaling_driver[16];
    uint32_t cpuinfo_cur_freq;
    uint32_t cpuinfo_max_freq;
    uint32_t cpuinfo_min_freq;
    uint32_t scaling_cur_freq;
    char scaling_governor[16];
    uint32_t scaling_max_freq;
    uint32_t scaling_min_freq;
    union {
        struct xen_userspace userspace;
        struct xen_ondemand ondemand;
    } u;
    int32_t turbo_enabled;
};
struct xen_set_cpufreq_gov {
    char scaling_governor[16];
};
struct xen_set_cpufreq_para {
    uint32_t ctrl_type;
    uint32_t ctrl_value;
};
struct xen_sysctl_pm_op {
    uint32_t cmd;
    uint32_t cpuid;
    union {
        struct xen_get_cpufreq_para get_para;
        struct xen_set_cpufreq_gov set_gov;
        struct xen_set_cpufreq_para set_para;
        uint64_t get_avgfreq;
        uint32_t set_sched_opt_smt;
        uint32_t get_max_cstate;
        uint32_t set_max_cstate;
        uint32_t get_vcpu_migration_delay;
        uint32_t set_vcpu_migration_delay;
    } u;
};
struct xen_sysctl_page_offline_op {
    uint32_t cmd;
    uint32_t start;
    uint32_t end;
    __guest_handle_uint32 status;
};
struct xen_sysctl_lockprof_data {
    char name[40];
    int32_t type;
    int32_t idx;
    uint64_t lock_cnt;
    uint64_t block_cnt;
    uint64_t lock_time;
    uint64_t block_time;
};
typedef struct xen_sysctl_lockprof_data xen_sysctl_lockprof_data_t;
typedef struct { xen_sysctl_lockprof_data_t *p; } __guest_handle_xen_sysctl_lockprof_data_t; typedef struct { const xen_sysctl_lockprof_data_t *p; } __guest_handle_const_xen_sysctl_lockprof_data_t;
struct xen_sysctl_lockprof_op {
    uint32_t cmd;
    uint32_t max_elem;
    uint32_t nr_elem;
    uint64_t time;
    __guest_handle_xen_sysctl_lockprof_data_t data;
};
typedef struct xen_sysctl_lockprof_op xen_sysctl_lockprof_op_t;
typedef struct { xen_sysctl_lockprof_op_t *p; } __guest_handle_xen_sysctl_lockprof_op_t; typedef struct { const xen_sysctl_lockprof_op_t *p; } __guest_handle_const_xen_sysctl_lockprof_op_t;
struct xen_sysctl_topologyinfo {
    uint32_t max_cpu_index;
    __guest_handle_uint32 cpu_to_core;
    __guest_handle_uint32 cpu_to_socket;
    __guest_handle_uint32 cpu_to_node;
};
typedef struct xen_sysctl_topologyinfo xen_sysctl_topologyinfo_t;
typedef struct { xen_sysctl_topologyinfo_t *p; } __guest_handle_xen_sysctl_topologyinfo_t; typedef struct { const xen_sysctl_topologyinfo_t *p; } __guest_handle_const_xen_sysctl_topologyinfo_t;
struct xen_sysctl_numainfo {
    uint32_t max_node_index;
    __guest_handle_uint64 node_to_memsize;
    __guest_handle_uint64 node_to_memfree;
    __guest_handle_uint32 node_to_node_distance;
};
typedef struct xen_sysctl_numainfo xen_sysctl_numainfo_t;
typedef struct { xen_sysctl_numainfo_t *p; } __guest_handle_xen_sysctl_numainfo_t; typedef struct { const xen_sysctl_numainfo_t *p; } __guest_handle_const_xen_sysctl_numainfo_t;
struct xen_sysctl_cpupool_op {
    uint32_t op;
    uint32_t cpupool_id;
    uint32_t sched_id;
    uint32_t domid;
    uint32_t cpu;
    uint32_t n_dom;
    struct xenctl_bitmap cpumap;
};
typedef struct xen_sysctl_cpupool_op xen_sysctl_cpupool_op_t;
typedef struct { xen_sysctl_cpupool_op_t *p; } __guest_handle_xen_sysctl_cpupool_op_t; typedef struct { const xen_sysctl_cpupool_op_t *p; } __guest_handle_const_xen_sysctl_cpupool_op_t;
struct xen_sysctl_arinc653_schedule {
    uint64_t major_frame;
    uint8_t num_sched_entries;
    struct {
        xen_domain_handle_t dom_handle;
        unsigned int vcpu_id;
        uint64_t runtime;
    } sched_entries[64];
};
typedef struct xen_sysctl_arinc653_schedule xen_sysctl_arinc653_schedule_t;
typedef struct { xen_sysctl_arinc653_schedule_t *p; } __guest_handle_xen_sysctl_arinc653_schedule_t; typedef struct { const xen_sysctl_arinc653_schedule_t *p; } __guest_handle_const_xen_sysctl_arinc653_schedule_t;
struct xen_sysctl_credit_schedule {
    unsigned tslice_ms;
    unsigned ratelimit_us;
};
typedef struct xen_sysctl_credit_schedule xen_sysctl_credit_schedule_t;
typedef struct { xen_sysctl_credit_schedule_t *p; } __guest_handle_xen_sysctl_credit_schedule_t; typedef struct { const xen_sysctl_credit_schedule_t *p; } __guest_handle_const_xen_sysctl_credit_schedule_t;
struct xen_sysctl_scheduler_op {
    uint32_t cpupool_id;
    uint32_t sched_id;
    uint32_t cmd;
    union {
        struct xen_sysctl_sched_arinc653 {
            __guest_handle_xen_sysctl_arinc653_schedule_t schedule;
        } sched_arinc653;
        struct xen_sysctl_credit_schedule sched_credit;
    } u;
};
typedef struct xen_sysctl_scheduler_op xen_sysctl_scheduler_op_t;
typedef struct { xen_sysctl_scheduler_op_t *p; } __guest_handle_xen_sysctl_scheduler_op_t; typedef struct { const xen_sysctl_scheduler_op_t *p; } __guest_handle_const_xen_sysctl_scheduler_op_t;
struct xen_sysctl_coverage_op {
    uint32_t cmd;
    union {
        uint32_t total_size;
        __guest_handle_uint8 raw_info;
    } u;
};
typedef struct xen_sysctl_coverage_op xen_sysctl_coverage_op_t;
typedef struct { xen_sysctl_coverage_op_t *p; } __guest_handle_xen_sysctl_coverage_op_t; typedef struct { const xen_sysctl_coverage_op_t *p; } __guest_handle_const_xen_sysctl_coverage_op_t;
struct xen_sysctl {
    uint32_t cmd;
    uint32_t interface_version;
    union {
        struct xen_sysctl_readconsole readconsole;
        struct xen_sysctl_tbuf_op tbuf_op;
        struct xen_sysctl_physinfo physinfo;
        struct xen_sysctl_topologyinfo topologyinfo;
        struct xen_sysctl_numainfo numainfo;
        struct xen_sysctl_sched_id sched_id;
        struct xen_sysctl_perfc_op perfc_op;
        struct xen_sysctl_getdomaininfolist getdomaininfolist;
        struct xen_sysctl_debug_keys debug_keys;
        struct xen_sysctl_getcpuinfo getcpuinfo;
        struct xen_sysctl_availheap availheap;
        struct xen_sysctl_get_pmstat get_pmstat;
        struct xen_sysctl_cpu_hotplug cpu_hotplug;
        struct xen_sysctl_pm_op pm_op;
        struct xen_sysctl_page_offline_op page_offline;
        struct xen_sysctl_lockprof_op lockprof_op;
        struct xen_sysctl_cpupool_op cpupool_op;
        struct xen_sysctl_scheduler_op scheduler_op;
        struct xen_sysctl_coverage_op coverage_op;
        uint8_t pad[128];
    } u;
};
typedef struct xen_sysctl xen_sysctl_t;
typedef struct { xen_sysctl_t *p; } __guest_handle_xen_sysctl_t; typedef struct { const xen_sysctl_t *p; } __guest_handle_const_xen_sysctl_t;
typedef unsigned int RING_IDX;
typedef struct mem_event_st {
    uint32_t flags;
    uint32_t vcpu_id;
    uint64_t gfn;
    uint64_t offset;
    uint64_t gla;
    uint32_t p2mt;
    uint16_t access_r:1;
    uint16_t access_w:1;
    uint16_t access_x:1;
    uint16_t gla_valid:1;
    uint16_t available:12;
    uint16_t reason;
} mem_event_request_t, mem_event_response_t;
union mem_event_sring_entry { mem_event_request_t req; mem_event_response_t rsp; }; struct mem_event_sring { RING_IDX req_prod, req_event; RING_IDX rsp_prod, rsp_event; union { struct { uint8_t smartpoll_active; } netif; struct { uint8_t msg; } tapif_user; uint8_t pvt_pad[4]; } private; uint8_t __pad[44]; union mem_event_sring_entry ring[1]; }; struct mem_event_front_ring { RING_IDX req_prod_pvt; RING_IDX rsp_cons; unsigned int nr_ents; struct mem_event_sring *sring; }; struct mem_event_back_ring { RING_IDX rsp_prod_pvt; RING_IDX req_cons; unsigned int nr_ents; struct mem_event_sring *sring; }; typedef struct mem_event_sring mem_event_sring_t; typedef struct mem_event_front_ring mem_event_front_ring_t; typedef struct mem_event_back_ring mem_event_back_ring_t;
#pragma pack(4)
#pragma pack(4)
struct compat_vcpu_runstate_info {
    int state;
    uint64_t state_entry_time;
    uint64_t time[4];
};
typedef struct compat_vcpu_runstate_info vcpu_runstate_info_compat_t;
typedef struct { compat_ptr_t c; vcpu_runstate_info_compat_t *_[0] __attribute__((__packed__)); } __compat_handle_vcpu_runstate_info_compat_t; typedef struct { compat_ptr_t c; const vcpu_runstate_info_compat_t *_[0] __attribute__((__packed__)); } __compat_handle_const_vcpu_runstate_info_compat_t;
struct compat_vcpu_register_runstate_memory_area {
    union {
        __compat_handle_vcpu_runstate_info_compat_t h;
        struct compat_vcpu_runstate_info *v;
        uint64_t p;
    } addr;
};
typedef struct compat_vcpu_register_runstate_memory_area vcpu_register_runstate_memory_area_compat_t;
typedef struct { compat_ptr_t c; vcpu_register_runstate_memory_area_compat_t *_[0] __attribute__((__packed__)); } __compat_handle_vcpu_register_runstate_memory_area_compat_t; typedef struct { compat_ptr_t c; const vcpu_register_runstate_memory_area_compat_t *_[0] __attribute__((__packed__)); } __compat_handle_const_vcpu_register_runstate_memory_area_compat_t;
struct compat_vcpu_set_periodic_timer {
    uint64_t period_ns;
};
typedef struct vcpu_set_periodic_timer vcpu_set_periodic_timer_compat_t;
typedef struct { compat_ptr_t c; vcpu_set_periodic_timer_compat_t *_[0] __attribute__((__packed__)); } __compat_handle_vcpu_set_periodic_timer_compat_t; typedef struct { compat_ptr_t c; const vcpu_set_periodic_timer_compat_t *_[0] __attribute__((__packed__)); } __compat_handle_const_vcpu_set_periodic_timer_compat_t;
struct compat_vcpu_set_singleshot_timer {
    uint64_t timeout_abs_ns;
    uint32_t flags;
};
typedef struct compat_vcpu_set_singleshot_timer vcpu_set_singleshot_timer_compat_t;
typedef struct { compat_ptr_t c; vcpu_set_singleshot_timer_compat_t *_[0] __attribute__((__packed__)); } __compat_handle_vcpu_set_singleshot_timer_compat_t; typedef struct { compat_ptr_t c; const vcpu_set_singleshot_timer_compat_t *_[0] __attribute__((__packed__)); } __compat_handle_const_vcpu_set_singleshot_timer_compat_t;
struct compat_vcpu_register_vcpu_info {
    uint64_t mfn;
    uint32_t offset;
    uint32_t rsvd;
};
typedef struct vcpu_register_vcpu_info vcpu_register_vcpu_info_compat_t;
typedef struct { compat_ptr_t c; vcpu_register_vcpu_info_compat_t *_[0] __attribute__((__packed__)); } __compat_handle_vcpu_register_vcpu_info_compat_t; typedef struct { compat_ptr_t c; const vcpu_register_vcpu_info_compat_t *_[0] __attribute__((__packed__)); } __compat_handle_const_vcpu_register_vcpu_info_compat_t;
struct compat_vcpu_get_physid {
    uint64_t phys_id;
};
typedef struct vcpu_get_physid vcpu_get_physid_compat_t;
typedef struct { compat_ptr_t c; vcpu_get_physid_compat_t *_[0] __attribute__((__packed__)); } __compat_handle_vcpu_get_physid_compat_t; typedef struct { compat_ptr_t c; const vcpu_get_physid_compat_t *_[0] __attribute__((__packed__)); } __compat_handle_const_vcpu_get_physid_compat_t;
typedef struct { compat_ptr_t c; vcpu_time_info_compat_t *_[0] __attribute__((__packed__)); } __compat_handle_vcpu_time_info_compat_t; typedef struct { compat_ptr_t c; const vcpu_time_info_compat_t *_[0] __attribute__((__packed__)); } __compat_handle_const_vcpu_time_info_compat_t;
struct compat_vcpu_register_time_memory_area {
    union {
        __compat_handle_vcpu_time_info_compat_t h;
        struct compat_vcpu_time_info *v;
        uint64_t p;
    } addr;
};
typedef struct compat_vcpu_register_time_memory_area vcpu_register_time_memory_area_compat_t;
typedef struct { compat_ptr_t c; vcpu_register_time_memory_area_compat_t *_[0] __attribute__((__packed__)); } __compat_handle_vcpu_register_time_memory_area_compat_t; typedef struct { compat_ptr_t c; const vcpu_register_time_memory_area_compat_t *_[0] __attribute__((__packed__)); } __compat_handle_const_vcpu_register_time_memory_area_compat_t;
#pragma pack()
typedef struct { vcpu_runstate_info_compat_t *p; } __guest_handle_vcpu_runstate_info_compat_t; typedef struct { const vcpu_runstate_info_compat_t *p; } __guest_handle_const_vcpu_runstate_info_compat_t;
extern struct domain *dom0;
struct evtchn
{
    u8 state;
    u8 xen_consumer;
    u16 notify_vcpu_id;
    u32 port;
    union {
        struct {
            domid_t remote_domid;
        } unbound;
        struct {
            u16 remote_port;
            struct domain *remote_dom;
        } interdomain;
        struct {
            u16 irq;
            u16 next_port;
            u16 prev_port;
        } pirq;
        u16 virq;
    } u;
    u8 priority;
    u8 pending:1;
    u16 last_vcpu_id;
    u8 last_priority;
};
int evtchn_init(struct domain *d);
void evtchn_destroy(struct domain *d);
void evtchn_destroy_final(struct domain *d);
struct waitqueue_vcpu;
struct vcpu
{
    int vcpu_id;
    int processor;
    vcpu_info_t *vcpu_info;
    struct domain *domain;
    struct vcpu *next_in_list;
    s_time_t periodic_period;
    s_time_t periodic_last_event;
    struct timer periodic_timer;
    struct timer singleshot_timer;
    struct timer poll_timer;
    void *sched_priv;
    struct vcpu_runstate_info runstate;
    union {
        __guest_handle_vcpu_runstate_info_t native;
        __guest_handle_vcpu_runstate_info_compat_t compat;
    } runstate_guest;
    uint64_t last_run_time;
    bool_t fpu_initialised;
    bool_t fpu_dirtied;
    bool_t is_initialised;
    bool_t is_running;
    bool_t is_urgent;
    struct {
        bool_t pending;
        uint8_t old_mask;
    } async_exception_state[2];
    uint8_t async_exception_mask;
    bool_t defer_shutdown;
    bool_t paused_for_shutdown;
    bool_t affinity_broken;
    int poll_evtchn;
    int pirq_evtchn_head;
    unsigned long pause_flags;
    atomic_t pause_count;
    u16 virq_to_evtchn[24];
    spinlock_t virq_lock;
    cpumask_var_t cpu_affinity;
    cpumask_var_t cpu_affinity_tmp;
    cpumask_var_t cpu_affinity_saved;
    cpumask_var_t vcpu_dirty_cpumask;
    struct tasklet continue_hypercall_tasklet;
    struct mc_state mc_state;
    struct waitqueue_vcpu *waitqueue_vcpu;
    unsigned long vcpu_info_mfn;
    struct evtchn_fifo_vcpu *evtchn_fifo;
    struct arch_vcpu arch;
};
struct mem_event_domain
{
    spinlock_t ring_lock;
    unsigned char foreign_producers;
    unsigned char target_producers;
    void *ring_page;
    struct page_info *ring_pg_struct;
    mem_event_front_ring_t front_ring;
    int xen_port;
    int pause_flag;
    struct waitqueue_head wq;
    unsigned int blocked;
    unsigned int last_vcpu_wake_up;
};
struct mem_event_per_domain
{
    struct mem_event_domain share;
    struct mem_event_domain paging;
    struct mem_event_domain access;
};
struct evtchn_port_ops;
enum guest_type {
    guest_type_pv, guest_type_pvh, guest_type_hvm
};
struct domain
{
    domid_t domain_id;
    shared_info_t *shared_info;
    spinlock_t domain_lock;
    spinlock_t page_alloc_lock;
    struct page_list_head page_list;
    struct page_list_head xenpage_list;
    unsigned int tot_pages;
    unsigned int outstanding_pages;
    unsigned int max_pages;
    atomic_t shr_pages;
    atomic_t paged_pages;
    unsigned int xenheap_pages;
    unsigned int max_vcpus;
    void *sched_priv;
    struct cpupool *cpupool;
    struct domain *next_in_list;
    struct domain *next_in_hashbucket;
    struct list_head rangesets;
    spinlock_t rangesets_lock;
    struct evtchn *evtchn;
    struct evtchn **evtchn_group[((((((sizeof(xen_ulong_t) * sizeof(xen_ulong_t) * 64)) > ((1 << 17)) ? ((sizeof(xen_ulong_t) * sizeof(xen_ulong_t) * 64)) : ((1 << 17)))) + (((((1L) << 12)/sizeof(struct evtchn *)) * (((1L) << 12) / ((( ( ( ( ((sizeof(struct evtchn))-1) | ( ((sizeof(struct evtchn))-1) >> 1)) | ( ( ((sizeof(struct evtchn))-1) | ( ((sizeof(struct evtchn))-1) >> 1)) >> 2)) | ( ( ( ((sizeof(struct evtchn))-1) | ( ((sizeof(struct evtchn))-1) >> 1)) | ( ( ((sizeof(struct evtchn))-1) | ( ((sizeof(struct evtchn))-1) >> 1)) >> 2)) >> 4)) | ( ( ( ( ((sizeof(struct evtchn))-1) | ( ((sizeof(struct evtchn))-1) >> 1)) | ( ( ((sizeof(struct evtchn))-1) | ( ((sizeof(struct evtchn))-1) >> 1)) >> 2)) | ( ( ( ((sizeof(struct evtchn))-1) | ( ((sizeof(struct evtchn))-1) >> 1)) | ( ( ((sizeof(struct evtchn))-1) | ( ((sizeof(struct evtchn))-1) >> 1)) >> 2)) >> 4)) >> 8)) | (( ( ( ( ((sizeof(struct evtchn))-1) | ( ((sizeof(struct evtchn))-1) >> 1)) | ( ( ((sizeof(struct evtchn))-1) | ( ((sizeof(struct evtchn))-1) >> 1)) >> 2)) | ( ( ( ((sizeof(struct evtchn))-1) | ( ((sizeof(struct evtchn))-1) >> 1)) | ( ( ((sizeof(struct evtchn))-1) | ( ((sizeof(struct evtchn))-1) >> 1)) >> 2)) >> 4)) | ( ( ( ( ((sizeof(struct evtchn))-1) | ( ((sizeof(struct evtchn))-1) >> 1)) | ( ( ((sizeof(struct evtchn))-1) | ( ((sizeof(struct evtchn))-1) >> 1)) >> 2)) | ( ( ( ((sizeof(struct evtchn))-1) | ( ((sizeof(struct evtchn))-1) >> 1)) | ( ( ((sizeof(struct evtchn))-1) | ( ((sizeof(struct evtchn))-1) >> 1)) >> 2)) >> 4)) >> 8)) >>16)) + 1)))) - 1) / (((((1L) << 12)/sizeof(struct evtchn *)) * (((1L) << 12) / ((( ( ( ( ((sizeof(struct evtchn))-1) | ( ((sizeof(struct evtchn))-1) >> 1)) | ( ( ((sizeof(struct evtchn))-1) | ( ((sizeof(struct evtchn))-1) >> 1)) >> 2)) | ( ( ( ((sizeof(struct evtchn))-1) | ( ((sizeof(struct evtchn))-1) >> 1)) | ( ( ((sizeof(struct evtchn))-1) | ( ((sizeof(struct evtchn))-1) >> 1)) >> 2)) >> 4)) | ( ( ( ( ((sizeof(struct evtchn))-1) | ( ((sizeof(struct evtchn))-1) >> 1)) | ( ( ((sizeof(struct evtchn))-1) | ( ((sizeof(struct evtchn))-1) >> 1)) >> 2)) | ( ( ( ((sizeof(struct evtchn))-1) | ( ((sizeof(struct evtchn))-1) >> 1)) | ( ( ((sizeof(struct evtchn))-1) | ( ((sizeof(struct evtchn))-1) >> 1)) >> 2)) >> 4)) >> 8)) | (( ( ( ( ((sizeof(struct evtchn))-1) | ( ((sizeof(struct evtchn))-1) >> 1)) | ( ( ((sizeof(struct evtchn))-1) | ( ((sizeof(struct evtchn))-1) >> 1)) >> 2)) | ( ( ( ((sizeof(struct evtchn))-1) | ( ((sizeof(struct evtchn))-1) >> 1)) | ( ( ((sizeof(struct evtchn))-1) | ( ((sizeof(struct evtchn))-1) >> 1)) >> 2)) >> 4)) | ( ( ( ( ((sizeof(struct evtchn))-1) | ( ((sizeof(struct evtchn))-1) >> 1)) | ( ( ((sizeof(struct evtchn))-1) | ( ((sizeof(struct evtchn))-1) >> 1)) >> 2)) | ( ( ( ((sizeof(struct evtchn))-1) | ( ((sizeof(struct evtchn))-1) >> 1)) | ( ( ((sizeof(struct evtchn))-1) | ( ((sizeof(struct evtchn))-1) >> 1)) >> 2)) >> 4)) >> 8)) >>16)) + 1)))))];
    unsigned int max_evtchns;
    unsigned int max_evtchn_port;
    spinlock_t event_lock;
    const struct evtchn_port_ops *evtchn_port_ops;
    struct evtchn_fifo_domain *evtchn_fifo;
    struct grant_table *grant_table;
    unsigned int nr_pirqs;
    struct radix_tree_root pirq_tree;
    struct rangeset *iomem_caps;
    struct rangeset *irq_caps;
    enum guest_type guest_type;
    s8 need_iommu;
    bool_t auto_node_affinity;
    bool_t is_privileged;
    struct domain *target;
    bool_t debugger_attached;
    enum { DOMDYING_alive, DOMDYING_dying, DOMDYING_dead } is_dying;
    bool_t is_paused_by_controller;
    bool_t is_pinned;
    unsigned long *poll_mask;
    spinlock_t shutdown_lock;
    bool_t is_shutting_down;
    bool_t is_shut_down;
    int shutdown_code;
    int suspend_evtchn;
    atomic_t pause_count;
    unsigned long vm_assist;
    atomic_t refcnt;
    struct vcpu **vcpu;
    cpumask_var_t domain_dirty_cpumask;
    struct arch_domain arch;
    void *ssid;
    xen_domain_handle_t handle;
    char *pbuf;
    unsigned pbuf_idx;
    spinlock_t pbuf_lock;
    struct xenoprof *xenoprof;
    int32_t time_offset_seconds;
    spinlock_t watchdog_lock;
    uint32_t watchdog_inuse_map;
    struct timer watchdog_timer[2];
    struct rcu_head rcu;
    spinlock_t hypercall_deadlock_mutex;
    struct client *tmem_client;
    struct lock_profile_qhead profile_head;
    bool_t disable_migrate;
    struct mem_event_per_domain *mem_event;
    nodemask_t node_affinity;
    unsigned int last_alloc_node;
    spinlock_t node_affinity_lock;
};
struct domain_setup_info
{
    unsigned long image_addr;
    unsigned long image_len;
    unsigned long v_start;
    unsigned long v_end;
    unsigned long v_kernstart;
    unsigned long v_kernend;
    unsigned long v_kernentry;
    unsigned int pae_kernel;
    unsigned long elf_paddr_offset;
    unsigned int load_symtab;
    unsigned long symtab_addr;
    unsigned long symtab_len;
};
extern spinlock_t domlist_update_lock;
extern rcu_read_lock_t domlist_read_lock;
extern struct vcpu *idle_vcpu[256];
static __inline__ __attribute__ ((always_inline)) int get_domain(struct domain *d)
{
    atomic_t old, new, seen = d->refcnt;
    do
    {
        old = seen;
        if ( __builtin_expect((((old).counter) & (1<<31)),0) )
            return 0;
        (((new).counter) = (((old).counter) + 1));
        seen = atomic_compareandswap(old, new, &d->refcnt);
    }
    while ( __builtin_expect((((seen).counter) != ((old).counter)),0) );
    return 1;
}
static __inline__ void get_knownalive_domain(struct domain *d)
{
    atomic_inc(&d->refcnt);
    do { if ( 0 && (!(({ typeof(*&((&d->refcnt)->counter)) __x; switch ( sizeof(*&((&d->refcnt)->counter)) ) { case 1: __x = (typeof(*&((&d->refcnt)->counter)))read_u8_atomic((uint8_t *)&((&d->refcnt)->counter)); break; case 2: __x = (typeof(*&((&d->refcnt)->counter)))read_u16_atomic((uint16_t *)&((&d->refcnt)->counter)); break; case 4: __x = (typeof(*&((&d->refcnt)->counter)))read_u32_atomic((uint32_t *)&((&d->refcnt)->counter)); break; case 8: __x = (typeof(*&((&d->refcnt)->counter)))read_u64_atomic((uint64_t *)&((&d->refcnt)->counter)); break; default: __x = 0; __bad_atomic_size(); break; } __x; }) & (1<<31))) ); } while (0);
}
int domain_set_node_affinity(struct domain *d, const nodemask_t *affinity);
void domain_update_node_affinity(struct domain *d);
struct domain *domain_create(
    domid_t domid, unsigned int domcr_flags, uint32_t ssidref);
struct domain *rcu_lock_domain_by_id(domid_t dom);
struct domain *rcu_lock_domain_by_any_id(domid_t dom);
int rcu_lock_remote_domain_by_id(domid_t dom, struct domain **d);
int rcu_lock_live_remote_domain_by_id(domid_t dom, struct domain **d);
static __inline__ void rcu_unlock_domain(struct domain *d)
{
    if ( d != ((get_cpu_info()->current_vcpu))->domain )
        ({ ((void)(&domlist_read_lock)); do { __asm__ __volatile__("": : :"memory"); ((*({ unsigned long __ptr; __asm__ ("" : "=r"(__ptr) : "0"(&per_cpu____preempt_count)); (typeof(&per_cpu____preempt_count)) (__ptr + (get_cpu_info()->per_cpu_offset)); })))--; } while (0); });
}
static __inline__ struct domain *rcu_lock_domain(struct domain *d)
{
    if ( d != ((get_cpu_info()->current_vcpu))->domain )
        ({ ((void)(d)); do { ((*({ unsigned long __ptr; __asm__ ("" : "=r"(__ptr) : "0"(&per_cpu____preempt_count)); (typeof(&per_cpu____preempt_count)) (__ptr + (get_cpu_info()->per_cpu_offset)); })))++; __asm__ __volatile__("": : :"memory"); } while (0); });
    return d;
}
static __inline__ struct domain *rcu_lock_current_domain(void)
{
    return (((get_cpu_info()->current_vcpu))->domain);
}
struct domain *get_domain_by_id(domid_t dom);
void domain_destroy(struct domain *d);
int domain_kill(struct domain *d);
void domain_shutdown(struct domain *d, u8 reason);
void domain_resume(struct domain *d);
void domain_pause_for_debugger(void);
int vcpu_start_shutdown_deferral(struct vcpu *v);
void vcpu_end_shutdown_deferral(struct vcpu *v);
void __domain_crash(struct domain *d);
void __domain_crash_synchronous(void) __attribute__((noreturn));
void asm_domain_crash_synchronous(unsigned long addr) __attribute__((noreturn));
void scheduler_init(void);
int sched_init_vcpu(struct vcpu *v, unsigned int processor);
void sched_destroy_vcpu(struct vcpu *v);
int sched_init_domain(struct domain *d);
void sched_destroy_domain(struct domain *d);
int sched_move_domain(struct domain *d, struct cpupool *c);
long sched_adjust(struct domain *, struct xen_domctl_scheduler_op *);
long sched_adjust_global(struct xen_sysctl_scheduler_op *);
void sched_set_node_affinity(struct domain *, nodemask_t *);
int sched_id(void);
void sched_tick_suspend(void);
void sched_tick_resume(void);
void vcpu_wake(struct vcpu *v);
void vcpu_sleep_nosync(struct vcpu *v);
void vcpu_sleep_sync(struct vcpu *v);
void sync_vcpu_execstate(struct vcpu *v);
void sync_local_execstate(void);
void context_switch(
    struct vcpu *prev,
    struct vcpu *next);
void context_saved(struct vcpu *prev);
void continue_running(
    struct vcpu *same);
void startup_cpu_idle_loop(void);
extern void (*pm_idle) (void);
extern void (*dead_idle) (void);
unsigned long hypercall_create_continuation(
    unsigned int op, const char *format, ...);
void hypercall_cancel_continuation(void);
extern struct domain *domain_list;
static __inline__ struct domain *first_domain_in_cpupool( struct cpupool *c)
{
    struct domain *d;
    for (d = (domain_list); d && d->cpupool != c;
         d = (d->next_in_list));
    return d;
}
static __inline__ struct domain *next_domain_in_cpupool(
    struct domain *d, struct cpupool *c)
{
    for (d = (d->next_in_list); d && d->cpupool != c;
         d = (d->next_in_list));
    return d;
}
static __inline__ int vcpu_runnable(struct vcpu *v)
{
    return !(v->pause_flags |
             ({ typeof(*&((&v->pause_count)->counter)) __x; switch ( sizeof(*&((&v->pause_count)->counter)) ) { case 1: __x = (typeof(*&((&v->pause_count)->counter)))read_u8_atomic((uint8_t *)&((&v->pause_count)->counter)); break; case 2: __x = (typeof(*&((&v->pause_count)->counter)))read_u16_atomic((uint16_t *)&((&v->pause_count)->counter)); break; case 4: __x = (typeof(*&((&v->pause_count)->counter)))read_u32_atomic((uint32_t *)&((&v->pause_count)->counter)); break; case 8: __x = (typeof(*&((&v->pause_count)->counter)))read_u64_atomic((uint64_t *)&((&v->pause_count)->counter)); break; default: __x = 0; __bad_atomic_size(); break; } __x; }) |
             ({ typeof(*&((&v->domain->pause_count)->counter)) __x; switch ( sizeof(*&((&v->domain->pause_count)->counter)) ) { case 1: __x = (typeof(*&((&v->domain->pause_count)->counter)))read_u8_atomic((uint8_t *)&((&v->domain->pause_count)->counter)); break; case 2: __x = (typeof(*&((&v->domain->pause_count)->counter)))read_u16_atomic((uint16_t *)&((&v->domain->pause_count)->counter)); break; case 4: __x = (typeof(*&((&v->domain->pause_count)->counter)))read_u32_atomic((uint32_t *)&((&v->domain->pause_count)->counter)); break; case 8: __x = (typeof(*&((&v->domain->pause_count)->counter)))read_u64_atomic((uint64_t *)&((&v->domain->pause_count)->counter)); break; default: __x = 0; __bad_atomic_size(); break; } __x; }));
}
void vcpu_block(void);
void vcpu_unblock(struct vcpu *v);
void vcpu_pause(struct vcpu *v);
void vcpu_pause_nosync(struct vcpu *v);
void domain_pause(struct domain *d);
void domain_pause_nosync(struct domain *d);
void vcpu_unpause(struct vcpu *v);
void domain_unpause(struct domain *d);
void domain_pause_by_systemcontroller(struct domain *d);
void domain_unpause_by_systemcontroller(struct domain *d);
void cpu_init(void);
struct scheduler;
struct scheduler *scheduler_get_default(void);
struct scheduler *scheduler_alloc(unsigned int sched_id, int *perr);
void scheduler_free(struct scheduler *sched);
int schedule_cpu_switch(unsigned int cpu, struct cpupool *c);
void vcpu_force_reschedule(struct vcpu *v);
int cpu_disable_scheduler(unsigned int cpu);
int vcpu_set_affinity(struct vcpu *v, const cpumask_t *affinity);
void restore_vcpu_affinity(struct domain *d);
void vcpu_runstate_get(struct vcpu *v, struct vcpu_runstate_info *runstate);
uint64_t get_cpu_idle_time(unsigned int cpu);
void watchdog_domain_init(struct domain *d);
void watchdog_domain_destroy(struct domain *d);
void set_vcpu_migration_delay(unsigned int delay);
unsigned int get_vcpu_migration_delay(void);
extern bool_t sched_smt_power_savings;
extern enum cpufreq_controller {
    FREQCTL_none, FREQCTL_dom0_kernel, FREQCTL_xen
} cpufreq_controller;
struct cpupool *cpupool_get_by_id(int poolid);
void cpupool_put(struct cpupool *pool);
int cpupool_add_domain(struct domain *d, int poolid);
void cpupool_rm_domain(struct domain *d);
int cpupool_do_sysctl(struct xen_sysctl_cpupool_op *op);
void schedule_dump(struct cpupool *c);
extern void dump_runq(unsigned char key);
void arch_do_physinfo(xen_sysctl_physinfo_t *pi);
void *map_domain_page(unsigned long mfn);
void unmap_domain_page(const void *va);
void clear_domain_page(unsigned long mfn);
void copy_domain_page(unsigned long dmfn, unsigned long smfn);
unsigned long domain_page_map_to_mfn(const void *va);
void *map_domain_page_global(unsigned long mfn);
void unmap_domain_page_global(const void *va);
struct domain_mmap_cache {
    unsigned long mfn;
    void *va;
    unsigned int flags;
};
static __inline__ void
domain_mmap_cache_init(struct domain_mmap_cache *cache)
{
    do { if ( 0 && (cache != ((void*)0)) ); } while (0);
    cache->flags = 0;
    cache->mfn = 0;
    cache->va = ((void*)0);
}
static __inline__ void *
map_domain_page_with_cache(unsigned long mfn, struct domain_mmap_cache *cache)
{
    do { if ( 0 && (cache != ((void*)0)) ); } while (0);
    do { if (__builtin_expect((cache->flags & 2U),0)) do { do { ((void)sizeof(struct { int:-!!((72) >> ((31 - 24) + (31 - 24))); })); asm volatile ( ".Lbug%=: ud2\n" ".pushsection .bug_frames.%c0, \"a\", @progbits\n" ".p2align 2\n" ".Lfrm%=:\n" ".long (.Lbug%= - .Lfrm%=) + %c4\n" ".long (%c1 - .Lfrm%=) + %c3\n" ".if " "0" "\n" ".long 0, %c2 - .Lfrm%=\n" ".endif\n" ".popsection" : : "i" (2), "i" ("/home/dquinlan/ROSE/Zen/rose-sh/workspace/xen/phase_1/xen-src/xen/include/xen/domain_page.h"), "i" (((void*)0)), "i" ((72 & ((1 << (31 - 24)) - 1)) << 24), "i" (((72) >> (31 - 24)) << 24)); } while (0); do {} while (1); } while (0); } while (0);
    if ( __builtin_expect((cache->flags & 1U),1) )
    {
        cache->flags |= 2U;
        if ( __builtin_expect((mfn == cache->mfn),1) )
            goto done;
        unmap_domain_page(cache->va);
    }
    cache->mfn = mfn;
    cache->va = map_domain_page(mfn);
    cache->flags = 2U | 1U;
 done:
    return cache->va;
}
static __inline__ void
unmap_domain_page_with_cache(const void *va, struct domain_mmap_cache *cache)
{
    do { if ( 0 && (cache != ((void*)0)) ); } while (0);
    cache->flags &= ~2U;
}
static __inline__ void
domain_mmap_cache_destroy(struct domain_mmap_cache *cache)
{
    do { if ( 0 && (cache != ((void*)0)) ); } while (0);
    do { if (__builtin_expect((cache->flags & 2U),0)) do { do { ((void)sizeof(struct { int:-!!((101) >> ((31 - 24) + (31 - 24))); })); asm volatile ( ".Lbug%=: ud2\n" ".pushsection .bug_frames.%c0, \"a\", @progbits\n" ".p2align 2\n" ".Lfrm%=:\n" ".long (.Lbug%= - .Lfrm%=) + %c4\n" ".long (%c1 - .Lfrm%=) + %c3\n" ".if " "0" "\n" ".long 0, %c2 - .Lfrm%=\n" ".endif\n" ".popsection" : : "i" (2), "i" ("/home/dquinlan/ROSE/Zen/rose-sh/workspace/xen/phase_1/xen-src/xen/include/xen/domain_page.h"), "i" (((void*)0)), "i" ((101 & ((1 << (31 - 24)) - 1)) << 24), "i" (((101) >> (31 - 24)) << 24)); } while (0); do {} while (1); } while (0); } while (0);
    if ( __builtin_expect((cache->flags & 1U),1) )
    {
        unmap_domain_page(cache->va);
        cache->flags = 0;
    }
}
extern u32 tlbflush_clock;
extern __typeof__(u32) per_cpu__tlbflush_time;
static __inline__ int NEED_FLUSH(u32 cpu_stamp, u32 lastuse_stamp)
{
    u32 curr_time = tlbflush_clock;
    return ((curr_time == 0) ||
            ((cpu_stamp <= lastuse_stamp) &&
             (lastuse_stamp <= curr_time)));
}
void new_tlbflush_clock_period(void);
static __inline__ unsigned long read_cr3(void)
{
    unsigned long cr3;
    __asm__ __volatile__ (
        "mov %%cr3, %0" : "=r" (cr3) : );
    return cr3;
}
void write_cr3(unsigned long cr3);
void flush_area_local(const void *va, unsigned int flags);
void flush_area_mask(const cpumask_t *, const void *va, unsigned int flags);
struct sh_emulate_ctxt;
struct shadow_paging_mode {
    void (*detach_old_tables )(struct vcpu *v);
    int (*x86_emulate_write )(struct vcpu *v, unsigned long va,
                                            void *src, u32 bytes,
                                            struct sh_emulate_ctxt *sh_ctxt);
    int (*x86_emulate_cmpxchg )(struct vcpu *v, unsigned long va,
                                            unsigned long old,
                                            unsigned long new,
                                            unsigned int bytes,
                                            struct sh_emulate_ctxt *sh_ctxt);
    mfn_t (*make_monitor_table )(struct vcpu *v);
    void (*destroy_monitor_table )(struct vcpu *v, mfn_t mmfn);
    int (*guess_wrmap )(struct vcpu *v,
                                            unsigned long vaddr, mfn_t gmfn);
    void (*pagetable_dying )(struct vcpu *v, paddr_t gpa);
    unsigned int shadow_levels;
};
struct paging_mode {
    int (*page_fault )(struct vcpu *v, unsigned long va,
                                            struct cpu_user_regs *regs);
    int (*invlpg )(struct vcpu *v, unsigned long va);
    unsigned long (*gva_to_gfn )(struct vcpu *v,
                                            struct p2m_domain *p2m,
                                            unsigned long va,
                                            uint32_t *pfec);
    unsigned long (*p2m_ga_to_gfn )(struct vcpu *v,
                                            struct p2m_domain *p2m,
                                            unsigned long cr3,
                                            paddr_t ga, uint32_t *pfec,
                                            unsigned int *page_order);
    void (*update_cr3 )(struct vcpu *v, int do_locking);
    void (*update_paging_modes )(struct vcpu *v);
    void (*write_p2m_entry )(struct vcpu *v, unsigned long gfn,
                                            l1_pgentry_t *p, mfn_t table_mfn,
                                            l1_pgentry_t new,
                                            unsigned int level);
    int (*write_guest_entry )(struct vcpu *v, intpte_t *p,
                                            intpte_t new, mfn_t gmfn);
    int (*cmpxchg_guest_entry )(struct vcpu *v, intpte_t *p,
                                            intpte_t *old, intpte_t new,
                                            mfn_t gmfn);
    void * (*guest_map_l1e )(struct vcpu *v, unsigned long va,
                                            unsigned long *gl1mfn);
    void (*guest_get_eff_l1e )(struct vcpu *v, unsigned long va,
                                            void *eff_l1e);
    unsigned int guest_levels;
    struct shadow_paging_mode shadow;
};
void paging_free_log_dirty_bitmap(struct domain *d);
void paging_log_dirty_range(struct domain *d,
                            unsigned long begin_pfn,
                            unsigned long nr,
                            uint8_t *dirty_bitmap);
int paging_log_dirty_enable(struct domain *d, bool_t log_global);
int paging_log_dirty_disable(struct domain *d);
void paging_log_dirty_init(struct domain *d,
                           int (*enable_log_dirty)(struct domain *d,
                                                    bool_t log_global),
                           int (*disable_log_dirty)(struct domain *d),
                           void (*clean_dirty_bitmap)(struct domain *d));
void paging_mark_dirty(struct domain *d, unsigned long guest_mfn);
int paging_mfn_is_dirty(struct domain *d, mfn_t gmfn);
struct sh_dirty_vram {
    unsigned long begin_pfn;
    unsigned long end_pfn;
    paddr_t *sl1ma;
    uint8_t *dirty_bitmap;
    s_time_t last_dirty;
};
void paging_vcpu_init(struct vcpu *v);
int paging_domain_init(struct domain *d, unsigned int domcr_flags);
int paging_domctl(struct domain *d, xen_domctl_shadow_op_t *sc,
                  __guest_handle_void u_domctl);
void paging_teardown(struct domain *d);
void paging_final_teardown(struct domain *d);
int paging_enable(struct domain *d, u32 mode);
const struct paging_mode *paging_get_mode(struct vcpu *v);
void paging_update_nestedmode(struct vcpu *v);
static __inline__ int
paging_fault(unsigned long va, struct cpu_user_regs *regs)
{
    struct vcpu *v = ((get_cpu_info()->current_vcpu));
    return ((v)->arch.paging.mode)->page_fault(v, va, regs);
}
static __inline__ int paging_invlpg(struct vcpu *v, unsigned long va)
{
    return ((v)->arch.paging.mode)->invlpg(v, va);
}
unsigned long paging_gva_to_gfn(struct vcpu *v,
                                unsigned long va,
                                uint32_t *pfec);
static __inline__ unsigned long paging_ga_to_gfn_cr3(struct vcpu *v,
                                                 unsigned long cr3,
                                                 paddr_t ga,
                                                 uint32_t *pfec,
                                                 unsigned int *page_order)
{
    struct p2m_domain *p2m = v->domain->arch.p2m;
    return ((v)->arch.paging.mode)->p2m_ga_to_gfn(v, p2m, cr3, ga, pfec,
        page_order);
}
static __inline__ void paging_update_cr3(struct vcpu *v)
{
    ((v)->arch.paging.mode)->update_cr3(v, 1);
}
static __inline__ void paging_update_paging_modes(struct vcpu *v)
{
    ((v)->arch.paging.mode)->update_paging_modes(v);
}
static __inline__ int paging_write_guest_entry(struct vcpu *v, intpte_t *p,
                                           intpte_t new, mfn_t gmfn)
{
    if ( __builtin_expect((((v->domain)->arch.paging.mode) && v->arch.paging.mode != ((void*)0)),0) )
        return ((v)->arch.paging.mode)->write_guest_entry(v, p, new, gmfn);
    else
        return (!__copy_to_user(p, &new, sizeof(new)));
}
static __inline__ int paging_cmpxchg_guest_entry(struct vcpu *v, intpte_t *p,
                                             intpte_t *old, intpte_t new,
                                             mfn_t gmfn)
{
    if ( __builtin_expect((((v->domain)->arch.paging.mode) && v->arch.paging.mode != ((void*)0)),0) )
        return ((v)->arch.paging.mode)->cmpxchg_guest_entry(v, p, old, new, gmfn);
    else
        return (!({ int _rc; switch ( sizeof(*(p)) ) { case 1: asm volatile ( "1: lock; cmpxchg""b"" %""b""2,%3\n" "2:\n" ".section .fixup,\"ax\"\n" "3:     movl $1,%1\n" "       jmp 2b\n" ".previous\n" " .section .ex_table" "" ",\"a\"\n" " .balign 4\n" " .long " "1b" "-." ", " "3b" "-." "\n" " .previous\n" : "=a" (*old), "=r" (_rc) : "q" (new), "m" (*((volatile struct __xchg_dummy *)((volatile void *)p))), "0" (*old), "1" (0) : "memory");; break; case 2: asm volatile ( "1: lock; cmpxchg""w"" %""w""2,%3\n" "2:\n" ".section .fixup,\"ax\"\n" "3:     movl $1,%1\n" "       jmp 2b\n" ".previous\n" " .section .ex_table" "" ",\"a\"\n" " .balign 4\n" " .long " "1b" "-." ", " "3b" "-." "\n" " .previous\n" : "=a" (*old), "=r" (_rc) : "r" (new), "m" (*((volatile struct __xchg_dummy *)((volatile void *)p))), "0" (*old), "1" (0) : "memory");; break; case 4: asm volatile ( "1: lock; cmpxchg""l"" %""k""2,%3\n" "2:\n" ".section .fixup,\"ax\"\n" "3:     movl $1,%1\n" "       jmp 2b\n" ".previous\n" " .section .ex_table" "" ",\"a\"\n" " .balign 4\n" " .long " "1b" "-." ", " "3b" "-." "\n" " .previous\n" : "=a" (*old), "=r" (_rc) : "r" (new), "m" (*((volatile struct __xchg_dummy *)((volatile void *)p))), "0" (*old), "1" (0) : "memory");; break; case 8: asm volatile ( "1: lock; cmpxchg""q"" %""""2,%3\n" "2:\n" ".section .fixup,\"ax\"\n" "3:     movl $1,%1\n" "       jmp 2b\n" ".previous\n" " .section .ex_table" "" ",\"a\"\n" " .balign 4\n" " .long " "1b" "-." ", " "3b" "-." "\n" " .previous\n" : "=a" (*old), "=r" (_rc) : "r" (new), "m" (*((volatile struct __xchg_dummy *)((volatile void *)p))), "0" (*old), "1" (0) : "memory");; break; } _rc; }));
}
static __inline__ void safe_write_pte(l1_pgentry_t *p, l1_pgentry_t new)
{
    *p = new;
}
struct p2m_domain;
void paging_write_p2m_entry(struct p2m_domain *p2m, unsigned long gfn,
                            l1_pgentry_t *p, mfn_t table_mfn,
                            l1_pgentry_t new, unsigned int level);
void pagetable_dying(struct domain *d, paddr_t gpa);
void paging_dump_domain_info(struct domain *d);
void paging_dump_vcpu_info(struct vcpu *v);
static __inline__ l1_pgentry_t *
guest_map_l1e(struct vcpu *v, unsigned long addr, unsigned long *gl1mfn)
{
    l2_pgentry_t l2e;
    if ( __builtin_expect((!(((unsigned long)(addr) < (1UL<<47)) || ((unsigned long)(addr) >= ((((((256UL) >> 8) * (0xffff000000000000UL)) | ((256UL) << 39))) + ((1UL) << 39)*16)))),0) )
        return ((void*)0);
    if ( __builtin_expect((((v->domain)->arch.paging.mode & ((1 << 3) << 10))),0) )
        return ((v)->arch.paging.mode)->guest_map_l1e(v, addr, gl1mfn);
    if ( __copy_from_user(&l2e,
                          &((l2_pgentry_t *)(((l1_pgentry_t *)((((((258UL) >> 8) * (0xffff000000000000UL)) | ((258UL) << 39))))) + ((((((((258UL) >> 8) * (0xffff000000000000UL)) | ((258UL) << 39)))) & ((1UL << 48)-1)) >> 12)))[(((addr) & ((1UL << 48)-1)) >> 21)],
                          sizeof(l2_pgentry_t)) != 0 )
        return ((void*)0);
    if ( (((((int)(((l2e).l2) >> 40) & ~0xFFF) | ((int)((l2e).l2) & 0xFFF))) & ((0x001U) | (0x080U)))
         != (0x001U) )
        return ((void*)0);
    *gl1mfn = ((unsigned long)(((l2e).l2 & (((1UL << 52)-1)&(~(((1L) << 12)-1)))) >> 12));
    return (l1_pgentry_t *)map_domain_page(*gl1mfn) + (((addr) >> 12) & ((1<<9) - 1));
}
static __inline__ void
guest_unmap_l1e(struct vcpu *v, void *p)
{
    unmap_domain_page(p);
}
static __inline__ void
guest_get_eff_l1e(struct vcpu *v, unsigned long addr, l1_pgentry_t *eff_l1e)
{
    if ( __builtin_expect((!(((unsigned long)(addr) < (1UL<<47)) || ((unsigned long)(addr) >= ((((((256UL) >> 8) * (0xffff000000000000UL)) | ((256UL) << 39))) + ((1UL) << 39)*16)))),0) )
    {
        *eff_l1e = ((l1_pgentry_t) { 0 });
        return;
    }
    if ( __builtin_expect((!((v->domain)->arch.paging.mode & ((1 << 3) << 10))),1) )
    {
        do { if ( 0 && (!((v->domain)->arch.paging.mode & ((1 << 4) << 10))) ); } while (0);
        if ( __copy_from_user(eff_l1e,
                              &((l1_pgentry_t *)((((((258UL) >> 8) * (0xffff000000000000UL)) | ((258UL) << 39)))))[(((addr) & ((1UL << 48)-1)) >> 12)],
                              sizeof(l1_pgentry_t)) != 0 )
            *eff_l1e = ((l1_pgentry_t) { 0 });
        return;
    }
    ((v)->arch.paging.mode)->guest_get_eff_l1e(v, addr, eff_l1e);
}
static __inline__ void
guest_get_eff_kern_l1e(struct vcpu *v, unsigned long addr, void *eff_l1e)
{
    int user_mode = !(v->arch.flags & (1<<0));
    if ( user_mode ) toggle_guest_mode(v);
    guest_get_eff_l1e(v, addr, eff_l1e);
    if ( user_mode ) toggle_guest_mode(v);
}
typedef struct hvm_domain_context {
    uint32_t cur;
    uint32_t size;
    uint8_t *data;
} hvm_domain_context_t;
int _hvm_init_entry(struct hvm_domain_context *h,
                    uint16_t tc, uint16_t inst, uint32_t len);
void _hvm_write_entry(struct hvm_domain_context *h,
                      void *src, uint32_t src_len);
int _hvm_check_entry(struct hvm_domain_context *h,
                     uint16_t type, uint32_t len, bool_t strict_length);
void _hvm_read_entry(struct hvm_domain_context *h,
                     void *dest, uint32_t dest_len);
static __inline__ uint16_t hvm_load_instance(struct hvm_domain_context *h)
{
    struct hvm_save_descriptor *d
        = (struct hvm_save_descriptor *)&h->data[h->cur];
    return d->instance;
}
typedef int (*hvm_save_handler) (struct domain *d,
                                 hvm_domain_context_t *h);
typedef int (*hvm_load_handler) (struct domain *d,
                                 hvm_domain_context_t *h);
void hvm_register_savevm(uint16_t typecode,
                         const char *name,
                         hvm_save_handler save_state,
                         hvm_load_handler load_state,
                         size_t size, int kind);
size_t hvm_save_size(struct domain *d);
int hvm_save(struct domain *d, hvm_domain_context_t *h);
int hvm_save_one(struct domain *d, uint16_t typecode, uint16_t instance,
                 __guest_handle_uint8 handle);
int hvm_load(struct domain *d, hvm_domain_context_t *h);
struct hvm_save_header;
void arch_hvm_save(struct domain *d, struct hvm_save_header *hdr);
int arch_hvm_load(struct domain *d, struct hvm_save_header *hdr);
static __inline__ ioreq_t *get_ioreq(struct vcpu *v)
{
    struct domain *d = v->domain;
    shared_iopage_t *p = d->arch.hvm_domain.ioreq.va;
    do { if ( 0 && ((v == ((get_cpu_info()->current_vcpu))) || _spin_is_locked(&d->arch.hvm_domain.ioreq.lock)) ); } while (0);
    return p ? &p->vcpu_ioreq[v->vcpu_id] : ((void*)0);
}
extern unsigned long hvm_io_bitmap[];
enum hvm_copy_result {
    HVMCOPY_okay = 0,
    HVMCOPY_bad_gva_to_gfn,
    HVMCOPY_bad_gfn_to_mfn,
    HVMCOPY_unhandleable,
    HVMCOPY_gfn_paged_out,
    HVMCOPY_gfn_shared,
};
enum hvm_copy_result hvm_copy_to_guest_phys(
    paddr_t paddr, void *buf, int size);
enum hvm_copy_result hvm_copy_from_guest_phys(
    void *buf, paddr_t paddr, int size);
enum hvm_copy_result hvm_copy_to_guest_virt(
    unsigned long vaddr, void *buf, int size, uint32_t pfec);
enum hvm_copy_result hvm_copy_from_guest_virt(
    void *buf, unsigned long vaddr, int size, uint32_t pfec);
enum hvm_copy_result hvm_fetch_from_guest_virt(
    void *buf, unsigned long vaddr, int size, uint32_t pfec);
enum hvm_copy_result hvm_copy_to_guest_virt_nofault(
    unsigned long vaddr, void *buf, int size, uint32_t pfec);
enum hvm_copy_result hvm_copy_from_guest_virt_nofault(
    void *buf, unsigned long vaddr, int size, uint32_t pfec);
enum hvm_copy_result hvm_fetch_from_guest_virt_nofault(
    void *buf, unsigned long vaddr, int size, uint32_t pfec);
int hvm_do_hypercall(struct cpu_user_regs *pregs);
void hvm_hlt(unsigned long rflags);
void hvm_triple_fault(void);
void hvm_rdtsc_intercept(struct cpu_user_regs *regs);
int __attribute__((warn_unused_result)) hvm_handle_xsetbv(u32 index, u64 new_bv);
void hvm_shadow_handle_cd(struct vcpu *v, unsigned long value);
int hvm_set_efer(uint64_t value);
int hvm_set_cr0(unsigned long value);
int hvm_set_cr3(unsigned long value);
int hvm_set_cr4(unsigned long value);
int hvm_msr_read_intercept(unsigned int msr, uint64_t *msr_content);
int hvm_msr_write_intercept(unsigned int msr, uint64_t msr_content);
int hvm_mov_to_cr(unsigned int cr, unsigned int gpr);
int hvm_mov_from_cr(unsigned int cr, unsigned int gpr);
unsigned long copy_to_user_hvm(void *to, const void *from, unsigned len);
unsigned long clear_user_hvm(void *to, unsigned int len);
unsigned long copy_from_user_hvm(void *to, const void *from, unsigned len);
struct xenpf_settime {
    uint32_t secs;
    uint32_t nsecs;
    uint64_t system_time;
};
typedef struct xenpf_settime xenpf_settime_t;
typedef struct { xenpf_settime_t *p; } __guest_handle_xenpf_settime_t; typedef struct { const xenpf_settime_t *p; } __guest_handle_const_xenpf_settime_t;
struct xenpf_add_memtype {
    xen_pfn_t mfn;
    uint64_t nr_mfns;
    uint32_t type;
    uint32_t handle;
    uint32_t reg;
};
typedef struct xenpf_add_memtype xenpf_add_memtype_t;
typedef struct { xenpf_add_memtype_t *p; } __guest_handle_xenpf_add_memtype_t; typedef struct { const xenpf_add_memtype_t *p; } __guest_handle_const_xenpf_add_memtype_t;
struct xenpf_del_memtype {
    uint32_t handle;
    uint32_t reg;
};
typedef struct xenpf_del_memtype xenpf_del_memtype_t;
typedef struct { xenpf_del_memtype_t *p; } __guest_handle_xenpf_del_memtype_t; typedef struct { const xenpf_del_memtype_t *p; } __guest_handle_const_xenpf_del_memtype_t;
struct xenpf_read_memtype {
    uint32_t reg;
    xen_pfn_t mfn;
    uint64_t nr_mfns;
    uint32_t type;
};
typedef struct xenpf_read_memtype xenpf_read_memtype_t;
typedef struct { xenpf_read_memtype_t *p; } __guest_handle_xenpf_read_memtype_t; typedef struct { const xenpf_read_memtype_t *p; } __guest_handle_const_xenpf_read_memtype_t;
struct xenpf_microcode_update {
    __guest_handle_const_void data;
    uint32_t length;
};
typedef struct xenpf_microcode_update xenpf_microcode_update_t;
typedef struct { xenpf_microcode_update_t *p; } __guest_handle_xenpf_microcode_update_t; typedef struct { const xenpf_microcode_update_t *p; } __guest_handle_const_xenpf_microcode_update_t;
struct xenpf_platform_quirk {
    uint32_t quirk_id;
};
typedef struct xenpf_platform_quirk xenpf_platform_quirk_t;
typedef struct { xenpf_platform_quirk_t *p; } __guest_handle_xenpf_platform_quirk_t; typedef struct { const xenpf_platform_quirk_t *p; } __guest_handle_const_xenpf_platform_quirk_t;
struct xenpf_efi_runtime_call {
    uint32_t function;
    uint32_t misc;
    unsigned long status;
    union {
        struct {
            struct xenpf_efi_time {
                uint16_t year;
                uint8_t month;
                uint8_t day;
                uint8_t hour;
                uint8_t min;
                uint8_t sec;
                uint32_t ns;
                int16_t tz;
                uint8_t daylight;
            } time;
            uint32_t resolution;
            uint32_t accuracy;
        } get_time;
        struct xenpf_efi_time set_time;
        struct xenpf_efi_time get_wakeup_time;
        struct xenpf_efi_time set_wakeup_time;
        struct {
            __guest_handle_void name;
            unsigned long size;
            __guest_handle_void data;
            struct xenpf_efi_guid {
                uint32_t data1;
                uint16_t data2;
                uint16_t data3;
                uint8_t data4[8];
            } vendor_guid;
        } get_variable, set_variable;
        struct {
            unsigned long size;
            __guest_handle_void name;
            struct xenpf_efi_guid vendor_guid;
        } get_next_variable_name;
        struct {
            uint32_t attr;
            uint64_t max_store_size;
            uint64_t remain_store_size;
            uint64_t max_size;
        } query_variable_info;
        struct {
            __guest_handle_void capsule_header_array;
            unsigned long capsule_count;
            uint64_t max_capsule_size;
            unsigned int reset_type;
        } query_capsule_capabilities;
        struct {
            __guest_handle_void capsule_header_array;
            unsigned long capsule_count;
            uint64_t sg_list;
        } update_capsule;
    } u;
};
typedef struct xenpf_efi_runtime_call xenpf_efi_runtime_call_t;
typedef struct { xenpf_efi_runtime_call_t *p; } __guest_handle_xenpf_efi_runtime_call_t; typedef struct { const xenpf_efi_runtime_call_t *p; } __guest_handle_const_xenpf_efi_runtime_call_t;
struct xenpf_firmware_info {
    uint32_t type;
    uint32_t index;
    union {
        struct {
            uint8_t device;
            uint8_t version;
            uint16_t interface_support;
            uint16_t legacy_max_cylinder;
            uint8_t legacy_max_head;
            uint8_t legacy_sectors_per_track;
            __guest_handle_void edd_params;
        } disk_info;
        struct {
            uint8_t device;
            uint32_t mbr_signature;
        } disk_mbr_signature;
        struct {
            uint8_t capabilities;
            uint8_t edid_transfer_time;
            __guest_handle_uint8 edid;
        } vbeddc_info;
        union xenpf_efi_info {
            uint32_t version;
            struct {
                uint64_t addr;
                uint32_t nent;
            } cfg;
            struct {
                uint32_t revision;
                uint32_t bufsz;
                __guest_handle_void name;
            } vendor;
            struct {
                uint64_t addr;
                uint64_t size;
                uint64_t attr;
                uint32_t type;
            } mem;
            struct {
                uint16_t segment;
                uint8_t bus;
                uint8_t devfn;
                uint16_t vendor;
                uint16_t devid;
                uint64_t address;
                xen_ulong_t size;
            } pci_rom;
        } efi_info;
        uint8_t kbd_shift_flags;
    } u;
};
typedef struct xenpf_firmware_info xenpf_firmware_info_t;
typedef struct { xenpf_firmware_info_t *p; } __guest_handle_xenpf_firmware_info_t; typedef struct { const xenpf_firmware_info_t *p; } __guest_handle_const_xenpf_firmware_info_t;
struct xenpf_enter_acpi_sleep {
    uint16_t val_a;
    uint16_t val_b;
    uint32_t sleep_state;
    uint32_t flags;
};
typedef struct xenpf_enter_acpi_sleep xenpf_enter_acpi_sleep_t;
typedef struct { xenpf_enter_acpi_sleep_t *p; } __guest_handle_xenpf_enter_acpi_sleep_t; typedef struct { const xenpf_enter_acpi_sleep_t *p; } __guest_handle_const_xenpf_enter_acpi_sleep_t;
struct xenpf_change_freq {
    uint32_t flags;
    uint32_t cpu;
    uint64_t freq;
};
typedef struct xenpf_change_freq xenpf_change_freq_t;
typedef struct { xenpf_change_freq_t *p; } __guest_handle_xenpf_change_freq_t; typedef struct { const xenpf_change_freq_t *p; } __guest_handle_const_xenpf_change_freq_t;
struct xenpf_getidletime {
    __guest_handle_uint8 cpumap_bitmap;
    uint32_t cpumap_nr_cpus;
    __guest_handle_uint64 idletime;
    uint64_t now;
};
typedef struct xenpf_getidletime xenpf_getidletime_t;
typedef struct { xenpf_getidletime_t *p; } __guest_handle_xenpf_getidletime_t; typedef struct { const xenpf_getidletime_t *p; } __guest_handle_const_xenpf_getidletime_t;
struct xen_power_register {
    uint32_t space_id;
    uint32_t bit_width;
    uint32_t bit_offset;
    uint32_t access_size;
    uint64_t address;
};
struct xen_processor_csd {
    uint32_t domain;
    uint32_t coord_type;
    uint32_t num;
};
typedef struct xen_processor_csd xen_processor_csd_t;
typedef struct { xen_processor_csd_t *p; } __guest_handle_xen_processor_csd_t; typedef struct { const xen_processor_csd_t *p; } __guest_handle_const_xen_processor_csd_t;
struct xen_processor_cx {
    struct xen_power_register reg;
    uint8_t type;
    uint32_t latency;
    uint32_t power;
    uint32_t dpcnt;
    __guest_handle_xen_processor_csd_t dp;
};
typedef struct xen_processor_cx xen_processor_cx_t;
typedef struct { xen_processor_cx_t *p; } __guest_handle_xen_processor_cx_t; typedef struct { const xen_processor_cx_t *p; } __guest_handle_const_xen_processor_cx_t;
struct xen_processor_flags {
    uint32_t bm_control:1;
    uint32_t bm_check:1;
    uint32_t has_cst:1;
    uint32_t power_setup_done:1;
    uint32_t bm_rld_set:1;
};
struct xen_processor_power {
    uint32_t count;
    struct xen_processor_flags flags;
    __guest_handle_xen_processor_cx_t states;
};
struct xen_pct_register {
    uint8_t descriptor;
    uint16_t length;
    uint8_t space_id;
    uint8_t bit_width;
    uint8_t bit_offset;
    uint8_t reserved;
    uint64_t address;
};
struct xen_processor_px {
    uint64_t core_frequency;
    uint64_t power;
    uint64_t transition_latency;
    uint64_t bus_master_latency;
    uint64_t control;
    uint64_t status;
};
typedef struct xen_processor_px xen_processor_px_t;
typedef struct { xen_processor_px_t *p; } __guest_handle_xen_processor_px_t; typedef struct { const xen_processor_px_t *p; } __guest_handle_const_xen_processor_px_t;
struct xen_psd_package {
    uint64_t num_entries;
    uint64_t revision;
    uint64_t domain;
    uint64_t coord_type;
    uint64_t num_processors;
};
struct xen_processor_performance {
    uint32_t flags;
    uint32_t platform_limit;
    struct xen_pct_register control_register;
    struct xen_pct_register status_register;
    uint32_t state_count;
    __guest_handle_xen_processor_px_t states;
    struct xen_psd_package domain_info;
    uint32_t shared_type;
};
typedef struct xen_processor_performance xen_processor_performance_t;
typedef struct { xen_processor_performance_t *p; } __guest_handle_xen_processor_performance_t; typedef struct { const xen_processor_performance_t *p; } __guest_handle_const_xen_processor_performance_t;
struct xenpf_set_processor_pminfo {
    uint32_t id;
    uint32_t type;
    union {
        struct xen_processor_power power;
        struct xen_processor_performance perf;
        __guest_handle_uint32 pdc;
    } u;
};
typedef struct xenpf_set_processor_pminfo xenpf_set_processor_pminfo_t;
typedef struct { xenpf_set_processor_pminfo_t *p; } __guest_handle_xenpf_set_processor_pminfo_t; typedef struct { const xenpf_set_processor_pminfo_t *p; } __guest_handle_const_xenpf_set_processor_pminfo_t;
struct xenpf_pcpuinfo {
    uint32_t xen_cpuid;
    uint32_t max_present;
    uint32_t flags;
    uint32_t apic_id;
    uint32_t acpi_id;
};
typedef struct xenpf_pcpuinfo xenpf_pcpuinfo_t;
typedef struct { xenpf_pcpuinfo_t *p; } __guest_handle_xenpf_pcpuinfo_t; typedef struct { const xenpf_pcpuinfo_t *p; } __guest_handle_const_xenpf_pcpuinfo_t;
struct xenpf_pcpu_version {
    uint32_t xen_cpuid;
    uint32_t max_present;
    char vendor_id[12];
    uint32_t family;
    uint32_t model;
    uint32_t stepping;
};
typedef struct xenpf_pcpu_version xenpf_pcpu_version_t;
typedef struct { xenpf_pcpu_version_t *p; } __guest_handle_xenpf_pcpu_version_t; typedef struct { const xenpf_pcpu_version_t *p; } __guest_handle_const_xenpf_pcpu_version_t;
struct xenpf_cpu_ol
{
    uint32_t cpuid;
};
typedef struct xenpf_cpu_ol xenpf_cpu_ol_t;
typedef struct { xenpf_cpu_ol_t *p; } __guest_handle_xenpf_cpu_ol_t; typedef struct { const xenpf_cpu_ol_t *p; } __guest_handle_const_xenpf_cpu_ol_t;
struct xenpf_cpu_hotadd
{
 uint32_t apic_id;
 uint32_t acpi_id;
 uint32_t pxm;
};
struct xenpf_mem_hotadd
{
    uint64_t spfn;
    uint64_t epfn;
    uint32_t pxm;
    uint32_t flags;
};
struct xenpf_core_parking {
    uint32_t type;
    uint32_t idle_nums;
};
typedef struct xenpf_core_parking xenpf_core_parking_t;
typedef struct { xenpf_core_parking_t *p; } __guest_handle_xenpf_core_parking_t; typedef struct { const xenpf_core_parking_t *p; } __guest_handle_const_xenpf_core_parking_t;
struct xen_platform_op {
    uint32_t cmd;
    uint32_t interface_version;
    union {
        struct xenpf_settime settime;
        struct xenpf_add_memtype add_memtype;
        struct xenpf_del_memtype del_memtype;
        struct xenpf_read_memtype read_memtype;
        struct xenpf_microcode_update microcode;
        struct xenpf_platform_quirk platform_quirk;
        struct xenpf_efi_runtime_call efi_runtime_call;
        struct xenpf_firmware_info firmware_info;
        struct xenpf_enter_acpi_sleep enter_acpi_sleep;
        struct xenpf_change_freq change_freq;
        struct xenpf_getidletime getidletime;
        struct xenpf_set_processor_pminfo set_pminfo;
        struct xenpf_pcpuinfo pcpu_info;
        struct xenpf_pcpu_version pcpu_version;
        struct xenpf_cpu_ol cpu_ol;
        struct xenpf_cpu_hotadd cpu_add;
        struct xenpf_mem_hotadd mem_add;
        struct xenpf_core_parking core_parking;
        uint8_t pad[128];
    } u;
};
typedef struct xen_platform_op xen_platform_op_t;
typedef struct { xen_platform_op_t *p; } __guest_handle_xen_platform_op_t; typedef struct { const xen_platform_op_t *p; } __guest_handle_const_xen_platform_op_t;
int set_px_pminfo(uint32_t cpu, struct xen_processor_performance *perf);
long set_cx_pminfo(uint32_t cpu, struct xen_processor_power *power);
uint32_t pmstat_get_cx_nr(uint32_t cpuid);
int pmstat_get_cx_stat(uint32_t cpuid, struct pm_cx_stat *stat);
int pmstat_reset_cx_stat(uint32_t cpuid);
int do_get_pm_info(struct xen_sysctl_get_pmstat *op);
int do_pm_op(struct xen_sysctl_pm_op *op);


void foobar()
   {
     long ret = 0;
     struct xen_platform_op curop, *op = &curop;
     ({ 
        const typeof(*((*
          ({ unsigned long __ptr; 
             __asm__ ("" : "=r"(__ptr) : "0"((typeof(&(boot_edid_info)))(__maddr_to_virt((unsigned long)((__virt_to_maddr((unsigned long)(&(boot_edid_info))))))))); 
             (typeof((typeof(&(boot_edid_info)))(__maddr_to_virt((unsigned long)((__virt_to_maddr((unsigned long)(&(boot_edid_info))))))))) (__ptr + (trampoline_phys-(__virt_to_maddr((unsigned long)(trampoline_start))))); 
           })))) *_s = ((*
               ({ unsigned long __ptr; __asm__ ("" : "=r"(__ptr) : "0"((typeof(&(boot_edid_info)))(__maddr_to_virt((unsigned long)((__virt_to_maddr((unsigned long)(&(boot_edid_info))))))))); 
                  (typeof((typeof(&(boot_edid_info)))(__maddr_to_virt((unsigned long)((__virt_to_maddr((unsigned long)(&(boot_edid_info))))))))) (__ptr + (trampoline_phys-(__virt_to_maddr((unsigned long)(trampoline_start))))); 
                })));
#if 1
        char (*_d)[sizeof(*_s)] = (void *)(op->u.firmware_info.u.vbeddc_info.edid).p;
        ((void)((op->u.firmware_info.u.vbeddc_info.edid).p == ((*
          ({ unsigned long __ptr;
             __asm__ ("" : "=r"(__ptr) : "0"((typeof(&(boot_edid_info)))(__maddr_to_virt((unsigned long)((__virt_to_maddr((unsigned long)(&(boot_edid_info)))))))));
            (typeof((typeof(&(boot_edid_info)))(__maddr_to_virt((unsigned long)((__virt_to_maddr((unsigned long)(&(boot_edid_info))))))))) (__ptr + (trampoline_phys-(__virt_to_maddr((unsigned long)(trampoline_start)))));
           })))));
#endif
        (((((((get_cpu_info()->current_vcpu)))->domain)->guest_type != guest_type_pv)) ? copy_to_user_hvm((_d+(0)), (_s), (sizeof(*_s)*(128))) : copy_to_user((_d+(0)), (_s), (sizeof(*_s)*(128))));
     });
   }
