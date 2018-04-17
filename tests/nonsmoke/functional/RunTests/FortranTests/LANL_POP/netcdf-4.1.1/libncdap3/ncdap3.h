 /*********************************************************************
  *   Copyright 1993, UCAR/Unidata
  *   See netcdf/COPYRIGHT file for copying and redistribution conditions.
  *   $Header: /upc/share/CVS/netcdf-3/libncdap3/ncdap3.h,v 1.33 2009/11/23 19:07:56 dmh Exp $
  *********************************************************************/
#ifndef NCDAP3_H
#define NCDAP3_H 1

#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <string.h>
#include <stdio.h>

#include "config.h"
#include "netcdf.h"
#include "nc.h"
#include "daprename.h"

 /* netcdf overrides*/
#include "dapnc.h"

#include "ncdap.h"
#include "getvara.h"
#include "constraints3.h"

#ifndef USE_NETCDF4
#define NC_MAX_BYTE 127
#define NC_MIN_BYTE (-NC_MAX_BYTE-1)
#define NC_MAX_CHAR 255
#define NC_MAX_SHORT 32767
#define NC_MIN_SHORT (-NC_MAX_SHORT - 1)
#define NC_MAX_INT 2147483647
#define NC_MIN_INT (-NC_MAX_INT - 1)
#define NC_MAX_FLOAT 3.402823466e+38f
#define NC_MIN_FLOAT (-NC_MAX_FLOAT)
#define NC_MAX_DOUBLE 1.7976931348623157e+308 
#define NC_MIN_DOUBLE (-NC_MAX_DOUBLE)
#define NC_MAX_UBYTE NC_MAX_CHAR
#define NC_MAX_USHORT 65535U
#define NC_MAX_UINT 4294967295U
#define NC_MAX_INT64 (9223372036854775807LL)
#define NC_MIN_INT64 (-9223372036854775807LL-1)
#define NC_MAX_UINT64 (18446744073709551615ULL)
#define X_INT64_MAX     (9223372036854775807LL)
#define X_INT64_MIN     (-X_INT64_MAX - 1)
#define X_UINT64_MAX    (18446744073709551615ULL)
#endif /*USE_NETCDF4*/


#define NCIOTEMP "/tmp/ncioXXXXXX"

 /* Import some internal procedures from libsrc*/

extern void drno_add_to_NCList(struct NC *ncp);
extern void drno_del_from_NCList(struct NC *ncp);
extern void drno_free_NC(struct NC *ncp);
extern struct NC* drno_new_NC(const size_t *chunkp);
extern void drno_set_numrecs(NC* ncp, size_t size);
extern size_t drno_get_numrecs(NC* ncp);
extern int drno_ncio_open(NC* ncp, const char* path, int mode);

 /* Internal, but non-static procedures */
extern NCerror computecdfvarnames3(NCDRNO*,CDFnode*,NClist*);
extern NCerror computecdfnodesets3(NCDRNO* drno);
extern NCerror computevarnodes3(NCDRNO*, NClist*, NClist*);
extern NCerror collectvardefdims(NCDRNO* drno, CDFnode* var, NClist* dimset);
extern NCerror fixgrids3(NCDRNO* drno);
extern NCerror dapmerge3(NCDRNO* drno, CDFnode* node, OCobject dasroot);
extern NCerror sequencecheck3(NCDRNO* drno);
extern NCerror computecdfdimnames3(NCDRNO*);
extern NCerror attachdatadds3(struct NCDRNO*);
extern NCerror detachdatadds3(struct NCDRNO*);
extern void dapdispatch3init(void);

/*
extern void dereference3(NCconstraint* constraint);
extern NCerror rereference3(NCconstraint*, NClist*);
*/

extern NCerror buildvaraprojection3(struct NCDRNO*, struct Getvara*,
		     const size_t* startp, const size_t* countp, const ptrdiff_t* stridep,
		     struct NCprojection** projectionlist);

#endif /*NCDAP3_H*/
