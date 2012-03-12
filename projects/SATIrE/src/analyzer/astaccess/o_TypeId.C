// GB (2008-06-04): Completing the move to an Id-based API by providing
// TypeId as abstract type usable in PAG analyses.

#include <cstring>
#include <cstdlib>

#include "cfg_support.h"
#include "IrCreation.h"

#include "o_TypeId.h"

// initial value, changed by TypeId_init
int TypeId::type_id = -1;

// initial value, changed by TypeId_init
static unsigned long numberOfTypes = 0;

// initial value, can be changed by TypeId::setPrintFormat
TypeId::PrintFormat TypeId::printFormat = TypeId::F_Type;

#if HAVE_PAG

#include "gc_mem.h"
#include "unum.h"
#include "str.h"

// PAG functions follow
extern "C" void o_TypeId_mark(void *)
{
 // dummy: TypeId does not contain any pointers, so there is nothing to mark
 // recursively
}

extern "C" void o_TypeId_mcopy(void *src, void *dst)
{
    TypeId *s = (TypeId *) src;
    TypeId *d = (TypeId *) dst;

    d->id = s->id;
}

// number of types; to be initialized once the AST has been numbered
ull o_TypeId_power;
int o_TypeId_is_power_unendl = 0;

extern "C" FLO_BOOL o_TypeId_eq(void *p, void *q)
{
    TypeId *a = (TypeId *) p;
    TypeId *b = (TypeId *) q;

    return (a->id == b->id ? FLO_TRUE : FLO_FALSE);
}

extern "C" FLO_BOOL o_TypeId_neq(void *p, void *q)
{
    TypeId *a = (TypeId *) p;
    TypeId *b = (TypeId *) q;

    return (a->id != b->id ? FLO_TRUE : FLO_FALSE);
}

extern "C" unsigned int o_TypeId_hash(void *p)
{
    TypeId *t = (TypeId *) p;
    return t->id;
}

#include <IrCreation.h>
extern "C" void o_TypeId_init(void)
{
    if (TypeId::type_id == -1)
    {
     // Register the type with the PAG garbage collector.
        TypeId::type_id = GC_registertype(sizeof (TypeId),
                                          o_TypeId_mark,
                                          o_TypeId_mcopy,
                                          o_TypeId_eq,
                                          o_TypeId_hash,
                                          /* noshare = */ 0);
        std::vector<SgType *> &types = get_global_cfg()->numbers_types;
        o_TypeId_power = types.size();
        numberOfTypes = types.size();

#if 0
     // debug: print all types
        std::vector<SgType *>::iterator t = types.begin();
        int i = 0;
        while (t != types.end())
        {
            std::cout
                << std::setw(2) << i++ << ": "
                << (void *) *t
                << " " << Ir::fragmentToString(*t)
                << "/" << (*t)->get_mangled().str()
                << std::endl;
            t++;
        }
#endif
    }
}

extern "C" void *o_TypeId_duplicate(void *src)
{
    TypeId *s = (TypeId *) src;
    TypeId *dst = (TypeId *) GC_alloc(TypeId::type_id);
    dst->id = s->id;
    return dst;
}

extern "C" void o_TypeId_find_obj(void)
{
 // dummy: we have no global instances of TypeId
}

extern "C" void o_TypeId_copy_obj(void)
{
 // dummy iff TypeId_find_obj is a dummy
}

extern "C" void o_TypeId_clear_flag(void)
{
 // dummy iff TypeId_find_obj is a dummy
}

EXTERN_C FLO_BOOL o_TypeId_le(void *a, void *b)
{
    TypeId *v = (TypeId *) a;
    TypeId *w = (TypeId *) b;
    return (v->id < w->id ? FLO_TRUE : FLO_FALSE);
}

EXTERN_C FLO_BOOL o_TypeId_leq(void *a, void *b)
{
    TypeId *v = (TypeId *) a;
    TypeId *w = (TypeId *) b;
    return (v->id <= w->id ? FLO_TRUE : FLO_FALSE);
}

