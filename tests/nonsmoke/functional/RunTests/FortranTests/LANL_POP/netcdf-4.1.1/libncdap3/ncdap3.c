/*********************************************************************
 *   Copyright 1993, UCAR/Unidata
 *   See netcdf/COPYRIGHT file for copying and redistribution conditions.
 *   $Header: /upc/share/CVS/netcdf-3/libncdap3/ncdap3.c,v 1.82 2010/04/02 23:18:41 dmh Exp $
 *********************************************************************/
#include "config.h"
#include "ncdap3.h"
#include "dispatch3.h"
#include "dapdispatch3.h"
#include "dapalign.h"
#include "daprename.h"
#include "oc.h"
#include "ocdrno.h"
#include "dapdump.h"

#ifdef HAVE_GETRLIMIT
#include <sys/time.h>
#include <sys/resource.h>
#endif

extern CDFnode* v4node;
int nc3dinitialized = 0;

#define DFALTMODELFLAGS (NCF_NC3|NCF_NCDAP)

struct NCTMODEL nctmodels[] = {
{3,NULL,		(NCF_NC3|NCF_NCDAP)},
{4,NULL,		(NCF_NC4|NCF_VLEN)},
{0,NULL,0}
};


/*Forward*/
static NCerror buildncstructures(NCDRNO*);
static NCerror builddims(NCDRNO*);
static NCerror buildvars(NCDRNO*);
static NCerror buildglobalattrs3(NCDRNO*, int ncid, CDFnode* root);
static NCerror buildattribute3a(NCDRNO*, NCattribute* att, nc_type, int varid, int ncid);
static NCerror addstringdims(NCDRNO* drno);
static NCerror defseqdims(NCDRNO* drno);
static NCerror getseqdimsize(NCDRNO* drno, CDFnode* seq, size_t* sizep);
static int fieldindex(CDFnode* parent, CDFnode* child);
static NCerror countsequence(NCDRNO*, CDFnode* node, size_t*);
static NCerror makeseqdim(NCDRNO* drno, CDFnode* node, size_t, CDFnode**);
static NCerror computeminconstraints3(NCDRNO*,CDFnode*,NCbytes*);
#ifndef PSEUDOFILE
static NCerror createnciofile3(NC* ncp);
#endif
static NCerror showprojection3(NCDRNO* drno, CDFnode* var);
static void estimatevarsizes3(NCDRNO* drno);
static NCerror suppressunusablevars3(NCDRNO* drno);
static NCerror fixzerodims3(NCDRNO* drno);
static void applyclientparamcontrols3(NCDRNO* drno);
static NCerror defrecorddim3(NCDRNO* drno);
static NClist* getalldims3(NClist* vars, int visibleonly);


#define getncid(ncp) (((NC*)(ncp))->nciop->fd)

/**************************************************/
/* Add an extra function whose sole purpose is to allow
   configure(.ac) to test for the presence of thiscode.
*/
int nc__opendap(void) {return 0;}

/**************************************************/


