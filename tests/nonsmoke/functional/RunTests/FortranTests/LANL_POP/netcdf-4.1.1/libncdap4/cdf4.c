/*********************************************************************
 *   Copyright 1993, UCAR/Unidata
 *   See netcdf/COPYRIGHT file for copying and redistribution conditions.
 *   $Header: /upc/share/CVS/netcdf-3/libncdap4/cdf4.c,v 1.15 2009/11/14 22:33:23 dmh Exp $
 *********************************************************************/
#include "config.h"
#include "ncdap4.h"
#include "dapalign.h"
#include "dapdump.h"

/* Forward*/
static NCerror computevarnodes4(NCDRNO*, NClist*);
static NCerror computeusertypes4r(NCDRNO*, CDFnode* tnode, NClist* usertypes);
static NCerror computetypesizes4(NCDRNO* drno, CDFnode* tnode);
static unsigned long cdftotalsize(NClist* dimensions);
static int getpadding(int offset, int alignment);

/* Accumulate useful node sets  */
NCerror
computecdfnodesets4(NCDRNO* drno)
{
    unsigned int i;
    NClist* varnodes = nclistnew(); 
    NClist* allnodes = drno->cdf.ddsroot->tree->nodes;

    if(drno->cdf.seqnodes == NULL) drno->cdf.seqnodes = nclistnew();
    if(drno->cdf.gridnodes == NULL) drno->cdf.gridnodes = nclistnew();
    nclistclear(drno->cdf.seqnodes);
    nclistclear(drno->cdf.gridnodes);

    /* Separately define the variable nodes */
    computevarnodes4(drno,varnodes);
    nclistfree(drno->cdf.varnodes);
    drno->cdf.varnodes = varnodes;

    for(i=0;i<nclistlength(allnodes);i++) {
	CDFnode* node = (CDFnode*)nclistget(allnodes,i);
	if(!node->visible) continue;
	switch (node->nctype) {
	case NC_Sequence:
	    nclistpush(drno->cdf.seqnodes,(ncelem)node);
	    break;
	case NC_Grid:
	    nclistpush(drno->cdf.gridnodes,(ncelem)node);
	    break;
	default: break;
	}
    }
    return NC_NOERR;
}

/*

The variables for a DAP->netcdf-4 translation are defined by
the top-level objects in the DAP tree.
Grids and Structures are converted to netcdf-4
structures. Sequences are converted to vlens. 
One exception about Sequences. If a sequence
with a single primitive field exists, then that field
aliases the sequence, although the var node is still
the sequence node.
*/

static NCerror
computevarnodes4(NCDRNO* drno, NClist* varnodes)
{
    unsigned int i;
    NClist* toplevel = drno->cdf.ddsroot->subnodes;

    for(i=0;i<nclistlength(toplevel);i++) {
	CDFnode* var = (CDFnode*)nclistget(toplevel,i);
	if(!var->visible) continue;
	if(var->nctype == NC_Sequence && singletonsequence(var)) {
	    var->singleton = 1;
	}
	nclistpush(varnodes,(ncelem)var);
    }
    return NC_NOERR;
}

