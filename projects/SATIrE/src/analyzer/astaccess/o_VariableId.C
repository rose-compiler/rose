// GB (2008-05-13): Preliminary experimental implementation of the
// VariableId type. If this works out, we will also want to have
// ExpressionId and TypeId. The idea is to have types that can be used in
// PAG analysis specifications (and in data flow information), but which are
// distinct from all other types and offer customizable print functions and
// whatever else may come in handy.
// Maybe we should even replace the statement labels by some opaque type
// that can be mapped more easily to original source code locations.
// These functions are described in the PAG manual in Section 11.3
// "Type-Storage Interface".

#include <cstring>

#include "cfg_support.h"

#include "o_VariableId.h"

// an auxiliary container needed to support PAG's abstract cursors
static std::vector<unsigned long> globalVariableIdPool;

// initial value, changed by VariableId_init
int VariableId::type_id = -1;

// initial value, can be changed by VariableId::setPrintFormat
VariableId::PrintFormat VariableId::printFormat = VariableId::F_Name;

#if HAVE_PAG

// PAG functions follow
#include "gc_mem.h"
#include "unum.h"
#include "str.h"

extern "C" void o_VariableId_mark(void *)
{
 // dummy: VariableId does not contain any pointers, so there is nothing to
 // mark recursively
}

extern "C" void o_VariableId_mcopy(void *src, void *dst)
{
    VariableId *s = (VariableId *) src;
    VariableId *d = (VariableId *) dst;

    d->id = s->id;
}

// number of variables; to be initialized once the AST has been numbered
ull o_VariableId_power;

int o_VariableId_is_power_unendl = 0;

extern "C" FLO_BOOL o_VariableId_eq(void *p, void *q)
{
    VariableId *a = (VariableId *) p;
    VariableId *b = (VariableId *) q;

    return (a->id == b->id ? FLO_TRUE : FLO_FALSE);
}

extern "C" FLO_BOOL o_VariableId_neq(void *p, void *q)
{
    VariableId *a = (VariableId *) p;
    VariableId *b = (VariableId *) q;

    return (a->id != b->id ? FLO_TRUE : FLO_FALSE);
}

extern "C" unsigned int o_VariableId_hash(void *p)
{
    VariableId *v = (VariableId *) p;
 // conversion to unsigned int from unsigned long might wrap around to a
 // smaller number, but that's fine for a hash
    return v->id;
}

extern "C" void o_VariableId_init(void)
{
    if (VariableId::type_id == -1)
    {
     // Register the type with the PAG garbage collector.
        VariableId::type_id = GC_registertype(sizeof (VariableId),
                                              o_VariableId_mark,
                                              o_VariableId_mcopy,
                                              o_VariableId_eq,
                                              o_VariableId_hash,
                                              /* noshare = */ 0);

     // This must be called after the ICFG has been built, and expressions
     // have been numbered.
        std::map<unsigned long, SgVariableSymbol *> &varsyms
            = get_global_cfg()->ids_varsyms;
        o_VariableId_power = varsyms.size();
     // Fill the global pool with pointers to all the Ids we have.
        globalVariableIdPool.clear();
        std::map<unsigned long, SgVariableSymbol *>::iterator id;
        for (id = varsyms.begin(); id != varsyms.end(); ++id)
            globalVariableIdPool.push_back(id->first);
    }
}

extern "C" void *o_VariableId_duplicate(void *src)
{
    VariableId *s = (VariableId *) src;
    VariableId *dst = VariableId::allocateGC(s->id);
    return dst;
}

extern "C" void o_VariableId_find_obj(void)
{
 // dummy: We have no global instances of VariableId. Or should the
 // globalVariableIdPool really be a list of global VariableIds in the
 // garbage-collected PAG heap? Probably not.
}

extern "C" void o_VariableId_copy_obj(void)
{
 // dummy iff VariableId_find_obj is a dummy
}

extern "C" void o_VariableId_clear_flag(void)
{
 // dummy iff VariableId_find_obj is a dummy
}

EXTERN_C FLO_BOOL o_VariableId_le(void *a, void *b)
{
    VariableId *v = (VariableId *) a;
    VariableId *w = (VariableId *) b;
    return (v->id < w->id ? FLO_TRUE : FLO_FALSE);
}

EXTERN_C FLO_BOOL o_VariableId_leq(void *a, void *b)
{
    VariableId *v = (VariableId *) a;
    VariableId *w = (VariableId *) b;
    return (v->id <= w->id ? FLO_TRUE : FLO_FALSE);
}

EXTERN_C FLO_BOOL o_VariableId_ge(void *a, void *b)
{
    VariableId *v = (VariableId *) a;
    VariableId *w = (VariableId *) b;
    return (v->id > w->id ? FLO_TRUE : FLO_FALSE);
}

EXTERN_C FLO_BOOL o_VariableId_geq(void *a, void *b)
{
    VariableId *v = (VariableId *) a;
    VariableId *w = (VariableId *) b;
    return (v->id >= w->id ? FLO_TRUE : FLO_FALSE);
}


// *** Analyzer support functions
// map variable symbol to VariableId object
extern "C" void *o_varsym_varid(void *p)
{
    SgVariableSymbol *symbol = (SgVariableSymbol *) p;
    unsigned long id = get_global_cfg()->varsyms_ids[symbol];
    VariableId *v = VariableId::allocateGC(id);
    return v;
}

