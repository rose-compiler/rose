
// TV (02/27/2018): Added guards for the definition of CUDA's built-in types, variables, functions, and API
#ifdef ROSE_DEFINE_CUDA
  #define ROSE_DEFINE_CUDA_BUILTIN 1
  #define ROSE_DEFINE_CUDA_API 1
#endif
#ifdef ROSE_DEFINE_CUDA_BUILTIN
  #define ROSE_DEFINE_CUDA_BUILTIN_TYPES 1
  #define ROSE_DEFINE_CUDA_BUILTIN_VARIABLES 1
  #define ROSE_DEFINE_CUDA_BUILTIN_FUNCTIONS 1
  #define ROSE_DEFINE_CUDA_BUILTIN_TEXTURES 1
#endif

#include <vector_types.h>

#define ROSE_DEFINE_CUDA_BUILTIN_VARIABLES 1
//#define ROSE_DEFINE_GNU_BUILTIN_FOR_CUDA 1
#define ROSE_DEFINE_CUDA_BUILTIN_SYNC_FUNCTIONS 1

// TV (7/24/2015): including cstdlib causes:
//      rose/src/backend/unparser/nameQualificationSupport.C:5293:
//               virtual NameQualificationInheritedAttribute NameQualificationTraversal::evaluateInheritedAttribute(SgNode*, NameQualificationInheritedAttribute):
//               Assertion `currentStatement != __null' failed.
#if 1
typedef unsigned long size_t;
#else
#include <cstdlib>
#endif

/* CUDA Built-in Macros */

/* DQ (1/18/2016): Adding #define values from Cuda (required for Jeff's example code) */
#define cudaMemAttachGlobal 0x01


/* CUDA Built-in Types */

  /*
    Extract and adapted from:
      CUDA_PATH/include/common_functions.h
      CUDA_PATH/include/common_types.h
      CUDA_PATH/include/device_functions.h
      CUDA_PATH/include/device_types.h
      CUDA_PATH/include/sm_11_atomic_functions.h
      CUDA_PATH/include/sm_12_atomic_functions.h
      CUDA_PATH/include/sm_13_atomic_functions.h
      CUDA_PATH/include/sm_20_atomic_functions.h
      CUDA_PATH/include/sm_20_intrinsics.h
      CUDA_PATH/include/vector_types.h
      CUDA_PATH/include/vector_functions.h
  */

  /* Vector Types */

#ifdef ROSE_DEFINE_CUDA_VECTOR_TYPES

struct char1
{
  signed char x;
};

struct uchar1 
{
  unsigned char x;
};

struct /*__builtin_align__(2)*/ char2
{
  signed char x, y;
};

struct /*__builtin_align__(2)*/ uchar2
{
  unsigned char x, y;
};

struct char3
{
  signed char x, y, z;
};

struct uchar3
{
  unsigned char x, y, z;
};

struct /*__builtin_align__(4)*/ char4
{
  signed char x, y, z, w;
};

struct /*__builtin_align__(4)*/ uchar4
{
  unsigned char x, y, z, w;
};

struct short1
{
  short x;
};

struct ushort1
{
  unsigned short x;
};

struct /*__builtin_align__(4)*/ short2
{
  short x, y;
};

struct /*__builtin_align__(4)*/ ushort2
{
  unsigned short x, y;
};

struct short3
{
  short x, y, z;
};

struct ushort3
{
  unsigned short x, y, z;
};

struct /*__builtin_align__(8)*/ short4
{
  short x, y, z, w;
};

struct /*__builtin_align__(8)*/ ushort4
{
  unsigned short x, y, z, w;
};

struct int1
{
  int x;
};

struct uint1
{
  unsigned int x;
};

struct /*__builtin_align__(8)*/ int2
{
  int x, y;
};

struct /*__builtin_align__(8)*/ uint2
{
  unsigned int x, y;
};

struct int3
{
  int x, y, z;
};

struct uint3
{
  unsigned int x, y, z;
};

struct /*__builtin_align__(16)*/ int4
{
  int x, y, z, w;
};

struct /*__builtin_align__(16)*/ uint4
{
  unsigned int x, y, z, w;
};

struct long1
{
  long int x;
};

struct ulong1
{
  unsigned long x;
};

struct 
/*#if defined (_WIN32)
       __builtin_align__(8)
#else
       __builtin_align__(2*sizeof(long int))
#endif*/
                                             long2
{
  long int x, y;
};