int
nc3d_open(const char* path, int mode, int* ncidp)
{
    NC *ncp = NULL;
    NCerror ncstat = NC_NOERR;
    OCerror ocstat = OC_NOERR;
    NCDRNO* drno = NULL;
    char* modifiedpath;
    DAPURL tmpurl;
    char* ce = NULL;
    int ncid = -1;
    const char* value;
#ifdef PSEUDOFILE
    int fd;
    char* tmpname = NULL;
#endif

    if(!nc3dinitialized) nc3dinitialize();
#ifdef DEBUG
extern int ocdebug;
ocdebug = 1;
#endif

    if(!dapurlparse(path,&tmpurl)) {
	dapurlclear(&tmpurl);
	ncstat = nc3d__open_mp(path,mode,0,NULL,ncidp);
	return THROW(ncstat);
    }
    dapurlclear(&tmpurl); /* no longer needed */

#ifdef OCCOMPILEBYDEFAULT
    /* set the compile flag by default */
    modifiedpath = (char*)emalloc(strlen(path)+strlen("[compile]")+1);
    strcpy(modifiedpath,"[compile]");
    strcat(modifiedpath,path);    
#else
    modifiedpath = nulldup(path);
#endif

    /* Setup tentative DRNO state*/
    drno = (NCDRNO*)emalloc(sizeof(NCDRNO));
    MEMCHECK(drno,NC_ENOMEM);
    memset((void*)drno,0,sizeof(NCDRNO));
    drno->dap.urltext = modifiedpath;
    dapurlparse(drno->dap.urltext,&drno->dap.url);
    if(!constrainable34(&drno->dap.url))
	SETFLAG(drno,NCF_UNCONSTRAINABLE);
    drno->cdf.separator = ".";
    drno->cdf.smallsizelimit = DFALTSMALLLIMIT;
    drno->cdf.cache.cachelimit = DFALTCACHELIMIT;
    drno->cdf.cache.cachesize = 0;
    drno->cdf.cache.nodes = nclistnew();
    drno->cdf.cache.cachecount = DFALTCACHECOUNT;
#ifdef HAVE_GETRLIMIT
    { struct rlimit rl;
      if(getrlimit(RLIMIT_NOFILE, &rl) >= 0) {
	drno->cdf.cache.cachecount = (size_t)(rl.rlim_cur / 2);
      }
    }
#endif

    /* process control client parameters */
    applyclientparamcontrols3(drno);

#ifdef PSEUDOFILE
    tmpname = nulldup(PSEUDOFILE);
    fd = mkstemp(tmpname);
    if(fd < 0) {THROWCHK(errno); goto fail;}
    /* Now, use the file to create the netcdf file */
    if(sizeof(size_t) == sizeof(unsigned int))
        ncstat = RENAMEDAP(_create_mp)(tmpname,0,0,0,NULL,&ncid);
    else
        ncstat = RENAMEDAP(_create_mp)(tmpname,NC_64BIT_OFFSET,0,0,NULL,&ncid);
    /* free the original fd */
    close(fd);
    /* unlink the temp file so it will automatically be reclaimed */
    unlink(tmpname);
    efree(tmpname);
    /* Avoid fill */
    RENAME3(set_fill)(ncid,NC_NOFILL,NULL);
    if(ncstat)
	{THROWCHK(ncstat); goto fail;}
    /* Find our metadata for this file. */
    ncstat = NC_check_id(ncid, &ncp); 
    if(ncstat)
	{THROWCHK(ncstat); goto fail;}
#else
    /* Set up the ncp structure*/
    ncp = drno_new_NC(NULL);
    if(ncp == NULL) {ncstat = THROW(NC_ENOMEM); goto fail;}
    
    /* Create and open an arbitrary file in /tmp
       to use as our ncio file; unlink immediately.
    */
    /* set up ncio file */
    ncstat = createnciofile3(ncp);
    if(ncstat != NC_NOERR) {THROWCHK(ncstat); goto fail;}

    /* Mark as indef state*/
    fSet(ncp->flags, NC_INDEF);
    /* Mark as writable (until after the schema is established) */
    fSet(ncp->nciop->ioflags, NC_WRITE);

    /* Make sure this ncp is visible before defining nc schema*/
    drno_add_to_NCList(ncp);

    ncid = ncp->nciop->fd;
#endif

    ncp->dispatch = &ncdap3lib;
    ncp->drno = drno;
    drno->controller = (void*)ncp; /* cross link*/

    /* Presume a DAP URL*/
    ocstat = oc_open(drno->dap.urltext,&drno->dap.conn);
    if(ocstat != OC_NOERR) {THROWCHK(ocstat); goto fail;}

    if(paramcheck34(drno,"show","fetch"))
	drno->controls.flags |= NCF_SHOWFETCH;

    /* Turn on logging; only do this after oc_open*/
    value = oc_clientparam_get(drno->dap.conn,"log");
    if(value != NULL) {
	oc_loginit();
        oc_setlogging(1);
        oc_logopen(value);
    }

    /* fetch and build the (almost) unconstrained DDS for use as
       template */
    ncstat = fetchtemplatemetadata3(drno);
    if(ncstat != NC_NOERR) goto fail;

    /* Processing the constraints is a multi-step action.
       1. retrieve the dds
       2. convert the nc constraint names to be dap constraint
          names
       3. parse and merge the dap and nc constraints
    */
    ncstat = buildconstraints3(drno);
    if(ncstat != NC_NOERR) goto fail;

    /* fetch and build the constrained DDS */
    ncstat = fetchconstrainedmetadata3(drno);
    if(ncstat != NC_NOERR) goto fail;

    /* The following actions are WRT to the
	constrained tree */

    /* Accumulate useful nodes sets  */
    ncstat = computecdfnodesets3(drno);
    if(ncstat) {THROWCHK(ncstat); goto fail;}

    /* Fix grids */
    ncstat = fixgrids3(drno);
    if(ncstat) {THROWCHK(ncstat); goto fail;}

    /* Locate and mark usable sequences */
    ncstat = sequencecheck3(drno);
    if(ncstat) {THROWCHK(ncstat); goto fail;}

    /* Conditionally suppress variables not in usable
       sequences */
    if(FLAGSET(drno,NCF_NOUNLIM)) {
        ncstat = suppressunusablevars3(drno);
        if(ncstat) {THROWCHK(ncstat); goto fail;}
    }

    /* apply client parameters (after computcdfinfo and computecdfvars)*/
    ncstat = applyclientparams34(drno);
    if(ncstat) {THROWCHK(ncstat); goto fail;}

    /* Add (as needed) string dimensions*/
    ncstat = addstringdims(drno);
    if(ncstat) {THROWCHK(ncstat); goto fail;}

    if(nclistlength(drno->cdf.seqnodes) > 0) {
	/* Build the sequence related dimensions */
        ncstat = defseqdims(drno);
        if(ncstat) {THROWCHK(ncstat); goto fail;}
    }

    /* Build a cloned set of dimensions for every variable */
    ncstat = clonecdfdims34(drno);
    if(ncstat) {THROWCHK(ncstat); goto fail;}

    /* Re-compute the dimension names*/
    ncstat = computecdfdimnames34(drno);
    if(ncstat) {THROWCHK(ncstat); goto fail;}

    /* Deal with zero size dimensions */
    ncstat = fixzerodims3(drno);
    if(ncstat) {THROWCHK(ncstat); goto fail;}

    if(nclistlength(drno->cdf.seqnodes) == 0
       && drno->cdf.recorddim != NULL) {
	/* Attempt to use the DODS_EXTRA info to turn
           one of the dimensions into unlimited. Can only do it
           in a sequence free DDS.
        */
        ncstat = defrecorddim3(drno);
        if(ncstat) {THROWCHK(ncstat); goto fail;}
   }

    /* Re-compute the var names*/
    ncstat = computecdfvarnames3(drno,drno->cdf.ddsroot,drno->cdf.varnodes);
    if(ncstat) {THROWCHK(ncstat); goto fail;}

    /* Estimate the variable sizes */
    estimatevarsizes3(drno);

    ncstat = buildncstructures(drno);
    if(ncstat != NC_NOERR) {
        drno_del_from_NCList(ncp); /* undefine here */
	{THROWCHK(ncstat); goto fail;}
    }

    /* Do any necessary data prefetch */
    ncstat = prefetchdata3(drno);
    if(ncstat != NC_NOERR) {
        drno_del_from_NCList(ncp); /* undefine here */
	{THROWCHK(ncstat); goto fail;}
    }

    /* Mark as no longer indef */
    fClr(ncp->flags, NC_INDEF);
    /* Mark as no longer writable */
    fClr(ncp->nciop->ioflags, NC_WRITE);

    *ncidp = ncid;
    return THROW(NC_NOERR);

fail:
    if(ncp != NULL) {
#ifndef PSEUDOFILE
        ncio_close(ncp->nciop,1);
	ncp->nciop = NULL;
   	drno_free_NC(ncp);
#endif
    }
    if(ce) efree(ce);
    if(drno != NULL) freeNCDRNO3(drno);
    if(ocstat != OC_NOERR) ncstat = ocerrtoncerr(ocstat);
    return THROW(ncstat);
}

int
nc3d_close(int ncid)
{
    int ncstatus = NC_NOERR;
    NCDRNO* drno;
    NC *ncp; 

    ncstatus = NC_check_id(ncid, &ncp); 
    if(ncstatus != NC_NOERR) return THROW(ncstatus);

    oc_logclose();

    drno = ncp->drno;
    freeNCDRNO3(drno);

#ifdef PSEUDOFILE
    RENAMEDAP(abort)(ncid);    
#else
    (void) ncio_close(ncp->nciop, 0); /* should also close the fd*/
    ncp->nciop = NULL;
    drno_del_from_NCList(ncp);
    drno_free_NC(ncp);
#endif

    return THROW(ncstatus);
}

void
nc3dinitialize(void)
{
    dapdispatch3init();
    compute_nccalignments();
    nc3dinitialized = 1;
}

#ifndef PSEUDOFILE
static NCerror
createnciofile3(NC* ncp)
{
    NCerror ncstat = NC_NOERR;
    char name[1024];
    NCDRNO* drno = ncp->drno;
    strcpy(name,NCIOTEMP);
    /* Note Potential problem: old versions of this function
       leave the file in mode 0666 instead of 0600 */
    drno->nciofd = mkstemp(name);
    if(drno->nciofd < 0) {THROWCHK(errno); goto fail;}
    drno->nciofile = nulldup(name); /* remember our tmp file name */
    /* Now, use the file for ncio */
    ncstat = drno_ncio_open(ncp,name,0); /* NOCLOBBER */
    if(ncstat != NC_NOERR) goto fail;
    /* unlink the temp file so it will automatically be reclaimed */
    unlink(drno->nciofile);
fail:
    return ncstat;
}
#endif

void
freegetvara(Getvara* vara)
{
    if(vara == NULL) return;
    freencprojection1(vara->varaprojection);
    efree(vara);
}

