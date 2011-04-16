/* OpenCL Built-in Types */
  
  /* Extract and adapted from: CUDA_HOME/include/CL/cl_platform.h */

  /* Define alignment keys */

#if defined( __GNUC__ )
    #define OPENCL_ALIGNED(_x)          __attribute__ ((aligned(_x)))
#elif defined( _WIN32) && (_MSC_VER)
    /* Alignment keys neutered on windows because MSVC can't swallow function arguments with alignment requirements     */
    /* http://msdn.microsoft.com/en-us/library/373ak2y1%28VS.71%29.aspx                                                 */
    /* #include <crtdefs.h>                                                                                             */
    /* #define OPENCL_ALIGNED(_x)          _CRT_ALIGN(_x)                                                                   */
    #define OPENCL_ALIGNED(_x)
#else
   #warning  Need to implement some method to align data here
   #define  OPENCL_ALIGNED(_x)
#endif

  /* Scalar Types  */

#if (defined (_WIN32) && defined(_MSC_VER))

typedef signed   __int8         opencl_char;
typedef unsigned __int8         opencl_uchar;
typedef signed   __int16        opencl_short;
typedef unsigned __int16        opencl_ushort;
typedef signed   __int32        opencl_int;
typedef unsigned __int32        opencl_uint;
typedef signed   __int64        opencl_long;
typedef unsigned __int64        opencl_ulong;

typedef float                   opencl_float;
typedef double                  opencl_double;

typedef unsigned __int16        half;

typedef unsigned __int32        size_t;

#else

#include <stdint.h>

typedef int8_t          opencl_char;
typedef uint8_t         opencl_uchar;
typedef int16_t         opencl_short    __attribute__((aligned(2)));
typedef uint16_t        opencl_ushort   __attribute__((aligned(2)));
typedef int32_t         opencl_int      __attribute__((aligned(4)));
typedef uint32_t        opencl_uint     __attribute__((aligned(4)));
typedef int64_t         opencl_long     __attribute__((aligned(8)));
typedef uint64_t        opencl_ulong    __attribute__((aligned(8)));

typedef float           opencl_float    __attribute__((aligned(4)));
typedef double          opencl_double   __attribute__((aligned(8)));

typedef uint16_t        half     __attribute__((aligned(2)));

#endif

typedef uint32_t ptrdiff_t;
typedef uint32_t intptr_t;
typedef uint32_t uintptr_t;

  /* Vector Types */
  
typedef union
{
    opencl_char  OPENCL_ALIGNED(2) s[2];
#if defined( __GNUC__) && ! defined( __STRICT_ANSI__ )
   __extension__ struct { opencl_char  x, y; };
   __extension__ struct { opencl_char  s0, s1; };
   __extension__ struct { opencl_char  lo, hi; };
#endif
} char2;

typedef union
{
    opencl_char  OPENCL_ALIGNED(4) s[4];
#if defined( __GNUC__) && ! defined( __STRICT_ANSI__ )
   __extension__ struct { opencl_char  x, y, z, w; };
   __extension__ struct { opencl_char  s0, s1, s2, s3; };
   __extension__ struct { char2 lo, hi; };
#endif
} char4;

typedef union
{
    opencl_char   OPENCL_ALIGNED(8) s[8];
#if defined( __GNUC__) && ! defined( __STRICT_ANSI__ )
   __extension__ struct { opencl_char  x, y, z, w; };
   __extension__ struct { opencl_char  s0, s1, s2, s3, s4, s5, s6, s7; };
   __extension__ struct { char4 lo, hi; };
#endif
} char8;

typedef union
{
    opencl_char  OPENCL_ALIGNED(16) s[16];
#if defined( __GNUC__) && ! defined( __STRICT_ANSI__ )
   __extension__ struct { opencl_char  x, y, z, w, __spacer4, __spacer5, __spacer6, __spacer7, __spacer8, __spacer9, sa, sb, sc, sd, se, sf; };
   __extension__ struct { opencl_char  s0, s1, s2, s3, s4, s5, s6, s7, s8, s9, sA, sB, sC, sD, sE, sF; };
   __extension__ struct { char8 lo, hi; };
#endif
} char16;

typedef union
{
    opencl_uchar  OPENCL_ALIGNED(2) s[2];
#if defined( __GNUC__) && ! defined( __STRICT_ANSI__ )
   __extension__ struct { opencl_uchar  x, y; };
   __extension__ struct { opencl_uchar  s0, s1; };
   __extension__ struct { opencl_uchar  lo, hi; };
#endif
} uchar2;

