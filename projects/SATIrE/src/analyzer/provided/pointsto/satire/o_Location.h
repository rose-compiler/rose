#ifndef H_O_LOCATION
#define H_O_LOCATION

#ifdef __cplusplus

#define EXTERN_C extern "C"

#include <string>

#include "pointsto.h"

// GB (2008-11-07): This class wraps the Location objects computed by the
// points-to analysis. This enables us to use Locations in PAG-generated
// analyzers.
class LocationWrapper
{
public:
 // identifier of the location
    unsigned long id;
 // the wrapped location
    SATIrE::Analyses::PointsToAnalysis::Location *loc;
 // string visualization of the location
    std::string print() const;

 // static stuff
 // PAG type id, a handle returned by GC_registertype and needed when
 // allocating instances on the PAG heap
    static int type_id;
 // TODO: Implement these at some point. They could be useful.
 // enum PrintFormat { F_Id, F_Variables, F_IdAndVariables };
 // static void setPrintFormat(PrintFormat format);
 // static PrintFormat printFormat;
};
#else
/* C part */
#define EXTERN_C

#endif

#if HAVE_PAG
EXTERN_C FLO_BOOL o_varid_has_location(void *vp);
EXTERN_C FLO_BOOL o_varid_has_location_cs(void *vp, void *cp);
EXTERN_C void *o_varid_location(void *vp);
EXTERN_C FLO_BOOL o_exprid_has_location(void *ep);
EXTERN_C void *o_exprid_location(void *ep);
EXTERN_C void *o_location_varsyms(void *lp);
EXTERN_C FLO_BOOL o_may_be_aliased(void *lp);
EXTERN_C FLO_BOOL o_is_ptr_location(void *lp);
EXTERN_C void *o_dereference(void *lp);

typedef void *o_Location;

#define o_Location_mark_self(x)  \
    ((x) == NULL ? (void) 0 : GC_mark((void **) &x))
#define o_Location_mcopy_self(x) \
    ((x) == NULL ? (x) : (o_Location) (GC_share_copy((void *) x)))

EXTERN_C void o_Location_mark(void *);
EXTERN_C void o_Location_mcopy(void *src, void *dst);
EXTERN_C FLO_BOOL o_Location_eq(void *p, void *q);
EXTERN_C FLO_BOOL o_Location_neq(void *p, void *q);
EXTERN_C unsigned int o_Location_hash(void *p);
EXTERN_C void o_Location_init(void);
EXTERN_C void *o_Location_duplicate(void *src);
EXTERN_C void o_Location_find_obj(void);
EXTERN_C void o_Location_copy_obj(void);
EXTERN_C void o_Location_clear_flag(void);
EXTERN_C char *o_Location_print(void *p);
EXTERN_C void o_Location_print_fp(FILE *f, void *p);
EXTERN_C char *o_Location_to_charp(void *p);
EXTERN_C char *o_Location_gdlprint(void *p);

EXTERN_C FLO_BOOL o_Location_le(void *a, void *b);
EXTERN_C FLO_BOOL o_Location_leq(void *a, void *b);
EXTERN_C FLO_BOOL o_Location_ge(void *a, void *b);
EXTERN_C FLO_BOOL o_Location_geq(void *a, void *b);


extern ull o_Location_power;
extern int o_Location_is_power_unendl;

typedef unsigned long _o_Location_acur;
typedef unsigned long *o_Location_acur;

EXTERN_C void o_Location_acur_reset(unsigned long *p);
EXTERN_C void o_Location_acur_next(unsigned long *p);
EXTERN_C void *o_Location_acur_get(unsigned long *p);
EXTERN_C FLO_BOOL o_Location_acur_is_empty(unsigned long *p);
#endif

#endif
