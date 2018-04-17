
#if 0
/* This is what will be allocated: */
typedef union alloc_block_t alloc_block_t;
union alloc_block_t
{
  int index_node;
  int list_node;

  /* Links free blocks into a freelist. */
  alloc_block_t *next_free;
};

/* C89 does not allow C++ style comments */
// static inline void * foo(int *pool, alloc_block_t **free_list)
#endif

static inline void * foo()
   {
     return 0L;
   }
