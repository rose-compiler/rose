/*---------------------------------------------------------------------------*
 |              PDFlib - A library for generating PDF on the fly             |
 +---------------------------------------------------------------------------+
 | Copyright (c) 1997-2005 Thomas Merz and PDFlib GmbH. All rights reserved. |
 +---------------------------------------------------------------------------+
 |                                                                           |
 |    This software is subject to the PDFlib license. It is NOT in the       |
 |    public domain. Extended versions and commercial licenses are           |
 |    available, please check http://www.pdflib.com.                         |
 |                                                                           |
 *---------------------------------------------------------------------------*/

/* $Id: pdflib_tcl.c,v 1.1 2005/10/11 17:18:09 vuduc2 Exp $
 *
 * in sync with pdflib.h 1.151.2.22
 *
 * Wrapper code for the PDFlib Tcl binding
 *
 */

/*
 * Build with STUBS enabled
 *
 * if building with older TCL Versions than 8.2 you have to undef this
 */
#define USE_TCL_STUBS

#include <tcl.h>

#include <string.h>
#include <stdlib.h>

#if defined(__WIN32__)
#   define WIN32_LEAN_AND_MEAN
#   include <windows.h>
#   undef WIN32_LEAN_AND_MEAN

#   if defined(__WIN32__) && \
	(defined(_MSC_VER) || (defined(__GNUC__) && defined(__declspec)))
#	define SWIGEXPORT(a,b) __declspec(dllexport) a b
#   else
#	if defined(__BORLANDC__)
#	    define SWIGEXPORT(a,b) a _export b
#	else
#	    define SWIGEXPORT(a,b) a b
#	endif
#   endif
#else
#   define SWIGEXPORT(a,b) a b
#endif

#include <stdlib.h>

#ifdef SWIG_GLOBAL
#ifdef __cplusplus
#define SWIGSTATIC extern "C"
#else
#define SWIGSTATIC
#endif
#endif

#ifndef SWIGSTATIC
#define SWIGSTATIC static
#endif

/* SWIG pointer structure */

typedef struct SwigPtrType {
  char               *name;               /* Datatype name                  */
  int               len;                /* Length (used for optimization) */
  void               *(*cast)(void *);    /* Pointer casting function       */
  struct SwigPtrType *next;               /* Linked list pointer            */
} SwigPtrType;

/* Pointer cache structure */

typedef struct {
  int               stat;               /* Status (valid) bit             */
  SwigPtrType        *tp;                 /* Pointer to type structure      */
  char                name[256];          /* Given datatype name            */
  char                mapped[256];        /* Equivalent name                */
} SwigCacheType;

/* Some variables  */

static int SwigPtrMax  = 64;           /* Max entries that can be currently held */
                                       /* This value may be adjusted dynamically */
static int SwigPtrN    = 0;            /* Current number of entries              */
static int SwigPtrSort = 0;            /* Status flag indicating sort            */
static int SwigStart[256];             /* Starting positions of types            */

/* Pointer table */
static SwigPtrType *SwigPtrTable = 0;  /* Table containing pointer equivalences  */

/* Cached values */

#define SWIG_CACHESIZE  8
#define SWIG_CACHEMASK  0x7
static SwigCacheType SwigCache[SWIG_CACHESIZE];
static int SwigCacheIndex = 0;
static int SwigLastCache = 0;

/* Sort comparison function */
static int swigsort(const void *data1, const void *data2) {
	SwigPtrType *d1 = (SwigPtrType *) data1;
	SwigPtrType *d2 = (SwigPtrType *) data2;
	return strcmp(d1->name,d2->name);
}

/* Binary Search function */
static int swigcmp(const void *key, const void *data) {
  char *k = (char *) key;
  SwigPtrType *d = (SwigPtrType *) data;
  return strncmp(k,d->name,d->len);
}

/* Register a new datatype with the type-checker */

SWIGSTATIC
void SWIG_RegisterMapping(char *origtype, char *newtype, void *(*cast)(void *)) {

  int i;
  SwigPtrType *t = 0,*t1;

  /* Allocate the pointer table if necessary */

  if (!SwigPtrTable) {
    SwigPtrTable = (SwigPtrType *) malloc(SwigPtrMax*sizeof(SwigPtrType));
    SwigPtrN = 0;
  }
  /* Grow the table */
  if (SwigPtrN >= SwigPtrMax) {
    SwigPtrMax = 2*SwigPtrMax;
    SwigPtrTable = (SwigPtrType *) realloc((char *) SwigPtrTable,SwigPtrMax*sizeof(SwigPtrType));
  }
  for (i = 0; i < SwigPtrN; i++)
    if (strcmp(SwigPtrTable[i].name,origtype) == 0) {
      t = &SwigPtrTable[i];
      break;
    }
  if (!t) {
    t = &SwigPtrTable[SwigPtrN];
    t->name = origtype;
    t->len = strlen(t->name);
    t->cast = 0;
    t->next = 0;
    SwigPtrN++;
  }

  /* Check for existing entry */

  while (t->next) {
    if ((strcmp(t->name,newtype) == 0)) {
      if (cast) t->cast = cast;
      return;
    }
    t = t->next;
  }

  /* Now place entry (in sorted order) */

  t1 = (SwigPtrType *) malloc(sizeof(SwigPtrType));
  t1->name = newtype;
  t1->len = strlen(t1->name);
  t1->cast = cast;
  t1->next = 0;
  t->next = t1;
  SwigPtrSort = 0;
}

/* Make a pointer value string */

SWIGSTATIC
void SWIG_MakePtr(char *_c, const void *_ptr, char *type) {
  static char _hex[16] =
  {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
   'a', 'b', 'c', 'd', 'e', 'f'};
  unsigned long _p, _s;
  char _result[20], *_r;    /* Note : a 64-bit hex number = 16 digits */
  _r = _result;
  _p = (unsigned long) _ptr;
  if (_p > 0) {
    while (_p > 0) {
      _s = _p & 0xf;
      *(_r++) = _hex[_s];
      _p = _p >> 4;
    }
    *_r = '_';
    while (_r >= _result)
      *(_c++) = *(_r--);
  } else {
    strcpy (_c, "NULL");
  }
  if (_ptr)
    strcpy (_c, type);
}

/* Function for getting a pointer value */

SWIGSTATIC
char *SWIG_GetPtr(char *_c, void **ptr, char *_t)
{
  unsigned long _p;
  char temp_type[256];
  char *name;
  int i, len;
  SwigPtrType *sp,*tp;
  SwigCacheType *cache;
  int start, end;
  _p = 0;

  /* Pointer values must start with leading underscore */
  if (*_c == '_') {
      _c++;
      /* Extract hex value from pointer */
      while (*_c) {
	  if ((*_c >= '0') && (*_c <= '9'))
	    _p = (_p << 4) + (*_c - '0');
	  else if ((*_c >= 'a') && (*_c <= 'f'))
	    _p = (_p << 4) + ((*_c - 'a') + 10);
	  else
	    break;
	  _c++;
      }

      if (_t) {
	if (strcmp(_t,_c)) {
	  if (!SwigPtrSort) {
	    qsort((void *) SwigPtrTable, SwigPtrN, sizeof(SwigPtrType), swigsort);
	    for (i = 0; i < 256; i++) {
	      SwigStart[i] = SwigPtrN;
	    }
	    for (i = SwigPtrN-1; i >= 0; i--) {
	      SwigStart[(int) (SwigPtrTable[i].name[1])] = i;
	    }
	    for (i = 255; i >= 1; i--) {
	      if (SwigStart[i-1] > SwigStart[i])
		SwigStart[i-1] = SwigStart[i];
	    }
	    SwigPtrSort = 1;
	    for (i = 0; i < SWIG_CACHESIZE; i++)
	      SwigCache[i].stat = 0;
	  }

	  /* First check cache for matches.  Uses last cache value as starting point */
	  cache = &SwigCache[SwigLastCache];
	  for (i = 0; i < SWIG_CACHESIZE; i++) {
	    if (cache->stat) {
	      if (strcmp(_t,cache->name) == 0) {
		if (strcmp(_c,cache->mapped) == 0) {
		  cache->stat++;
		  *ptr = (void *) _p;
		  if (cache->tp->cast) *ptr = (*(cache->tp->cast))(*ptr);
		  return (char *) 0;
		}
	      }
	    }
	    SwigLastCache = (SwigLastCache+1) & SWIG_CACHEMASK;
	    if (!SwigLastCache) cache = SwigCache;
	    else cache++;
	  }
	  /* We have a type mismatch.  Will have to look through our type
	     mapping table to figure out whether or not we can accept this datatype */

	  start = SwigStart[(int) _t[1]];
	  end = SwigStart[(int) _t[1]+1];
	  sp = &SwigPtrTable[start];
	  while (start < end) {
	    if (swigcmp(_t,sp) == 0) break;
	    sp++;
	    start++;
	  }
	  if (start >= end) sp = 0;
	  /* Try to find a match for this */
	  if (sp) {
	    while (swigcmp(_t,sp) == 0) {
	      name = sp->name;
	      len = sp->len;
	      tp = sp->next;
	      /* Try to find entry for our given datatype */
	      while(tp) {
		if (tp->len >= 255) {
		  return _c;
		}
		strcpy(temp_type,tp->name);
		strncat(temp_type,_t+len,255-tp->len);
		if (strcmp(_c,temp_type) == 0) {

		  strcpy(SwigCache[SwigCacheIndex].mapped,_c);
		  strcpy(SwigCache[SwigCacheIndex].name,_t);
		  SwigCache[SwigCacheIndex].stat = 1;
		  SwigCache[SwigCacheIndex].tp = tp;
		  SwigCacheIndex = SwigCacheIndex & SWIG_CACHEMASK;

		  /* Get pointer value */
		  *ptr = (void *) _p;
		  if (tp->cast) *ptr = (*(tp->cast))(*ptr);
		  return (char *) 0;
		}
		tp = tp->next;
	      }
	      sp++;
	      /* Hmmm. Didn't find it this time */
	    }
	  }
	  /* Didn't find any sort of match for this data.
	     Get the pointer value and return the received type */
	  *ptr = (void *) _p;
	  return _c;
	} else {
	  /* Found a match on the first try.  Return pointer value */
	  *ptr = (void *) _p;
	  return (char *) 0;
	}
      } else {
	/* No type specified.  Good luck */
	*ptr = (void *) _p;
	return (char *) 0;
      }
  } else {
    if (strcmp (_c, "NULL") == 0) {
	*ptr = (void *) 0;
	return (char *) 0;
    }
    *ptr = (void *) 0;
    return _c;
  }
}

#ifdef __cplusplus
extern "C" {
#endif
SWIGEXPORT(int,Pdflib_Init)(Tcl_Interp *);
SWIGEXPORT(int,Pdflib_SafeInit)(Tcl_Interp *);
SWIGEXPORT(int,Pdflib_tcl_SafeInit)(Tcl_Interp *);
SWIGEXPORT(int,Pdflib_tcl_Init)(Tcl_Interp *);
SWIGEXPORT(int,Pdf_tcl_Init)(Tcl_Interp *);
SWIGEXPORT(int,Pdf_tcl_SafeInit)(Tcl_Interp *);
#ifdef __cplusplus
}
#endif

#include <setjmp.h>

#include "pdflib.h"

/* Exception handling */

#define try	PDF_TRY(p)
#define catch	PDF_CATCH(p) {\
		char errmsg[1024];\
		sprintf(errmsg, "PDFlib Error [%d] %s: %s", PDF_get_errnum(p),\
		    PDF_get_apiname(p), PDF_get_errmsg(p));\
		Tcl_SetResult(interp, errmsg, TCL_STATIC);\
		    return TCL_ERROR;			\
	    }


/*
 * String handling
 */

#define PDF_0BYTES 0
#define PDF_BYTES  1
#define PDF_UTF8   2
#define PDF_UTF16  3
#define PDF_0UTF16 4
#define PDF_DATA   5

static const char *
PDF_GetStringFromObj(PDF *p, Tcl_Interp *interp, Tcl_Obj *objPtr, int key,
                     int *len)
{
    Tcl_UniChar *unistring = NULL;

    *len = 0;

    if (objPtr != NULL)
    {
        switch (key)
        {
            case PDF_0BYTES:
            case PDF_BYTES:
            case PDF_DATA:
            return (const char *) Tcl_GetStringFromObj(objPtr, len);

            case PDF_UTF8:
            case PDF_UTF16:
            case PDF_0UTF16:
            unistring = Tcl_GetUnicode(objPtr);
            if (unistring)
            {
                *len = 2 * Tcl_UniCharLen(unistring);
                if (key == PDF_UTF8)
                   return PDF_utf16_to_utf8(p, (char *) unistring, *len, len);
                return (const char *) unistring;
            }
        }
    }

    return NULL;
}

static void
PDF_WrongCommand(Tcl_Interp *interp, const char *vartext)
{
    char text[128];

    sprintf(text, "Wrong # args. %s", vartext);
    Tcl_SetResult(interp, text, TCL_STATIC);
}

static void
PDF_NoPDFHandle(Tcl_Interp *interp, const char *vartext)
{
    char text[128];

    sprintf(text, "Couldn't retrieve PDF handle in %s", vartext);
    Tcl_SetResult(interp, text, TCL_STATIC);
}

static void
PDF_WrongPDFHandle(Tcl_Interp *interp, const char *vartext)
{
    char text[128];

    sprintf(text, "Wrong PDF handle in %s", vartext);
    Tcl_SetResult(interp, text, TCL_STATIC);
}

static void
PDF_WrongParameter(Tcl_Interp *interp, const char *vartext)
{
    char text[128];

    sprintf(text, "Wrong parameter %s", vartext);
    Tcl_SetResult(interp, text, TCL_STATIC);
}


