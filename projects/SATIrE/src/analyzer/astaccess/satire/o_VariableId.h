#ifndef H_VARIABLE_ID
#define H_VARIABLE_ID

#if HAVE_PAG
#include "global.h"
#endif

/* GB (2008-05-19): Made this header includable by C programs because we
   need to replace the PAG-generated o_VariableId.h by ours, and we still
   compile PAG analyzers as C. */
#ifdef __cplusplus

#define EXTERN_C extern "C"

#include <string>

// GB (2008-05-14): This class wraps the variable Ids computed by the AST
// numbering mechanism. Pointers to instances of this class can be used in
// PAG-generated analyzers like built-in types.
// The advantages over simply using numbers as before are stricter typing,
// which leads to more self-documenting code (and possibly the avoidance of
// a few mistakes), and better control of how variables are printed.
class VariableId
{
public:
 // identifier of the variable
    unsigned long id;
 // string visualization of the variable
    std::string print() const;

 // static stuff
 // PAG type id, a handle returned by GC_registertype and needed when
 // allocating instances on the PAG heap
    static int type_id;
 // Support for setting the format for visualizing variable Ids: number
 // only, name only (default), or a combined form that looks like
 // "42(my_variable)".
 // We might add a format string based approach later, although that might
 // be overkill.
    enum PrintFormat { F_Id, F_Name, F_IdAndName };
    static void setPrintFormat(PrintFormat format);
    static PrintFormat printFormat;
 // Allocate a new garbage-collected instance with the given Id.
    static VariableId *allocateGC(unsigned long id);
};

#if HAVE_PAG
#include "str.h"

extern "C" FLO_BOOL o_VariableId_eq(void *p, void *q);
extern "C" FLO_BOOL o_VariableId_neq(void *p, void *q);
extern "C" void *o_varsym_varid(void *p);
extern "C" void *o_varref_varid(void *p);
extern "C" FLO_BOOL o_is_tmpvarid(void *p);
extern "C" str o_varid_str(void *p);
extern "C" void *o_varid_exprid(void *p);
extern "C" void *o_add_tmpvarid(void *p_type);
extern "C" char *o_VariableId_print(void *p);
extern "C" void o_VariableId_print_fp(FILE *f, void *p);
extern "C" char *o_VariableId_to_charp(void *p);
extern "C" char *o_VariableId_gdlprint(void *p);
extern "C" void o_VariableId_acur_reset(unsigned long *p);
extern "C" void o_VariableId_acur_next(unsigned long *p);
extern "C" void *o_VariableId_acur_get(unsigned long *p);
extern "C" FLO_BOOL o_VariableId_acur_is_empty(unsigned long *p);
#endif

#else
/* C part */
#define EXTERN_C

#endif

typedef void *o_VariableId;

#if HAVE_PAG 
/* Macros required by PAG-generated code, taken from PAG manual (but with
   two errors fixed here). */
#define o_VariableId_mark_self(x)  \
    ((x) == NULL ? (void) 0 : GC_mark((void **) &x))
#define o_VariableId_mcopy_self(x) \
    ((x) == NULL ? (x) : (o_VariableId) (GC_share_copy((void *) x)))

/* Functions required by PAG-generated code and documented in the manual. */
EXTERN_C void o_VariableId_mark(void *);
EXTERN_C void o_VariableId_mcopy(void *src, void *dst);
EXTERN_C FLO_BOOL o_VariableId_eq(void *p, void *q);
EXTERN_C FLO_BOOL o_VariableId_neq(void *p, void *q);
EXTERN_C unsigned int o_VariableId_hash(void *p);
EXTERN_C void o_VariableId_init(void);
EXTERN_C void *o_VariableId_duplicate(void *src);
EXTERN_C void o_VariableId_find_obj(void);
EXTERN_C void o_VariableId_copy_obj(void);
EXTERN_C void o_VariableId_clear_flag(void);
EXTERN_C char *o_VariableId_print(void *p);
EXTERN_C void o_VariableId_print_fp(FILE *f, void *p);
EXTERN_C char *o_VariableId_to_charp(void *p);
EXTERN_C char *o_VariableId_gdlprint(void *p);
/* Comparison functions; at least the less-than operation is needed if PAG's
 * binary trees are used. */
EXTERN_C FLO_BOOL o_VariableId_le(void *a, void *b);
EXTERN_C FLO_BOOL o_VariableId_leq(void *a, void *b);
EXTERN_C FLO_BOOL o_VariableId_ge(void *a, void *b);
EXTERN_C FLO_BOOL o_VariableId_geq(void *a, void *b);

/* Useless macros required by PAG-generated code but NOT DOCUMENTED in the
   PAG manual. */
#define o_VariableId_RCI(foo) (foo)
#define o_VariableId_RCD(foo) (foo)

/* Other required stuff. */
extern ull o_VariableId_power;
extern int o_VariableId_is_power_unendl;

typedef unsigned long _o_VariableId_acur;
typedef unsigned long *o_VariableId_acur;

EXTERN_C void o_VariableId_acur_reset(unsigned long *p);
EXTERN_C void o_VariableId_acur_next(unsigned long *p);
EXTERN_C void *o_VariableId_acur_get(unsigned long *p);
EXTERN_C FLO_BOOL o_VariableId_acur_is_empty(unsigned long *p);
#endif

#endif