struct 
/*#if defined (_WIN32)
       __builtin_align__(8)
#else
       __builtin_align__(2*sizeof(unsigned long int))
#endif*/
                                                      ulong2
{
  unsigned long int x, y;
};

#if !defined(__LP64__)

struct long3
{
  long int x, y, z;
};

struct ulong3
{
  unsigned long int x, y, z;
};

struct /*__builtin_align__(16)*/ long4
{
  long int x, y, z, w;
};

struct /*__builtin_align__(16)*/ ulong4
{
  unsigned long int x, y, z, w;
};

#endif /* !__LP64__ */

struct float1
{
  float x;
};

struct /*__builtin_align__(8)*/ float2
{
  float x, y;
};

struct float3
{
  float x, y, z;
};

struct /*__builtin_align__(16)*/ float4
{
  float x, y, z, w;
};

struct longlong1
{
  long long int x;
};

struct ulonglong1
{
  unsigned long long int x;
};

struct /*__builtin_align__(16)*/ longlong2
{
  long long int x, y;
};

struct /*__builtin_align__(16)*/ ulonglong2
{
  unsigned long long int x, y;
};

struct double1
{
  double x;
};

struct /*__builtin_align__(16)*/ double2
{
  double x, y;
};

typedef struct char1 char1;
typedef struct uchar1 uchar1;
typedef struct char2 char2;
typedef struct uchar2 uchar2;
typedef struct char3 char3;
typedef struct uchar3 uchar3;
typedef struct char4 char4;
typedef struct uchar4 uchar4;
typedef struct short1 short1;
typedef struct ushort1 ushort1;
typedef struct short2 short2;
typedef struct ushort2 ushort2;
typedef struct short3 short3;
typedef struct ushort3 ushort3;
typedef struct short4 short4;
typedef struct ushort4 ushort4;
typedef struct int1 int1;
typedef struct uint1 uint1;
typedef struct int2 int2;
typedef struct uint2 uint2;
typedef struct int3 int3;
typedef struct uint3 uint3;
typedef struct int4 int4;
typedef struct uint4 uint4;
typedef struct long1 long1;
typedef struct ulong1 ulong1;
typedef struct long2 long2;
typedef struct ulong2 ulong2;
typedef struct long3 long3;
typedef struct ulong3 ulong3;
typedef struct long4 long4;
typedef struct ulong4 ulong4;
typedef struct float1 float1;
typedef struct float2 float2;
typedef struct float3 float3;
typedef struct float4 float4;
typedef struct longlong1 longlong1;
typedef struct ulonglong1 ulonglong1;
typedef struct longlong2 longlong2;
typedef struct ulonglong2 ulonglong2;
typedef struct double1 double1;
typedef struct double2 double2;

#endif /* ROSE_DEFINE_CUDA_VECTOR_TYPES */

#ifdef ROSE_DEFINE_CUDA_BUILTIN_TYPES

typedef struct dim3 dim3;

struct dim3
{
    unsigned int x, y, z;
#if defined(__cplusplus)
    dim3(unsigned int x = 1, unsigned int y = 1, unsigned int z = 1) : x(x), y(y), z(z) {}
    dim3(uint3 v) : x(v.x), y(v.y), z(v.z) {}
    operator uint3(void) { uint3 t; t.x = x; t.y = y; t.z = z; return t; }
#endif
};

#endif /* ROSE_DEFINE_CUDA_BUILTIN_TYPES */

#ifdef ROSE_DEFINE_CUDA_API_TYPES

enum cudaError
{
  cudaSuccess = 0,
  cudaErrorMissingConfiguration,
  cudaErrorMemoryAllocation,
  cudaErrorInitializationError,
  cudaErrorLaunchFailure,
  cudaErrorPriorLaunchFailure,
  cudaErrorLaunchTimeout,
  cudaErrorLaunchOutOfResources,
  cudaErrorInvalidDeviceFunction,
  cudaErrorInvalidConfiguration,
  cudaErrorInvalidDevice,
  cudaErrorInvalidValue,
  cudaErrorInvalidPitchValue,
  cudaErrorInvalidSymbol,
  cudaErrorMapBufferObjectFailed,
  cudaErrorUnmapBufferObjectFailed,
  cudaErrorInvalidHostPointer,
  cudaErrorInvalidDevicePointer,
  cudaErrorInvalidTexture,
  cudaErrorInvalidTextureBinding,
  cudaErrorInvalidChannelDescriptor,
  cudaErrorInvalidMemcpyDirection,
  cudaErrorAddressOfConstant,
  cudaErrorTextureFetchFailed,
  cudaErrorTextureNotBound,
  cudaErrorSynchronizationError,
  cudaErrorInvalidFilterSetting,
  cudaErrorInvalidNormSetting,
  cudaErrorMixedDeviceExecution,
  cudaErrorCudartUnloading,
  cudaErrorUnknown,
  cudaErrorNotYetImplemented,
  cudaErrorMemoryValueTooLarge,
  cudaErrorInvalidResourceHandle,
  cudaErrorNotReady,
  cudaErrorInsufficientDriver,
  cudaErrorSetOnActiveProcess,
  cudaErrorStartupFailure = 0x7f,
  cudaErrorApiFailureBase = 10000
};