typedef union
{
    opencl_uchar  OPENCL_ALIGNED(4) s[4];
#if defined( __GNUC__) && ! defined( __STRICT_ANSI__ )
   __extension__ struct { opencl_uchar  x, y, z, w; };
   __extension__ struct { opencl_uchar  s0, s1, s2, s3; };
   __extension__ struct { uchar2 lo, hi; };
#endif
} uchar4;

typedef union
{
    opencl_uchar   OPENCL_ALIGNED(8) s[8];
#if defined( __GNUC__) && ! defined( __STRICT_ANSI__ )
   __extension__ struct { opencl_uchar  x, y, z, w; };
   __extension__ struct { opencl_uchar  s0, s1, s2, s3, s4, s5, s6, s7; };
   __extension__ struct { uchar4 lo, hi; };
#endif
} uchar8;

typedef union
{
    opencl_uchar  OPENCL_ALIGNED(16) s[16];
#if defined( __GNUC__) && ! defined( __STRICT_ANSI__ )
   __extension__ struct { opencl_uchar  x, y, z, w, __spacer4, __spacer5, __spacer6, __spacer7, __spacer8, __spacer9, sa, sb, sc, sd, se, sf; };
   __extension__ struct { opencl_uchar  s0, s1, s2, s3, s4, s5, s6, s7, s8, s9, sA, sB, sC, sD, sE, sF; };
   __extension__ struct { uchar8 lo, hi; };
#endif
} uchar16;

typedef union
{
    opencl_short  OPENCL_ALIGNED(2) s[2];
#if defined( __GNUC__) && ! defined( __STRICT_ANSI__ )
   __extension__ struct { opencl_short  x, y; };
   __extension__ struct { opencl_short  s0, s1; };
   __extension__ struct { opencl_short  lo, hi; };
#endif
} short2;

typedef union
{
    opencl_short  OPENCL_ALIGNED(4) s[4];
#if defined( __GNUC__) && ! defined( __STRICT_ANSI__ )
   __extension__ struct { opencl_short  x, y, z, w; };
   __extension__ struct { opencl_short  s0, s1, s2, s3; };
   __extension__ struct { short2 lo, hi; };
#endif
} short4;

typedef union
{
    opencl_short   OPENCL_ALIGNED(8) s[8];
#if defined( __GNUC__) && ! defined( __STRICT_ANSI__ )
   __extension__ struct { opencl_short  x, y, z, w; };
   __extension__ struct { opencl_short  s0, s1, s2, s3, s4, s5, s6, s7; };
   __extension__ struct { short4 lo, hi; };
#endif
} short8;

typedef union
{
    opencl_short  OPENCL_ALIGNED(16) s[16];
#if defined( __GNUC__) && ! defined( __STRICT_ANSI__ )
   __extension__ struct { opencl_short  x, y, z, w, __spacer4, __spacer5, __spacer6, __spacer7, __spacer8, __spacer9, sa, sb, sc, sd, se, sf; };
   __extension__ struct { opencl_short  s0, s1, s2, s3, s4, s5, s6, s7, s8, s9, sA, sB, sC, sD, sE, sF; };
   __extension__ struct { short8 lo, hi; };
#endif
} short16;

typedef union
{
    opencl_ushort  OPENCL_ALIGNED(2) s[2];
#if defined( __GNUC__) && ! defined( __STRICT_ANSI__ )
   __extension__ struct { opencl_ushort  x, y; };
   __extension__ struct { opencl_ushort  s0, s1; };
   __extension__ struct { opencl_ushort  lo, hi; };
#endif
} ushort2;

typedef union
{
    opencl_ushort  OPENCL_ALIGNED(4) s[4];
#if defined( __GNUC__) && ! defined( __STRICT_ANSI__ )
   __extension__ struct { opencl_ushort  x, y, z, w; };
   __extension__ struct { opencl_ushort  s0, s1, s2, s3; };
   __extension__ struct { ushort2 lo, hi; };
#endif
} ushort4;

typedef union
{
    opencl_ushort   OPENCL_ALIGNED(8) s[8];
#if defined( __GNUC__) && ! defined( __STRICT_ANSI__ )
   __extension__ struct { opencl_ushort  x, y, z, w; };
   __extension__ struct { opencl_ushort  s0, s1, s2, s3, s4, s5, s6, s7; };
   __extension__ struct { ushort4 lo, hi; };
#endif
} ushort8;

