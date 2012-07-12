#include "ncdap4.h"
#include "netcdf4l.h"
#include "dispatch4.h"
#include "dapdebug.h"
#include "dapalign.h"
#include <unistd.h>

#ifdef HAVE_GETRLIMIT
#include <sys/time.h>
#include <sys/resource.h>
#endif

#ifdef DEBUG
#include "dapdump.h"
#endif

#define DFALTMODELFLAGS (NCF_NC3|NCF_NCDAP)

/* Mnemonic */
#define getgrpid(nfi) ((nfi)->ext_ncid)
#define getncid(drno) (((NC_FILE_INFO_T*)((drno)->controller))->ext_ncid)

ptrdiff_t dapsinglestride4[NC_MAX_DIMS];

extern NC_FILE_INFO_T* nc_file;

extern NCerror freeNCDRNO4(NCDRNO* drno);

static void nc4dinitialize(void);
static void setncdap4dispatch(int ncid);
static void setnetcdf4dispatch(int ncid);
static NCerror buildnc4(NC_FILE_INFO_T* nfi);
static NCerror builddims4(NCDRNO*);
static NCerror buildtypes4(NCDRNO*);
static NCerror buildtypes4r(NCDRNO* drno, CDFnode* tnode);
static NCerror buildvars4(NCDRNO*);
static NCerror buildglobalattrs4(NCDRNO*, int, CDFnode* root);
static NCerror buildattribute4a(NCDRNO* drno, NCattribute* att, int varid, int ncid);
static NCerror showprojection4(NCDRNO* drno, CDFnode* var);
static size_t estimatesizes4r(NCDRNO* drno, CDFnode* node);
static void estimatesizes4(NCDRNO* drno);
static NCerror fixzerodims4(NCDRNO* drno);
static NCerror fixzerodims4r(NCDRNO* drno, CDFnode* node);
static NCerror cvtunlimiteddim(NCDRNO* drno, CDFnode* dim);
static void applyclientparamcontrols4(NCDRNO* drno);

static int nc4dinitialized = 0;

static void
setnetcdf4dispatch(int ncid)
{
    NC_FILE_INFO_T* nc = nc4_find_nc_file(ncid);
    if(nc != NULL) nc->dispatch = &netcdf4lib;
}

static void
setncdap4dispatch(int ncid)
{
    NC_FILE_INFO_T* nc = nc4_find_nc_file(ncid);
    if(nc != NULL) nc->dispatch = &ncdap4lib;
}