enum cudaChannelFormatKind
{
  cudaChannelFormatKindSigned,
  cudaChannelFormatKindUnsigned,
  cudaChannelFormatKindFloat,
  cudaChannelFormatKindNone
};

struct cudaChannelFormatDesc
{
  int                        x;
  int                        y;
  int                        z;
  int                        w;
  enum cudaChannelFormatKind f;
};

struct cudaArray;

enum cudaMemcpyKind
{
  cudaMemcpyHostToHost = 0,
  cudaMemcpyHostToDevice,
  cudaMemcpyDeviceToHost,
  cudaMemcpyDeviceToDevice
};

struct cudaPitchedPtr
{
  void   *ptr;
  size_t  pitch;
  size_t  xsize;
  size_t  ysize;
};

struct cudaExtent
{
  size_t width;
  size_t height;
  size_t depth;
};

struct cudaPos
{
  size_t x;
  size_t y;
  size_t z;
};

struct cudaMemcpy3DParms
{
  struct cudaArray      *srcArray;
  struct cudaPos         srcPos;
  struct cudaPitchedPtr  srcPtr;

  struct cudaArray      *dstArray;
  struct cudaPos         dstPos;
  struct cudaPitchedPtr  dstPtr;

  struct cudaExtent      extent;
  enum cudaMemcpyKind    kind;
};

struct cudaDeviceProp
{
  char   name[256];
  size_t totalGlobalMem;
  size_t sharedMemPerBlock;
  int    regsPerBlock;
  int    warpSize;
  size_t memPitch;
  int    maxThreadsPerBlock;
  int    maxThreadsDim[3];
  int    maxGridSize[3]; 
  int    clockRate;
  size_t totalConstMem; 
  int    major;
  int    minor;
  size_t textureAlignment;
  int    deviceOverlap;
  int    multiProcessorCount;
  int    kernelExecTimeoutEnabled;
  int    __cudaReserved[39];
};

#define cudaDevicePropDontCare                             \
        {                                                  \
          {'\0'},    /* char   name[256];               */ \
          0,         /* size_t totalGlobalMem;          */ \
          0,         /* size_t sharedMemPerBlock;       */ \
          0,         /* int    regsPerBlock;            */ \
          0,         /* int    warpSize;                */ \
          0,         /* size_t memPitch;                */ \
          0,         /* int    maxThreadsPerBlock;      */ \
          {0, 0, 0}, /* int    maxThreadsDim[3];        */ \
          {0, 0, 0}, /* int    maxGridSize[3];          */ \
          0,         /* int    clockRate;               */ \
          0,         /* size_t totalConstMem;           */ \
          -1,        /* int    major;                   */ \
          -1,        /* int    minor;                   */ \
          0,         /* size_t textureAlignment;        */ \
          -1,        /* int    deviceOverlap;           */ \
          0,         /* int    multiProcessorCount;     */ \
          0          /* int    kernelExecTimeoutEnabled */ \
        }

typedef enum cudaError cudaError_t;

typedef int cudaStream_t;

typedef int cudaEvent_t;

enum cudaTextureAddressMode
{
  cudaAddressModeWrap,
  cudaAddressModeClamp
};

enum cudaTextureFilterMode
{
  cudaFilterModePoint,
  cudaFilterModeLinear
};

enum cudaTextureReadMode
{
  cudaReadModeElementType,
  cudaReadModeNormalizedFloat
};

struct textureReference
{
  int                          normalized;
  enum cudaTextureFilterMode   filterMode;
  enum cudaTextureAddressMode  addressMode[3];
  struct cudaChannelFormatDesc channelDesc;
  int                          __cudaReserved[16];
};

#endif /* ROSE_DEFINE_CUDA_API_TYPES */

#ifdef ROSE_DEFINE_CUDA_BUILTIN_VARIABLES

/* CUDA Built-in Variables */