typedef union
{
    opencl_ushort  OPENCL_ALIGNED(16) s[16];
#if defined( __GNUC__) && ! defined( __STRICT_ANSI__ )
   __extension__ struct { opencl_ushort  x, y, z, w, __spacer4, __spacer5, __spacer6, __spacer7, __spacer8, __spacer9, sa, sb, sc, sd, se, sf; };
   __extension__ struct { opencl_ushort  s0, s1, s2, s3, s4, s5, s6, s7, s8, s9, sA, sB, sC, sD, sE, sF; };
   __extension__ struct { ushort8 lo, hi; };
#endif
} ushort16;

typedef union
{
    opencl_int  OPENCL_ALIGNED(2) s[2];
#if defined( __GNUC__) && ! defined( __STRICT_ANSI__ )
   __extension__ struct { opencl_int  x, y; };
   __extension__ struct { opencl_int  s0, s1; };
   __extension__ struct { opencl_int  lo, hi; };
#endif
} int2;

typedef union
{
    opencl_int  OPENCL_ALIGNED(4) s[4];
#if defined( __GNUC__) && ! defined( __STRICT_ANSI__ )
   __extension__ struct { opencl_int  x, y, z, w; };
   __extension__ struct { opencl_int  s0, s1, s2, s3; };
   __extension__ struct { int2 lo, hi; };
#endif
} int4;

typedef union
{
    opencl_int   OPENCL_ALIGNED(8) s[8];
#if defined( __GNUC__) && ! defined( __STRICT_ANSI__ )
   __extension__ struct { opencl_int  x, y, z, w; };
   __extension__ struct { opencl_int  s0, s1, s2, s3, s4, s5, s6, s7; };
   __extension__ struct { int4 lo, hi; };
#endif
} int8;

typedef union
{
    opencl_int  OPENCL_ALIGNED(16) s[16];
#if defined( __GNUC__) && ! defined( __STRICT_ANSI__ )
   __extension__ struct { opencl_int  x, y, z, w, __spacer4, __spacer5, __spacer6, __spacer7, __spacer8, __spacer9, sa, sb, sc, sd, se, sf; };
   __extension__ struct { opencl_int  s0, s1, s2, s3, s4, s5, s6, s7, s8, s9, sA, sB, sC, sD, sE, sF; };
   __extension__ struct { int8 lo, hi; };
#endif
} int16;

typedef union
{
    opencl_uint  OPENCL_ALIGNED(2) s[2];
#if defined( __GNUC__) && ! defined( __STRICT_ANSI__ )
   __extension__ struct { opencl_uint  x, y; };
   __extension__ struct { opencl_uint  s0, s1; };
   __extension__ struct { opencl_uint  lo, hi; };
#endif
} uint2;

typedef union
{
    opencl_uint  OPENCL_ALIGNED(4) s[4];
#if defined( __GNUC__) && ! defined( __STRICT_ANSI__ )
   __extension__ struct { opencl_uint  x, y, z, w; };
   __extension__ struct { opencl_uint  s0, s1, s2, s3; };
   __extension__ struct { uint2 lo, hi; };
#endif
} uint4;

typedef union
{
    opencl_uint   OPENCL_ALIGNED(8) s[8];
#if defined( __GNUC__) && ! defined( __STRICT_ANSI__ )
   __extension__ struct { opencl_uint  x, y, z, w; };
   __extension__ struct { opencl_uint  s0, s1, s2, s3, s4, s5, s6, s7; };
   __extension__ struct { uint4 lo, hi; };
#endif
} uint8;

typedef union
{
    opencl_uint  OPENCL_ALIGNED(16) s[16];
#if defined( __GNUC__) && ! defined( __STRICT_ANSI__ )
   __extension__ struct { opencl_uint  x, y, z, w, __spacer4, __spacer5, __spacer6, __spacer7, __spacer8, __spacer9, sa, sb, sc, sd, se, sf; };
   __extension__ struct { opencl_uint  s0, s1, s2, s3, s4, s5, s6, s7, s8, s9, sA, sB, sC, sD, sE, sF; };
   __extension__ struct { uint8 lo, hi; };
#endif
} uint16;

typedef union
{
    opencl_long  OPENCL_ALIGNED(2) s[2];
#if defined( __GNUC__) && ! defined( __STRICT_ANSI__ )
   __extension__ struct { opencl_long  x, y; };
   __extension__ struct { opencl_long  s0, s1; };
   __extension__ struct { opencl_long  lo, hi; };
#endif
} long2;