int
nc4d_open_file(const char *path, int mode, int basepe, size_t *chunksizehintp, 
          int use_parallel, MPI_Comm comm, MPI_Info info, int *ncidp)
{
    NCerror ncstat = NC_NOERR;
    OCerror ocstat = OC_NOERR;
    DAPURL tmpurl;
    NCDRNO* drno = NULL; /* reuse the ncdap3 structure*/
    NC_HDF5_FILE_INFO_T* h5 = NULL;
    NC_FILE_INFO_T* nc = NULL;
    NC_GRP_INFO_T *grp = NULL;
    int ncid = -1;
    char* modifiedpath = NULL;
    const char* value;
#ifdef PSEUDOFILE
    int fd;
    char* tmpname = NULL;
#else
    short fileid;
#endif
    NClist* params = NULL;
    int translation_flags = 0;

    if(path == NULL || ncidp == NULL) {ncstat = NC_EINVAL; goto fail;}

    LOG((1, "nc_open_file: path %s mode %d comm %d info %d", 
         path, mode, comm, info));

    if(!nc4dinitialized) nc4dinitialize();

    /* Allow access to the libncdap3 layer through testing for a URL,
       through the mode flags and through the client parameters
    */
    if(!dapurlparse(path,&tmpurl)) {
	translation_flags = 0;
    }

    if((mode & NC_CLASSIC_MODEL) != 0)
        translation_flags = NCF_NC3;
    else if((mode & NC_NETCDF4) != 0)
	translation_flags = NCF_NC4;
    else { /* check client parameters */
    	const char* model;
	int translation = 3 ; /* default */
        params = dapparamdecode(tmpurl.params);
	if(dapparamlookup(params,"netcdf4")
	   || dapparamlookup(params,"netcdf-4"))
	    translation = 4;
        model = dapparamlookup(params,"model");
	translation_flags = modeldecode(translation,model,
					    nctmodels,DFALTMODELFLAGS);
        dapparamfree(params); /* Reclaim */
    }

    /* Do we want to directly call into netcdf4/netcdf3? */
    if(translation_flags == 0 || (translation_flags & NCF_NC3) != 0) {
	/* call the regular netcdf-4 open, which in turn will call
	   netcdf-3 */
	dapurlclear(&tmpurl);
	ncstat = l4nc_open_file(path, mode,
				basepe, chunksizehintp, 
                                use_parallel,
                                comm, info,
                                &ncid);
	if(ncstat == NC_NOERR) setnetcdf4dispatch(ncid);
	*ncidp = ncid;
	return THROW(ncstat);
    }
    dapurlclear(&tmpurl); /* no longer needed */

    /* Check for legal mode flags */
    if((mode & NC_WRITE) != 0) ncstat = NC_EINVAL;
    else if(mode & (NC_WRITE|NC_CLOBBER)) ncstat = NC_EPERM;
    if(ncstat != NC_NOERR) {THROWCHK(ncstat); goto fail;}

    mode = (mode & ~(NC_MPIIO | NC_MPIPOSIX));
    /* Despite the above check, we want the file to be initially writable */
    mode |= (NC_WRITE|NC_CLOBBER);

#ifdef DEBUG
extern int ocdebug;
ocdebug = 1;
#endif

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
    drno->dap.urltext = modifiedpath;
    drno->cdf.separator = ".";
    dapurlparse(drno->dap.urltext,&drno->dap.url);
    if(!constrainable34(&drno->dap.url))
	SETFLAG(drno,NCF_UNCONSTRAINABLE);
    drno->cdf.smallsizelimit = DFALTSMALLLIMIT;
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

    /* Re-scan the client parameters */
    applyclientparamcontrols4(drno);

#ifdef PSEUDOFILE
    tmpname = nulldup(PSEUDOFILE);
    fd = mkstemp(tmpname);
    if(fd < 0) {THROWCHK(errno); goto fail;}
    /* Now, use the file to create the hdf5 file */
    ncstat = l4nc_create_file(tmpname,NC_NETCDF4,0,0,NULL,0,0,&ncid);
    /* unlink the temp file so it will automatically be reclaimed */
    unlink(tmpname);
    efree(tmpname);
    /* Avoid fill */
    nc_set_fill(ncid,NC_NOFILL,NULL);
    if(ncstat)
	{THROWCHK(ncstat); goto fail;}
    /* Find our metadata for this file. */
    ncstat = nc4_find_nc_grp_h5(ncid, &nc, &grp, &h5);
    if(ncstat)
	{THROWCHK(ncstat); goto fail;}
#else
    /* Allocate the storage for this file info struct, and fill it with zeros*/
    /* WARNING: this stores struct ptr at front of global list "nc_file" */
    fileid = drno_delta_file_id(1);
    if((ncstat = nc4_file_list_add(fileid << ID_SHIFT)))
	goto fail;
    nc = nc_file;

    nc->int_ncid = nc->ext_ncid;
    ncid = nc->int_ncid;

    /* Add necessary structs to hold netcdf-4 file data. */
    if((ncstat = nc4_nc4f_list_add(nc, path, mode)))
	{THROWCHK(ncstat); goto fail;}
    ASSERT((nc->nc4_info && nc->nc4_info->root_grp));

    /* get the h5 structure*/
    h5 = nc->nc4_info;

    /* Mark as indef state and writable */
    h5->flags |= NC_INDEF;
    h5->no_write = 0;
#endif

    nc->drno = drno;
    drno->controller = (void*)nc; /* cross link*/
    /* pass back the new ncid */
    *ncidp = ncid;
    /* set the dispatch table */
    setncdap4dispatch(ncid);

    ocstat = oc_open(drno->dap.urltext,&drno->dap.conn);
    if(ocstat != OC_NOERR) {THROWCHK(ocstat); goto fail;}

    if(paramcheck34(drno,"show","fetch"))
	drno->controls.flags |= NCF_SHOWFETCH;

    /* Turn on logging */
    value = oc_clientparam_get(drno->dap.conn,"log");
    if(value != NULL) {
	oc_loginit();
        oc_setlogging(1);
        oc_logopen(value);
    }

    /* fetch and build the unconstrained DDS */
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
    ncstat = computecdfnodesets4(drno);
    if(ncstat) {THROWCHK(ncstat); goto fail;}

    /* Fix grids */
    ncstat = fixgrids4(drno);
    if(ncstat) {THROWCHK(ncstat); goto fail;}

    /* apply client parameters (after computcdfinfo and computecdfvars)*/
    ncstat = applyclientparams34(drno);
    if(ncstat) {THROWCHK(ncstat); goto fail;}

    /* Accumulate the nodes representing user types*/
    ncstat = computeusertypes4(drno);
    if(ncstat) {THROWCHK(ncstat); goto fail;}

    /* Re-compute the type names*/
    ncstat = shortentypenames4(drno);
    if(ncstat) {THROWCHK(ncstat); goto fail;}

    /* Re-compute the dimension names*/
    ncstat = computecdfdimnames34(drno);
    if(ncstat) {THROWCHK(ncstat); goto fail;}

    /* deal with zero-size dimensions */
    ncstat = fixzerodims4(drno);
    if(ncstat) {THROWCHK(ncstat); goto fail;}

    /* Estimate the variable sizes */
    estimatesizes4(drno);

    ncstat = buildnc4(nc);
    if(ncstat != NC_NOERR) {THROWCHK(ncstat); goto fail;}

    /* Do any necessary data prefetch */
    ncstat = prefetchdata3(drno);
    if(ncstat != NC_NOERR)
	{THROWCHK(ncstat); goto fail;}

    /* Mark as no longer indef and no longer writable*/
    h5->flags &= ~(NC_INDEF);
    h5->no_write = 1;

    drno_delta_numfiles(1);

    return ncstat;

fail:
#ifdef PSEUDOFILE
    if(ncid >= 0) l4nc_abort(ncid);
#else
    if(nc != NULL) {
        nc4_file_list_del(nc);
	drno_delta_file_id(-1);
        drno_delta_numfiles(1);
    }
#endif
    freeNCDRNO4(drno);
    if(ocstat != OC_NOERR) ncstat = ocerrtoncerr(ocstat);
    return THROW(ncstat);
}