dim3  gridDim;
uint3 blockIdx;
dim3  blockDim;
uint3 threadIdx;
int   warpSize;

#endif /* ROSE_DEFINE_CUDA_BUILTIN_VARIABLES */

#ifdef ROSE_DEFINE_CUDA_BUILTIN_VECTOR_FUNCTIONS

/* CUDA Built-in Functions */

  /* Vector Functions (constructors) */
  
static __inline__ __host__ __device__ char1 make_char1(signed char x);
static __inline__ __host__ __device__ uchar1 make_uchar1(unsigned char x);
static __inline__ __host__ __device__ char2 make_char2(signed char x, signed char y);
static __inline__ __host__ __device__ uchar2 make_uchar2(unsigned char x, unsigned char y);
static __inline__ __host__ __device__ char3 make_char3(signed char x, signed char y, signed char z);
static __inline__ __host__ __device__ uchar3 make_uchar3(unsigned char x, unsigned char y, unsigned char z);
static __inline__ __host__ __device__ char4 make_char4(signed char x, signed char y, signed char z, signed char w);
static __inline__ __host__ __device__ uchar4 make_uchar4(unsigned char x, unsigned char y, unsigned char z, unsigned char w);
static __inline__ __host__ __device__ short1 make_short1(short x);
static __inline__ __host__ __device__ ushort1 make_ushort1(unsigned short x);
static __inline__ __host__ __device__ short2 make_short2(short x, short y);
static __inline__ __host__ __device__ ushort2 make_ushort2(unsigned short x, unsigned short y);
static __inline__ __host__ __device__ short3 make_short3(short x,short y, short z);
static __inline__ __host__ __device__ ushort3 make_ushort3(unsigned short x, unsigned short y, unsigned short z);
static __inline__ __host__ __device__ short4 make_short4(short x, short y, short z, short w);
static __inline__ __host__ __device__ ushort4 make_ushort4(unsigned short x, unsigned short y, unsigned short z, unsigned short w);
static __inline__ __host__ __device__ int1 make_int1(int x);
static __inline__ __host__ __device__ uint1 make_uint1(unsigned int x);
static __inline__ __host__ __device__ int2 make_int2(int x, int y);
static __inline__ __host__ __device__ uint2 make_uint2(unsigned int x, unsigned int y);
static __inline__ __host__ __device__ int3 make_int3(int x, int y, int z);
static __inline__ __host__ __device__ uint3 make_uint3(unsigned int x, unsigned int y, unsigned int z);
static __inline__ __host__ __device__ int4 make_int4(int x, int y, int z, int w);
static __inline__ __host__ __device__ uint4 make_uint4(unsigned int x, unsigned int y, unsigned int z, unsigned int w);
static __inline__ __host__ __device__ long1 make_long1(long int x);
static __inline__ __host__ __device__ ulong1 make_ulong1(unsigned long int x);
static __inline__ __host__ __device__ long2 make_long2(long int x, long int y);
static __inline__ __host__ __device__ ulong2 make_ulong2(unsigned long int x, unsigned long int y);
static __inline__ __host__ __device__ long3 make_long3(long int x, long int y, long int z);
static __inline__ __host__ __device__ ulong3 make_ulong3(unsigned long int x, unsigned long int y, unsigned long int z);
static __inline__ __host__ __device__ long4 make_long4(long int x, long int y, long int z, long int w);
static __inline__ __host__ __device__ ulong4 make_ulong4(unsigned long int x, unsigned long int y, unsigned long int z, unsigned long int w);
static __inline__ __host__ __device__ float1 make_float1(float x);
static __inline__ __host__ __device__ float2 make_float2(float x, float y);
static __inline__ __host__ __device__ float3 make_float3(float x, float y, float z);
static __inline__ __host__ __device__ float4 make_float4(float x, float y, float z, float w);
static __inline__ __host__ __device__ longlong1 make_longlong1(long long int x);
static __inline__ __host__ __device__ ulonglong1 make_ulonglong1(unsigned long long int x);
static __inline__ __host__ __device__ longlong2 make_longlong2(long long int x, long long int y);
static __inline__ __host__ __device__ ulonglong2 make_ulonglong2(unsigned long long int x, unsigned long long int y);
static __inline__ __host__ __device__ double1 make_double1(double x);
static __inline__ __host__ __device__ double2 make_double2(double x, double y);
  
#endif /* ROSE_DEFINE_CUDA_BUILTIN_VECTOR_FUNCTIONS */

#ifdef ROSE_DEFINE_CUDA_BUILTIN_SYNC_FUNCTIONS

  /* Synchronization functions */

