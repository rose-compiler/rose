/* Copyright 2009, UCAR/Unidata and OPeNDAP, Inc.
   See the COPYRIGHT file for more information. */

#ifndef OCCLIENTPARAMS_H
#define OCCLIENTPARAMS_H

extern OClist* ocparamdecode(char* params0);
extern const char* ocparamlookup(OClist* params, const char* clientparam);
extern int ocparamdelete(OClist* params, const char* clientparam);
extern int ocparaminsert(OClist* params, const char* clientparam, const char* value);
extern int ocparamreplace(OClist* params, const char* clientparam, const char* value);
extern void ocparamfree(OClist* params);

#endif /*OCCLIENTPARAMS_H*/
