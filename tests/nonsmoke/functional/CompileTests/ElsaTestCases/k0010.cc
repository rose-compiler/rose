// type sizes used in compile-time assertions

// originally found in package lzo

typedef unsigned int size_t;
typedef int ptrdiff_t;
typedef unsigned int lzo_uint32;
typedef int lzo_int32;
typedef unsigned int lzo_uint;
typedef int lzo_int;
typedef int lzo_bool;
typedef ptrdiff_t lzo_ptrdiff_t;
typedef unsigned long lzo_ptr_t;
typedef long lzo_sptr_t;


int foo() {
    { typedef int F[1 - 2 * !(8 == 8)]; };
    { typedef int F[1 - 2 * !(sizeof(char) == 1)]; };
    { typedef int F[1 - 2 * !(sizeof(short) >= 2)]; };
    { typedef int F[1 - 2 * !(sizeof(long) >= 4)]; };
    { typedef int F[1 - 2 * !(sizeof(int) >= sizeof(short))]; };
    { typedef int F[1 - 2 * !(sizeof(long) >= sizeof(int))]; };
    { typedef int F[1 - 2 * !(sizeof(lzo_uint) == sizeof(lzo_int))]; };
    { typedef int F[1 - 2 * !(sizeof(lzo_uint32) == sizeof(lzo_int32))]; };
    { typedef int F[1 - 2 * !(sizeof(lzo_uint32) >= 4)]; };
    { typedef int F[1 - 2 * !(sizeof(lzo_uint32) >= sizeof(unsigned))]; };
    { typedef int F[1 - 2 * !(sizeof(lzo_uint) >= 4)]; };
    { typedef int F[1 - 2 * !(sizeof(lzo_uint) >= sizeof(unsigned))]; };
    { typedef int F[1 - 2 * !(sizeof(short) == 2)]; };
    { typedef int F[1 - 2 * !(sizeof(int) == 4)]; };
    { typedef int F[1 - 2 * !(sizeof(long) == 4)]; };
    { typedef int F[1 - 2 * !(4 == sizeof(unsigned))]; };
    { typedef int F[1 - 2 * !(4 == sizeof(unsigned long))]; };
    { typedef int F[1 - 2 * !(2 == sizeof(unsigned short))]; };
    { typedef int F[1 - 2 * !(4 == sizeof(size_t))]; };
    { typedef int F[1 - 2 * !((((unsigned char) (-1)) > ((unsigned char) 0)))]; };
    { typedef int F[1 - 2 * !((((unsigned short) (-1)) > ((unsigned short) 0)))]; };
    { typedef int F[1 - 2 * !((((unsigned) (-1)) > ((unsigned) 0)))]; };
    { typedef int F[1 - 2 * !((((unsigned long) (-1)) > ((unsigned long) 0)))]; };
    { typedef int F[1 - 2 * !((((short) (-1)) < ((short) 0)))]; };
    { typedef int F[1 - 2 * !((((int) (-1)) < ((int) 0)))]; };
    { typedef int F[1 - 2 * !((((long) (-1)) < ((long) 0)))]; };
    { typedef int F[1 - 2 * !((((lzo_uint32) (-1)) > ((lzo_uint32) 0)))]; };
    { typedef int F[1 - 2 * !((((lzo_uint) (-1)) > ((lzo_uint) 0)))]; };
    { typedef int F[1 - 2 * !((((lzo_int32) (-1)) < ((lzo_int32) 0)))]; };
    { typedef int F[1 - 2 * !((((lzo_int) (-1)) < ((lzo_int) 0)))]; };
    { typedef int F[1 - 2 * !(2147483647 == (((1l << (8*(sizeof(int))-2)) - 1l) + (1l << (8*(sizeof(int))-2))))]; };
    { typedef int F[1 - 2 * !((2147483647 * 2U + 1U) == (((1ul << (8*(sizeof(unsigned))-1)) - 1ul) + (1ul << (8*(sizeof(unsigned))-1))))]; };
    { typedef int F[1 - 2 * !(2147483647L == (((1l << (8*(sizeof(long))-2)) - 1l) + (1l << (8*(sizeof(long))-2))))]; };
    { typedef int F[1 - 2 * !((2147483647L * 2UL + 1UL) == (((1ul << (8*(sizeof(unsigned long))-1)) - 1ul) + (1ul << (8*(sizeof(unsigned long))-1))))]; };
    { typedef int F[1 - 2 * !(32767 == (((1l << (8*(sizeof(short))-2)) - 1l) + (1l << (8*(sizeof(short))-2))))]; };
    { typedef int F[1 - 2 * !((32767 * 2 + 1) == (((1ul << (8*(sizeof(unsigned short))-1)) - 1ul) + (1ul << (8*(sizeof(unsigned short))-1))))]; };
    { typedef int F[1 - 2 * !((2147483647 * 2U + 1U) == (((1ul << (8*(sizeof(lzo_uint32))-1)) - 1ul) + (1ul << (8*(sizeof(lzo_uint32))-1))))]; };
    { typedef int F[1 - 2 * !((2147483647 * 2U + 1U) == (((1ul << (8*(sizeof(lzo_uint))-1)) - 1ul) + (1ul << (8*(sizeof(lzo_uint))-1))))]; };
    { typedef int F[1 - 2 * !((((1ul << (8*(4)-1)) - 1ul) + (1ul << (8*(4)-1))) == (((1ul << (8*(sizeof(size_t))-1)) - 1ul) + (1ul << (8*(sizeof(size_t))-1))))]; };
    { typedef int F[1 - 2 * !(sizeof(char *) >= sizeof(int))]; };
    { typedef int F[1 - 2 * !(sizeof(unsigned char *) >= sizeof(char *))]; };
    { typedef int F[1 - 2 * !(sizeof(void *) == sizeof(unsigned char *))]; };
    { typedef int F[1 - 2 * !(sizeof(void *) == sizeof(void * *))]; };
    { typedef int F[1 - 2 * !(sizeof(void *) == sizeof(unsigned char * *))]; };
    { typedef int F[1 - 2 * !(sizeof(void *) >= sizeof(lzo_uint))]; };
    { typedef int F[1 - 2 * !(sizeof(lzo_ptr_t) == sizeof(void *))]; };
    { typedef int F[1 - 2 * !(sizeof(lzo_ptr_t) == sizeof(lzo_sptr_t))]; };
    { typedef int F[1 - 2 * !(sizeof(lzo_ptr_t) >= sizeof(lzo_uint))]; };
    { typedef int F[1 - 2 * !(sizeof(lzo_ptrdiff_t) >= 4)]; };
    { typedef int F[1 - 2 * !(sizeof(lzo_ptrdiff_t) >= sizeof(ptrdiff_t))]; };
    { typedef int F[1 - 2 * !(sizeof(ptrdiff_t) >= sizeof(size_t))]; };
    { typedef int F[1 - 2 * !(sizeof(lzo_ptrdiff_t) >= sizeof(lzo_uint))]; };
    { typedef int F[1 - 2 * !(4 == sizeof(char *))]; };
    { typedef int F[1 - 2 * !(4 == sizeof(ptrdiff_t))]; };
    { typedef int F[1 - 2 * !((((ptrdiff_t) (-1)) < ((ptrdiff_t) 0)))]; };
    { typedef int F[1 - 2 * !((((size_t) (-1)) > ((size_t) 0)))]; };
    { typedef int F[1 - 2 * !((((lzo_ptrdiff_t) (-1)) < ((lzo_ptrdiff_t) 0)))]; };
    { typedef int F[1 - 2 * !((((lzo_sptr_t) (-1)) < ((lzo_sptr_t) 0)))]; };
    { typedef int F[1 - 2 * !((((lzo_ptr_t) (-1)) > ((lzo_ptr_t) 0)))]; };
    { typedef int F[1 - 2 * !((((lzo_uint) (-1)) > ((lzo_uint) 0)))]; };
    { typedef int F[1 - 2 * !((int) ((unsigned char) ((signed char) -1)) == 255)]; };
    { typedef int F[1 - 2 * !((((unsigned char)128) << (int)(8*sizeof(int)-8)) < 0)]; };
    { typedef int F[1 - 2 * !(sizeof(short) == 2)]; };
    { typedef int F[1 - 2 * !(sizeof(lzo_uint32) == 4)]; };
    { typedef int F[1 - 2 * !(sizeof(unsigned char *) == sizeof(const unsigned char *))]; };
}
