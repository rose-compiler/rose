#ifndef H_EXPRESSION_ID
#define H_EXPRESSION_ID

#if HAVE_PAG
#include "global.h"
#endif

/* GB (2008-05-19): Made this header includable by C programs because we
   need to replace the PAG-generated o_ExpressionId.h. */
#ifdef __cplusplus

#define EXTERN_C extern "C"

#include <string>

// GB (2008-05-14): This class wraps the expression Ids computed by the AST
// numbering mechanism. Pointers to instances of this class can be used in
// PAG-generated analyzers like built-in types.
// That's neat because we don't have to mess around with numbers anymore,
// and when visualizing analysis results, expression Ids can be told to
// print as the expression they represent rather than an opaque number.
class ExpressionId
{
public:
 // identifier of the expression
    unsigned long id;
 // string visualization of the expression
    std::string print() const;

 // static stuff
 // PAG type id, a handle returned by GC_registertype and needed when
 // allocating instances on the PAG heap
    static int type_id;
 // Support for setting the format for visualizing expression Ids: number,
 // expression string, or both (default).
    enum PrintFormat { F_Id, F_Expression, F_IdAndExpression };
    static void setPrintFormat(PrintFormat format);
    static PrintFormat printFormat;
};

#if HAVE_PAG
#include "str.h"
extern "C" void *o_expr_exprid(void *p);
extern "C" void *o_exprid_expr(void *p);
extern "C" str o_exprid_str(void *p);
#endif

#else
/* C part */
#define EXTERN_C

#endif

#if HAVE_PAG
typedef void *o_ExpressionId;

#define o_ExpressionId_mark_self(x)  \
    ((x) == NULL ? (void) 0 : GC_mark((void **) &x))
#define o_ExpressionId_mcopy_self(x) \
    ((x) == NULL ? (x) : (o_ExpressionId) (GC_share_copy((void *) x)))

EXTERN_C void o_ExpressionId_mark(void *);
EXTERN_C void o_ExpressionId_mcopy(void *src, void *dst);
EXTERN_C FLO_BOOL o_ExpressionId_eq(void *p, void *q);
EXTERN_C FLO_BOOL o_ExpressionId_neq(void *p, void *q);
EXTERN_C unsigned int o_ExpressionId_hash(void *p);
EXTERN_C void o_ExpressionId_init(void);
EXTERN_C void *o_ExpressionId_duplicate(void *src);
EXTERN_C void o_ExpressionId_find_obj(void);
EXTERN_C void o_ExpressionId_copy_obj(void);
EXTERN_C void o_ExpressionId_clear_flag(void);
EXTERN_C char *o_ExpressionId_print(void *p);
EXTERN_C void o_ExpressionId_print_fp(FILE *f, void *p);
EXTERN_C char *o_ExpressionId_to_charp(void *p);
EXTERN_C char *o_ExpressionId_gdlprint(void *p);

EXTERN_C FLO_BOOL o_ExpressionId_le(void *a, void *b);
EXTERN_C FLO_BOOL o_ExpressionId_leq(void *a, void *b);
EXTERN_C FLO_BOOL o_ExpressionId_ge(void *a, void *b);
EXTERN_C FLO_BOOL o_ExpressionId_geq(void *a, void *b);

#define o_ExpressionId_RCI(foo) (foo)
#define o_ExpressionId_RCD(foo) (foo)

extern ull o_ExpressionId_power;
extern int o_ExpressionId_is_power_unendl;

typedef unsigned long _o_ExpressionId_acur;
typedef unsigned long *o_ExpressionId_acur;

EXTERN_C void o_ExpressionId_acur_reset(unsigned long *p);
EXTERN_C void o_ExpressionId_acur_next(unsigned long *p);
EXTERN_C void *o_ExpressionId_acur_get(unsigned long *p);
EXTERN_C FLO_BOOL o_ExpressionId_acur_is_empty(unsigned long *p);
#endif

#endif