int
nc4d_close(int ncid)
{
    NC_FILE_INFO_T *nc;
    NC_GRP_INFO_T *grp;
    NC_HDF5_FILE_INFO_T *h5;
    NCDRNO* drno;
    int ncstat = NC_NOERR;

    LOG((1, "nc_close: ncid 0x%x", ncid));
    /* Find our metadata for this file. */
    ncstat = nc4_find_nc_grp_h5(ncid, &nc, &grp, &h5);
    if(ncstat != NC_NOERR) return THROW(ncstat);

    nc = grp->file;
    ASSERT((nc));
      
    /* This must be the root group. */
    if (grp->parent) ncstat = NC_EBADGRPID;

    oc_logclose();

    /* Destroy/close the NCDRNO state */
    drno = nc->drno;
    freeNCDRNO4(drno);

#ifdef PSEUDOFILE
    l4nc_abort(ncid);
#else    
    /* Invoke the relevant parts of close_netcdf4_file() */
    nc4_rec_grp_del(&h5->root_grp, h5->root_grp);
    /* Free the nc4_info struct. */
    nc_free(h5);
    /* Delete this entry from our list of open files. */
    nc4_file_list_del(nc);
    drno_delta_numfiles(-1);
#endif
    return THROW(ncstat);
}

int
nc_create_file(const char *path,
               int cmode, size_t initialsz, 
	       int basepe, size_t *chunksizehintp, MPI_Comm comm, 
	       MPI_Info info, int *ncidp)
{
    NC_GRP_INFO_T *grp;
    NC_FILE_INFO_T *nc;
    NC_HDF5_FILE_INFO_T *h5;
    int ncstat;

    ncstat =  l4nc_create_file(path, cmode, initialsz, basepe,
				 chunksizehintp, comm, info, ncidp);
    if(ncstat != NC_NOERR) return ncstat;
    /* Find our metadata for this file. */
    ncstat = nc4_find_nc_grp_h5(*ncidp, &nc, &grp, &h5);
    if(ncstat == NC_NOERR) /* set the dispatch table */
	setnetcdf4dispatch(*ncidp);
    return ncstat;    
}

