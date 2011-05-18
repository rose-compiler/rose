/*********************************************************************
 *   Copyright 1993, UCAR/Unidata
 *   See netcdf/COPYRIGHT file for copying and redistribution conditions.
 *   $Header: /upc/share/CVS/netcdf-3/libncdap3/getvara3.c,v 1.40 2009/12/03 18:53:16 dmh Exp $
 *********************************************************************/
#include "ncdap3.h"
#include "dapodom.h"
#include "dapdump.h"
#include "dapdispatch3.h"

#define NEWVARM

/* Forward:*/
static NCerror moveto(NCDRNO*, Getvara*, CDFnode* dataroot, void* memory);
static NCerror movetor(NCDRNO*, OCdata currentcontent,
		   NClist* path, int depth,
		   Getvara*, int dimindex,
		   struct NCMEMORY*, NClist* segments);

static int findfield(CDFnode* node, CDFnode* subnode);
static NCerror slicestring(OCconnection, OCdata, size_t, NCslice*, struct NCMEMORY*);
static int wholeslicepoint(Dapodometer* odom);
static void removepseudodims3(NCprojection* clone);

NCerror
nc3d_getvarx(int ncid, int varid,
	    const size_t *startp,
	    const size_t *countp,
	    const ptrdiff_t* stridep,
	    void *data,
	    nc_type dsttype0)
{
    NCerror ncstat = NC_NOERR;
    OCerror ocstat = OC_NOERR;
    int i;
    NC* ncp;
    NCDRNO* drno;
    NC_var* var;
    CDFnode* cdfvar; /* cdf node mapping to var*/
    NClist* varnodes;
    nc_type dsttype;
    Getvara* varainfo = NULL;
    CDFnode* xtarget = NULL; /* target in DATADDS */
    CDFnode* target = NULL; /* target in constrained DDS */
    NCprojection* varaprojection = NULL;
    NCcachenode* cachenode = NULL;
    size_t localcount[NC_MAX_DIMS];
    NClist* vars = nclistnew();
    NCconstraint constraint = {NULL,NULL};

    ncstat = NC_check_id(ncid, &ncp); 
    if(ncstat != NC_NOERR) goto fail;

    drno = ncp->drno;
    
    var = NC_lookupvar(ncp,varid);
    if(var == NULL) {ncstat = NC_ENOTVAR; goto fail;}

    /* Locate var node via varid */
    varnodes = drno->cdf.varnodes;
    for(i=0;i<nclistlength(varnodes);i++) {
	CDFnode* node = (CDFnode*)nclistget(varnodes,i);
	if(node->array.basevar == NULL
           && node->nctype == NC_Primitive
           && node->ncid == varid) {
	    cdfvar = node;
	    break;
	}
    }

    ASSERT((cdfvar != NULL));
    ASSERT((strcmp(cdfvar->ncfullname,var->name->cp)==0));

    if(countp == NULL) {
        /* Accumulate the dimension sizes */
        NClist* ncdims = cdfvar->array.dimensions;
        size_t ncrank = nclistlength(ncdims);
        for(i=0;i<ncrank;i++) {
	    CDFnode* dim = (CDFnode*)nclistget(ncdims,i);
	    localcount[i] = dim->dim.declsize;
	}
	countp = localcount;
    }

#ifdef DEBUG
{ NClist* dims = cdfvar->array.dimensions;
fprintf(stderr,"getvarx: %s",cdfvar->ncfullname);
if(nclistlength(dims) > 0) {int i;
for(i=0;i<nclistlength(dims);i++) 
fprintf(stderr,"[%lu:%lu:%lu]",(unsigned long)startp[i],(unsigned long)countp[i],(unsigned long)stridep[i]);
fprintf(stderr," -> ");
for(i=0;i<nclistlength(dims);i++) 
if(stridep[i]==1)
fprintf(stderr,"[%lu:%lu]",(unsigned long)startp[i],(unsigned long)((startp[i]+countp[i])-1));
else
fprintf(stderr,"[%lu:%lu:%lu]",
(unsigned long)startp[i],
(unsigned long)stridep[i],
(unsigned long)(((startp[i]+countp[i])*stridep[i])-1));
}
fprintf(stderr,"\n");
}
#endif

    dsttype = (dsttype0);

    /* Default to using the inquiry type for this var*/
    if(dsttype == NC_NAT) dsttype = cdfvar->externaltype;

    /* Validate any implied type conversion*/
    if(cdfvar->etype != dsttype && dsttype == NC_CHAR) {
	/* The only disallowed conversion is to/from char and non-byte
           numeric types*/
	switch (cdfvar->etype) {
	case NC_STRING: case NC_URL:
	case NC_CHAR: case NC_BYTE: case NC_UBYTE:
	    break;
	default:
	    return THROW(NC_ECHAR);
	}
    }

    ncstat = makegetvar34(drno,cdfvar,data,dsttype,&varainfo);
    if(ncstat != NC_NOERR) {THROWCHK(ncstat); goto fail;}
#ifdef IGNORE
    freegetvara(drno->cdf.vara);
    drno->cdf.vara = varainfo;
#endif

    ncstat = buildvaraprojection3(drno,varainfo,startp,countp,stridep,&varaprojection);
    if(ncstat != NC_NOERR) {THROWCHK(ncstat); goto fail;}

    if(FLAGSET(drno,NCF_UNCONSTRAINABLE)) {
#ifdef DEBUG
fprintf(stderr,"Unconstrained: reusing prefetch\n");
#endif
	cachenode = drno->cdf.cache.prefetch;	
	ASSERT((cachenode != NULL));
    } else if(iscached(drno,varaprojection->leaf,&cachenode)) {
#ifdef DEBUG
fprintf(stderr,"Reusing cached fetch constraint: %s\n",
	dumpconstraint(&cachenode->constraint));
#endif
    } else { /* load with constraints */
	nclistpush(vars,(ncelem)varainfo->target);
        constraint.projections = cloneprojections(drno->dap.constraint.projections);
        if(!FLAGSET(drno,NCF_CACHE)) {
	    /* If we are not caching, then merge the getvara projections */
	    NClist* tmp = nclistnew();
	    NCprojection* clone = cloneprojection1(varaprojection);
	    /* We need to modify the clone to remove
               pseudo dimensions (i.e. string and sequence dimensions)
               because the server will not recognize them
	    */

	    removepseudodims3(clone);
	    nclistpush(tmp,(ncelem)clone);
            ncstat = mergeprojections3(drno,constraint.projections,tmp);
	    freencprojection1(clone);
	    nclistfree(tmp);
            if(ncstat != NC_NOERR) {THROWCHK(ncstat); goto fail;}
#ifdef DEBUG
fprintf(stderr,"vara merge: %s\n",
	dumpprojections(constraint.projections));
#endif
        }

        restrictprojection3(drno,vars,constraint.projections);
        constraint.selections = cloneselections(drno->dap.constraint.selections);

	/* buildcachenode3 will also fetch the corresponding datadds */
        ncstat = buildcachenode3(drno,&constraint,vars,&cachenode,0);
        if(ncstat != NC_NOERR) {THROWCHK(ncstat); goto fail;}

#ifdef DEBUG
fprintf(stderr,"cache.datadds=%s\n",dumptree(cachenode->datadds));
#endif
    }

    /* attach DATADDS to DDS */
    unattach34(drno->cdf.ddsroot);
    ncstat = attachsubset34(cachenode->datadds,drno->cdf.ddsroot);
    if(ncstat) goto fail;	

    /* Fix up varainfo to use the cache */
    varainfo->cache = cachenode;
    /* However use this current vara projection */
    varainfo->varaprojection = varaprojection;
    varaprojection = NULL;

    /* Now, walk to the relevant instance */

    /* Get the var correlate from the datadds */
    target = varainfo->target;
    xtarget = target->attachment;
    if(xtarget == NULL) 
	{THROWCHK(ncstat=NC_ENODATA); goto fail;}

    /* Switch to datadds tree space*/
    varainfo->target = xtarget;
    ncstat = moveto(drno,varainfo,cachenode->datadds,data);
    if(ncstat != NC_NOERR) {THROWCHK(ncstat); goto fail;}
    goto ok;
fail:
    if(ocstat != OC_NOERR) ncstat = ocerrtoncerr(ocstat);
    freencprojection1(varaprojection);
ok:
    clearncconstraint(&constraint);
    nclistfree(vars);
    if(!FLAGSET(drno,NCF_UNCONSTRAINABLE) && !FLAGSET(drno,NCF_CACHE))
	freenccachenode(drno,cachenode);
    freegetvara(varainfo);
    return THROW(ncstat);
}