typedef union
{
    opencl_long  OPENCL_ALIGNED(4) s[4];
#if defined( __GNUC__) && ! defined( __STRICT_ANSI__ )
   __extension__ struct { opencl_long  x, y, z, w; };
   __extension__ struct { opencl_long  s0, s1, s2, s3; };
   __extension__ struct { long2 lo, hi; };
#endif
} long4;

typedef union
{
    opencl_long   OPENCL_ALIGNED(8) s[8];
#if defined( __GNUC__) && ! defined( __STRICT_ANSI__ )
   __extension__ struct { opencl_long  x, y, z, w; };
   __extension__ struct { opencl_long  s0, s1, s2, s3, s4, s5, s6, s7; };
   __extension__ struct { long4 lo, hi; };
#endif
} long8;

typedef union
{
    opencl_long  OPENCL_ALIGNED(16) s[16];
#if defined( __GNUC__) && ! defined( __STRICT_ANSI__ )
   __extension__ struct { opencl_long  x, y, z, w, __spacer4, __spacer5, __spacer6, __spacer7, __spacer8, __spacer9, sa, sb, sc, sd, se, sf; };
   __extension__ struct { opencl_long  s0, s1, s2, s3, s4, s5, s6, s7, s8, s9, sA, sB, sC, sD, sE, sF; };
   __extension__ struct { long8 lo, hi; };
#endif
} long16;

typedef union
{
    opencl_ulong  OPENCL_ALIGNED(2) s[2];
#if defined( __GNUC__) && ! defined( __STRICT_ANSI__ )
   __extension__ struct { opencl_ulong  x, y; };
   __extension__ struct { opencl_ulong  s0, s1; };
   __extension__ struct { opencl_ulong  lo, hi; };
#endif
} ulong2;

typedef union
{
    opencl_ulong  OPENCL_ALIGNED(4) s[4];
#if defined( __GNUC__) && ! defined( __STRICT_ANSI__ )
   __extension__ struct { opencl_ulong  x, y, z, w; };
   __extension__ struct { opencl_ulong  s0, s1, s2, s3; };
   __extension__ struct { ulong2 lo, hi; };
#endif
} ulong4;

typedef union
{
    opencl_ulong   OPENCL_ALIGNED(8) s[8];
#if defined( __GNUC__) && ! defined( __STRICT_ANSI__ )
   __extension__ struct { opencl_ulong  x, y, z, w; };
   __extension__ struct { opencl_ulong  s0, s1, s2, s3, s4, s5, s6, s7; };
   __extension__ struct { ulong4 lo, hi; };
#endif
} ulong8;

typedef union
{
    opencl_ulong  OPENCL_ALIGNED(16) s[16];
#if defined( __GNUC__) && ! defined( __STRICT_ANSI__ )
   __extension__ struct { opencl_ulong  x, y, z, w, __spacer4, __spacer5, __spacer6, __spacer7, __spacer8, __spacer9, sa, sb, sc, sd, se, sf; };
   __extension__ struct { opencl_ulong  s0, s1, s2, s3, s4, s5, s6, s7, s8, s9, sA, sB, sC, sD, sE, sF; };
   __extension__ struct { ulong8 lo, hi; };
#endif
} ulong16;

typedef union
{
    opencl_float  OPENCL_ALIGNED(2) s[2];
#if defined( __GNUC__) && ! defined( __STRICT_ANSI__ )
   __extension__ struct { opencl_float  x, y; };
   __extension__ struct { opencl_float  s0, s1; };
   __extension__ struct { opencl_float  lo, hi; };
#endif
} float2;

typedef union
{
    opencl_float  OPENCL_ALIGNED(4) s[4];
#if defined( __GNUC__) && ! defined( __STRICT_ANSI__ )
   __extension__ struct { opencl_float  x, y, z, w; };
   __extension__ struct { opencl_float  s0, s1, s2, s3; };
   __extension__ struct { float2 lo, hi; };
#endif
} float4;

typedef union
{
    opencl_float   OPENCL_ALIGNED(8) s[8];
#if defined( __GNUC__) && ! defined( __STRICT_ANSI__ )
   __extension__ struct { opencl_float  x, y, z, w; };
   __extension__ struct { opencl_float  s0, s1, s2, s3, s4, s5, s6, s7; };
   __extension__ struct { float4 lo, hi; };
#endif
} float8;

