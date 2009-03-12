#ifndef H_CONTEXT_INFO
#define H_CONTEXT_INFO

#if HAVE_PAG
#include "global.h"
#endif

#ifdef __cplusplus

#define EXTERN_C extern "C"

#include <string>

// This class wraps one of PAG's interprocedural contexts, for use within a
// PAG analysis. Essentially, it is meant to be passed around by the
// analysis, but nothing more. This merely contains the current procedure
// and the "position" within that procedure.
class ContextInfo
{
public:
    int procnum;
    int position;

    std::string print() const;

    static int type_id;

    ContextInfo(int procnum, int position);

private:
    ContextInfo();
};

#else
/* C part */

#define EXTERN_C

#endif

/* common part */
#if HAVE_PAG
typedef void *o_ContextInfo;

#define o_ContextInfo_mark_self(x)  \
    ((x) == NULL ? (void) 0 : GC_mark((void **) &x))
#define o_ContextInfo_mcopy_self(x) \
    ((x) == NULL ? (x) : (o_ContextInfo) (GC_share_copy((void *) x)))

EXTERN_C void o_ContextInfo_mark(void *);
EXTERN_C void o_ContextInfo_mcopy(void *src, void *dst);
EXTERN_C FLO_BOOL o_ContextInfo_eq(void *p, void *q);
EXTERN_C FLO_BOOL o_ContextInfo_neq(void *p, void *q);
EXTERN_C unsigned int o_ContextInfo_hash(void *p);
EXTERN_C void o_ContextInfo_init(void);
EXTERN_C void *o_ContextInfo_duplicate(void *src);
EXTERN_C void o_ContextInfo_find_obj(void);
EXTERN_C void o_ContextInfo_copy_obj(void);
EXTERN_C void o_ContextInfo_clear_flag(void);
EXTERN_C char *o_ContextInfo_print(void *p);
EXTERN_C void o_ContextInfo_print_fp(FILE *f, void *p);
EXTERN_C char *o_ContextInfo_to_charp(void *p);
EXTERN_C char *o_ContextInfo_gdlprint(void *p);

EXTERN_C FLO_BOOL o_ContextInfo_le(void *a, void *b);
EXTERN_C FLO_BOOL o_ContextInfo_leq(void *a, void *b);
EXTERN_C FLO_BOOL o_ContextInfo_ge(void *a, void *b);
EXTERN_C FLO_BOOL o_ContextInfo_geq(void *a, void *b);

#define o_ContextInfo_RCI(foo) (foo)
#define o_ContextInfo_RCD(foo) (foo)

extern ull o_ContextInfo_power;
extern int o_ContextInfo_is_power_unendl;

// these are dummies; iterating over ContextInfos is meaningless
typedef void *_o_ContextInfo_acur;
typedef void **o_ContextInfo_acur;

EXTERN_C void o_ContextInfo_acur_reset(void *p);
EXTERN_C void o_ContextInfo_acur_next(void *p);
EXTERN_C void *o_ContextInfo_acur_get(void *p);
EXTERN_C FLO_BOOL o_ContextInfo_acur_is_empty(void *p);
#endif

#endif
