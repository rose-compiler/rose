/*********************************************************************
 *   Copyright 1993, UCAR/Unidata
 *   See netcdf/COPYRIGHT file for copying and redistribution conditions.
 *   $Header: /upc/share/CVS/netcdf-3/libncdap3/dapodom.h,v 1.9 2009/11/14 22:33:21 dmh Exp $
 *********************************************************************/
/* Copyright 2009, UCAR/Unidata and OPeNDAP, Inc.
   See the COPYRIGHT file for more information. */

#ifndef DAPODOM_H
#define DAPODOM_H 1

typedef struct Dapodometer {
    int            rank;
    NCslice        slices[NC_MAX_DIMS];
    size_t         index[NC_MAX_DIMS];
} Dapodometer;

/* Odometer operators*/
extern Dapodometer* newdapodometer(NCslice* slices, unsigned int first, unsigned int count);

extern Dapodometer* newsimpledapodometer(struct NCsegment*,unsigned int);

extern Dapodometer* newdapodometer1(unsigned int count);
extern Dapodometer* newdapodometer2(const size_t*, const size_t*,
                      const ptrdiff_t*, unsigned int, unsigned int);
extern Dapodometer* newdapodometer3(int, size_t*);

extern void freedapodometer(Dapodometer*);
extern char* dapodometerprint(Dapodometer* odom);

extern int dapodometermore(Dapodometer* odom);
extern int dapodometerincr(Dapodometer* odo);
extern int dapodometerincrith(Dapodometer* odo,int);
extern size_t dapodometercount(Dapodometer* odo);
extern void dapodometerreset(Dapodometer*);
extern Dapodometer* dapodometersplit(Dapodometer* odom, int tail);

extern size_t dapodometerspace(Dapodometer* odom, unsigned int wheel);
extern size_t dapodometerpoints(Dapodometer*);

extern size_t* dapodometerindices(Dapodometer*);
extern int dapodometervarmcount(Dapodometer*, const ptrdiff_t*, const size_t*);

#endif /*DAPODOM_H*/
