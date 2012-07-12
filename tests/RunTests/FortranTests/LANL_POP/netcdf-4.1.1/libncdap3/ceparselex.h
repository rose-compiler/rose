/* Copyright 2009, UCAR/Unidata and OPeNDAP, Inc.
   See the COPYRIGHT file for more information. */

#ifndef CEPARSELEX_H
#define CEPARSELEX_H

#include "config.h"
#include "ncdap3.h"
#include "cetab.h"
#include "dapdebug.h"
#include "dapdump.h"

#ifdef WIN32
#define strcasecmp stricmp
#define snprintf _snprintf
#endif

/* For consistency with Java parser */
#define null NULL

typedef void* Object;

#define YYSTYPE Object

#define MAX_TOKEN_LENGTH 1024

/*! Specifies CElexstate. */
typedef struct CElexstate {
    char* input;
    char* next; /* next char in uri.query */
    NCbytes* yytext;
    /*! Specifies the Lasttoken. */
    int lasttoken;
    char lasttokentext[MAX_TOKEN_LENGTH+1]; /* leave room for trailing null */
    NClist* reclaim; /* reclaim SCAN_WORD instances */
} CElexstate;


/*! Specifies CEparsestate. */
typedef struct CEparsestate {
    NClist* projections;
    NClist* selections;
    char errorbuf[1024];
    int errorcode;
    CElexstate* lexstate;
    int ncconstraint;
} CEparsestate;

/* Define a generic object carrier; this serves
   essentially the same role as the typical bison %union
   declaration
*/
   
#ifdef IGNORE
typedef struct CEobject {
    NClist* list; /* contents depend on context */
    struct NCsegment* segment; /*temporary */
    struct NCprojection* projection; /*temporary */
    struct NCselection* selection; /* temporary */
} CEobject;
#endif

extern int ceerror(CEparsestate*,char*);
extern void ce_parse_error(CEparsestate*,const char *fmt, ...);
/* bison parse entry point */
extern int ceparse(CEparsestate*);

extern int yyerror(CEparsestate* state, char* msg);
extern void projections(CEparsestate* state, Object list0);
extern void selections(CEparsestate* state, Object list0);
extern Object projectionlist(CEparsestate* state, Object list0, Object decl);
extern Object projection(CEparsestate* state, Object segmentlist);
extern Object segmentlist(CEparsestate* state, Object list0, Object decl);
extern Object segment(CEparsestate* state, Object name, Object slices0);
extern Object array_indices(CEparsestate* state, Object list0, Object decl);
extern Object range(CEparsestate* state, Object, Object, Object);
extern Object selectionlist(CEparsestate* state, Object list0, Object decl);
extern Object sel_clause(CEparsestate* state, int selcase, Object path0, Object relop0, Object values);
extern Object selectionpath(CEparsestate* state, Object list0, Object text);
extern Object function(CEparsestate* state, Object fcnname, Object args);
extern Object arg_list(CEparsestate* state, Object list0, Object decl);
extern Object value_list(CEparsestate* state, Object list0, Object decl);
extern Object value(CEparsestate* state, Object text, int tag);

/* lexer interface */
extern int celex(YYSTYPE*, CEparsestate*);
extern void celexinit(char* input, CElexstate** lexstatep);
extern void celexcleanup(CElexstate** lexstatep);

extern int cedebug;

#endif /*CEPARSELEX_H*/