__device__ void __threadfence_block();
__device__ void __threadfence();
__device__ void __threadfence_system();
__device__ void __syncthreads();
__device__ int  __syncthreads_count(int predicate);
__device__ int  __syncthreads_and(int predicate);
__device__ int  __syncthreads_or(int predicate);
  
#endif /* ROSE_DEFINE_CUDA_BUILTIN_SYNC_FUNCTIONS */

  /* Time function */

#ifdef ROSE_DEFINE_CUDA_BUILTIN_CLOCK_FUNCTIONS
  
//__device__ clock_t clock();
  
#endif /* ROSE_DEFINE_CUDA_BUILTIN_CLOCK_FUNCTIONS */

  /* Atomic functions */

#ifdef ROSE_DEFINE_CUDA_BUILTIN_ATOMIC_FUNCTIONS

static __inline__ __device__ int atomicAdd(int *address, int val);
static __inline__ __device__ unsigned int atomicAdd(unsigned int *address, unsigned int val);
static __inline__ __device__ unsigned long long int atomicAdd(unsigned long long int *address, unsigned long long int val);
static __inline__ __device__ float atomicAdd(float *address, float val);
static __inline__ __device__ int atomicSub(int *address, int val);
static __inline__ __device__ unsigned int atomicSub(unsigned int *address, unsigned int val);
static __inline__ __device__ int atomicExch(int *address, int val);
static __inline__ __device__ unsigned int atomicExch(unsigned int *address, unsigned int val);
static __inline__ __device__ unsigned long long int atomicExch(unsigned long long int *address, unsigned long long int val);
static __inline__ __device__ float atomicExch(float *address, float val);
static __inline__ __device__ int atomicMin(int *address, int val);
static __inline__ __device__ unsigned int atomicMin(unsigned int *address, unsigned int val);
static __inline__ __device__ int atomicMax(int *address, int val);
static __inline__ __device__ unsigned int atomicMax(unsigned int *address, unsigned int val);
static __inline__ __device__ unsigned int atomicInc(unsigned int *address, unsigned int val);
static __inline__ __device__ unsigned int atomicDec(unsigned int *address, unsigned int val);
static __inline__ __device__ int atomicCAS(int *address, int compare, int val);
static __inline__ __device__ unsigned int atomicCAS(unsigned int *address, unsigned int compare, unsigned int val);
static __inline__ __device__ unsigned long long int atomicCAS(unsigned long long int *address, unsigned long long int compare, unsigned long long int val);
static __inline__ __device__ int atomicAnd(int *address, int val);
static __inline__ __device__ unsigned int atomicAnd(unsigned int *address, unsigned int val);
static __inline__ __device__ int atomicOr(int *address, int val);
static __inline__ __device__ unsigned int atomicOr(unsigned int *address, unsigned int val);
static __inline__ __device__ int atomicXor(int *address, int val);
static __inline__ __device__ unsigned int atomicXor(unsigned int *address, unsigned int val);

#endif /* ROSE_DEFINE_CUDA_BUILTIN_ATOMIC_FUNCTIONS */

  /* Warp Vote functions */
  
#ifdef ROSE_DEFINE_CUDA_BUILTIN_VOTE_FUNCTIONS

__device__ int __all(int cond);
__device__ int __any(int cond);
__device__ unsigned int __ballot(int);

#endif /* ROSE_DEFINE_CUDA_BUILTIN_VOTE_FUNCTIONS */

  /* Profiler Counter functions */

#ifdef ROSE_DEFINE_CUDA_BUILTIN_PROFILE_FUNCTIONS

__device__ void __prof_trigger(int);

#endif /* ROSE_DEFINE_CUDA_BUILTIN_PROFILE_FUNCTIONS */

  /* Mathematical functions (TODO-CUDA) */

#ifdef ROSE_DEFINE_CUDA_BUILTIN_MATH_FUNCTIONS
#endif /* ROSE_DEFINE_CUDA_BUILTIN_MATH_FUNCTIONS */

  /* Texture functions (TODO-CUDA) */

#ifdef ROSE_DEFINE_CUDA_BUILTIN_TEXTURE_FUNCTIONS
#endif /* ROSE_DEFINE_CUDA_BUILTIN_TEXTURE_FUNCTIONS */

/* CUDA API (TODO-CUDA) */

#ifdef ROSE_DEFINE_CUDA_API

#if !defined(__dv)

#if defined(__cplusplus)

#define __dv(v) \
        = v

