// GB (2008-05-13): Preliminary experimental implementation of the
// VariableID type. If this works out, we will also want to have
// ExpressionID and TypeID. The idea is to have types that can be used in
// PAG analysis specifications (and in data flow information), but which are
// distinct from all other types and offer customizable print functions and
// whatever else may come in handy.
// Maybe we should even replace the statement labels by some opaque type
// that can be mapped more easily to original source code locations.
// These functions are described in the PAG manual in Section 11.3
// "Type-Storage Interface".

#include "gc_mem.h"
#include "unum.h"

#include "cfg_support.h"

#include "o_VariableID.h"

// an auxiliary container needed to support PAG's abstract cursors
static std::vector<unsigned long> globalVariableIDPool;

// initial value, changed by VariableID_init
int VariableID::type_id = -1;

// initial value, can be changed by VariableID::setPrintFormat
VariableID::PrintFormat VariableID::printFormat = VariableID::F_IDAndName;

// PAG functions follow
extern "C" void o_VariableID_mark(void *)
{
 // dummy: VariableID does not contain any pointers, so there is nothing to
 // mark recursively
}

extern "C" void o_VariableID_mcopy(void *src, void *dst)
{
    VariableID *s = (VariableID *) src;
    VariableID *d = (VariableID *) dst;

    d->id = s->id;
}

// number of variables; to be initialized once the AST has been numbered
ull o_VariableID_power;

int o_VariableID_is_power_unendl = 0;

extern "C" FLO_BOOL o_VariableID_eq(void *p, void *q)
{
    VariableID *a = (VariableID *) p;
    VariableID *b = (VariableID *) q;

    return (a->id == b->id ? FLO_TRUE : FLO_FALSE);
}

extern "C" FLO_BOOL o_VariableID_neq(void *p, void *q)
{
    VariableID *a = (VariableID *) p;
    VariableID *b = (VariableID *) q;

    return (a->id != b->id ? FLO_TRUE : FLO_FALSE);
}

extern "C" unsigned int o_VariableID_hash(void *p)
{
    VariableID *v = (VariableID *) p;
 // conversion to unsigned int from unsigned long might wrap around to a
 // smaller number, but that's fine for a hash
    return v->id;
}

extern "C" void o_VariableID_init(void)
{
    if (VariableID::type_id == -1)
    {
     // Register the type with the PAG garbage collector.
        VariableID::type_id = GC_registertype(sizeof (VariableID),
                                              o_VariableID_mark,
                                              o_VariableID_mcopy,
                                              o_VariableID_eq,
                                              o_VariableID_hash,
                                              /* noshare = */ 0);

     // This must be called after the ICFG has been built, and expressions
     // have been numbered.
        std::map<unsigned long, SgVariableSymbol *> &varsyms
            = get_global_cfg()->ids_varsyms;
        o_VariableID_power = varsyms.size();
     // Fill the global pool with pointers to all the IDs we have, and
     // NULL-terminate it.
        globalVariableIDPool.clear();
        std::map<unsigned long, SgVariableSymbol *>::iterator id;
        for (id = varsyms.begin(); id != varsyms.end(); ++id)
            globalVariableIDPool.push_back(id->first);
    }
}

extern "C" void *o_VariableID_duplicate(void *src)
{
    VariableID *s = (VariableID *) src;
    VariableID *dst = VariableID::allocateGC();
    dst->id = s->id;
    return dst;
}

extern "C" void o_VariableID_find_obj(void)
{
 // dummy: We have no global instances of VariableID. Or should the
 // globalVariableIDPool really be a list of global VariableIDs in the
 // garbage-collected PAG heap? Probably not.
}

extern "C" void o_VariableID_copy_obj(void)
{
 // dummy iff VariableID_find_obj is a dummy
}

extern "C" void o_VariableID_clear_flag(void)
{
 // dummy iff VariableID_find_obj is a dummy
}

// map variable number to VariableID object, i.e. construct a new instance
// on the PAG heap
extern "C" void *o_varnum_id(unum i)
{
    VariableID *v = VariableID::allocateGC();
    v->id = unum_to_unsigned(i);
    return v;
}

// map variable symbol to VariableID object
extern "C" void *o_variable_id(void *p)
{
    SgVariableSymbol *symbol = (SgVariableSymbol *) p;
    unsigned long id = get_global_cfg()->varsyms_ids[symbol];
    VariableID *v = VariableID::allocateGC();
    v->id = id;
    return v;
}

// map variable reference to VariableID
extern "C" void *o_varref_id(void *p)
{
    SgVarRefExp *expr = (SgVarRefExp *) p;
    unsigned long id = get_global_cfg()->exprs_numbers[expr];
    VariableID *v = VariableID::allocateGC();
    v->id = id;
    return v;
}

// We also need to implement some support stuff:
// 10.2 Common Functions
extern "C" char *o_VariableID_print(void *p)
{
 // print a label for the variable into a buffer allocated using gc_tmp
    VariableID *v = (VariableID *) p;
    std::string label = v->print();
    const char *s = label.c_str();
    char *result = (char *) gc_tmp(std::strlen(s) + 1);
    std::strcpy(result, s);
    return result;
}

extern "C" void o_VariableID_print_fp(FILE *f, void *p)
{
    VariableID *v = (VariableID *) p;
    std::string label = v->print();
    const char *s = label.c_str();
    std::fputs(s, f);
}

extern "C" char *o_VariableID_to_charp(void *p)
{
 // the PAG manual is quite vague about what this is supposed to be; use
 // print for now
    return o_VariableID_print(p);
}

extern "C" char *o_VariableID_gdlprint(void *p)
{
    return o_VariableID_print(p);
}

// 10.2.1.2 Abstract Cursors
// The cursor is simply an index into the globalVariableIDPool.
extern "C" void o_VariableID_acur_reset(unsigned long *p)
{
    *p = 0;
}

extern "C" void o_VariableID_acur_next(unsigned long *p)
{
    ++*p;
}

extern "C" void *o_VariableID_acur_get(unsigned long *p)
{
    VariableID *v = VariableID::allocateGC();
    v->id = globalVariableIDPool[*p];
    return v;
}

extern "C" FLO_BOOL o_VariableID_acur_is_empty(unsigned long *p)
{
    return (*p >= globalVariableIDPool.size() ? FLO_TRUE : FLO_FALSE);
}

// implementation of VariableID member functions
std::string VariableID::print() const
{
    std::stringstream result;

    switch (printFormat)
    {
    case F_ID:
        result << id;
        break;

    case F_Name:
        result << get_global_cfg()->ids_varsyms[id]->get_name().str();
        break;

    case F_IDAndName:
        result
            << id
            << "("
            << get_global_cfg()->ids_varsyms[id]->get_name().str()
            << ")";
        break;
    }

    return result.str();
}

void VariableID::setPrintFormat(PrintFormat format)
{
    printFormat = format;
}

VariableID *VariableID::allocateGC()
{
    return (VariableID *) GC_alloc(VariableID::type_id);
}