NCerror
freeNCDRNO3(NCDRNO* drno)
{
#ifdef IGNORE
    freegetvara(drno->cdf.vara);
#endif
    clearnccache(drno,&drno->cdf.cache);
    nclistfree(drno->cdf.cache.nodes);
    nclistfree(drno->cdf.varnodes);
    nclistfree(drno->cdf.seqnodes);
    nclistfree(drno->cdf.gridnodes);
    nclistfree(drno->cdf.usertypes);
    efree(drno->cdf.recorddim);

    /* free the trees */
    freecdfroot34(drno->cdf.ddsroot);
    drno->cdf.ddsroot = NULL;

    oc_close(drno->dap.conn); /* also reclaims remaining OC trees */
    dapurlclear(&drno->dap.url);
    efree(drno->dap.urltext);

    clearncconstraint(&drno->dap.constraint);
    clearncconstraint(&drno->dap.dapconstraint);
#ifdef NCCONSTRAINTS
    clearncconstraint(&drno->dap.netcdfconstraint);
#endif

    if(drno->nciofile != NULL) {
	efree(drno->nciofile);
	close(drno->nciofd);	
    }

    efree(drno);
    return NC_NOERR;
}


#ifdef IGNORE
/* Given a path, collect the set of dimensions along that path */
static void
collectdims3(NClist* path, NClist* dimset)
{
    int i,j;
    nclistclear(dimset);
    for(i=0;i<nclistlength(path);i++) {
	CDFnode* node = (CDFnode*)nclistget(path,i);
	if(node->nctype == NC_Sequence) {
	    CDFnode* sqdim = (CDFnode*)nclistget(node->array.dimensions,0);
	    if(DIMFLAG(sqdim,CDFDIMUNLIM))
		nclistclear(dimset); /* unlimited is always first */
        }
	for(j=0;j<nclistlength(node->array.dimensions);j++) {
	    CDFnode* dim = (CDFnode*)nclistget(node->array.dimensions,j);
	    nclistpush(dimset,(ncelem)dim);
	}
	if(node->array.stringdim != NULL) 
	    nclistpush(dimset,(ncelem)node->array.stringdim);
    }
}
#endif

static NCerror
addstringdims(NCDRNO* drno)
{
    /* for all variables of string type, we will need another dimension
       to represent the string; Accumulate the needed sizes and create
       the dimensions with a specific name: either as specified
       in DODS{...} attribute set or defaulting to the variable name.
       All such dimensions are global.
    */
    int i;
    NClist* varnodes = drno->cdf.varnodes;
    for(i=0;i<nclistlength(varnodes);i++) {
	CDFnode* var = (CDFnode*)nclistget(varnodes,i);
	CDFnode* sdim = NULL;
	char dimname[4096];
	size_t dimsize;

	if(var->etype != NC_STRING && var->etype != NC_URL) continue;
	/* check is a string length was specified */
	if(var->dodsspecial.maxstrlen > 0)
	    dimsize = var->dodsspecial.maxstrlen;
	else
	    dimsize = var->maxstringlength;
	/* create a psuedo dimension for the charification of the string*/
	if(var->dodsspecial.dimname != NULL) {
	    strncpy(dimname,var->dodsspecial.dimname,sizeof(dimname));
	} else {
	    snprintf(dimname,sizeof(dimname),"maxStrlen%lu",
			(unsigned long)dimsize);
	}
	sdim = makecdfnode34(drno, dimname, OC_Dimension, OCNULL,
                             drno->cdf.ddsroot);
	if(sdim == NULL) return THROW(NC_ENOMEM);
	nclistpush(drno->cdf.ddsroot->tree->nodes,(ncelem)sdim);
	sdim->dim.dimflags |= CDFDIMSTRING;
	sdim->dim.declsize = dimsize;
	efree(sdim->ncbasename);
	efree(sdim->ncfullname);
	sdim->ncbasename = cdflegalname3(sdim->name);
	sdim->ncfullname = nulldup(sdim->ncbasename);
	/* tag the variable with its string dimension*/
	var->array.stringdim = sdim;
    }
    return NC_NOERR;
}

static NCerror
defrecorddim3(NCDRNO* drno)
{
    unsigned int i;
    NCerror ncstat = NC_NOERR;
    NClist* alldims;

    ASSERT((drno->cdf.recorddim != NULL));

    /* Locate the dimension matching the record dim */
    alldims = getalldims3(drno->cdf.varnodes,1);
    for(i=0;i<nclistlength(alldims);i++) {
        CDFnode* dim = (CDFnode*)nclistget(alldims,i);
	if(dim->nctype != NC_Dimension) continue;    
	if(dim->dim.basedim != NULL) continue; /* not the controlling dim */
	if(strcmp(dim->name,drno->cdf.recorddim) != 0) continue;
	if(DIMFLAG(dim,CDFDIMCLONE)) PANIC("cloned record dim");
	if(drno->cdf.unlimited != NULL) PANIC("Multiple unlimited");
        DIMFLAGSET(dim,CDFDIMUNLIM|CDFDIMRECORD);
	drno->cdf.unlimited = dim;
    }
    nclistfree(alldims);
    /* Now, locate all the string dims and see if they are the record dim,
       then replace */
    if(drno->cdf.unlimited != NULL) {
	CDFnode* unlim = drno->cdf.unlimited;
        for(i=0;i<nclistlength(drno->cdf.varnodes);i++) {
            CDFnode* var = (CDFnode*)nclistget(drno->cdf.varnodes,i);
	    CDFnode* sdim = var->array.stringdim;
            if(sdim == NULL) continue;
	    if(strcmp(sdim->ncfullname,unlim->ncfullname)==0
	       && sdim->dim.declsize == unlim->dim.declsize) {
	        var->array.stringdim = unlim;
	        nclistpop(var->array.dimensions);
	        nclistpush(var->array.dimensions,(ncelem)drno->cdf.unlimited);
	    }
	}
    }

    return ncstat;
}