#else /* __cplusplus */

#define __dv(v)

#endif /* __cplusplus */

#endif /* !__dv */

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

/*******************************************************************************
*                                                                              *
*                                                                              *
*                                                                              *
*******************************************************************************/

extern __host__ cudaError_t cudaMalloc3D(struct cudaPitchedPtr* pitchDevPtr, struct cudaExtent extent);
extern __host__ cudaError_t cudaMalloc3DArray(struct cudaArray** arrayPtr, const struct cudaChannelFormatDesc* desc, struct cudaExtent extent);
extern __host__ cudaError_t cudaMemset3D(struct cudaPitchedPtr pitchDevPtr, int value, struct cudaExtent extent);
extern __host__ cudaError_t cudaMemcpy3D(const struct cudaMemcpy3DParms *p);
extern __host__ cudaError_t cudaMemcpy3DAsync(const struct cudaMemcpy3DParms *p, cudaStream_t stream);


/*******************************************************************************
*                                                                              *
*                                                                              *
*                                                                              *
*******************************************************************************/

extern __host__ cudaError_t cudaMalloc(void **devPtr, size_t size);
extern __host__ cudaError_t cudaMallocHost(void **ptr, size_t size);
extern __host__ cudaError_t cudaMallocPitch(void **devPtr, size_t *pitch, size_t width, size_t height);
extern __host__ cudaError_t cudaMallocArray(struct cudaArray **array, const struct cudaChannelFormatDesc *desc, size_t width, size_t height __dv(1));
extern __host__ cudaError_t cudaFree(void *devPtr);
extern __host__ cudaError_t cudaFreeHost(void *ptr);
extern __host__ cudaError_t cudaFreeArray(struct cudaArray *array);


/*******************************************************************************
*                                                                              *
*                                                                              *
*                                                                              *
*******************************************************************************/

extern __host__ cudaError_t cudaMemcpy(void *dst, const void *src, size_t count, enum cudaMemcpyKind kind);
extern __host__ cudaError_t cudaMemcpyToArray(struct cudaArray *dst, size_t wOffset, size_t hOffset, const void *src, size_t count, enum cudaMemcpyKind kind);
extern __host__ cudaError_t cudaMemcpyFromArray(void *dst, const struct cudaArray *src, size_t wOffset, size_t hOffset, size_t count, enum cudaMemcpyKind kind);
extern __host__ cudaError_t cudaMemcpyArrayToArray(struct cudaArray *dst, size_t wOffsetDst, size_t hOffsetDst, const struct cudaArray *src, size_t wOffsetSrc, size_t hOffsetSrc, size_t count, enum cudaMemcpyKind kind __dv(cudaMemcpyDeviceToDevice));
extern __host__ cudaError_t cudaMemcpy2D(void *dst, size_t dpitch, const void *src, size_t spitch, size_t width, size_t height, enum cudaMemcpyKind kind);
extern __host__ cudaError_t cudaMemcpy2DToArray(struct cudaArray *dst, size_t wOffset, size_t hOffset, const void *src, size_t spitch, size_t width, size_t height, enum cudaMemcpyKind kind);
extern __host__ cudaError_t cudaMemcpy2DFromArray(void *dst, size_t dpitch, const struct cudaArray *src, size_t wOffset, size_t hOffset, size_t width, size_t height, enum cudaMemcpyKind kind);
extern __host__ cudaError_t cudaMemcpy2DArrayToArray(struct cudaArray *dst, size_t wOffsetDst, size_t hOffsetDst, const struct cudaArray *src, size_t wOffsetSrc, size_t hOffsetSrc, size_t width, size_t height, enum cudaMemcpyKind kind __dv(cudaMemcpyDeviceToDevice));
extern __host__ cudaError_t cudaMemcpyToSymbol(const char *symbol, const void *src, size_t count, size_t offset __dv(0), enum cudaMemcpyKind kind __dv(cudaMemcpyHostToDevice));
extern __host__ cudaError_t cudaMemcpyFromSymbol(void *dst, const char *symbol, size_t count, size_t offset __dv(0), enum cudaMemcpyKind kind __dv(cudaMemcpyDeviceToHost));

/*******************************************************************************
*                                                                              *
*                                                                              *
*                                                                              *
*******************************************************************************/

