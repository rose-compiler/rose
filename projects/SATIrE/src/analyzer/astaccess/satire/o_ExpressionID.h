#ifndef H_EXPRESSION_ID
#define H_EXPRESSION_ID

/* GB (2008-05-19): Made this header includable by C programs because we
   need to replace the PAG-generated o_ExpressionID.h. */
#ifdef __cplusplus

#define EXTERN_C extern "C"

#include <string>

// GB (2008-05-14): This class wraps the expression IDs computed by the AST
// numbering mechanism. Pointers to instances of this class can be used in
// PAG-generated analyzers like built-in types.
// That's neat because we don't have to mess around with numbers anymore,
// and when visualizing analysis results, expression IDs can be told to
// print as the expression they represent rather than an opaque number.
class ExpressionID
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
 // Support for setting the format for visualizing expression IDs: number,
 // expression string, or both (default).
    enum PrintFormat { F_ID, F_Expression, F_IDAndExpression };
    static void setPrintFormat(PrintFormat format);
    static PrintFormat printFormat;
};
#else
/* C part */
#define EXTERN_C

#endif

typedef void *o_ExpressionID;

#define o_ExpressionID_mark_self(x)  \
    ((x) == NULL ? (void) 0 : GC_mark((void **) &x))
#define o_ExpressionID_mcopy_self(x) \
    ((x) == NULL ? (x) : (o_ExpressionID) (GC_share_copy((void *) x)))

EXTERN_C void o_ExpressionID_mark(void *);
EXTERN_C void o_ExpressionID_mcopy(void *src, void *dst);
EXTERN_C FLO_BOOL o_ExpressionID_eq(void *p, void *q);
EXTERN_C FLO_BOOL o_ExpressionID_neq(void *p, void *q);
EXTERN_C unsigned int o_ExpressionID_hash(void *p);
EXTERN_C void o_ExpressionID_init(void);
EXTERN_C void *o_ExpressionID_duplicate(void *src);
EXTERN_C void o_ExpressionID_find_obj(void);
EXTERN_C void o_ExpressionID_copy_obj(void);
EXTERN_C void o_ExpressionID_clear_flag(void);
EXTERN_C char *o_ExpressionID_print(void *p);
EXTERN_C void o_ExpressionID_print_fp(FILE *f, void *p);
EXTERN_C char *o_ExpressionID_to_charp(void *p);
EXTERN_C char *o_ExpressionID_gdlprint(void *p);

#define o_ExpressionID_RCI(foo) (foo)
#define o_ExpressionID_RCD(foo) (foo)

extern ull o_ExpressionID_power;
extern int o_ExpressionID_is_power_unendl;

typedef unsigned long _o_ExpressionID_acur;
typedef unsigned long *o_ExpressionID_acur;

EXTERN_C void o_ExpressionID_acur_reset(unsigned long *p);
EXTERN_C void o_ExpressionID_acur_next(unsigned long *p);
EXTERN_C void *o_ExpressionID_acur_get(unsigned long *p);
EXTERN_C FLO_BOOL o_ExpressionID_acur_is_empty(unsigned long *p);

#endif