static NCerror
defseqdims(NCDRNO* drno)
{
    unsigned int i;
    CDFnode* unlimited = NULL;
    NCerror ncstat = NC_NOERR;
    int seqdims = 1; /* default is to compute seq dims counts */

    /* Does the user want to see which dims are sequence dims? */
    if(paramcheck34(drno,"show","seqdims")) seqdims = 0;
  
    /* Build the unlimited node if needed */
    if(!FLAGSET(drno,NCF_NOUNLIM)) {
        unlimited = makecdfnode34(drno,"unlimited",OC_Dimension,OCNULL,drno->cdf.ddsroot);
        nclistpush(drno->cdf.ddsroot->tree->nodes,(ncelem)unlimited);
        efree(unlimited->ncbasename);
        efree(unlimited->ncfullname);
        unlimited->ncbasename = cdflegalname3(unlimited->name);
        unlimited->ncfullname = nulldup(unlimited->ncbasename);
        DIMFLAGSET(unlimited,CDFDIMUNLIM);
        drno->cdf.unlimited = unlimited;
    }

    /* Compute and define pseudo dimensions for all sequences */

    for(i=0;i<nclistlength(drno->cdf.seqnodes);i++) {
        CDFnode* seq = (CDFnode*)nclistget(drno->cdf.seqnodes,i);
	CDFnode* sqdim;
	size_t seqsize;

        seq->array.dimensions = nclistnew();

	if(!seq->usesequence) {
	    /* Mark sequence with unlimited dimension */
	    seq->array.seqdim = unlimited;
	    nclistpush(seq->array.dimensions,(ncelem)unlimited);
	    continue;
	}

	/* Does the user want us to compute the sequence dim size? */
	sqdim = NULL;
	if(seqdims) {
	    ncstat = getseqdimsize(drno,seq,&seqsize);
	    if(ncstat != NC_NOERR) {
                /* Cannot get DATADDDS; convert to unlimited */
		sqdim = unlimited;
	    }
	} else { /* !seqdims default to size = 1 */
	    seqsize = 1; 
	}
	if(sqdim == NULL) {
	    /* Note: we are making the dimension in the dds root tree */
            ncstat = makeseqdim(drno,seq,seqsize,&sqdim);
            if(ncstat) goto fail;
	}
        seq->array.seqdim = sqdim;
	nclistpush(seq->array.dimensions,(ncelem)sqdim);
    }

fail:
    return ncstat;
}

static NCerror
getseqdimsize(NCDRNO* drno, CDFnode* seq, size_t* sizep)
{
    NCerror ncstat = NC_NOERR;
    OCerror ocstat = OC_NOERR;
    OCconnection conn = drno->dap.conn;
    OCdata rootcontent = OCNULL;
    OCobject ocroot;
    CDFnode* dxdroot;
    CDFnode* xseq;
    NCbytes* minconstraints = ncbytesnew();
    size_t seqsize;

    /* Read the minimal amount of data in order to get the count */
    /* If the url is unconstrainable, then get the whole thing */
    ncbytescat(minconstraints,"?");
    computeminconstraints3(drno,seq,minconstraints);
#ifdef DEBUG
fprintf(stderr,"minconstraints: %s\n",ncbytescontents(minconstraints));
#endif
    /* Obtain the record counts for the sequence */
    if(FLAGSET(drno,NCF_UNCONSTRAINABLE))
        ocstat = dap_oc_fetch(drno,conn,NULL,OCDATADDS,&ocroot);
    else
        ocstat = dap_oc_fetch(drno,conn,ncbytescontents(minconstraints),OCDATADDS,&ocroot);
    if(ocstat) goto fail;
    ncstat = buildcdftree34(drno,ocroot,OCDATA,&dxdroot);
    if(ncstat) goto fail;	
    /* attach DATADDS to DDS */
    ncstat = attach34(dxdroot,seq);
    if(ncstat) goto fail;	
    /* WARNING: we are now switching to datadds tree */
    xseq = seq->attachment;
    ncstat = countsequence(drno,xseq,&seqsize);
    if(ncstat) goto fail;
    /* throw away the fetch'd trees */
    unattach34(drno->cdf.ddsroot);
    freecdfroot34(dxdroot);
    if(ncstat != NC_NOERR) {
        /* Cannot get DATADDDS; convert to unlimited */
	char* code;
	char* msg;
	long httperr;
	oc_svcerrordata(drno->dap.conn,&code,&msg,&httperr);
	if(code != NULL) {
	    oc_log(OCLOGERR,"oc_fetch_datadds failed: %s %s %l",
			code,msg,httperr);
	}
	ocstat = OC_NOERR;
    }		
    if(sizep) *sizep = seqsize;

fail:
    ncbytesfree(minconstraints);
    oc_data_free(conn,rootcontent);
    if(ocstat) ncstat = ocerrtoncerr(ocstat);
    return ncstat;
}


#ifdef IGNORE
static NCerror
addseqdims(NCDRNO* drno)
{
    unsigned int i;
    NCerror ncstat = NC_NOERR;
    NClist* varnodes = drno->cdf.varnodes;
    for(i=0;i<nclistlength(varnodes);i++) {
        CDFnode* var = (CDFnode*)nclistget(varnodes,i);
        ASSERT((var->array.ncdimensions != NULL));
        if(var->array.sequence == NULL) continue; /* nothing to add */
        if(var->array.sequence->usesequence) {
            /* For sequenceable dimensions, insert the recordcount dim */
	    nclistinsert(var->array.ncdimensions,0,
			 (ncelem)var->array.sequence->recorddim);
	    var->array.ncrank++;
        } else {
	    /* Unreachable variables => replace some prefix with unlimited */
	    int partition;
	    /* locate the start of the set of dimensions below
               the innermost sequence
	    */
	    partition = seqpartition(var);
	    ASSERT((partition >= 0));
	    /* Remove all the dimensions above partition */
	    while(partition--) {
		nclistremove(var->array.ncdimensions,0);
	        var->array.ncrank--;
	    }
	    /* Add unlimited */
	    nclistinsert(var->array.ncdimensions,0,
			 nclistget(drno->cdf.unlimiteds,0));
	    var->array.ncrank++;
        }
    }
    return ncstat;
}

/* Locate the first dimension under the innermost sequence */
static int
seqpartition(CDFnode* var)
{
    unsigned int i;
    NClist* dimset = var->array.ncdimensions;
    CDFnode* node, *dimnode, *firstdim;
    /* Locate the ranked container just under the innermost Sequence */
    node=var;
    dimnode = var;
    while(node != NULL && node->nctype != NC_Sequence) {
	if(node->array.rank > 0) dimnode = node;
	node = node->container;
    }
    ASSERT((node != NULL)); /* there must have been an innermost sequence */
    if(dimnode == var) {
	/* There are no dimensions above that of the var itself */
	return (var->array.ncrank - var->array.rank);
    } else {
        firstdim = (CDFnode*)nclistget(dimnode->array.dimensions,0);
        /* locate the index of the first dimension of dimnode */
        for(i=0;i<nclistlength(dimset);i++) {    
	    CDFnode* dim = (CDFnode*)nclistget(dimset,i);
	    if(dim == firstdim) return i;
	}
    }
    ASSERT((1));/* should never happen */
    return -1;
}
#endif

static NCerror
makeseqdim(NCDRNO* drno, CDFnode* seq, size_t count, CDFnode** sqdimp)
{
    CDFnode* sqdim;
    CDFnode* root = seq->root;
    CDFtree* tree = root->tree;

    /* build the dimension with given size */
    sqdim = makecdfnode34(drno,seq->name,OC_Dimension,OCNULL,root);
    if(sqdim == NULL) return THROW(NC_ENOMEM);
    nclistpush(tree->nodes,(ncelem)sqdim);
    efree(sqdim->ncbasename);
    efree(sqdim->ncfullname);
    sqdim->ncbasename = cdflegalname3(seq->name);
    sqdim->ncfullname = nulldup(sqdim->ncbasename);
    sqdim->dim.declsize = count;
    DIMFLAGSET(sqdim,CDFDIMSEQ);
    sqdim->dim.array = seq;
    if(sqdimp) *sqdimp = sqdim;
    return NC_NOERR;
}