// map variable reference to VariableId
extern "C" void *o_varref_varid(void *p)
{
    SgVarRefExp *expr = (SgVarRefExp *) p;
    unsigned long id = get_global_cfg()->exprs_numbers[expr];
    VariableId *v = VariableId::allocateGC(id);
    return v;
}

// determine whether the variable is a temporary
extern "C" FLO_BOOL o_is_tmpvarid(void *p)
{
    VariableId *v = (VariableId *) p;
    SgVariableSymbol *symbol = get_global_cfg()->ids_varsyms[v->id];
    const char *str = symbol->get_name().str();
    return (*str == '$' ? FLO_TRUE : FLO_FALSE);
}

// return a PAG string with the variable's name
extern "C" str o_varid_str(void *p)
{
    VariableId *v = (VariableId *) p;
    SgVariableSymbol *symbol = get_global_cfg()->ids_varsyms[v->id];
    const char *name = symbol->get_name().str();
 // This PAG function shifts the burden of memory management to PAG. It
 // seems to allocate memory that lives as long as the memory manager does.
    str s = str_create(name);
    return s;
}

#include <o_ExpressionId.h>
// return expression identifier corresponding to this variable
extern "C" void *o_varid_exprid(void *p)
{
    VariableId *v = (VariableId *) p;
    ExpressionId *e = (ExpressionId *) GC_alloc(ExpressionId::type_id);
    e->id = v->id;
    return e;
}

#include <o_TypeId.h>
#include <IrCreation.h>
// add new, unique temporary variable
extern "C" void *o_add_tmpvarid(void *p_type)
{
    static unsigned long add_tmpvarid_counter = 0;

    TypeId *t = (TypeId *) p_type;
    SgType *type = get_global_cfg()->numbers_types[t->id];
    std::stringstream varname;
    varname << "$tmpvar$" << add_tmpvarid_counter++;

 // Determine the new id value.
    unsigned long i = get_global_cfg()->numbers_exprs.size();

 // Add the value to the variable symbol and expression maps, and to the
 // list of variable ids.
    SgVariableSymbol *sym = Ir::createVariableSymbol(varname.str(), type);
    get_global_cfg()->varsyms_ids[sym] = i;
    get_global_cfg()->ids_varsyms[i] = sym;

    SgVarRefExp *exp = Ir::createVarRefExp(sym);
    get_global_cfg()->exprs_numbers[exp] = i;
    get_global_cfg()->numbers_exprs.push_back(exp);

    globalVariableIdPool.push_back(i);

    VariableId *v = (VariableId *) GC_alloc(VariableId::type_id);
    v->id = i;

    return v;
}


// *** Some more PAG support stuff
// 10.2 Common Functions
extern "C" char *o_VariableId_print(void *p)
{
 // print a label for the variable into a buffer allocated using gc_tmp
    VariableId *v = (VariableId *) p;
    std::string label = v->print();
    const char *s = label.c_str();
    char *result = (char *) gc_tmp(std::strlen(s) + 1);
    std::strcpy(result, s);
    return result;
}

extern "C" void o_VariableId_print_fp(FILE *f, void *p)
{
    VariableId *v = (VariableId *) p;
    std::string label = v->print();
    const char *s = label.c_str();
    std::fputs(s, f);
}

extern "C" char *o_VariableId_to_charp(void *p)
{
 // the PAG manual is quite vague about what this is supposed to be; use
 // print for now
    return o_VariableId_print(p);
}

extern "C" char *o_VariableId_gdlprint(void *p)
{
    return o_VariableId_print(p);
}

// 10.2.1.2 Abstract Cursors
// The cursor is simply an index into the globalVariableIdPool.
extern "C" void o_VariableId_acur_reset(unsigned long *p)
{
    *p = 0;
}

extern "C" void o_VariableId_acur_next(unsigned long *p)
{
    ++*p;
}

extern "C" void *o_VariableId_acur_get(unsigned long *p)
{
    VariableId *v = VariableId::allocateGC(globalVariableIdPool[*p]);
    return v;
}

extern "C" FLO_BOOL o_VariableId_acur_is_empty(unsigned long *p)
{
    return (*p >= globalVariableIdPool.size() ? FLO_TRUE : FLO_FALSE);
}

#endif

// implementation of VariableId member functions
std::string VariableId::print() const
{
    std::stringstream result;

    switch (printFormat)
    {
    case F_Id:
        result << id;
        break;

    case F_Name:
        result << get_global_cfg()->ids_varsyms[id]->get_name().str();
        break;

    case F_IdAndName:
        result
            << "("
            << id
	    << ","
            << get_global_cfg()->ids_varsyms[id]->get_name().str()
            << ")";
        break;
    default: std::cerr << "Wrong format mode or print mode for nodes (Variable:print) in gdl file"; std::abort();
    }

    return result.str();
}

void VariableId::setPrintFormat(PrintFormat format)
{
    printFormat = format;
}

#if !HAVE_PAG
static VariableId *
GC_alloc(int)
{
    return new VariableId();
}
#endif

VariableId *VariableId::allocateGC(unsigned long id)
{
    VariableId *v = (VariableId *) GC_alloc(VariableId::type_id);
    v->id = id;
    return v;
}