typedef union
{
    opencl_float  OPENCL_ALIGNED(16) s[16];
#if defined( __GNUC__) && ! defined( __STRICT_ANSI__ )
   __extension__ struct { opencl_float  x, y, z, w, __spacer4, __spacer5, __spacer6, __spacer7, __spacer8, __spacer9, sa, sb, sc, sd, se, sf; };
   __extension__ struct { opencl_float  s0, s1, s2, s3, s4, s5, s6, s7, s8, s9, sA, sB, sC, sD, sE, sF; };
   __extension__ struct { float8 lo, hi; };
#endif
} float16;

typedef union
{
    opencl_double  OPENCL_ALIGNED(2) s[2];
#if defined( __GNUC__) && ! defined( __STRICT_ANSI__ )
   __extension__ struct { opencl_double  x, y; };
   __extension__ struct { opencl_double  s0, s1; };
   __extension__ struct { opencl_double  lo, hi; };
#endif
} double2;

typedef union
{
    opencl_double  OPENCL_ALIGNED(4) s[4];
#if defined( __GNUC__) && ! defined( __STRICT_ANSI__ )
   __extension__ struct { opencl_double  x, y, z, w; };
   __extension__ struct { opencl_double  s0, s1, s2, s3; };
   __extension__ struct { double2 lo, hi; };
#endif
} double4;

typedef union
{
    opencl_double   OPENCL_ALIGNED(8) s[8];
#if defined( __GNUC__) && ! defined( __STRICT_ANSI__ )
   __extension__ struct { opencl_double  x, y, z, w; };
   __extension__ struct { opencl_double  s0, s1, s2, s3, s4, s5, s6, s7; };
   __extension__ struct { double4 lo, hi; };
#endif
} double8;

typedef union
{
    opencl_double  OPENCL_ALIGNED(16) s[16];
#if defined( __GNUC__) && ! defined( __STRICT_ANSI__ )
   __extension__ struct { opencl_double  x, y, z, w, __spacer4, __spacer5, __spacer6, __spacer7, __spacer8, __spacer9, sa, sb, sc, sd, se, sf; };
   __extension__ struct { opencl_double  s0, s1, s2, s3, s4, s5, s6, s7, s8, s9, sA, sB, sC, sD, sE, sF; };
   __extension__ struct { double8 lo, hi; };
#endif
} double16;


/* OpenCL Built-in Functions */

  /* Work-Item Functions */

uint get_work_dim();
size_t get_global_size(uint dimindx);
size_t get_global_id(uint dimindx);
size_t get_local_size(uint dimindx);
size_t get_local_id(uint dimindx);
size_t get_num_groups(uint dimindx);
size_t get_group_id(uint dimindx);

  /* Image Functions */
  
float4 read_imagef  (image2d_t image, sampler_t sampler, int2 coord);
float4 read_imagef  (image2d_t image, sampler_t sampler, float2 coord);
int4   read_imagei  (image2d_t image, sampler_t sampler, int2 coord);
int4   read_imagei  (image2d_t image, sampler_t sampler, float2 coord);
uint4  read_imageui (image2d_t image, sampler_t sampler, int2 coord);
uint4  read_imageui (image2d_t image, sampler_t sampler, float2 coord);
void   write_imagef (image2d_t image, int2 coord, ﬂoat4 color);
void   write_imagei (image2d_t image, int2 coord, int4 color);
void   write_imageui(image2d_t image, int2 coord, uint4 color);
float4 read_imagef  (image3d_t image, sampler_t sampler, int4 coord);
float4 read_imagef  (image3d_t image, sampler_t sampler, float4 coord);
int4   read_imagei  (image3d_t image, sampler_t sampler, int4 coord);
int4   read_imagei  (image3d_t image, sampler_t sampler, float4 coord);
uint4  read_imageui (image3d_t image, sampler_t sampler, int4 coord);
uint4  read_imageui (image3d_t image, sampler_t sampler, float4 coord);
int    get_image_width (image2d_t image);
int    get_image_width (image3d_t image);
int    get_image_height(image2d_t image);
int    get_image_height(image3d_t image);
int    get_image_depht (image3d_t image);
int    get_image_channel_data_type (image2d_t image);
int    get_image_channel_data_type (image3d_t image);
int    get_image_channel_order (image2d_t image);
int    get_image_channel_order (image3d_t image);
int2   get_image_dim (image2d_t image);
int4   get_image_dim (image3d_t image);

  /* Synchronization Functions */
  