NCerror
fixgrids4(NCDRNO* drno)
{
    unsigned int i;
    NClist* topgrids = nclistnew();
    NClist* gridnodes = drno->cdf.gridnodes;

    /* fix the dimensions of all grids (also collect top-level grids)*/
    for(i=0;i<nclistlength(gridnodes);i++) {
        CDFnode* grid = (CDFnode*)nclistget(gridnodes,i);
        if(daptoplevel(grid)) nclistpush(topgrids,(ncelem)grid);	    
	(void)fixgrid34(drno,grid); /* Ignore mal-formed grids */
    }

#ifdef IGNORE
    /* Rename selected array variables */
    for(i=0;i<nclistlength(vars);i++) {
        CDFnode* var = (CDFnode*)nclistget(vars,i);
	CDFnode* grid = var->container;
	if(grid == NULL || grid->nctype != NC_Grid) continue;
	if(strcmp(grid->name,var->name)==0) {
	    /* shorten the var name */
	    efree(var->ncfullname);
	    var->ncfullname = nulldup(grid->ncbasename);
	    MEMCHECK(var->ncfullname,NC_ENOMEM);
	}
    }
#endif

#ifdef IGNORE
    /* Attempt to hoist the top-level grid arrays and maps */
    /* Find vars that have the same base name as some other var;
       and remove corresponding grid container (if any) */
    for(i=0;i<nclistlength(vars);i++) {
	int j;
        CDFnode* var = (CDFnode*)nclistget(vars,i);
	if(var->container == NULL || var->container->nctype != NC_Grid)
	    continue;
        for(j=0;j<nclistlength(vars);j++) {
            CDFnode* testvar = (CDFnode*)nclistget(vars,j);
	    if(var == testvar) continue;
	    if(strcmp(testvar->ncbasename,var->ncbasename)==0) {
	        nclistdeleteall(topgrids,(ncelem)var->container);
	        nclistdeleteall(topgrids,(ncelem)testvar->container);
	    }
	}
    }

    /* hoist remaining grids vars */
    for(i=0;i<nclistlength(vars);i++) {
        CDFnode* var = (CDFnode*)nclistget(vars,i);
	if(nclistcontains(topgrids,(ncelem)var->container)) {
	    efree(var->ncfullname);
	    var->ncfullname = nulldup(var->ncbasename);
	    MEMCHECK(var,NC_ENOMEM);
	}
    }
#endif
    nclistfree(topgrids);
    return NC_NOERR;
}

#ifdef IGNORE
NCerror
computecdfdimnames4(NCDRNO* drno)
{
    int i,j;
    NClist* topdims = nclistnew();
    NCerror ncstat = NC_NOERR;

    /* Collect the dimensions that are referenced by
       top-level variables; these are the only dimensions
       that need to be defined under the current translation.
    */    
    for(i=0;i<nclistlength(drno->cdf.varnodes);i++) {
	CDFnode* var = (CDFnode*)nclistget(drno->cdf.varnodes,i);
	if(nclistlength(var->array.dimensions) == 0) continue;
	for(j=0;j<nclistlength(var->array.dimensions);j++) {
	    CDFnode* dim = (CDFnode*)nclistget(var->array.dimensions,j);
	    if(nclistcontains(topdims,(ncelem)dim)) continue;
	    nclistpush(topdims,(ncelem)dim);
	}
    }
    ncstat = computecdfdimnames34(drno);
    /* clean up*/
    nclistfree(topdims);
    return ncstat;
}
#endif

NCerror
computetypenames4(NCDRNO* drno, CDFnode* tnode)
{
    char* basename;
    char* tname;
    if(tnode->nctype == NC_Primitive) {
	/* Use the field name directly as its type field name */
	ASSERT((tnode->ncbasename != NULL));
        if(tnode->typename != NULL) efree(tnode->typename);	
	tnode->typename = nulldup(tnode->ncbasename);
	return NC_NOERR;
    }
    /* Otherwise: non-primitive */
    basename = makecdfpathstring3(tnode,drno->cdf.separator);
    /* Use special naming for Sequences */
    if(tnode->nctype != NC_Sequence) {
        tname = (char*)emalloc(strlen(basename)+strlen("_t")+1);
	MEMCHECK(tname,NC_ENOMEM);
        strcpy(tname,basename);
        strcat(tname,"_t");
	efree(tnode->typename);
        tnode->typename = tname;
    } else { /* Sequence */
        tname = (char*)emalloc(strlen(basename)+strlen("_record")+1);
	MEMCHECK(tname,NC_ENOMEM);
        strcpy(tname,basename);
        strcat(tname,"_record");
	efree(tnode->typename);
        tnode->typename = tname;
	/* Also create the vlen name */
        tname = (char*)emalloc(strlen(basename)+strlen("_seq_t")+1);
	MEMCHECK(tname,NC_ENOMEM);
        strcpy(tname,basename);
        strcat(tname,"_seq_t");
        if(tnode->vlenname != NULL) efree(tnode->vlenname);
        tnode->vlenname = tname;
    }
    efree(basename);
    return NC_NOERR;
}

