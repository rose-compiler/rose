// GB (2008-11-07): This is the initial implementation of PAG analyzer
// access to points-to information.

// Support for the following functions:
// varid_has_location :: VariableId -> bool;
// varid_location :: VariableId -> Location;
// varid_location_cs :: VariableId * ContextInfo -> Location;
// exprid_has_location :: ExpressionId -> bool;
// exprid_location :: ExpressionId -> Location;
// exprid_location_cs :: ExpressionId * ContextInfo -> Location;
// location_varsyms :: Location -> *VariableSymbolNT;
// may_be_aliased :: Location -> bool;
// is_ptr_location :: Location -> bool;
// dereference :: Location -> Location;

// The _cs variants use the context-sensitive points-to analysis
// information.

// void **Ir::createNodeList(std::vector<T> &) should come in handy

// TODO: Ideally, the abstract locations seen by PAG analyses are only the
// equivalence class representatives; not every "location" node created by
// the analysis is meaningful in the sense that it is the canonical
// representative for a memory region. This is relevant if some user tries
// to construct the set of all locations, for instance. For now, our
// functions try to ensure to only look at non-dummy locations, and always
// fetch the equivalence class representative. Let's assume that users will
// not try to consult the set of all locations right away. (Although it
// might not really hurt, as every variable should still belong to exactly
// one unique location.)

#if HAVE_PAG
#include "gc_mem.h"
#include "unum.h"
#include "str.h"
#endif

#include "cfg_support.h"
#include "IrCreation.h"
#include "o_VariableId.h"
#include "o_ExpressionId.h"
#include "o_ContextInfo.h"

#include "pointsto.h"
#include "o_Location.h"

using SATIrE::Analyses::PointsToAnalysis;

// initial value, changed by o_Location_init
int LocationWrapper::type_id = -1;

// initial value, changed by o_Location_init
static unsigned long numberOfLocations = 0;

// initial value, can be changed by ExpressionId::setPrintFormat
// ExpressionId::PrintFormat ExpressionId::printFormat = ExpressionId::F_Expression;

#if HAVE_PAG
// PAG functions follow
extern "C" void o_Location_mark(void *)
{
 // dummy: an abstract location does not contain any PAG heap pointers, so
 // there is nothing to mark recursively
}

extern "C" void o_Location_mcopy(void *src, void *dst)
{
    LocationWrapper *s = (LocationWrapper *) src;
    LocationWrapper *d = (LocationWrapper *) dst;

    d->id = s->id;
    d->loc = s->loc;
}

// number of locations; to be initialized after points-to analysis
ull o_Location_power;
#endif

int o_Location_is_power_unendl = 0;

#include <cstdlib>

#if HAVE_PAG
static
PointsToAnalysis::PointsToAnalysis *
get_icfgContextSensitivePointsToAnalysis(void);
#endif

static
PointsToAnalysis::PointsToAnalysis *
get_icfgPointsToAnalysis(void)
{
#if !HAVE_PAG
    CFG *global_cfg = get_global_cfg();
    if (global_cfg == NULL)
    {
     // This cannot really happen unless a user does something very bad.
        std::cerr
            << "*** internal error: no ICFG present"
            << std::endl;
        std::abort();
    }
    PointsToAnalysis::PointsToAnalysis *pointsToAnalysis
        = global_cfg->pointsToAnalysis;
    if (pointsToAnalysis == NULL)
    {
        std::cerr
            << "*** error: no points-to analysis instance found; did you "
            << "specify --run-pointsto-analysis?"
            << std::endl;
        std::exit(EXIT_FAILURE);
    }
    return pointsToAnalysis;
#else
    return get_icfgContextSensitivePointsToAnalysis();
#endif
}

#if HAVE_PAG
static
PointsToAnalysis::PointsToAnalysis *
get_icfgContextSensitivePointsToAnalysis(void)
{
    CFG *icfg = get_global_cfg();
    if (icfg == NULL)
    {
        std::cerr
            << "*** internal error: no ICFG present"
            << std::endl;
        std::abort();
    }
    PointsToAnalysis::PointsToAnalysis *cspta
        = icfg->contextSensitivePointsToAnalysis;
    if (cspta == NULL)
    {
        std::cerr
            << "*** error: no points-to analysis instance found; did you "
            << "specify --run-pointsto-analysis?"
            << std::endl;
        std::exit(EXIT_FAILURE);
    }
    static bool once = true;
    if (once)
    {
        cspta->run(icfg);
        cspta->doDot("cs_pointsto");

        o_Location_power = cspta->get_locations().size();
        numberOfLocations = cspta->get_locations().size();

        once = false;
    }
    return cspta;
}
#endif