int
nc4d_sync(int ncid)
{
    LOG((1, "nc_sync: ncid 0x%x", ncid));
    return NC_NOERR;
}

int
nc4d_abort(int ncid)
{
    LOG((1, "nc_abort: ncid 0x%x", ncid));
    /* Turn into close */
    return nc4d_close(ncid);
}

int
nc4d_put_vara(NC_FILE_INFO_T* nc, int ncid, int varid, 
			const size_t* startp, const size_t* countp, 
			nc_type mem_nc_type, int is_long, void* data)
{
    LOG((1, "nc_put_vara: ncid 0x%x varid 0x%x", ncid, varid));
    return NC_EPERM;
}

/**************************************************/
/* Auxilliary routines                            */
/**************************************************/

static void
nc4dinitialize()
{
    int i;
    nc3dinitialize();
    for(i=0;i<NC_MAX_DIMS;i++) dapsinglestride4[i] = 1;
    nc4dinitialized = 1;
}

NCerror
freeNCDRNO4(NCDRNO* drno)
{
    return freeNCDRNO3(drno);
}

/*
Note: never use any of the libncdap3 code to call
netcdf API functions because it will use the netcdf-3 API.
*/
static NCerror
buildnc4(NC_FILE_INFO_T* nfi)
{
    NCerror ncstat = NC_NOERR;
    NCDRNO* drno = nfi->drno;
    CDFnode* dds = drno->cdf.ddsroot;

    ncstat = buildglobalattrs4(drno,getncid(drno),dds);
    if(ncstat != NC_NOERR) {THROWCHK(ncstat); goto fail;}
    ncstat = builddims4(drno);
    if(ncstat != NC_NOERR) {THROWCHK(ncstat); goto fail;}
    ncstat = buildtypes4(drno);
    if(ncstat != NC_NOERR) {THROWCHK(ncstat); goto fail;}
    ncstat = buildvars4(drno);
    if(ncstat != NC_NOERR) {THROWCHK(ncstat); goto fail;}
fail:
    return THROW(ncstat);
}