static NCerror
countsequence(NCDRNO* drno, CDFnode* xseq, size_t* sizep)
{
    unsigned int i;
    NClist* path = nclistnew();
    OCdata parent = OCNULL;
    OCdata child = OCNULL;
    OCdata tmp;
    CDFnode* prev = NULL;
    int index;
    OCerror ocstat = OC_NOERR;
    NCerror ncstat = NC_NOERR;
    OCconnection conn = drno->dap.conn;
    size_t recordcount;
    CDFnode* xroot;

    ASSERT((xseq->nctype == NC_Sequence));

    parent = oc_data_new(conn);
    child = oc_data_new(conn);

    collectnodepath3(xseq,path,WITHDATASET);

    prev = (CDFnode*)nclistget(path,0);
    ASSERT((prev->nctype == NC_Dataset));

    xroot = xseq->root;
    ocstat = oc_data_root(conn,xroot->tree->ocroot,parent);
    if(ocstat) goto fail;

    for(i=1;i<nclistlength(path);i++) {
	xseq = (CDFnode*)nclistget(path,i);
	index = fieldindex(prev,xseq);
	ocstat = oc_data_ith(conn,parent,index,child);
	if(ocstat) goto fail;
	prev = xseq;
	/* swap the content markers */
	tmp = parent;
	parent = child;
	child = tmp;
    }
    oc_data_count(conn,parent,&recordcount);
    if(sizep) *sizep = recordcount;

fail:
    nclistfree(path);
    if(ocstat) ncstat = ocerrtoncerr(ocstat);
    oc_data_free(conn,parent);
    oc_data_free(conn,child);
    return ncstat;
}

static int
fieldindex(CDFnode* parent, CDFnode* child)
{
    unsigned int i;
    for(i=0;i<nclistlength(parent->subnodes);i++) {
	CDFnode* node = (CDFnode*)nclistget(parent->subnodes,i);
	if(node == child) return i;
    }
    return -1;
}

static NCerror
buildncstructures(NCDRNO* drno)

{
    NCerror ncstat = NC_NOERR;
    NC* ncp = (NC*)drno->controller;
    CDFnode* dds = drno->cdf.ddsroot;
    ncstat = buildglobalattrs3(drno,getncid(ncp),dds);
    if(ncstat != NC_NOERR) goto fail;
    ncstat = builddims(drno);
    if(ncstat != NC_NOERR) goto fail;
    ncstat = buildvars(drno);
    if(ncstat != NC_NOERR) goto fail;
fail:
    return THROW(ncstat);
}

static NCerror
builddims(NCDRNO* drno)
{
    int i;
    NCerror ncstat = NC_NOERR;
    int dimid;
    NC* ncp = (NC*)drno->controller;
    int ncid = getncid(ncp);
    int defunlimited = 0;
    NClist* dimset = NULL;

    /* collect all dimensions from variables */
    dimset = getalldims3(drno->cdf.varnodes,1);
    /* exclude unlimited */
    for(i=nclistlength(dimset)-1;i>=0;i--) {
	CDFnode* dim = (CDFnode*)nclistget(dimset,i);
        if(DIMFLAG(dim,CDFDIMUNLIM)) {
	    defunlimited = 1;
	    nclistremove(dimset,i);
        }
    }
    /* Sort by fullname just for the fun of it */
    for(;;) {
	int last = nclistlength(dimset) - 1;
	int swap = 0;
        for(i=0;i<last;i++) {
	    CDFnode* dim1 = (CDFnode*)nclistget(dimset,i);
	    CDFnode* dim2 = (CDFnode*)nclistget(dimset,i+1);
   	    if(strcmp(dim1->ncfullname,dim2->ncfullname) > 0) {
		nclistset(dimset,i,(ncelem)dim2);
		nclistset(dimset,i+1,(ncelem)dim1);
		swap = 1;
		break;
	    }
	}
	if(!swap) break;
    }
    /* Define unlimited only if needed */ 
    if(defunlimited && drno->cdf.unlimited != NULL) {
	CDFnode* unlimited = drno->cdf.unlimited;
	size_t unlimsize;
        ncstat = nc3d_def_dim(ncid,
			unlimited->name,
			NC_UNLIMITED,
			&unlimited->ncid);
        if(ncstat != NC_NOERR) goto fail;
        if(DIMFLAG(unlimited,CDFDIMRECORD)) {
	    /* This dimension was defined as unlimited by DODS_EXTRA */
	    unlimsize = unlimited->dim.declsize;
	} else { /* Sequence UNLIMITED */
	    unlimsize = 0;
	}
        /* Set the effective size of UNLIMITED;
           note that this cannot be done thru the normal API.*/
        drno_set_numrecs(ncp,unlimsize);
    }

    for(i=0;i<nclistlength(dimset);i++) {
	CDFnode* dim = (CDFnode*)nclistget(dimset,i);
        if(dim->dim.basedim != NULL) continue; /* handle below */
        ncstat = nc3d_def_dim(ncid,dim->ncfullname,dim->dim.declsize,&dimid);
        if(ncstat != NC_NOERR) {THROWCHK(ncstat); goto fail;}
        dim->ncid = dimid;
    }

    /* Make all duplicate dims have same dimid as basedim*/
    /* (see computecdfdimnames)*/
    for(i=0;i<nclistlength(dimset);i++) {
	CDFnode* dim = (CDFnode*)nclistget(dimset,i);
        if(dim->dim.basedim != NULL) {
	    dim->ncid = dim->dim.basedim->ncid;
	}
    }
fail:
    nclistfree(dimset);
    return THROW(ncstat);
}

/* Simultaneously build any associated attributes*/
/* and any necessary pseudo-dimensions for string types*/
static NCerror
buildvars(NCDRNO* drno)
{
    int i,j,dimindex;
    NCerror ncstat = NC_NOERR;
    int varid;
    int ncid = getncid(drno->controller);
    NClist* varnodes = drno->cdf.varnodes;

    ASSERT((varnodes != NULL));
    for(i=0;i<nclistlength(varnodes);i++) {
	CDFnode* var = (CDFnode*)nclistget(varnodes,i);
        int dimids[NC_MAX_DIMS];
	unsigned int ncrank;
        NClist* vardims = NULL;

	if(!var->visible) continue;
	if(var->array.basevar != NULL) continue;

	vardims = var->array.dimensions;
	ncrank = nclistlength(vardims);
	if(ncrank > 0) {
	    dimindex = 0;
            for(j=0;j<ncrank;j++) {
                CDFnode* dim = (CDFnode*)nclistget(vardims,j);
                dimids[dimindex++] = dim->ncid;
 	    }
        }   
        ncstat = nc3d_def_var(ncid,var->ncfullname,
                        var->externaltype,
                        ncrank,
                        (ncrank==0?NULL:dimids),
                        &varid);
        if(ncstat != NC_NOERR) {THROWCHK(ncstat); goto fail;}
        var->ncid = varid;
	if(var->attributes != NULL) {
	    for(j=0;j<nclistlength(var->attributes);j++) {
		NCattribute* att = (NCattribute*)nclistget(var->attributes,j);
		ncstat = buildattribute3a(drno,att,var->etype,varid,ncid);
        	if(ncstat != NC_NOERR) goto fail;
	    }
	}
	/* Tag the variable with its DAP path */
	if(paramcheck34(drno,"show","projection"))
	    showprojection3(drno,var);
    }    
fail:
    return THROW(ncstat);
}


