// GB (2008-05-14): Initial and somewhat experimental implementation of the
// ExpressionID type. This type implements the interface required for PAG
// data types and can therefore be used in PAG analysis specifications as if
// it were a built-in type.
// These functions are described in the PAG manual in Section 11.3
// "Type-Storage Interface".

#include "gc_mem.h"
#include "unum.h"

#include "cfg_support.h"
#include "IrCreation.h"

#include "o_ExpressionID.h"

// initial value, changed by ExpressionID_init
int ExpressionID::type_id = -1;

// initial value, changed by ExpressionID_init
static unsigned long numberOfExpressions = 0;

// initial value, can be changed by ExpressionID::setPrintFormat
ExpressionID::PrintFormat ExpressionID::printFormat = ExpressionID::F_Expression;

// PAG functions follow
extern "C" void o_ExpressionID_mark(void *)
{
 // dummy: ExpressionID does not contain any pointers, so there is nothing to
 // mark recursively
}

extern "C" void o_ExpressionID_mcopy(void *src, void *dst)
{
    ExpressionID *s = (ExpressionID *) src;
    ExpressionID *d = (ExpressionID *) dst;

    d->id = s->id;
}

// number of variables; to be initialized once the AST has been numbered
ull o_ExpressionID_power;

int o_ExpressionID_is_power_unendl = 0;

extern "C" FLO_BOOL o_ExpressionID_eq(void *p, void *q)
{
    ExpressionID *a = (ExpressionID *) p;
    ExpressionID *b = (ExpressionID *) q;

    return (a->id == b->id ? FLO_TRUE : FLO_FALSE);
}

extern "C" FLO_BOOL o_ExpressionID_neq(void *p, void *q)
{
    ExpressionID *a = (ExpressionID *) p;
    ExpressionID *b = (ExpressionID *) q;

    return (a->id != b->id ? FLO_TRUE : FLO_FALSE);
}

extern "C" unsigned int o_ExpressionID_hash(void *p)
{
    ExpressionID *v = (ExpressionID *) p;
 // conversion to unsigned int from unsigned long might wrap around to a
 // smaller number, but that's fine for a hash
    return v->id;
}

extern "C" void o_ExpressionID_init(void)
{
    if (ExpressionID::type_id == -1)
    {
     // Register the type with the PAG garbage collector.
        ExpressionID::type_id = GC_registertype(sizeof (ExpressionID),
                                                o_ExpressionID_mark,
                                                o_ExpressionID_mcopy,
                                                o_ExpressionID_eq,
                                                o_ExpressionID_hash,
                                                /* noshare = */ 0);

     // This must be called after the ICFG has been built, and expressions
     // have been numbered.
        std::vector<SgExpression *> &exprs = get_global_cfg()->numbers_exprs;
     // numberOfExpressions is needed to test for termination of the
     // abstract cursor functions below.
        o_ExpressionID_power = exprs.size();
        numberOfExpressions = exprs.size();
    }
}

extern "C" void *o_ExpressionID_duplicate(void *src)
{
    ExpressionID *s = (ExpressionID *) src;
    ExpressionID *dst = (ExpressionID *) GC_alloc(ExpressionID::type_id);
    dst->id = s->id;
    return dst;
}

extern "C" void o_ExpressionID_find_obj(void)
{
 // dummy: We have no global instances of ExpressionID. Or should the
 // globalExpressionIDPool really be a list of global ExpressionIDs in the
 // garbage-collected PAG heap? Probably not.
}

extern "C" void o_ExpressionID_copy_obj(void)
{
 // dummy iff ExpressionID_find_obj is a dummy
}

extern "C" void o_ExpressionID_clear_flag(void)
{
 // dummy iff ExpressionID_find_obj is a dummy
}

// map expression to ExpressionID object, i.e. construct a new instance
// on the PAG heap
extern "C" void *o_expr_id(void *p)
{
    SgExpression *expr = (SgExpression *) p;
    void *n = GC_alloc(ExpressionID::type_id);
    ExpressionID *e = (ExpressionID *) n;
    e->id = get_global_cfg()->exprs_numbers[expr];
    return n;
}

// We also need to implement some support stuff:
// 10.2 Common Functions
extern "C" char *o_ExpressionID_print(void *p)
{
 // print a label for the variable into a buffer allocated using gc_tmp
    ExpressionID *v = (ExpressionID *) p;
    std::string label = v->print();
    const char *s = label.c_str();
    char *result = (char *) gc_tmp(std::strlen(s) + 1);
    std::strcpy(result, s);
    return result;
}

extern "C" void o_ExpressionID_print_fp(FILE *f, void *p)
{
    ExpressionID *v = (ExpressionID *) p;
    std::string label = v->print();
    const char *s = label.c_str();
    std::fputs(s, f);
}

extern "C" char *o_ExpressionID_to_charp(void *p)
{
 // the PAG manual is quite vague about what this is supposed to be; use
 // print for now
    return o_ExpressionID_print(p);
}

extern "C" char *o_ExpressionID_gdlprint(void *p)
{
    return o_ExpressionID_print(p);
}

// 10.2.1.2 Abstract Cursors
// The cursor is simply an index into the ICFG's map from numbers to
// expressions. As such, the cursor's value is the same as the expression ID
// it represents, which makes these definitions really really simple.
extern "C" void o_ExpressionID_acur_reset(unsigned long *p)
{
    *p = 0;
}

extern "C" void o_ExpressionID_acur_next(unsigned long *p)
{
    ++*p;
}

extern "C" void *o_ExpressionID_acur_get(unsigned long *p)
{
    void *n = GC_alloc(ExpressionID::type_id);
    ExpressionID *v = (ExpressionID *) n;
    v->id = *p;
    return n;
}

extern "C" FLO_BOOL o_ExpressionID_acur_is_empty(unsigned long *p)
{
    return (*p >= numberOfExpressions ? FLO_TRUE : FLO_FALSE);
}

// implementation of ExpressionID member functions
std::string ExpressionID::print() const
{
    std::stringstream result;

    switch (printFormat)
    {
    case F_ID:
        result << id;
        break;

    case F_Expression:
        result << Ir::fragmentToString(get_global_cfg()->numbers_exprs[id]);
        break;

    case F_IDAndExpression:
        result
            << id
            << "("
            << Ir::fragmentToString(get_global_cfg()->numbers_exprs[id])
            << ")";
        break;
    }

    return result.str();
}

void ExpressionID::setPrintFormat(PrintFormat format)
{
    printFormat = format;
}
