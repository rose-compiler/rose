
typedef struct _HTStream HTStream;

struct _HTStream {};

typedef HTStream * HTConverter (void * param);

extern HTConverter HTThroughLine;

extern HTConverter HTBlackHoleConverter;