static NCerror
moveto(NCDRNO* drno, Getvara* xgetvar, CDFnode* xrootnode, void* memory)
{
    OCerror ocstat = OC_NOERR;
    NCerror ncstat = NC_NOERR;
    OCconnection conn = drno->dap.conn;
    OCdata xrootcontent;
    OCobject ocroot;
    NClist* path = nclistnew();
    struct NCMEMORY memstate;

    memstate.next = (memstate.memory = memory);

    /* Get the root content*/
    ocroot = xrootnode->tree->ocroot;
    xrootcontent = oc_data_new(conn);
    ocstat = oc_data_root(conn,ocroot,xrootcontent);
    if(ocstat != OC_NOERR) {THROWCHK(ocstat); goto fail;}

    /* Remember: xgetvar->target is in DATADDS tree */
    collectnodepath3(xgetvar->target,path,WITHDATASET);
    ncstat = movetor(drno,xrootcontent,
                     path,0,xgetvar,0,&memstate,
                     xgetvar->varaprojection->segments);

fail:
    nclistfree(path);
    oc_data_free(conn,xrootcontent);
    if(ocstat != OC_NOERR) ncstat = ocerrtoncerr(ocstat);
    return THROW(ncstat);
}

static NCerror
movetor(NCDRNO* drno,
	OCdata currentcontent,
	NClist* path, int depth, /* depth is position in segment list*/
	Getvara* xgetvar, int dimindex, /* dimindex is position in xgetvar->slices*/
	struct NCMEMORY* memory,
	NClist* segments)
{
    int i;
    OCerror ocstat = OC_NOERR;
    NCerror ncstat = NC_NOERR;
    size_t fieldindex,gridindex,rank;
    OCconnection conn = drno->dap.conn;
    CDFnode* xnode = (CDFnode*)nclistget(path,depth);
    OCdata reccontent = OCNULL;
    OCdata dimcontent = OCNULL;
    OCdata fieldcontent = OCNULL;
    Dapodometer* odom = OCNULL;
    OCmode currentmode;
    CDFnode* xnext;
    int hasstringdim = 0;
    size_t dimoffset;
    NCslice stringslice;
    NCsegment* segment;
    int newdepth;
    int caching = FLAGSET(drno,NCF_CACHE);
    int unconstrainable = FLAGSET(drno,NCF_UNCONSTRAINABLE);
    int wholepoint;
    size_t externtypesize;
    size_t interntypesize;
    char* localmemory = NULL;
    size_t odomsubsize;
    size_t internlen;

    /* Note that we use depth-1 because the path contains the DATASET
       but the segment list does not */
    segment = (NCsegment*)nclistget(segments,depth-1);

    if(xnode->etype == NC_STRING || xnode->etype == NC_URL) hasstringdim = 1;

    ocstat = oc_data_mode(conn,currentcontent,&currentmode);

    /* Switch on the combination of nctype and mode */
#define CASE(nc1,nc2) (nc1*1024+nc2)

    switch (CASE(xnode->nctype,currentmode)) {

    default:
	PANIC2("Illegal combination: nctype=%d mode=%d",
		(int)xnode->nctype,(int)currentmode);
	break;

    case CASE(NC_Dataset,OCFIELDMODE):
    case CASE(NC_Grid,OCFIELDMODE):
    case CASE(NC_Structure,OCFIELDMODE):
    case CASE(NC_Sequence,OCFIELDMODE):
	/* Since these are never dimensioned, we can go directly to
	   the appropriate field; locate the field index for the next
	   item in the path.
	*/
	xnext = (CDFnode*)nclistget(path,depth+1);
	ASSERT((xnext != NULL));
	fieldindex = findfield(xnode,xnext);
	/* If the next node is a virtual node, then
	   we need to effectively
	   ignore it and use the appropriate subnode.
	   If the next node is a structuregrid node, then
	   use it as is.
	*/
        if(xnext->virtual) {
	    CDFnode* xgrid = xnext;
	    xnext = (CDFnode*)nclistget(path,depth+2); /* real node */
	    gridindex = fieldindex;
	    fieldindex = findfield(xgrid,xnext);
	    fieldindex += gridindex;
	    newdepth = depth+2;
	} else {
	    newdepth = depth+1;
	}
        fieldcontent = oc_data_new(conn);
        ocstat = oc_data_ith(conn,currentcontent,fieldindex,fieldcontent);
	if(ocstat != OC_NOERR) {THROWCHK(ocstat); goto fail;}
	ncstat = movetor(drno,fieldcontent,
                         path,newdepth,xgetvar,dimindex,memory,
			 segments);
	break;

    case CASE(NC_Structure,OCARRAYMODE):
        /* figure out which slices refer to this node:
           dimindex upto dimindex+rank; */
        rank = segment->slicerank;
	if(caching || unconstrainable) {
            odom = newdapodometer(segment->slices,0,rank);	    
	} else { /*Since vara was projected out, build a simple odometer*/
            odom = newsimpledapodometer(segment,rank);
	}
        while(dapodometermore(odom)) {
            OCmode mode;
            /* Compute which instance to move to*/
            dimoffset = dapodometercount(odom);
            dimcontent = oc_data_new(conn);
            ocstat = oc_data_ith(conn,currentcontent,dimoffset,dimcontent);
            if(ocstat != OC_NOERR) {THROWCHK(ocstat); goto fail;}
            ocstat = oc_data_mode(conn,dimcontent,&mode);
            ASSERT((mode == OCFIELDMODE));
            ncstat = movetor(drno,dimcontent,
                                 path,depth,
                                 xgetvar,dimindex+rank,
                                 memory,segments);
            dapodometerincr(odom);
        }
        freedapodometer(odom);
        break;

    case CASE(NC_Sequence,OCRECORDMODE): {
        NCslice* uslice;
        CDFnode* seqdim;
        ASSERT((currentmode == OCRECORDMODE));
        /* Get and check the corresponding sequence dimension from DDS */
        ASSERT((xnode->attachment != NULL));
        seqdim = (CDFnode*)nclistget(xnode->attachment->array.dimensions,0);
        ASSERT((seqdim != NULL));
	if(DIMFLAG(seqdim,CDFDIMUNLIM))
            {THROWCHK(ncstat = NC_ENODATA); goto fail;}         
        /* use uslice to walk the sequence; however, watch out
           for the case when the user set a limit and that limit
           is not actually reached in this request.
        */
        /* By construction, this sequence represents the first 
           (and only) dimension of this segment */
        uslice = &segment->slices[0];
        reccontent = oc_data_new(conn);
        for(i=uslice->first;i<uslice->stop;i+=uslice->stride) {
            ocstat = oc_data_ith(conn,currentcontent,i,reccontent);
            if(ocstat == OC_EINVALCOORDS) {
                /* We asked for too much */
                THROWCHK(ocstat);
                goto fail;
            } else if(ocstat != OC_NOERR) {
                THROWCHK(ocstat);
                goto fail;
            }
            ncstat = movetor(drno,reccontent,
                                 path,depth,
                                 xgetvar,dimindex+1,
                                 memory,segments);
            if(ncstat != OC_NOERR) {THROWCHK(ncstat); goto fail;}
        }
        } break;

    case CASE(NC_Primitive,OCSCALARMODE):
	externtypesize = nctypesizeof(xgetvar->dsttype);
        if(hasstringdim) {
	    /* Get the string dimension */
	    CDFnode* strdim = xnode->attachment->array.stringdim;
	    ASSERT((strdim != NULL));
	    stringslice = segment->slices[segment->slicerank-1];
  	    ncstat = slicestring(conn,currentcontent,0,&stringslice,memory);
	} else {
            /* Read the whole scalar directly into memory (with conversion) */
            char value[16]; /* to hold any value*/
            ocstat = oc_data_get(conn,currentcontent,value,sizeof(value),0,1);
            if(ocstat != OC_NOERR) {THROWCHK(ocstat); goto fail;}
            ncstat = dapconvert3(xnode->etype,xgetvar->dsttype,memory->next,value,1);
            if(ncstat != NC_NOERR) {THROWCHK(ncstat); goto fail;}
	    memory->next += (externtypesize);
	}
	break;

    case CASE(NC_Primitive,OCARRAYMODE): {
	NCslice stringslice;
	CDFnode* strdim;
        char value[16]; /* to hold any value*/

        rank = segment->slicerank;

        /* whole variable is assumed to always be available
	   (because currently we do not attempt to fold get_vars parameters
            into the fetch projections)
	*/
		
	if(caching || unconstrainable) {	
            if(hasstringdim) {
	        /* Get the string dimension */
	        stringslice = segment->slices[rank-1];
	        strdim = xnode->attachment->array.stringdim;
	        ASSERT((strdim != NULL));
                odom = newdapodometer(segment->slices,0,rank-1);
	    } else {
                odom = newdapodometer(segment->slices,0,rank);
	    }
	} else {/* ! caching */
            if(hasstringdim) {
	        /* Get the string dimension */
	        stringslice = segment->slices[rank-1];
	        strdim = xnode->attachment->array.stringdim;
	        ASSERT((strdim != NULL));
                odom = newsimpledapodometer(segment,rank-1);
	    } else {
                odom = newsimpledapodometer(segment,rank);
	    }
	}
        /* Optimize off the use of the odometer by checking the slicing
           to see if the whole variable, or some whole subslice
           is being extracted.
           However do not do this if the external type conversion is needed
           or hasstringdim or if the whole slice point is rank-1.
        */
	externtypesize = nctypesizeof(xgetvar->dsttype);
        interntypesize = nctypesizeof(xnode->etype);
	wholepoint = wholeslicepoint(odom);
	if(wholepoint == -1)
	    odomsubsize = 1; /* no whole point */
	else
            odomsubsize = dapodometerspace(odom,wholepoint);
	internlen = (odomsubsize*interntypesize);
	if(!hasstringdim && xnode->etype != xgetvar->dsttype) {
	    /* copy the data locally before conversion */
	    localmemory = (char*)emalloc(internlen);
	} else {
	    localmemory = NULL;
	}
        if(wholepoint == 0 && !hasstringdim) {/* whole variable */
            /* Read the whole n elements directly into memory.*/
	    if(localmemory == NULL) {
                ocstat = oc_data_get(conn,currentcontent,memory->next,
                                     internlen,0,odomsubsize);
                if(ocstat != OC_NOERR) {THROWCHK(ocstat); goto fail;}
	    } else {
                ocstat = oc_data_get(conn,currentcontent,localmemory,
                                     internlen,0,odomsubsize);
                if(ocstat != OC_NOERR) {THROWCHK(ocstat); goto fail;}
	        /* do conversion */
                ncstat = dapconvert3(xnode->etype,xgetvar->dsttype,
				     memory->next,localmemory,odomsubsize);
                if(ncstat != NC_NOERR) {THROWCHK(ncstat); goto fail;}
	    }
            memory->next += (externtypesize*odomsubsize);
        } else if(wholepoint > 0 && !hasstringdim) {/* whole subslice */
	    odom->rank = wholepoint; /* truncate */
            while(dapodometermore(odom)) {
                size_t dimoffset = dapodometercount(odom) * odomsubsize;
	        if(localmemory == NULL) {
                    ocstat = oc_data_get(conn,currentcontent,memory->next,
                                         internlen,dimoffset,odomsubsize);
                    if(ocstat != OC_NOERR) {THROWCHK(ocstat); goto fail;}
		} else {
                    ocstat = oc_data_get(conn,currentcontent,localmemory,
                                         internlen,dimoffset,odomsubsize);
                    if(ocstat != OC_NOERR) {THROWCHK(ocstat); goto fail;}
	            /* do conversion */
                    ncstat = dapconvert3(xnode->etype,xgetvar->dsttype,
				         memory->next,localmemory,odomsubsize);
                    if(ncstat != NC_NOERR) {THROWCHK(ncstat); goto fail;}
	        }
                memory->next += (externtypesize*odomsubsize);
                dapodometerincr(odom);
	    }
        } else { /* Oh well, use the odometer to walk to the
                    appropriate fields*/
            while(dapodometermore(odom)) {
                size_t dimoffset = dapodometercount(odom);
		if(hasstringdim) {
		    ncstat = slicestring(conn,currentcontent,dimoffset,
					 &stringslice,memory);
		    if(ncstat != NC_NOERR) {THROWCHK(ncstat); goto fail;}
		} else {
                    ocstat = oc_data_get(conn,currentcontent,value,sizeof(value),dimoffset,1);
                    if(ocstat != OC_NOERR) {THROWCHK(ocstat); goto fail;}
                    ncstat = dapconvert3(xnode->etype,xgetvar->dsttype,memory->next,value,1);
                    if(ncstat != NC_NOERR) {THROWCHK(ncstat); goto fail;}
                    memory->next += externtypesize;
		}
                dapodometerincr(odom);
            }
        }
        freedapodometer(odom);
	efree(localmemory);
        } break;
    }
    goto ok;

fail:
ok:
    oc_data_free(conn,dimcontent);
    oc_data_free(conn,fieldcontent);
    oc_data_free(conn,reccontent);
    if(ocstat != OC_NOERR) ncstat = ocerrtoncerr(ocstat);
    return THROW(ncstat);
}

