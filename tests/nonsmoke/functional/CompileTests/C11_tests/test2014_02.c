// Example C11 code: simple example to start things off (testing command line option: -rose:c11)

// Note that this feature requires EDG 4.8 or greater:
#include<stdalign.h>

// DQ (1/18/2017): Fixed position of alignment keyword.
// every object of type sse_t will be aligned to 16-byte boundary
// struct alignas(16) sse_tag
// struct _Alignas(16) sse_t
struct sse_tag
{
  float sse_data[4];
} alignas(16) sse_t;
 
// the array "cacheline" will be aligned to 128-byte boundary
alignas(128) char cacheline[128];
// _Alignas(128) char cacheline[128];