void
setvarbasetype(NCDRNO* drno, CDFnode* field)
{
    switch (field->nctype) {
    case NC_Primitive: {
	CDFnode* seq = field->container;
	if(seq->nctype == NC_Sequence && nclistlength(seq->subnodes) == 1) {
	    /* Make the type of the field be the parent*/
	    field->typeid = seq->typeid;
	} else
	    field->typeid = field->etype;
      } break;
    default: break;
    }
}

static NCerror
computetypesizes4(NCDRNO* drno, CDFnode* tnode)
{
    unsigned int i;
    CDFnode* field;
    size_t offset, alignment, maxalign;

    if(tnode->typesize.aligned) return NC_NOERR;

    switch (tnode->nctype) {
    case NC_Grid:
    case NC_Structure:
	offset = 0;
#ifdef ALIGNCHECK
fprintf(stderr,"computesize: struct=%s:\n",tnode->name);
#endif
	/* Alignment of a struct is that of its most stringent field */
	maxalign = 0; 
        /* Recurse on each field */	
        for(i=0;i<nclistlength(tnode->subnodes);i++) {
            field = (CDFnode*)nclistget(tnode->subnodes,i);
	    if(!field->visible) continue;
	    computetypesizes4(drno,field);
	    if(field->typesize.field.alignment > maxalign)
		maxalign = field->typesize.field.alignment;
	    alignment = field->typesize.field.alignment;
	    offset += getpadding(offset,alignment);
	    field->typesize.field.offset = offset;
	    offset += field->typesize.field.size;
#ifdef ALIGNCHECK
fprintf(stderr,"\tfield=%s\n", field->name);
fprintf(stderr,"\t\tinstance: %s\n",dumpalign(&field->typesize.instance));
fprintf(stderr,"\t\tfield: %s\n",dumpalign(&field->typesize.field));
#endif
	}
	/* The instance size of a struct must be rounded to its alignment */
	tnode->typesize.instance.size = (offset + nccpadding(offset,maxalign));
        tnode->typesize.instance.alignment = maxalign;
        tnode->typesize.instance.offset = 0;
	/* The field size is instance size * # array elements */
	tnode->typesize.field = tnode->typesize.instance;
        tnode->typesize.field.size = tnode->typesize.instance.size
			    * cdftotalsize(tnode->array.dimensions);
#ifdef ALIGNCHECK
fprintf(stderr,"computesize.final: struct (%s):\n",tnode->name);
fprintf(stderr,"\tinstance: %s\n",dumpalign(&tnode->typesize.instance));
fprintf(stderr,"\tfield: %s\n",dumpalign(&tnode->typesize.field));
fprintf(stderr,"\n");
#endif
	break;

    case NC_Sequence:
	/* The Sequence instance is like a structure */
#ifdef ALIGNCHECK
fprintf(stderr,"computesize: sequence=%s:\n",tnode->name);
#endif
	offset = 0;
	maxalign = 0;
        for(i=0;i<nclistlength(tnode->subnodes);i++) {
            field = (CDFnode*)nclistget(tnode->subnodes,i);
	    if(!field->visible) continue;
	    computetypesizes4(drno,field);
	    if(field->typesize.field.alignment > maxalign)
		maxalign = field->typesize.field.alignment;
	    alignment = field->typesize.field.alignment;
	    offset += getpadding(offset,alignment);
	    field->typesize.field.offset = offset;
	    offset += field->typesize.field.size;
#ifdef ALIGNCHECK
fprintf(stderr,"\tfield=%s\n", field->name);
fprintf(stderr,"\t\tinstance: %s\n",dumpalign(&field->typesize.instance));
fprintf(stderr,"\t\tfield: %s\n",dumpalign(&field->typesize.field));
#endif
	}
	/* The instance size of a record must be rounded to its alignment */
	tnode->typesize.instance.size = (offset + nccpadding(offset,maxalign));
	tnode->typesize.instance.alignment = maxalign;
	tnode->typesize.instance.offset = 0;
	/* set field alignment, etc that of nc_vlen_t */
	tnode->typesize.field.alignment = ncctypealignment(NC_VLEN);
	tnode->typesize.field.size = sizeof(nc_vlen_t);
	tnode->typesize.field.offset = 0;
#ifdef ALIGNCHECK
fprintf(stderr,"computesize.final: sequence (%s):\n",tnode->name);
fprintf(stderr,"\tinstance: %s\n",dumpalign(&tnode->typesize.instance));
fprintf(stderr,"\tfield: %s\n",dumpalign(&tnode->typesize.field));
fprintf(stderr,"\n");
#endif
  	break;

    case NC_Primitive:
	tnode->typeid = tnode->etype;
        tnode->typesize.instance.size = nctypesizeof(tnode->etype);
        tnode->typesize.instance.alignment = ncctypealignment(tnode->etype);
	tnode->typesize.instance.offset = 0;
	tnode->typesize.field = tnode->typesize.instance; /* except for... */
        tnode->typesize.field.size = tnode->typesize.instance.size
                            * cdftotalsize(tnode->array.dimensions);
	break;

    default: PANIC1("unexpected nctype: %d",tnode->nctype);
    }
    tnode->typesize.aligned = 1;

    return NC_NOERR;
}