/* Extract a slice of a string; many special cases to consider and optimize*/
static NCerror
slicestring(OCconnection conn, OCdata content, size_t dimoffset,
                NCslice* slice, struct NCMEMORY* memory)
{
    char*  stringmem = NULL;
    size_t stringlen;
    unsigned int i;
    OCerror ocstat = OC_NOERR;
    NCerror ncstat = NC_NOERR;

    /* Get the whole string into local memory*/
    ocstat = oc_data_get(conn,content,&stringmem,sizeof(stringmem),
                         dimoffset,1);
    if(ocstat != OC_NOERR) {THROWCHK(ocstat); goto fail;}
    /* libnc-dap chooses to convert string escapes to the corresponding
       character; so we do likewise.
    */
    dapexpandescapes(stringmem); 
    stringlen = strlen(stringmem);

/*
fprintf(stderr,"stringslice: %d string=|%s|\n",stringlen,stringmem);
fprintf(stderr,"stringslice: slice=[%lu:%lu:%lu/%lu]\n",
slice->first,slice->stride,slice->stop,slice->declsize);
*/

    /* Stride across string; if we go past end of string, then pad*/
    for(i=slice->first;i<slice->length;i+=slice->stride) {
            if(i < stringlen)
                *memory->next++ = stringmem[i];
            else /* i >= stringlen*/
                *memory->next++ = NC_FILL_CHAR;
    }

fail:
    if(stringmem != NULL) efree(stringmem);
    if(ocstat != OC_NOERR) ncstat = ocerrtoncerr(ocstat);
    return THROW(ncstat);
}

