#ifndef H_KFG_TYPES
#define H_KFG_TYPES

enum edgetypes
{
    LOCAL,
    BB_INTERN,
    TRUE_EDGE,
    FALSE_EDGE,
    NORMAL_EDGE,
    CALL_EDGE,
    RETURN_EDGE,
    EDGE_TYPE_MAX
};

extern const char * const edgetypenames[];

enum nodetypes
{
    CALL,
    RETURN,
    START,
    END,
    INNER
};

typedef enum edgetypes KFG_EDGE_TYPE;

typedef void *KFG;
typedef void *KFG_NODE;
typedef void *KFG_NODE_LIST;

#if HAVE_PAG
    #include "genkfg.h"
#else
    typedef enum nodetypes KFG_NODE_TYPE;
    typedef int KFG_NODE_ID;
#endif

#endif