static NCerror
buildglobalattrs3(NCDRNO* drno, int ncid, CDFnode* root)
{
    int i;
    NCerror ncstat = NC_NOERR;
    const char* txt;
    char *nltxt, *p;
    NCbytes* buf = NULL;
    NClist* cdfnodes;

    if(root->attributes != NULL) {
        for(i=0;i<nclistlength(root->attributes);i++) {
   	    NCattribute* att = (NCattribute*)nclistget(root->attributes,i);
	    ncstat = buildattribute3a(drno,att,NC_NAT,NC_GLOBAL,ncid);
            if(ncstat != NC_NOERR) goto fail;
	}
    }

    /* Add global attribute identifying the sequence dimensions */
    if(paramcheck34(drno,"show","seqdims")) {
        buf = ncbytesnew();
        cdfnodes = drno->cdf.ddsroot->tree->nodes;
        for(i=0;i<nclistlength(cdfnodes);i++) {
	    CDFnode* dim = (CDFnode*)nclistget(cdfnodes,i);
	    if(dim->nctype != NC_Dimension) continue;
	    if(DIMFLAG(dim,CDFDIMSEQ)) {
	        char* cname = cdflegalname3(dim->name);
	        if(ncbyteslength(buf) > 0) ncbytescat(buf,", ");
	        ncbytescat(buf,cname);
	        efree(cname);
	    }
	}
        if(ncbyteslength(buf) > 0) {
            ncstat = nc3d_put_att_text(ncid,NC_GLOBAL,"_sequence_dimensions",
	           ncbyteslength(buf),ncbytescontents(buf));
	}
    }

    /* Define some additional system global attributes
       depending on show= clientparams*/
    /* Ignore failures*/

    if(paramcheck34(drno,"show","translate")) {
        /* Add a global attribute to show the translation */
        ncstat = nc3d_put_att_text(ncid,NC_GLOBAL,"_translate",
	           strlen("netcdf-3"),"netcdf-3");
    }
    if(paramcheck34(drno,"show","url")) {
	if(drno->dap.urltext != NULL)
            ncstat = nc3d_put_att_text(ncid,NC_GLOBAL,"_url",
				       strlen(drno->dap.urltext),drno->dap.urltext);
    }
    if(paramcheck34(drno,"show","dds")) {
	txt = NULL;
	if(drno->cdf.ddsroot != NULL)
  	    txt = oc_inq_text(drno->dap.conn,drno->cdf.ddsroot->dds);
	if(txt != NULL) {
	    /* replace newlines with spaces*/
	    nltxt = nulldup(txt);
	    for(p=nltxt;*p;p++) {if(*p == '\n' || *p == '\r' || *p == '\t') {*p = ' ';}};
            ncstat = nc3d_put_att_text(ncid,NC_GLOBAL,"_dds",strlen(nltxt),nltxt);
	    efree(nltxt);
	}
    }
    if(paramcheck34(drno,"show","das")) {
	txt = NULL;
	if(drno->dap.ocdasroot != OCNULL)
	    txt = oc_inq_text(drno->dap.conn,drno->dap.ocdasroot);
	if(txt != NULL) {
	    nltxt = nulldup(txt);
	    for(p=nltxt;*p;p++) {if(*p == '\n' || *p == '\r' || *p == '\t') {*p = ' ';}};
            ncstat = nc3d_put_att_text(ncid,NC_GLOBAL,"_das",strlen(nltxt),nltxt);
	    efree(nltxt);
	}
    }

fail:
    ncbytesfree(buf);
    return THROW(ncstat);
}

static NCerror
showprojection3(NCDRNO* drno, CDFnode* var)
{
    int i,rank;
    NCerror ncstat = NC_NOERR;
    NCbytes* projection = ncbytesnew();
    NClist* path = nclistnew();
    /* Collect the set of DDS node name forming the xpath */
    collectnodepath3(var,path,WITHOUTDATASET);
    for(i=0;i<nclistlength(path);i++) {
        CDFnode* node = (CDFnode*)nclistget(path,i);
	if(i > 0) ncbytescat(projection,".");
	ncbytescat(projection,node->name);
    }
    /* Now, add the dimension info */
    rank = nclistlength(var->array.dimensions);
    for(i=0;i<rank;i++) {
	CDFnode* dim = (CDFnode*)nclistget(var->array.dimensions,i);
	char tmp[32];
	ncbytescat(projection,"[");
	snprintf(tmp,sizeof(tmp),"%lu",(unsigned long)dim->dim.declsize);
	ncbytescat(projection,tmp);
	ncbytescat(projection,"]");
    }    
    /* Define the attribute */
    ncstat = nc3d_put_att_text(getncid(drno->controller),var->ncid,
                               "_projection",
		               ncbyteslength(projection),
			       ncbytescontents(projection));
    return ncstat;
}

static NCerror
buildattribute3a(NCDRNO* drno, NCattribute* att, nc_type vartype, int varid, int ncid)
{
    int i;
    NCerror ncstat = NC_NOERR;
    char* cname = cdflegalname3(att->name);
    unsigned int nvalues = nclistlength(att->values);

    /* If the type of the attribute is string, then we need*/
    /* to convert to a single character string by concatenation.
	modified: 10/23/09 to insert newlines.
	modified: 10/28/09 to interpret escapes
    */
    if(att->etype == NC_STRING || att->etype == NC_URL) {
	char* newstring;
	size_t newlen = 0;
	for(i=0;i<nvalues;i++) {
	    char* s = (char*)nclistget(att->values,i);
	    newlen += (1+strlen(s));
	}
	newstring = (char*)emalloc(newlen);
        MEMCHECK(newstring,NC_ENOMEM);
	newstring[0] = '\0';
	for(i=0;i<nvalues;i++) {
	    char* s = (char*)nclistget(att->values,i);
	    if(i > 0) strcat(newstring,"\n");
	    strcat(newstring,s);
	}
        dapexpandescapes(newstring);
	if(newstring[0]=='\0')
	    ncstat = nc3d_put_att_text(ncid,varid,cname,1,newstring);
	else
	    ncstat = nc3d_put_att_text(ncid,varid,cname,strlen(newstring),newstring);
	free(newstring);
    } else {
	nc_type atype;
	unsigned int typesize;
	void* mem;
	/* It turns out that some servers upgrade the type
           of _FillValue in order to correctly preserve the
           original value. However, since the type of the
           underlying variable is not changes, we get a type
           mismatch. So, make sure the type of the fillvalue
           is the same as that of the controlling variable.
	*/
        if(varid != NC_GLOBAL && strcmp(att->name,"_FillValue")==0)
	    atype = nctypeconvert(drno,vartype);
	else
	    atype = nctypeconvert(drno,att->etype);
	typesize = nctypesizeof(atype);
	mem = emalloc(typesize * nvalues);
        ncstat = dapcvtattrval3(atype,mem,att->values);
        ncstat = nc3d_put_att(ncid,varid,cname,atype,nvalues,mem);
	efree(mem);
    }
    free(cname);
    return THROW(ncstat);
}