extern __host__ cudaError_t cudaMemcpyAsync(void *dst, const void *src, size_t count, enum cudaMemcpyKind kind, cudaStream_t stream);
extern __host__ cudaError_t cudaMemcpyToArrayAsync(struct cudaArray *dst, size_t wOffset, size_t hOffset, const void *src, size_t count, enum cudaMemcpyKind kind, cudaStream_t stream);
extern __host__ cudaError_t cudaMemcpyFromArrayAsync(void *dst, const struct cudaArray *src, size_t wOffset, size_t hOffset, size_t count, enum cudaMemcpyKind kind, cudaStream_t stream);
extern __host__ cudaError_t cudaMemcpy2DAsync(void *dst, size_t dpitch, const void *src, size_t spitch, size_t width, size_t height, enum cudaMemcpyKind kind, cudaStream_t stream);
extern __host__ cudaError_t cudaMemcpy2DToArrayAsync(struct cudaArray *dst, size_t wOffset, size_t hOffset, const void *src, size_t spitch, size_t width, size_t height, enum cudaMemcpyKind kind, cudaStream_t stream);
extern __host__ cudaError_t cudaMemcpy2DFromArrayAsync(void *dst, size_t dpitch, const struct cudaArray *src, size_t wOffset, size_t hOffset, size_t width, size_t height, enum cudaMemcpyKind kind, cudaStream_t stream);
extern __host__ cudaError_t cudaMemcpyToSymbolAsync(const char *symbol, const void *src, size_t count, size_t offset, enum cudaMemcpyKind kind, cudaStream_t stream);
extern __host__ cudaError_t cudaMemcpyFromSymbolAsync(void *dst, const char *symbol, size_t count, size_t offset, enum cudaMemcpyKind kind, cudaStream_t stream);

/*******************************************************************************
*                                                                              *
*                                                                              *
*                                                                              *
*******************************************************************************/

extern __host__ cudaError_t cudaMemset(void *mem, int c, size_t count);
extern __host__ cudaError_t cudaMemset2D(void *mem, size_t pitch, int c, size_t width, size_t height);

/*******************************************************************************
*                                                                              *
*                                                                              *
*                                                                              *
*******************************************************************************/

extern __host__ cudaError_t cudaGetSymbolAddress(void **devPtr, const char *symbol);
extern __host__ cudaError_t cudaGetSymbolSize(size_t *size, const char *symbol);

/*******************************************************************************
*                                                                              *
*                                                                              *
*                                                                              *
*******************************************************************************/

extern __host__ cudaError_t cudaGetDeviceCount(int *count);
extern __host__ cudaError_t cudaGetDeviceProperties(struct cudaDeviceProp *prop, int device);
extern __host__ cudaError_t cudaChooseDevice(int *device, const struct cudaDeviceProp *prop);
extern __host__ cudaError_t cudaSetDevice(int device);
extern __host__ cudaError_t cudaGetDevice(int *device);

/*******************************************************************************
*                                                                              *
*                                                                              *
*                                                                              *
*******************************************************************************/

extern __host__ cudaError_t cudaBindTexture(size_t *offset, const struct textureReference *texref, const void *devPtr, const struct cudaChannelFormatDesc *desc, size_t size /*__dv(UINT_MAX)*/);
extern __host__ cudaError_t cudaBindTextureToArray(const struct textureReference *texref, const struct cudaArray *array, const struct cudaChannelFormatDesc *desc);
extern __host__ cudaError_t cudaUnbindTexture(const struct textureReference *texref);
extern __host__ cudaError_t cudaGetTextureAlignmentOffset(size_t *offset, const struct textureReference *texref);
extern __host__ cudaError_t cudaGetTextureReference(const struct textureReference **texref, const char *symbol);

/*******************************************************************************
*                                                                              *
*                                                                              *
*                                                                              *
*******************************************************************************/

extern __host__ cudaError_t cudaGetChannelDesc(struct cudaChannelFormatDesc *desc, const struct cudaArray *array);
extern __host__ struct cudaChannelFormatDesc cudaCreateChannelDesc(int x, int y, int z, int w, enum cudaChannelFormatKind f);

/*******************************************************************************
*                                                                              *
*                                                                              *
*                                                                              *
*******************************************************************************/

extern __host__ cudaError_t cudaGetLastError(void);
extern __host__ const char* cudaGetErrorString(cudaError_t error);

/*******************************************************************************
*                                                                              *
*                                                                              *
*                                                                              *
*******************************************************************************/

extern __host__ cudaError_t cudaConfigureCall(dim3 gridDim, dim3 blockDim, size_t sharedMem __dv(0), cudaStream_t stream __dv(0));
extern __host__ cudaError_t cudaSetupArgument(const void *arg, size_t size, size_t offset);
extern __host__ cudaError_t cudaLaunch(const char *symbol);

