// GB (2008-05-14): Initial and somewhat experimental implementation of the
// ExpressionId type. This type implements the interface required for PAG
// data types and can therefore be used in PAG analysis specifications as if
// it were a built-in type.
// These functions are described in the PAG manual in Section 11.3
// "Type-Storage Interface".

#include <cstring>
#include <cstdlib>

#include "cfg_support.h"
#include "IrCreation.h"

#include "o_ExpressionId.h"

// initial value, changed by ExpressionId_init
int ExpressionId::type_id = -1;

// initial value, changed by ExpressionId_init
static unsigned long numberOfExpressions = 0;

// initial value, can be changed by ExpressionId::setPrintFormat
ExpressionId::PrintFormat ExpressionId::printFormat = ExpressionId::F_Expression;

#if HAVE_PAG

// PAG functions follow
#include "gc_mem.h"
#include "unum.h"
#include "str.h"

extern "C" void o_ExpressionId_mark(void *)
{
 // dummy: ExpressionId does not contain any pointers, so there is nothing to
 // mark recursively
}

extern "C" void o_ExpressionId_mcopy(void *src, void *dst)
{
    ExpressionId *s = (ExpressionId *) src;
    ExpressionId *d = (ExpressionId *) dst;

    d->id = s->id;
}

// number of variables; to be initialized once the AST has been numbered
ull o_ExpressionId_power;

int o_ExpressionId_is_power_unendl = 0;

extern "C" FLO_BOOL o_ExpressionId_eq(void *p, void *q)
{
    ExpressionId *a = (ExpressionId *) p;
    ExpressionId *b = (ExpressionId *) q;

    return (a->id == b->id ? FLO_TRUE : FLO_FALSE);
}

extern "C" FLO_BOOL o_ExpressionId_neq(void *p, void *q)
{
    ExpressionId *a = (ExpressionId *) p;
    ExpressionId *b = (ExpressionId *) q;

    return (a->id != b->id ? FLO_TRUE : FLO_FALSE);
}

extern "C" unsigned int o_ExpressionId_hash(void *p)
{
    ExpressionId *v = (ExpressionId *) p;
 // conversion to unsigned int from unsigned long might wrap around to a
 // smaller number, but that's fine for a hash
    return v->id;
}

extern "C" void o_ExpressionId_init(void)
{
    if (ExpressionId::type_id == -1)
    {
     // Register the type with the PAG garbage collector.
        ExpressionId::type_id = GC_registertype(sizeof (ExpressionId),
                                                o_ExpressionId_mark,
                                                o_ExpressionId_mcopy,
                                                o_ExpressionId_eq,
                                                o_ExpressionId_hash,
                                                /* noshare = */ 0);

     // This must be called after the ICFG has been built, and expressions
     // have been numbered.
        std::vector<SgExpression *> &exprs = get_global_cfg()->numbers_exprs;
     // numberOfExpressions is needed to test for termination of the
     // abstract cursor functions below.
        o_ExpressionId_power = exprs.size();
        numberOfExpressions = exprs.size();
    }
}

extern "C" void *o_ExpressionId_duplicate(void *src)
{
    ExpressionId *s = (ExpressionId *) src;
    ExpressionId *dst = (ExpressionId *) GC_alloc(ExpressionId::type_id);
    dst->id = s->id;
    return dst;
}

extern "C" void o_ExpressionId_find_obj(void)
{
 // dummy: We have no global instances of ExpressionId. Or should the
 // globalExpressionIdPool really be a list of global ExpressionIds in the
 // garbage-collected PAG heap? Probably not.
}

extern "C" void o_ExpressionId_copy_obj(void)
{
 // dummy iff ExpressionId_find_obj is a dummy
}

extern "C" void o_ExpressionId_clear_flag(void)
{
 // dummy iff ExpressionId_find_obj is a dummy
}

EXTERN_C FLO_BOOL o_ExpressionId_le(void *a, void *b)
{
    ExpressionId *v = (ExpressionId *) a;
    ExpressionId *w = (ExpressionId *) b;
    return (v->id < w->id ? FLO_TRUE : FLO_FALSE);
}

EXTERN_C FLO_BOOL o_ExpressionId_leq(void *a, void *b)
{
    ExpressionId *v = (ExpressionId *) a;
    ExpressionId *w = (ExpressionId *) b;
    return (v->id <= w->id ? FLO_TRUE : FLO_FALSE);
}

EXTERN_C FLO_BOOL o_ExpressionId_ge(void *a, void *b)
{
    ExpressionId *v = (ExpressionId *) a;
    ExpressionId *w = (ExpressionId *) b;
    return (v->id > w->id ? FLO_TRUE : FLO_FALSE);
}