#ifdef IGNORE
NCerror
detachdatadds3(NCDRNO* drno)
{
    int i;
    for(i=0;i<nclistlength(drno->cdf.dds->tree.nodes);i++) {
	CDFnode* node = (CDFnode*)nclistget(drno->cdf.dds->tree.nodes,i);
	node->active = 0;
	node->dim.datasize = node->dim.declsize;
   }
   return NC_NOERR;
}

NCerror
attachdatadds3(NCDRNO* drno)
{
    int i;
    NClist* cdfnodes = drno->cdf.dds->tree.nodes;
    for(i=0;i<nclistlength(cdfnodes);i++) {
	CDFnode* node = (CDFnode*)nclistget(cdfnodes,i);
	OCobject dds = node->dds;
	if(dds == OCNULL) continue;
	node->active = oc_datadds_active(drno->dap.conn,dds);
	if(node->nctype == NC_Dimension) {
	    oc_datadds_dimsize(drno->dap.conn,node->dds,&node->dim.datasize);
	}
    }
    return NC_NOERR;
}
#endif

/*
This is more complex than one might think. We want to find
a path to a variable inside the given node so that we can
ask for a single instance of that variable to minimize the
amount of data we retrieve. However, we want to avoid passing
through any nested sequence. This is possible because of the way
that sequencecheck() works.
*/
static NCerror
computeminconstraints3(NCDRNO* drno , CDFnode* seq, NCbytes* minconstraints)
{
    NClist* path = nclistnew();
    CDFnode* var;
    CDFnode* candidate;
    unsigned int i,j,ndims;
    char* prefix;

    /* Locate a variable that is inside this sequence */
    /* Preferably one that is a numeric type*/
    for(candidate=NULL,var=NULL,i=0;i<nclistlength(drno->cdf.varnodes);i++) {
	CDFnode* node = (CDFnode*)nclistget(drno->cdf.varnodes,i);
	if(node->array.sequence == seq) {
	    if(node->nctype == NC_Primitive) {
		switch(node->etype) {
		case NC_BYTE: case NC_SHORT: case NC_INT:
		case NC_FLOAT: case NC_DOUBLE:
		case NC_UBYTE: case NC_USHORT: case NC_UINT:
		case NC_INT64: case NC_UINT64:
		    if(var == NULL) {
			var = node; /* good choice */
		    }
		    break;
		case NC_CHAR: case NC_STRING:
		default:
		    candidate = node; /* usable */
		    break;
		}
	    }
	}
    }
    if(var == NULL && candidate != NULL) var = candidate;
    else if(var == NULL) return THROW(NC_EINVAL);

    /* collect seq path prefix */
    prefix = makecdfpathstring3(seq->container,".");
    ncbytescat(minconstraints,prefix);
    if(strlen(prefix) > 0) ncbytescat(minconstraints,".");

    /* Compute a short path from the var back to and including
       the sequence
    */
    collectnodepath3(var,path,WITHOUTDATASET);
    while(nclistlength(path) > 0) {
	CDFnode* node = (CDFnode*)nclistget(path,0);
	if(node == seq) break;
	nclistremove(path,0);
    }
    ASSERT((nclistlength(path) > 0));

    /* construct the projection path using minimal index values */
    for(i=0;i<nclistlength(path);i++) {
	CDFnode* node = (CDFnode*)nclistget(path,i);
	if(i > 0) ncbytescat(minconstraints,".");
	ncbytescat(minconstraints,node->name);
	if(node == seq) {
	    /* Use the limit */
	    if(node->sequencelimit > 0) {
		char tmp[64];
		snprintf(tmp,sizeof(tmp),"[0:%lu]",
		         (unsigned long)(node->sequencelimit - 1));
		ncbytescat(minconstraints,tmp);
	    }
	} else if(nclistlength(node->array.dimensions) > 0) {
	    ndims = nclistlength(node->array.dimensions);
	    for(j=0;j<ndims;j++) {
		CDFnode* dim = (CDFnode*)nclistget(node->array.dimensions,j);
		if(dim->dim.dimflags & CDFDIMSTRING) {
		    ASSERT((j == (ndims - 1)));
		    break;
		}
		ncbytescat(minconstraints,"[0]");
	    }
	}
    }
    nclistfree(path);
    /* Finally, add in any selection from the original URL */
    if(drno->dap.url.selection != NULL)
        ncbytescat(minconstraints,drno->dap.url.selection);
    efree(prefix);
    return NC_NOERR;
}

static unsigned long
cdftotalsize3(NClist* dimensions)
{
    unsigned int i;
    unsigned long total = 1;
    if(dimensions != NULL) {
	for(i=0;i<nclistlength(dimensions);i++) {
	    CDFnode* dim = (CDFnode*)nclistget(dimensions,i);
	    total *= dim->dim.declsize;
	}
    }
    return total;
}

/* Estimate variables sizes and then resort the variable list
   by that size
*/
static void
estimatevarsizes3(NCDRNO* drno)
{
    int ivar;
    unsigned int rank;
    size_t totalsize = 0;

    for(ivar=0;ivar<nclistlength(drno->cdf.varnodes);ivar++) {
        CDFnode* var = (CDFnode*)nclistget(drno->cdf.varnodes,ivar);
	NClist* ncdims = var->array.dimensions;
	rank = nclistlength(ncdims);
	if(rank == 0) { /* use instance size of the type */
	    var->estimatedsize = nctypesizeof(var->etype);
#ifdef DEBUG
fprintf(stderr,"scalar %s.estimatedsize = %lu\n",
	makecdfpathstring3(var,"."),var->estimatedsize);
#endif
	} else {
	    unsigned long size = cdftotalsize3(ncdims);
	    size *= nctypesizeof(var->etype);
#ifdef DEBUG
fprintf(stderr,"array %s(%u).estimatedsize = %lu\n",
	makecdfpathstring3(var,"."),rank,size);
#endif
	    var->estimatedsize = size;
	}
	totalsize += var->estimatedsize;
    }
#ifdef DEBUG
fprintf(stderr,"total estimatedsize = %lu\n",totalsize);
#endif
    drno->cdf.totalestimatedsize = totalsize;
}