static unsigned long
cdftotalsize(NClist* dimensions)
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

static int
getpadding(int offset, int alignment)
{
    int rem = (alignment==0?0:(offset % alignment));
    int pad = (rem==0?0:(alignment - rem));
    return pad;
}


NCerror
computeusertypes4(NCDRNO* drno)
{
    NClist* toplevel = drno->cdf.ddsroot->subnodes;
    unsigned int i;

    drno->cdf.usertypes = nclistnew();
    for(i=0;i<nclistlength(toplevel);i++) {
	CDFnode* node = (CDFnode*)nclistget(toplevel,i);
	if(!node->visible) continue;
        computeusertypes4r(drno,node,drno->cdf.usertypes);
    }
    return NC_NOERR;
}

static NCerror
computeusertypes4r(NCDRNO* drno, CDFnode* tnode, NClist* usertypes)
{
    unsigned int i;
    unsigned int fieldcount;

    switch (tnode->nctype) {
    case NC_Primitive:
        break;
    case NC_Grid:
    case NC_Sequence:
    case NC_Structure:
	fieldcount = 0;
        for(i=0;i<nclistlength(tnode->subnodes);i++) {
	    CDFnode* sub = (CDFnode*)nclistget(tnode->subnodes,i);
	    if(!sub->visible) continue;
	    computeusertypes4r(drno,sub,usertypes);
	    fieldcount++;
	}
	/* if a struct/seq has no visible fields, then make it invisible */
	if(fieldcount == 0) tnode->visible = 0;
        nclistpush(usertypes,(ncelem)tnode);
	if(tnode->nctype == NC_Sequence && singletonsequence(tnode))
	    tnode->singleton = 1;
	break;
    default: break;
    }
    computetypesizes4(drno,tnode);
    computetypenames4(drno,tnode);
    return NC_NOERR;
}


/**************************************************/
/* Test for special sequence field translation:
   namely undimensioned field whose base type is
   primitive and whose parent container has
   only one (visible) field.
   (Actually allow test on field and on the Sequence)
*/

int
singletonsequence(CDFnode* node)
{
    CDFnode* thefield;
    if(node == NULL) return 0;
    if(node->nctype == NC_Primitive) return singletonsequence(node->container);
    if(!node->nctype == NC_Sequence) return 0;
    thefield = getsingletonfield(node->subnodes);
    return (thefield == NULL?0:1);
}

/* Return the single primitive visible field from the list;
   return NULL otherwise */
CDFnode*
getsingletonfield(NClist* list)
{
    int i,fieldcount = 0;
    CDFnode* thefield = NULL;
    for(i=0;i<nclistlength(list);i++) {
	CDFnode* field = (CDFnode*)nclistget(list,i);
	if(!field->visible) continue;
        fieldcount++;
	if(field->nctype == NC_Primitive && nclistlength(field->array.dimensions) == 0)
	    thefield = field;
    }
    if(fieldcount != 1) thefield = NULL; /* not the right type of field */
    return thefield;
}


/**************************************************/

