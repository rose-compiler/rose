// test __builtin_constant_p
typedef struct {
	volatile unsigned int lock;
} rwlock_t;
extern inline void read_lock(rwlock_t *rw)
{
  if (__builtin_constant_p( rw )) {
  }
}