void barrier (cl_mem_fence_ﬂags ﬂags);

  /* Explicit Memory Fence Functions */
  
void mem_fence (cl_mem_fence_ﬂags ﬂags);
void read_mem_fence (cl_mem_fence_ﬂags ﬂags);
void write_mem_fence (cl_mem_fence_ﬂags ﬂags);

  /* Miscellaneous Functions */
  
void wait_group_events (int num_events, event_t * event_list);
event_t async_work_group_copy (__local char *dst, const __global char *src, size_t num_elements, event_t event);
event_t async_work_group_copy (__global char *dst, const __local char *src, size_t num_elements, event_t event);
void prefetch (const __global char *p, size_t num_elements);
event_t async_work_group_copy (__local char2 *dst, const __global char2 *src, size_t num_elements, event_t event);
event_t async_work_group_copy (__global char2 *dst, const __local char2 *src, size_t num_elements, event_t event);
void prefetch (const __global char2 *p, size_t num_elements);
event_t async_work_group_copy (__local char4 *dst, const __global char4 *src, size_t num_elements, event_t event);
event_t async_work_group_copy (__global char4 *dst, const __local char4 *src, size_t num_elements, event_t event);
void prefetch (const __global char4 *p, size_t num_elements);
event_t async_work_group_copy (__local char8 *dst, const __global char8 *src, size_t num_elements, event_t event);
event_t async_work_group_copy (__global char8 *dst, const __local char8 *src, size_t num_elements, event_t event);
void prefetch (const __global char8 *p, size_t num_elements);
event_t async_work_group_copy (__local char16 *dst, const __global char16 *src, size_t num_elements, event_t event);
event_t async_work_group_copy (__global char16 *dst, const __local char16 *src, size_t num_elements, event_t event);
void prefetch (const __global char16 *p, size_t num_elements);
event_t async_work_group_copy (__local uchar *dst, const __global uchar *src, size_t num_elements, event_t event);
event_t async_work_group_copy (__global uchar *dst, const __local uchar *src, size_t num_elements, event_t event);
void prefetch (const __global uchar *p, size_t num_elements);
event_t async_work_group_copy (__local uchar2 *dst, const __global uchar2 *src, size_t num_elements, event_t event);
event_t async_work_group_copy (__global uchar2 *dst, const __local uchar2 *src, size_t num_elements, event_t event);
void prefetch (const __global uchar2 *p, size_t num_elements);
event_t async_work_group_copy (__local uchar4 *dst, const __global uchar4 *src, size_t num_elements, event_t event);
event_t async_work_group_copy (__global uchar4 *dst, const __local uchar4 *src, size_t num_elements, event_t event);
void prefetch (const __global uchar4 *p, size_t num_elements);
event_t async_work_group_copy (__local uchar8 *dst, const __global uchar8 *src, size_t num_elements, event_t event);
event_t async_work_group_copy (__global uchar8 *dst, const __local uchar8 *src, size_t num_elements, event_t event);
void prefetch (const __global uchar8 *p, size_t num_elements);
event_t async_work_group_copy (__local uchar16 *dst, const __global uchar16 *src, size_t num_elements, event_t event);
event_t async_work_group_copy (__global uchar16 *dst, const __local uchar16 *src, size_t num_elements, event_t event);
void prefetch (const __global uchar16 *p, size_t num_elements);
event_t async_work_group_copy (__local short *dst, const __global short *src, size_t num_elements, event_t event);
event_t async_work_group_copy (__global short *dst, const __local short *src, size_t num_elements, event_t event);
void prefetch (const __global short *p, size_t num_elements);
event_t async_work_group_copy (__local short2 *dst, const __global short2 *src, size_t num_elements, event_t event);
event_t async_work_group_copy (__global short2 *dst, const __local short2 *src, size_t num_elements, event_t event);
void prefetch (const __global short2 *p, size_t num_elements);
event_t async_work_group_copy (__local short4 *dst, const __global short4 *src, size_t num_elements, event_t event);
event_t async_work_group_copy (__global short4 *dst, const __local short4 *src, size_t num_elements, event_t event);
void prefetch (const __global short4 *p, size_t num_elements);
event_t async_work_group_copy (__local short8 *dst, const __global short8 *src, size_t num_elements, event_t event);
event_t async_work_group_copy (__global short8 *dst, const __local short8 *src, size_t num_elements, event_t event);
void prefetch (const __global short8 *p, size_t num_elements);
event_t async_work_group_copy (__local short16 *dst, const __global short16 *src, size_t num_elements, event_t event);
event_t async_work_group_copy (__global short16 *dst, const __local short16 *src, size_t num_elements, event_t event);
void prefetch (const __global short16 *p, size_t num_elements);
event_t async_work_group_copy (__local ushort *dst, const __global ushort *src, size_t num_elements, event_t event);
event_t async_work_group_copy (__global ushort *dst, const __local ushort *src, size_t num_elements, event_t event);
void prefetch (const __global ushort *p, size_t num_elements);
event_t async_work_group_copy (__local ushort2 *dst, const __global ushort2 *src, size_t num_elements, event_t event);
event_t async_work_group_copy (__global ushort2 *dst, const __local ushort2 *src, size_t num_elements, event_t event);
void prefetch (const __global ushort2 *p, size_t num_elements);
event_t async_work_group_copy (__local ushort4 *dst, const __global ushort4 *src, size_t num_elements, event_t event);
event_t async_work_group_copy (__global ushort4 *dst, const __local ushort4 *src, size_t num_elements, event_t event);
void prefetch (const __global ushort4 *p, size_t num_elements);
event_t async_work_group_copy (__local ushort8 *dst, const __global ushort8 *src, size_t num_elements, event_t event);
event_t async_work_group_copy (__global ushort8 *dst, const __local ushort8 *src, size_t num_elements, event_t event);
void prefetch (const __global ushort8 *p, size_t num_elements);
event_t async_work_group_copy (__local ushort16 *dst, const __global ushort16 *src, size_t num_elements, event_t event);
event_t async_work_group_copy (__global ushort16 *dst, const __local ushort16 *src, size_t num_elements, event_t event);
void prefetch (const __global ushort16 *p, size_t num_elements);
event_t async_work_group_copy (__local int *dst, const __global int *src, size_t num_elements, event_t event);
event_t async_work_group_copy (__global int *dst, const __local int *src, size_t num_elements, event_t event);
void prefetch (const __global int *p, size_t num_elements);
event_t async_work_group_copy (__local int2 *dst, const __global int2 *src, size_t num_elements, event_t event);
event_t async_work_group_copy (__global int2 *dst, const __local int2 *src, size_t num_elements, event_t event);
void prefetch (const __global int2 *p, size_t num_elements);
event_t async_work_group_copy (__local int4 *dst, const __global int4 *src, size_t num_elements, event_t event);
event_t async_work_group_copy (__global int4 *dst, const __local int4 *src, size_t num_elements, event_t event);
void prefetch (const __global int4 *p, size_t num_elements);
event_t async_work_group_copy (__local int8 *dst, const __global int8 *src, size_t num_elements, event_t event);
event_t async_work_group_copy (__global int8 *dst, const __local int8 *src, size_t num_elements, event_t event);
void prefetch (const __global int8 *p, size_t num_elements);
event_t async_work_group_copy (__local int16 *dst, const __global int16 *src, size_t num_elements, event_t event);
event_t async_work_group_copy (__global int16 *dst, const __local int16 *src, size_t num_elements, event_t event);
void prefetch (const __global int16 *p, size_t num_elements);
event_t async_work_group_copy (__local uint *dst, const __global uint *src, size_t num_elements, event_t event);
event_t async_work_group_copy (__global uint *dst, const __local uint *src, size_t num_elements, event_t event);
void prefetch (const __global uint *p, size_t num_elements);
event_t async_work_group_copy (__local uint2 *dst, const __global uint2 *src, size_t num_elements, event_t event);
event_t async_work_group_copy (__global uint2 *dst, const __local uint2 *src, size_t num_elements, event_t event);
void prefetch (const __global uint2 *p, size_t num_elements);
event_t async_work_group_copy (__local uint4 *dst, const __global uint4 *src, size_t num_elements, event_t event);
event_t async_work_group_copy (__global uint4 *dst, const __local uint4 *src, size_t num_elements, event_t event);
void prefetch (const __global uint4 *p, size_t num_elements);
event_t async_work_group_copy (__local uint8 *dst, const __global uint8 *src, size_t num_elements, event_t event);
event_t async_work_group_copy (__global uint8 *dst, const __local uint8 *src, size_t num_elements, event_t event);
void prefetch (const __global uint8 *p, size_t num_elements);
event_t async_work_group_copy (__local uint16 *dst, const __global uint16 *src, size_t num_elements, event_t event);
event_t async_work_group_copy (__global uint16 *dst, const __local uint16 *src, size_t num_elements, event_t event);
void prefetch (const __global uint16 *p, size_t num_elements);
event_t async_work_group_copy (__local long *dst, const __global long *src, size_t num_elements, event_t event);
event_t async_work_group_copy (__global long *dst, const __local long *src, size_t num_elements, event_t event);
void prefetch (const __global long *p, size_t num_elements);
event_t async_work_group_copy (__local long2 *dst, const __global long2 *src, size_t num_elements, event_t event);
event_t async_work_group_copy (__global long2 *dst, const __local long2 *src, size_t num_elements, event_t event);
void prefetch (const __global long2 *p, size_t num_elements);
event_t async_work_group_copy (__local long4 *dst, const __global long4 *src, size_t num_elements, event_t event);
event_t async_work_group_copy (__global long4 *dst, const __local long4 *src, size_t num_elements, event_t event);
void prefetch (const __global long4 *p, size_t num_elements);
event_t async_work_group_copy (__local long8 *dst, const __global long8 *src, size_t num_elements, event_t event);
event_t async_work_group_copy (__global long8 *dst, const __local long8 *src, size_t num_elements, event_t event);
void prefetch (const __global long8 *p, size_t num_elements);
event_t async_work_group_copy (__local long16 *dst, const __global long16 *src, size_t num_elements, event_t event);
event_t async_work_group_copy (__global long16 *dst, const __local long16 *src, size_t num_elements, event_t event);
void prefetch (const __global long16 *p, size_t num_elements);
event_t async_work_group_copy (__local ulong *dst, const __global ulong *src, size_t num_elements, event_t event);
event_t async_work_group_copy (__global ulong *dst, const __local ulong *src, size_t num_elements, event_t event);
void prefetch (const __global ulong *p, size_t num_elements);
event_t async_work_group_copy (__local ulong2 *dst, const __global ulong2 *src, size_t num_elements, event_t event);
event_t async_work_group_copy (__global ulong2 *dst, const __local ulong2 *src, size_t num_elements, event_t event);
void prefetch (const __global ulong2 *p, size_t num_elements);
event_t async_work_group_copy (__local ulong4 *dst, const __global ulong4 *src, size_t num_elements, event_t event);
event_t async_work_group_copy (__global ulong4 *dst, const __local ulong4 *src, size_t num_elements, event_t event);
void prefetch (const __global ulong4 *p, size_t num_elements);
event_t async_work_group_copy (__local ulong8 *dst, const __global ulong8 *src, size_t num_elements, event_t event);
event_t async_work_group_copy (__global ulong8 *dst, const __local ulong8 *src, size_t num_elements, event_t event);
void prefetch (const __global ulong8 *p, size_t num_elements);
event_t async_work_group_copy (__local ulong16 *dst, const __global ulong16 *src, size_t num_elements, event_t event);
event_t async_work_group_copy (__global ulong16 *dst, const __local ulong16 *src, size_t num_elements, event_t event);
void prefetch (const __global ulong16 *p, size_t num_elements);
event_t async_work_group_copy (__local float *dst, const __global float *src, size_t num_elements, event_t event);
event_t async_work_group_copy (__global float *dst, const __local float *src, size_t num_elements, event_t event);
void prefetch (const __global float *p, size_t num_elements);
event_t async_work_group_copy (__local float2 *dst, const __global float2 *src, size_t num_elements, event_t event);
event_t async_work_group_copy (__global float2 *dst, const __local float2 *src, size_t num_elements, event_t event);
void prefetch (const __global float2 *p, size_t num_elements);
event_t async_work_group_copy (__local float4 *dst, const __global float4 *src, size_t num_elements, event_t event);
event_t async_work_group_copy (__global float4 *dst, const __local float4 *src, size_t num_elements, event_t event);
void prefetch (const __global float4 *p, size_t num_elements);
event_t async_work_group_copy (__local float8 *dst, const __global float8 *src, size_t num_elements, event_t event);
event_t async_work_group_copy (__global float8 *dst, const __local float8 *src, size_t num_elements, event_t event);
void prefetch (const __global float8 *p, size_t num_elements);
event_t async_work_group_copy (__local float16 *dst, const __global float16 *src, size_t num_elements, event_t event);
event_t async_work_group_copy (__global float16 *dst, const __local float16 *src, size_t num_elements, event_t event);
void prefetch (const __global float16 *p, size_t num_elements);

  /* Miscellaneous Functions (TODO) */