/* Determine the index in the odometer at which
   the odometer will be walking the whole subslice
   This will allow us to optimize.
*/
static int 
wholeslicepoint(Dapodometer* odom)
{
    unsigned int i;
    int point;
    for(point=-1,i=0;i<odom->rank;i++) {
        ASSERT((odom->slices[i].declsize != 0));
        if(odom->slices[i].first != 0 || odom->slices[i].stride != 1
           || odom->slices[i].length != odom->slices[i].declsize)
	    point = i;
    }
    if(point == -1)
	point = 0; /* wholevariable */
    else if(point == (odom->rank - 1)) 
	point = -1; /* no whole point */
    else
	point += 1; /* intermediate point */
    return point;
}

#ifdef UNUSED
static int
samevarinfo(Getvara* v1, Getvara* v2)
{
    unsigned int i;
    NCslice *s1, *s2;
    if(v1 == NULL || v2 == NULL) return 0;
    if(v1->target != v2->target) return 0;
    if(v1->nslices != v2->nslices) return 0;
    s1 = v1->slices;
    s2 = v2->slices;
    for(i=0;i<v1->nslices;i++) {
        if(s1->first != s2->first) return 0;
        if(s1->count != s2->count) return 0;
    }
    return 1;
}
#endif


static int
findfield(CDFnode* node, CDFnode* field)
{
    size_t i;
    for(i=0;i<nclistlength(node->subnodes);i++) {
        CDFnode* test = (CDFnode*) nclistget(node->subnodes,i);
        if(test == field) return i;
    }
    return -1;
}