#if !HAVE_PAG
static LocationWrapper *
GC_alloc(int)
{
    return new LocationWrapper();
}
#endif

LocationWrapper *createLocationWrapper(PointsToAnalysis::Location *loc)
{
    void *n = GC_alloc(LocationWrapper::type_id);
    LocationWrapper *l = (LocationWrapper *) n;
#if 0
    l->id = get_icfgPointsToAnalysis()->location_id(loc);
    std::cout
        << "createLocationWrapper: starting from location " << l->id
        << " at " << (void *) loc << std::endl;
#endif
    loc = get_icfgPointsToAnalysis()->location_representative(loc);
    l->id = get_icfgPointsToAnalysis()->location_id(loc);
#if 0
    std::cout
        << "representative: location " << l->id << " at " << (void *) loc
        << std::endl;
#endif
    l->loc = loc;
    return l;
}

#if HAVE_PAG
extern "C" FLO_BOOL o_Location_eq(void *p, void *q)
{
    LocationWrapper *a = (LocationWrapper *) p;
    LocationWrapper *b = (LocationWrapper *) q;

    bool ids_eq = (a->id == b->id);
    bool locs_eq = (a->loc == b->loc);

    if (ids_eq != locs_eq)
    {
        std::cerr
            << "SATIrE internal error: inconsistent data structures "
            << "in o_Location_eq!"
            << std::endl;
        std::abort();
    }

    return (ids_eq && locs_eq ? FLO_TRUE : FLO_FALSE);
}

extern "C" FLO_BOOL o_Location_neq(void *p, void *q)
{
    LocationWrapper *a = (LocationWrapper *) p;
    LocationWrapper *b = (LocationWrapper *) q;

    return (a->id != b->id ? FLO_TRUE : FLO_FALSE);
}

extern "C" unsigned int o_Location_hash(void *p)
{
    LocationWrapper *v = (LocationWrapper *) p;
 // conversion to unsigned int from unsigned long might wrap around to a
 // smaller number, but that's fine for a hash
    return v->id;
}

extern "C" void o_Location_init(void)
{
    if (LocationWrapper::type_id == -1)
    {
     // Register the type with the PAG garbage collector.
        LocationWrapper::type_id = GC_registertype(sizeof (LocationWrapper),
                                                   o_Location_mark,
                                                   o_Location_mcopy,
                                                   o_Location_eq,
                                                   o_Location_hash,
                                                   /* noshare = */ 0);

        if (get_global_cfg()->pointsToAnalysis != NULL)
        {
         // This must be called after the ICFG has been built, and points-to
         // analysis has been performed. The call above gets the pointer to
         // the points-to analysis instance directly from the ICFG, all
         // further calls use the get_icfgPointsToAnalysis function which
         // dies if the pointer is NULL.
         // This code is now in the "once" part of
         // get_icfgContextSensitivePointsToAnalysis since it refers to the
         // data of the context-sensitive points-to analysis, which has not
         // been run at this point.
#if 0
            const std::vector<PointsToAnalysis::Location *> &locations
                = get_icfgPointsToAnalysis()->get_locations();
            o_Location_power = locations.size();
         // numberOfLocations is needed to test for termination of the
         // abstract cursor functions below.
            numberOfLocations = locations.size();
#endif
        }
        else
        {
            o_Location_power = 0;
            numberOfLocations = 0;
        }
    }
}

extern "C" void *o_Location_duplicate(void *src)
{
    LocationWrapper *s = (LocationWrapper *) src;
    LocationWrapper *dst = (LocationWrapper *) GC_alloc(
                                                    LocationWrapper::type_id);
    dst->id = s->id;
    dst->loc = s->loc;
    return dst;
}

extern "C" void o_Location_find_obj(void)
{
 // dummy: We have no global instances of locations.
}

extern "C" void o_Location_copy_obj(void)
{
 // dummy iff o_Location_find_obj is a dummy
}

extern "C" void o_Location_clear_flag(void)
{
 // dummy iff o_Location_find_obj is a dummy
}

EXTERN_C FLO_BOOL o_Location_le(void *a, void *b)
{
    LocationWrapper *v = (LocationWrapper *) a;
    LocationWrapper *w = (LocationWrapper *) b;
    return (v->id < w->id ? FLO_TRUE : FLO_FALSE);
}