static int
_wrap_PDF_activate_item(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
    PDF *p;
    int id;
    char *res;

    if (objc != 3)
    {
        PDF_WrongCommand(interp, "PDF_activate_item p id ");
        return TCL_ERROR;
    }

    if ((res = Tcl_GetStringFromObj(objv[1], NULL)) == NULL)
    {
        PDF_NoPDFHandle(interp, "PDF_activate_item");
        return TCL_ERROR;
    }

    if (SWIG_GetPtr(res, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle(interp, "PDF_activate_item");
        Tcl_AppendResult(interp, res, (char *) NULL);
        return TCL_ERROR;
    }

    if (Tcl_GetIntFromObj(interp, objv[2], &id) != TCL_OK)
    {
        PDF_WrongParameter(interp, "id in PDF_activate_item");
        return TCL_ERROR;
    }

    try { PDF_activate_item(p, id);
    } catch;

    return TCL_OK;
}


static int
_wrap_PDF_add_bookmark(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
    PDF *p;
    const char *text;
    int text_len;
    int parent;
    int open;
    char *res;
    int volatile _result = -1;

    if (objc != 5)
    {
        PDF_WrongCommand(interp, "PDF_add_bookmark p text parent open ");
        return TCL_ERROR;
    }

    if ((res = Tcl_GetStringFromObj(objv[1], NULL)) == NULL)
    {
        PDF_NoPDFHandle(interp, "PDF_add_bookmark");
        return TCL_ERROR;
    }

    if (SWIG_GetPtr(res, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle(interp, "PDF_add_bookmark");
        Tcl_AppendResult(interp, res, (char *) NULL);
        return TCL_ERROR;
    }

    if ((text = PDF_GetStringFromObj(p, interp, objv[2], PDF_UTF16, &text_len)) == NULL)
    {
        PDF_WrongParameter(interp, "text in PDF_add_bookmark");
        return TCL_ERROR;
    }

    if (Tcl_GetIntFromObj(interp, objv[3], &parent) != TCL_OK)
    {
        PDF_WrongParameter(interp, "parent in PDF_add_bookmark");
        return TCL_ERROR;
    }

    if (Tcl_GetIntFromObj(interp, objv[4], &open) != TCL_OK)
    {
        PDF_WrongParameter(interp, "open in PDF_add_bookmark");
        return TCL_ERROR;
    }

    try { _result = (int) PDF_add_bookmark2(p, text, text_len, parent, open);
    } catch;

    sprintf(interp->result, "%d", _result);
    return TCL_OK;
}


static int
_wrap_PDF_add_launchlink(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
    PDF *p;
    double llx;
    double lly;
    double urx;
    double ury;
    const char *filename;
    int filename_len;
    char *res;

    if (objc != 7)
    {
        PDF_WrongCommand(interp, "PDF_add_launchlink p llx lly urx ury filename ");
        return TCL_ERROR;
    }

    if ((res = Tcl_GetStringFromObj(objv[1], NULL)) == NULL)
    {
        PDF_NoPDFHandle(interp, "PDF_add_launchlink");
        return TCL_ERROR;
    }

    if (SWIG_GetPtr(res, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle(interp, "PDF_add_launchlink");
        Tcl_AppendResult(interp, res, (char *) NULL);
        return TCL_ERROR;
    }

    if (Tcl_GetDoubleFromObj(interp, objv[2], &llx) != TCL_OK)
    {
        PDF_WrongParameter(interp, "llx in PDF_add_launchlink");
        return TCL_ERROR;
    }

    if (Tcl_GetDoubleFromObj(interp, objv[3], &lly) != TCL_OK)
    {
        PDF_WrongParameter(interp, "lly in PDF_add_launchlink");
        return TCL_ERROR;
    }

    if (Tcl_GetDoubleFromObj(interp, objv[4], &urx) != TCL_OK)
    {
        PDF_WrongParameter(interp, "urx in PDF_add_launchlink");
        return TCL_ERROR;
    }

    if (Tcl_GetDoubleFromObj(interp, objv[5], &ury) != TCL_OK)
    {
        PDF_WrongParameter(interp, "ury in PDF_add_launchlink");
        return TCL_ERROR;
    }

    if ((filename = PDF_GetStringFromObj(p, interp, objv[6], PDF_BYTES, &filename_len)) == NULL)
    {
        PDF_WrongParameter(interp, "filename in PDF_add_launchlink");
        return TCL_ERROR;
    }

    try { PDF_add_launchlink(p, llx, lly, urx, ury, filename);
    } catch;

    return TCL_OK;
}


static int
_wrap_PDF_add_locallink(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
    PDF *p;
    double llx;
    double lly;
    double urx;
    double ury;
    int page;
    const char *optlist;
    int optlist_len;
    char *res;

    if (objc != 8)
    {
        PDF_WrongCommand(interp, "PDF_add_locallink p llx lly urx ury page optlist ");
        return TCL_ERROR;
    }

    if ((res = Tcl_GetStringFromObj(objv[1], NULL)) == NULL)
    {
        PDF_NoPDFHandle(interp, "PDF_add_locallink");
        return TCL_ERROR;
    }

    if (SWIG_GetPtr(res, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle(interp, "PDF_add_locallink");
        Tcl_AppendResult(interp, res, (char *) NULL);
        return TCL_ERROR;
    }

    if (Tcl_GetDoubleFromObj(interp, objv[2], &llx) != TCL_OK)
    {
        PDF_WrongParameter(interp, "llx in PDF_add_locallink");
        return TCL_ERROR;
    }

    if (Tcl_GetDoubleFromObj(interp, objv[3], &lly) != TCL_OK)
    {
        PDF_WrongParameter(interp, "lly in PDF_add_locallink");
        return TCL_ERROR;
    }

    if (Tcl_GetDoubleFromObj(interp, objv[4], &urx) != TCL_OK)
    {
        PDF_WrongParameter(interp, "urx in PDF_add_locallink");
        return TCL_ERROR;
    }

    if (Tcl_GetDoubleFromObj(interp, objv[5], &ury) != TCL_OK)
    {
        PDF_WrongParameter(interp, "ury in PDF_add_locallink");
        return TCL_ERROR;
    }

    if (Tcl_GetIntFromObj(interp, objv[6], &page) != TCL_OK)
    {
        PDF_WrongParameter(interp, "page in PDF_add_locallink");
        return TCL_ERROR;
    }

    if ((optlist = PDF_GetStringFromObj(p, interp, objv[7], PDF_UTF8, &optlist_len)) == NULL)
    {
        PDF_WrongParameter(interp, "optlist in PDF_add_locallink");
        return TCL_ERROR;
    }

    try { PDF_add_locallink(p, llx, lly, urx, ury, page, optlist);
    } catch;

    return TCL_OK;
}


static int
_wrap_PDF_add_nameddest(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
    PDF *p;
    const char *name;
    int name_len;
    const char *optlist;
    int optlist_len;
    char *res;

    if (objc != 4)
    {
        PDF_WrongCommand(interp, "PDF_add_nameddest p name optlist ");
        return TCL_ERROR;
    }

    if ((res = Tcl_GetStringFromObj(objv[1], NULL)) == NULL)
    {
        PDF_NoPDFHandle(interp, "PDF_add_nameddest");
        return TCL_ERROR;
    }

    if (SWIG_GetPtr(res, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle(interp, "PDF_add_nameddest");
        Tcl_AppendResult(interp, res, (char *) NULL);
        return TCL_ERROR;
    }

    if ((name = PDF_GetStringFromObj(p, interp, objv[2], PDF_UTF16, &name_len)) == NULL)
    {
        PDF_WrongParameter(interp, "name in PDF_add_nameddest");
        return TCL_ERROR;
    }

    if ((optlist = PDF_GetStringFromObj(p, interp, objv[3], PDF_UTF8, &optlist_len)) == NULL)
    {
        PDF_WrongParameter(interp, "optlist in PDF_add_nameddest");
        return TCL_ERROR;
    }

    try { PDF_add_nameddest(p, name, name_len, optlist);
    } catch;

    return TCL_OK;
}


static int
_wrap_PDF_add_note(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
    PDF *p;
    double llx;
    double lly;
    double urx;
    double ury;
    const char *contents;
    int contents_len;
    const char *title;
    int title_len;
    const char *icon;
    int icon_len;
    int open;
    char *res;

    if (objc != 10)
    {
        PDF_WrongCommand(interp, "PDF_add_note p llx lly urx ury contents title icon open ");
        return TCL_ERROR;
    }

    if ((res = Tcl_GetStringFromObj(objv[1], NULL)) == NULL)
    {
        PDF_NoPDFHandle(interp, "PDF_add_note");
        return TCL_ERROR;
    }

    if (SWIG_GetPtr(res, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle(interp, "PDF_add_note");
        Tcl_AppendResult(interp, res, (char *) NULL);
        return TCL_ERROR;
    }

    if (Tcl_GetDoubleFromObj(interp, objv[2], &llx) != TCL_OK)
    {
        PDF_WrongParameter(interp, "llx in PDF_add_note");
        return TCL_ERROR;
    }

    if (Tcl_GetDoubleFromObj(interp, objv[3], &lly) != TCL_OK)
    {
        PDF_WrongParameter(interp, "lly in PDF_add_note");
        return TCL_ERROR;
    }

    if (Tcl_GetDoubleFromObj(interp, objv[4], &urx) != TCL_OK)
    {
        PDF_WrongParameter(interp, "urx in PDF_add_note");
        return TCL_ERROR;
    }

    if (Tcl_GetDoubleFromObj(interp, objv[5], &ury) != TCL_OK)
    {
        PDF_WrongParameter(interp, "ury in PDF_add_note");
        return TCL_ERROR;
    }

    if ((contents = PDF_GetStringFromObj(p, interp, objv[6], PDF_UTF16, &contents_len)) == NULL)
    {
        PDF_WrongParameter(interp, "contents in PDF_add_note");
        return TCL_ERROR;
    }

    if ((title = PDF_GetStringFromObj(p, interp, objv[7], PDF_UTF16, &title_len)) == NULL)
    {
        PDF_WrongParameter(interp, "title in PDF_add_note");
        return TCL_ERROR;
    }

    if ((icon = PDF_GetStringFromObj(p, interp, objv[8], PDF_BYTES, &icon_len)) == NULL)
    {
        PDF_WrongParameter(interp, "icon in PDF_add_note");
        return TCL_ERROR;
    }

    if (Tcl_GetIntFromObj(interp, objv[9], &open) != TCL_OK)
    {
        PDF_WrongParameter(interp, "open in PDF_add_note");
        return TCL_ERROR;
    }

    try { PDF_add_note2(p, llx, lly, urx, ury, contents, contents_len, title, title_len, icon, open);
    } catch;

    return TCL_OK;
}


static int
_wrap_PDF_add_pdflink(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
    PDF *p;
    double llx;
    double lly;
    double urx;
    double ury;
    const char *filename;
    int filename_len;
    int page;
    const char *optlist;
    int optlist_len;
    char *res;

    if (objc != 9)
    {
        PDF_WrongCommand(interp, "PDF_add_pdflink p llx lly urx ury filename page optlist ");
        return TCL_ERROR;
    }

    if ((res = Tcl_GetStringFromObj(objv[1], NULL)) == NULL)
    {
        PDF_NoPDFHandle(interp, "PDF_add_pdflink");
        return TCL_ERROR;
    }

    if (SWIG_GetPtr(res, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle(interp, "PDF_add_pdflink");
        Tcl_AppendResult(interp, res, (char *) NULL);
        return TCL_ERROR;
    }

    if (Tcl_GetDoubleFromObj(interp, objv[2], &llx) != TCL_OK)
    {
        PDF_WrongParameter(interp, "llx in PDF_add_pdflink");
        return TCL_ERROR;
    }

    if (Tcl_GetDoubleFromObj(interp, objv[3], &lly) != TCL_OK)
    {
        PDF_WrongParameter(interp, "lly in PDF_add_pdflink");
        return TCL_ERROR;
    }

    if (Tcl_GetDoubleFromObj(interp, objv[4], &urx) != TCL_OK)
    {
        PDF_WrongParameter(interp, "urx in PDF_add_pdflink");
        return TCL_ERROR;
    }

    if (Tcl_GetDoubleFromObj(interp, objv[5], &ury) != TCL_OK)
    {
        PDF_WrongParameter(interp, "ury in PDF_add_pdflink");
        return TCL_ERROR;
    }

    if ((filename = PDF_GetStringFromObj(p, interp, objv[6], PDF_BYTES, &filename_len)) == NULL)
    {
        PDF_WrongParameter(interp, "filename in PDF_add_pdflink");
        return TCL_ERROR;
    }

    if (Tcl_GetIntFromObj(interp, objv[7], &page) != TCL_OK)
    {
        PDF_WrongParameter(interp, "page in PDF_add_pdflink");
        return TCL_ERROR;
    }

    if ((optlist = PDF_GetStringFromObj(p, interp, objv[8], PDF_UTF8, &optlist_len)) == NULL)
    {
        PDF_WrongParameter(interp, "optlist in PDF_add_pdflink");
        return TCL_ERROR;
    }

    try { PDF_add_pdflink(p, llx, lly, urx, ury, filename, page, optlist);
    } catch;

    return TCL_OK;
}


static int
_wrap_PDF_add_thumbnail(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
    PDF *p;
    int image;
    char *res;

    if (objc != 3)
    {
        PDF_WrongCommand(interp, "PDF_add_thumbnail p image ");
        return TCL_ERROR;
    }

    if ((res = Tcl_GetStringFromObj(objv[1], NULL)) == NULL)
    {
        PDF_NoPDFHandle(interp, "PDF_add_thumbnail");
        return TCL_ERROR;
    }

    if (SWIG_GetPtr(res, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle(interp, "PDF_add_thumbnail");
        Tcl_AppendResult(interp, res, (char *) NULL);
        return TCL_ERROR;
    }

    if (Tcl_GetIntFromObj(interp, objv[2], &image) != TCL_OK)
    {
        PDF_WrongParameter(interp, "image in PDF_add_thumbnail");
        return TCL_ERROR;
    }

    try { PDF_add_thumbnail(p, image);
    } catch;

    return TCL_OK;
}


static int
_wrap_PDF_add_weblink(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
    PDF *p;
    double llx;
    double lly;
    double urx;
    double ury;
    const char *url;
    int url_len;
    char *res;

    if (objc != 7)
    {
        PDF_WrongCommand(interp, "PDF_add_weblink p llx lly urx ury url ");
        return TCL_ERROR;
    }

    if ((res = Tcl_GetStringFromObj(objv[1], NULL)) == NULL)
    {
        PDF_NoPDFHandle(interp, "PDF_add_weblink");
        return TCL_ERROR;
    }

    if (SWIG_GetPtr(res, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle(interp, "PDF_add_weblink");
        Tcl_AppendResult(interp, res, (char *) NULL);
        return TCL_ERROR;
    }

    if (Tcl_GetDoubleFromObj(interp, objv[2], &llx) != TCL_OK)
    {
        PDF_WrongParameter(interp, "llx in PDF_add_weblink");
        return TCL_ERROR;
    }

    if (Tcl_GetDoubleFromObj(interp, objv[3], &lly) != TCL_OK)
    {
        PDF_WrongParameter(interp, "lly in PDF_add_weblink");
        return TCL_ERROR;
    }

    if (Tcl_GetDoubleFromObj(interp, objv[4], &urx) != TCL_OK)
    {
        PDF_WrongParameter(interp, "urx in PDF_add_weblink");
        return TCL_ERROR;
    }

    if (Tcl_GetDoubleFromObj(interp, objv[5], &ury) != TCL_OK)
    {
        PDF_WrongParameter(interp, "ury in PDF_add_weblink");
        return TCL_ERROR;
    }

    if ((url = PDF_GetStringFromObj(p, interp, objv[6], PDF_BYTES, &url_len)) == NULL)
    {
        PDF_WrongParameter(interp, "url in PDF_add_weblink");
        return TCL_ERROR;
    }

    try { PDF_add_weblink(p, llx, lly, urx, ury, url);
    } catch;

    return TCL_OK;
}


static int
_wrap_PDF_arc(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
    PDF *p;
    double x;
    double y;
    double r;
    double alpha;
    double beta;
    char *res;

    if (objc != 7)
    {
        PDF_WrongCommand(interp, "PDF_arc p x y r alpha beta ");
        return TCL_ERROR;
    }

    if ((res = Tcl_GetStringFromObj(objv[1], NULL)) == NULL)
    {
        PDF_NoPDFHandle(interp, "PDF_arc");
        return TCL_ERROR;
    }

    if (SWIG_GetPtr(res, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle(interp, "PDF_arc");
        Tcl_AppendResult(interp, res, (char *) NULL);
        return TCL_ERROR;
    }

    if (Tcl_GetDoubleFromObj(interp, objv[2], &x) != TCL_OK)
    {
        PDF_WrongParameter(interp, "x in PDF_arc");
        return TCL_ERROR;
    }

    if (Tcl_GetDoubleFromObj(interp, objv[3], &y) != TCL_OK)
    {
        PDF_WrongParameter(interp, "y in PDF_arc");
        return TCL_ERROR;
    }

    if (Tcl_GetDoubleFromObj(interp, objv[4], &r) != TCL_OK)
    {
        PDF_WrongParameter(interp, "r in PDF_arc");
        return TCL_ERROR;
    }

    if (Tcl_GetDoubleFromObj(interp, objv[5], &alpha) != TCL_OK)
    {
        PDF_WrongParameter(interp, "alpha in PDF_arc");
        return TCL_ERROR;
    }

    if (Tcl_GetDoubleFromObj(interp, objv[6], &beta) != TCL_OK)
    {
        PDF_WrongParameter(interp, "beta in PDF_arc");
        return TCL_ERROR;
    }

    try { PDF_arc(p, x, y, r, alpha, beta);
    } catch;

    return TCL_OK;
}


static int
_wrap_PDF_arcn(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
    PDF *p;
    double x;
    double y;
    double r;
    double alpha;
    double beta;
    char *res;

    if (objc != 7)
    {
        PDF_WrongCommand(interp, "PDF_arcn p x y r alpha beta ");
        return TCL_ERROR;
    }

    if ((res = Tcl_GetStringFromObj(objv[1], NULL)) == NULL)
    {
        PDF_NoPDFHandle(interp, "PDF_arcn");
        return TCL_ERROR;
    }

    if (SWIG_GetPtr(res, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle(interp, "PDF_arcn");
        Tcl_AppendResult(interp, res, (char *) NULL);
        return TCL_ERROR;
    }

    if (Tcl_GetDoubleFromObj(interp, objv[2], &x) != TCL_OK)
    {
        PDF_WrongParameter(interp, "x in PDF_arcn");
        return TCL_ERROR;
    }

    if (Tcl_GetDoubleFromObj(interp, objv[3], &y) != TCL_OK)
    {
        PDF_WrongParameter(interp, "y in PDF_arcn");
        return TCL_ERROR;
    }

    if (Tcl_GetDoubleFromObj(interp, objv[4], &r) != TCL_OK)
    {
        PDF_WrongParameter(interp, "r in PDF_arcn");
        return TCL_ERROR;
    }

    if (Tcl_GetDoubleFromObj(interp, objv[5], &alpha) != TCL_OK)
    {
        PDF_WrongParameter(interp, "alpha in PDF_arcn");
        return TCL_ERROR;
    }

    if (Tcl_GetDoubleFromObj(interp, objv[6], &beta) != TCL_OK)
    {
        PDF_WrongParameter(interp, "beta in PDF_arcn");
        return TCL_ERROR;
    }

    try { PDF_arcn(p, x, y, r, alpha, beta);
    } catch;

    return TCL_OK;
}


static int
_wrap_PDF_attach_file(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
    PDF *p;
    double llx;
    double lly;
    double urx;
    double ury;
    const char *filename;
    int filename_len;
    const char *description;
    int description_len;
    const char *author;
    int author_len;
    const char *mimetype;
    int mimetype_len;
    const char *icon;
    int icon_len;
    char *res;

    if (objc != 11)
    {
        PDF_WrongCommand(interp, "PDF_attach_file p llx lly urx ury filename description author mimetype icon ");
        return TCL_ERROR;
    }

    if ((res = Tcl_GetStringFromObj(objv[1], NULL)) == NULL)
    {
        PDF_NoPDFHandle(interp, "PDF_attach_file");
        return TCL_ERROR;
    }

    if (SWIG_GetPtr(res, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle(interp, "PDF_attach_file");
        Tcl_AppendResult(interp, res, (char *) NULL);
        return TCL_ERROR;
    }

    if (Tcl_GetDoubleFromObj(interp, objv[2], &llx) != TCL_OK)
    {
        PDF_WrongParameter(interp, "llx in PDF_attach_file");
        return TCL_ERROR;
    }

    if (Tcl_GetDoubleFromObj(interp, objv[3], &lly) != TCL_OK)
    {
        PDF_WrongParameter(interp, "lly in PDF_attach_file");
        return TCL_ERROR;
    }

    if (Tcl_GetDoubleFromObj(interp, objv[4], &urx) != TCL_OK)
    {
        PDF_WrongParameter(interp, "urx in PDF_attach_file");
        return TCL_ERROR;
    }

    if (Tcl_GetDoubleFromObj(interp, objv[5], &ury) != TCL_OK)
    {
        PDF_WrongParameter(interp, "ury in PDF_attach_file");
        return TCL_ERROR;
    }

    if ((filename = PDF_GetStringFromObj(p, interp, objv[6], PDF_0UTF16, &filename_len)) == NULL)
    {
        PDF_WrongParameter(interp, "filename in PDF_attach_file");
        return TCL_ERROR;
    }

    if ((description = PDF_GetStringFromObj(p, interp, objv[7], PDF_UTF16, &description_len)) == NULL)
    {
        PDF_WrongParameter(interp, "description in PDF_attach_file");
        return TCL_ERROR;
    }

    if ((author = PDF_GetStringFromObj(p, interp, objv[8], PDF_UTF16, &author_len)) == NULL)
    {
        PDF_WrongParameter(interp, "author in PDF_attach_file");
        return TCL_ERROR;
    }

    if ((mimetype = PDF_GetStringFromObj(p, interp, objv[9], PDF_BYTES, &mimetype_len)) == NULL)
    {
        PDF_WrongParameter(interp, "mimetype in PDF_attach_file");
        return TCL_ERROR;
    }

    if ((icon = PDF_GetStringFromObj(p, interp, objv[10], PDF_BYTES, &icon_len)) == NULL)
    {
        PDF_WrongParameter(interp, "icon in PDF_attach_file");
        return TCL_ERROR;
    }

    try { PDF_attach_file2(p, llx, lly, urx, ury, filename, filename_len, description, description_len, author, author_len, mimetype, icon);
    } catch;

    return TCL_OK;
}


static int
_wrap_PDF_begin_document(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
    PDF *p;
    const char *filename;
    int filename_len;
    const char *optlist;
    int optlist_len;
    char *res;
    int volatile _result = -1;

    if (objc != 4)
    {
        PDF_WrongCommand(interp, "PDF_begin_document p filename optlist ");
        return TCL_ERROR;
    }

    if ((res = Tcl_GetStringFromObj(objv[1], NULL)) == NULL)
    {
        PDF_NoPDFHandle(interp, "PDF_begin_document");
        return TCL_ERROR;
    }

    if (SWIG_GetPtr(res, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle(interp, "PDF_begin_document");
        Tcl_AppendResult(interp, res, (char *) NULL);
        return TCL_ERROR;
    }

    if ((filename = PDF_GetStringFromObj(p, interp, objv[2], PDF_0UTF16, &filename_len)) == NULL)
    {
        PDF_WrongParameter(interp, "filename in PDF_begin_document");
        return TCL_ERROR;
    }

    if ((optlist = PDF_GetStringFromObj(p, interp, objv[3], PDF_UTF8, &optlist_len)) == NULL)
    {
        PDF_WrongParameter(interp, "optlist in PDF_begin_document");
        return TCL_ERROR;
    }

    try { _result = (int) PDF_begin_document(p, filename, filename_len, optlist);
    } catch;

    sprintf(interp->result, "%d", _result);
    return TCL_OK;
}


static int
_wrap_PDF_begin_font(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
    PDF *p;
    const char *fontname;
    int fontname_len;
    double a;
    double b;
    double c;
    double d;
    double e;
    double f;
    const char *optlist;
    int optlist_len;
    char *res;

    if (objc != 10)
    {
        PDF_WrongCommand(interp, "PDF_begin_font p fontname a b c d e f optlist ");
        return TCL_ERROR;
    }

    if ((res = Tcl_GetStringFromObj(objv[1], NULL)) == NULL)
    {
        PDF_NoPDFHandle(interp, "PDF_begin_font");
        return TCL_ERROR;
    }

    if (SWIG_GetPtr(res, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle(interp, "PDF_begin_font");
        Tcl_AppendResult(interp, res, (char *) NULL);
        return TCL_ERROR;
    }

    if ((fontname = PDF_GetStringFromObj(p, interp, objv[2], PDF_0UTF16, &fontname_len)) == NULL)
    {
        PDF_WrongParameter(interp, "fontname in PDF_begin_font");
        return TCL_ERROR;
    }

    if (Tcl_GetDoubleFromObj(interp, objv[3], &a) != TCL_OK)
    {
        PDF_WrongParameter(interp, "a in PDF_begin_font");
        return TCL_ERROR;
    }

    if (Tcl_GetDoubleFromObj(interp, objv[4], &b) != TCL_OK)
    {
        PDF_WrongParameter(interp, "b in PDF_begin_font");
        return TCL_ERROR;
    }

    if (Tcl_GetDoubleFromObj(interp, objv[5], &c) != TCL_OK)
    {
        PDF_WrongParameter(interp, "c in PDF_begin_font");
        return TCL_ERROR;
    }

    if (Tcl_GetDoubleFromObj(interp, objv[6], &d) != TCL_OK)
    {
        PDF_WrongParameter(interp, "d in PDF_begin_font");
        return TCL_ERROR;
    }

    if (Tcl_GetDoubleFromObj(interp, objv[7], &e) != TCL_OK)
    {
        PDF_WrongParameter(interp, "e in PDF_begin_font");
        return TCL_ERROR;
    }

    if (Tcl_GetDoubleFromObj(interp, objv[8], &f) != TCL_OK)
    {
        PDF_WrongParameter(interp, "f in PDF_begin_font");
        return TCL_ERROR;
    }

    if ((optlist = PDF_GetStringFromObj(p, interp, objv[9], PDF_UTF8, &optlist_len)) == NULL)
    {
        PDF_WrongParameter(interp, "optlist in PDF_begin_font");
        return TCL_ERROR;
    }

    try { PDF_begin_font(p, fontname, fontname_len, a, b, c, d, e, f, optlist);
    } catch;

    return TCL_OK;
}


static int
_wrap_PDF_begin_glyph(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
    PDF *p;
    const char *glyphname;
    int glyphname_len;
    double wx;
    double llx;
    double lly;
    double urx;
    double ury;
    char *res;

    if (objc != 8)
    {
        PDF_WrongCommand(interp, "PDF_begin_glyph p glyphname wx llx lly urx ury ");
        return TCL_ERROR;
    }

    if ((res = Tcl_GetStringFromObj(objv[1], NULL)) == NULL)
    {
        PDF_NoPDFHandle(interp, "PDF_begin_glyph");
        return TCL_ERROR;
    }

    if (SWIG_GetPtr(res, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle(interp, "PDF_begin_glyph");
        Tcl_AppendResult(interp, res, (char *) NULL);
        return TCL_ERROR;
    }

    if ((glyphname = PDF_GetStringFromObj(p, interp, objv[2], PDF_BYTES, &glyphname_len)) == NULL)
    {
        PDF_WrongParameter(interp, "glyphname in PDF_begin_glyph");
        return TCL_ERROR;
    }

    if (Tcl_GetDoubleFromObj(interp, objv[3], &wx) != TCL_OK)
    {
        PDF_WrongParameter(interp, "wx in PDF_begin_glyph");
        return TCL_ERROR;
    }

    if (Tcl_GetDoubleFromObj(interp, objv[4], &llx) != TCL_OK)
    {
        PDF_WrongParameter(interp, "llx in PDF_begin_glyph");
        return TCL_ERROR;
    }

    if (Tcl_GetDoubleFromObj(interp, objv[5], &lly) != TCL_OK)
    {
        PDF_WrongParameter(interp, "lly in PDF_begin_glyph");
        return TCL_ERROR;
    }

    if (Tcl_GetDoubleFromObj(interp, objv[6], &urx) != TCL_OK)
    {
        PDF_WrongParameter(interp, "urx in PDF_begin_glyph");
        return TCL_ERROR;
    }

    if (Tcl_GetDoubleFromObj(interp, objv[7], &ury) != TCL_OK)
    {
        PDF_WrongParameter(interp, "ury in PDF_begin_glyph");
        return TCL_ERROR;
    }

    try { PDF_begin_glyph(p, glyphname, wx, llx, lly, urx, ury);
    } catch;

    return TCL_OK;
}


static int
_wrap_PDF_begin_item(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
    PDF *p;
    const char *tag;
    int tag_len;
    const char *optlist;
    int optlist_len;
    char *res;
    int volatile _result = -1;

    if (objc != 4)
    {
        PDF_WrongCommand(interp, "PDF_begin_item p tag optlist ");
        return TCL_ERROR;
    }

    if ((res = Tcl_GetStringFromObj(objv[1], NULL)) == NULL)
    {
        PDF_NoPDFHandle(interp, "PDF_begin_item");
        return TCL_ERROR;
    }

    if (SWIG_GetPtr(res, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle(interp, "PDF_begin_item");
        Tcl_AppendResult(interp, res, (char *) NULL);
        return TCL_ERROR;
    }

    if ((tag = PDF_GetStringFromObj(p, interp, objv[2], PDF_BYTES, &tag_len)) == NULL)
    {
        PDF_WrongParameter(interp, "tag in PDF_begin_item");
        return TCL_ERROR;
    }

    if ((optlist = PDF_GetStringFromObj(p, interp, objv[3], PDF_UTF8, &optlist_len)) == NULL)
    {
        PDF_WrongParameter(interp, "optlist in PDF_begin_item");
        return TCL_ERROR;
    }

    try { _result = (int) PDF_begin_item(p, tag, optlist);
    } catch;

    sprintf(interp->result, "%d", _result);
    return TCL_OK;
}


static int
_wrap_PDF_begin_layer(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
    PDF *p;
    int layer;
    char *res;

    if (objc != 3)
    {
        PDF_WrongCommand(interp, "PDF_begin_layer p layer ");
        return TCL_ERROR;
    }

    if ((res = Tcl_GetStringFromObj(objv[1], NULL)) == NULL)
    {
        PDF_NoPDFHandle(interp, "PDF_begin_layer");
        return TCL_ERROR;
    }

    if (SWIG_GetPtr(res, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle(interp, "PDF_begin_layer");
        Tcl_AppendResult(interp, res, (char *) NULL);
        return TCL_ERROR;
    }

    if (Tcl_GetIntFromObj(interp, objv[2], &layer) != TCL_OK)
    {
        PDF_WrongParameter(interp, "layer in PDF_begin_layer");
        return TCL_ERROR;
    }

    try { PDF_begin_layer(p, layer);
    } catch;

    return TCL_OK;
}


static int
_wrap_PDF_begin_mc(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
    PDF *p;
    const char *tag;
    int tag_len;
    const char *optlist;
    int optlist_len;
    char *res;

    if (objc != 4)
    {
        PDF_WrongCommand(interp, "PDF_begin_mc p tag optlist ");
        return TCL_ERROR;
    }

    if ((res = Tcl_GetStringFromObj(objv[1], NULL)) == NULL)
    {
        PDF_NoPDFHandle(interp, "PDF_begin_mc");
        return TCL_ERROR;
    }

    if (SWIG_GetPtr(res, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle(interp, "PDF_begin_mc");
        Tcl_AppendResult(interp, res, (char *) NULL);
        return TCL_ERROR;
    }

    if ((tag = PDF_GetStringFromObj(p, interp, objv[2], PDF_BYTES, &tag_len)) == NULL)
    {
        PDF_WrongParameter(interp, "tag in PDF_begin_mc");
        return TCL_ERROR;
    }

    if ((optlist = PDF_GetStringFromObj(p, interp, objv[3], PDF_UTF8, &optlist_len)) == NULL)
    {
        PDF_WrongParameter(interp, "optlist in PDF_begin_mc");
        return TCL_ERROR;
    }

    try { PDF_begin_mc(p, tag, optlist);
    } catch;

    return TCL_OK;
}


static int
_wrap_PDF_begin_page(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
    PDF *p;
    double width;
    double height;
    char *res;

    if (objc != 4)
    {
        PDF_WrongCommand(interp, "PDF_begin_page p width height ");
        return TCL_ERROR;
    }

    if ((res = Tcl_GetStringFromObj(objv[1], NULL)) == NULL)
    {
        PDF_NoPDFHandle(interp, "PDF_begin_page");
        return TCL_ERROR;
    }

    if (SWIG_GetPtr(res, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle(interp, "PDF_begin_page");
        Tcl_AppendResult(interp, res, (char *) NULL);
        return TCL_ERROR;
    }

    if (Tcl_GetDoubleFromObj(interp, objv[2], &width) != TCL_OK)
    {
        PDF_WrongParameter(interp, "width in PDF_begin_page");
        return TCL_ERROR;
    }

    if (Tcl_GetDoubleFromObj(interp, objv[3], &height) != TCL_OK)
    {
        PDF_WrongParameter(interp, "height in PDF_begin_page");
        return TCL_ERROR;
    }

    try { PDF_begin_page(p, width, height);
    } catch;

    return TCL_OK;
}


static int
_wrap_PDF_begin_page_ext(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
    PDF *p;
    double width;
    double height;
    const char *optlist;
    int optlist_len;
    char *res;

    if (objc != 5)
    {
        PDF_WrongCommand(interp, "PDF_begin_page_ext p width height optlist ");
        return TCL_ERROR;
    }

    if ((res = Tcl_GetStringFromObj(objv[1], NULL)) == NULL)
    {
        PDF_NoPDFHandle(interp, "PDF_begin_page_ext");
        return TCL_ERROR;
    }

    if (SWIG_GetPtr(res, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle(interp, "PDF_begin_page_ext");
        Tcl_AppendResult(interp, res, (char *) NULL);
        return TCL_ERROR;
    }

    if (Tcl_GetDoubleFromObj(interp, objv[2], &width) != TCL_OK)
    {
        PDF_WrongParameter(interp, "width in PDF_begin_page_ext");
        return TCL_ERROR;
    }

    if (Tcl_GetDoubleFromObj(interp, objv[3], &height) != TCL_OK)
    {
        PDF_WrongParameter(interp, "height in PDF_begin_page_ext");
        return TCL_ERROR;
    }

    if ((optlist = PDF_GetStringFromObj(p, interp, objv[4], PDF_UTF8, &optlist_len)) == NULL)
    {
        PDF_WrongParameter(interp, "optlist in PDF_begin_page_ext");
        return TCL_ERROR;
    }

    try { PDF_begin_page_ext(p, width, height, optlist);
    } catch;

    return TCL_OK;
}


static int
_wrap_PDF_begin_pattern(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
    PDF *p;
    double width;
    double height;
    double xstep;
    double ystep;
    int painttype;
    char *res;
    int volatile _result = -1;

    if (objc != 7)
    {
        PDF_WrongCommand(interp, "PDF_begin_pattern p width height xstep ystep painttype ");
        return TCL_ERROR;
    }

    if ((res = Tcl_GetStringFromObj(objv[1], NULL)) == NULL)
    {
        PDF_NoPDFHandle(interp, "PDF_begin_pattern");
        return TCL_ERROR;
    }

    if (SWIG_GetPtr(res, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle(interp, "PDF_begin_pattern");
        Tcl_AppendResult(interp, res, (char *) NULL);
        return TCL_ERROR;
    }

    if (Tcl_GetDoubleFromObj(interp, objv[2], &width) != TCL_OK)
    {
        PDF_WrongParameter(interp, "width in PDF_begin_pattern");
        return TCL_ERROR;
    }

    if (Tcl_GetDoubleFromObj(interp, objv[3], &height) != TCL_OK)
    {
        PDF_WrongParameter(interp, "height in PDF_begin_pattern");
        return TCL_ERROR;
    }

    if (Tcl_GetDoubleFromObj(interp, objv[4], &xstep) != TCL_OK)
    {
        PDF_WrongParameter(interp, "xstep in PDF_begin_pattern");
        return TCL_ERROR;
    }

    if (Tcl_GetDoubleFromObj(interp, objv[5], &ystep) != TCL_OK)
    {
        PDF_WrongParameter(interp, "ystep in PDF_begin_pattern");
        return TCL_ERROR;
    }

    if (Tcl_GetIntFromObj(interp, objv[6], &painttype) != TCL_OK)
    {
        PDF_WrongParameter(interp, "painttype in PDF_begin_pattern");
        return TCL_ERROR;
    }

    try { _result = (int) PDF_begin_pattern(p, width, height, xstep, ystep, painttype);
    } catch;

    sprintf(interp->result, "%d", _result);
    return TCL_OK;
}


static int
_wrap_PDF_begin_template(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
    PDF *p;
    double width;
    double height;
    char *res;
    int volatile _result = -1;

    if (objc != 4)
    {
        PDF_WrongCommand(interp, "PDF_begin_template p width height ");
        return TCL_ERROR;
    }

    if ((res = Tcl_GetStringFromObj(objv[1], NULL)) == NULL)
    {
        PDF_NoPDFHandle(interp, "PDF_begin_template");
        return TCL_ERROR;
    }

    if (SWIG_GetPtr(res, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle(interp, "PDF_begin_template");
        Tcl_AppendResult(interp, res, (char *) NULL);
        return TCL_ERROR;
    }

    if (Tcl_GetDoubleFromObj(interp, objv[2], &width) != TCL_OK)
    {
        PDF_WrongParameter(interp, "width in PDF_begin_template");
        return TCL_ERROR;
    }

    if (Tcl_GetDoubleFromObj(interp, objv[3], &height) != TCL_OK)
    {
        PDF_WrongParameter(interp, "height in PDF_begin_template");
        return TCL_ERROR;
    }

    try { _result = (int) PDF_begin_template(p, width, height);
    } catch;

    sprintf(interp->result, "%d", _result);
    return TCL_OK;
}


static int
_wrap_PDF_circle(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
    PDF *p;
    double x;
    double y;
    double r;
    char *res;

    if (objc != 5)
    {
        PDF_WrongCommand(interp, "PDF_circle p x y r ");
        return TCL_ERROR;
    }

    if ((res = Tcl_GetStringFromObj(objv[1], NULL)) == NULL)
    {
        PDF_NoPDFHandle(interp, "PDF_circle");
        return TCL_ERROR;
    }

    if (SWIG_GetPtr(res, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle(interp, "PDF_circle");
        Tcl_AppendResult(interp, res, (char *) NULL);
        return TCL_ERROR;
    }

    if (Tcl_GetDoubleFromObj(interp, objv[2], &x) != TCL_OK)
    {
        PDF_WrongParameter(interp, "x in PDF_circle");
        return TCL_ERROR;
    }

    if (Tcl_GetDoubleFromObj(interp, objv[3], &y) != TCL_OK)
    {
        PDF_WrongParameter(interp, "y in PDF_circle");
        return TCL_ERROR;
    }

    if (Tcl_GetDoubleFromObj(interp, objv[4], &r) != TCL_OK)
    {
        PDF_WrongParameter(interp, "r in PDF_circle");
        return TCL_ERROR;
    }

    try { PDF_circle(p, x, y, r);
    } catch;

    return TCL_OK;
}


static int
_wrap_PDF_clip(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
    PDF *p;
    char *res;

    if (objc != 2)
    {
        PDF_WrongCommand(interp, "PDF_clip p ");
        return TCL_ERROR;
    }

    if ((res = Tcl_GetStringFromObj(objv[1], NULL)) == NULL)
    {
        PDF_NoPDFHandle(interp, "PDF_clip");
        return TCL_ERROR;
    }

    if (SWIG_GetPtr(res, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle(interp, "PDF_clip");
        Tcl_AppendResult(interp, res, (char *) NULL);
        return TCL_ERROR;
    }

    try { PDF_clip(p);
    } catch;

    return TCL_OK;
}


static int
_wrap_PDF_close(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
    PDF *p;
    char *res;

    if (objc != 2)
    {
        PDF_WrongCommand(interp, "PDF_close p ");
        return TCL_ERROR;
    }

    if ((res = Tcl_GetStringFromObj(objv[1], NULL)) == NULL)
    {
        PDF_NoPDFHandle(interp, "PDF_close");
        return TCL_ERROR;
    }

    if (SWIG_GetPtr(res, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle(interp, "PDF_close");
        Tcl_AppendResult(interp, res, (char *) NULL);
        return TCL_ERROR;
    }

    try { PDF_close(p);
    } catch;

    return TCL_OK;
}


static int
_wrap_PDF_close_image(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
    PDF *p;
    int image;
    char *res;

    if (objc != 3)
    {
        PDF_WrongCommand(interp, "PDF_close_image p image ");
        return TCL_ERROR;
    }

    if ((res = Tcl_GetStringFromObj(objv[1], NULL)) == NULL)
    {
        PDF_NoPDFHandle(interp, "PDF_close_image");
        return TCL_ERROR;
    }

    if (SWIG_GetPtr(res, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle(interp, "PDF_close_image");
        Tcl_AppendResult(interp, res, (char *) NULL);
        return TCL_ERROR;
    }

    if (Tcl_GetIntFromObj(interp, objv[2], &image) != TCL_OK)
    {
        PDF_WrongParameter(interp, "image in PDF_close_image");
        return TCL_ERROR;
    }

    try { PDF_close_image(p, image);
    } catch;

    return TCL_OK;
}


static int
_wrap_PDF_close_pdi(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
    PDF *p;
    int doc;
    char *res;

    if (objc != 3)
    {
        PDF_WrongCommand(interp, "PDF_close_pdi p doc ");
        return TCL_ERROR;
    }

    if ((res = Tcl_GetStringFromObj(objv[1], NULL)) == NULL)
    {
        PDF_NoPDFHandle(interp, "PDF_close_pdi");
        return TCL_ERROR;
    }

    if (SWIG_GetPtr(res, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle(interp, "PDF_close_pdi");
        Tcl_AppendResult(interp, res, (char *) NULL);
        return TCL_ERROR;
    }

    if (Tcl_GetIntFromObj(interp, objv[2], &doc) != TCL_OK)
    {
        PDF_WrongParameter(interp, "doc in PDF_close_pdi");
        return TCL_ERROR;
    }

    try { PDF_close_pdi(p, doc);
    } catch;

    return TCL_OK;
}


static int
_wrap_PDF_close_pdi_page(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
    PDF *p;
    int page;
    char *res;

    if (objc != 3)
    {
        PDF_WrongCommand(interp, "PDF_close_pdi_page p page ");
        return TCL_ERROR;
    }

    if ((res = Tcl_GetStringFromObj(objv[1], NULL)) == NULL)
    {
        PDF_NoPDFHandle(interp, "PDF_close_pdi_page");
        return TCL_ERROR;
    }

    if (SWIG_GetPtr(res, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle(interp, "PDF_close_pdi_page");
        Tcl_AppendResult(interp, res, (char *) NULL);
        return TCL_ERROR;
    }

    if (Tcl_GetIntFromObj(interp, objv[2], &page) != TCL_OK)
    {
        PDF_WrongParameter(interp, "page in PDF_close_pdi_page");
        return TCL_ERROR;
    }

    try { PDF_close_pdi_page(p, page);
    } catch;

    return TCL_OK;
}


static int
_wrap_PDF_closepath(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
    PDF *p;
    char *res;

    if (objc != 2)
    {
        PDF_WrongCommand(interp, "PDF_closepath p ");
        return TCL_ERROR;
    }

    if ((res = Tcl_GetStringFromObj(objv[1], NULL)) == NULL)
    {
        PDF_NoPDFHandle(interp, "PDF_closepath");
        return TCL_ERROR;
    }

    if (SWIG_GetPtr(res, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle(interp, "PDF_closepath");
        Tcl_AppendResult(interp, res, (char *) NULL);
        return TCL_ERROR;
    }

    try { PDF_closepath(p);
    } catch;

    return TCL_OK;
}


static int
_wrap_PDF_closepath_fill_stroke(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
    PDF *p;
    char *res;

    if (objc != 2)
    {
        PDF_WrongCommand(interp, "PDF_closepath_fill_stroke p ");
        return TCL_ERROR;
    }

    if ((res = Tcl_GetStringFromObj(objv[1], NULL)) == NULL)
    {
        PDF_NoPDFHandle(interp, "PDF_closepath_fill_stroke");
        return TCL_ERROR;
    }

    if (SWIG_GetPtr(res, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle(interp, "PDF_closepath_fill_stroke");
        Tcl_AppendResult(interp, res, (char *) NULL);
        return TCL_ERROR;
    }

    try { PDF_closepath_fill_stroke(p);
    } catch;

    return TCL_OK;
}


static int
_wrap_PDF_closepath_stroke(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
    PDF *p;
    char *res;

    if (objc != 2)
    {
        PDF_WrongCommand(interp, "PDF_closepath_stroke p ");
        return TCL_ERROR;
    }

    if ((res = Tcl_GetStringFromObj(objv[1], NULL)) == NULL)
    {
        PDF_NoPDFHandle(interp, "PDF_closepath_stroke");
        return TCL_ERROR;
    }

    if (SWIG_GetPtr(res, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle(interp, "PDF_closepath_stroke");
        Tcl_AppendResult(interp, res, (char *) NULL);
        return TCL_ERROR;
    }

    try { PDF_closepath_stroke(p);
    } catch;

    return TCL_OK;
}


static int
_wrap_PDF_concat(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
    PDF *p;
    double a;
    double b;
    double c;
    double d;
    double e;
    double f;
    char *res;

    if (objc != 8)
    {
        PDF_WrongCommand(interp, "PDF_concat p a b c d e f ");
        return TCL_ERROR;
    }

    if ((res = Tcl_GetStringFromObj(objv[1], NULL)) == NULL)
    {
        PDF_NoPDFHandle(interp, "PDF_concat");
        return TCL_ERROR;
    }

    if (SWIG_GetPtr(res, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle(interp, "PDF_concat");
        Tcl_AppendResult(interp, res, (char *) NULL);
        return TCL_ERROR;
    }

    if (Tcl_GetDoubleFromObj(interp, objv[2], &a) != TCL_OK)
    {
        PDF_WrongParameter(interp, "a in PDF_concat");
        return TCL_ERROR;
    }

    if (Tcl_GetDoubleFromObj(interp, objv[3], &b) != TCL_OK)
    {
        PDF_WrongParameter(interp, "b in PDF_concat");
        return TCL_ERROR;
    }

    if (Tcl_GetDoubleFromObj(interp, objv[4], &c) != TCL_OK)
    {
        PDF_WrongParameter(interp, "c in PDF_concat");
        return TCL_ERROR;
    }

    if (Tcl_GetDoubleFromObj(interp, objv[5], &d) != TCL_OK)
    {
        PDF_WrongParameter(interp, "d in PDF_concat");
        return TCL_ERROR;
    }

    if (Tcl_GetDoubleFromObj(interp, objv[6], &e) != TCL_OK)
    {
        PDF_WrongParameter(interp, "e in PDF_concat");
        return TCL_ERROR;
    }

    if (Tcl_GetDoubleFromObj(interp, objv[7], &f) != TCL_OK)
    {
        PDF_WrongParameter(interp, "f in PDF_concat");
        return TCL_ERROR;
    }

    try { PDF_concat(p, a, b, c, d, e, f);
    } catch;

    return TCL_OK;
}


static int
_wrap_PDF_continue_text(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
    PDF *p;
    const char *text;
    int text_len;
    char *res;

    if (objc != 3)
    {
        PDF_WrongCommand(interp, "PDF_continue_text p text ");
        return TCL_ERROR;
    }

    if ((res = Tcl_GetStringFromObj(objv[1], NULL)) == NULL)
    {
        PDF_NoPDFHandle(interp, "PDF_continue_text");
        return TCL_ERROR;
    }

    if (SWIG_GetPtr(res, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle(interp, "PDF_continue_text");
        Tcl_AppendResult(interp, res, (char *) NULL);
        return TCL_ERROR;
    }

    if ((text = PDF_GetStringFromObj(p, interp, objv[2], PDF_UTF16, &text_len)) == NULL)
    {
        PDF_WrongParameter(interp, "text in PDF_continue_text");
        return TCL_ERROR;
    }

    try { PDF_continue_text2(p, text, text_len);
    } catch;

    return TCL_OK;
}


static int
_wrap_PDF_create_action(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
    PDF *p;
    const char *type;
    int type_len;
    const char *optlist;
    int optlist_len;
    char *res;
    int volatile _result = -1;

    if (objc != 4)
    {
        PDF_WrongCommand(interp, "PDF_create_action p type optlist ");
        return TCL_ERROR;
    }

    if ((res = Tcl_GetStringFromObj(objv[1], NULL)) == NULL)
    {
        PDF_NoPDFHandle(interp, "PDF_create_action");
        return TCL_ERROR;
    }

    if (SWIG_GetPtr(res, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle(interp, "PDF_create_action");
        Tcl_AppendResult(interp, res, (char *) NULL);
        return TCL_ERROR;
    }

    if ((type = PDF_GetStringFromObj(p, interp, objv[2], PDF_BYTES, &type_len)) == NULL)
    {
        PDF_WrongParameter(interp, "type in PDF_create_action");
        return TCL_ERROR;
    }

    if ((optlist = PDF_GetStringFromObj(p, interp, objv[3], PDF_UTF8, &optlist_len)) == NULL)
    {
        PDF_WrongParameter(interp, "optlist in PDF_create_action");
        return TCL_ERROR;
    }

    try { _result = (int) PDF_create_action(p, type, optlist);
    } catch;

    sprintf(interp->result, "%d", _result);
    return TCL_OK;
}


static int
_wrap_PDF_create_annotation(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
    PDF *p;
    double llx;
    double lly;
    double urx;
    double ury;
    const char *type;
    int type_len;
    const char *optlist;
    int optlist_len;
    char *res;

    if (objc != 8)
    {
        PDF_WrongCommand(interp, "PDF_create_annotation p llx lly urx ury type optlist ");
        return TCL_ERROR;
    }

    if ((res = Tcl_GetStringFromObj(objv[1], NULL)) == NULL)
    {
        PDF_NoPDFHandle(interp, "PDF_create_annotation");
        return TCL_ERROR;
    }

    if (SWIG_GetPtr(res, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle(interp, "PDF_create_annotation");
        Tcl_AppendResult(interp, res, (char *) NULL);
        return TCL_ERROR;
    }

    if (Tcl_GetDoubleFromObj(interp, objv[2], &llx) != TCL_OK)
    {
        PDF_WrongParameter(interp, "llx in PDF_create_annotation");
        return TCL_ERROR;
    }

    if (Tcl_GetDoubleFromObj(interp, objv[3], &lly) != TCL_OK)
    {
        PDF_WrongParameter(interp, "lly in PDF_create_annotation");
        return TCL_ERROR;
    }

    if (Tcl_GetDoubleFromObj(interp, objv[4], &urx) != TCL_OK)
    {
        PDF_WrongParameter(interp, "urx in PDF_create_annotation");
        return TCL_ERROR;
    }

    if (Tcl_GetDoubleFromObj(interp, objv[5], &ury) != TCL_OK)
    {
        PDF_WrongParameter(interp, "ury in PDF_create_annotation");
        return TCL_ERROR;
    }

    if ((type = PDF_GetStringFromObj(p, interp, objv[6], PDF_BYTES, &type_len)) == NULL)
    {
        PDF_WrongParameter(interp, "type in PDF_create_annotation");
        return TCL_ERROR;
    }

    if ((optlist = PDF_GetStringFromObj(p, interp, objv[7], PDF_UTF8, &optlist_len)) == NULL)
    {
        PDF_WrongParameter(interp, "optlist in PDF_create_annotation");
        return TCL_ERROR;
    }

    try { PDF_create_annotation(p, llx, lly, urx, ury, type, optlist);
    } catch;

    return TCL_OK;
}


static int
_wrap_PDF_create_bookmark(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
    PDF *p;
    const char *text;
    int text_len;
    const char *optlist;
    int optlist_len;
    char *res;
    int volatile _result = -1;

    if (objc != 4)
    {
        PDF_WrongCommand(interp, "PDF_create_bookmark p text optlist ");
        return TCL_ERROR;
    }

    if ((res = Tcl_GetStringFromObj(objv[1], NULL)) == NULL)
    {
        PDF_NoPDFHandle(interp, "PDF_create_bookmark");
        return TCL_ERROR;
    }

    if (SWIG_GetPtr(res, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle(interp, "PDF_create_bookmark");
        Tcl_AppendResult(interp, res, (char *) NULL);
        return TCL_ERROR;
    }

    if ((text = PDF_GetStringFromObj(p, interp, objv[2], PDF_UTF16, &text_len)) == NULL)
    {
        PDF_WrongParameter(interp, "text in PDF_create_bookmark");
        return TCL_ERROR;
    }

    if ((optlist = PDF_GetStringFromObj(p, interp, objv[3], PDF_UTF8, &optlist_len)) == NULL)
    {
        PDF_WrongParameter(interp, "optlist in PDF_create_bookmark");
        return TCL_ERROR;
    }

    try { _result = (int) PDF_create_bookmark(p, text, text_len, optlist);
    } catch;

    sprintf(interp->result, "%d", _result);
    return TCL_OK;
}


static int
_wrap_PDF_create_field(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
    PDF *p;
    double llx;
    double lly;
    double urx;
    double ury;
    const char *name;
    int name_len;
    const char *type;
    int type_len;
    const char *optlist;
    int optlist_len;
    char *res;

    if (objc != 9)
    {
        PDF_WrongCommand(interp, "PDF_create_field p llx lly urx ury name type optlist ");
        return TCL_ERROR;
    }

    if ((res = Tcl_GetStringFromObj(objv[1], NULL)) == NULL)
    {
        PDF_NoPDFHandle(interp, "PDF_create_field");
        return TCL_ERROR;
    }

    if (SWIG_GetPtr(res, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle(interp, "PDF_create_field");
        Tcl_AppendResult(interp, res, (char *) NULL);
        return TCL_ERROR;
    }

    if (Tcl_GetDoubleFromObj(interp, objv[2], &llx) != TCL_OK)
    {
        PDF_WrongParameter(interp, "llx in PDF_create_field");
        return TCL_ERROR;
    }

    if (Tcl_GetDoubleFromObj(interp, objv[3], &lly) != TCL_OK)
    {
        PDF_WrongParameter(interp, "lly in PDF_create_field");
        return TCL_ERROR;
    }

    if (Tcl_GetDoubleFromObj(interp, objv[4], &urx) != TCL_OK)
    {
        PDF_WrongParameter(interp, "urx in PDF_create_field");
        return TCL_ERROR;
    }

    if (Tcl_GetDoubleFromObj(interp, objv[5], &ury) != TCL_OK)
    {
        PDF_WrongParameter(interp, "ury in PDF_create_field");
        return TCL_ERROR;
    }

    if ((name = PDF_GetStringFromObj(p, interp, objv[6], PDF_UTF16, &name_len)) == NULL)
    {
        PDF_WrongParameter(interp, "name in PDF_create_field");
        return TCL_ERROR;
    }

    if ((type = PDF_GetStringFromObj(p, interp, objv[7], PDF_BYTES, &type_len)) == NULL)
    {
        PDF_WrongParameter(interp, "type in PDF_create_field");
        return TCL_ERROR;
    }

    if ((optlist = PDF_GetStringFromObj(p, interp, objv[8], PDF_UTF8, &optlist_len)) == NULL)
    {
        PDF_WrongParameter(interp, "optlist in PDF_create_field");
        return TCL_ERROR;
    }

    try { PDF_create_field(p, llx, lly, urx, ury, name, name_len, type, optlist);
    } catch;

    return TCL_OK;
}


static int
_wrap_PDF_create_fieldgroup(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
    PDF *p;
    const char *name;
    int name_len;
    const char *optlist;
    int optlist_len;
    char *res;

    if (objc != 4)
    {
        PDF_WrongCommand(interp, "PDF_create_fieldgroup p name optlist ");
        return TCL_ERROR;
    }

    if ((res = Tcl_GetStringFromObj(objv[1], NULL)) == NULL)
    {
        PDF_NoPDFHandle(interp, "PDF_create_fieldgroup");
        return TCL_ERROR;
    }

    if (SWIG_GetPtr(res, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle(interp, "PDF_create_fieldgroup");
        Tcl_AppendResult(interp, res, (char *) NULL);
        return TCL_ERROR;
    }

    if ((name = PDF_GetStringFromObj(p, interp, objv[2], PDF_UTF16, &name_len)) == NULL)
    {
        PDF_WrongParameter(interp, "name in PDF_create_fieldgroup");
        return TCL_ERROR;
    }

    if ((optlist = PDF_GetStringFromObj(p, interp, objv[3], PDF_UTF8, &optlist_len)) == NULL)
    {
        PDF_WrongParameter(interp, "optlist in PDF_create_fieldgroup");
        return TCL_ERROR;
    }

    try { PDF_create_fieldgroup(p, name, name_len, optlist);
    } catch;

    return TCL_OK;
}


static int
_wrap_PDF_create_gstate(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
    PDF *p;
    const char *optlist;
    int optlist_len;
    char *res;
    int volatile _result = -1;

    if (objc != 3)
    {
        PDF_WrongCommand(interp, "PDF_create_gstate p optlist ");
        return TCL_ERROR;
    }

    if ((res = Tcl_GetStringFromObj(objv[1], NULL)) == NULL)
    {
        PDF_NoPDFHandle(interp, "PDF_create_gstate");
        return TCL_ERROR;
    }

    if (SWIG_GetPtr(res, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle(interp, "PDF_create_gstate");
        Tcl_AppendResult(interp, res, (char *) NULL);
        return TCL_ERROR;
    }

    if ((optlist = PDF_GetStringFromObj(p, interp, objv[2], PDF_UTF8, &optlist_len)) == NULL)
    {
        PDF_WrongParameter(interp, "optlist in PDF_create_gstate");
        return TCL_ERROR;
    }

    try { _result = (int) PDF_create_gstate(p, optlist);
    } catch;

    sprintf(interp->result, "%d", _result);
    return TCL_OK;
}


static int
_wrap_PDF_create_pvf(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
    PDF *p;
    const char *filename;
    int filename_len;
    const void *data;
    int data_len;
    const char *optlist;
    int optlist_len;
    char *res;

    if (objc != 5)
    {
        PDF_WrongCommand(interp, "PDF_create_pvf p filename data optlist ");
        return TCL_ERROR;
    }

    if ((res = Tcl_GetStringFromObj(objv[1], NULL)) == NULL)
    {
        PDF_NoPDFHandle(interp, "PDF_create_pvf");
        return TCL_ERROR;
    }

    if (SWIG_GetPtr(res, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle(interp, "PDF_create_pvf");
        Tcl_AppendResult(interp, res, (char *) NULL);
        return TCL_ERROR;
    }

    if ((filename = PDF_GetStringFromObj(p, interp, objv[2], PDF_0UTF16, &filename_len)) == NULL)
    {
        PDF_WrongParameter(interp, "filename in PDF_create_pvf");
        return TCL_ERROR;
    }

    if ((data = PDF_GetStringFromObj(p, interp, objv[3], PDF_DATA, &data_len)) == NULL)
    {
        PDF_WrongParameter(interp, "data in PDF_create_pvf");
        return TCL_ERROR;
    }

    if ((optlist = PDF_GetStringFromObj(p, interp, objv[4], PDF_UTF8, &optlist_len)) == NULL)
    {
        PDF_WrongParameter(interp, "optlist in PDF_create_pvf");
        return TCL_ERROR;
    }

    try { PDF_create_pvf(p, filename, filename_len, data, (size_t) data_len, optlist);
    } catch;

    return TCL_OK;
}


static int
_wrap_PDF_create_textflow(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
    PDF *p;
    const char *text;
    int text_len;
    const char *optlist;
    int optlist_len;
    char *res;
    int volatile _result = -1;

    if (objc != 4)
    {
        PDF_WrongCommand(interp, "PDF_create_textflow p text optlist ");
        return TCL_ERROR;
    }

    if ((res = Tcl_GetStringFromObj(objv[1], NULL)) == NULL)
    {
        PDF_NoPDFHandle(interp, "PDF_create_textflow");
        return TCL_ERROR;
    }

    if (SWIG_GetPtr(res, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle(interp, "PDF_create_textflow");
        Tcl_AppendResult(interp, res, (char *) NULL);
        return TCL_ERROR;
    }

    if ((text = PDF_GetStringFromObj(p, interp, objv[2], PDF_UTF16, &text_len)) == NULL)
    {
        PDF_WrongParameter(interp, "text in PDF_create_textflow");
        return TCL_ERROR;
    }

    if ((optlist = PDF_GetStringFromObj(p, interp, objv[3], PDF_UTF8, &optlist_len)) == NULL)
    {
        PDF_WrongParameter(interp, "optlist in PDF_create_textflow");
        return TCL_ERROR;
    }

    try { _result = (int) PDF_create_textflow(p, text, text_len, optlist);
    } catch;

    sprintf(interp->result, "%d", _result);
    return TCL_OK;
}


static int
_wrap_PDF_curveto(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
    PDF *p;
    double x_1;
    double y_1;
    double x_2;
    double y_2;
    double x_3;
    double y_3;
    char *res;

    if (objc != 8)
    {
        PDF_WrongCommand(interp, "PDF_curveto p x_1 y_1 x_2 y_2 x_3 y_3 ");
        return TCL_ERROR;
    }

    if ((res = Tcl_GetStringFromObj(objv[1], NULL)) == NULL)
    {
        PDF_NoPDFHandle(interp, "PDF_curveto");
        return TCL_ERROR;
    }

    if (SWIG_GetPtr(res, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle(interp, "PDF_curveto");
        Tcl_AppendResult(interp, res, (char *) NULL);
        return TCL_ERROR;
    }

    if (Tcl_GetDoubleFromObj(interp, objv[2], &x_1) != TCL_OK)
    {
        PDF_WrongParameter(interp, "x_1 in PDF_curveto");
        return TCL_ERROR;
    }

    if (Tcl_GetDoubleFromObj(interp, objv[3], &y_1) != TCL_OK)
    {
        PDF_WrongParameter(interp, "y_1 in PDF_curveto");
        return TCL_ERROR;
    }

    if (Tcl_GetDoubleFromObj(interp, objv[4], &x_2) != TCL_OK)
    {
        PDF_WrongParameter(interp, "x_2 in PDF_curveto");
        return TCL_ERROR;
    }

    if (Tcl_GetDoubleFromObj(interp, objv[5], &y_2) != TCL_OK)
    {
        PDF_WrongParameter(interp, "y_2 in PDF_curveto");
        return TCL_ERROR;
    }

    if (Tcl_GetDoubleFromObj(interp, objv[6], &x_3) != TCL_OK)
    {
        PDF_WrongParameter(interp, "x_3 in PDF_curveto");
        return TCL_ERROR;
    }

    if (Tcl_GetDoubleFromObj(interp, objv[7], &y_3) != TCL_OK)
    {
        PDF_WrongParameter(interp, "y_3 in PDF_curveto");
        return TCL_ERROR;
    }

    try { PDF_curveto(p, x_1, y_1, x_2, y_2, x_3, y_3);
    } catch;

    return TCL_OK;
}


static int
_wrap_PDF_define_layer(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
    PDF *p;
    const char *name;
    int name_len;
    const char *optlist;
    int optlist_len;
    char *res;
    int volatile _result = -1;

    if (objc != 4)
    {
        PDF_WrongCommand(interp, "PDF_define_layer p name optlist ");
        return TCL_ERROR;
    }

    if ((res = Tcl_GetStringFromObj(objv[1], NULL)) == NULL)
    {
        PDF_NoPDFHandle(interp, "PDF_define_layer");
        return TCL_ERROR;
    }

    if (SWIG_GetPtr(res, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle(interp, "PDF_define_layer");
        Tcl_AppendResult(interp, res, (char *) NULL);
        return TCL_ERROR;
    }

    if ((name = PDF_GetStringFromObj(p, interp, objv[2], PDF_UTF16, &name_len)) == NULL)
    {
        PDF_WrongParameter(interp, "name in PDF_define_layer");
        return TCL_ERROR;
    }

    if ((optlist = PDF_GetStringFromObj(p, interp, objv[3], PDF_UTF8, &optlist_len)) == NULL)
    {
        PDF_WrongParameter(interp, "optlist in PDF_define_layer");
        return TCL_ERROR;
    }

    try { _result = (int) PDF_define_layer(p, name, name_len, optlist);
    } catch;

    sprintf(interp->result, "%d", _result);
    return TCL_OK;
}


static int
_wrap_PDF_delete(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
    PDF *p;
    char *res;

    if (objc != 2)
    {
        PDF_WrongCommand(interp, "PDF_delete p ");
        return TCL_ERROR;
    }

    if ((res = Tcl_GetStringFromObj(objv[1], NULL)) == NULL)
    {
        PDF_NoPDFHandle(interp, "PDF_delete");
        return TCL_ERROR;
    }

    if (SWIG_GetPtr(res, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle(interp, "PDF_delete");
        Tcl_AppendResult(interp, res, (char *) NULL);
        return TCL_ERROR;
    }

    PDF_delete(p);

    return TCL_OK;
}


static int
_wrap_PDF_delete_pvf(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
    PDF *p;
    const char *filename;
    int filename_len;
    char *res;
    int volatile _result = -1;

    if (objc != 3)
    {
        PDF_WrongCommand(interp, "PDF_delete_pvf p filename ");
        return TCL_ERROR;
    }

    if ((res = Tcl_GetStringFromObj(objv[1], NULL)) == NULL)
    {
        PDF_NoPDFHandle(interp, "PDF_delete_pvf");
        return TCL_ERROR;
    }

    if (SWIG_GetPtr(res, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle(interp, "PDF_delete_pvf");
        Tcl_AppendResult(interp, res, (char *) NULL);
        return TCL_ERROR;
    }

    if ((filename = PDF_GetStringFromObj(p, interp, objv[2], PDF_0UTF16, &filename_len)) == NULL)
    {
        PDF_WrongParameter(interp, "filename in PDF_delete_pvf");
        return TCL_ERROR;
    }

    try { _result = (int) PDF_delete_pvf(p, filename, filename_len);
    } catch;

    sprintf(interp->result, "%d", _result);
    return TCL_OK;
}


static int
_wrap_PDF_delete_textflow(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
    PDF *p;
    int textflow;
    char *res;

    if (objc != 3)
    {
        PDF_WrongCommand(interp, "PDF_delete_textflow p textflow ");
        return TCL_ERROR;
    }

    if ((res = Tcl_GetStringFromObj(objv[1], NULL)) == NULL)
    {
        PDF_NoPDFHandle(interp, "PDF_delete_textflow");
        return TCL_ERROR;
    }

    if (SWIG_GetPtr(res, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle(interp, "PDF_delete_textflow");
        Tcl_AppendResult(interp, res, (char *) NULL);
        return TCL_ERROR;
    }

    if (Tcl_GetIntFromObj(interp, objv[2], &textflow) != TCL_OK)
    {
        PDF_WrongParameter(interp, "textflow in PDF_delete_textflow");
        return TCL_ERROR;
    }

    try { PDF_delete_textflow(p, textflow);
    } catch;

    return TCL_OK;
}


static int
_wrap_PDF_encoding_set_char(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
    PDF *p;
    const char *encoding;
    int encoding_len;
    int slot;
    const char *glyphname;
    int glyphname_len;
    int uv;
    char *res;

    if (objc != 6)
    {
        PDF_WrongCommand(interp, "PDF_encoding_set_char p encoding slot glyphname uv ");
        return TCL_ERROR;
    }

    if ((res = Tcl_GetStringFromObj(objv[1], NULL)) == NULL)
    {
        PDF_NoPDFHandle(interp, "PDF_encoding_set_char");
        return TCL_ERROR;
    }

    if (SWIG_GetPtr(res, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle(interp, "PDF_encoding_set_char");
        Tcl_AppendResult(interp, res, (char *) NULL);
        return TCL_ERROR;
    }

    if ((encoding = PDF_GetStringFromObj(p, interp, objv[2], PDF_BYTES, &encoding_len)) == NULL)
    {
        PDF_WrongParameter(interp, "encoding in PDF_encoding_set_char");
        return TCL_ERROR;
    }

    if (Tcl_GetIntFromObj(interp, objv[3], &slot) != TCL_OK)
    {
        PDF_WrongParameter(interp, "slot in PDF_encoding_set_char");
        return TCL_ERROR;
    }

    if ((glyphname = PDF_GetStringFromObj(p, interp, objv[4], PDF_BYTES, &glyphname_len)) == NULL)
    {
        PDF_WrongParameter(interp, "glyphname in PDF_encoding_set_char");
        return TCL_ERROR;
    }

    if (Tcl_GetIntFromObj(interp, objv[5], &uv) != TCL_OK)
    {
        PDF_WrongParameter(interp, "uv in PDF_encoding_set_char");
        return TCL_ERROR;
    }

    try { PDF_encoding_set_char(p, encoding, slot, glyphname, uv);
    } catch;

    return TCL_OK;
}


static int
_wrap_PDF_end_document(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
    PDF *p;
    const char *optlist;
    int optlist_len;
    char *res;

    if (objc != 3)
    {
        PDF_WrongCommand(interp, "PDF_end_document p optlist ");
        return TCL_ERROR;
    }

    if ((res = Tcl_GetStringFromObj(objv[1], NULL)) == NULL)
    {
        PDF_NoPDFHandle(interp, "PDF_end_document");
        return TCL_ERROR;
    }

    if (SWIG_GetPtr(res, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle(interp, "PDF_end_document");
        Tcl_AppendResult(interp, res, (char *) NULL);
        return TCL_ERROR;
    }

    if ((optlist = PDF_GetStringFromObj(p, interp, objv[2], PDF_UTF8, &optlist_len)) == NULL)
    {
        PDF_WrongParameter(interp, "optlist in PDF_end_document");
        return TCL_ERROR;
    }

    try { PDF_end_document(p, optlist);
    } catch;

    return TCL_OK;
}


static int
_wrap_PDF_end_font(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
    PDF *p;
    char *res;

    if (objc != 2)
    {
        PDF_WrongCommand(interp, "PDF_end_font p ");
        return TCL_ERROR;
    }

    if ((res = Tcl_GetStringFromObj(objv[1], NULL)) == NULL)
    {
        PDF_NoPDFHandle(interp, "PDF_end_font");
        return TCL_ERROR;
    }

    if (SWIG_GetPtr(res, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle(interp, "PDF_end_font");
        Tcl_AppendResult(interp, res, (char *) NULL);
        return TCL_ERROR;
    }

    try { PDF_end_font(p);
    } catch;

    return TCL_OK;
}


static int
_wrap_PDF_end_glyph(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
    PDF *p;
    char *res;

    if (objc != 2)
    {
        PDF_WrongCommand(interp, "PDF_end_glyph p ");
        return TCL_ERROR;
    }

    if ((res = Tcl_GetStringFromObj(objv[1], NULL)) == NULL)
    {
        PDF_NoPDFHandle(interp, "PDF_end_glyph");
        return TCL_ERROR;
    }

    if (SWIG_GetPtr(res, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle(interp, "PDF_end_glyph");
        Tcl_AppendResult(interp, res, (char *) NULL);
        return TCL_ERROR;
    }

    try { PDF_end_glyph(p);
    } catch;

    return TCL_OK;
}


static int
_wrap_PDF_end_item(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
    PDF *p;
    int id;
    char *res;

    if (objc != 3)
    {
        PDF_WrongCommand(interp, "PDF_end_item p id ");
        return TCL_ERROR;
    }

    if ((res = Tcl_GetStringFromObj(objv[1], NULL)) == NULL)
    {
        PDF_NoPDFHandle(interp, "PDF_end_item");
        return TCL_ERROR;
    }

    if (SWIG_GetPtr(res, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle(interp, "PDF_end_item");
        Tcl_AppendResult(interp, res, (char *) NULL);
        return TCL_ERROR;
    }

    if (Tcl_GetIntFromObj(interp, objv[2], &id) != TCL_OK)
    {
        PDF_WrongParameter(interp, "id in PDF_end_item");
        return TCL_ERROR;
    }

    try { PDF_end_item(p, id);
    } catch;

    return TCL_OK;
}


static int
_wrap_PDF_end_layer(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
    PDF *p;
    char *res;

    if (objc != 2)
    {
        PDF_WrongCommand(interp, "PDF_end_layer p ");
        return TCL_ERROR;
    }

    if ((res = Tcl_GetStringFromObj(objv[1], NULL)) == NULL)
    {
        PDF_NoPDFHandle(interp, "PDF_end_layer");
        return TCL_ERROR;
    }

    if (SWIG_GetPtr(res, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle(interp, "PDF_end_layer");
        Tcl_AppendResult(interp, res, (char *) NULL);
        return TCL_ERROR;
    }

    try { PDF_end_layer(p);
    } catch;

    return TCL_OK;
}


static int
_wrap_PDF_end_mc(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
    PDF *p;
    char *res;

    if (objc != 2)
    {
        PDF_WrongCommand(interp, "PDF_end_mc p ");
        return TCL_ERROR;
    }

    if ((res = Tcl_GetStringFromObj(objv[1], NULL)) == NULL)
    {
        PDF_NoPDFHandle(interp, "PDF_end_mc");
        return TCL_ERROR;
    }

    if (SWIG_GetPtr(res, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle(interp, "PDF_end_mc");
        Tcl_AppendResult(interp, res, (char *) NULL);
        return TCL_ERROR;
    }

    try { PDF_end_mc(p);
    } catch;

    return TCL_OK;
}


static int
_wrap_PDF_end_page(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
    PDF *p;
    char *res;

    if (objc != 2)
    {
        PDF_WrongCommand(interp, "PDF_end_page p ");
        return TCL_ERROR;
    }

    if ((res = Tcl_GetStringFromObj(objv[1], NULL)) == NULL)
    {
        PDF_NoPDFHandle(interp, "PDF_end_page");
        return TCL_ERROR;
    }

    if (SWIG_GetPtr(res, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle(interp, "PDF_end_page");
        Tcl_AppendResult(interp, res, (char *) NULL);
        return TCL_ERROR;
    }

    try { PDF_end_page(p);
    } catch;

    return TCL_OK;
}


static int
_wrap_PDF_end_page_ext(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
    PDF *p;
    const char *optlist;
    int optlist_len;
    char *res;

    if (objc != 3)
    {
        PDF_WrongCommand(interp, "PDF_end_page_ext p optlist ");
        return TCL_ERROR;
    }

    if ((res = Tcl_GetStringFromObj(objv[1], NULL)) == NULL)
    {
        PDF_NoPDFHandle(interp, "PDF_end_page_ext");
        return TCL_ERROR;
    }

    if (SWIG_GetPtr(res, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle(interp, "PDF_end_page_ext");
        Tcl_AppendResult(interp, res, (char *) NULL);
        return TCL_ERROR;
    }

    if ((optlist = PDF_GetStringFromObj(p, interp, objv[2], PDF_UTF8, &optlist_len)) == NULL)
    {
        PDF_WrongParameter(interp, "optlist in PDF_end_page_ext");
        return TCL_ERROR;
    }

    try { PDF_end_page_ext(p, optlist);
    } catch;

    return TCL_OK;
}


static int
_wrap_PDF_end_pattern(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
    PDF *p;
    char *res;

    if (objc != 2)
    {
        PDF_WrongCommand(interp, "PDF_end_pattern p ");
        return TCL_ERROR;
    }

    if ((res = Tcl_GetStringFromObj(objv[1], NULL)) == NULL)
    {
        PDF_NoPDFHandle(interp, "PDF_end_pattern");
        return TCL_ERROR;
    }

    if (SWIG_GetPtr(res, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle(interp, "PDF_end_pattern");
        Tcl_AppendResult(interp, res, (char *) NULL);
        return TCL_ERROR;
    }

    try { PDF_end_pattern(p);
    } catch;

    return TCL_OK;
}


static int
_wrap_PDF_end_template(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
    PDF *p;
    char *res;

    if (objc != 2)
    {
        PDF_WrongCommand(interp, "PDF_end_template p ");
        return TCL_ERROR;
    }

    if ((res = Tcl_GetStringFromObj(objv[1], NULL)) == NULL)
    {
        PDF_NoPDFHandle(interp, "PDF_end_template");
        return TCL_ERROR;
    }

    if (SWIG_GetPtr(res, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle(interp, "PDF_end_template");
        Tcl_AppendResult(interp, res, (char *) NULL);
        return TCL_ERROR;
    }

    try { PDF_end_template(p);
    } catch;

    return TCL_OK;
}


static int
_wrap_PDF_endpath(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
    PDF *p;
    char *res;

    if (objc != 2)
    {
        PDF_WrongCommand(interp, "PDF_endpath p ");
        return TCL_ERROR;
    }

    if ((res = Tcl_GetStringFromObj(objv[1], NULL)) == NULL)
    {
        PDF_NoPDFHandle(interp, "PDF_endpath");
        return TCL_ERROR;
    }

    if (SWIG_GetPtr(res, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle(interp, "PDF_endpath");
        Tcl_AppendResult(interp, res, (char *) NULL);
        return TCL_ERROR;
    }

    try { PDF_endpath(p);
    } catch;

    return TCL_OK;
}


static int
_wrap_PDF_fill(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
    PDF *p;
    char *res;

    if (objc != 2)
    {
        PDF_WrongCommand(interp, "PDF_fill p ");
        return TCL_ERROR;
    }

    if ((res = Tcl_GetStringFromObj(objv[1], NULL)) == NULL)
    {
        PDF_NoPDFHandle(interp, "PDF_fill");
        return TCL_ERROR;
    }

    if (SWIG_GetPtr(res, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle(interp, "PDF_fill");
        Tcl_AppendResult(interp, res, (char *) NULL);
        return TCL_ERROR;
    }

    try { PDF_fill(p);
    } catch;

    return TCL_OK;
}


static int
_wrap_PDF_fill_imageblock(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
    PDF *p;
    int page;
    const char *blockname;
    int blockname_len;
    int image;
    const char *optlist;
    int optlist_len;
    char *res;
    int volatile _result = -1;

    if (objc != 6)
    {
        PDF_WrongCommand(interp, "PDF_fill_imageblock p page blockname image optlist ");
        return TCL_ERROR;
    }

    if ((res = Tcl_GetStringFromObj(objv[1], NULL)) == NULL)
    {
        PDF_NoPDFHandle(interp, "PDF_fill_imageblock");
        return TCL_ERROR;
    }

    if (SWIG_GetPtr(res, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle(interp, "PDF_fill_imageblock");
        Tcl_AppendResult(interp, res, (char *) NULL);
        return TCL_ERROR;
    }

    if (Tcl_GetIntFromObj(interp, objv[2], &page) != TCL_OK)
    {
        PDF_WrongParameter(interp, "page in PDF_fill_imageblock");
        return TCL_ERROR;
    }

    if ((blockname = PDF_GetStringFromObj(p, interp, objv[3], PDF_UTF8, &blockname_len)) == NULL)
    {
        PDF_WrongParameter(interp, "blockname in PDF_fill_imageblock");
        return TCL_ERROR;
    }

    if (Tcl_GetIntFromObj(interp, objv[4], &image) != TCL_OK)
    {
        PDF_WrongParameter(interp, "image in PDF_fill_imageblock");
        return TCL_ERROR;
    }

    if ((optlist = PDF_GetStringFromObj(p, interp, objv[5], PDF_UTF8, &optlist_len)) == NULL)
    {
        PDF_WrongParameter(interp, "optlist in PDF_fill_imageblock");
        return TCL_ERROR;
    }

    try { _result = (int) PDF_fill_imageblock(p, page, blockname, image, optlist);
    } catch;

    sprintf(interp->result, "%d", _result);
    return TCL_OK;
}


static int
_wrap_PDF_fill_pdfblock(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
    PDF *p;
    int page;
    const char *blockname;
    int blockname_len;
    int contents;
    const char *optlist;
    int optlist_len;
    char *res;
    int volatile _result = -1;

    if (objc != 6)
    {
        PDF_WrongCommand(interp, "PDF_fill_pdfblock p page blockname contents optlist ");
        return TCL_ERROR;
    }

    if ((res = Tcl_GetStringFromObj(objv[1], NULL)) == NULL)
    {
        PDF_NoPDFHandle(interp, "PDF_fill_pdfblock");
        return TCL_ERROR;
    }

    if (SWIG_GetPtr(res, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle(interp, "PDF_fill_pdfblock");
        Tcl_AppendResult(interp, res, (char *) NULL);
        return TCL_ERROR;
    }

    if (Tcl_GetIntFromObj(interp, objv[2], &page) != TCL_OK)
    {
        PDF_WrongParameter(interp, "page in PDF_fill_pdfblock");
        return TCL_ERROR;
    }

    if ((blockname = PDF_GetStringFromObj(p, interp, objv[3], PDF_UTF8, &blockname_len)) == NULL)
    {
        PDF_WrongParameter(interp, "blockname in PDF_fill_pdfblock");
        return TCL_ERROR;
    }

    if (Tcl_GetIntFromObj(interp, objv[4], &contents) != TCL_OK)
    {
        PDF_WrongParameter(interp, "contents in PDF_fill_pdfblock");
        return TCL_ERROR;
    }

    if ((optlist = PDF_GetStringFromObj(p, interp, objv[5], PDF_UTF8, &optlist_len)) == NULL)
    {
        PDF_WrongParameter(interp, "optlist in PDF_fill_pdfblock");
        return TCL_ERROR;
    }

    try { _result = (int) PDF_fill_pdfblock(p, page, blockname, contents, optlist);
    } catch;

    sprintf(interp->result, "%d", _result);
    return TCL_OK;
}


static int
_wrap_PDF_fill_stroke(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
    PDF *p;
    char *res;

    if (objc != 2)
    {
        PDF_WrongCommand(interp, "PDF_fill_stroke p ");
        return TCL_ERROR;
    }

    if ((res = Tcl_GetStringFromObj(objv[1], NULL)) == NULL)
    {
        PDF_NoPDFHandle(interp, "PDF_fill_stroke");
        return TCL_ERROR;
    }

    if (SWIG_GetPtr(res, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle(interp, "PDF_fill_stroke");
        Tcl_AppendResult(interp, res, (char *) NULL);
        return TCL_ERROR;
    }

    try { PDF_fill_stroke(p);
    } catch;

    return TCL_OK;
}


static int
_wrap_PDF_fill_textblock(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
    PDF *p;
    int page;
    const char *blockname;
    int blockname_len;
    const char *text;
    int text_len;
    const char *optlist;
    int optlist_len;
    char *res;
    int volatile _result = -1;

    if (objc != 6)
    {
        PDF_WrongCommand(interp, "PDF_fill_textblock p page blockname text optlist ");
        return TCL_ERROR;
    }

    if ((res = Tcl_GetStringFromObj(objv[1], NULL)) == NULL)
    {
        PDF_NoPDFHandle(interp, "PDF_fill_textblock");
        return TCL_ERROR;
    }

    if (SWIG_GetPtr(res, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle(interp, "PDF_fill_textblock");
        Tcl_AppendResult(interp, res, (char *) NULL);
        return TCL_ERROR;
    }

    if (Tcl_GetIntFromObj(interp, objv[2], &page) != TCL_OK)
    {
        PDF_WrongParameter(interp, "page in PDF_fill_textblock");
        return TCL_ERROR;
    }

    if ((blockname = PDF_GetStringFromObj(p, interp, objv[3], PDF_UTF8, &blockname_len)) == NULL)
    {
        PDF_WrongParameter(interp, "blockname in PDF_fill_textblock");
        return TCL_ERROR;
    }

    if ((text = PDF_GetStringFromObj(p, interp, objv[4], PDF_UTF16, &text_len)) == NULL)
    {
        PDF_WrongParameter(interp, "text in PDF_fill_textblock");
        return TCL_ERROR;
    }

    if ((optlist = PDF_GetStringFromObj(p, interp, objv[5], PDF_UTF8, &optlist_len)) == NULL)
    {
        PDF_WrongParameter(interp, "optlist in PDF_fill_textblock");
        return TCL_ERROR;
    }

    try { _result = (int) PDF_fill_textblock(p, page, blockname, text, text_len, optlist);
    } catch;

    sprintf(interp->result, "%d", _result);
    return TCL_OK;
}


static int
_wrap_PDF_findfont(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
    PDF *p;
    const char *fontname;
    int fontname_len;
    const char *encoding;
    int encoding_len;
    int embed;
    char *res;
    int volatile _result = -1;

    if (objc != 5)
    {
        PDF_WrongCommand(interp, "PDF_findfont p fontname encoding embed ");
        return TCL_ERROR;
    }

    if ((res = Tcl_GetStringFromObj(objv[1], NULL)) == NULL)
    {
        PDF_NoPDFHandle(interp, "PDF_findfont");
        return TCL_ERROR;
    }

    if (SWIG_GetPtr(res, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle(interp, "PDF_findfont");
        Tcl_AppendResult(interp, res, (char *) NULL);
        return TCL_ERROR;
    }

    if ((fontname = PDF_GetStringFromObj(p, interp, objv[2], PDF_BYTES, &fontname_len)) == NULL)
    {
        PDF_WrongParameter(interp, "fontname in PDF_findfont");
        return TCL_ERROR;
    }

    if ((encoding = PDF_GetStringFromObj(p, interp, objv[3], PDF_BYTES, &encoding_len)) == NULL)
    {
        PDF_WrongParameter(interp, "encoding in PDF_findfont");
        return TCL_ERROR;
    }

    if (Tcl_GetIntFromObj(interp, objv[4], &embed) != TCL_OK)
    {
        PDF_WrongParameter(interp, "embed in PDF_findfont");
        return TCL_ERROR;
    }

    try { _result = (int) PDF_findfont(p, fontname, encoding, embed);
    } catch;

    sprintf(interp->result, "%d", _result);
    return TCL_OK;
}


static int
_wrap_PDF_fit_image(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
    PDF *p;
    int image;
    double x;
    double y;
    const char *optlist;
    int optlist_len;
    char *res;

    if (objc != 6)
    {
        PDF_WrongCommand(interp, "PDF_fit_image p image x y optlist ");
        return TCL_ERROR;
    }

    if ((res = Tcl_GetStringFromObj(objv[1], NULL)) == NULL)
    {
        PDF_NoPDFHandle(interp, "PDF_fit_image");
        return TCL_ERROR;
    }

    if (SWIG_GetPtr(res, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle(interp, "PDF_fit_image");
        Tcl_AppendResult(interp, res, (char *) NULL);
        return TCL_ERROR;
    }

    if (Tcl_GetIntFromObj(interp, objv[2], &image) != TCL_OK)
    {
        PDF_WrongParameter(interp, "image in PDF_fit_image");
        return TCL_ERROR;
    }

    if (Tcl_GetDoubleFromObj(interp, objv[3], &x) != TCL_OK)
    {
        PDF_WrongParameter(interp, "x in PDF_fit_image");
        return TCL_ERROR;
    }

    if (Tcl_GetDoubleFromObj(interp, objv[4], &y) != TCL_OK)
    {
        PDF_WrongParameter(interp, "y in PDF_fit_image");
        return TCL_ERROR;
    }

    if ((optlist = PDF_GetStringFromObj(p, interp, objv[5], PDF_UTF8, &optlist_len)) == NULL)
    {
        PDF_WrongParameter(interp, "optlist in PDF_fit_image");
        return TCL_ERROR;
    }

    try { PDF_fit_image(p, image, x, y, optlist);
    } catch;

    return TCL_OK;
}


static int
_wrap_PDF_fit_pdi_page(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
    PDF *p;
    int page;
    double x;
    double y;
    const char *optlist;
    int optlist_len;
    char *res;

    if (objc != 6)
    {
        PDF_WrongCommand(interp, "PDF_fit_pdi_page p page x y optlist ");
        return TCL_ERROR;
    }

    if ((res = Tcl_GetStringFromObj(objv[1], NULL)) == NULL)
    {
        PDF_NoPDFHandle(interp, "PDF_fit_pdi_page");
        return TCL_ERROR;
    }

    if (SWIG_GetPtr(res, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle(interp, "PDF_fit_pdi_page");
        Tcl_AppendResult(interp, res, (char *) NULL);
        return TCL_ERROR;
    }

    if (Tcl_GetIntFromObj(interp, objv[2], &page) != TCL_OK)
    {
        PDF_WrongParameter(interp, "page in PDF_fit_pdi_page");
        return TCL_ERROR;
    }

    if (Tcl_GetDoubleFromObj(interp, objv[3], &x) != TCL_OK)
    {
        PDF_WrongParameter(interp, "x in PDF_fit_pdi_page");
        return TCL_ERROR;
    }

    if (Tcl_GetDoubleFromObj(interp, objv[4], &y) != TCL_OK)
    {
        PDF_WrongParameter(interp, "y in PDF_fit_pdi_page");
        return TCL_ERROR;
    }

    if ((optlist = PDF_GetStringFromObj(p, interp, objv[5], PDF_UTF8, &optlist_len)) == NULL)
    {
        PDF_WrongParameter(interp, "optlist in PDF_fit_pdi_page");
        return TCL_ERROR;
    }

    try { PDF_fit_pdi_page(p, page, x, y, optlist);
    } catch;

    return TCL_OK;
}


static int
_wrap_PDF_fit_textflow(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
    PDF *p;
    int textflow;
    double llx;
    double lly;
    double urx;
    double ury;
    const char *optlist;
    int optlist_len;
    char *res;
    char volatile *_result = NULL;

    if (objc != 8)
    {
        PDF_WrongCommand(interp, "PDF_fit_textflow p textflow llx lly urx ury optlist ");
        return TCL_ERROR;
    }

    if ((res = Tcl_GetStringFromObj(objv[1], NULL)) == NULL)
    {
        PDF_NoPDFHandle(interp, "PDF_fit_textflow");
        return TCL_ERROR;
    }

    if (SWIG_GetPtr(res, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle(interp, "PDF_fit_textflow");
        Tcl_AppendResult(interp, res, (char *) NULL);
        return TCL_ERROR;
    }

    if (Tcl_GetIntFromObj(interp, objv[2], &textflow) != TCL_OK)
    {
        PDF_WrongParameter(interp, "textflow in PDF_fit_textflow");
        return TCL_ERROR;
    }

    if (Tcl_GetDoubleFromObj(interp, objv[3], &llx) != TCL_OK)
    {
        PDF_WrongParameter(interp, "llx in PDF_fit_textflow");
        return TCL_ERROR;
    }

    if (Tcl_GetDoubleFromObj(interp, objv[4], &lly) != TCL_OK)
    {
        PDF_WrongParameter(interp, "lly in PDF_fit_textflow");
        return TCL_ERROR;
    }

    if (Tcl_GetDoubleFromObj(interp, objv[5], &urx) != TCL_OK)
    {
        PDF_WrongParameter(interp, "urx in PDF_fit_textflow");
        return TCL_ERROR;
    }

    if (Tcl_GetDoubleFromObj(interp, objv[6], &ury) != TCL_OK)
    {
        PDF_WrongParameter(interp, "ury in PDF_fit_textflow");
        return TCL_ERROR;
    }

    if ((optlist = PDF_GetStringFromObj(p, interp, objv[7], PDF_UTF8, &optlist_len)) == NULL)
    {
        PDF_WrongParameter(interp, "optlist in PDF_fit_textflow");
        return TCL_ERROR;
    }

    try { _result = (char *) PDF_fit_textflow(p, textflow, llx, lly, urx, ury, optlist);
    } catch;

    Tcl_SetResult(interp, (char *) _result, TCL_VOLATILE);
    return TCL_OK;
}


static int
_wrap_PDF_fit_textline(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
    PDF *p;
    const char *text;
    int text_len;
    double x;
    double y;
    const char *optlist;
    int optlist_len;
    char *res;

    if (objc != 6)
    {
        PDF_WrongCommand(interp, "PDF_fit_textline p text x y optlist ");
        return TCL_ERROR;
    }

    if ((res = Tcl_GetStringFromObj(objv[1], NULL)) == NULL)
    {
        PDF_NoPDFHandle(interp, "PDF_fit_textline");
        return TCL_ERROR;
    }

    if (SWIG_GetPtr(res, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle(interp, "PDF_fit_textline");
        Tcl_AppendResult(interp, res, (char *) NULL);
        return TCL_ERROR;
    }

    if ((text = PDF_GetStringFromObj(p, interp, objv[2], PDF_UTF16, &text_len)) == NULL)
    {
        PDF_WrongParameter(interp, "text in PDF_fit_textline");
        return TCL_ERROR;
    }

    if (Tcl_GetDoubleFromObj(interp, objv[3], &x) != TCL_OK)
    {
        PDF_WrongParameter(interp, "x in PDF_fit_textline");
        return TCL_ERROR;
    }

    if (Tcl_GetDoubleFromObj(interp, objv[4], &y) != TCL_OK)
    {
        PDF_WrongParameter(interp, "y in PDF_fit_textline");
        return TCL_ERROR;
    }

    if ((optlist = PDF_GetStringFromObj(p, interp, objv[5], PDF_UTF8, &optlist_len)) == NULL)
    {
        PDF_WrongParameter(interp, "optlist in PDF_fit_textline");
        return TCL_ERROR;
    }

    try { PDF_fit_textline(p, text, text_len, x, y, optlist);
    } catch;

    return TCL_OK;
}


static int
_wrap_PDF_get_apiname(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
    PDF *p;
    char *res;
    char volatile *_result = NULL;

    if (objc != 2)
    {
        PDF_WrongCommand(interp, "PDF_get_apiname p ");
        return TCL_ERROR;
    }

    if ((res = Tcl_GetStringFromObj(objv[1], NULL)) == NULL)
    {
        PDF_NoPDFHandle(interp, "PDF_get_apiname");
        return TCL_ERROR;
    }

    if (SWIG_GetPtr(res, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle(interp, "PDF_get_apiname");
        Tcl_AppendResult(interp, res, (char *) NULL);
        return TCL_ERROR;
    }

    try { _result = (char *) PDF_get_apiname(p);
    } catch;

    Tcl_SetResult(interp, (char *) _result, TCL_VOLATILE);
    return TCL_OK;
}


static int
_wrap_PDF_get_buffer(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
    PDF *p;
    char *res;
    char volatile *_result = NULL;
    long size;
    Tcl_Obj *objPtr;

    if (objc != 2)
    {
        PDF_WrongCommand(interp, "PDF_get_buffer p ");
        return TCL_ERROR;
    }

    if ((res = Tcl_GetStringFromObj(objv[1], NULL)) == NULL)
    {
        PDF_NoPDFHandle(interp, "PDF_get_buffer");
        return TCL_ERROR;
    }

    if (SWIG_GetPtr(res, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle(interp, "PDF_get_buffer");
        Tcl_AppendResult(interp, res, (char *) NULL);
        return TCL_ERROR;
    }

    try { _result = (char *) PDF_get_buffer(p, &size);
    } catch;

    objPtr = Tcl_GetObjResult(interp);
    Tcl_SetByteArrayObj(objPtr, (unsigned char *) _result, (int) size);
    return TCL_OK;
}


static int
_wrap_PDF_get_errmsg(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
    PDF *p;
    char *res;
    char volatile *_result = NULL;

    if (objc != 2)
    {
        PDF_WrongCommand(interp, "PDF_get_errmsg p ");
        return TCL_ERROR;
    }

    if ((res = Tcl_GetStringFromObj(objv[1], NULL)) == NULL)
    {
        PDF_NoPDFHandle(interp, "PDF_get_errmsg");
        return TCL_ERROR;
    }

    if (SWIG_GetPtr(res, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle(interp, "PDF_get_errmsg");
        Tcl_AppendResult(interp, res, (char *) NULL);
        return TCL_ERROR;
    }

    try { _result = (char *) PDF_get_errmsg(p);
    } catch;

    Tcl_SetResult(interp, (char *) _result, TCL_VOLATILE);
    return TCL_OK;
}


static int
_wrap_PDF_get_errnum(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
    PDF *p;
    char *res;
    int volatile _result = -1;

    if (objc != 2)
    {
        PDF_WrongCommand(interp, "PDF_get_errnum p ");
        return TCL_ERROR;
    }

    if ((res = Tcl_GetStringFromObj(objv[1], NULL)) == NULL)
    {
        PDF_NoPDFHandle(interp, "PDF_get_errnum");
        return TCL_ERROR;
    }

    if (SWIG_GetPtr(res, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle(interp, "PDF_get_errnum");
        Tcl_AppendResult(interp, res, (char *) NULL);
        return TCL_ERROR;
    }

    try { _result = (int) PDF_get_errnum(p);
    } catch;

    sprintf(interp->result, "%d", _result);
    return TCL_OK;
}


static int
_wrap_PDF_get_parameter(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
    PDF *p;
    const char *key;
    int key_len;
    double modifier;
    char *res;
    char volatile *_result = NULL;

    if (objc != 4)
    {
        PDF_WrongCommand(interp, "PDF_get_parameter p key modifier ");
        return TCL_ERROR;
    }

    if ((res = Tcl_GetStringFromObj(objv[1], NULL)) == NULL)
    {
        PDF_NoPDFHandle(interp, "PDF_get_parameter");
        return TCL_ERROR;
    }

    if (SWIG_GetPtr(res, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle(interp, "PDF_get_parameter");
        Tcl_AppendResult(interp, res, (char *) NULL);
        return TCL_ERROR;
    }

    if ((key = PDF_GetStringFromObj(p, interp, objv[2], PDF_BYTES, &key_len)) == NULL)
    {
        PDF_WrongParameter(interp, "key in PDF_get_parameter");
        return TCL_ERROR;
    }

    if (Tcl_GetDoubleFromObj(interp, objv[3], &modifier) != TCL_OK)
    {
        PDF_WrongParameter(interp, "modifier in PDF_get_parameter");
        return TCL_ERROR;
    }

    try { _result = (char *) PDF_get_parameter(p, key, modifier);
    } catch;

    Tcl_SetResult(interp, (char *) _result, TCL_VOLATILE);
    return TCL_OK;
}


static int
_wrap_PDF_get_pdi_parameter(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
    PDF *p;
    const char *key;
    int key_len;
    int doc;
    int page;
    int reserved;
    char *res;
    char volatile *_result = NULL;

    if (objc != 6)
    {
        PDF_WrongCommand(interp, "PDF_get_pdi_parameter p key doc page reserved ");
        return TCL_ERROR;
    }

    if ((res = Tcl_GetStringFromObj(objv[1], NULL)) == NULL)
    {
        PDF_NoPDFHandle(interp, "PDF_get_pdi_parameter");
        return TCL_ERROR;
    }

    if (SWIG_GetPtr(res, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle(interp, "PDF_get_pdi_parameter");
        Tcl_AppendResult(interp, res, (char *) NULL);
        return TCL_ERROR;
    }

    if ((key = PDF_GetStringFromObj(p, interp, objv[2], PDF_BYTES, &key_len)) == NULL)
    {
        PDF_WrongParameter(interp, "key in PDF_get_pdi_parameter");
        return TCL_ERROR;
    }

    if (Tcl_GetIntFromObj(interp, objv[3], &doc) != TCL_OK)
    {
        PDF_WrongParameter(interp, "doc in PDF_get_pdi_parameter");
        return TCL_ERROR;
    }

    if (Tcl_GetIntFromObj(interp, objv[4], &page) != TCL_OK)
    {
        PDF_WrongParameter(interp, "page in PDF_get_pdi_parameter");
        return TCL_ERROR;
    }

    if (Tcl_GetIntFromObj(interp, objv[5], &reserved) != TCL_OK)
    {
        PDF_WrongParameter(interp, "reserved in PDF_get_pdi_parameter");
        return TCL_ERROR;
    }

    try { _result = (char *) PDF_get_pdi_parameter(p, key, doc, page, reserved, NULL);
    } catch;

    Tcl_SetResult(interp, (char *) _result, TCL_VOLATILE);
    return TCL_OK;
}


static int
_wrap_PDF_get_pdi_value(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
    PDF *p;
    const char *key;
    int key_len;
    int doc;
    int page;
    int reserved;
    char *res;
    double volatile _result = 0;

    if (objc != 6)
    {
        PDF_WrongCommand(interp, "PDF_get_pdi_value p key doc page reserved ");
        return TCL_ERROR;
    }

    if ((res = Tcl_GetStringFromObj(objv[1], NULL)) == NULL)
    {
        PDF_NoPDFHandle(interp, "PDF_get_pdi_value");
        return TCL_ERROR;
    }

    if (SWIG_GetPtr(res, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle(interp, "PDF_get_pdi_value");
        Tcl_AppendResult(interp, res, (char *) NULL);
        return TCL_ERROR;
    }

    if ((key = PDF_GetStringFromObj(p, interp, objv[2], PDF_BYTES, &key_len)) == NULL)
    {
        PDF_WrongParameter(interp, "key in PDF_get_pdi_value");
        return TCL_ERROR;
    }

    if (Tcl_GetIntFromObj(interp, objv[3], &doc) != TCL_OK)
    {
        PDF_WrongParameter(interp, "doc in PDF_get_pdi_value");
        return TCL_ERROR;
    }

    if (Tcl_GetIntFromObj(interp, objv[4], &page) != TCL_OK)
    {
        PDF_WrongParameter(interp, "page in PDF_get_pdi_value");
        return TCL_ERROR;
    }

    if (Tcl_GetIntFromObj(interp, objv[5], &reserved) != TCL_OK)
    {
        PDF_WrongParameter(interp, "reserved in PDF_get_pdi_value");
        return TCL_ERROR;
    }

    try { _result = (double) PDF_get_pdi_value(p, key, doc, page, reserved);
    } catch;

    Tcl_PrintDouble(interp, (double) _result, interp->result);
    return TCL_OK;
}


static int
_wrap_PDF_get_value(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
    PDF *p;
    const char *key;
    int key_len;
    double modifier;
    char *res;
    double volatile _result = 0;

    if (objc != 4)
    {
        PDF_WrongCommand(interp, "PDF_get_value p key modifier ");
        return TCL_ERROR;
    }

    if ((res = Tcl_GetStringFromObj(objv[1], NULL)) == NULL)
    {
        PDF_NoPDFHandle(interp, "PDF_get_value");
        return TCL_ERROR;
    }

    if (SWIG_GetPtr(res, (void **) &p, "_PDF_p"))
    {
        p = NULL;
    }

    if ((key = PDF_GetStringFromObj(p, interp, objv[2], PDF_BYTES, &key_len)) == NULL)
    {
        PDF_WrongParameter(interp, "key in PDF_get_value");
        return TCL_ERROR;
    }

    if (Tcl_GetDoubleFromObj(interp, objv[3], &modifier) != TCL_OK)
    {
        PDF_WrongParameter(interp, "modifier in PDF_get_value");
        return TCL_ERROR;
    }

    if (p != NULL)
    {
        try { _result = (double) PDF_get_value(p, key, modifier);
        } catch;
    }
    else
    {
        _result = (double) PDF_get_value(p, key, modifier);
    }

    Tcl_PrintDouble(interp, (double) _result, interp->result);
    return TCL_OK;
}


static int
_wrap_PDF_info_textflow(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
    PDF *p;
    int textflow;
    const char *keyword;
    int keyword_len;
    char *res;
    double volatile _result = 0;

    if (objc != 4)
    {
        PDF_WrongCommand(interp, "PDF_info_textflow p textflow keyword ");
        return TCL_ERROR;
    }

    if ((res = Tcl_GetStringFromObj(objv[1], NULL)) == NULL)
    {
        PDF_NoPDFHandle(interp, "PDF_info_textflow");
        return TCL_ERROR;
    }

    if (SWIG_GetPtr(res, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle(interp, "PDF_info_textflow");
        Tcl_AppendResult(interp, res, (char *) NULL);
        return TCL_ERROR;
    }

    if (Tcl_GetIntFromObj(interp, objv[2], &textflow) != TCL_OK)
    {
        PDF_WrongParameter(interp, "textflow in PDF_info_textflow");
        return TCL_ERROR;
    }

    if ((keyword = PDF_GetStringFromObj(p, interp, objv[3], PDF_BYTES, &keyword_len)) == NULL)
    {
        PDF_WrongParameter(interp, "keyword in PDF_info_textflow");
        return TCL_ERROR;
    }

    try { _result = (double) PDF_info_textflow(p, textflow, keyword);
    } catch;

    Tcl_PrintDouble(interp, (double) _result, interp->result);
    return TCL_OK;
}


static int
_wrap_PDF_initgraphics(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
    PDF *p;
    char *res;

    if (objc != 2)
    {
        PDF_WrongCommand(interp, "PDF_initgraphics p ");
        return TCL_ERROR;
    }

    if ((res = Tcl_GetStringFromObj(objv[1], NULL)) == NULL)
    {
        PDF_NoPDFHandle(interp, "PDF_initgraphics");
        return TCL_ERROR;
    }

    if (SWIG_GetPtr(res, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle(interp, "PDF_initgraphics");
        Tcl_AppendResult(interp, res, (char *) NULL);
        return TCL_ERROR;
    }

    try { PDF_initgraphics(p);
    } catch;

    return TCL_OK;
}


static int
_wrap_PDF_lineto(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
    PDF *p;
    double x;
    double y;
    char *res;

    if (objc != 4)
    {
        PDF_WrongCommand(interp, "PDF_lineto p x y ");
        return TCL_ERROR;
    }

    if ((res = Tcl_GetStringFromObj(objv[1], NULL)) == NULL)
    {
        PDF_NoPDFHandle(interp, "PDF_lineto");
        return TCL_ERROR;
    }

    if (SWIG_GetPtr(res, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle(interp, "PDF_lineto");
        Tcl_AppendResult(interp, res, (char *) NULL);
        return TCL_ERROR;
    }

    if (Tcl_GetDoubleFromObj(interp, objv[2], &x) != TCL_OK)
    {
        PDF_WrongParameter(interp, "x in PDF_lineto");
        return TCL_ERROR;
    }

    if (Tcl_GetDoubleFromObj(interp, objv[3], &y) != TCL_OK)
    {
        PDF_WrongParameter(interp, "y in PDF_lineto");
        return TCL_ERROR;
    }

    try { PDF_lineto(p, x, y);
    } catch;

    return TCL_OK;
}


static int
_wrap_PDF_load_font(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
    PDF *p;
    const char *fontname;
    int fontname_len;
    const char *encoding;
    int encoding_len;
    const char *optlist;
    int optlist_len;
    char *res;
    int volatile _result = -1;

    if (objc != 5)
    {
        PDF_WrongCommand(interp, "PDF_load_font p fontname encoding optlist ");
        return TCL_ERROR;
    }

    if ((res = Tcl_GetStringFromObj(objv[1], NULL)) == NULL)
    {
        PDF_NoPDFHandle(interp, "PDF_load_font");
        return TCL_ERROR;
    }

    if (SWIG_GetPtr(res, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle(interp, "PDF_load_font");
        Tcl_AppendResult(interp, res, (char *) NULL);
        return TCL_ERROR;
    }

    if ((fontname = PDF_GetStringFromObj(p, interp, objv[2], PDF_0UTF16, &fontname_len)) == NULL)
    {
        PDF_WrongParameter(interp, "fontname in PDF_load_font");
        return TCL_ERROR;
    }

    if ((encoding = PDF_GetStringFromObj(p, interp, objv[3], PDF_BYTES, &encoding_len)) == NULL)
    {
        PDF_WrongParameter(interp, "encoding in PDF_load_font");
        return TCL_ERROR;
    }

    if ((optlist = PDF_GetStringFromObj(p, interp, objv[4], PDF_UTF8, &optlist_len)) == NULL)
    {
        PDF_WrongParameter(interp, "optlist in PDF_load_font");
        return TCL_ERROR;
    }

    try { _result = (int) PDF_load_font(p, fontname, fontname_len, encoding, optlist);
    } catch;

    sprintf(interp->result, "%d", _result);
    return TCL_OK;
}


static int
_wrap_PDF_load_iccprofile(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
    PDF *p;
    const char *profilename;
    int profilename_len;
    const char *optlist;
    int optlist_len;
    char *res;
    int volatile _result = -1;

    if (objc != 4)
    {
        PDF_WrongCommand(interp, "PDF_load_iccprofile p profilename optlist ");
        return TCL_ERROR;
    }

    if ((res = Tcl_GetStringFromObj(objv[1], NULL)) == NULL)
    {
        PDF_NoPDFHandle(interp, "PDF_load_iccprofile");
        return TCL_ERROR;
    }

    if (SWIG_GetPtr(res, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle(interp, "PDF_load_iccprofile");
        Tcl_AppendResult(interp, res, (char *) NULL);
        return TCL_ERROR;
    }

    if ((profilename = PDF_GetStringFromObj(p, interp, objv[2], PDF_0UTF16, &profilename_len)) == NULL)
    {
        PDF_WrongParameter(interp, "profilename in PDF_load_iccprofile");
        return TCL_ERROR;
    }

    if ((optlist = PDF_GetStringFromObj(p, interp, objv[3], PDF_UTF8, &optlist_len)) == NULL)
    {
        PDF_WrongParameter(interp, "optlist in PDF_load_iccprofile");
        return TCL_ERROR;
    }

    try { _result = (int) PDF_load_iccprofile(p, profilename, profilename_len, optlist);
    } catch;

    sprintf(interp->result, "%d", _result);
    return TCL_OK;
}


static int
_wrap_PDF_load_image(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
    PDF *p;
    const char *imagetype;
    int imagetype_len;
    const char *filename;
    int filename_len;
    const char *optlist;
    int optlist_len;
    char *res;
    int volatile _result = -1;

    if (objc != 5)
    {
        PDF_WrongCommand(interp, "PDF_load_image p imagetype filename optlist ");
        return TCL_ERROR;
    }

    if ((res = Tcl_GetStringFromObj(objv[1], NULL)) == NULL)
    {
        PDF_NoPDFHandle(interp, "PDF_load_image");
        return TCL_ERROR;
    }

    if (SWIG_GetPtr(res, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle(interp, "PDF_load_image");
        Tcl_AppendResult(interp, res, (char *) NULL);
        return TCL_ERROR;
    }

    if ((imagetype = PDF_GetStringFromObj(p, interp, objv[2], PDF_BYTES, &imagetype_len)) == NULL)
    {
        PDF_WrongParameter(interp, "imagetype in PDF_load_image");
        return TCL_ERROR;
    }

    if ((filename = PDF_GetStringFromObj(p, interp, objv[3], PDF_0UTF16, &filename_len)) == NULL)
    {
        PDF_WrongParameter(interp, "filename in PDF_load_image");
        return TCL_ERROR;
    }

    if ((optlist = PDF_GetStringFromObj(p, interp, objv[4], PDF_UTF8, &optlist_len)) == NULL)
    {
        PDF_WrongParameter(interp, "optlist in PDF_load_image");
        return TCL_ERROR;
    }

    try { _result = (int) PDF_load_image(p, imagetype, filename, filename_len, optlist);
    } catch;

    sprintf(interp->result, "%d", _result);
    return TCL_OK;
}


static int
_wrap_PDF_makespotcolor(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
    PDF *p;
    const char *spotname;
    int spotname_len;
    char *res;
    int volatile _result = -1;

    if (objc != 3 && objc != 4)  /* downward compatibility */
    {
        PDF_WrongCommand(interp, "PDF_makespotcolor p spotname ");
        return TCL_ERROR;
    }

    if ((res = Tcl_GetStringFromObj(objv[1], NULL)) == NULL)
    {
        PDF_NoPDFHandle(interp, "PDF_makespotcolor");
        return TCL_ERROR;
    }

    if (SWIG_GetPtr(res, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle(interp, "PDF_makespotcolor");
        Tcl_AppendResult(interp, res, (char *) NULL);
        return TCL_ERROR;
    }

    if ((spotname = PDF_GetStringFromObj(p, interp, objv[2], PDF_0BYTES, &spotname_len)) == NULL)
    {
        PDF_WrongParameter(interp, "spotname in PDF_makespotcolor");
        return TCL_ERROR;
    }

    try { _result = (int) PDF_makespotcolor(p, spotname, 0);
    } catch;

    sprintf(interp->result, "%d", _result);
    return TCL_OK;
}


static int
_wrap_PDF_mc_point(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
    PDF *p;
    const char *tag;
    int tag_len;
    const char *optlist;
    int optlist_len;
    char *res;

    if (objc != 4)
    {
        PDF_WrongCommand(interp, "PDF_mc_point p tag optlist ");
        return TCL_ERROR;
    }

    if ((res = Tcl_GetStringFromObj(objv[1], NULL)) == NULL)
    {
        PDF_NoPDFHandle(interp, "PDF_mc_point");
        return TCL_ERROR;
    }

    if (SWIG_GetPtr(res, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle(interp, "PDF_mc_point");
        Tcl_AppendResult(interp, res, (char *) NULL);
        return TCL_ERROR;
    }

    if ((tag = PDF_GetStringFromObj(p, interp, objv[2], PDF_BYTES, &tag_len)) == NULL)
    {
        PDF_WrongParameter(interp, "tag in PDF_mc_point");
        return TCL_ERROR;
    }

    if ((optlist = PDF_GetStringFromObj(p, interp, objv[3], PDF_UTF8, &optlist_len)) == NULL)
    {
        PDF_WrongParameter(interp, "optlist in PDF_mc_point");
        return TCL_ERROR;
    }

    try { PDF_mc_point(p, tag, optlist);
    } catch;

    return TCL_OK;
}


static int
_wrap_PDF_moveto(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
    PDF *p;
    double x;
    double y;
    char *res;

    if (objc != 4)
    {
        PDF_WrongCommand(interp, "PDF_moveto p x y ");
        return TCL_ERROR;
    }

    if ((res = Tcl_GetStringFromObj(objv[1], NULL)) == NULL)
    {
        PDF_NoPDFHandle(interp, "PDF_moveto");
        return TCL_ERROR;
    }

    if (SWIG_GetPtr(res, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle(interp, "PDF_moveto");
        Tcl_AppendResult(interp, res, (char *) NULL);
        return TCL_ERROR;
    }

    if (Tcl_GetDoubleFromObj(interp, objv[2], &x) != TCL_OK)
    {
        PDF_WrongParameter(interp, "x in PDF_moveto");
        return TCL_ERROR;
    }

    if (Tcl_GetDoubleFromObj(interp, objv[3], &y) != TCL_OK)
    {
        PDF_WrongParameter(interp, "y in PDF_moveto");
        return TCL_ERROR;
    }

    try { PDF_moveto(p, x, y);
    } catch;

    return TCL_OK;
}


static int
_wrap_PDF_new(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
    PDF *p;

    if (objc != 1)
    {
        PDF_WrongCommand(interp, "PDF_new ");
        return TCL_ERROR;
    }

    p = (PDF *) PDF_new();
    if (p != NULL)
    {
        int major, minor, type, patchlevel;
        char versionbuf[32];

        Tcl_GetVersion(&major, &minor, &patchlevel, &type);
        sprintf(versionbuf, "Tcl %d.%d%c%d", major, minor, "ab."[type], patchlevel);

        PDF_set_parameter(p, "binding", versionbuf);
        PDF_set_parameter(p, "textformat", "auto2");
        PDF_set_parameter(p, "hypertextformat", "auto2");
        PDF_set_parameter(p, "hypertextencoding", "");

        SWIG_MakePtr(interp->result, (void *) p, "_PDF_p");
    }
    else
    {
        Tcl_SetResult(interp, "Couldn't create PDF handle", TCL_STATIC);
    }

    return TCL_OK;
}


static int
_wrap_PDF_open_CCITT(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
    PDF *p;
    const char *filename;
    int filename_len;
    int width;
    int height;
    int BitReverse;
    int K;
    int BlackIs1;
    char *res;
    int volatile _result = -1;

    if (objc != 8)
    {
        PDF_WrongCommand(interp, "PDF_open_CCITT p filename width height BitReverse K BlackIs1 ");
        return TCL_ERROR;
    }

    if ((res = Tcl_GetStringFromObj(objv[1], NULL)) == NULL)
    {
        PDF_NoPDFHandle(interp, "PDF_open_CCITT");
        return TCL_ERROR;
    }

    if (SWIG_GetPtr(res, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle(interp, "PDF_open_CCITT");
        Tcl_AppendResult(interp, res, (char *) NULL);
        return TCL_ERROR;
    }

    if ((filename = PDF_GetStringFromObj(p, interp, objv[2], PDF_BYTES, &filename_len)) == NULL)
    {
        PDF_WrongParameter(interp, "filename in PDF_open_CCITT");
        return TCL_ERROR;
    }

    if (Tcl_GetIntFromObj(interp, objv[3], &width) != TCL_OK)
    {
        PDF_WrongParameter(interp, "width in PDF_open_CCITT");
        return TCL_ERROR;
    }

    if (Tcl_GetIntFromObj(interp, objv[4], &height) != TCL_OK)
    {
        PDF_WrongParameter(interp, "height in PDF_open_CCITT");
        return TCL_ERROR;
    }

    if (Tcl_GetIntFromObj(interp, objv[5], &BitReverse) != TCL_OK)
    {
        PDF_WrongParameter(interp, "BitReverse in PDF_open_CCITT");
        return TCL_ERROR;
    }

    if (Tcl_GetIntFromObj(interp, objv[6], &K) != TCL_OK)
    {
        PDF_WrongParameter(interp, "K in PDF_open_CCITT");
        return TCL_ERROR;
    }

    if (Tcl_GetIntFromObj(interp, objv[7], &BlackIs1) != TCL_OK)
    {
        PDF_WrongParameter(interp, "BlackIs1 in PDF_open_CCITT");
        return TCL_ERROR;
    }

    try { _result = (int) PDF_open_CCITT(p, filename, width, height, BitReverse, K, BlackIs1);
    } catch;

    sprintf(interp->result, "%d", _result);
    return TCL_OK;
}


static int
_wrap_PDF_open_file(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
    PDF *p;
    const char *filename;
    int filename_len;
    char *res;
    int volatile _result = -1;

    if (objc != 3)
    {
        PDF_WrongCommand(interp, "PDF_open_file p filename ");
        return TCL_ERROR;
    }

    if ((res = Tcl_GetStringFromObj(objv[1], NULL)) == NULL)
    {
        PDF_NoPDFHandle(interp, "PDF_open_file");
        return TCL_ERROR;
    }

    if (SWIG_GetPtr(res, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle(interp, "PDF_open_file");
        Tcl_AppendResult(interp, res, (char *) NULL);
        return TCL_ERROR;
    }

    if ((filename = PDF_GetStringFromObj(p, interp, objv[2], PDF_BYTES, &filename_len)) == NULL)
    {
        PDF_WrongParameter(interp, "filename in PDF_open_file");
        return TCL_ERROR;
    }

    try { _result = (int) PDF_open_file(p, filename);
    } catch;

    sprintf(interp->result, "%d", _result);
    return TCL_OK;
}


static int
_wrap_PDF_open_image(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
    PDF *p;
    const char *imagetype;
    int imagetype_len;
    const char *source;
    int source_len;
    const char *data;
    int data_len;
    long length;
    int width;
    int height;
    int components;
    int bpc;
    const char *params;
    int params_len;
    char *res;
    int volatile _result = -1;

    if (objc != 11)
    {
        PDF_WrongCommand(interp, "PDF_open_image p imagetype source data length width height components bpc params ");
        return TCL_ERROR;
    }

    if ((res = Tcl_GetStringFromObj(objv[1], NULL)) == NULL)
    {
        PDF_NoPDFHandle(interp, "PDF_open_image");
        return TCL_ERROR;
    }

    if (SWIG_GetPtr(res, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle(interp, "PDF_open_image");
        Tcl_AppendResult(interp, res, (char *) NULL);
        return TCL_ERROR;
    }

    if ((imagetype = PDF_GetStringFromObj(p, interp, objv[2], PDF_BYTES, &imagetype_len)) == NULL)
    {
        PDF_WrongParameter(interp, "imagetype in PDF_open_image");
        return TCL_ERROR;
    }

    if ((source = PDF_GetStringFromObj(p, interp, objv[3], PDF_BYTES, &source_len)) == NULL)
    {
        PDF_WrongParameter(interp, "source in PDF_open_image");
        return TCL_ERROR;
    }

    if ((data = Tcl_GetByteArrayFromObj(objv[4], &data_len)) == NULL)
    {
        PDF_WrongParameter(interp, "data in PDF_open_image");
        return TCL_ERROR;
    }

    if (Tcl_GetLongFromObj(interp, objv[5], &length) != TCL_OK)
    {
        PDF_WrongParameter(interp, "length in PDF_open_image");
        return TCL_ERROR;
    }

    if (Tcl_GetIntFromObj(interp, objv[6], &width) != TCL_OK)
    {
        PDF_WrongParameter(interp, "width in PDF_open_image");
        return TCL_ERROR;
    }

    if (Tcl_GetIntFromObj(interp, objv[7], &height) != TCL_OK)
    {
        PDF_WrongParameter(interp, "height in PDF_open_image");
        return TCL_ERROR;
    }

    if (Tcl_GetIntFromObj(interp, objv[8], &components) != TCL_OK)
    {
        PDF_WrongParameter(interp, "components in PDF_open_image");
        return TCL_ERROR;
    }

    if (Tcl_GetIntFromObj(interp, objv[9], &bpc) != TCL_OK)
    {
        PDF_WrongParameter(interp, "bpc in PDF_open_image");
        return TCL_ERROR;
    }

    if ((params = PDF_GetStringFromObj(p, interp, objv[10], PDF_BYTES, &params_len)) == NULL)
    {
        PDF_WrongParameter(interp, "params in PDF_open_image");
        return TCL_ERROR;
    }

    try { _result = (int) PDF_open_image(p, imagetype, source, data, length, width, height, components, bpc, params);
    } catch;

    sprintf(interp->result, "%d", _result);
    return TCL_OK;
}


static int
_wrap_PDF_open_image_file(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
    PDF *p;
    const char *imagetype;
    int imagetype_len;
    const char *filename;
    int filename_len;
    const char *stringparam;
    int stringparam_len;
    int intparam;
    char *res;
    int volatile _result = -1;

    if (objc != 6)
    {
        PDF_WrongCommand(interp, "PDF_open_image_file p imagetype filename stringparam intparam ");
        return TCL_ERROR;
    }

    if ((res = Tcl_GetStringFromObj(objv[1], NULL)) == NULL)
    {
        PDF_NoPDFHandle(interp, "PDF_open_image_file");
        return TCL_ERROR;
    }

    if (SWIG_GetPtr(res, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle(interp, "PDF_open_image_file");
        Tcl_AppendResult(interp, res, (char *) NULL);
        return TCL_ERROR;
    }

    if ((imagetype = PDF_GetStringFromObj(p, interp, objv[2], PDF_BYTES, &imagetype_len)) == NULL)
    {
        PDF_WrongParameter(interp, "imagetype in PDF_open_image_file");
        return TCL_ERROR;
    }

    if ((filename = PDF_GetStringFromObj(p, interp, objv[3], PDF_BYTES, &filename_len)) == NULL)
    {
        PDF_WrongParameter(interp, "filename in PDF_open_image_file");
        return TCL_ERROR;
    }

    if ((stringparam = PDF_GetStringFromObj(p, interp, objv[4], PDF_BYTES, &stringparam_len)) == NULL)
    {
        PDF_WrongParameter(interp, "stringparam in PDF_open_image_file");
        return TCL_ERROR;
    }

    if (Tcl_GetIntFromObj(interp, objv[5], &intparam) != TCL_OK)
    {
        PDF_WrongParameter(interp, "intparam in PDF_open_image_file");
        return TCL_ERROR;
    }

    try { _result = (int) PDF_open_image_file(p, imagetype, filename, stringparam, intparam);
    } catch;

    sprintf(interp->result, "%d", _result);
    return TCL_OK;
}


static int
_wrap_PDF_open_pdi(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
    PDF *p;
    const char *filename;
    int filename_len;
    const char *optlist;
    int optlist_len;
    char *res;
    int volatile _result = -1;

    if (objc != 4 && objc != 5)  /* downward compatibility */
    {
        PDF_WrongCommand(interp, "PDF_open_pdi p filename optlist ");
        return TCL_ERROR;
    }

    if ((res = Tcl_GetStringFromObj(objv[1], NULL)) == NULL)
    {
        PDF_NoPDFHandle(interp, "PDF_open_pdi");
        return TCL_ERROR;
    }

    if (SWIG_GetPtr(res, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle(interp, "PDF_open_pdi");
        Tcl_AppendResult(interp, res, (char *) NULL);
        return TCL_ERROR;
    }

    if ((filename = PDF_GetStringFromObj(p, interp, objv[2], PDF_0UTF16, &filename_len)) == NULL)
    {
        PDF_WrongParameter(interp, "filename in PDF_open_pdi");
        return TCL_ERROR;
    }

    if ((optlist = PDF_GetStringFromObj(p, interp, objv[3], PDF_UTF8, &optlist_len)) == NULL)
    {
        PDF_WrongParameter(interp, "optlist in PDF_open_pdi");
        return TCL_ERROR;
    }

    try { _result = (int) PDF_open_pdi(p, filename, optlist, filename_len);
    } catch;

    sprintf(interp->result, "%d", _result);
    return TCL_OK;
}


static int
_wrap_PDF_open_pdi_page(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
    PDF *p;
    int doc;
    int pagenumber;
    const char *optlist;
    int optlist_len;
    char *res;
    int volatile _result = -1;

    if (objc != 5)
    {
        PDF_WrongCommand(interp, "PDF_open_pdi_page p doc pagenumber optlist ");
        return TCL_ERROR;
    }

    if ((res = Tcl_GetStringFromObj(objv[1], NULL)) == NULL)
    {
        PDF_NoPDFHandle(interp, "PDF_open_pdi_page");
        return TCL_ERROR;
    }

    if (SWIG_GetPtr(res, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle(interp, "PDF_open_pdi_page");
        Tcl_AppendResult(interp, res, (char *) NULL);
        return TCL_ERROR;
    }

    if (Tcl_GetIntFromObj(interp, objv[2], &doc) != TCL_OK)
    {
        PDF_WrongParameter(interp, "doc in PDF_open_pdi_page");
        return TCL_ERROR;
    }

    if (Tcl_GetIntFromObj(interp, objv[3], &pagenumber) != TCL_OK)
    {
        PDF_WrongParameter(interp, "pagenumber in PDF_open_pdi_page");
        return TCL_ERROR;
    }

    if ((optlist = PDF_GetStringFromObj(p, interp, objv[4], PDF_UTF8, &optlist_len)) == NULL)
    {
        PDF_WrongParameter(interp, "optlist in PDF_open_pdi_page");
        return TCL_ERROR;
    }

    try { _result = (int) PDF_open_pdi_page(p, doc, pagenumber, optlist);
    } catch;

    sprintf(interp->result, "%d", _result);
    return TCL_OK;
}


static int
_wrap_PDF_place_image(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
    PDF *p;
    int image;
    double x;
    double y;
    double scale;
    char *res;

    if (objc != 6)
    {
        PDF_WrongCommand(interp, "PDF_place_image p image x y scale ");
        return TCL_ERROR;
    }

    if ((res = Tcl_GetStringFromObj(objv[1], NULL)) == NULL)
    {
        PDF_NoPDFHandle(interp, "PDF_place_image");
        return TCL_ERROR;
    }

    if (SWIG_GetPtr(res, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle(interp, "PDF_place_image");
        Tcl_AppendResult(interp, res, (char *) NULL);
        return TCL_ERROR;
    }

    if (Tcl_GetIntFromObj(interp, objv[2], &image) != TCL_OK)
    {
        PDF_WrongParameter(interp, "image in PDF_place_image");
        return TCL_ERROR;
    }

    if (Tcl_GetDoubleFromObj(interp, objv[3], &x) != TCL_OK)
    {
        PDF_WrongParameter(interp, "x in PDF_place_image");
        return TCL_ERROR;
    }

    if (Tcl_GetDoubleFromObj(interp, objv[4], &y) != TCL_OK)
    {
        PDF_WrongParameter(interp, "y in PDF_place_image");
        return TCL_ERROR;
    }

    if (Tcl_GetDoubleFromObj(interp, objv[5], &scale) != TCL_OK)
    {
        PDF_WrongParameter(interp, "scale in PDF_place_image");
        return TCL_ERROR;
    }

    try { PDF_place_image(p, image, x, y, scale);
    } catch;

    return TCL_OK;
}


static int
_wrap_PDF_place_pdi_page(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
    PDF *p;
    int page;
    double x;
    double y;
    double sx;
    double sy;
    char *res;

    if (objc != 7)
    {
        PDF_WrongCommand(interp, "PDF_place_pdi_page p page x y sx sy ");
        return TCL_ERROR;
    }

    if ((res = Tcl_GetStringFromObj(objv[1], NULL)) == NULL)
    {
        PDF_NoPDFHandle(interp, "PDF_place_pdi_page");
        return TCL_ERROR;
    }

    if (SWIG_GetPtr(res, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle(interp, "PDF_place_pdi_page");
        Tcl_AppendResult(interp, res, (char *) NULL);
        return TCL_ERROR;
    }

    if (Tcl_GetIntFromObj(interp, objv[2], &page) != TCL_OK)
    {
        PDF_WrongParameter(interp, "page in PDF_place_pdi_page");
        return TCL_ERROR;
    }

    if (Tcl_GetDoubleFromObj(interp, objv[3], &x) != TCL_OK)
    {
        PDF_WrongParameter(interp, "x in PDF_place_pdi_page");
        return TCL_ERROR;
    }

    if (Tcl_GetDoubleFromObj(interp, objv[4], &y) != TCL_OK)
    {
        PDF_WrongParameter(interp, "y in PDF_place_pdi_page");
        return TCL_ERROR;
    }

    if (Tcl_GetDoubleFromObj(interp, objv[5], &sx) != TCL_OK)
    {
        PDF_WrongParameter(interp, "sx in PDF_place_pdi_page");
        return TCL_ERROR;
    }

    if (Tcl_GetDoubleFromObj(interp, objv[6], &sy) != TCL_OK)
    {
        PDF_WrongParameter(interp, "sy in PDF_place_pdi_page");
        return TCL_ERROR;
    }

    try { PDF_place_pdi_page(p, page, x, y, sx, sy);
    } catch;

    return TCL_OK;
}


static int
_wrap_PDF_process_pdi(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
    PDF *p;
    int doc;
    int page;
    const char *optlist;
    int optlist_len;
    char *res;
    int volatile _result = -1;

    if (objc != 5)
    {
        PDF_WrongCommand(interp, "PDF_process_pdi p doc page optlist ");
        return TCL_ERROR;
    }

    if ((res = Tcl_GetStringFromObj(objv[1], NULL)) == NULL)
    {
        PDF_NoPDFHandle(interp, "PDF_process_pdi");
        return TCL_ERROR;
    }

    if (SWIG_GetPtr(res, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle(interp, "PDF_process_pdi");
        Tcl_AppendResult(interp, res, (char *) NULL);
        return TCL_ERROR;
    }

    if (Tcl_GetIntFromObj(interp, objv[2], &doc) != TCL_OK)
    {
        PDF_WrongParameter(interp, "doc in PDF_process_pdi");
        return TCL_ERROR;
    }

    if (Tcl_GetIntFromObj(interp, objv[3], &page) != TCL_OK)
    {
        PDF_WrongParameter(interp, "page in PDF_process_pdi");
        return TCL_ERROR;
    }

    if ((optlist = PDF_GetStringFromObj(p, interp, objv[4], PDF_UTF8, &optlist_len)) == NULL)
    {
        PDF_WrongParameter(interp, "optlist in PDF_process_pdi");
        return TCL_ERROR;
    }

    try { _result = (int) PDF_process_pdi(p, doc, page, optlist);
    } catch;

    sprintf(interp->result, "%d", _result);
    return TCL_OK;
}


static int
_wrap_PDF_rect(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
    PDF *p;
    double x;
    double y;
    double width;
    double height;
    char *res;

    if (objc != 6)
    {
        PDF_WrongCommand(interp, "PDF_rect p x y width height ");
        return TCL_ERROR;
    }

    if ((res = Tcl_GetStringFromObj(objv[1], NULL)) == NULL)
    {
        PDF_NoPDFHandle(interp, "PDF_rect");
        return TCL_ERROR;
    }

    if (SWIG_GetPtr(res, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle(interp, "PDF_rect");
        Tcl_AppendResult(interp, res, (char *) NULL);
        return TCL_ERROR;
    }

    if (Tcl_GetDoubleFromObj(interp, objv[2], &x) != TCL_OK)
    {
        PDF_WrongParameter(interp, "x in PDF_rect");
        return TCL_ERROR;
    }

    if (Tcl_GetDoubleFromObj(interp, objv[3], &y) != TCL_OK)
    {
        PDF_WrongParameter(interp, "y in PDF_rect");
        return TCL_ERROR;
    }

    if (Tcl_GetDoubleFromObj(interp, objv[4], &width) != TCL_OK)
    {
        PDF_WrongParameter(interp, "width in PDF_rect");
        return TCL_ERROR;
    }

    if (Tcl_GetDoubleFromObj(interp, objv[5], &height) != TCL_OK)
    {
        PDF_WrongParameter(interp, "height in PDF_rect");
        return TCL_ERROR;
    }

    try { PDF_rect(p, x, y, width, height);
    } catch;

    return TCL_OK;
}


static int
_wrap_PDF_restore(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
    PDF *p;
    char *res;

    if (objc != 2)
    {
        PDF_WrongCommand(interp, "PDF_restore p ");
        return TCL_ERROR;
    }

    if ((res = Tcl_GetStringFromObj(objv[1], NULL)) == NULL)
    {
        PDF_NoPDFHandle(interp, "PDF_restore");
        return TCL_ERROR;
    }

    if (SWIG_GetPtr(res, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle(interp, "PDF_restore");
        Tcl_AppendResult(interp, res, (char *) NULL);
        return TCL_ERROR;
    }

    try { PDF_restore(p);
    } catch;

    return TCL_OK;
}


static int
_wrap_PDF_resume_page(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
    PDF *p;
    const char *optlist;
    int optlist_len;
    char *res;

    if (objc != 3)
    {
        PDF_WrongCommand(interp, "PDF_resume_page p optlist ");
        return TCL_ERROR;
    }

    if ((res = Tcl_GetStringFromObj(objv[1], NULL)) == NULL)
    {
        PDF_NoPDFHandle(interp, "PDF_resume_page");
        return TCL_ERROR;
    }

    if (SWIG_GetPtr(res, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle(interp, "PDF_resume_page");
        Tcl_AppendResult(interp, res, (char *) NULL);
        return TCL_ERROR;
    }

    if ((optlist = PDF_GetStringFromObj(p, interp, objv[2], PDF_UTF8, &optlist_len)) == NULL)
    {
        PDF_WrongParameter(interp, "optlist in PDF_resume_page");
        return TCL_ERROR;
    }

    try { PDF_resume_page(p, optlist);
    } catch;

    return TCL_OK;
}


static int
_wrap_PDF_rotate(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
    PDF *p;
    double phi;
    char *res;

    if (objc != 3)
    {
        PDF_WrongCommand(interp, "PDF_rotate p phi ");
        return TCL_ERROR;
    }

    if ((res = Tcl_GetStringFromObj(objv[1], NULL)) == NULL)
    {
        PDF_NoPDFHandle(interp, "PDF_rotate");
        return TCL_ERROR;
    }

    if (SWIG_GetPtr(res, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle(interp, "PDF_rotate");
        Tcl_AppendResult(interp, res, (char *) NULL);
        return TCL_ERROR;
    }

    if (Tcl_GetDoubleFromObj(interp, objv[2], &phi) != TCL_OK)
    {
        PDF_WrongParameter(interp, "phi in PDF_rotate");
        return TCL_ERROR;
    }

    try { PDF_rotate(p, phi);
    } catch;

    return TCL_OK;
}


static int
_wrap_PDF_save(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
    PDF *p;
    char *res;

    if (objc != 2)
    {
        PDF_WrongCommand(interp, "PDF_save p ");
        return TCL_ERROR;
    }

    if ((res = Tcl_GetStringFromObj(objv[1], NULL)) == NULL)
    {
        PDF_NoPDFHandle(interp, "PDF_save");
        return TCL_ERROR;
    }

    if (SWIG_GetPtr(res, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle(interp, "PDF_save");
        Tcl_AppendResult(interp, res, (char *) NULL);
        return TCL_ERROR;
    }

    try { PDF_save(p);
    } catch;

    return TCL_OK;
}


static int
_wrap_PDF_scale(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
    PDF *p;
    double sx;
    double sy;
    char *res;

    if (objc != 4)
    {
        PDF_WrongCommand(interp, "PDF_scale p sx sy ");
        return TCL_ERROR;
    }

    if ((res = Tcl_GetStringFromObj(objv[1], NULL)) == NULL)
    {
        PDF_NoPDFHandle(interp, "PDF_scale");
        return TCL_ERROR;
    }

    if (SWIG_GetPtr(res, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle(interp, "PDF_scale");
        Tcl_AppendResult(interp, res, (char *) NULL);
        return TCL_ERROR;
    }

    if (Tcl_GetDoubleFromObj(interp, objv[2], &sx) != TCL_OK)
    {
        PDF_WrongParameter(interp, "sx in PDF_scale");
        return TCL_ERROR;
    }

    if (Tcl_GetDoubleFromObj(interp, objv[3], &sy) != TCL_OK)
    {
        PDF_WrongParameter(interp, "sy in PDF_scale");
        return TCL_ERROR;
    }

    try { PDF_scale(p, sx, sy);
    } catch;

    return TCL_OK;
}


static int
_wrap_PDF_set_border_color(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
    PDF *p;
    double red;
    double green;
    double blue;
    char *res;

    if (objc != 5)
    {
        PDF_WrongCommand(interp, "PDF_set_border_color p red green blue ");
        return TCL_ERROR;
    }

    if ((res = Tcl_GetStringFromObj(objv[1], NULL)) == NULL)
    {
        PDF_NoPDFHandle(interp, "PDF_set_border_color");
        return TCL_ERROR;
    }

    if (SWIG_GetPtr(res, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle(interp, "PDF_set_border_color");
        Tcl_AppendResult(interp, res, (char *) NULL);
        return TCL_ERROR;
    }

    if (Tcl_GetDoubleFromObj(interp, objv[2], &red) != TCL_OK)
    {
        PDF_WrongParameter(interp, "red in PDF_set_border_color");
        return TCL_ERROR;
    }

    if (Tcl_GetDoubleFromObj(interp, objv[3], &green) != TCL_OK)
    {
        PDF_WrongParameter(interp, "green in PDF_set_border_color");
        return TCL_ERROR;
    }

    if (Tcl_GetDoubleFromObj(interp, objv[4], &blue) != TCL_OK)
    {
        PDF_WrongParameter(interp, "blue in PDF_set_border_color");
        return TCL_ERROR;
    }

    try { PDF_set_border_color(p, red, green, blue);
    } catch;

    return TCL_OK;
}


static int
_wrap_PDF_set_border_dash(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
    PDF *p;
    double b;
    double w;
    char *res;

    if (objc != 4)
    {
        PDF_WrongCommand(interp, "PDF_set_border_dash p b w ");
        return TCL_ERROR;
    }

    if ((res = Tcl_GetStringFromObj(objv[1], NULL)) == NULL)
    {
        PDF_NoPDFHandle(interp, "PDF_set_border_dash");
        return TCL_ERROR;
    }

    if (SWIG_GetPtr(res, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle(interp, "PDF_set_border_dash");
        Tcl_AppendResult(interp, res, (char *) NULL);
        return TCL_ERROR;
    }

    if (Tcl_GetDoubleFromObj(interp, objv[2], &b) != TCL_OK)
    {
        PDF_WrongParameter(interp, "b in PDF_set_border_dash");
        return TCL_ERROR;
    }

    if (Tcl_GetDoubleFromObj(interp, objv[3], &w) != TCL_OK)
    {
        PDF_WrongParameter(interp, "w in PDF_set_border_dash");
        return TCL_ERROR;
    }

    try { PDF_set_border_dash(p, b, w);
    } catch;

    return TCL_OK;
}


static int
_wrap_PDF_set_border_style(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
    PDF *p;
    const char *style;
    int style_len;
    double width;
    char *res;

    if (objc != 4)
    {
        PDF_WrongCommand(interp, "PDF_set_border_style p style width ");
        return TCL_ERROR;
    }

    if ((res = Tcl_GetStringFromObj(objv[1], NULL)) == NULL)
    {
        PDF_NoPDFHandle(interp, "PDF_set_border_style");
        return TCL_ERROR;
    }

    if (SWIG_GetPtr(res, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle(interp, "PDF_set_border_style");
        Tcl_AppendResult(interp, res, (char *) NULL);
        return TCL_ERROR;
    }

    if ((style = PDF_GetStringFromObj(p, interp, objv[2], PDF_BYTES, &style_len)) == NULL)
    {
        PDF_WrongParameter(interp, "style in PDF_set_border_style");
        return TCL_ERROR;
    }

    if (Tcl_GetDoubleFromObj(interp, objv[3], &width) != TCL_OK)
    {
        PDF_WrongParameter(interp, "width in PDF_set_border_style");
        return TCL_ERROR;
    }

    try { PDF_set_border_style(p, style, width);
    } catch;

    return TCL_OK;
}


static int
_wrap_PDF_set_gstate(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
    PDF *p;
    int gstate;
    char *res;

    if (objc != 3)
    {
        PDF_WrongCommand(interp, "PDF_set_gstate p gstate ");
        return TCL_ERROR;
    }

    if ((res = Tcl_GetStringFromObj(objv[1], NULL)) == NULL)
    {
        PDF_NoPDFHandle(interp, "PDF_set_gstate");
        return TCL_ERROR;
    }

    if (SWIG_GetPtr(res, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle(interp, "PDF_set_gstate");
        Tcl_AppendResult(interp, res, (char *) NULL);
        return TCL_ERROR;
    }

    if (Tcl_GetIntFromObj(interp, objv[2], &gstate) != TCL_OK)
    {
        PDF_WrongParameter(interp, "gstate in PDF_set_gstate");
        return TCL_ERROR;
    }

    try { PDF_set_gstate(p, gstate);
    } catch;

    return TCL_OK;
}


static int
_wrap_PDF_set_info(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
    PDF *p;
    const char *key;
    int key_len;
    const char *value;
    int value_len;
    char *res;

    if (objc != 4)
    {
        PDF_WrongCommand(interp, "PDF_set_info p key value ");
        return TCL_ERROR;
    }

    if ((res = Tcl_GetStringFromObj(objv[1], NULL)) == NULL)
    {
        PDF_NoPDFHandle(interp, "PDF_set_info");
        return TCL_ERROR;
    }

    if (SWIG_GetPtr(res, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle(interp, "PDF_set_info");
        Tcl_AppendResult(interp, res, (char *) NULL);
        return TCL_ERROR;
    }

    if ((key = PDF_GetStringFromObj(p, interp, objv[2], PDF_UTF8, &key_len)) == NULL)
    {
        PDF_WrongParameter(interp, "key in PDF_set_info");
        return TCL_ERROR;
    }

    if ((value = PDF_GetStringFromObj(p, interp, objv[3], PDF_UTF16, &value_len)) == NULL)
    {
        PDF_WrongParameter(interp, "value in PDF_set_info");
        return TCL_ERROR;
    }

    try { PDF_set_info2(p, key, value, value_len);
    } catch;

    return TCL_OK;
}


static int
_wrap_PDF_set_layer_dependency(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
    PDF *p;
    const char *type;
    int type_len;
    const char *optlist;
    int optlist_len;
    char *res;

    if (objc != 4)
    {
        PDF_WrongCommand(interp, "PDF_set_layer_dependency p type optlist ");
        return TCL_ERROR;
    }

    if ((res = Tcl_GetStringFromObj(objv[1], NULL)) == NULL)
    {
        PDF_NoPDFHandle(interp, "PDF_set_layer_dependency");
        return TCL_ERROR;
    }

    if (SWIG_GetPtr(res, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle(interp, "PDF_set_layer_dependency");
        Tcl_AppendResult(interp, res, (char *) NULL);
        return TCL_ERROR;
    }

    if ((type = PDF_GetStringFromObj(p, interp, objv[2], PDF_BYTES, &type_len)) == NULL)
    {
        PDF_WrongParameter(interp, "type in PDF_set_layer_dependency");
        return TCL_ERROR;
    }

    if ((optlist = PDF_GetStringFromObj(p, interp, objv[3], PDF_UTF8, &optlist_len)) == NULL)
    {
        PDF_WrongParameter(interp, "optlist in PDF_set_layer_dependency");
        return TCL_ERROR;
    }

    try { PDF_set_layer_dependency(p, type, optlist);
    } catch;

    return TCL_OK;
}


static int
_wrap_PDF_set_parameter(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
    PDF *p;
    const char *key;
    int key_len;
    const char *value;
    int value_len;
    char *res;

    if (objc != 4)
    {
        PDF_WrongCommand(interp, "PDF_set_parameter p key value ");
        return TCL_ERROR;
    }

    if ((res = Tcl_GetStringFromObj(objv[1], NULL)) == NULL)
    {
        PDF_NoPDFHandle(interp, "PDF_set_parameter");
        return TCL_ERROR;
    }

    if (SWIG_GetPtr(res, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle(interp, "PDF_set_parameter");
        Tcl_AppendResult(interp, res, (char *) NULL);
        return TCL_ERROR;
    }

    if ((key = PDF_GetStringFromObj(p, interp, objv[2], PDF_BYTES, &key_len)) == NULL)
    {
        PDF_WrongParameter(interp, "key in PDF_set_parameter");
        return TCL_ERROR;
    }

    if ((value = PDF_GetStringFromObj(p, interp, objv[3], PDF_UTF8, &value_len)) == NULL)
    {
        PDF_WrongParameter(interp, "value in PDF_set_parameter");
        return TCL_ERROR;
    }

    try { PDF_set_parameter(p, key, value);
    } catch;

    return TCL_OK;
}


static int
_wrap_PDF_set_text_pos(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
    PDF *p;
    double x;
    double y;
    char *res;

    if (objc != 4)
    {
        PDF_WrongCommand(interp, "PDF_set_text_pos p x y ");
        return TCL_ERROR;
    }

    if ((res = Tcl_GetStringFromObj(objv[1], NULL)) == NULL)
    {
        PDF_NoPDFHandle(interp, "PDF_set_text_pos");
        return TCL_ERROR;
    }

    if (SWIG_GetPtr(res, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle(interp, "PDF_set_text_pos");
        Tcl_AppendResult(interp, res, (char *) NULL);
        return TCL_ERROR;
    }

    if (Tcl_GetDoubleFromObj(interp, objv[2], &x) != TCL_OK)
    {
        PDF_WrongParameter(interp, "x in PDF_set_text_pos");
        return TCL_ERROR;
    }

    if (Tcl_GetDoubleFromObj(interp, objv[3], &y) != TCL_OK)
    {
        PDF_WrongParameter(interp, "y in PDF_set_text_pos");
        return TCL_ERROR;
    }

    try { PDF_set_text_pos(p, x, y);
    } catch;

    return TCL_OK;
}


static int
_wrap_PDF_set_value(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
    PDF *p;
    const char *key;
    int key_len;
    double value;
    char *res;

    if (objc != 4)
    {
        PDF_WrongCommand(interp, "PDF_set_value p key value ");
        return TCL_ERROR;
    }

    if ((res = Tcl_GetStringFromObj(objv[1], NULL)) == NULL)
    {
        PDF_NoPDFHandle(interp, "PDF_set_value");
        return TCL_ERROR;
    }

    if (SWIG_GetPtr(res, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle(interp, "PDF_set_value");
        Tcl_AppendResult(interp, res, (char *) NULL);
        return TCL_ERROR;
    }

    if ((key = PDF_GetStringFromObj(p, interp, objv[2], PDF_BYTES, &key_len)) == NULL)
    {
        PDF_WrongParameter(interp, "key in PDF_set_value");
        return TCL_ERROR;
    }

    if (Tcl_GetDoubleFromObj(interp, objv[3], &value) != TCL_OK)
    {
        PDF_WrongParameter(interp, "value in PDF_set_value");
        return TCL_ERROR;
    }

    try { PDF_set_value(p, key, value);
    } catch;

    return TCL_OK;
}


static int
_wrap_PDF_setcolor(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
    PDF *p;
    const char *fstype;
    int fstype_len;
    const char *colorspace;
    int colorspace_len;
    double c1;
    double c2;
    double c3;
    double c4;
    char *res;

    if (objc != 8)
    {
        PDF_WrongCommand(interp, "PDF_setcolor p fstype colorspace c1 c2 c3 c4 ");
        return TCL_ERROR;
    }

    if ((res = Tcl_GetStringFromObj(objv[1], NULL)) == NULL)
    {
        PDF_NoPDFHandle(interp, "PDF_setcolor");
        return TCL_ERROR;
    }

    if (SWIG_GetPtr(res, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle(interp, "PDF_setcolor");
        Tcl_AppendResult(interp, res, (char *) NULL);
        return TCL_ERROR;
    }

    if ((fstype = PDF_GetStringFromObj(p, interp, objv[2], PDF_BYTES, &fstype_len)) == NULL)
    {
        PDF_WrongParameter(interp, "fstype in PDF_setcolor");
        return TCL_ERROR;
    }

    if ((colorspace = PDF_GetStringFromObj(p, interp, objv[3], PDF_BYTES, &colorspace_len)) == NULL)
    {
        PDF_WrongParameter(interp, "colorspace in PDF_setcolor");
        return TCL_ERROR;
    }

    if (Tcl_GetDoubleFromObj(interp, objv[4], &c1) != TCL_OK)
    {
        PDF_WrongParameter(interp, "c1 in PDF_setcolor");
        return TCL_ERROR;
    }

    if (Tcl_GetDoubleFromObj(interp, objv[5], &c2) != TCL_OK)
    {
        PDF_WrongParameter(interp, "c2 in PDF_setcolor");
        return TCL_ERROR;
    }

    if (Tcl_GetDoubleFromObj(interp, objv[6], &c3) != TCL_OK)
    {
        PDF_WrongParameter(interp, "c3 in PDF_setcolor");
        return TCL_ERROR;
    }

    if (Tcl_GetDoubleFromObj(interp, objv[7], &c4) != TCL_OK)
    {
        PDF_WrongParameter(interp, "c4 in PDF_setcolor");
        return TCL_ERROR;
    }

    try { PDF_setcolor(p, fstype, colorspace, c1, c2, c3, c4);
    } catch;

    return TCL_OK;
}


static int
_wrap_PDF_setdash(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
    PDF *p;
    double b;
    double w;
    char *res;

    if (objc != 4)
    {
        PDF_WrongCommand(interp, "PDF_setdash p b w ");
        return TCL_ERROR;
    }

    if ((res = Tcl_GetStringFromObj(objv[1], NULL)) == NULL)
    {
        PDF_NoPDFHandle(interp, "PDF_setdash");
        return TCL_ERROR;
    }

    if (SWIG_GetPtr(res, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle(interp, "PDF_setdash");
        Tcl_AppendResult(interp, res, (char *) NULL);
        return TCL_ERROR;
    }

    if (Tcl_GetDoubleFromObj(interp, objv[2], &b) != TCL_OK)
    {
        PDF_WrongParameter(interp, "b in PDF_setdash");
        return TCL_ERROR;
    }

    if (Tcl_GetDoubleFromObj(interp, objv[3], &w) != TCL_OK)
    {
        PDF_WrongParameter(interp, "w in PDF_setdash");
        return TCL_ERROR;
    }

    try { PDF_setdash(p, b, w);
    } catch;

    return TCL_OK;
}


static int
_wrap_PDF_setdashpattern(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
    PDF *p;
    const char *optlist;
    int optlist_len;
    char *res;

    if (objc != 3)
    {
        PDF_WrongCommand(interp, "PDF_setdashpattern p optlist ");
        return TCL_ERROR;
    }

    if ((res = Tcl_GetStringFromObj(objv[1], NULL)) == NULL)
    {
        PDF_NoPDFHandle(interp, "PDF_setdashpattern");
        return TCL_ERROR;
    }

    if (SWIG_GetPtr(res, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle(interp, "PDF_setdashpattern");
        Tcl_AppendResult(interp, res, (char *) NULL);
        return TCL_ERROR;
    }

    if ((optlist = PDF_GetStringFromObj(p, interp, objv[2], PDF_UTF8, &optlist_len)) == NULL)
    {
        PDF_WrongParameter(interp, "optlist in PDF_setdashpattern");
        return TCL_ERROR;
    }

    try { PDF_setdashpattern(p, optlist);
    } catch;

    return TCL_OK;
}


static int
_wrap_PDF_setflat(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
    PDF *p;
    double flatness;
    char *res;

    if (objc != 3)
    {
        PDF_WrongCommand(interp, "PDF_setflat p flatness ");
        return TCL_ERROR;
    }

    if ((res = Tcl_GetStringFromObj(objv[1], NULL)) == NULL)
    {
        PDF_NoPDFHandle(interp, "PDF_setflat");
        return TCL_ERROR;
    }

    if (SWIG_GetPtr(res, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle(interp, "PDF_setflat");
        Tcl_AppendResult(interp, res, (char *) NULL);
        return TCL_ERROR;
    }

    if (Tcl_GetDoubleFromObj(interp, objv[2], &flatness) != TCL_OK)
    {
        PDF_WrongParameter(interp, "flatness in PDF_setflat");
        return TCL_ERROR;
    }

    try { PDF_setflat(p, flatness);
    } catch;

    return TCL_OK;
}


static int
_wrap_PDF_setfont(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
    PDF *p;
    int font;
    double fontsize;
    char *res;

    if (objc != 4)
    {
        PDF_WrongCommand(interp, "PDF_setfont p font fontsize ");
        return TCL_ERROR;
    }

    if ((res = Tcl_GetStringFromObj(objv[1], NULL)) == NULL)
    {
        PDF_NoPDFHandle(interp, "PDF_setfont");
        return TCL_ERROR;
    }

    if (SWIG_GetPtr(res, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle(interp, "PDF_setfont");
        Tcl_AppendResult(interp, res, (char *) NULL);
        return TCL_ERROR;
    }

    if (Tcl_GetIntFromObj(interp, objv[2], &font) != TCL_OK)
    {
        PDF_WrongParameter(interp, "font in PDF_setfont");
        return TCL_ERROR;
    }

    if (Tcl_GetDoubleFromObj(interp, objv[3], &fontsize) != TCL_OK)
    {
        PDF_WrongParameter(interp, "fontsize in PDF_setfont");
        return TCL_ERROR;
    }

    try { PDF_setfont(p, font, fontsize);
    } catch;

    return TCL_OK;
}


static int
_wrap_PDF_setgray(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
    PDF *p;
    double gray;
    char *res;

    if (objc != 3)
    {
        PDF_WrongCommand(interp, "PDF_setgray p gray ");
        return TCL_ERROR;
    }

    if ((res = Tcl_GetStringFromObj(objv[1], NULL)) == NULL)
    {
        PDF_NoPDFHandle(interp, "PDF_setgray");
        return TCL_ERROR;
    }

    if (SWIG_GetPtr(res, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle(interp, "PDF_setgray");
        Tcl_AppendResult(interp, res, (char *) NULL);
        return TCL_ERROR;
    }

    if (Tcl_GetDoubleFromObj(interp, objv[2], &gray) != TCL_OK)
    {
        PDF_WrongParameter(interp, "gray in PDF_setgray");
        return TCL_ERROR;
    }

    try { PDF_setgray(p, gray);
    } catch;

    return TCL_OK;
}


static int
_wrap_PDF_setgray_stroke(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
    PDF *p;
    double gray;
    char *res;

    if (objc != 3)
    {
        PDF_WrongCommand(interp, "PDF_setgray_stroke p gray ");
        return TCL_ERROR;
    }

    if ((res = Tcl_GetStringFromObj(objv[1], NULL)) == NULL)
    {
        PDF_NoPDFHandle(interp, "PDF_setgray_stroke");
        return TCL_ERROR;
    }

    if (SWIG_GetPtr(res, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle(interp, "PDF_setgray_stroke");
        Tcl_AppendResult(interp, res, (char *) NULL);
        return TCL_ERROR;
    }

    if (Tcl_GetDoubleFromObj(interp, objv[2], &gray) != TCL_OK)
    {
        PDF_WrongParameter(interp, "gray in PDF_setgray_stroke");
        return TCL_ERROR;
    }

    try { PDF_setgray_stroke(p, gray);
    } catch;

    return TCL_OK;
}


static int
_wrap_PDF_setgray_fill(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
    PDF *p;
    double gray;
    char *res;

    if (objc != 3)
    {
        PDF_WrongCommand(interp, "PDF_setgray_fill p gray ");
        return TCL_ERROR;
    }

    if ((res = Tcl_GetStringFromObj(objv[1], NULL)) == NULL)
    {
        PDF_NoPDFHandle(interp, "PDF_setgray_fill");
        return TCL_ERROR;
    }

    if (SWIG_GetPtr(res, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle(interp, "PDF_setgray_fill");
        Tcl_AppendResult(interp, res, (char *) NULL);
        return TCL_ERROR;
    }

    if (Tcl_GetDoubleFromObj(interp, objv[2], &gray) != TCL_OK)
    {
        PDF_WrongParameter(interp, "gray in PDF_setgray_fill");
        return TCL_ERROR;
    }

    try { PDF_setgray_fill(p, gray);
    } catch;

    return TCL_OK;
}


static int
_wrap_PDF_setlinecap(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
    PDF *p;
    int linecap;
    char *res;

    if (objc != 3)
    {
        PDF_WrongCommand(interp, "PDF_setlinecap p linecap ");
        return TCL_ERROR;
    }

    if ((res = Tcl_GetStringFromObj(objv[1], NULL)) == NULL)
    {
        PDF_NoPDFHandle(interp, "PDF_setlinecap");
        return TCL_ERROR;
    }

    if (SWIG_GetPtr(res, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle(interp, "PDF_setlinecap");
        Tcl_AppendResult(interp, res, (char *) NULL);
        return TCL_ERROR;
    }

    if (Tcl_GetIntFromObj(interp, objv[2], &linecap) != TCL_OK)
    {
        PDF_WrongParameter(interp, "linecap in PDF_setlinecap");
        return TCL_ERROR;
    }

    try { PDF_setlinecap(p, linecap);
    } catch;

    return TCL_OK;
}


static int
_wrap_PDF_setlinejoin(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
    PDF *p;
    int linejoin;
    char *res;

    if (objc != 3)
    {
        PDF_WrongCommand(interp, "PDF_setlinejoin p linejoin ");
        return TCL_ERROR;
    }

    if ((res = Tcl_GetStringFromObj(objv[1], NULL)) == NULL)
    {
        PDF_NoPDFHandle(interp, "PDF_setlinejoin");
        return TCL_ERROR;
    }

    if (SWIG_GetPtr(res, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle(interp, "PDF_setlinejoin");
        Tcl_AppendResult(interp, res, (char *) NULL);
        return TCL_ERROR;
    }

    if (Tcl_GetIntFromObj(interp, objv[2], &linejoin) != TCL_OK)
    {
        PDF_WrongParameter(interp, "linejoin in PDF_setlinejoin");
        return TCL_ERROR;
    }

    try { PDF_setlinejoin(p, linejoin);
    } catch;

    return TCL_OK;
}


static int
_wrap_PDF_setlinewidth(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
    PDF *p;
    double width;
    char *res;

    if (objc != 3)
    {
        PDF_WrongCommand(interp, "PDF_setlinewidth p width ");
        return TCL_ERROR;
    }

    if ((res = Tcl_GetStringFromObj(objv[1], NULL)) == NULL)
    {
        PDF_NoPDFHandle(interp, "PDF_setlinewidth");
        return TCL_ERROR;
    }

    if (SWIG_GetPtr(res, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle(interp, "PDF_setlinewidth");
        Tcl_AppendResult(interp, res, (char *) NULL);
        return TCL_ERROR;
    }

    if (Tcl_GetDoubleFromObj(interp, objv[2], &width) != TCL_OK)
    {
        PDF_WrongParameter(interp, "width in PDF_setlinewidth");
        return TCL_ERROR;
    }

    try { PDF_setlinewidth(p, width);
    } catch;

    return TCL_OK;
}


static int
_wrap_PDF_setmatrix(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
    PDF *p;
    double a;
    double b;
    double c;
    double d;
    double e;
    double f;
    char *res;

    if (objc != 8)
    {
        PDF_WrongCommand(interp, "PDF_setmatrix p a b c d e f ");
        return TCL_ERROR;
    }

    if ((res = Tcl_GetStringFromObj(objv[1], NULL)) == NULL)
    {
        PDF_NoPDFHandle(interp, "PDF_setmatrix");
        return TCL_ERROR;
    }

    if (SWIG_GetPtr(res, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle(interp, "PDF_setmatrix");
        Tcl_AppendResult(interp, res, (char *) NULL);
        return TCL_ERROR;
    }

    if (Tcl_GetDoubleFromObj(interp, objv[2], &a) != TCL_OK)
    {
        PDF_WrongParameter(interp, "a in PDF_setmatrix");
        return TCL_ERROR;
    }

    if (Tcl_GetDoubleFromObj(interp, objv[3], &b) != TCL_OK)
    {
        PDF_WrongParameter(interp, "b in PDF_setmatrix");
        return TCL_ERROR;
    }

    if (Tcl_GetDoubleFromObj(interp, objv[4], &c) != TCL_OK)
    {
        PDF_WrongParameter(interp, "c in PDF_setmatrix");
        return TCL_ERROR;
    }

    if (Tcl_GetDoubleFromObj(interp, objv[5], &d) != TCL_OK)
    {
        PDF_WrongParameter(interp, "d in PDF_setmatrix");
        return TCL_ERROR;
    }

    if (Tcl_GetDoubleFromObj(interp, objv[6], &e) != TCL_OK)
    {
        PDF_WrongParameter(interp, "e in PDF_setmatrix");
        return TCL_ERROR;
    }

    if (Tcl_GetDoubleFromObj(interp, objv[7], &f) != TCL_OK)
    {
        PDF_WrongParameter(interp, "f in PDF_setmatrix");
        return TCL_ERROR;
    }

    try { PDF_setmatrix(p, a, b, c, d, e, f);
    } catch;

    return TCL_OK;
}


static int
_wrap_PDF_setmiterlimit(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
    PDF *p;
    double miter;
    char *res;

    if (objc != 3)
    {
        PDF_WrongCommand(interp, "PDF_setmiterlimit p miter ");
        return TCL_ERROR;
    }

    if ((res = Tcl_GetStringFromObj(objv[1], NULL)) == NULL)
    {
        PDF_NoPDFHandle(interp, "PDF_setmiterlimit");
        return TCL_ERROR;
    }

    if (SWIG_GetPtr(res, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle(interp, "PDF_setmiterlimit");
        Tcl_AppendResult(interp, res, (char *) NULL);
        return TCL_ERROR;
    }

    if (Tcl_GetDoubleFromObj(interp, objv[2], &miter) != TCL_OK)
    {
        PDF_WrongParameter(interp, "miter in PDF_setmiterlimit");
        return TCL_ERROR;
    }

    try { PDF_setmiterlimit(p, miter);
    } catch;

    return TCL_OK;
}


static int
_wrap_PDF_setpolydash(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
    PDF *p;
    float *dasharray;
    int length;
    char *res;
    float *carray;
    double dval;
    int i;
    Tcl_Obj *val;

    if (objc != 3)
    {
        PDF_WrongCommand(interp, "PDF_setpolydash p dasharray ");
        return TCL_ERROR;
    }

    if ((res = Tcl_GetStringFromObj(objv[1], NULL)) == NULL)
    {
        PDF_NoPDFHandle(interp, "PDF_setpolydash");
        return TCL_ERROR;
    }

    if (SWIG_GetPtr(res, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle(interp, "PDF_setpolydash");
        Tcl_AppendResult(interp, res, (char *) NULL);
        return TCL_ERROR;
    }

    if (Tcl_ListObjLength(interp, objv[2], &length) != TCL_OK) {
        PDF_WrongParameter(interp, "length in PDF_setpolydash");
        return TCL_ERROR;
    }

    carray = (float *) malloc(sizeof(float) * length);
    if (carray == NULL)
    {
        Tcl_SetResult(interp, "Out of memory in PDF_setpolydash", TCL_STATIC);
        return TCL_ERROR;
    }

    for (i = 0; i < length; i++)
    {
        if (Tcl_ListObjIndex(interp, objv[2], i, &val) != TCL_OK ||
            Tcl_GetDoubleFromObj(interp, val, &dval) != TCL_OK)
        {
            PDF_WrongParameter(interp, "array value in PDF_setpolydash");
            return TCL_ERROR;
        }
        carray[i] = (float) dval;
    }

    try { PDF_setpolydash(p, carray, length);
    } catch;

    free(carray);

    return TCL_OK;
}


static int
_wrap_PDF_setrgbcolor(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
    PDF *p;
    double red;
    double green;
    double blue;
    char *res;

    if (objc != 5)
    {
        PDF_WrongCommand(interp, "PDF_setrgbcolor p red green blue ");
        return TCL_ERROR;
    }

    if ((res = Tcl_GetStringFromObj(objv[1], NULL)) == NULL)
    {
        PDF_NoPDFHandle(interp, "PDF_setrgbcolor");
        return TCL_ERROR;
    }

    if (SWIG_GetPtr(res, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle(interp, "PDF_setrgbcolor");
        Tcl_AppendResult(interp, res, (char *) NULL);
        return TCL_ERROR;
    }

    if (Tcl_GetDoubleFromObj(interp, objv[2], &red) != TCL_OK)
    {
        PDF_WrongParameter(interp, "red in PDF_setrgbcolor");
        return TCL_ERROR;
    }

    if (Tcl_GetDoubleFromObj(interp, objv[3], &green) != TCL_OK)
    {
        PDF_WrongParameter(interp, "green in PDF_setrgbcolor");
        return TCL_ERROR;
    }

    if (Tcl_GetDoubleFromObj(interp, objv[4], &blue) != TCL_OK)
    {
        PDF_WrongParameter(interp, "blue in PDF_setrgbcolor");
        return TCL_ERROR;
    }

    try { PDF_setrgbcolor(p, red, green, blue);
    } catch;

    return TCL_OK;
}


static int
_wrap_PDF_setrgbcolor_fill(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
    PDF *p;
    double red;
    double green;
    double blue;
    char *res;

    if (objc != 5)
    {
        PDF_WrongCommand(interp, "PDF_setrgbcolor_fill p red green blue ");
        return TCL_ERROR;
    }

    if ((res = Tcl_GetStringFromObj(objv[1], NULL)) == NULL)
    {
        PDF_NoPDFHandle(interp, "PDF_setrgbcolor_fill");
        return TCL_ERROR;
    }

    if (SWIG_GetPtr(res, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle(interp, "PDF_setrgbcolor_fill");
        Tcl_AppendResult(interp, res, (char *) NULL);
        return TCL_ERROR;
    }

    if (Tcl_GetDoubleFromObj(interp, objv[2], &red) != TCL_OK)
    {
        PDF_WrongParameter(interp, "red in PDF_setrgbcolor_fill");
        return TCL_ERROR;
    }

    if (Tcl_GetDoubleFromObj(interp, objv[3], &green) != TCL_OK)
    {
        PDF_WrongParameter(interp, "green in PDF_setrgbcolor_fill");
        return TCL_ERROR;
    }

    if (Tcl_GetDoubleFromObj(interp, objv[4], &blue) != TCL_OK)
    {
        PDF_WrongParameter(interp, "blue in PDF_setrgbcolor_fill");
        return TCL_ERROR;
    }

    try { PDF_setrgbcolor_fill(p, red, green, blue);
    } catch;

    return TCL_OK;
}


static int
_wrap_PDF_setrgbcolor_stroke(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
    PDF *p;
    double red;
    double green;
    double blue;
    char *res;

    if (objc != 5)
    {
        PDF_WrongCommand(interp, "PDF_setrgbcolor_stroke p red green blue ");
        return TCL_ERROR;
    }

    if ((res = Tcl_GetStringFromObj(objv[1], NULL)) == NULL)
    {
        PDF_NoPDFHandle(interp, "PDF_setrgbcolor_stroke");
        return TCL_ERROR;
    }

    if (SWIG_GetPtr(res, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle(interp, "PDF_setrgbcolor_stroke");
        Tcl_AppendResult(interp, res, (char *) NULL);
        return TCL_ERROR;
    }

    if (Tcl_GetDoubleFromObj(interp, objv[2], &red) != TCL_OK)
    {
        PDF_WrongParameter(interp, "red in PDF_setrgbcolor_stroke");
        return TCL_ERROR;
    }

    if (Tcl_GetDoubleFromObj(interp, objv[3], &green) != TCL_OK)
    {
        PDF_WrongParameter(interp, "green in PDF_setrgbcolor_stroke");
        return TCL_ERROR;
    }

    if (Tcl_GetDoubleFromObj(interp, objv[4], &blue) != TCL_OK)
    {
        PDF_WrongParameter(interp, "blue in PDF_setrgbcolor_stroke");
        return TCL_ERROR;
    }

    try { PDF_setrgbcolor_stroke(p, red, green, blue);
    } catch;

    return TCL_OK;
}


static int
_wrap_PDF_shading(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
    PDF *p;
    const char *shtype;
    int shtype_len;
    double x_0;
    double y_0;
    double x_1;
    double y_1;
    double c_1;
    double c_2;
    double c_3;
    double c_4;
    const char *optlist;
    int optlist_len;
    char *res;
    int volatile _result = -1;

    if (objc != 12)
    {
        PDF_WrongCommand(interp, "PDF_shading p shtype x_0 y_0 x_1 y_1 c_1 c_2 c_3 c_4 optlist ");
        return TCL_ERROR;
    }

    if ((res = Tcl_GetStringFromObj(objv[1], NULL)) == NULL)
    {
        PDF_NoPDFHandle(interp, "PDF_shading");
        return TCL_ERROR;
    }

    if (SWIG_GetPtr(res, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle(interp, "PDF_shading");
        Tcl_AppendResult(interp, res, (char *) NULL);
        return TCL_ERROR;
    }

    if ((shtype = PDF_GetStringFromObj(p, interp, objv[2], PDF_BYTES, &shtype_len)) == NULL)
    {
        PDF_WrongParameter(interp, "shtype in PDF_shading");
        return TCL_ERROR;
    }

    if (Tcl_GetDoubleFromObj(interp, objv[3], &x_0) != TCL_OK)
    {
        PDF_WrongParameter(interp, "x_0 in PDF_shading");
        return TCL_ERROR;
    }

    if (Tcl_GetDoubleFromObj(interp, objv[4], &y_0) != TCL_OK)
    {
        PDF_WrongParameter(interp, "y_0 in PDF_shading");
        return TCL_ERROR;
    }

    if (Tcl_GetDoubleFromObj(interp, objv[5], &x_1) != TCL_OK)
    {
        PDF_WrongParameter(interp, "x_1 in PDF_shading");
        return TCL_ERROR;
    }

    if (Tcl_GetDoubleFromObj(interp, objv[6], &y_1) != TCL_OK)
    {
        PDF_WrongParameter(interp, "y_1 in PDF_shading");
        return TCL_ERROR;
    }

    if (Tcl_GetDoubleFromObj(interp, objv[7], &c_1) != TCL_OK)
    {
        PDF_WrongParameter(interp, "c_1 in PDF_shading");
        return TCL_ERROR;
    }

    if (Tcl_GetDoubleFromObj(interp, objv[8], &c_2) != TCL_OK)
    {
        PDF_WrongParameter(interp, "c_2 in PDF_shading");
        return TCL_ERROR;
    }

    if (Tcl_GetDoubleFromObj(interp, objv[9], &c_3) != TCL_OK)
    {
        PDF_WrongParameter(interp, "c_3 in PDF_shading");
        return TCL_ERROR;
    }

    if (Tcl_GetDoubleFromObj(interp, objv[10], &c_4) != TCL_OK)
    {
        PDF_WrongParameter(interp, "c_4 in PDF_shading");
        return TCL_ERROR;
    }

    if ((optlist = PDF_GetStringFromObj(p, interp, objv[11], PDF_UTF8, &optlist_len)) == NULL)
    {
        PDF_WrongParameter(interp, "optlist in PDF_shading");
        return TCL_ERROR;
    }

    try { _result = (int) PDF_shading(p, shtype, x_0, y_0, x_1, y_1, c_1, c_2, c_3, c_4, optlist);
    } catch;

    sprintf(interp->result, "%d", _result);
    return TCL_OK;
}


static int
_wrap_PDF_shading_pattern(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
    PDF *p;
    int shading;
    const char *optlist;
    int optlist_len;
    char *res;
    int volatile _result = -1;

    if (objc != 4)
    {
        PDF_WrongCommand(interp, "PDF_shading_pattern p shading optlist ");
        return TCL_ERROR;
    }

    if ((res = Tcl_GetStringFromObj(objv[1], NULL)) == NULL)
    {
        PDF_NoPDFHandle(interp, "PDF_shading_pattern");
        return TCL_ERROR;
    }

    if (SWIG_GetPtr(res, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle(interp, "PDF_shading_pattern");
        Tcl_AppendResult(interp, res, (char *) NULL);
        return TCL_ERROR;
    }

    if (Tcl_GetIntFromObj(interp, objv[2], &shading) != TCL_OK)
    {
        PDF_WrongParameter(interp, "shading in PDF_shading_pattern");
        return TCL_ERROR;
    }

    if ((optlist = PDF_GetStringFromObj(p, interp, objv[3], PDF_UTF8, &optlist_len)) == NULL)
    {
        PDF_WrongParameter(interp, "optlist in PDF_shading_pattern");
        return TCL_ERROR;
    }

    try { _result = (int) PDF_shading_pattern(p, shading, optlist);
    } catch;

    sprintf(interp->result, "%d", _result);
    return TCL_OK;
}


static int
_wrap_PDF_shfill(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
    PDF *p;
    int shading;
    char *res;

    if (objc != 3)
    {
        PDF_WrongCommand(interp, "PDF_shfill p shading ");
        return TCL_ERROR;
    }

    if ((res = Tcl_GetStringFromObj(objv[1], NULL)) == NULL)
    {
        PDF_NoPDFHandle(interp, "PDF_shfill");
        return TCL_ERROR;
    }

    if (SWIG_GetPtr(res, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle(interp, "PDF_shfill");
        Tcl_AppendResult(interp, res, (char *) NULL);
        return TCL_ERROR;
    }

    if (Tcl_GetIntFromObj(interp, objv[2], &shading) != TCL_OK)
    {
        PDF_WrongParameter(interp, "shading in PDF_shfill");
        return TCL_ERROR;
    }

    try { PDF_shfill(p, shading);
    } catch;

    return TCL_OK;
}


static int
_wrap_PDF_show(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
    PDF *p;
    const char *text;
    int text_len;
    char *res;

    if (objc != 3)
    {
        PDF_WrongCommand(interp, "PDF_show p text ");
        return TCL_ERROR;
    }

    if ((res = Tcl_GetStringFromObj(objv[1], NULL)) == NULL)
    {
        PDF_NoPDFHandle(interp, "PDF_show");
        return TCL_ERROR;
    }

    if (SWIG_GetPtr(res, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle(interp, "PDF_show");
        Tcl_AppendResult(interp, res, (char *) NULL);
        return TCL_ERROR;
    }

    if ((text = PDF_GetStringFromObj(p, interp, objv[2], PDF_UTF16, &text_len)) == NULL)
    {
        PDF_WrongParameter(interp, "text in PDF_show");
        return TCL_ERROR;
    }

    try { PDF_show2(p, text, text_len);
    } catch;

    return TCL_OK;
}


static int
_wrap_PDF_show_boxed(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
    PDF *p;
    const char *text;
    int text_len;
    double left;
    double top;
    double width;
    double height;
    const char *hmode;
    int hmode_len;
    const char *feature;
    int feature_len;
    char *res;
    int volatile _result = -1;

    if (objc != 9)
    {
        PDF_WrongCommand(interp, "PDF_show_boxed p text left top width height hmode feature ");
        return TCL_ERROR;
    }

    if ((res = Tcl_GetStringFromObj(objv[1], NULL)) == NULL)
    {
        PDF_NoPDFHandle(interp, "PDF_show_boxed");
        return TCL_ERROR;
    }

    if (SWIG_GetPtr(res, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle(interp, "PDF_show_boxed");
        Tcl_AppendResult(interp, res, (char *) NULL);
        return TCL_ERROR;
    }

    if ((text = PDF_GetStringFromObj(p, interp, objv[2], PDF_UTF16, &text_len)) == NULL)
    {
        PDF_WrongParameter(interp, "text in PDF_show_boxed");
        return TCL_ERROR;
    }

    if (Tcl_GetDoubleFromObj(interp, objv[3], &left) != TCL_OK)
    {
        PDF_WrongParameter(interp, "left in PDF_show_boxed");
        return TCL_ERROR;
    }

    if (Tcl_GetDoubleFromObj(interp, objv[4], &top) != TCL_OK)
    {
        PDF_WrongParameter(interp, "top in PDF_show_boxed");
        return TCL_ERROR;
    }

    if (Tcl_GetDoubleFromObj(interp, objv[5], &width) != TCL_OK)
    {
        PDF_WrongParameter(interp, "width in PDF_show_boxed");
        return TCL_ERROR;
    }

    if (Tcl_GetDoubleFromObj(interp, objv[6], &height) != TCL_OK)
    {
        PDF_WrongParameter(interp, "height in PDF_show_boxed");
        return TCL_ERROR;
    }

    if ((hmode = PDF_GetStringFromObj(p, interp, objv[7], PDF_BYTES, &hmode_len)) == NULL)
    {
        PDF_WrongParameter(interp, "hmode in PDF_show_boxed");
        return TCL_ERROR;
    }

    if ((feature = PDF_GetStringFromObj(p, interp, objv[8], PDF_BYTES, &feature_len)) == NULL)
    {
        PDF_WrongParameter(interp, "feature in PDF_show_boxed");
        return TCL_ERROR;
    }

    try { _result = (int) PDF_show_boxed2(p, text, text_len, left, top, width, height, hmode, feature);
    } catch;

    sprintf(interp->result, "%d", _result);
    return TCL_OK;
}


static int
_wrap_PDF_show_xy(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
    PDF *p;
    const char *text;
    int text_len;
    double x;
    double y;
    char *res;

    if (objc != 5)
    {
        PDF_WrongCommand(interp, "PDF_show_xy p text x y ");
        return TCL_ERROR;
    }

    if ((res = Tcl_GetStringFromObj(objv[1], NULL)) == NULL)
    {
        PDF_NoPDFHandle(interp, "PDF_show_xy");
        return TCL_ERROR;
    }

    if (SWIG_GetPtr(res, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle(interp, "PDF_show_xy");
        Tcl_AppendResult(interp, res, (char *) NULL);
        return TCL_ERROR;
    }

    if ((text = PDF_GetStringFromObj(p, interp, objv[2], PDF_UTF16, &text_len)) == NULL)
    {
        PDF_WrongParameter(interp, "text in PDF_show_xy");
        return TCL_ERROR;
    }

    if (Tcl_GetDoubleFromObj(interp, objv[3], &x) != TCL_OK)
    {
        PDF_WrongParameter(interp, "x in PDF_show_xy");
        return TCL_ERROR;
    }

    if (Tcl_GetDoubleFromObj(interp, objv[4], &y) != TCL_OK)
    {
        PDF_WrongParameter(interp, "y in PDF_show_xy");
        return TCL_ERROR;
    }

    try { PDF_show_xy2(p, text, text_len, x, y);
    } catch;

    return TCL_OK;
}


static int
_wrap_PDF_skew(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
    PDF *p;
    double alpha;
    double beta;
    char *res;

    if (objc != 4)
    {
        PDF_WrongCommand(interp, "PDF_skew p alpha beta ");
        return TCL_ERROR;
    }

    if ((res = Tcl_GetStringFromObj(objv[1], NULL)) == NULL)
    {
        PDF_NoPDFHandle(interp, "PDF_skew");
        return TCL_ERROR;
    }

    if (SWIG_GetPtr(res, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle(interp, "PDF_skew");
        Tcl_AppendResult(interp, res, (char *) NULL);
        return TCL_ERROR;
    }

    if (Tcl_GetDoubleFromObj(interp, objv[2], &alpha) != TCL_OK)
    {
        PDF_WrongParameter(interp, "alpha in PDF_skew");
        return TCL_ERROR;
    }

    if (Tcl_GetDoubleFromObj(interp, objv[3], &beta) != TCL_OK)
    {
        PDF_WrongParameter(interp, "beta in PDF_skew");
        return TCL_ERROR;
    }

    try { PDF_skew(p, alpha, beta);
    } catch;

    return TCL_OK;
}


static int
_wrap_PDF_stringwidth(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
    PDF *p;
    const char *text;
    int text_len;
    int font;
    double fontsize;
    char *res;
    double volatile _result = 0;

    if (objc != 5)
    {
        PDF_WrongCommand(interp, "PDF_stringwidth p text font fontsize ");
        return TCL_ERROR;
    }

    if ((res = Tcl_GetStringFromObj(objv[1], NULL)) == NULL)
    {
        PDF_NoPDFHandle(interp, "PDF_stringwidth");
        return TCL_ERROR;
    }

    if (SWIG_GetPtr(res, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle(interp, "PDF_stringwidth");
        Tcl_AppendResult(interp, res, (char *) NULL);
        return TCL_ERROR;
    }

    if ((text = PDF_GetStringFromObj(p, interp, objv[2], PDF_UTF16, &text_len)) == NULL)
    {
        PDF_WrongParameter(interp, "text in PDF_stringwidth");
        return TCL_ERROR;
    }

    if (Tcl_GetIntFromObj(interp, objv[3], &font) != TCL_OK)
    {
        PDF_WrongParameter(interp, "font in PDF_stringwidth");
        return TCL_ERROR;
    }

    if (Tcl_GetDoubleFromObj(interp, objv[4], &fontsize) != TCL_OK)
    {
        PDF_WrongParameter(interp, "fontsize in PDF_stringwidth");
        return TCL_ERROR;
    }

    try { _result = (double) PDF_stringwidth2(p, text, text_len, font, fontsize);
    } catch;

    Tcl_PrintDouble(interp, (double) _result, interp->result);
    return TCL_OK;
}


static int
_wrap_PDF_stroke(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
    PDF *p;
    char *res;

    if (objc != 2)
    {
        PDF_WrongCommand(interp, "PDF_stroke p ");
        return TCL_ERROR;
    }

    if ((res = Tcl_GetStringFromObj(objv[1], NULL)) == NULL)
    {
        PDF_NoPDFHandle(interp, "PDF_stroke");
        return TCL_ERROR;
    }

    if (SWIG_GetPtr(res, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle(interp, "PDF_stroke");
        Tcl_AppendResult(interp, res, (char *) NULL);
        return TCL_ERROR;
    }

    try { PDF_stroke(p);
    } catch;

    return TCL_OK;
}


static int
_wrap_PDF_suspend_page(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
    PDF *p;
    const char *optlist;
    int optlist_len;
    char *res;

    if (objc != 3)
    {
        PDF_WrongCommand(interp, "PDF_suspend_page p optlist ");
        return TCL_ERROR;
    }

    if ((res = Tcl_GetStringFromObj(objv[1], NULL)) == NULL)
    {
        PDF_NoPDFHandle(interp, "PDF_suspend_page");
        return TCL_ERROR;
    }

    if (SWIG_GetPtr(res, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle(interp, "PDF_suspend_page");
        Tcl_AppendResult(interp, res, (char *) NULL);
        return TCL_ERROR;
    }

    if ((optlist = PDF_GetStringFromObj(p, interp, objv[2], PDF_UTF8, &optlist_len)) == NULL)
    {
        PDF_WrongParameter(interp, "optlist in PDF_suspend_page");
        return TCL_ERROR;
    }

    try { PDF_suspend_page(p, optlist);
    } catch;

    return TCL_OK;
}


static int
_wrap_PDF_translate(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
    PDF *p;
    double tx;
    double ty;
    char *res;

    if (objc != 4)
    {
        PDF_WrongCommand(interp, "PDF_translate p tx ty ");
        return TCL_ERROR;
    }

    if ((res = Tcl_GetStringFromObj(objv[1], NULL)) == NULL)
    {
        PDF_NoPDFHandle(interp, "PDF_translate");
        return TCL_ERROR;
    }

    if (SWIG_GetPtr(res, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle(interp, "PDF_translate");
        Tcl_AppendResult(interp, res, (char *) NULL);
        return TCL_ERROR;
    }

    if (Tcl_GetDoubleFromObj(interp, objv[2], &tx) != TCL_OK)
    {
        PDF_WrongParameter(interp, "tx in PDF_translate");
        return TCL_ERROR;
    }

    if (Tcl_GetDoubleFromObj(interp, objv[3], &ty) != TCL_OK)
    {
        PDF_WrongParameter(interp, "ty in PDF_translate");
        return TCL_ERROR;
    }

    try { PDF_translate(p, tx, ty);
    } catch;

    return TCL_OK;
}



/* This is required to make our extension work with safe Tcl interpreters */
SWIGEXPORT(int,Pdflib_SafeInit)(Tcl_Interp *interp)
{
    return TCL_OK;
}

/* This is required to satisfy pkg_mkIndex */
SWIGEXPORT(int,Pdflib_tcl_SafeInit)(Tcl_Interp *interp)
{
    return Pdflib_SafeInit(interp);
}

/* This is required to satisfy pkg_mkIndex */
SWIGEXPORT(int,Pdflib_tcl_Init)(Tcl_Interp *interp)
{
    return Pdflib_Init(interp);
}

/* This is required to satisfy pkg_mkIndex */
SWIGEXPORT(int,Pdf_tcl_Init)(Tcl_Interp *interp)
{
    return Pdflib_Init(interp);
}

/* This is required to satisfy pkg_mkIndex */
SWIGEXPORT(int,Pdf_tcl_SafeInit)(Tcl_Interp *interp)
{
    return Pdflib_SafeInit(interp);
}

SWIGEXPORT(int,Pdflib_Init)(Tcl_Interp *interp)
{
    if (interp == 0)
	return TCL_ERROR;

#ifdef USE_TCL_STUBS
    if (Tcl_InitStubs(interp, "8.2", 0) == NULL) {
 	return TCL_ERROR;
    }
#else
    if (Tcl_PkgRequire(interp, "Tcl", TCL_VERSION, 1) == NULL) {
 	return TCL_ERROR;
    }
#endif

    /* Boot the PDFlib core */
    PDF_boot();

    /* Tell Tcl which package we are going to define */
    Tcl_PkgProvide(interp, "pdflib", PDFLIB_VERSIONSTRING);


    Tcl_CreateObjCommand(interp, "PDF_activate_item", (Tcl_ObjCmdProc*) _wrap_PDF_activate_item, (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
    Tcl_CreateObjCommand(interp, "PDF_add_bookmark", (Tcl_ObjCmdProc*) _wrap_PDF_add_bookmark, (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
    Tcl_CreateObjCommand(interp, "PDF_add_launchlink", (Tcl_ObjCmdProc*) _wrap_PDF_add_launchlink, (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
    Tcl_CreateObjCommand(interp, "PDF_add_locallink", (Tcl_ObjCmdProc*) _wrap_PDF_add_locallink, (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
    Tcl_CreateObjCommand(interp, "PDF_add_nameddest", (Tcl_ObjCmdProc*) _wrap_PDF_add_nameddest, (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
    Tcl_CreateObjCommand(interp, "PDF_add_note", (Tcl_ObjCmdProc*) _wrap_PDF_add_note, (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
    Tcl_CreateObjCommand(interp, "PDF_add_pdflink", (Tcl_ObjCmdProc*) _wrap_PDF_add_pdflink, (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
    Tcl_CreateObjCommand(interp, "PDF_add_thumbnail", (Tcl_ObjCmdProc*) _wrap_PDF_add_thumbnail, (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
    Tcl_CreateObjCommand(interp, "PDF_add_weblink", (Tcl_ObjCmdProc*) _wrap_PDF_add_weblink, (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
    Tcl_CreateObjCommand(interp, "PDF_arc", (Tcl_ObjCmdProc*) _wrap_PDF_arc, (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
    Tcl_CreateObjCommand(interp, "PDF_arcn", (Tcl_ObjCmdProc*) _wrap_PDF_arcn, (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
    Tcl_CreateObjCommand(interp, "PDF_attach_file", (Tcl_ObjCmdProc*) _wrap_PDF_attach_file, (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
    Tcl_CreateObjCommand(interp, "PDF_begin_document", (Tcl_ObjCmdProc*) _wrap_PDF_begin_document, (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
    Tcl_CreateObjCommand(interp, "PDF_begin_font", (Tcl_ObjCmdProc*) _wrap_PDF_begin_font, (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
    Tcl_CreateObjCommand(interp, "PDF_begin_glyph", (Tcl_ObjCmdProc*) _wrap_PDF_begin_glyph, (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
    Tcl_CreateObjCommand(interp, "PDF_begin_item", (Tcl_ObjCmdProc*) _wrap_PDF_begin_item, (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
    Tcl_CreateObjCommand(interp, "PDF_begin_layer", (Tcl_ObjCmdProc*) _wrap_PDF_begin_layer, (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
    Tcl_CreateObjCommand(interp, "PDF_begin_mc", (Tcl_ObjCmdProc*) _wrap_PDF_begin_mc, (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
    Tcl_CreateObjCommand(interp, "PDF_begin_page", (Tcl_ObjCmdProc*) _wrap_PDF_begin_page, (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
    Tcl_CreateObjCommand(interp, "PDF_begin_page_ext", (Tcl_ObjCmdProc*) _wrap_PDF_begin_page_ext, (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
    Tcl_CreateObjCommand(interp, "PDF_begin_pattern", (Tcl_ObjCmdProc*) _wrap_PDF_begin_pattern, (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
    Tcl_CreateObjCommand(interp, "PDF_begin_template", (Tcl_ObjCmdProc*) _wrap_PDF_begin_template, (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
    Tcl_CreateObjCommand(interp, "PDF_circle", (Tcl_ObjCmdProc*) _wrap_PDF_circle, (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
    Tcl_CreateObjCommand(interp, "PDF_clip", (Tcl_ObjCmdProc*) _wrap_PDF_clip, (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
    Tcl_CreateObjCommand(interp, "PDF_close", (Tcl_ObjCmdProc*) _wrap_PDF_close, (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
    Tcl_CreateObjCommand(interp, "PDF_close_image", (Tcl_ObjCmdProc*) _wrap_PDF_close_image, (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
    Tcl_CreateObjCommand(interp, "PDF_close_pdi", (Tcl_ObjCmdProc*) _wrap_PDF_close_pdi, (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
    Tcl_CreateObjCommand(interp, "PDF_close_pdi_page", (Tcl_ObjCmdProc*) _wrap_PDF_close_pdi_page, (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
    Tcl_CreateObjCommand(interp, "PDF_closepath", (Tcl_ObjCmdProc*) _wrap_PDF_closepath, (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
    Tcl_CreateObjCommand(interp, "PDF_closepath_fill_stroke", (Tcl_ObjCmdProc*) _wrap_PDF_closepath_fill_stroke, (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
    Tcl_CreateObjCommand(interp, "PDF_closepath_stroke", (Tcl_ObjCmdProc*) _wrap_PDF_closepath_stroke, (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
    Tcl_CreateObjCommand(interp, "PDF_concat", (Tcl_ObjCmdProc*) _wrap_PDF_concat, (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
    Tcl_CreateObjCommand(interp, "PDF_continue_text", (Tcl_ObjCmdProc*) _wrap_PDF_continue_text, (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
    Tcl_CreateObjCommand(interp, "PDF_create_action", (Tcl_ObjCmdProc*) _wrap_PDF_create_action, (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
    Tcl_CreateObjCommand(interp, "PDF_create_annotation", (Tcl_ObjCmdProc*) _wrap_PDF_create_annotation, (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
    Tcl_CreateObjCommand(interp, "PDF_create_bookmark", (Tcl_ObjCmdProc*) _wrap_PDF_create_bookmark, (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
    Tcl_CreateObjCommand(interp, "PDF_create_field", (Tcl_ObjCmdProc*) _wrap_PDF_create_field, (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
    Tcl_CreateObjCommand(interp, "PDF_create_fieldgroup", (Tcl_ObjCmdProc*) _wrap_PDF_create_fieldgroup, (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
    Tcl_CreateObjCommand(interp, "PDF_create_gstate", (Tcl_ObjCmdProc*) _wrap_PDF_create_gstate, (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
    Tcl_CreateObjCommand(interp, "PDF_create_pvf", (Tcl_ObjCmdProc*) _wrap_PDF_create_pvf, (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
    Tcl_CreateObjCommand(interp, "PDF_create_textflow", (Tcl_ObjCmdProc*) _wrap_PDF_create_textflow, (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
    Tcl_CreateObjCommand(interp, "PDF_curveto", (Tcl_ObjCmdProc*) _wrap_PDF_curveto, (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
    Tcl_CreateObjCommand(interp, "PDF_define_layer", (Tcl_ObjCmdProc*) _wrap_PDF_define_layer, (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
    Tcl_CreateObjCommand(interp, "PDF_delete", (Tcl_ObjCmdProc*) _wrap_PDF_delete, (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
    Tcl_CreateObjCommand(interp, "PDF_delete_pvf", (Tcl_ObjCmdProc*) _wrap_PDF_delete_pvf, (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
    Tcl_CreateObjCommand(interp, "PDF_delete_textflow", (Tcl_ObjCmdProc*) _wrap_PDF_delete_textflow, (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
    Tcl_CreateObjCommand(interp, "PDF_encoding_set_char", (Tcl_ObjCmdProc*) _wrap_PDF_encoding_set_char, (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
    Tcl_CreateObjCommand(interp, "PDF_end_document", (Tcl_ObjCmdProc*) _wrap_PDF_end_document, (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
    Tcl_CreateObjCommand(interp, "PDF_end_font", (Tcl_ObjCmdProc*) _wrap_PDF_end_font, (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
    Tcl_CreateObjCommand(interp, "PDF_end_glyph", (Tcl_ObjCmdProc*) _wrap_PDF_end_glyph, (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
    Tcl_CreateObjCommand(interp, "PDF_end_item", (Tcl_ObjCmdProc*) _wrap_PDF_end_item, (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
    Tcl_CreateObjCommand(interp, "PDF_end_layer", (Tcl_ObjCmdProc*) _wrap_PDF_end_layer, (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
    Tcl_CreateObjCommand(interp, "PDF_end_mc", (Tcl_ObjCmdProc*) _wrap_PDF_end_mc, (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
    Tcl_CreateObjCommand(interp, "PDF_end_page", (Tcl_ObjCmdProc*) _wrap_PDF_end_page, (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
    Tcl_CreateObjCommand(interp, "PDF_end_page_ext", (Tcl_ObjCmdProc*) _wrap_PDF_end_page_ext, (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
    Tcl_CreateObjCommand(interp, "PDF_end_pattern", (Tcl_ObjCmdProc*) _wrap_PDF_end_pattern, (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
    Tcl_CreateObjCommand(interp, "PDF_end_template", (Tcl_ObjCmdProc*) _wrap_PDF_end_template, (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
    Tcl_CreateObjCommand(interp, "PDF_endpath", (Tcl_ObjCmdProc*) _wrap_PDF_endpath, (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
    Tcl_CreateObjCommand(interp, "PDF_fill", (Tcl_ObjCmdProc*) _wrap_PDF_fill, (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
    Tcl_CreateObjCommand(interp, "PDF_fill_imageblock", (Tcl_ObjCmdProc*) _wrap_PDF_fill_imageblock, (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
    Tcl_CreateObjCommand(interp, "PDF_fill_pdfblock", (Tcl_ObjCmdProc*) _wrap_PDF_fill_pdfblock, (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
    Tcl_CreateObjCommand(interp, "PDF_fill_stroke", (Tcl_ObjCmdProc*) _wrap_PDF_fill_stroke, (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
    Tcl_CreateObjCommand(interp, "PDF_fill_textblock", (Tcl_ObjCmdProc*) _wrap_PDF_fill_textblock, (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
    Tcl_CreateObjCommand(interp, "PDF_findfont", (Tcl_ObjCmdProc*) _wrap_PDF_findfont, (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
    Tcl_CreateObjCommand(interp, "PDF_fit_image", (Tcl_ObjCmdProc*) _wrap_PDF_fit_image, (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
    Tcl_CreateObjCommand(interp, "PDF_fit_pdi_page", (Tcl_ObjCmdProc*) _wrap_PDF_fit_pdi_page, (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
    Tcl_CreateObjCommand(interp, "PDF_fit_textflow", (Tcl_ObjCmdProc*) _wrap_PDF_fit_textflow, (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
    Tcl_CreateObjCommand(interp, "PDF_fit_textline", (Tcl_ObjCmdProc*) _wrap_PDF_fit_textline, (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
    Tcl_CreateObjCommand(interp, "PDF_get_apiname", (Tcl_ObjCmdProc*) _wrap_PDF_get_apiname, (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
    Tcl_CreateObjCommand(interp, "PDF_get_buffer", (Tcl_ObjCmdProc*) _wrap_PDF_get_buffer, (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
    Tcl_CreateObjCommand(interp, "PDF_get_errmsg", (Tcl_ObjCmdProc*) _wrap_PDF_get_errmsg, (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
    Tcl_CreateObjCommand(interp, "PDF_get_errnum", (Tcl_ObjCmdProc*) _wrap_PDF_get_errnum, (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
    Tcl_CreateObjCommand(interp, "PDF_get_parameter", (Tcl_ObjCmdProc*) _wrap_PDF_get_parameter, (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
    Tcl_CreateObjCommand(interp, "PDF_get_pdi_parameter", (Tcl_ObjCmdProc*) _wrap_PDF_get_pdi_parameter, (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
    Tcl_CreateObjCommand(interp, "PDF_get_pdi_value", (Tcl_ObjCmdProc*) _wrap_PDF_get_pdi_value, (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
    Tcl_CreateObjCommand(interp, "PDF_get_value", (Tcl_ObjCmdProc*) _wrap_PDF_get_value, (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
    Tcl_CreateObjCommand(interp, "PDF_info_textflow", (Tcl_ObjCmdProc*) _wrap_PDF_info_textflow, (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
    Tcl_CreateObjCommand(interp, "PDF_initgraphics", (Tcl_ObjCmdProc*) _wrap_PDF_initgraphics, (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
    Tcl_CreateObjCommand(interp, "PDF_lineto", (Tcl_ObjCmdProc*) _wrap_PDF_lineto, (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
    Tcl_CreateObjCommand(interp, "PDF_load_font", (Tcl_ObjCmdProc*) _wrap_PDF_load_font, (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
    Tcl_CreateObjCommand(interp, "PDF_load_iccprofile", (Tcl_ObjCmdProc*) _wrap_PDF_load_iccprofile, (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
    Tcl_CreateObjCommand(interp, "PDF_load_image", (Tcl_ObjCmdProc*) _wrap_PDF_load_image, (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
    Tcl_CreateObjCommand(interp, "PDF_makespotcolor", (Tcl_ObjCmdProc*) _wrap_PDF_makespotcolor, (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
    Tcl_CreateObjCommand(interp, "PDF_mc_point", (Tcl_ObjCmdProc*) _wrap_PDF_mc_point, (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
    Tcl_CreateObjCommand(interp, "PDF_moveto", (Tcl_ObjCmdProc*) _wrap_PDF_moveto, (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
    Tcl_CreateObjCommand(interp, "PDF_new", (Tcl_ObjCmdProc*) _wrap_PDF_new, (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
    Tcl_CreateObjCommand(interp, "PDF_open_CCITT", (Tcl_ObjCmdProc*) _wrap_PDF_open_CCITT, (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
    Tcl_CreateObjCommand(interp, "PDF_open_file", (Tcl_ObjCmdProc*) _wrap_PDF_open_file, (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
    Tcl_CreateObjCommand(interp, "PDF_open_image", (Tcl_ObjCmdProc*) _wrap_PDF_open_image, (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
    Tcl_CreateObjCommand(interp, "PDF_open_image_file", (Tcl_ObjCmdProc*) _wrap_PDF_open_image_file, (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
    Tcl_CreateObjCommand(interp, "PDF_open_pdi", (Tcl_ObjCmdProc*) _wrap_PDF_open_pdi, (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
    Tcl_CreateObjCommand(interp, "PDF_open_pdi_page", (Tcl_ObjCmdProc*) _wrap_PDF_open_pdi_page, (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
    Tcl_CreateObjCommand(interp, "PDF_place_image", (Tcl_ObjCmdProc*) _wrap_PDF_place_image, (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
    Tcl_CreateObjCommand(interp, "PDF_place_pdi_page", (Tcl_ObjCmdProc*) _wrap_PDF_place_pdi_page, (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
    Tcl_CreateObjCommand(interp, "PDF_process_pdi", (Tcl_ObjCmdProc*) _wrap_PDF_process_pdi, (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
    Tcl_CreateObjCommand(interp, "PDF_rect", (Tcl_ObjCmdProc*) _wrap_PDF_rect, (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
    Tcl_CreateObjCommand(interp, "PDF_restore", (Tcl_ObjCmdProc*) _wrap_PDF_restore, (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
    Tcl_CreateObjCommand(interp, "PDF_resume_page", (Tcl_ObjCmdProc*) _wrap_PDF_resume_page, (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
    Tcl_CreateObjCommand(interp, "PDF_rotate", (Tcl_ObjCmdProc*) _wrap_PDF_rotate, (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
    Tcl_CreateObjCommand(interp, "PDF_save", (Tcl_ObjCmdProc*) _wrap_PDF_save, (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
    Tcl_CreateObjCommand(interp, "PDF_scale", (Tcl_ObjCmdProc*) _wrap_PDF_scale, (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
    Tcl_CreateObjCommand(interp, "PDF_set_border_color", (Tcl_ObjCmdProc*) _wrap_PDF_set_border_color, (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
    Tcl_CreateObjCommand(interp, "PDF_set_border_dash", (Tcl_ObjCmdProc*) _wrap_PDF_set_border_dash, (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
    Tcl_CreateObjCommand(interp, "PDF_set_border_style", (Tcl_ObjCmdProc*) _wrap_PDF_set_border_style, (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
    Tcl_CreateObjCommand(interp, "PDF_set_gstate", (Tcl_ObjCmdProc*) _wrap_PDF_set_gstate, (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
    Tcl_CreateObjCommand(interp, "PDF_set_info", (Tcl_ObjCmdProc*) _wrap_PDF_set_info, (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
    Tcl_CreateObjCommand(interp, "PDF_set_layer_dependency", (Tcl_ObjCmdProc*) _wrap_PDF_set_layer_dependency, (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
    Tcl_CreateObjCommand(interp, "PDF_set_parameter", (Tcl_ObjCmdProc*) _wrap_PDF_set_parameter, (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
    Tcl_CreateObjCommand(interp, "PDF_set_text_pos", (Tcl_ObjCmdProc*) _wrap_PDF_set_text_pos, (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
    Tcl_CreateObjCommand(interp, "PDF_set_value", (Tcl_ObjCmdProc*) _wrap_PDF_set_value, (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
    Tcl_CreateObjCommand(interp, "PDF_setcolor", (Tcl_ObjCmdProc*) _wrap_PDF_setcolor, (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
    Tcl_CreateObjCommand(interp, "PDF_setdash", (Tcl_ObjCmdProc*) _wrap_PDF_setdash, (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
    Tcl_CreateObjCommand(interp, "PDF_setdashpattern", (Tcl_ObjCmdProc*) _wrap_PDF_setdashpattern, (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
    Tcl_CreateObjCommand(interp, "PDF_setflat", (Tcl_ObjCmdProc*) _wrap_PDF_setflat, (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
    Tcl_CreateObjCommand(interp, "PDF_setfont", (Tcl_ObjCmdProc*) _wrap_PDF_setfont, (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
    Tcl_CreateObjCommand(interp, "PDF_setgray", (Tcl_ObjCmdProc*) _wrap_PDF_setgray, (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
    Tcl_CreateObjCommand(interp, "PDF_setgray_stroke", (Tcl_ObjCmdProc*) _wrap_PDF_setgray_stroke, (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
    Tcl_CreateObjCommand(interp, "PDF_setgray_fill", (Tcl_ObjCmdProc*) _wrap_PDF_setgray_fill, (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
    Tcl_CreateObjCommand(interp, "PDF_setlinecap", (Tcl_ObjCmdProc*) _wrap_PDF_setlinecap, (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
    Tcl_CreateObjCommand(interp, "PDF_setlinejoin", (Tcl_ObjCmdProc*) _wrap_PDF_setlinejoin, (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
    Tcl_CreateObjCommand(interp, "PDF_setlinewidth", (Tcl_ObjCmdProc*) _wrap_PDF_setlinewidth, (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
    Tcl_CreateObjCommand(interp, "PDF_setmatrix", (Tcl_ObjCmdProc*) _wrap_PDF_setmatrix, (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
    Tcl_CreateObjCommand(interp, "PDF_setmiterlimit", (Tcl_ObjCmdProc*) _wrap_PDF_setmiterlimit, (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
    Tcl_CreateObjCommand(interp, "PDF_setpolydash", (Tcl_ObjCmdProc*) _wrap_PDF_setpolydash, (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
    Tcl_CreateObjCommand(interp, "PDF_setrgbcolor", (Tcl_ObjCmdProc*) _wrap_PDF_setrgbcolor, (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
    Tcl_CreateObjCommand(interp, "PDF_setrgbcolor_fill", (Tcl_ObjCmdProc*) _wrap_PDF_setrgbcolor_fill, (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
    Tcl_CreateObjCommand(interp, "PDF_setrgbcolor_stroke", (Tcl_ObjCmdProc*) _wrap_PDF_setrgbcolor_stroke, (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
    Tcl_CreateObjCommand(interp, "PDF_shading", (Tcl_ObjCmdProc*) _wrap_PDF_shading, (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
    Tcl_CreateObjCommand(interp, "PDF_shading_pattern", (Tcl_ObjCmdProc*) _wrap_PDF_shading_pattern, (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
    Tcl_CreateObjCommand(interp, "PDF_shfill", (Tcl_ObjCmdProc*) _wrap_PDF_shfill, (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
    Tcl_CreateObjCommand(interp, "PDF_show", (Tcl_ObjCmdProc*) _wrap_PDF_show, (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
    Tcl_CreateObjCommand(interp, "PDF_show_boxed", (Tcl_ObjCmdProc*) _wrap_PDF_show_boxed, (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
    Tcl_CreateObjCommand(interp, "PDF_show_xy", (Tcl_ObjCmdProc*) _wrap_PDF_show_xy, (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
    Tcl_CreateObjCommand(interp, "PDF_skew", (Tcl_ObjCmdProc*) _wrap_PDF_skew, (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
    Tcl_CreateObjCommand(interp, "PDF_stringwidth", (Tcl_ObjCmdProc*) _wrap_PDF_stringwidth, (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
    Tcl_CreateObjCommand(interp, "PDF_stroke", (Tcl_ObjCmdProc*) _wrap_PDF_stroke, (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
    Tcl_CreateObjCommand(interp, "PDF_suspend_page", (Tcl_ObjCmdProc*) _wrap_PDF_suspend_page, (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
    Tcl_CreateObjCommand(interp, "PDF_translate", (Tcl_ObjCmdProc*) _wrap_PDF_translate, (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);


/*
 * These are the pointer type-equivalency mappings.
 * (Used by the SWIG pointer type-checker).
 */
    SWIG_RegisterMapping("_signed_long","_long",0);
    SWIG_RegisterMapping("_struct_PDF_s","_PDF",0);
    SWIG_RegisterMapping("_long","_unsigned_long",0);
    SWIG_RegisterMapping("_long","_signed_long",0);
    SWIG_RegisterMapping("_PDF","_struct_PDF_s",0);
    SWIG_RegisterMapping("_unsigned_long","_long",0);
    SWIG_RegisterMapping("_signed_int","_int",0);
    SWIG_RegisterMapping("_unsigned_short","_short",0);
    SWIG_RegisterMapping("_signed_short","_short",0);
    SWIG_RegisterMapping("_unsigned_int","_int",0);
    SWIG_RegisterMapping("_short","_unsigned_short",0);
    SWIG_RegisterMapping("_short","_signed_short",0);
    SWIG_RegisterMapping("_int","_unsigned_int",0);
    SWIG_RegisterMapping("_int","_signed_int",0);
    return TCL_OK;
}
