/*********************************************************************
 *   Copyright 1993, UCAR/Unidata
 *   See netcdf/COPYRIGHT file for copying and redistribution conditions.
 *   $Header: /upc/share/CVS/netcdf-3/libncdap3/dapdump.h,v 1.9 2010/04/10 20:56:37 dmh Exp $
 *********************************************************************/
#ifndef DUMP_H
#define DUMP_H

typedef struct Dimschema {
    int dimid;
/*    int cloneid;*/
    size_t size;
    char name[NC_MAX_NAME+1];
} Dim;

typedef struct Varschema {
    int varid;
/*    int cloneid;*/
    char name[NC_MAX_NAME+1];
    nc_type nctype;
    int ndims;
    int dimids[NC_MAX_VAR_DIMS];
    size_t nelems; /*# elements*/
    size_t alloc; /* malloc size*/
    int natts;
    NCattribute* atts;
} Var;

typedef struct NChdr {
    int ncid;
    int format;
    int ndims;
    int nvars;
    int ngatts;
    int unlimid; /* id of the (1) unlimited dimension*/
    Dim* dims;
    Var* vars;
    NCattribute* gatts;
    NCbytes* content;
} NChdr;

extern int dumpmetadata(int ncid, NChdr**);
extern void dumpdata1(nc_type nctype, size_t index, char* data);


extern char* dumpprojection1(struct NCprojection* projections);
extern char* dumpprojections(NClist* projections);
extern char* dumpselections(NClist* selections);
extern char* dumpselection1(NCselection* sel);
extern char* dumpconstraint(NCconstraint* con);
extern char* dumpsegments(NClist* segments);
extern char* dumpsegment(NCsegment*);
extern char* dumpslices(NCslice* slice, unsigned int rank);
extern char* dumpslice(NCslice* slice);

extern char* dumppath(struct CDFnode* node);
extern char* dumptree(CDFnode* root);
extern char* dumpvisible(CDFnode* root);
extern char* dumpnode(CDFnode* node);

extern char* dumpalign(struct NCalignment*);

extern char* dumpcachenode(NCcachenode* node);
extern char* dumpcache(NCcache* cache);


#endif /*DUMP_H*/
