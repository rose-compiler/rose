/* Copyright 2009, UCAR/Unidata and OPeNDAP, Inc.
   See the COPYRIGHT file for more information. */

#ifndef DAPPARSELEX_H
#define DAPPARSELEX_H 1

#include "ocinternal.h"
#include "ocdebug.h"
#ifdef USE_DAP
/* To avoid "make distclean" wiping out dap.tab.h */
#include "daptab.h"
#else
#include "dap.tab.h"
#endif

#ifdef WIN32
#define strcasecmp stricmp
#define snprintf _snprintf
#endif

/* For consistency with Java parser */
#define null NULL

typedef void* Object;

#define YYSTYPE Object

#define MAX_TOKEN_LENGTH 1024

/*! Specifies the Lexstate. */
typedef struct DAPlexstate {
    char* input;
    char* next; /* next char in uri.query*/
    OCbytes* yytext;
    int lineno;
    /*! Specifies the Lasttoken. */
    int lasttoken;
    char lasttokentext[MAX_TOKEN_LENGTH+1];
    char* wordchars1;
    char* wordcharsn;
    char* worddelims;
    OClist* reclaim; /* reclaim SCAN_WORD instances */
} DAPlexstate;

/*! Specifies the DAPparsestate. */
typedef struct DAPparsestate {
    struct OCnode* root;
    DAPlexstate* lexstate;
    OClist* ocnodes;
    struct OCstate* conn;
    /* For error returns from the server */
    int svcerror; /* 1 => we had an error from the server */
    char* code;
    char* message;
    char* progtype;
    char* progname;
    /* State for constraint expressions */
    struct CEstate* cestate;
} DAPparsestate;

extern int yyerror(DAPparsestate*,char*);
extern void dap_parse_error(DAPparsestate*,const char *fmt, ...);
/* bison parse entry point */
extern int dapparse(DAPparsestate*);

extern Object datasetbody(DAPparsestate*,Object decls, Object name);
extern Object declarations(DAPparsestate*,Object decls, Object decl);
extern Object arraydecls(DAPparsestate*,Object arraydecls, Object arraydecl);
extern Object arraydecl(DAPparsestate*,Object name, Object size);

extern void attributesetup(DAPparsestate*);
extern Object attributebody(DAPparsestate*,Object attrlist);
extern Object attrlist(DAPparsestate*,Object attrlist, Object attrtuple);
extern Object attribute(DAPparsestate*,Object name, Object value, Object etype);
extern Object attrset(DAPparsestate*,Object name, Object attributes);
extern Object attrvalue(DAPparsestate*,Object valuelist, Object value, Object etype);

extern Object makebase(DAPparsestate*,Object name, Object etype, Object dimensions);
extern Object makestructure(DAPparsestate*,Object name, Object dimensions, Object fields);
extern Object makesequence(DAPparsestate*,Object name, Object members);
extern Object makegrid(DAPparsestate*,Object name, Object arraydecl, Object mapdecls);

extern Object errorbody(DAPparsestate*, Object, Object, Object, Object);
extern Object unrecognizedresponse(DAPparsestate*);

/* Lexer entry points */
extern int daplex(YYSTYPE*, DAPparsestate*);
extern void daplexinit(char* input, DAPlexstate** lexstatep);
extern void daplexcleanup(DAPlexstate** lexstatep);

#endif /*DAPPARSELEX_H*/
