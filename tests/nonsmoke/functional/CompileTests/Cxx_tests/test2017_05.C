// MySQL header that is mising some builtins in ROSE.

typedef int int32;
typedef long long int64;

// typedef long int64;
// typedef __INT32_TYPE__ int32;
// typedef __INT64_TYPE__ int64;

static inline int my_atomic_cas32(int32 volatile *a, int32 *cmp, int32 set)
{
  int32 cmp_val= *cmp;
  int32 sav= __sync_val_compare_and_swap(a, cmp_val, set);
  int ret= (sav == cmp_val);
  if (!ret)
    *cmp = sav;
  return ret;
}

static inline int my_atomic_cas64(int64 volatile *a, int64 *cmp, int64 set)
{
  int64 cmp_val= *cmp;
  int64 sav= __sync_val_compare_and_swap(a, cmp_val, set);
  int ret= (sav == cmp_val);
  if (!ret)
    *cmp = sav;
  return ret;
}

static inline int my_atomic_casptr(void * volatile *a, void **cmp, void *set)
{
  void *cmp_val= *cmp;
  void *sav= __sync_val_compare_and_swap(a, cmp_val, set);
  int ret= (sav == cmp_val);
  if (!ret)
    *cmp = sav;
  return ret;
}

static inline int32 my_atomic_add32(int32 volatile *a, int32 v)
{
  return __sync_fetch_and_add(a, v);
}

static inline int64 my_atomic_add64(int64 volatile *a, int64 v)
{
  return __sync_fetch_and_add(a, v);
}

static inline int32 my_atomic_fas32(int32 volatile *a, int32 v)
{
  return __sync_lock_test_and_set(a, v);
}

static inline int64 my_atomic_fas64(int64 volatile *a, int64 v)
{
  return __sync_lock_test_and_set(a, v);
}

static inline void * my_atomic_fasptr(void * volatile *a, void * v)
{
  return __sync_lock_test_and_set(a, v);
}

static inline int32 my_atomic_load32(int32 volatile *a)
{
  return __sync_fetch_and_or(a, 0);
}

static inline int64 my_atomic_load64(int64 volatile *a)
{
  return __sync_fetch_and_or(a, 0);
}

static inline void* my_atomic_loadptr(void * volatile *a)
{
  return __sync_fetch_and_or(a, 0);
}

static inline void my_atomic_store32(int32 volatile *a, int32 v)
{
  (void) __sync_lock_test_and_set(a, v);
}

static inline void my_atomic_store64(int64 volatile *a, int64 v)
{
  (void) __sync_lock_test_and_set(a, v);
}

static inline void my_atomic_storeptr(void * volatile *a, void *v)
{
  (void) __sync_lock_test_and_set(a, v);
}