EXTERN_C FLO_BOOL o_Location_leq(void *a, void *b)
{
    LocationWrapper *v = (LocationWrapper *) a;
    LocationWrapper *w = (LocationWrapper *) b;
    return (v->id <= w->id ? FLO_TRUE : FLO_FALSE);
}

EXTERN_C FLO_BOOL o_Location_ge(void *a, void *b)
{
    LocationWrapper *v = (LocationWrapper *) a;
    LocationWrapper *w = (LocationWrapper *) b;
    return (v->id > w->id ? FLO_TRUE : FLO_FALSE);
}

EXTERN_C FLO_BOOL o_Location_geq(void *a, void *b)
{
    LocationWrapper *v = (LocationWrapper *) a;
    LocationWrapper *w = (LocationWrapper *) b;
    return (v->id >= w->id ? FLO_TRUE : FLO_FALSE);
}


// *** Analyzer support functions
extern "C" FLO_BOOL o_varid_has_location(void *vp)
{
    if (o_is_tmpvarid(vp) == FLO_TRUE)
        return FLO_FALSE;

    VariableId *v = (VariableId *) vp;
    SgVariableSymbol *sym = get_global_cfg()->ids_varsyms[v->id];
    PointsToAnalysis::Location *loc
        = get_icfgPointsToAnalysis()->symbol_location(sym);
    return (loc != NULL ? FLO_TRUE : FLO_FALSE);
}

extern "C" void *o_varid_location(void *vp)
{
    VariableId *v = (VariableId *) vp;
    SgVariableSymbol *sym = get_global_cfg()->ids_varsyms[v->id];
    PointsToAnalysis::Location *loc
        = get_icfgPointsToAnalysis()->symbol_location(sym);
    return createLocationWrapper(loc);
}

extern "C" void *o_varid_location_cs(void *vp, void *cp)
{
    VariableId *v = (VariableId *) vp;
    ContextInfo *ctx = (ContextInfo *) cp;
 // ctx->procnum, ctx->position
 // std::cout << ctx->print();
    SgVariableSymbol *sym = get_global_cfg()->ids_varsyms[v->id];
    PointsToAnalysis *pta = get_icfgContextSensitivePointsToAnalysis();
    PointsToAnalysis::Location *loc
        = pta->symbol_location(sym,
                ContextInformation::Context(ctx->procnum, ctx->position,
                                            get_global_cfg()));
    return createLocationWrapper(loc);
}

extern "C" FLO_BOOL o_exprid_has_location(void *ep)
{
    SgExpression *exp = (SgExpression *) o_exprid_expr(ep);
 // Similarly to the varid case, simple VarRefExps to tmpvars do not have
 // valid locations.
    if (SgVarRefExp *vr = isSgVarRefExp(exp))
    {
        if (o_is_tmpvarid(o_varref_varid(vr)))
            return FLO_FALSE;
    }

    PointsToAnalysis::Location *loc
        = get_icfgPointsToAnalysis()->expressionLocation(exp);
 // We move further responsibility to decide what a "valid location" is to
 // the points-to analysis.
    if (get_icfgPointsToAnalysis()->valid_location(loc))
        return FLO_TRUE;
    else
        return FLO_FALSE;
}

extern "C" void *o_exprid_location(void *ep)
{
    SgExpression *exp = (SgExpression *) o_exprid_expr(ep);
    PointsToAnalysis::Location *loc
        = get_icfgPointsToAnalysis()->expressionLocation(exp);
    return createLocationWrapper(loc);
}

extern "C" void *o_exprid_location_cs(void *ep, void *cp)
{
    SgExpression *exp = (SgExpression *) o_exprid_expr(ep);
    ContextInfo *ctx = (ContextInfo *) cp;
    PointsToAnalysis *pta = get_icfgContextSensitivePointsToAnalysis();
    PointsToAnalysis::Location *loc
        = pta->expressionLocation(exp,
                ContextInformation::Context(ctx->procnum, ctx->position,
                                            get_global_cfg()));
    return createLocationWrapper(loc);
}

extern "C" void *o_location_varsyms(void *lp)
{
    LocationWrapper *wrapper = (LocationWrapper *) lp;
    PointsToAnalysis::Location *loc = wrapper->loc;
    const std::list<SgSymbol *> &syms
        = get_icfgPointsToAnalysis()->location_symbols(loc);
    std::list<SgSymbol *>::const_iterator s;
    std::vector<SgVariableSymbol *> varsyms;
    for (s = syms.begin(); s != syms.end(); ++s)
    {
        if (SgVariableSymbol *varsym = isSgVariableSymbol(*s))
            varsyms.push_back(varsym);
    }
    void **result = Ir::createNodeList(varsyms);
#if 0
    std::cout
        << "o_location_varsyms: " << varsyms.size()
        << " symbols, result = " << (void *) result
        << std::endl;
#endif
    return result;
}

