/*********************************************************************
 *   Copyright 1993, UCAR/Unidata
 *   See netcdf/COPYRIGHT file for copying and redistribution conditions.
 *   $Header: /upc/share/CVS/netcdf-3/libncdap3/daprename.h,v 1.8 2009/09/23 22:26:01 dmh Exp $
 *********************************************************************/
/*
Define rename criteris
NETCDF4=no  => use netcdf.h
NETCDF4=yes => use netcdf3.h then nctonc3.h + RENAME
where
netcdf3l.h is netcdf3 with nc3_* converted to lnc3_*
nctonc3.h converts nc_* to nc3_

See also libsrc/rename.h
*/

#ifndef USE_NETCDF4
# include "netcdf3l.h"
# define RENAMEDAP(name) l3nc_##name
# define RENAME3(name) nc_##name
#endif

#ifdef USE_NETCDF4
# include "netcdf3l.h"
# define RENAMEDAP(name) l3nc_##name
# define RENAME3(name) nc3_##name
#endif