/* Do not include the final Dataset */
static void
reversepath(CDFnode* node, NClist* rpath)
{
    nclistclear(rpath);
    while(node != NULL && node->nctype != NC_Dataset) {
        nclistpush(rpath,(ncelem)node);
	node = node->container;
    }
}

/* Compute the reverse path name to some length */
static void
reversepathstring(unsigned int count, NClist* path, NCbytes* name,
                  const char* separator)
{
    unsigned int i, depth;
    ncbytesclear(name);
    if(count > nclistlength(path)) count = nclistlength(path);
    for(depth=(count-1),i=0;i<count;i++,depth--) {
	CDFnode* node = (CDFnode*)nclistget(path,depth);
	if(i>0)	ncbytescat(name,(char*)separator);
	ncbytescat(name,node->ncbasename);
    }	
}

NCerror
shortentypenames4(NCDRNO* drno)
{
    unsigned int i,j,len,depth;
    NClist* containers = nclistnew();
    NClist* rpath = nclistnew();
    NClist* unique = nclistnew();
    NCbytes* name = ncbytesnew();

    /* Collect the reverse paths for all the container user types */
    for(i=0;i<nclistlength(drno->cdf.usertypes);i++) {
	CDFnode* tnode = (CDFnode*)nclistget(drno->cdf.usertypes,i);
	switch (tnode->nctype) {
	case NC_Structure: case NC_Sequence: case NC_Grid:
	    nclistpush(containers,(ncelem)tnode);
	    break;
	default: break;
	}
    }

    /* repeatedly attempt to create names of increasing length
       until no more conflicts
    */
    
    depth=0;
    do {
	depth++;
	len = nclistlength(containers);
	/* compute revised full name for each container */
	for(i=0;i<len;i++) {
	    CDFnode* container = (CDFnode*)nclistget(containers,i);
	    reversepath(container,rpath);
	    reversepathstring(depth,rpath,name,drno->cdf.separator);
	    efree(container->ncfullname);
	    container->ncfullname = ncbytesdup(name);
	}
	/* Now, look for the unique elements */
        nclistclear(unique);
	for(i=0;i<len;i++) {
	    CDFnode* orig = (CDFnode*)nclistget(containers,i);
	    int match = 0;
	    for(j=(i+1);j<len;j++) {
	        CDFnode* dup = (CDFnode*)nclistget(containers,j);
		if(strcmp(dup->ncfullname,orig->ncfullname)==0){
		    match=1;
		    break;
		}
	    }
	    if(!match) {
		nclistpush(unique,(ncelem)orig);
	    }
	}
	/* remove the unique names from further consideration */
	nclistminus(containers,unique);
    } while(nclistlength(containers) > 0);
    /* Now, go thru and rename all the user types */
    for(i=0;i<nclistlength(drno->cdf.usertypes);i++) {
	CDFnode* tnode = (CDFnode*)nclistget(drno->cdf.usertypes,i);
	switch (tnode->nctype) {
	case NC_Structure: case NC_Grid:
	    efree(tnode->typename);
	    tnode->typename = (char*)emalloc(strlen(tnode->ncfullname)
					     + strlen("_t")+1);
	    MEMCHECK(tnode->typename,NC_ENOMEM);
	    strcpy(tnode->typename,tnode->ncfullname);
	    strcat(tnode->typename,"_t");
	    break;
	case NC_Sequence:
	    efree(tnode->typename);
	    tnode->typename = (char*)emalloc(strlen(tnode->ncfullname)
					     + strlen("_record_t")+1);
	    MEMCHECK(tnode->typename,NC_ENOMEM);
	    strcpy(tnode->typename,tnode->ncfullname);
	    strcat(tnode->typename,"_record_t");
	    efree(tnode->vlenname);
	    tnode->vlenname = (char*)emalloc(strlen(tnode->ncfullname)
					         + strlen("_t")+1);
	    MEMCHECK(tnode->vlenname,NC_ENOMEM);
	    strcpy(tnode->vlenname,tnode->ncfullname);
	    strcat(tnode->vlenname,"_t");
	    break;
	default: break;
	}
    }
    nclistfree(containers);
    nclistfree(rpath);
    nclistfree(unique);
    ncbytesfree(name);
    return NC_NOERR;
}
