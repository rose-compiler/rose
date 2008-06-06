#ifndef H_TYPE_ID
#define H_TYPE_ID

#ifdef __cplusplus

#define EXTERN_C extern "C"

#include <string>

class TypeId
{
public:
 // identifier of the type
    unsigned long id;
 // string visualization of the expression
    std::string print() const;

 // static stuff
 // PAG type id, a handle used with PAG's memory manager
    static int type_id;
 // Support for setting the visualization format: number, type string
 // (default), or both.
    enum PrintFormat { F_Id, F_Type, F_IdAndType };
    static void setPrintFormat(PrintFormat format);
    static PrintFormat printFormat;
};
#else
/* C part */
#define EXTERN_C

#endif

typedef void *o_TypeId;

#define o_TypeId_mark_self(x)  ((x) == NULL ? (void) 0 : GC_mark((void **) &x))
#define o_TypeId_mcopy_self(x) ((x) == NULL ? (x) : (o_TypeId) (GC_share_copy((void *) x)))

EXTERN_C void o_TypeId_mark(void *);
EXTERN_C void o_TypeId_mcopy(void *src, void *dst);
EXTERN_C FLO_BOOL o_TypeId_eq(void *p, void *q);
EXTERN_C FLO_BOOL o_TypeId_neq(void *p, void *q);
EXTERN_C unsigned int o_TypeId_hash(void *p);
EXTERN_C void o_TypeId_init(void);
EXTERN_C void *o_TypeId_duplicate(void *src);
EXTERN_C void o_TypeId_find_obj(void);
EXTERN_C void o_TypeId_copy_obj(void);
EXTERN_C void o_TypeId_clear_flag(void);
EXTERN_C char *o_TypeId_print(void *p);
EXTERN_C void o_TypeId_print_fp(FILE *f, void *p);
EXTERN_C char *o_TypeId_to_charp(void *p);
EXTERN_C char *o_TypeId_gdlprint(void *p);

#define o_TypeId_RCI(foo) (foo)
#define o_TypeId_RCD(foo) (foo)

extern ull o_TypeId_power;
extern int o_TypeId_is_power_unendl;

typedef unsigned long _o_TypeId_acur;
typedef unsigned long *o_TypeId_acur;

EXTERN_C void o_TypeId_acur_reset(unsigned long *p);
EXTERN_C void o_TypeId_acur_next(unsigned long *p);
EXTERN_C void *o_TypeId_acur_get(unsigned long *p);
EXTERN_C FLO_BOOL o_TypeId_acur_is_empty(unsigned long *p);

#endif