extern "C" FLO_BOOL o_may_be_aliased(void *lp)
{
    LocationWrapper *wrapper = (LocationWrapper *) lp;
    PointsToAnalysis::Location *loc = wrapper->loc;
    bool mayBeAliased = get_icfgPointsToAnalysis()->mayBeAliased(loc);
    return (mayBeAliased ? FLO_TRUE : FLO_FALSE);
}

extern "C" FLO_BOOL o_is_array_location(void *lp)
{
    LocationWrapper *wrapper = (LocationWrapper *) lp;
    PointsToAnalysis::Location *loc = wrapper->loc;
    bool mayBeAliased = get_icfgPointsToAnalysis()->isArrayLocation(loc);
    return (mayBeAliased ? FLO_TRUE : FLO_FALSE);
}

extern "C" FLO_BOOL o_is_ptr_location(void *lp)
{
    LocationWrapper *wrapper = (LocationWrapper *) lp;
    PointsToAnalysis::Location *loc = wrapper->loc;
    loc = get_icfgPointsToAnalysis()->location_representative(loc);
    loc = get_icfgPointsToAnalysis()->base_location(loc);
    if (get_icfgPointsToAnalysis()->valid_location(loc))
        return FLO_TRUE;
    else
        return FLO_FALSE;
}

extern "C" void *o_dereference(void *lp)
{
    LocationWrapper *wrapper = (LocationWrapper *) lp;
    PointsToAnalysis::Location *loc = wrapper->loc;
    loc = get_icfgPointsToAnalysis()->location_representative(loc);
    loc = get_icfgPointsToAnalysis()->base_location(loc);
    return createLocationWrapper(loc);
}

#if 0
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
#endif

// *** Other PAG support functions
// 10.2 Common Functions
extern "C" char *o_Location_print(void *p)
{
    LocationWrapper *wrapper = (LocationWrapper *) p;
    std::string label = wrapper->print();
    const char *s = label.c_str();
    char *result = (char *) gc_tmp(std::strlen(s) + 1);
    std::strcpy(result, s);
    return result;
#if 0
 // print a label for the variable into a buffer allocated using gc_tmp
    ExpressionId *v = (ExpressionId *) p;
    std::string label = v->print();
    const char *s = label.c_str();
    char *result = (char *) gc_tmp(std::strlen(s) + 1);
    std::strcpy(result, s);
    return result;
#endif
}

extern "C" void o_Location_print_fp(FILE *f, void *p)
{
    LocationWrapper *v = (LocationWrapper *) p;
    std::string label = v->print();
    const char *s = label.c_str();
    std::fputs(s, f);
}

extern "C" char *o_Location_to_charp(void *p)
{
 // the PAG manual is quite vague about what this is supposed to be; use
 // print for now
    return o_Location_print(p);
}

extern "C" char *o_Location_gdlprint(void *p)
{
    return o_Location_print(p);
}

// 10.2.1.2 Abstract Cursors
// The abstract cursor is an index into the points-to analysis' vector of
// locations; initialization and advancing of the cursor is therefore quite
// simple. For the "get" operation, we fetch the corresponding location and
// build a wrapper instance on the PAG heap.
extern "C" void o_Location_acur_reset(unsigned long *p)
{
    *p = 0;
}

extern "C" void o_Location_acur_next(unsigned long *p)
{
    ++*p;
}

extern "C" void *o_Location_acur_get(unsigned long *p)
{
    PointsToAnalysis::Location *loc
        = get_icfgPointsToAnalysis()->get_locations().at(*p);
    return createLocationWrapper(loc);
#if 0
    void *n = GC_alloc(ExpressionId::type_id);
    ExpressionId *v = (ExpressionId *) n;
    v->id = *p;
    return n;
#endif
}

extern "C" FLO_BOOL o_Location_acur_is_empty(unsigned long *p)
{
    return (*p >= numberOfLocations ? FLO_TRUE : FLO_FALSE);
}
#endif


// implementation of LocationWrapper member functions
std::string LocationWrapper::print() const
{
    std::stringstream result;

    result << id;

    return result.str();
}

#if 0
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
#endif
