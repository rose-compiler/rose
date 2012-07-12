/* Copyright 2009, UCAR/Unidata and OPeNDAP, Inc.
   See the COPYRIGHT file for more information. */

#include "ocinternal.h"
#include "ocdebug.h"

#define LBRACKET '['
#define RBRACKET ']'


/*
Client parameters are assumed to be
one or more instances of bracketed pairs:
e.g "[...][...]...".
The bracket content in turn is assumed to be a
comma separated list of <name>=<value> pairs.
e.g. x=y,z=,a=b.
If the same parameter is specifed more than once,
then the first occurrence is used; this is so that
is possible to forcibly override user specified
parameters by prefixing.
IMPORTANT: client parameter string is assumed to
have blanks compress out.
*/

OClist*
ocparamdecode(char* params0)
{
    char* cp;
    char* cq;
    int c;
    int i;
    int nparams;
    OClist* plist = oclistnew();
    char* params;
    char* params1;

    if(params0 == NULL) return plist;

    /* Pass 1 to replace beginning '[' and ending ']' */
    if(params0[0] == '[') 
	params = strdup(params0+1);
    else
	params = strdup(params0);	

    if(params[strlen(params)-1] == ']')
	params[strlen(params)-1] = '\0';

    /* Pass 2 to replace "][" pairs with ','*/
    params1 = strdup(params);
    cp=params; cq = params1;
    while((c=*cp++)) {
	if(c == RBRACKET && *cp == LBRACKET) {cp++; c = ',';}
	*cq++ = c;
    }
    *cq = '\0';
    free(params);
    params = params1;

    /* Pass 3 to break string into pieces and count # of pairs */
    nparams=0;
    for(cp=params;(c=*cp);cp++) {
	if(c == ',') {*cp = '\0'; nparams++;}
    }
    nparams++; /* for last one */

    /* Pass 4 to break up each pass into a (name,value) pair*/
    /* and insert into the param list */
    /* parameters of the form name name= are converted to name=""*/
    cp = params;
    for(i=0;i<nparams;i++) {
	char* next = cp+strlen(cp)+1; /* save ptr to next pair*/
	char* vp;
	/*break up the ith param*/
	vp = strchr(cp,'=');
	if(vp != NULL) {*vp = '\0'; vp++;} else {vp = "";}
	if(!oclistcontains(plist,(ocelem)cp)) {
	    oclistpush(plist,(ocelem)strdup(cp));
	    oclistpush(plist,(ocelem)strdup(vp));
	}
	cp = next;
    }
    free(params);
    return plist;
}

const char*
ocparamlookup(OClist* params, const char* clientparam)
{
    int i;
    if(params == NULL || clientparam == NULL) return NULL;
    for(i=0;i<oclistlength(params);i+=2) {
	char* name = (char*)oclistget(params,i);
	if(strcmp(clientparam,name)==0)
	    return (char*)oclistget(params,i+1);
    }
    return NULL;
}

void
ocparamfree(OClist* params)
{
    int i;
    if(params == NULL) return;
    for(i=0;i<oclistlength(params);i++) {
	char* s = (char*)oclistget(params,i);
	if(s != NULL) free((void*)s);
    }
    oclistfree(params);
}

/*
Delete the entry.
return value = 1 => found and deleted;
               0 => param not found
*/
int
ocparamdelete(OClist* params, const char* clientparam)
{
    int i,found = 0;
    if(params == NULL || clientparam == NULL) return 0;
    for(i=0;i<oclistlength(params);i+=2) {
	char* name = (char*)oclistget(params,i);
	if(strcmp(clientparam,name)==0) {found=1; break;}
    }
    if(found) {
	oclistremove(params,i+1); /* remove value */
	oclistremove(params,i); /* remove name */
    }
    return found;
}


/*
Insert new client param (name,value);
return value = 1 => not already defined
               0 => param already defined (no change)
*/
int
ocparaminsert(OClist* params, const char* clientparam, const char* value)
{
    int i;
    if(params == NULL || clientparam == NULL) return 0;
    for(i=0;i<oclistlength(params);i+=2) {
	char* name = (char*)oclistget(params,i);
	if(strcmp(clientparam,name)==0) return 0;
    }
    /* not found, append */
    oclistpush(params,(ocelem)strdup(clientparam));
    oclistpush(params,(ocelem)nulldup(value));
    return 1;
}


/*
Replace new client param (name,value);
return value = 1 => replacement performed
               0 => insertion performed
*/
int
ocparamreplace(OClist* params, const char* clientparam, const char* value)
{
    int i;
    if(params == NULL || clientparam == NULL) return 0;
    for(i=0;i<oclistlength(params);i+=2) {
	char* name = (char*)oclistget(params,i);
	if(strcmp(clientparam,name)==0) {
	    oclistinsert(params,i+1,(ocelem)nulldup(value));
	    return 1;
	}
    }
    ocparaminsert(params,clientparam,value);
    return 0;
}
