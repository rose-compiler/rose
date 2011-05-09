/* Copyright 2009, UCAR/Unidata and OPeNDAP, Inc.
   See the COPYRIGHT file for more information. */

#ifndef DAPURL_H
#define DAPURL_H

/*! This is an open structure meaning
	it is ok to directly access its fields*/
typedef struct DAPURL {
    char* url;        /* as passed by the caller */
    char* base;	      /*!< without constraints*/
    char* protocol;
    char* projection; /*!< without leading '?'*/
    char* selection;  /*!< with leading '&'*/
    char* params;
} DAPURL;

extern int dapurlparse(const char* s, DAPURL* dapurl);
extern void dapurlclear(DAPURL* dapurl);/*!<Release strings associated
                                           with the DAPURL, but NOT the struct
                                           itself; that is caller's duty.*/

/*! NULL result => entry not found.
    Empty value should be represented as a zero length string */
extern const char* dapurllookup(DAPURL*, const char* clientparam);

/* ! If value is NULL, then param is removed.
return value = 1 => found and inserted/replaced/deleted;
               0 => param not found
*/
extern int dapurlreplace(DAPURL*, const char* clientparam, const char* value);

/* Replace the constraints */
extern void dapurlsetconstraints(DAPURL*,const char* constraints);

/* Construct a complete DAP URL; caller frees returned string */
extern char* dapurlgeturl(DAPURL*,const char* prefix, const char* suffix, int withconstraints);

#endif /*DAPURL_H*/