EXTERN_C FLO_BOOL o_TypeId_ge(void *a, void *b)
{
    TypeId *v = (TypeId *) a;
    TypeId *w = (TypeId *) b;
    return (v->id > w->id ? FLO_TRUE : FLO_FALSE);
}

EXTERN_C FLO_BOOL o_TypeId_geq(void *a, void *b)
{
    TypeId *v = (TypeId *) a;
    TypeId *w = (TypeId *) b;
    return (v->id >= w->id ? FLO_TRUE : FLO_FALSE);
}


// *** Analyzer support functions
// map type to TypeId object, i.e. construct a new instance on the PAG heap
extern "C" void *o_type_typeid(void *p)
{
    SgType *type = (SgType *) p;
    void *n = GC_alloc(TypeId::type_id);
    TypeId *t = (TypeId *) n;
    t->id = get_global_cfg()->types_numbers[type];
    return t;
}

// map TypeId to its type
extern "C" void *o_typeid_type(void *p)
{
    TypeId *t = (TypeId *) p;
    SgType *type = get_global_cfg()->numbers_types[t->id];
    return type;
}

// return a PAG string with the type's representation
extern "C" str o_typeid_str(void *p)
{
    TypeId *t = (TypeId *) p;
    SgType *type = get_global_cfg()->numbers_types[t->id];
    std::string representation = Ir::fragmentToString(type);
    str s = str_create(representation.c_str());
    return s;
}

#include <o_ExpressionId.h>
// get the type identifier for a given expression identifier
extern "C" void *o_exprid_typeid(void *p)
{
    ExpressionId *e = (ExpressionId *) p;
    SgExpression *expr = get_global_cfg()->numbers_exprs[e->id];
    return o_type_typeid(expr->get_type());
}


// *** Other PAG support functions
extern "C" char *o_TypeId_print(void *p)
{
    TypeId *t = (TypeId *) p;
    std::string label = t->print();
    const char *s = label.c_str();
    char *result = (char *) gc_tmp(std::strlen(s) + 1);
    std::strcpy(result, s);
    return result;
}

extern "C" void o_TypeId_print_fp(FILE *f, void *p)
{
    TypeId *v = (TypeId *) p;
    std::string label = v->print();
    const char *s = label.c_str();
    std::fputs(s, f);
}

extern "C" char *o_TypeId_to_charp(void *p)
{
 // the PAG manual is quite vague about what this is supposed to be; use
 // print for now
    return o_TypeId_print(p);
}

extern "C" char *o_TypeId_gdlprint(void *p)
{
    return o_TypeId_print(p);
}

// 10.2.1.2 Abstract Cursors
// The cursor is simply an index into the ICFG's map from numbers to
// types. As such, the cursor's value is the same as the type Id
// it represents, which makes these definitions really really simple.
extern "C" void o_TypeId_acur_reset(unsigned long *p)
{
    *p = 0;
}

extern "C" void o_TypeId_acur_next(unsigned long *p)
{
    ++*p;
}

extern "C" void *o_TypeId_acur_get(unsigned long *p)
{
    void *n = GC_alloc(TypeId::type_id);
    TypeId *v = (TypeId *) n;
    v->id = *p;
    return n;
}

extern "C" FLO_BOOL o_TypeId_acur_is_empty(unsigned long *p)
{
    return (*p >= numberOfTypes ? FLO_TRUE : FLO_FALSE);
}

#endif


// implementation of TypeId member functions
std::string TypeId::print() const
{
    std::stringstream result;

    switch (printFormat)
    {
    case F_Id:
        result << id;
        break;

    case F_Type:
        result << Ir::fragmentToString(get_global_cfg()->numbers_types[id]);
        break;

    case F_IdAndType:
        result
            << "("
            << id
            << ","
            << Ir::fragmentToString(get_global_cfg()->numbers_types[id])
            << ")";
        break;
    default: std::cerr << "Wrong format mode or print mode for nodes (Type:print) in gdl file"; std::abort();
    }

    return result.str();
}

void TypeId::setPrintFormat(PrintFormat format)
{
    printFormat = format;
}