/* Define dim info for top-level dims */
static NCerror
builddims4(NCDRNO* drno)
{
    unsigned int i,j;
    NCerror ncstat = NC_NOERR;
    int dimid;
    NC_FILE_INFO_T* nfi = (NC_FILE_INFO_T*)drno->controller;
    int ncid = getgrpid(nfi);
    NClist* dimset = nclistnew();

    /* collect all dimensions from variables,
       including duplicates; note we use array.dimensions
       not array.ncdimensions.
    */
    for(i=0;i<nclistlength(drno->cdf.varnodes);i++) {
	CDFnode* var = (CDFnode*)nclistget(drno->cdf.varnodes,i);
        if(!var->visible) continue;
	nclistextend(dimset,nclistlength(var->array.dimensions));
        for(j=0;j<nclistlength(var->array.dimensions);j++) {
	    CDFnode* dim = (CDFnode*)nclistget(var->array.dimensions,j);
	    int k,inserted = 0;
            /* Sort by fullname just for the fun of it */
            for(k=0;k<nclistlength(dimset);k++) {
	        CDFnode* kdim = (CDFnode*)nclistget(dimset,k);
		if(strcmp(kdim->ncfullname,dim->ncfullname) > 0) {
		    nclistinsert(dimset,k,(ncelem)dim);
		    inserted = 1;
		    break;
		}
	    }
	    if(!inserted) nclistpush(dimset,(ncelem)dim);
	}
    }

    /* Define those top-level dims */
    for(i=0;i<nclistlength(dimset);i++) {
	CDFnode* dim = (CDFnode*)nclistget(dimset,i);
	if(dim->dim.basedim != NULL) continue;
        ncstat = nc_def_dim(ncid,dim->ncfullname,dim->dim.declsize,&dimid);
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
/*ok:*/
fail:
    nclistfree(dimset);
    return THROW(ncstat);
}

static NCerror
buildtypes4(NCDRNO* drno)
{
    unsigned int i;
    NCerror ncstat = NC_NOERR;

    /* Define user types in postorder */
    for(i=0;i<nclistlength(drno->cdf.usertypes);i++) {
	CDFnode* node = (CDFnode*)nclistget(drno->cdf.usertypes,i);
	if(!node->visible) continue;
	ncstat = buildtypes4r(drno,node);
	if(ncstat != NC_NOERR) {THROWCHK(ncstat); goto fail;}
    }
fail:
    return THROW(ncstat);
}

static NCerror
buildtypes4r(NCDRNO* drno, CDFnode* tnode)
{
    unsigned int i,j;
    int typeid;
    NCerror ncstat = NC_NOERR;

    if(!tnode->visible) goto done;

    switch (tnode->nctype) {
    case NC_Sequence:
	/* Look for sequences that have a single field whose
           type is primitive; for these, we will not generate
           the compound type.
        */
	if(tnode->singleton) {
	    /* ok, just generate the vlen type using the sequence's
               singleton field */
	    /* Find the first primitive visible field */
	    CDFnode* prim = getsingletonfield(tnode->subnodes);
	    ASSERT((prim != NULL));
	    ncstat = nc_def_vlen(getncid(drno),tnode->vlenname,
                                     prim->etype,&typeid);
	    if(ncstat != NC_NOERR) {THROWCHK(ncstat); goto done;}
	    tnode->basetypeid = prim->etype;
	    tnode->typeid = typeid;
	    break;
	}
        /* fall thru */
    case NC_Grid:
    case NC_Structure:
	ncstat = nc_def_compound(getncid(drno),tnode->typesize.instance.size,
				tnode->typename,&typeid);
	if(ncstat != NC_NOERR) {THROWCHK(ncstat); goto done;}
	tnode->typeid = typeid;
	for(i=0;i<nclistlength(tnode->subnodes);i++) {
	    CDFnode* field = (CDFnode*)nclistget(tnode->subnodes,i);
	    if(!field->visible) continue;
	    if(nclistlength(field->array.dimensions) == 0) {
   	        ncstat = nc_insert_compound(getncid(drno),typeid,
					field->ncbasename,
				        field->typesize.field.offset,
					field->typeid);
	        if(ncstat != NC_NOERR) {THROWCHK(ncstat); goto done;}
	    } else {
	        int dimsizes[NC_MAX_DIMS];
	        for(j=0;j<nclistlength(field->array.dimensions);j++) {
		    CDFnode* dim=(CDFnode*)nclistget(field->array.dimensions,j);
		    dimsizes[j] = dim->dim.declsize;
		}
   	        ncstat = nc_insert_array_compound(getncid(drno),typeid,
					field->ncbasename,
				        field->typesize.field.offset,
					field->typeid,
					nclistlength(field->array.dimensions),
                                        dimsizes);
	        if(ncstat != NC_NOERR) {THROWCHK(ncstat); goto done;}
	    }
	}
	/* If the node is a sequence, also define the corresponding vlen type*/
	if(tnode->nctype == NC_Sequence) {
	    ncstat = nc_def_vlen(getncid(drno),tnode->vlenname,tnode->typeid,&typeid);
	    if(ncstat != NC_NOERR) {THROWCHK(ncstat); goto done;}
	    tnode->basetypeid = typeid;
	    tnode->typeid = typeid;
	}
        break;

    case NC_Primitive:
	break;

    default: PANIC1("unexpected nctype: %d",tnode->nctype);
    }

done:
    return THROW(ncstat);
}

/* Simultaneously build any associated attributes */
static NCerror
buildvars4(NCDRNO* drno)
{
    /* Variables (in this translation) are (mostly)
       the direct fields of the Dataset*/
    unsigned int i,j;
    NCerror ncstat = NC_NOERR;
    int varid;
    NC_FILE_INFO_T* nfi = (NC_FILE_INFO_T*)drno->controller;
    int ncid = getgrpid(nfi);

    for(i=0;i<nclistlength(drno->cdf.varnodes);i++) {
	CDFnode* var = (CDFnode*)nclistget(drno->cdf.varnodes,i);
	NClist* vardims = var->array.dimensions;
	int dimids[NC_MAX_DIMS];
	int ncrank,dimindex=0;

	if(!var->visible) continue;

	ncrank = nclistlength(vardims);
	if(ncrank > 0) {
	    dimindex = 0;
            for(j=0;j<ncrank;j++) {
                CDFnode* dim = (CDFnode*)nclistget(vardims,j);
                dimids[dimindex++] = dim->ncid;
 	    }
        }   
	setvarbasetype(drno,var);
	ASSERT((var->typeid > 0));
        ncstat = nc_def_var(getncid(drno),var->ncfullname,
			    var->typeid,
                            nclistlength(var->array.dimensions),
                            (ncrank==0?NULL:dimids),
                            &varid);
	if(ncstat != NC_NOERR) {THROWCHK(ncstat); goto fail;}
	var->ncid = varid;
	if(var->attributes != NULL) {
	    for(j=0;j<nclistlength(var->attributes);j++) {
		NCattribute* att = (NCattribute*)nclistget(var->attributes,j);
		ncstat = buildattribute4a(drno,att,varid,ncid);
        	if(ncstat != NC_NOERR) {THROWCHK(ncstat); goto fail;}
	    }
	}
	/* Tag the variable with its DAP path */
	if(paramcheck34(drno,"show","projection"))
	    showprojection4(drno,var);
    }
    
fail:
    return THROW(ncstat);
}

static NCerror
buildglobalattrs4(NCDRNO* drno, int ncid, CDFnode* root)
{
    int i;
    const char* txt;
    char *nltxt, *p;
    NCerror ncstat = NC_NOERR;

    if(root->attributes != NULL) {
        for(i=0;i<nclistlength(root->attributes);i++) {
   	    NCattribute* att = (NCattribute*)nclistget(root->attributes,i);
	    ncstat = buildattribute4a(drno,att,NC_GLOBAL,ncid);
            if(ncstat != NC_NOERR) {THROWCHK(ncstat); goto fail;}
	}
    }

    /* Define some additional system global attributes depending
       on show= clientparams*/
    /* Ignore failures*/

    if(paramcheck34(drno,"show","translate")) {
        /* Add a global attribute to show the translation */
        ncstat = nc_put_att_text(ncid,NC_GLOBAL,"_translate",
	           strlen("netcdf-4"),"netcdf-4");
    }

    if(paramcheck34(drno,"show","url")) {
	if(drno->dap.urltext != NULL)
            ncstat = nc_put_att_text(ncid,NC_GLOBAL,"_url",
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
            ncstat = nc_put_att_text(ncid,NC_GLOBAL,"_DDS",strlen(nltxt),nltxt);
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
            ncstat = nc_put_att_text(ncid,NC_GLOBAL,"_DAS",strlen(nltxt),nltxt);
	    efree(nltxt);
	}
    }

fail:
    return THROW(ncstat);
}

static NCerror
buildattribute4a(NCDRNO* drno, NCattribute* att, int varid, int ncid)
{
    NCerror ncstat = NC_NOERR;
    char* cname = cdflegalname3(att->name);
    unsigned int nvalues = nclistlength(att->values);
    unsigned int typesize = nctypesizeof(att->etype);
    void* mem = emalloc(typesize * nvalues);

    ncstat = dapcvtattrval3(att->etype,mem,att->values);
    ncstat = nc_put_att(ncid,varid,cname,att->etype,nvalues,mem);
    if(att->etype == NC_STRING) {
	int i;
	for(i=0;i<nvalues;i++) efree(((char**)mem)[i]);
    }
    efree(mem);
    free(cname);
    return THROW(ncstat);
}

static NCerror
showprojection4(NCDRNO* drno, CDFnode* var)
{
    int i,rank;
    NCerror ncstat = NC_NOERR;
    NCbytes* projection = ncbytesnew();
    NClist* path = nclistnew();

    /* If this is not a true leaf variable, then ignore it */
    if(var->nctype == NC_Sequence) return NC_NOERR;
    
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
    ncstat = nc_put_att_text(getncid(drno),var->ncid,
                               "_projection",
		               ncbyteslength(projection),
			       ncbytescontents(projection));
    return ncstat;
}


static unsigned long
cdftotalsize4(NClist* dimensions)
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

static size_t
estimatesizes4r(NCDRNO* drno, CDFnode* node)
{
    int i;
    size_t size = 0;
    size_t totalsize = 0;
    NClist* ncdims;
    unsigned int rank;
    unsigned long dimsize;

    for(i=0;i<nclistlength(node->subnodes);i++) {
	CDFnode* subnode = (CDFnode*)nclistget(node->subnodes,i);
        size += estimatesizes4r(drno,subnode);
    }
    switch (node->nctype) {
    case NC_Primitive:
	node->estimatedsize = nctypesizeof(node->etype);
	break;
    case NC_Dataset:
    case NC_Structure:
    case NC_Sequence:
    case NC_Grid:
	node->estimatedsize = size;
	break;
    default:
	PANIC1("Estimatesize: Unexpected node type: %d",node->nctype);
	break;
    }
    /* Now, take dimensions into account */
    ncdims = node->array.dimensions;
    rank = nclistlength(ncdims);
    dimsize = cdftotalsize4(ncdims);
    totalsize = (dimsize * node->estimatedsize);
    if(!node->visible) totalsize = 0;
#ifdef DEBUG
fprintf(stderr,"estimatedsize: %s%s/%u = %lu (= %lu = %lu * %lu)\n",
	(node->visible?"":"*"),
	node->name,rank,
	totalsize,
	node->estimatedsize,
	size,dimsize);
#endif
    node->estimatedsize = totalsize;
    return totalsize;
}


static void
estimatesizes4(NCDRNO* drno)
{
    size_t totalsize;
    CDFnode* root = drno->cdf.ddsroot;
    /* Recursively compute the sizes of each node */
    totalsize = estimatesizes4r(drno,root);
}

/*
For variables which have a zero size dimension,
either use unlimited, or make them invisible.
*/
static NCerror
fixzerodims4(NCDRNO* drno)
{
    int i;
    NCerror ncstat = NC_NOERR;
    for(i=0;i<nclistlength(drno->cdf.varnodes);i++) {
	CDFnode* var = (CDFnode*)nclistget(drno->cdf.varnodes,i);
	ncstat = fixzerodims4r(drno,var);
    }
    return ncstat;
}

static NCerror
fixzerodims4r(NCDRNO* drno, CDFnode* node)
{
    int i;
    NCerror ncstat = NC_NOERR;
    if(nclistlength(node->array.dimensions) > 0) {
        for(i=0;i<nclistlength(node->array.dimensions);i++) {
	    CDFnode* dim = (CDFnode*)nclistget(node->array.dimensions,i);
	    if(dim->dim.declsize == 0) {
	        if(node->container->nctype == NC_Dataset) { /* use unlimited */
		    ncstat = cvtunlimiteddim(drno,dim);
		} else { /* make node invisible */
		    node->visible = 0;
		    node->zerodim = 1;
		}
	    }
	}
    }
    /* walk the subnodes */    
    for(i=0;i<nclistlength(node->subnodes);i++) {
	CDFnode* subnode = (CDFnode*)nclistget(node->subnodes,i);
	ncstat = fixzerodims4r(drno,subnode);
    }
    return ncstat;
}

/* Convert a dimension to unlimited */
static NCerror
cvtunlimiteddim(NCDRNO* drno, CDFnode* dim)
{
    DIMFLAGSET(dim,CDFDIMUNLIM);
    drno->cdf.unlimited = dim;
    return NC_NOERR;
}

static void
applyclientparamcontrols4(NCDRNO* drno)
{
    NClist* params = NULL;
    const char* value;

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

    /* Set the translation base  */
    drno->controls.flags |= (NCF_NC4);

    /* No longer need params */
    dapparamfree(params);
}