static void
removepseudodims3(NCprojection* clone)
{
    int i;
    int nsegs;
    NCsegment* seg;

    ASSERT((clone != NULL));
    nsegs = nclistlength(clone->segments);

    /* 1. scan for sequences and remove
	  any index projections. */
    for(i=0;i<nsegs;i++) {
	seg = (NCsegment*)nclistget(clone->segments,i);
	if(seg->node->nctype != NC_Sequence) continue; /* not a sequence */
	seg->slicerank = 0;
    }

    /* 2. Check the terminal segment to see if it is a String primitive,
          and if so, then remove the string dimension */
    ASSERT((nsegs > 0));
    seg = (NCsegment*)nclistget(clone->segments,nsegs-1);
    /* See if the node has a string dimension */
    if(seg->node->nctype == NC_Primitive
       && seg->node->array.stringdim != NULL) {
	/* Remove the string dimension projection from the segment */
	seg->slicerank--; /* always the last */
    }
}

int
nc3d_getvarmx(int ncid, int varid,
	    const size_t *start,
	    const size_t *edges,
	    const ptrdiff_t* stride,
 	    const ptrdiff_t* map,
	    void* data,
	    nc_type dsttype0)
{
    NCerror ncstat = NC_NOERR;
    int i;
    NC* ncp;
    NCDRNO* drno;
    NC_var* var;
    CDFnode* cdfvar; /* cdf node mapping to var*/
    NClist* varnodes;
    nc_type dsttype;
    size_t externsize;
    size_t dimsizes[NC_MAX_DIMS];
    Dapodometer* odom = NULL;
    unsigned int ncrank;
    NClist* ncdims = NULL;
    size_t nelems;
#ifdef NEWVARM
    char* localcopy; /* of whole variable */
#endif

    ncstat = NC_check_id(ncid, &ncp); 
    if(ncstat != NC_NOERR) goto done;

    drno = ncp->drno;
    
    var = NC_lookupvar(ncp,varid);
    if(var == NULL) {ncstat = NC_ENOTVAR; goto done;}

    /* Locate var node via varid */
    varnodes = drno->cdf.varnodes;
    for(i=0;i<nclistlength(varnodes);i++) {
	CDFnode* node = (CDFnode*)nclistget(varnodes,i);
	if(node->array.basevar == NULL
           && node->nctype == NC_Primitive
           && node->ncid == varid) {
	    cdfvar = node;
	    break;
	}
    }

    ASSERT((cdfvar != NULL));
    ASSERT((strcmp(cdfvar->ncfullname,var->name->cp)==0));

    if(nclistlength(cdfvar->array.dimensions) == 0) {
       /* The variable is a scalar; consequently, there is only one
          thing to get and only one place to put it.  (Why was I
          called?) */
       return nc3d_getvar(ncid, varid, data, dsttype0);
    }
         
    dsttype = (dsttype0);

    /* Default to using the inquiry type for this var*/
    if(dsttype == NC_NAT) dsttype = cdfvar->externaltype;

    /* Validate any implied type conversion*/
    if(cdfvar->etype != dsttype && dsttype == NC_CHAR) {
	/* The only disallowed conversion is to/from char and non-byte
           numeric types*/
	switch (cdfvar->etype) {
	case NC_STRING: case NC_URL:
	case NC_CHAR: case NC_BYTE: case NC_UBYTE:
 	    break;
	default:
	    return THROW(NC_ECHAR);
	}
    }

    externsize = nctypesizeof(dsttype);

    /* Accumulate the dimension sizes and the total # of elements */
    ncdims = cdfvar->array.dimensions;
    ncrank = nclistlength(ncdims);

    nelems = 1; /* also Compute the number of elements being retrieved */
    for(i=0;i<ncrank;i++) {
	CDFnode* dim = (CDFnode*)nclistget(ncdims,i);
	dimsizes[i] = dim->dim.declsize;
	nelems *= edges[i];
    }

    /* Originally, this code repeatedly extracted single values
       using get_var1. In an attempt to improve performance,
       I have converted to reading the whole variable at once
       and walking it locally.
    */

#ifdef NEWVARM
    localcopy = (char*)emalloc(nelems*externsize);

    /* We need to use the varieties of get_vars in order to
       properly do conversion to the external type
    */

    switch (dsttype) {

    case NC_CHAR:
	ncstat = nc3d_get_vars_text(ncid,varid,start, edges, stride,
				  (char*)localcopy);
	break;
    case NC_BYTE:
	ncstat = nc3d_get_vars_schar(ncid,varid,start, edges, stride,
				   (signed char*)localcopy);
	break;
    case NC_SHORT:
	ncstat = nc3d_get_vars_short(ncid,varid, start, edges, stride,
			  	   (short*)localcopy);
	break;
    case NC_INT:
	ncstat = nc3d_get_vars_int(ncid,varid,start, edges, stride,
				 (int*)localcopy);
	break;
    case NC_FLOAT:
	ncstat = nc3d_get_vars_float(ncid,varid,start, edges, stride,
				   (float*)localcopy);
	break;
    case NC_DOUBLE:
	ncstat = nc3d_get_vars_double(ncid,varid,	start, edges, stride,
		 		    (double*)localcopy);
	break;
    default: break;
    }

    odom = newdapodometer2(start,edges,stride,0,ncrank);

    /* Walk the local copy */
    for(i=0;i<nelems;i++) {
	size_t voffset = dapodometervarmcount(odom,map,dimsizes);
	void* dataoffset = (void*)(((char*)data) + (externsize*voffset));
	char* localpos = (localcopy + externsize*i);
	/* extract the indexset'th value from local copy */
	memcpy(dataoffset,(void*)localpos,externsize);
/*
fprintf(stderr,"new: %lu -> %lu  %f\n",
	(unsigned long)(i),
        (unsigned long)voffset,
	*(float*)localpos);
*/
	dapodometerincr(odom);
    }    
#else
    odom = newdapodometer2(start,edges,stride,0,ncrank);
    while(dapodometermore(odom)) {
	size_t* indexset = dapodometerindices(odom);
	size_t voffset = dapodometervarmcount(odom,map,dimsizes);
	char internalmem[128];
	char externalmem[128];
	void* dataoffset = (void*)(((char*)data) + (externsize*voffset));

	/* get the indexset'th value using variable's internal type */
	ncstat = nc3d_get_var1(ncid,varid,indexset,(void*)&internalmem);
        if(ncstat != NC_NOERR) goto done;
	/* Convert to external type */
	ncstat = dapconvert3(cdfvar->etype,dsttype,externalmem,internalmem);
        if(ncstat != NC_NOERR) goto done;
	memcpy(dataoffset,(void*)externalmem,externsize);
/*
fprintf(stderr,"old: %lu -> %lu  %f\n",
	(unsigned long)dapodometercount(odom),
        (unsigned long)voffset,
	*(float*)externalmem);
*/
	dapodometerincr(odom);
    }    
#endif

done:
    return ncstat;
}
