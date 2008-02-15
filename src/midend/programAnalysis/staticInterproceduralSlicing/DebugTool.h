#ifdef DEBUG
#define VERBOSE_DEBUG_BLOCK(x) {x}
#define VERBOSE_DEBUG_STMT(x) x
#else
#define VERBOSE_DEBUG_BLOCK(x) 
#define VERBOSE_DEBUG_STMT(x)
#endif