EXTERN_C FLO_BOOL o_ExpressionId_geq(void *a, void *b)
{
    ExpressionId *v = (ExpressionId *) a;
    ExpressionId *w = (ExpressionId *) b;
    return (v->id >= w->id ? FLO_TRUE : FLO_FALSE);
}


// *** Analyzer support functions
// map expression to ExpressionId object, i.e. construct a new instance
// on the PAG heap
extern "C" void *o_expr_exprid(void *p)
{
    SgExpression *expr = (SgExpression *) p;
    void *n = GC_alloc(ExpressionId::type_id);
    ExpressionId *e = (ExpressionId *) n;
    e->id = get_global_cfg()->exprs_numbers[expr];
    return n;
}

// map ExpressionId to its expression
extern "C" void *o_exprid_expr(void *p)
{
    ExpressionId *id = (ExpressionId *) p;
    SgExpression *exp = get_global_cfg()->numbers_exprs[id->id];
    return exp;
}

unsigned long addExpressionIdForExpression(void *e)
{
    SgExpression *exp = (SgExpression *) e;

    unsigned long i = get_global_cfg()->numbers_exprs.size();
    get_global_cfg()->exprs_numbers[exp] = i;
    get_global_cfg()->numbers_exprs.push_back(exp);

    if (ExpressionId::type_id != -1)
    {
     // If ExpressionId has been initialized before, we need to adjust these
     // counters.
        numberOfExpressions++;
        o_ExpressionId_power++;
    }

    return i;
}

// return a PAG string with the expression's representation
extern "C" str o_exprid_str(void *p)
{
    ExpressionId *id = (ExpressionId *) p;
    SgExpression *exp = get_global_cfg()->numbers_exprs[id->id];
    std::string representation = Ir::fragmentToString(exp);
 // This PAG function shifts the burden of memory management to PAG. It
 // seems to allocate memory that lives as long as the memory manager does.
    str s = str_create(representation.c_str());
    return s;
}


// *** Other PAG support functions
// 10.2 Common Functions
extern "C" char *o_ExpressionId_print(void *p)
{
 // print a label for the variable into a buffer allocated using gc_tmp
    ExpressionId *v = (ExpressionId *) p;
    std::string label = v->print();
    const char *s = label.c_str();
    char *result = (char *) gc_tmp(std::strlen(s) + 1);
    std::strcpy(result, s);
    return result;
}

extern "C" void o_ExpressionId_print_fp(FILE *f, void *p)
{
    ExpressionId *v = (ExpressionId *) p;
    std::string label = v->print();
    const char *s = label.c_str();
    std::fputs(s, f);
}

extern "C" char *o_ExpressionId_to_charp(void *p)
{
 // the PAG manual is quite vague about what this is supposed to be; use
 // print for now
    return o_ExpressionId_print(p);
}

extern "C" char *o_ExpressionId_gdlprint(void *p)
{
    return o_ExpressionId_print(p);
}

// 10.2.1.2 Abstract Cursors
// The cursor is simply an index into the ICFG's map from numbers to
// expressions. As such, the cursor's value is the same as the expression Id
// it represents, which makes these definitions really really simple.
extern "C" void o_ExpressionId_acur_reset(unsigned long *p)
{
    *p = 0;
}

extern "C" void o_ExpressionId_acur_next(unsigned long *p)
{
    ++*p;
}

extern "C" void *o_ExpressionId_acur_get(unsigned long *p)
{
    void *n = GC_alloc(ExpressionId::type_id);
    ExpressionId *v = (ExpressionId *) n;
    v->id = *p;
    return n;
}

extern "C" FLO_BOOL o_ExpressionId_acur_is_empty(unsigned long *p)
{
    return (*p >= numberOfExpressions ? FLO_TRUE : FLO_FALSE);
}

#endif

// implementation of ExpressionId member functions
std::string ExpressionId::print() const
{
    std::stringstream result;

    switch (printFormat)
    {
    case F_Id:
        result << id;
        break;

    case F_Expression:
        result << Ir::fragmentToString(get_global_cfg()->numbers_exprs[id]);
        break;

    case F_IdAndExpression:
        result
            << "("
            << id
	    << ","
            << Ir::fragmentToString(get_global_cfg()->numbers_exprs[id])
            << ")";
        break;
    default: std::cerr << "Wrong format mode or print mode for nodes (Expression:print) in gdl file"; std::abort();
    }

    return result.str();
}

void ExpressionId::setPrintFormat(PrintFormat format)
{
    printFormat = format;
}