NCerror
fetchtemplatemetadata3(NCDRNO* drno)
{
    NCerror ncstat = NC_NOERR;
    OCerror ocstat = OC_NOERR;
    OCobject ocroot = OCNULL;
    CDFnode* ddsroot = NULL;
    char* ce = NULL;

    /* Temporary hack: we need to get the selection string
       from the url
    */
    /* Get (almost) unconstrained DDS; In order to handle functions
       correctly, those selections must always be included
    */
    if(FLAGSET(drno,NCF_UNCONSTRAINABLE))
	ce = NULL;
    else
        ce = nulldup(drno->dap.url.selection);

    /* Get selection constrained DDS */
    ocstat = dap_oc_fetch(drno,drno->dap.conn,ce,OCDDS,&ocroot);
    if(ocstat != OC_NOERR) {THROWCHK(ocstat); goto done;}

    /* Get selection constrained DAS */
    ocstat = dap_oc_fetch(drno,drno->dap.conn,ce,OCDAS,&drno->dap.ocdasroot);
    if(ocstat != OC_NOERR) {THROWCHK(ocstat); goto done;}

    /* Construct our parallel dds tree */
    ncstat = buildcdftree34(drno,ocroot,OCDDS,&ddsroot);
    if(ncstat != NC_NOERR) {THROWCHK(ncstat); goto done;}
    drno->cdf.ddsroot = ddsroot;

    /* Combine */
    ncstat = dapmerge3(drno,ddsroot,drno->dap.ocdasroot);
    if(ncstat != NC_NOERR) {THROWCHK(ncstat); goto done;}

done:
    efree(ce);
    if(ocstat != OC_NOERR) ncstat = ocerrtoncerr(ocstat);
    return ncstat;
}

NCerror
fetchconstrainedmetadata3(NCDRNO* drno)
{
    NCerror ncstat = NC_NOERR;
    OCerror ocstat = OC_NOERR;
    OCobject ocroot;
    CDFnode* ddsroot; /* constrained */
    char* ce = NULL;

    if(FLAGSET(drno,NCF_UNCONSTRAINABLE))
	ce = NULL;
    else
        ce = makeconstraintstring3(&drno->dap.constraint);

    if(ce == NULL || strlen(ce) == 0) {
	/* no need to get the dds again; just imprint on self */
        ncstat = imprintself3(drno->cdf.ddsroot);
        if(ncstat) goto fail;
    } else {
        ocstat = dap_oc_fetch(drno,drno->dap.conn,ce,OCDDS,&ocroot);
        if(ocstat != OC_NOERR) {THROWCHK(ocstat); goto fail;}

        /* Construct our parallel dds tree; including attributes*/
        ncstat = buildcdftree34(drno,ocroot,OCDDS,&ddsroot);
        if(ncstat) goto fail;

        if(!FLAGSET(drno,NCF_UNCONSTRAINABLE)) {
            /* fix DAP server problem by adding back any missing grid nodes */
            ncstat = regrid3(ddsroot,drno->cdf.ddsroot,drno->dap.constraint.projections);    
            if(ncstat) goto fail;
	}

#ifdef DEBUG
fprintf(stderr,"constrained:\n%s",dumptree(ddsroot));
#endif

        /* Imprint the constrained DDS data over the unconstrained DDS */
        ncstat = imprint3(drno->cdf.ddsroot,ddsroot);
        if(ncstat) goto fail;

        /* Throw away the constrained DDS */
        freecdfroot34(ddsroot);
    }

fail:
    efree(ce);
    if(ocstat != OC_NOERR) ncstat = ocerrtoncerr(ocstat);
    return ncstat;
}

/* Suppress variables not in usable sequences */
static NCerror
suppressunusablevars3(NCDRNO* drno)
{
    int i,j;
    int found = 1;
    NClist* path = nclistnew();
    while(found) {
	found = 0;
	for(i=0;i<nclistlength(drno->cdf.varnodes);i++) {
	    CDFnode* var = (CDFnode*)nclistget(drno->cdf.varnodes,i);
	    /* See if this var is under an unusable sequence */
	    nclistclear(path);
	    collectnodepath3(var,path,WITHOUTDATASET);
	    for(j=0;j<nclistlength(path);j++) {
		CDFnode* node = (CDFnode*)nclistget(path,j);
		if(node->nctype == NC_Sequence
		   && !node->usesequence) {
		    nclistremove(drno->cdf.varnodes,i);
		    found = 1;
		    break;
		}
	    }
	    if(found) break;
	}
    }
    nclistfree(path);
    return NC_NOERR;
}


/*
For variables which have a zero size dimension,
either use unlimited, or make them invisible.
*/
static NCerror
fixzerodims3(NCDRNO* drno)
{
    int i,j;
    for(i=0;i<nclistlength(drno->cdf.varnodes);i++) {
	CDFnode* var = (CDFnode*)nclistget(drno->cdf.varnodes,i);
        NClist* ncdims = var->array.dimensions;
	if(nclistlength(ncdims) == 0) continue;
        for(j=0;j<nclistlength(ncdims);j++) {
	    CDFnode* dim = (CDFnode*)nclistget(ncdims,j);
	    if(DIMFLAG(dim,CDFDIMUNLIM)) continue;
	    if(dim->dim.declsize == 0) {
		if(j == 0) {/* can make it unlimited */
		    nclistset(ncdims,j,(ncelem)drno->cdf.unlimited);
		} else { /* make node invisible */
		    var->visible = 0;
		    var->zerodim = 1;
		}
	    }
	}
    }
    return NC_NOERR;
}

static void
applyclientparamcontrols3(NCDRNO* drno)
{
    NClist* params = NULL;
    const char* model;
    const char* value;
    int translation;

    /* Get client parameters */
    params = dapparamdecode(drno->dap.url.params);

    /* enable/disable caching */
    value = dapparamlookup(params,"cache");    
    if(value == NULL)
	drno->controls.flags |= DFALTCACHEFLAG;
    else if(strlen(value) == 0)
	drno->controls.flags |= NCF_CACHE;
    else if(strcmp(value,"1")==0 || value[0] == 'y')
	drno->controls.flags |= NCF_CACHE;

    if(FLAGSET(drno,NCF_UNCONSTRAINABLE))
	drno->controls.flags |= NCF_CACHE;

    oc_log(OCLOGNOTE,"Caching=%d",FLAGSET(drno,NCF_CACHE));

    /* Establish the mode */
    translation = 3;
    model = dapparamlookup(params,"model");
    drno->controls.flags |=
	modeldecode(translation,model,nctmodels,DFALTMODELFLAGS);

    /* No longer need params */
    dapparamfree(params);
}

/* Accumulate a set of all the known dimensions */
static NClist*
getalldims3(NClist* vars, int visibleonly)
{
    int i,j;
    NClist* dimset = nclistnew();

    /* get bag of all dimensions */
    for(i=0;i<nclistlength(vars);i++) {
	CDFnode* var = (CDFnode*)nclistget(vars,i);
	if(!visibleonly || var->visible) {
            NClist* vardims = var->array.dimensions;
   	    for(j=0;j<nclistlength(vardims);j++) {
	        CDFnode* dim = (CDFnode*)nclistget(vardims,j);
	        nclistpush(dimset,(ncelem)dim);
	    }
	}
    }
    /* make unique */
    nclistunique(dimset);
    return dimset;
}
