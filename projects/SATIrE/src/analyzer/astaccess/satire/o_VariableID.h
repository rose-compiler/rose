#ifndef H_VARIABLE_ID
#define H_VARIABLE_ID

/* GB (2008-05-19): Made this header includable by C programs because we
   need to replace the PAG-generated o_VariableID.h by ours, and we still
   compile PAG analyzers as C. */
#ifdef __cplusplus

#define EXTERN_C extern "C"

#include <string>

// GB (2008-05-14): This class wraps the variable IDs computed by the AST
// numbering mechanism. Pointers to instances of this class can be used in
// PAG-generated analyzers like built-in types.
// The advantages over simply using numbers as before are stricter typing,
// which leads to more self-documenting code (and possibly the avoidance of
// a few mistakes), and better control of how variables are printed.
class VariableID
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
 // Support for setting the format for visualizing variable IDs: number
 // only, name only (default), or a combined form that looks like
 // "42(my_variable)".
 // We might add a format string based approach later, although that might
 // be overkill.
    enum PrintFormat { F_ID, F_Name, F_IDAndName };
    static void setPrintFormat(PrintFormat format);
    static PrintFormat printFormat;
 // Allocate a new garbage-collected instance with the given ID.
    static VariableID *allocateGC(unsigned long id);
};
#else
/* C part */
#define EXTERN_C

#endif

typedef void *o_VariableID;

/* Macros required by PAG-generated code, taken from PAG manual (but with
   two errors fixed here). */
#define o_VariableID_mark_self(x)  \
    ((x) == NULL ? (void) 0 : GC_mark((void **) &x))
#define o_VariableID_mcopy_self(x) \
    ((x) == NULL ? (x) : (o_VariableID) (GC_share_copy((void *) x)))

/* Functions required by PAG-generated code and documented in the manual. */
EXTERN_C void o_VariableID_mark(void *);
EXTERN_C void o_VariableID_mcopy(void *src, void *dst);
EXTERN_C FLO_BOOL o_VariableID_eq(void *p, void *q);
EXTERN_C FLO_BOOL o_VariableID_neq(void *p, void *q);
EXTERN_C unsigned int o_VariableID_hash(void *p);
EXTERN_C void o_VariableID_init(void);
EXTERN_C void *o_VariableID_duplicate(void *src);
EXTERN_C void o_VariableID_find_obj(void);
EXTERN_C void o_VariableID_copy_obj(void);
EXTERN_C void o_VariableID_clear_flag(void);
EXTERN_C char *o_VariableID_print(void *p);
EXTERN_C void o_VariableID_print_fp(FILE *f, void *p);
EXTERN_C char *o_VariableID_to_charp(void *p);
EXTERN_C char *o_VariableID_gdlprint(void *p);

/* Useless macros required by PAG-generated code but NOT DOCUMENTED in the
   PAG manual. */
#define o_VariableID_RCI(foo) (foo)
#define o_VariableID_RCD(foo) (foo)

/* Other required stuff. */
extern ull o_VariableID_power;
extern int o_VariableID_is_power_unendl;

typedef unsigned long _o_VariableID_acur;
typedef unsigned long *o_VariableID_acur;

EXTERN_C void o_VariableID_acur_reset(unsigned long *p);
EXTERN_C void o_VariableID_acur_next(unsigned long *p);
EXTERN_C void *o_VariableID_acur_get(unsigned long *p);
EXTERN_C FLO_BOOL o_VariableID_acur_is_empty(unsigned long *p);

#endif