/*******************************************************************************
*                                                                              *
*                                                                              *
*                                                                              *
*******************************************************************************/

extern __host__ cudaError_t cudaStreamCreate(cudaStream_t *stream);
extern __host__ cudaError_t cudaStreamDestroy(cudaStream_t stream);
extern __host__ cudaError_t cudaStreamSynchronize(cudaStream_t stream);
extern __host__ cudaError_t cudaStreamQuery(cudaStream_t stream);

/*******************************************************************************
*                                                                              *
*                                                                              *
*                                                                              *
*******************************************************************************/

extern __host__ cudaError_t cudaEventCreate(cudaEvent_t *event);
extern __host__ cudaError_t cudaEventRecord(cudaEvent_t event, cudaStream_t stream);
extern __host__ cudaError_t cudaEventQuery(cudaEvent_t event);
extern __host__ cudaError_t cudaEventSynchronize(cudaEvent_t event);
extern __host__ cudaError_t cudaEventDestroy(cudaEvent_t event);
extern __host__ cudaError_t cudaEventElapsedTime(float *ms, cudaEvent_t start, cudaEvent_t end);

/*******************************************************************************
*                                                                              *
*                                                                              *
*                                                                              *
*******************************************************************************/

extern __host__ cudaError_t cudaSetDoubleForDevice(double *d);
extern __host__ cudaError_t cudaSetDoubleForHost(double *d);

/*******************************************************************************
*                                                                              *
*                                                                              *
*                                                                              *
*******************************************************************************/

extern __host__ cudaError_t cudaThreadExit(void);
extern __host__ cudaError_t cudaThreadSynchronize(void);

/* DQ (1/18/2016): Adding functions that appear to be specific to Cuda 7.0 (required for Jeff's example code) */
cudaError_t cudaMallocManaged(void **devPtr, size_t size, unsigned int flags=0);
cudaError_t cudaDeviceSynchronize();

#if defined(__cplusplus)
}
#endif /* __cplusplus */

#endif /* ROSE_DEFINE_CUDA_API */

#if ROSE_DEFINE_GNU_BUILTIN_FOR_CUDA

/* DQ (3/1/2017): Moved this to be outside of the extern "C" declarations above (required to 
   support possible multiple overloaded declarations of __builtin_bswap32).
   DQ (2/28/2017): Added builtin function to support CUDA code (this declaration is required 
   to compile cu test file test_2011_13.cu on gcc-4.8-c++11 ubuntu1404 platform).
 */

// int __builtin_bswap32 (int x);
// int32_t __builtin_bswap32 (int32_t x);
#ifdef __INT32_TYPE__
__INT32_TYPE__ __builtin_bswap32 (__INT32_TYPE__ x);
#else
int __builtin_bswap32 (int x);
#endif



#endif /* ROSE_DEFINE_GNU_BUILTIN_FOR_CUDA */

#if ROSE_DEFINE_CUDA_BUILTIN_TEXTURES
// \pp added preliminry support for textures (incomplete)
#include "texture_types.h"

#if defined(__cplusplus)

// \todo to be completed..
// is there a header for the C++ variants?
template <class DataType, int, cudaTextureReadMode> 
struct texture : textureReference
{};

template <class DataType>
DataType
tex1Dfetch(texture <DataType, cudaTextureType1D, cudaReadModeElementType> texRef, int x ) ;  

template<class T>
cudaChannelFormatDesc cudaCreateChannelDesc();  	

template<class T, int dim, cudaTextureReadMode readMode>
cudaError_t 
cudaBindTexture( size_t* offset, 
                 const texture< T, dim, readMode>& tex,
                 const void*                       devPtr,
                 const cudaChannelFormatDesc&  	   desc,
                 size_t                            size = UINT_MAX
               ); 	

template<class T, int dim, cudaTextureReadMode readMode>
cudaError_t 
cudaBindTexture( size_t*                          offset, 
                 const texture<T, dim, readMode>& tex,
                 const void*                      devPtr,
                 size_t                           size = UINT_MAX
               );
#endif /* __cplusplus */

#endif /* ROSE_DEFINE_CUDA_BUILTIN_TEXTURES */

#ifdef __host__
#  undef __host__
#endif

#ifdef __device__
# undef __device__
#endif

#ifdef __global__
#  undef __global__
#endif

#ifdef __shared__
# undef __shared__
#endif

#ifdef __constant__
# undef __constant__
#endif

#ifdef __managed__
# undef __managed__
#endif

