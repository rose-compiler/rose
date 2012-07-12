/*********************************************************************
 *   Copyright 1993, UCAR/Unidata
 *   See netcdf/COPYRIGHT file for copying and redistribution conditions.
 *   $Header: /upc/share/CVS/netcdf-3/libncdap4/constraints4.c,v 1.8 2010/04/02 23:18:41 dmh Exp $
 *********************************************************************/
#include "ncdap4.h"
#include "dapodom.h"

#ifdef DEBUG
#include "dapdump.h"
#endif

/* In order to construct the projection,
we need to make sure to match the relevant dimensions
against the relevant nodes in which the ultimate target
is contained.
*/
NCerror
buildvaraprojection4(NCDRNO* drno, Getvara* getvar,
		     const size_t* startp, const size_t* countp, const ptrdiff_t* stridep,
		     NCprojection** projectionp)
{
    int i;
    NCerror ncstat = NC_NOERR;
    NClist* dimset;
    CDFnode* var = getvar->target;
    NCprojection* projection = NULL;
    NClist* segments = NULL;
    NCsegment* segment;

    segment = createncsegment();
    segment->node = var;
    ASSERT((segment->node != NULL));
    segment->segment = nulldup(segment->node->name);
    segment->slicesdefined = 0; /* temporary */
    segments = nclistnew();
    nclistpush(segments,(ncelem)segment);

    projection = createncprojection();
    projection->leaf = var;
    projection->segments = segments;

    /* All slices are assigned to the first (and only segment) */
    dimset = var->array.dimensions;
    segment->slicerank = nclistlength(var->array.dimensions);
    for(i=0;i<segment->slicerank;i++) {
        NCslice* slice = &segment->slices[i];
	CDFnode* dim = (CDFnode*)nclistget(dimset,i);
        slice->first = startp[i];
	slice->stride = stridep[i];
	slice->count = countp[i];
        slice->length = slice->count * slice->stride;
	slice->stop = (slice->first + slice->length);
	ASSERT(dim->dim.declsize > 0);
    	slice->declsize = dim->dim.declsize;
    }
    segment->slicesdefined = 1;

    if(projectionp) *projectionp = projection;
    if(ncstat) freencprojection1(projection);
    return ncstat;
}

/* Compute the set of prefetched data */
NCerror
prefetchdata4(NCDRNO* drno)
{
    int i,j;
    NCerror ncstat = NC_NOERR;
    NClist* allvars = drno->cdf.varnodes;
    NCconstraint* constraint = &drno->dap.constraint;
    NClist* vars = nclistnew();
    NCcachenode* cache = NULL;
    NCconstraint newconstraint;

    /* If caching is off, and we can do constraints, then
       don't even do prefetch
    */
    if(!FLAGSET(drno,NCF_CACHE) && !FLAGSET(drno,NCF_UNCONSTRAINABLE)) {
	drno->cdf.cache.prefetch = NULL;
	goto done;
    }

    for(i=0;i<nclistlength(allvars);i++) {
	CDFnode* var = (CDFnode*)nclistget(allvars,i);
	size_t nelems = 1;
	/* Compute the # of elements in the variable */
	for(j=0;j<nclistlength(var->array.dimensions);j++) {
	    CDFnode* dim = (CDFnode*)nclistget(var->array.dimensions,j);
	    nelems *= dim->dim.declsize;
	}
	/* If we cannot constrain, then pull in everything */
	if(FLAGSET(drno,NCF_UNCONSTRAINABLE)
           || nelems <= drno->cdf.smallsizelimit)
	    nclistpush(vars,(ncelem)var);
    }

    /* If we cannot constrain, then pull in everything */
    if(FLAGSET(drno,NCF_UNCONSTRAINABLE)) {
	newconstraint.projections = NULL;
	newconstraint.selections= NULL;
    } else { /* Construct the projections for this set of vars */
        /* Construct the projections for this set of vars */
        /* Initially, the constraints are same as the merged constraints */
        newconstraint.projections = cloneprojections(constraint->projections);
        restrictprojection3(drno,vars,newconstraint.projections);
        /* similar for selections */
        newconstraint.selections = cloneselections(constraint->selections);
    }

    ncstat = buildcachenode3(drno,&newconstraint,vars,&cache,0);
    if(ncstat) goto done;

if(FLAGSET(drno,NCF_SHOWFETCH)) {
/* Log the set of prefetch variables */
NCbytes* buf = ncbytesnew();
ncbytescat(buf,"prefetch.vars: ");
for(i=0;i<nclistlength(vars);i++) {
CDFnode* var = (CDFnode*)nclistget(vars,i);
ncbytescat(buf," ");
ncbytescat(buf,makesimplepathstring3(var));
}
ncbytescat(buf,"\n");
oc_log(OCLOGNOTE,ncbytescontents(buf));
ncbytesfree(buf);
}

done:
    if(ncstat) {
	freenccachenode(drno,cache);
    }
    return THROW(ncstat);
}

#ifdef IGNORE
/* Based on the tactic, determine the set of variables to add */
static void
computevarset4(NCDRNO* drno, Getvara* getvar, NClist* varlist)
{
    int i;
    nclistclear(varlist);
    for(i=0;i<nclistlength(drno->cdf.varnodes);i++) {
	CDFnode* var = (CDFnode*)nclistget(drno->cdf.varnodes,i);
#ifdef IGNORE
	int ok = 1;
	for(j=0;j<nclistlength(var->array.ncdimensions);j++) {
	    CDFnode* dim = (CDFnode*)nclistget(var->array.ncdimensions,j);
	    if(dim->dim.declsize == NC_UNLIMITED) {ok = 0; break;}
	}
	if(!ok) continue;
#endif
        switch (getvar->tactic->tactic) {
        case tactic_all: /* add all visible variables */
	    nclistpush(varlist,(ncelem)var);
	    break;	    
        case tactic_partial: /* add only small variables + target */
	    if(var->estimatedsize < drno->cdf.smallsizelimit
	       || getvar->target == var) {
		nclistpush(varlist,(ncelem)var);
	    }
	    break;	    
        case tactic_var: /* add only target var */
	    if(getvar->target == var) nclistpush(varlist,(ncelem)var);
	    break;	    
	default: break;
	}
    }
}
#endif

