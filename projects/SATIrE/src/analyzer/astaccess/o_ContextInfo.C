// Implementation of SATIrE's ContextInfo data type for PAG.

#include <cstring>
#include <cstdlib>

#include "o_ContextInfo.h"

// initial value, changed by ContextInfo_init
int ContextInfo::type_id = -1;

#if HAVE_PAG

#include "gc_mem.h"
#include "unum.h"
#include "str.h"

extern "C" void o_ContextInfo_mark(void *)
{
}

extern "C" void o_ContextInfo_mcopy(void *src, void *dst)
{
    ContextInfo *s = (ContextInfo *) src;
    ContextInfo *d = (ContextInfo *) dst;

    *s = *d;
}

// If we pretend that there is an infinite number of contexts, we might get
// around implementing abstract cursor functions.
ull o_ContextInfo_power;
int o_ContextInfo_is_power_unendl = 1;

extern "C" FLO_BOOL o_ContextInfo_eq(void *p, void *q)
{
    ContextInfo *a = (ContextInfo *) p;
    ContextInfo *b = (ContextInfo *) q;

    return (a->procnum == b->procnum && a->position == b->position
            ? FLO_TRUE : FLO_FALSE);
}

extern "C" FLO_BOOL o_ContextInfo_neq(void *p, void *q)
{
    return (o_ContextInfo_eq(p, q) ? FLO_FALSE : FLO_TRUE);
}

extern "C" unsigned int o_ContextInfo_hash(void *p)
{
    ContextInfo *v = (ContextInfo *) p;
 // This should be unique in most reasonable cases...
    return (v->procnum << 8) & 0xFF | v->position & 0xFF;
}

extern "C" void o_ContextInfo_init(void)
{
    if (ContextInfo::type_id == -1)
    {
        ContextInfo::type_id = GC_registertype(sizeof (ContextInfo),
                                               o_ContextInfo_mark,
                                               o_ContextInfo_mcopy,
                                               o_ContextInfo_eq,
                                               o_ContextInfo_hash,
                                               /* noshare = */ 0);
    }
}

extern "C" void *o_ContextInfo_duplicate(void *src)
{
    ContextInfo *s = (ContextInfo *) src;
    ContextInfo *dst = (ContextInfo *) GC_alloc(ContextInfo::type_id);
    *dst = *s;
    return dst;
}

extern "C" void o_ContextInfo_find_obj(void)
{
}

extern "C" void o_ContextInfo_copy_obj(void)
{
}

extern "C" void o_ContextInfo_clear_flag(void)
{
}

extern "C" FLO_BOOL o_ContextInfo_le(void *a, void *b)
{
    ContextInfo *v = (ContextInfo *) a;
    ContextInfo *w = (ContextInfo *) b;
    return (v->procnum < w->procnum
            || (v->procnum == w->procnum && v->position < w->position)
            ? FLO_TRUE : FLO_FALSE);
}

extern "C" FLO_BOOL o_ContextInfo_leq(void *a, void *b)
{
    return (o_ContextInfo_le(a, b) == FLO_TRUE
            ? FLO_TRUE
            : o_ContextInfo_eq(a, b));
}

extern "C" FLO_BOOL o_ContextInfo_ge(void *a, void *b)
{
    return o_ContextInfo_le(b, a);
}

extern "C" FLO_BOOL o_ContextInfo_geq(void *a, void *b)
{
    return o_ContextInfo_leq(b, a);
}

extern "C" char *o_ContextInfo_print(void *p)
{
    ContextInfo *v = (ContextInfo *) p;
    std::string str = v->print();
    const char *s = str.c_str();
    char *result = (char *) gc_tmp(std::strlen(s) + 1);
    std::strcpy(result, s);
    return result;
}

extern "C" void o_ContextInfo_print_fp(FILE *f, void *p)
{
    ContextInfo *v = (ContextInfo *) p;
    std::string str = v->print();
    const char *s = str.c_str();
    std::fputs(s, f);
}

extern "C" char *o_ContextInfo_to_charp(void *p)
{
    return o_ContextInfo_print(p);
}

extern "C" char *o_ContextInfo_gdlprint(void *p)
{
    return o_ContextInfo_print(p);
}

#include <iostream>

extern "C" void o_ContextInfo_acur_reset(void *)
{
    std::cerr
        << "*** SATIrE user error: can't iterate over infinite ContextInfo set"
        << std::endl;
    std::abort();
}

extern "C" void o_ContextInfo_acur_next(void *)
{
    std::cerr
        << "*** SATIrE user error: can't iterate over infinite ContextInfo set"
        << std::endl;
    std::abort();
}

extern "C" void *o_ContextInfo_acur_get(void *)
{
    std::cerr
        << "*** SATIrE user error: can't iterate over infinite ContextInfo set"
        << std::endl;
    std::abort();
}

extern "C" FLO_BOOL o_ContextInfo_acur_is_empty(void *)
{
    std::cerr
        << "*** SATIrE user error: can't iterate over infinite ContextInfo set"
        << std::endl;
    std::abort();
}

#endif

#include <sstream>

std::string ContextInfo::print() const
{
    std::stringstream result;
    result << "Context(" << procnum << "," << position << ")";
    return result.str();
}

ContextInfo::ContextInfo(int procnum, int position)
  : procnum(procnum), position(position)
{
}
