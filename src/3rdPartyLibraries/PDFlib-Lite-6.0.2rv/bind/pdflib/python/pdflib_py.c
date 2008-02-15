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

/* $Id: pdflib_py.c,v 1.1 2005/10/11 17:18:09 vuduc2 Exp $
 *
 * Wrapper code for the PDFlib Python binding
 *
 */

#include <string.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif
#include <Python.h>
#ifdef __cplusplus
}
#endif

/* Compilers which are not strictly ANSI conforming can set PDF_VOLATILE
 * to an empty value.
 */
#ifndef PDF_VOLATILE
#define PDF_VOLATILE    volatile
#endif

/* Definitions for Windows/Unix exporting */
#if defined(__WIN32__)
#   if defined(_MSC_VER)
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

typedef struct {
  char  *name;
  PyObject *(*get_attr)(void);
  int (*set_attr)(PyObject *);
} swig_globalvar;

typedef struct swig_varlinkobject {
  PyObject_HEAD
  swig_globalvar **vars;
  int    nvars;
  int    maxvars;
} swig_varlinkobject;

/* ----------------------------------------------------------------------
   swig_varlink_repr()

   Function for python repr method
   ---------------------------------------------------------------------- */

static PyObject *
swig_varlink_repr(swig_varlinkobject *v)
{
  v = v;
  return PyString_FromString("<Global variables>");
}

/* ---------------------------------------------------------------------
   swig_varlink_print()

   Print out all of the global variable names
   --------------------------------------------------------------------- */

static int
swig_varlink_print(swig_varlinkobject *v, FILE *fp, int flags)
{

  int i = 0;
  flags = flags;
  fprintf(fp,"Global variables { ");
  while (v->vars[i]) {
    fprintf(fp,"%s", v->vars[i]->name);
    i++;
    if (v->vars[i]) fprintf(fp,", ");
  }
  fprintf(fp," }\n");
  return 0;
}

/* --------------------------------------------------------------------
   swig_varlink_getattr

   This function gets the value of a variable and returns it as a
   PyObject.   In our case, we'll be looking at the datatype and
   converting into a number or string
   -------------------------------------------------------------------- */

static PyObject *
swig_varlink_getattr(swig_varlinkobject *v, char *n)
{
  int i = 0;
  char temp[128];

  while (v->vars[i]) {
    if (strcmp(v->vars[i]->name,n) == 0) {
      return (*v->vars[i]->get_attr)();
    }
    i++;
  }
  sprintf(temp,"C global variable %s not found.", n);
  PyErr_SetString(PyExc_NameError,temp);
  return NULL;
}

/* -------------------------------------------------------------------
   swig_varlink_setattr()

   This function sets the value of a variable.
   ------------------------------------------------------------------- */

static int
swig_varlink_setattr(swig_varlinkobject *v, char *n, PyObject *p)
{
  char temp[128];
  int i = 0;
  while (v->vars[i]) {
    if (strcmp(v->vars[i]->name,n) == 0) {
      return (*v->vars[i]->set_attr)(p);
    }
    i++;
  }
  sprintf(temp,"C global variable %s not found.", n);
  PyErr_SetString(PyExc_NameError,temp);
  return 1;
}

statichere PyTypeObject varlinktype = {
/*  PyObject_HEAD_INIT(&PyType_Type)  Note : This doesn't work on some machines */
  PyObject_HEAD_INIT(0)
  0,
  "varlink",                          /* Type name    */
  sizeof(swig_varlinkobject),         /* Basic size   */
  0,                                  /* Itemsize     */
  0,                                  /* Deallocator  */
  (printfunc) swig_varlink_print,     /* Print      */
  (getattrfunc) swig_varlink_getattr, /* get attr     */
  (setattrfunc) swig_varlink_setattr, /* Set attr     */
  0,                                  /* tp_compare   */
  (reprfunc) swig_varlink_repr,       /* tp_repr      */
  0,                                  /* tp_as_number */
  0,                                  /* tp_as_mapping*/
  0,                                  /* tp_hash      */
};

/* Create a variable linking object for use later */

SWIGSTATIC PyObject *
SWIG_newvarlink(void)
{
  swig_varlinkobject *result = 0;
  result = PyMem_NEW(swig_varlinkobject,1);
  varlinktype.ob_type = &PyType_Type;    /* Patch varlinktype into a PyType */
  result->ob_type = &varlinktype;
  /*  _Py_NewReference(result);  Does not seem to be necessary */
  result->nvars = 0;
  result->maxvars = 64;
  result->vars = (swig_globalvar **) malloc(64*sizeof(swig_globalvar *));
  result->vars[0] = 0;
  result->ob_refcnt = 0;
  Py_XINCREF((PyObject *) result);
  return ((PyObject*) result);
}

#ifdef PDFLIB_UNUSED
SWIGSTATIC void
SWIG_addvarlink(PyObject *p, char *name,
	   PyObject *(*get_attr)(void), int (*set_attr)(PyObject *p))
{
  swig_varlinkobject *v;
  v= (swig_varlinkobject *) p;

  if (v->nvars >= v->maxvars -1) {
    v->maxvars = 2*v->maxvars;
    v->vars = (swig_globalvar **) realloc(v->vars,v->maxvars*sizeof(swig_globalvar *));
    if (v->vars == NULL) {
      fprintf(stderr,"SWIG : Fatal error in initializing Python module.\n");
      exit(1);
    }
  }
  v->vars[v->nvars] = (swig_globalvar *) malloc(sizeof(swig_globalvar));
  v->vars[v->nvars]->name = (char *) malloc(strlen(name)+1);
  strcpy(v->vars[v->nvars]->name,name);
  v->vars[v->nvars]->get_attr = get_attr;
  v->vars[v->nvars]->set_attr = set_attr;
  v->nvars++;
  v->vars[v->nvars] = 0;
}

#endif /* PDFLIB_UNUSED */

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

#include <setjmp.h>

#include "pdflib.h"


/* Exception handling */

#define try	PDF_TRY(p)
#define catch	PDF_CATCH(p) { \
		char errmsg[1024];\
		sprintf(errmsg, "PDFlib Error [%d] %s: %s", PDF_get_errnum(p),\
		                    PDF_get_apiname(p), PDF_get_errmsg(p));\
		PyErr_SetString(PyExc_SystemError,errmsg); \
		return NULL; \
		}

static void
PDF_WrongPDFHandle(const char *funcname)
{
    char text[128];

    sprintf(text, "Type error in argument 1 of %s. Expected _PDF_p.", funcname);
    PyErr_SetString(PyExc_TypeError, text);
}






static PyObject *_wrap_PDF_activate_item(PyObject *self, PyObject *args)
{
    PDF *p;
    char *py_p = 0;
    int id;

    if (!PyArg_ParseTuple(args,"si:PDF_activate_item", &py_p, &id))
        return NULL;
    if (py_p && SWIG_GetPtr(py_p, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle("PDF_activate_item");
        return NULL;
    }

    try { PDF_activate_item(p, id);
    } catch;

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *_wrap_PDF_add_bookmark(PyObject *self, PyObject *args)
{
    PDF *p;
    char *py_p = 0;
    const char *text;
    int text_len;
    int parent;
    int open;
    int PDF_VOLATILE _result = 0;

    if (!PyArg_ParseTuple(args,"ss#ii:PDF_add_bookmark", &py_p, &text, &text_len, &parent, &open))
        return NULL;
    if (py_p && SWIG_GetPtr(py_p, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle("PDF_add_bookmark");
        return NULL;
    }

    try { _result = (int) PDF_add_bookmark2(p, text, text_len, parent, open);
    } catch;

    return Py_BuildValue("i", _result);
}

static PyObject *_wrap_PDF_add_launchlink(PyObject *self, PyObject *args)
{
    PDF *p;
    char *py_p = 0;
    double llx;
    double lly;
    double urx;
    double ury;
    const char *filename;

    if (!PyArg_ParseTuple(args,"sdddds:PDF_add_launchlink", &py_p, &llx, &lly, &urx, &ury, &filename))
        return NULL;
    if (py_p && SWIG_GetPtr(py_p, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle("PDF_add_launchlink");
        return NULL;
    }

    try { PDF_add_launchlink(p, llx, lly, urx, ury, filename);
    } catch;

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *_wrap_PDF_add_locallink(PyObject *self, PyObject *args)
{
    PDF *p;
    char *py_p = 0;
    double llx;
    double lly;
    double urx;
    double ury;
    int page;
    const char *optlist;

    if (!PyArg_ParseTuple(args,"sddddis:PDF_add_locallink", &py_p, &llx, &lly, &urx, &ury, &page, &optlist))
        return NULL;
    if (py_p && SWIG_GetPtr(py_p, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle("PDF_add_locallink");
        return NULL;
    }

    try { PDF_add_locallink(p, llx, lly, urx, ury, page, optlist);
    } catch;

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *_wrap_PDF_add_nameddest(PyObject *self, PyObject *args)
{
    PDF *p;
    char *py_p = 0;
    const char *name;
    int name_len;
    const char *optlist;

    if (!PyArg_ParseTuple(args,"ss#s:PDF_add_nameddest", &py_p, &name, &name_len, &optlist))
        return NULL;
    if (py_p && SWIG_GetPtr(py_p, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle("PDF_add_nameddest");
        return NULL;
    }

    try { PDF_add_nameddest(p, name, name_len, optlist);
    } catch;

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *_wrap_PDF_add_note(PyObject *self, PyObject *args)
{
    PDF *p;
    char *py_p = 0;
    double llx;
    double lly;
    double urx;
    double ury;
    const char *contents;
    int contents_len;
    const char *title;
    int title_len;
    const char *icon;
    int open;

    if (!PyArg_ParseTuple(args,"sdddds#s#si:PDF_add_note", &py_p, &llx, &lly, &urx, &ury, &contents, &contents_len, &title, &title_len, &icon, &open))
        return NULL;
    if (py_p && SWIG_GetPtr(py_p, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle("PDF_add_note");
        return NULL;
    }

    try { PDF_add_note2(p, llx, lly, urx, ury, contents, contents_len, title, title_len, icon, open);
    } catch;

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *_wrap_PDF_add_pdflink(PyObject *self, PyObject *args)
{
    PDF *p;
    char *py_p = 0;
    double llx;
    double lly;
    double urx;
    double ury;
    const char *filename;
    int page;
    const char *optlist;

    if (!PyArg_ParseTuple(args,"sddddsis:PDF_add_pdflink", &py_p, &llx, &lly, &urx, &ury, &filename, &page, &optlist))
        return NULL;
    if (py_p && SWIG_GetPtr(py_p, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle("PDF_add_pdflink");
        return NULL;
    }

    try { PDF_add_pdflink(p, llx, lly, urx, ury, filename, page, optlist);
    } catch;

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *_wrap_PDF_add_thumbnail(PyObject *self, PyObject *args)
{
    PDF *p;
    char *py_p = 0;
    int image;

    if (!PyArg_ParseTuple(args,"si:PDF_add_thumbnail", &py_p, &image))
        return NULL;
    if (py_p && SWIG_GetPtr(py_p, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle("PDF_add_thumbnail");
        return NULL;
    }

    try { PDF_add_thumbnail(p, image);
    } catch;

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *_wrap_PDF_add_weblink(PyObject *self, PyObject *args)
{
    PDF *p;
    char *py_p = 0;
    double llx;
    double lly;
    double urx;
    double ury;
    const char *url;

    if (!PyArg_ParseTuple(args,"sdddds:PDF_add_weblink", &py_p, &llx, &lly, &urx, &ury, &url))
        return NULL;
    if (py_p && SWIG_GetPtr(py_p, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle("PDF_add_weblink");
        return NULL;
    }

    try { PDF_add_weblink(p, llx, lly, urx, ury, url);
    } catch;

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *_wrap_PDF_arc(PyObject *self, PyObject *args)
{
    PDF *p;
    char *py_p = 0;
    double x;
    double y;
    double r;
    double alpha;
    double beta;

    if (!PyArg_ParseTuple(args,"sddddd:PDF_arc", &py_p, &x, &y, &r, &alpha, &beta))
        return NULL;
    if (py_p && SWIG_GetPtr(py_p, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle("PDF_arc");
        return NULL;
    }

    try { PDF_arc(p, x, y, r, alpha, beta);
    } catch;

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *_wrap_PDF_arcn(PyObject *self, PyObject *args)
{
    PDF *p;
    char *py_p = 0;
    double x;
    double y;
    double r;
    double alpha;
    double beta;

    if (!PyArg_ParseTuple(args,"sddddd:PDF_arcn", &py_p, &x, &y, &r, &alpha, &beta))
        return NULL;
    if (py_p && SWIG_GetPtr(py_p, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle("PDF_arcn");
        return NULL;
    }

    try { PDF_arcn(p, x, y, r, alpha, beta);
    } catch;

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *_wrap_PDF_attach_file(PyObject *self, PyObject *args)
{
    PDF *p;
    char *py_p = 0;
    double llx;
    double lly;
    double urx;
    double ury;
    const char *filename;
    const char *description;
    int description_len;
    const char *author;
    int author_len;
    const char *mimetype;
    const char *icon;

    if (!PyArg_ParseTuple(args,"sddddss#s#ss:PDF_attach_file", &py_p, &llx, &lly, &urx, &ury, &filename, &description, &description_len, &author, &author_len, &mimetype, &icon))
        return NULL;
    if (py_p && SWIG_GetPtr(py_p, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle("PDF_attach_file");
        return NULL;
    }

    try { PDF_attach_file2(p, llx, lly, urx, ury, filename, 0, description, description_len, author, author_len, mimetype, icon);
    } catch;

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *_wrap_PDF_begin_document(PyObject *self, PyObject *args)
{
    PDF *p;
    char *py_p = 0;
    const char *filename;
    const char *optlist;
    int PDF_VOLATILE _result = 0;

    if (!PyArg_ParseTuple(args,"sss:PDF_begin_document", &py_p, &filename, &optlist))
        return NULL;
    if (py_p && SWIG_GetPtr(py_p, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle("PDF_begin_document");
        return NULL;
    }

    try { _result = (int) PDF_begin_document(p, filename, 0, optlist);
    } catch;

    return Py_BuildValue("i", _result);
}

static PyObject *_wrap_PDF_begin_font(PyObject *self, PyObject *args)
{
    PDF *p;
    char *py_p = 0;
    const char *fontname;
    double a;
    double b;
    double c;
    double d;
    double e;
    double f;
    const char *optlist;

    if (!PyArg_ParseTuple(args,"ssdddddds:PDF_begin_font", &py_p, &fontname, &a, &b, &c, &d, &e, &f, &optlist))
        return NULL;
    if (py_p && SWIG_GetPtr(py_p, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle("PDF_begin_font");
        return NULL;
    }

    try { PDF_begin_font(p, fontname, 0, a, b, c, d, e, f, optlist);
    } catch;

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *_wrap_PDF_begin_glyph(PyObject *self, PyObject *args)
{
    PDF *p;
    char *py_p = 0;
    const char *glyphname;
    double wx;
    double llx;
    double lly;
    double urx;
    double ury;

    if (!PyArg_ParseTuple(args,"ssddddd:PDF_begin_glyph", &py_p, &glyphname, &wx, &llx, &lly, &urx, &ury))
        return NULL;
    if (py_p && SWIG_GetPtr(py_p, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle("PDF_begin_glyph");
        return NULL;
    }

    try { PDF_begin_glyph(p, glyphname, wx, llx, lly, urx, ury);
    } catch;

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *_wrap_PDF_begin_item(PyObject *self, PyObject *args)
{
    PDF *p;
    char *py_p = 0;
    const char *tag;
    const char *optlist;
    int PDF_VOLATILE _result = 0;

    if (!PyArg_ParseTuple(args,"sss:PDF_begin_item", &py_p, &tag, &optlist))
        return NULL;
    if (py_p && SWIG_GetPtr(py_p, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle("PDF_begin_item");
        return NULL;
    }

    try { _result = (int) PDF_begin_item(p, tag, optlist);
    } catch;

    return Py_BuildValue("i", _result);
}

static PyObject *_wrap_PDF_begin_layer(PyObject *self, PyObject *args)
{
    PDF *p;
    char *py_p = 0;
    int layer;

    if (!PyArg_ParseTuple(args,"si:PDF_begin_layer", &py_p, &layer))
        return NULL;
    if (py_p && SWIG_GetPtr(py_p, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle("PDF_begin_layer");
        return NULL;
    }

    try { PDF_begin_layer(p, layer);
    } catch;

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *_wrap_PDF_begin_mc(PyObject *self, PyObject *args)
{
    PDF *p;
    char *py_p = 0;
    const char *tag;
    const char *optlist;

    if (!PyArg_ParseTuple(args,"sss:PDF_begin_mc", &py_p, &tag, &optlist))
        return NULL;
    if (py_p && SWIG_GetPtr(py_p, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle("PDF_begin_mc");
        return NULL;
    }

    try { PDF_begin_mc(p, tag, optlist);
    } catch;

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *_wrap_PDF_begin_page(PyObject *self, PyObject *args)
{
    PDF *p;
    char *py_p = 0;
    double width;
    double height;

    if (!PyArg_ParseTuple(args,"sdd:PDF_begin_page", &py_p, &width, &height))
        return NULL;
    if (py_p && SWIG_GetPtr(py_p, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle("PDF_begin_page");
        return NULL;
    }

    try { PDF_begin_page(p, width, height);
    } catch;

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *_wrap_PDF_begin_page_ext(PyObject *self, PyObject *args)
{
    PDF *p;
    char *py_p = 0;
    double width;
    double height;
    const char *optlist;

    if (!PyArg_ParseTuple(args,"sdds:PDF_begin_page_ext", &py_p, &width, &height, &optlist))
        return NULL;
    if (py_p && SWIG_GetPtr(py_p, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle("PDF_begin_page_ext");
        return NULL;
    }

    try { PDF_begin_page_ext(p, width, height, optlist);
    } catch;

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *_wrap_PDF_begin_pattern(PyObject *self, PyObject *args)
{
    PDF *p;
    char *py_p = 0;
    double width;
    double height;
    double xstep;
    double ystep;
    int painttype;
    int PDF_VOLATILE _result = 0;

    if (!PyArg_ParseTuple(args,"sddddi:PDF_begin_pattern", &py_p, &width, &height, &xstep, &ystep, &painttype))
        return NULL;
    if (py_p && SWIG_GetPtr(py_p, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle("PDF_begin_pattern");
        return NULL;
    }

    try { _result = (int) PDF_begin_pattern(p, width, height, xstep, ystep, painttype);
    } catch;

    return Py_BuildValue("i", _result);
}

static PyObject *_wrap_PDF_begin_template(PyObject *self, PyObject *args)
{
    PDF *p;
    char *py_p = 0;
    double width;
    double height;
    int PDF_VOLATILE _result = 0;

    if (!PyArg_ParseTuple(args,"sdd:PDF_begin_template", &py_p, &width, &height))
        return NULL;
    if (py_p && SWIG_GetPtr(py_p, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle("PDF_begin_template");
        return NULL;
    }

    try { _result = (int) PDF_begin_template(p, width, height);
    } catch;

    return Py_BuildValue("i", _result);
}

static PyObject *_wrap_PDF_circle(PyObject *self, PyObject *args)
{
    PDF *p;
    char *py_p = 0;
    double x;
    double y;
    double r;

    if (!PyArg_ParseTuple(args,"sddd:PDF_circle", &py_p, &x, &y, &r))
        return NULL;
    if (py_p && SWIG_GetPtr(py_p, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle("PDF_circle");
        return NULL;
    }

    try { PDF_circle(p, x, y, r);
    } catch;

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *_wrap_PDF_clip(PyObject *self, PyObject *args)
{
    PDF *p;
    char *py_p = 0;

    if (!PyArg_ParseTuple(args,"s:PDF_clip", &py_p))
        return NULL;
    if (py_p && SWIG_GetPtr(py_p, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle("PDF_clip");
        return NULL;
    }

    try { PDF_clip(p);
    } catch;

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *_wrap_PDF_close(PyObject *self, PyObject *args)
{
    PDF *p;
    char *py_p = 0;

    if (!PyArg_ParseTuple(args,"s:PDF_close", &py_p))
        return NULL;
    if (py_p && SWIG_GetPtr(py_p, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle("PDF_close");
        return NULL;
    }

    try { PDF_close(p);
    } catch;

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *_wrap_PDF_close_image(PyObject *self, PyObject *args)
{
    PDF *p;
    char *py_p = 0;
    int image;

    if (!PyArg_ParseTuple(args,"si:PDF_close_image", &py_p, &image))
        return NULL;
    if (py_p && SWIG_GetPtr(py_p, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle("PDF_close_image");
        return NULL;
    }

    try { PDF_close_image(p, image);
    } catch;

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *_wrap_PDF_close_pdi(PyObject *self, PyObject *args)
{
    PDF *p;
    char *py_p = 0;
    int doc;

    if (!PyArg_ParseTuple(args,"si:PDF_close_pdi", &py_p, &doc))
        return NULL;
    if (py_p && SWIG_GetPtr(py_p, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle("PDF_close_pdi");
        return NULL;
    }

    try { PDF_close_pdi(p, doc);
    } catch;

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *_wrap_PDF_close_pdi_page(PyObject *self, PyObject *args)
{
    PDF *p;
    char *py_p = 0;
    int page;

    if (!PyArg_ParseTuple(args,"si:PDF_close_pdi_page", &py_p, &page))
        return NULL;
    if (py_p && SWIG_GetPtr(py_p, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle("PDF_close_pdi_page");
        return NULL;
    }

    try { PDF_close_pdi_page(p, page);
    } catch;

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *_wrap_PDF_closepath(PyObject *self, PyObject *args)
{
    PDF *p;
    char *py_p = 0;

    if (!PyArg_ParseTuple(args,"s:PDF_closepath", &py_p))
        return NULL;
    if (py_p && SWIG_GetPtr(py_p, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle("PDF_closepath");
        return NULL;
    }

    try { PDF_closepath(p);
    } catch;

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *_wrap_PDF_closepath_fill_stroke(PyObject *self, PyObject *args)
{
    PDF *p;
    char *py_p = 0;

    if (!PyArg_ParseTuple(args,"s:PDF_closepath_fill_stroke", &py_p))
        return NULL;
    if (py_p && SWIG_GetPtr(py_p, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle("PDF_closepath_fill_stroke");
        return NULL;
    }

    try { PDF_closepath_fill_stroke(p);
    } catch;

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *_wrap_PDF_closepath_stroke(PyObject *self, PyObject *args)
{
    PDF *p;
    char *py_p = 0;

    if (!PyArg_ParseTuple(args,"s:PDF_closepath_stroke", &py_p))
        return NULL;
    if (py_p && SWIG_GetPtr(py_p, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle("PDF_closepath_stroke");
        return NULL;
    }

    try { PDF_closepath_stroke(p);
    } catch;

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *_wrap_PDF_concat(PyObject *self, PyObject *args)
{
    PDF *p;
    char *py_p = 0;
    double a;
    double b;
    double c;
    double d;
    double e;
    double f;

    if (!PyArg_ParseTuple(args,"sdddddd:PDF_concat", &py_p, &a, &b, &c, &d, &e, &f))
        return NULL;
    if (py_p && SWIG_GetPtr(py_p, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle("PDF_concat");
        return NULL;
    }

    try { PDF_concat(p, a, b, c, d, e, f);
    } catch;

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *_wrap_PDF_continue_text(PyObject *self, PyObject *args)
{
    PDF *p;
    char *py_p = 0;
    const char *text;
    int text_len;

    if (!PyArg_ParseTuple(args,"ss#:PDF_continue_text", &py_p, &text, &text_len))
        return NULL;
    if (py_p && SWIG_GetPtr(py_p, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle("PDF_continue_text");
        return NULL;
    }

    try { PDF_continue_text2(p, text, text_len);
    } catch;

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *_wrap_PDF_create_action(PyObject *self, PyObject *args)
{
    PDF *p;
    char *py_p = 0;
    const char *type;
    const char *optlist;
    int PDF_VOLATILE _result = 0;

    if (!PyArg_ParseTuple(args,"sss:PDF_create_action", &py_p, &type, &optlist))
        return NULL;
    if (py_p && SWIG_GetPtr(py_p, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle("PDF_create_action");
        return NULL;
    }

    try { _result = (int) PDF_create_action(p, type, optlist);
    } catch;

    return Py_BuildValue("i", _result);
}

static PyObject *_wrap_PDF_create_annotation(PyObject *self, PyObject *args)
{
    PDF *p;
    char *py_p = 0;
    double llx;
    double lly;
    double urx;
    double ury;
    const char *type;
    const char *optlist;

    if (!PyArg_ParseTuple(args,"sddddss:PDF_create_annotation", &py_p, &llx, &lly, &urx, &ury, &type, &optlist))
        return NULL;
    if (py_p && SWIG_GetPtr(py_p, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle("PDF_create_annotation");
        return NULL;
    }

    try { PDF_create_annotation(p, llx, lly, urx, ury, type, optlist);
    } catch;

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *_wrap_PDF_create_bookmark(PyObject *self, PyObject *args)
{
    PDF *p;
    char *py_p = 0;
    const char *text;
    int text_len;
    const char *optlist;
    int PDF_VOLATILE _result = 0;

    if (!PyArg_ParseTuple(args,"ss#s:PDF_create_bookmark", &py_p, &text, &text_len, &optlist))
        return NULL;
    if (py_p && SWIG_GetPtr(py_p, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle("PDF_create_bookmark");
        return NULL;
    }

    try { _result = (int) PDF_create_bookmark(p, text, text_len, optlist);
    } catch;

    return Py_BuildValue("i", _result);
}

static PyObject *_wrap_PDF_create_field(PyObject *self, PyObject *args)
{
    PDF *p;
    char *py_p = 0;
    double llx;
    double lly;
    double urx;
    double ury;
    const char *name;
    int name_len;
    const char *type;
    const char *optlist;

    if (!PyArg_ParseTuple(args,"sdddds#ss:PDF_create_field", &py_p, &llx, &lly, &urx, &ury, &name, &name_len, &type, &optlist))
        return NULL;
    if (py_p && SWIG_GetPtr(py_p, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle("PDF_create_field");
        return NULL;
    }

    try { PDF_create_field(p, llx, lly, urx, ury, name, name_len, type, optlist);
    } catch;

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *_wrap_PDF_create_fieldgroup(PyObject *self, PyObject *args)
{
    PDF *p;
    char *py_p = 0;
    const char *name;
    int name_len;
    const char *optlist;

    if (!PyArg_ParseTuple(args,"ss#s:PDF_create_fieldgroup", &py_p, &name, &name_len, &optlist))
        return NULL;
    if (py_p && SWIG_GetPtr(py_p, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle("PDF_create_fieldgroup");
        return NULL;
    }

    try { PDF_create_fieldgroup(p, name, name_len, optlist);
    } catch;

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *_wrap_PDF_create_gstate(PyObject *self, PyObject *args)
{
    PDF *p;
    char *py_p = 0;
    const char *optlist;
    int PDF_VOLATILE _result = 0;

    if (!PyArg_ParseTuple(args,"ss:PDF_create_gstate", &py_p, &optlist))
        return NULL;
    if (py_p && SWIG_GetPtr(py_p, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle("PDF_create_gstate");
        return NULL;
    }

    try { _result = (int) PDF_create_gstate(p, optlist);
    } catch;

    return Py_BuildValue("i", _result);
}

static PyObject *_wrap_PDF_create_pvf(PyObject *self, PyObject *args)
{
    PDF *p;
    char *py_p = 0;
    const char *filename;
    const void *data;
    int data_len;
    const char *optlist;

    if (!PyArg_ParseTuple(args,"sss#s:PDF_create_pvf", &py_p, &filename, &data, &data_len, &optlist))
        return NULL;
    if (py_p && SWIG_GetPtr(py_p, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle("PDF_create_pvf");
        return NULL;
    }

    try { PDF_create_pvf(p, filename, 0, data, (size_t) data_len, optlist);
    } catch;

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *_wrap_PDF_create_textflow(PyObject *self, PyObject *args)
{
    PDF *p;
    char *py_p = 0;
    const char *text;
    int text_len;
    const char *optlist;
    int PDF_VOLATILE _result = 0;

    if (!PyArg_ParseTuple(args,"ss#s:PDF_create_textflow", &py_p, &text, &text_len, &optlist))
        return NULL;
    if (py_p && SWIG_GetPtr(py_p, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle("PDF_create_textflow");
        return NULL;
    }

    try { _result = (int) PDF_create_textflow(p, text, text_len, optlist);
    } catch;

    return Py_BuildValue("i", _result);
}

static PyObject *_wrap_PDF_curveto(PyObject *self, PyObject *args)
{
    PDF *p;
    char *py_p = 0;
    double x_1;
    double y_1;
    double x_2;
    double y_2;
    double x_3;
    double y_3;

    if (!PyArg_ParseTuple(args,"sdddddd:PDF_curveto", &py_p, &x_1, &y_1, &x_2, &y_2, &x_3, &y_3))
        return NULL;
    if (py_p && SWIG_GetPtr(py_p, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle("PDF_curveto");
        return NULL;
    }

    try { PDF_curveto(p, x_1, y_1, x_2, y_2, x_3, y_3);
    } catch;

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *_wrap_PDF_define_layer(PyObject *self, PyObject *args)
{
    PDF *p;
    char *py_p = 0;
    const char *name;
    int name_len;
    const char *optlist;
    int PDF_VOLATILE _result = 0;

    if (!PyArg_ParseTuple(args,"ss#s:PDF_define_layer", &py_p, &name, &name_len, &optlist))
        return NULL;
    if (py_p && SWIG_GetPtr(py_p, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle("PDF_define_layer");
        return NULL;
    }

    try { _result = (int) PDF_define_layer(p, name, name_len, optlist);
    } catch;

    return Py_BuildValue("i", _result);
}

static PyObject *_wrap_PDF_delete(PyObject *self, PyObject *args)
{
    PDF *p;
    char *py_p = 0;

    if (!PyArg_ParseTuple(args,"s:PDF_delete", &py_p))
        return NULL;
    if (py_p && SWIG_GetPtr(py_p, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle("PDF_delete");
        return NULL;
    }

    PDF_delete(p);

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *_wrap_PDF_delete_pvf(PyObject *self, PyObject *args)
{
    PDF *p;
    char *py_p = 0;
    const char *filename;
    int PDF_VOLATILE _result = 0;

    if (!PyArg_ParseTuple(args,"ss:PDF_delete_pvf", &py_p, &filename))
        return NULL;
    if (py_p && SWIG_GetPtr(py_p, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle("PDF_delete_pvf");
        return NULL;
    }

    try { _result = (int) PDF_delete_pvf(p, filename, 0);
    } catch;

    return Py_BuildValue("i", _result);
}

static PyObject *_wrap_PDF_delete_textflow(PyObject *self, PyObject *args)
{
    PDF *p;
    char *py_p = 0;
    int textflow;

    if (!PyArg_ParseTuple(args,"si:PDF_delete_textflow", &py_p, &textflow))
        return NULL;
    if (py_p && SWIG_GetPtr(py_p, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle("PDF_delete_textflow");
        return NULL;
    }

    try { PDF_delete_textflow(p, textflow);
    } catch;

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *_wrap_PDF_encoding_set_char(PyObject *self, PyObject *args)
{
    PDF *p;
    char *py_p = 0;
    const char *encoding;
    int slot;
    const char *glyphname;
    int uv;

    if (!PyArg_ParseTuple(args,"ssisi:PDF_encoding_set_char", &py_p, &encoding, &slot, &glyphname, &uv))
        return NULL;
    if (py_p && SWIG_GetPtr(py_p, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle("PDF_encoding_set_char");
        return NULL;
    }

    try { PDF_encoding_set_char(p, encoding, slot, glyphname, uv);
    } catch;

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *_wrap_PDF_end_document(PyObject *self, PyObject *args)
{
    PDF *p;
    char *py_p = 0;
    const char *optlist;

    if (!PyArg_ParseTuple(args,"ss:PDF_end_document", &py_p, &optlist))
        return NULL;
    if (py_p && SWIG_GetPtr(py_p, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle("PDF_end_document");
        return NULL;
    }

    try { PDF_end_document(p, optlist);
    } catch;

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *_wrap_PDF_end_font(PyObject *self, PyObject *args)
{
    PDF *p;
    char *py_p = 0;

    if (!PyArg_ParseTuple(args,"s:PDF_end_font", &py_p))
        return NULL;
    if (py_p && SWIG_GetPtr(py_p, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle("PDF_end_font");
        return NULL;
    }

    try { PDF_end_font(p);
    } catch;

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *_wrap_PDF_end_glyph(PyObject *self, PyObject *args)
{
    PDF *p;
    char *py_p = 0;

    if (!PyArg_ParseTuple(args,"s:PDF_end_glyph", &py_p))
        return NULL;
    if (py_p && SWIG_GetPtr(py_p, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle("PDF_end_glyph");
        return NULL;
    }

    try { PDF_end_glyph(p);
    } catch;

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *_wrap_PDF_end_item(PyObject *self, PyObject *args)
{
    PDF *p;
    char *py_p = 0;
    int id;

    if (!PyArg_ParseTuple(args,"si:PDF_end_item", &py_p, &id))
        return NULL;
    if (py_p && SWIG_GetPtr(py_p, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle("PDF_end_item");
        return NULL;
    }

    try { PDF_end_item(p, id);
    } catch;

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *_wrap_PDF_end_layer(PyObject *self, PyObject *args)
{
    PDF *p;
    char *py_p = 0;

    if (!PyArg_ParseTuple(args,"s:PDF_end_layer", &py_p))
        return NULL;
    if (py_p && SWIG_GetPtr(py_p, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle("PDF_end_layer");
        return NULL;
    }

    try { PDF_end_layer(p);
    } catch;

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *_wrap_PDF_end_mc(PyObject *self, PyObject *args)
{
    PDF *p;
    char *py_p = 0;

    if (!PyArg_ParseTuple(args,"s:PDF_end_mc", &py_p))
        return NULL;
    if (py_p && SWIG_GetPtr(py_p, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle("PDF_end_mc");
        return NULL;
    }

    try { PDF_end_mc(p);
    } catch;

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *_wrap_PDF_end_page(PyObject *self, PyObject *args)
{
    PDF *p;
    char *py_p = 0;

    if (!PyArg_ParseTuple(args,"s:PDF_end_page", &py_p))
        return NULL;
    if (py_p && SWIG_GetPtr(py_p, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle("PDF_end_page");
        return NULL;
    }

    try { PDF_end_page(p);
    } catch;

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *_wrap_PDF_end_page_ext(PyObject *self, PyObject *args)
{
    PDF *p;
    char *py_p = 0;
    const char *optlist;

    if (!PyArg_ParseTuple(args,"ss:PDF_end_page_ext", &py_p, &optlist))
        return NULL;
    if (py_p && SWIG_GetPtr(py_p, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle("PDF_end_page_ext");
        return NULL;
    }

    try { PDF_end_page_ext(p, optlist);
    } catch;

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *_wrap_PDF_end_pattern(PyObject *self, PyObject *args)
{
    PDF *p;
    char *py_p = 0;

    if (!PyArg_ParseTuple(args,"s:PDF_end_pattern", &py_p))
        return NULL;
    if (py_p && SWIG_GetPtr(py_p, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle("PDF_end_pattern");
        return NULL;
    }

    try { PDF_end_pattern(p);
    } catch;

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *_wrap_PDF_end_template(PyObject *self, PyObject *args)
{
    PDF *p;
    char *py_p = 0;

    if (!PyArg_ParseTuple(args,"s:PDF_end_template", &py_p))
        return NULL;
    if (py_p && SWIG_GetPtr(py_p, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle("PDF_end_template");
        return NULL;
    }

    try { PDF_end_template(p);
    } catch;

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *_wrap_PDF_endpath(PyObject *self, PyObject *args)
{
    PDF *p;
    char *py_p = 0;

    if (!PyArg_ParseTuple(args,"s:PDF_endpath", &py_p))
        return NULL;
    if (py_p && SWIG_GetPtr(py_p, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle("PDF_endpath");
        return NULL;
    }

    try { PDF_endpath(p);
    } catch;

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *_wrap_PDF_fill(PyObject *self, PyObject *args)
{
    PDF *p;
    char *py_p = 0;

    if (!PyArg_ParseTuple(args,"s:PDF_fill", &py_p))
        return NULL;
    if (py_p && SWIG_GetPtr(py_p, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle("PDF_fill");
        return NULL;
    }

    try { PDF_fill(p);
    } catch;

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *_wrap_PDF_fill_imageblock(PyObject *self, PyObject *args)
{
    PDF *p;
    char *py_p = 0;
    int page;
    const char *blockname;
    int image;
    const char *optlist;
    int PDF_VOLATILE _result = 0;

    if (!PyArg_ParseTuple(args,"sisis:PDF_fill_imageblock", &py_p, &page, &blockname, &image, &optlist))
        return NULL;
    if (py_p && SWIG_GetPtr(py_p, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle("PDF_fill_imageblock");
        return NULL;
    }

    try { _result = (int) PDF_fill_imageblock(p, page, blockname, image, optlist);
    } catch;

    return Py_BuildValue("i", _result);
}

static PyObject *_wrap_PDF_fill_pdfblock(PyObject *self, PyObject *args)
{
    PDF *p;
    char *py_p = 0;
    int page;
    const char *blockname;
    int contents;
    const char *optlist;
    int PDF_VOLATILE _result = 0;

    if (!PyArg_ParseTuple(args,"sisis:PDF_fill_pdfblock", &py_p, &page, &blockname, &contents, &optlist))
        return NULL;
    if (py_p && SWIG_GetPtr(py_p, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle("PDF_fill_pdfblock");
        return NULL;
    }

    try { _result = (int) PDF_fill_pdfblock(p, page, blockname, contents, optlist);
    } catch;

    return Py_BuildValue("i", _result);
}

static PyObject *_wrap_PDF_fill_stroke(PyObject *self, PyObject *args)
{
    PDF *p;
    char *py_p = 0;

    if (!PyArg_ParseTuple(args,"s:PDF_fill_stroke", &py_p))
        return NULL;
    if (py_p && SWIG_GetPtr(py_p, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle("PDF_fill_stroke");
        return NULL;
    }

    try { PDF_fill_stroke(p);
    } catch;

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *_wrap_PDF_fill_textblock(PyObject *self, PyObject *args)
{
    PDF *p;
    char *py_p = 0;
    int page;
    const char *blockname;
    const char *text;
    int text_len;
    const char *optlist;
    int PDF_VOLATILE _result = 0;

    if (!PyArg_ParseTuple(args,"siss#s:PDF_fill_textblock", &py_p, &page, &blockname, &text, &text_len, &optlist))
        return NULL;
    if (py_p && SWIG_GetPtr(py_p, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle("PDF_fill_textblock");
        return NULL;
    }

    try { _result = (int) PDF_fill_textblock(p, page, blockname, text, text_len, optlist);
    } catch;

    return Py_BuildValue("i", _result);
}

static PyObject *_wrap_PDF_findfont(PyObject *self, PyObject *args)
{
    PDF *p;
    char *py_p = 0;
    const char *fontname;
    const char *encoding;
    int embed;
    int PDF_VOLATILE _result = 0;

    if (!PyArg_ParseTuple(args,"sssi:PDF_findfont", &py_p, &fontname, &encoding, &embed))
        return NULL;
    if (py_p && SWIG_GetPtr(py_p, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle("PDF_findfont");
        return NULL;
    }

    try { _result = (int) PDF_findfont(p, fontname, encoding, embed);
    } catch;

    return Py_BuildValue("i", _result);
}

static PyObject *_wrap_PDF_fit_image(PyObject *self, PyObject *args)
{
    PDF *p;
    char *py_p = 0;
    int image;
    double x;
    double y;
    const char *optlist;

    if (!PyArg_ParseTuple(args,"sidds:PDF_fit_image", &py_p, &image, &x, &y, &optlist))
        return NULL;
    if (py_p && SWIG_GetPtr(py_p, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle("PDF_fit_image");
        return NULL;
    }

    try { PDF_fit_image(p, image, x, y, optlist);
    } catch;

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *_wrap_PDF_fit_pdi_page(PyObject *self, PyObject *args)
{
    PDF *p;
    char *py_p = 0;
    int page;
    double x;
    double y;
    const char *optlist;

    if (!PyArg_ParseTuple(args,"sidds:PDF_fit_pdi_page", &py_p, &page, &x, &y, &optlist))
        return NULL;
    if (py_p && SWIG_GetPtr(py_p, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle("PDF_fit_pdi_page");
        return NULL;
    }

    try { PDF_fit_pdi_page(p, page, x, y, optlist);
    } catch;

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *_wrap_PDF_fit_textflow(PyObject *self, PyObject *args)
{
    PDF *p;
    char *py_p = 0;
    int textflow;
    double llx;
    double lly;
    double urx;
    double ury;
    const char *optlist;
    char PDF_VOLATILE *_result = NULL;

    if (!PyArg_ParseTuple(args,"sidddds:PDF_fit_textflow", &py_p, &textflow, &llx, &lly, &urx, &ury, &optlist))
        return NULL;
    if (py_p && SWIG_GetPtr(py_p, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle("PDF_fit_textflow");
        return NULL;
    }

    try { _result = (char *) PDF_fit_textflow(p, textflow, llx, lly, urx, ury, optlist);
    } catch;

    return Py_BuildValue("s", _result);
}

static PyObject *_wrap_PDF_fit_textline(PyObject *self, PyObject *args)
{
    PDF *p;
    char *py_p = 0;
    const char *text;
    int text_len;
    double x;
    double y;
    const char *optlist;

    if (!PyArg_ParseTuple(args,"ss#dds:PDF_fit_textline", &py_p, &text, &text_len, &x, &y, &optlist))
        return NULL;
    if (py_p && SWIG_GetPtr(py_p, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle("PDF_fit_textline");
        return NULL;
    }

    try { PDF_fit_textline(p, text, text_len, x, y, optlist);
    } catch;

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *_wrap_PDF_get_apiname(PyObject *self, PyObject *args)
{
    PDF *p;
    char *py_p = 0;
    char PDF_VOLATILE *_result = NULL;

    if (!PyArg_ParseTuple(args,"s:PDF_get_apiname", &py_p))
        return NULL;
    if (py_p && SWIG_GetPtr(py_p, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle("PDF_get_apiname");
        return NULL;
    }

    try { _result = (char *) PDF_get_apiname(p);
    } catch;

    return Py_BuildValue("s", _result);
}

static PyObject *_wrap_PDF_get_buffer(PyObject *self, PyObject *args)
{
    PDF *p;
    char *py_p = 0;
    long size;
    char PDF_VOLATILE *_result = NULL;

    if (!PyArg_ParseTuple(args,"s:PDF_get_buffer", &py_p))
        return NULL;
    if (py_p && SWIG_GetPtr(py_p, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle("PDF_get_buffer");
        return NULL;
    }

    try { _result = (char *) PDF_get_buffer(p, &size);
    } catch;

    return Py_BuildValue("s#", _result, size);
}

static PyObject *_wrap_PDF_get_errmsg(PyObject *self, PyObject *args)
{
    PDF *p;
    char *py_p = 0;
    char PDF_VOLATILE *_result = NULL;

    if (!PyArg_ParseTuple(args,"s:PDF_get_errmsg", &py_p))
        return NULL;
    if (py_p && SWIG_GetPtr(py_p, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle("PDF_get_errmsg");
        return NULL;
    }

    try { _result = (char *) PDF_get_errmsg(p);
    } catch;

    return Py_BuildValue("s", _result);
}

static PyObject *_wrap_PDF_get_errnum(PyObject *self, PyObject *args)
{
    PDF *p;
    char *py_p = 0;
    int PDF_VOLATILE _result = 0;

    if (!PyArg_ParseTuple(args,"s:PDF_get_errnum", &py_p))
        return NULL;
    if (py_p && SWIG_GetPtr(py_p, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle("PDF_get_errnum");
        return NULL;
    }

    try { _result = (int) PDF_get_errnum(p);
    } catch;

    return Py_BuildValue("i", _result);
}

static PyObject *_wrap_PDF_get_parameter(PyObject *self, PyObject *args)
{
    PDF *p;
    char *py_p = 0;
    const char *key;
    double modifier;
    char PDF_VOLATILE *_result = NULL;

    if (!PyArg_ParseTuple(args,"ssd:PDF_get_parameter", &py_p, &key, &modifier))
        return NULL;
    if (py_p && SWIG_GetPtr(py_p, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle("PDF_get_parameter");
        return NULL;
    }

    try { _result = (char *) PDF_get_parameter(p, key, modifier);
    } catch;

    return Py_BuildValue("s", _result);
}

static PyObject *_wrap_PDF_get_pdi_parameter(PyObject *self, PyObject *args)
{
    PDF *p;
    char *py_p = 0;
    const char *key;
    int doc;
    int page;
    int reserved;
    char PDF_VOLATILE *_result = NULL;

    if (!PyArg_ParseTuple(args,"ssiii:PDF_get_pdi_parameter", &py_p, &key, &doc, &page, &reserved))
        return NULL;
    if (py_p && SWIG_GetPtr(py_p, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle("PDF_get_pdi_parameter");
        return NULL;
    }

    try { _result = (char *) PDF_get_pdi_parameter(p, key, doc, page, reserved, NULL);
    } catch;

    return Py_BuildValue("s", _result);
}

static PyObject *_wrap_PDF_get_pdi_value(PyObject *self, PyObject *args)
{
    PDF *p;
    char *py_p = 0;
    const char *key;
    int doc;
    int page;
    int reserved;
    double PDF_VOLATILE _result = 0;

    if (!PyArg_ParseTuple(args,"ssiii:PDF_get_pdi_value", &py_p, &key, &doc, &page, &reserved))
        return NULL;
    if (py_p && SWIG_GetPtr(py_p, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle("PDF_get_pdi_value");
        return NULL;
    }

    try { _result = (double) PDF_get_pdi_value(p, key, doc, page, reserved);
    } catch;

    return Py_BuildValue("d", _result);
}

static PyObject *_wrap_PDF_get_value(PyObject *self, PyObject *args)
{
    PDF *p;
    char *py_p = 0;
    const char *key;
    double modifier;
    double PDF_VOLATILE _result = 0;

    if (!PyArg_ParseTuple(args,"ssd:PDF_get_value", &py_p, &key, &modifier))
        return NULL;
    if (py_p && SWIG_GetPtr(py_p, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle("PDF_get_value");
        return NULL;
    }

    try { _result = (double) PDF_get_value(p, key, modifier);
    } catch;

    return Py_BuildValue("d", _result);
}

static PyObject *_wrap_PDF_info_textflow(PyObject *self, PyObject *args)
{
    PDF *p;
    char *py_p = 0;
    int textflow;
    const char *keyword;
    double PDF_VOLATILE _result = 0;

    if (!PyArg_ParseTuple(args,"sis:PDF_info_textflow", &py_p, &textflow, &keyword))
        return NULL;
    if (py_p && SWIG_GetPtr(py_p, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle("PDF_info_textflow");
        return NULL;
    }

    try { _result = (double) PDF_info_textflow(p, textflow, keyword);
    } catch;

    return Py_BuildValue("d", _result);
}

static PyObject *_wrap_PDF_initgraphics(PyObject *self, PyObject *args)
{
    PDF *p;
    char *py_p = 0;

    if (!PyArg_ParseTuple(args,"s:PDF_initgraphics", &py_p))
        return NULL;
    if (py_p && SWIG_GetPtr(py_p, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle("PDF_initgraphics");
        return NULL;
    }

    try { PDF_initgraphics(p);
    } catch;

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *_wrap_PDF_lineto(PyObject *self, PyObject *args)
{
    PDF *p;
    char *py_p = 0;
    double x;
    double y;

    if (!PyArg_ParseTuple(args,"sdd:PDF_lineto", &py_p, &x, &y))
        return NULL;
    if (py_p && SWIG_GetPtr(py_p, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle("PDF_lineto");
        return NULL;
    }

    try { PDF_lineto(p, x, y);
    } catch;

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *_wrap_PDF_load_font(PyObject *self, PyObject *args)
{
    PDF *p;
    char *py_p = 0;
    const char *fontname;
    const char *encoding;
    const char *optlist;
    int PDF_VOLATILE _result = 0;

    if (!PyArg_ParseTuple(args,"ssss:PDF_load_font", &py_p, &fontname, &encoding, &optlist))
        return NULL;
    if (py_p && SWIG_GetPtr(py_p, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle("PDF_load_font");
        return NULL;
    }

    try { _result = (int) PDF_load_font(p, fontname, 0, encoding, optlist);
    } catch;

    return Py_BuildValue("i", _result);
}

static PyObject *_wrap_PDF_load_iccprofile(PyObject *self, PyObject *args)
{
    PDF *p;
    char *py_p = 0;
    const char *profilename;
    const char *optlist;
    int PDF_VOLATILE _result = 0;

    if (!PyArg_ParseTuple(args,"sss:PDF_load_iccprofile", &py_p, &profilename, &optlist))
        return NULL;
    if (py_p && SWIG_GetPtr(py_p, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle("PDF_load_iccprofile");
        return NULL;
    }

    try { _result = (int) PDF_load_iccprofile(p, profilename, 0, optlist);
    } catch;

    return Py_BuildValue("i", _result);
}

static PyObject *_wrap_PDF_load_image(PyObject *self, PyObject *args)
{
    PDF *p;
    char *py_p = 0;
    const char *imagetype;
    const char *filename;
    const char *optlist;
    int PDF_VOLATILE _result = 0;

    if (!PyArg_ParseTuple(args,"ssss:PDF_load_image", &py_p, &imagetype, &filename, &optlist))
        return NULL;
    if (py_p && SWIG_GetPtr(py_p, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle("PDF_load_image");
        return NULL;
    }

    try { _result = (int) PDF_load_image(p, imagetype, filename, 0, optlist);
    } catch;

    return Py_BuildValue("i", _result);
}

static PyObject *_wrap_PDF_makespotcolor(PyObject *self, PyObject *args)
{
    PDF *p;
    char *py_p = 0;
    const char *spotname;
    int PDF_VOLATILE _result = 0;

    if (!PyArg_ParseTuple(args,"ss:PDF_makespotcolor", &py_p, &spotname))
        return NULL;
    if (py_p && SWIG_GetPtr(py_p, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle("PDF_makespotcolor");
        return NULL;
    }

    try { _result = (int) PDF_makespotcolor(p, spotname, 0);
    } catch;

    return Py_BuildValue("i", _result);
}

static PyObject *_wrap_PDF_mc_point(PyObject *self, PyObject *args)
{
    PDF *p;
    char *py_p = 0;
    const char *tag;
    const char *optlist;

    if (!PyArg_ParseTuple(args,"sss:PDF_mc_point", &py_p, &tag, &optlist))
        return NULL;
    if (py_p && SWIG_GetPtr(py_p, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle("PDF_mc_point");
        return NULL;
    }

    try { PDF_mc_point(p, tag, optlist);
    } catch;

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *_wrap_PDF_moveto(PyObject *self, PyObject *args)
{
    PDF *p;
    char *py_p = 0;
    double x;
    double y;

    if (!PyArg_ParseTuple(args,"sdd:PDF_moveto", &py_p, &x, &y))
        return NULL;
    if (py_p && SWIG_GetPtr(py_p, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle("PDF_moveto");
        return NULL;
    }

    try { PDF_moveto(p, x, y);
    } catch;

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *_wrap_PDF_new(PyObject *self, PyObject *args)
{
    PDF *p;
    char _ptemp[128];
    char versionbuf[32];

    if(!PyArg_ParseTuple(args,":PDF_new"))
        return NULL;

    p = PDF_new();

    if (p) {
#if defined(PY_VERSION)
        sprintf(versionbuf, "Python %s", PY_VERSION);
#elif defined(PATCHLEVEL)
        sprintf(versionbuf, "Python %s", PATCHLEVEL);
#else
        sprintf(versionbuf, "Python (unknown)");
#endif
        PDF_set_parameter(p, "binding", versionbuf);
    }
    else {
        PyErr_SetString(PyExc_SystemError, "PDFlib error: in PDF_new()");
        return NULL;
    }

    SWIG_MakePtr(_ptemp, (char *) p,"_PDF_p");
    return Py_BuildValue("s",_ptemp);
}

static PyObject *_wrap_PDF_open_CCITT(PyObject *self, PyObject *args)
{
    PDF *p;
    char *py_p = 0;
    const char *filename;
    int width;
    int height;
    int BitReverse;
    int K;
    int BlackIs1;
    int PDF_VOLATILE _result = 0;

    if (!PyArg_ParseTuple(args,"ssiiiii:PDF_open_CCITT", &py_p, &filename, &width, &height, &BitReverse, &K, &BlackIs1))
        return NULL;
    if (py_p && SWIG_GetPtr(py_p, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle("PDF_open_CCITT");
        return NULL;
    }

    try { _result = (int) PDF_open_CCITT(p, filename, width, height, BitReverse, K, BlackIs1);
    } catch;

    return Py_BuildValue("i", _result);
}

static PyObject *_wrap_PDF_open_file(PyObject *self, PyObject *args)
{
    PDF *p;
    char *py_p = 0;
    const char *filename;
    int PDF_VOLATILE _result = 0;

    if (!PyArg_ParseTuple(args,"ss:PDF_open_file", &py_p, &filename))
        return NULL;
    if (py_p && SWIG_GetPtr(py_p, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle("PDF_open_file");
        return NULL;
    }

    try { _result = (int) PDF_open_file(p, filename);
    } catch;

    return Py_BuildValue("i", _result);
}

static PyObject *_wrap_PDF_open_image(PyObject *self, PyObject *args)
{
    PDF *p;
    char *py_p = 0;
    const char *imagetype;
    const char *source;
    const char *data;
    int dummy;
    long length;
    int width;
    int height;
    int components;
    int bpc;
    const char *params;
    int PDF_VOLATILE _result = 0;

    if (!PyArg_ParseTuple(args,"ssss#liiiis:PDF_open_image", &py_p, &imagetype, &source, &data, &dummy, &length, &width, &height, &components, &bpc, &params))
        return NULL;
    if (py_p && SWIG_GetPtr(py_p, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle("PDF_open_image");
        return NULL;
    }

    try { _result = (int) PDF_open_image(p, imagetype, source, data, length, width, height, components, bpc, params);
    } catch;

    return Py_BuildValue("i", _result);
}

static PyObject *_wrap_PDF_open_image_file(PyObject *self, PyObject *args)
{
    PDF *p;
    char *py_p = 0;
    const char *imagetype;
    const char *filename;
    const char *stringparam;
    int intparam;
    int PDF_VOLATILE _result = 0;

    if (!PyArg_ParseTuple(args,"ssssi:PDF_open_image_file", &py_p, &imagetype, &filename, &stringparam, &intparam))
        return NULL;
    if (py_p && SWIG_GetPtr(py_p, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle("PDF_open_image_file");
        return NULL;
    }

    try { _result = (int) PDF_open_image_file(p, imagetype, filename, stringparam, intparam);
    } catch;

    return Py_BuildValue("i", _result);
}

static PyObject *_wrap_PDF_open_pdi(PyObject *self, PyObject *args)
{
    PDF *p;
    char *py_p = 0;
    const char *filename;
    const char *optlist;
    int reserved;
    int PDF_VOLATILE _result = 0;

    if (!PyArg_ParseTuple(args,"sssi:PDF_open_pdi", &py_p, &filename, &optlist, &reserved))
        return NULL;
    if (py_p && SWIG_GetPtr(py_p, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle("PDF_open_pdi");
        return NULL;
    }

    try { _result = (int) PDF_open_pdi(p, filename, optlist, 0);
    } catch;

    return Py_BuildValue("i", _result);
}

static PyObject *_wrap_PDF_open_pdi_page(PyObject *self, PyObject *args)
{
    PDF *p;
    char *py_p = 0;
    int doc;
    int pagenumber;
    const char *optlist;
    int PDF_VOLATILE _result = 0;

    if (!PyArg_ParseTuple(args,"siis:PDF_open_pdi_page", &py_p, &doc, &pagenumber, &optlist))
        return NULL;
    if (py_p && SWIG_GetPtr(py_p, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle("PDF_open_pdi_page");
        return NULL;
    }

    try { _result = (int) PDF_open_pdi_page(p, doc, pagenumber, optlist);
    } catch;

    return Py_BuildValue("i", _result);
}

static PyObject *_wrap_PDF_place_image(PyObject *self, PyObject *args)
{
    PDF *p;
    char *py_p = 0;
    int image;
    double x;
    double y;
    double scale;

    if (!PyArg_ParseTuple(args,"siddd:PDF_place_image", &py_p, &image, &x, &y, &scale))
        return NULL;
    if (py_p && SWIG_GetPtr(py_p, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle("PDF_place_image");
        return NULL;
    }

    try { PDF_place_image(p, image, x, y, scale);
    } catch;

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *_wrap_PDF_place_pdi_page(PyObject *self, PyObject *args)
{
    PDF *p;
    char *py_p = 0;
    int page;
    double x;
    double y;
    double sx;
    double sy;

    if (!PyArg_ParseTuple(args,"sidddd:PDF_place_pdi_page", &py_p, &page, &x, &y, &sx, &sy))
        return NULL;
    if (py_p && SWIG_GetPtr(py_p, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle("PDF_place_pdi_page");
        return NULL;
    }

    try { PDF_place_pdi_page(p, page, x, y, sx, sy);
    } catch;

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *_wrap_PDF_process_pdi(PyObject *self, PyObject *args)
{
    PDF *p;
    char *py_p = 0;
    int doc;
    int page;
    const char *optlist;
    int PDF_VOLATILE _result = 0;

    if (!PyArg_ParseTuple(args,"siis:PDF_process_pdi", &py_p, &doc, &page, &optlist))
        return NULL;
    if (py_p && SWIG_GetPtr(py_p, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle("PDF_process_pdi");
        return NULL;
    }

    try { _result = (int) PDF_process_pdi(p, doc, page, optlist);
    } catch;

    return Py_BuildValue("i", _result);
}

static PyObject *_wrap_PDF_rect(PyObject *self, PyObject *args)
{
    PDF *p;
    char *py_p = 0;
    double x;
    double y;
    double width;
    double height;

    if (!PyArg_ParseTuple(args,"sdddd:PDF_rect", &py_p, &x, &y, &width, &height))
        return NULL;
    if (py_p && SWIG_GetPtr(py_p, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle("PDF_rect");
        return NULL;
    }

    try { PDF_rect(p, x, y, width, height);
    } catch;

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *_wrap_PDF_restore(PyObject *self, PyObject *args)
{
    PDF *p;
    char *py_p = 0;

    if (!PyArg_ParseTuple(args,"s:PDF_restore", &py_p))
        return NULL;
    if (py_p && SWIG_GetPtr(py_p, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle("PDF_restore");
        return NULL;
    }

    try { PDF_restore(p);
    } catch;

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *_wrap_PDF_resume_page(PyObject *self, PyObject *args)
{
    PDF *p;
    char *py_p = 0;
    const char *optlist;

    if (!PyArg_ParseTuple(args,"ss:PDF_resume_page", &py_p, &optlist))
        return NULL;
    if (py_p && SWIG_GetPtr(py_p, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle("PDF_resume_page");
        return NULL;
    }

    try { PDF_resume_page(p, optlist);
    } catch;

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *_wrap_PDF_rotate(PyObject *self, PyObject *args)
{
    PDF *p;
    char *py_p = 0;
    double phi;

    if (!PyArg_ParseTuple(args,"sd:PDF_rotate", &py_p, &phi))
        return NULL;
    if (py_p && SWIG_GetPtr(py_p, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle("PDF_rotate");
        return NULL;
    }

    try { PDF_rotate(p, phi);
    } catch;

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *_wrap_PDF_save(PyObject *self, PyObject *args)
{
    PDF *p;
    char *py_p = 0;

    if (!PyArg_ParseTuple(args,"s:PDF_save", &py_p))
        return NULL;
    if (py_p && SWIG_GetPtr(py_p, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle("PDF_save");
        return NULL;
    }

    try { PDF_save(p);
    } catch;

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *_wrap_PDF_scale(PyObject *self, PyObject *args)
{
    PDF *p;
    char *py_p = 0;
    double sx;
    double sy;

    if (!PyArg_ParseTuple(args,"sdd:PDF_scale", &py_p, &sx, &sy))
        return NULL;
    if (py_p && SWIG_GetPtr(py_p, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle("PDF_scale");
        return NULL;
    }

    try { PDF_scale(p, sx, sy);
    } catch;

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *_wrap_PDF_set_border_color(PyObject *self, PyObject *args)
{
    PDF *p;
    char *py_p = 0;
    double red;
    double green;
    double blue;

    if (!PyArg_ParseTuple(args,"sddd:PDF_set_border_color", &py_p, &red, &green, &blue))
        return NULL;
    if (py_p && SWIG_GetPtr(py_p, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle("PDF_set_border_color");
        return NULL;
    }

    try { PDF_set_border_color(p, red, green, blue);
    } catch;

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *_wrap_PDF_set_border_dash(PyObject *self, PyObject *args)
{
    PDF *p;
    char *py_p = 0;
    double b;
    double w;

    if (!PyArg_ParseTuple(args,"sdd:PDF_set_border_dash", &py_p, &b, &w))
        return NULL;
    if (py_p && SWIG_GetPtr(py_p, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle("PDF_set_border_dash");
        return NULL;
    }

    try { PDF_set_border_dash(p, b, w);
    } catch;

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *_wrap_PDF_set_border_style(PyObject *self, PyObject *args)
{
    PDF *p;
    char *py_p = 0;
    const char *style;
    double width;

    if (!PyArg_ParseTuple(args,"ssd:PDF_set_border_style", &py_p, &style, &width))
        return NULL;
    if (py_p && SWIG_GetPtr(py_p, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle("PDF_set_border_style");
        return NULL;
    }

    try { PDF_set_border_style(p, style, width);
    } catch;

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *_wrap_PDF_set_gstate(PyObject *self, PyObject *args)
{
    PDF *p;
    char *py_p = 0;
    int gstate;

    if (!PyArg_ParseTuple(args,"si:PDF_set_gstate", &py_p, &gstate))
        return NULL;
    if (py_p && SWIG_GetPtr(py_p, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle("PDF_set_gstate");
        return NULL;
    }

    try { PDF_set_gstate(p, gstate);
    } catch;

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *_wrap_PDF_set_info(PyObject *self, PyObject *args)
{
    PDF *p;
    char *py_p = 0;
    const char *key;
    const char *value;
    int value_len;

    if (!PyArg_ParseTuple(args,"sss#:PDF_set_info", &py_p, &key, &value, &value_len))
        return NULL;
    if (py_p && SWIG_GetPtr(py_p, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle("PDF_set_info");
        return NULL;
    }

    try { PDF_set_info2(p, key, value, value_len);
    } catch;

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *_wrap_PDF_set_layer_dependency(PyObject *self, PyObject *args)
{
    PDF *p;
    char *py_p = 0;
    const char *type;
    const char *optlist;

    if (!PyArg_ParseTuple(args,"sss:PDF_set_layer_dependency", &py_p, &type, &optlist))
        return NULL;
    if (py_p && SWIG_GetPtr(py_p, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle("PDF_set_layer_dependency");
        return NULL;
    }

    try { PDF_set_layer_dependency(p, type, optlist);
    } catch;

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *_wrap_PDF_set_parameter(PyObject *self, PyObject *args)
{
    PDF *p;
    char *py_p = 0;
    const char *key;
    const char *value;

    if (!PyArg_ParseTuple(args,"sss:PDF_set_parameter", &py_p, &key, &value))
        return NULL;
    if (py_p && SWIG_GetPtr(py_p, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle("PDF_set_parameter");
        return NULL;
    }

    try { PDF_set_parameter(p, key, value);
    } catch;

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *_wrap_PDF_set_text_pos(PyObject *self, PyObject *args)
{
    PDF *p;
    char *py_p = 0;
    double x;
    double y;

    if (!PyArg_ParseTuple(args,"sdd:PDF_set_text_pos", &py_p, &x, &y))
        return NULL;
    if (py_p && SWIG_GetPtr(py_p, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle("PDF_set_text_pos");
        return NULL;
    }

    try { PDF_set_text_pos(p, x, y);
    } catch;

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *_wrap_PDF_set_value(PyObject *self, PyObject *args)
{
    PDF *p;
    char *py_p = 0;
    const char *key;
    double value;

    if (!PyArg_ParseTuple(args,"ssd:PDF_set_value", &py_p, &key, &value))
        return NULL;
    if (py_p && SWIG_GetPtr(py_p, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle("PDF_set_value");
        return NULL;
    }

    try { PDF_set_value(p, key, value);
    } catch;

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *_wrap_PDF_setcolor(PyObject *self, PyObject *args)
{
    PDF *p;
    char *py_p = 0;
    const char *fstype;
    const char *colorspace;
    double c1;
    double c2;
    double c3;
    double c4;

    if (!PyArg_ParseTuple(args,"sssdddd:PDF_setcolor", &py_p, &fstype, &colorspace, &c1, &c2, &c3, &c4))
        return NULL;
    if (py_p && SWIG_GetPtr(py_p, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle("PDF_setcolor");
        return NULL;
    }

    try { PDF_setcolor(p, fstype, colorspace, c1, c2, c3, c4);
    } catch;

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *_wrap_PDF_setdash(PyObject *self, PyObject *args)
{
    PDF *p;
    char *py_p = 0;
    double b;
    double w;

    if (!PyArg_ParseTuple(args,"sdd:PDF_setdash", &py_p, &b, &w))
        return NULL;
    if (py_p && SWIG_GetPtr(py_p, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle("PDF_setdash");
        return NULL;
    }

    try { PDF_setdash(p, b, w);
    } catch;

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *_wrap_PDF_setdashpattern(PyObject *self, PyObject *args)
{
    PDF *p;
    char *py_p = 0;
    const char *optlist;

    if (!PyArg_ParseTuple(args,"ss:PDF_setdashpattern", &py_p, &optlist))
        return NULL;
    if (py_p && SWIG_GetPtr(py_p, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle("PDF_setdashpattern");
        return NULL;
    }

    try { PDF_setdashpattern(p, optlist);
    } catch;

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *_wrap_PDF_setflat(PyObject *self, PyObject *args)
{
    PDF *p;
    char *py_p = 0;
    double flatness;

    if (!PyArg_ParseTuple(args,"sd:PDF_setflat", &py_p, &flatness))
        return NULL;
    if (py_p && SWIG_GetPtr(py_p, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle("PDF_setflat");
        return NULL;
    }

    try { PDF_setflat(p, flatness);
    } catch;

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *_wrap_PDF_setfont(PyObject *self, PyObject *args)
{
    PDF *p;
    char *py_p = 0;
    int font;
    double fontsize;

    if (!PyArg_ParseTuple(args,"sid:PDF_setfont", &py_p, &font, &fontsize))
        return NULL;
    if (py_p && SWIG_GetPtr(py_p, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle("PDF_setfont");
        return NULL;
    }

    try { PDF_setfont(p, font, fontsize);
    } catch;

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *_wrap_PDF_setgray(PyObject *self, PyObject *args)
{
    PDF *p;
    char *py_p = 0;
    double gray;

    if (!PyArg_ParseTuple(args,"sd:PDF_setgray", &py_p, &gray))
        return NULL;
    if (py_p && SWIG_GetPtr(py_p, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle("PDF_setgray");
        return NULL;
    }

    try { PDF_setgray(p, gray);
    } catch;

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *_wrap_PDF_setgray_stroke(PyObject *self, PyObject *args)
{
    PDF *p;
    char *py_p = 0;
    double gray;

    if (!PyArg_ParseTuple(args,"sd:PDF_setgray_stroke", &py_p, &gray))
        return NULL;
    if (py_p && SWIG_GetPtr(py_p, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle("PDF_setgray_stroke");
        return NULL;
    }

    try { PDF_setgray_stroke(p, gray);
    } catch;

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *_wrap_PDF_setgray_fill(PyObject *self, PyObject *args)
{
    PDF *p;
    char *py_p = 0;
    double gray;

    if (!PyArg_ParseTuple(args,"sd:PDF_setgray_fill", &py_p, &gray))
        return NULL;
    if (py_p && SWIG_GetPtr(py_p, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle("PDF_setgray_fill");
        return NULL;
    }

    try { PDF_setgray_fill(p, gray);
    } catch;

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *_wrap_PDF_setlinecap(PyObject *self, PyObject *args)
{
    PDF *p;
    char *py_p = 0;
    int linecap;

    if (!PyArg_ParseTuple(args,"si:PDF_setlinecap", &py_p, &linecap))
        return NULL;
    if (py_p && SWIG_GetPtr(py_p, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle("PDF_setlinecap");
        return NULL;
    }

    try { PDF_setlinecap(p, linecap);
    } catch;

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *_wrap_PDF_setlinejoin(PyObject *self, PyObject *args)
{
    PDF *p;
    char *py_p = 0;
    int linejoin;

    if (!PyArg_ParseTuple(args,"si:PDF_setlinejoin", &py_p, &linejoin))
        return NULL;
    if (py_p && SWIG_GetPtr(py_p, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle("PDF_setlinejoin");
        return NULL;
    }

    try { PDF_setlinejoin(p, linejoin);
    } catch;

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *_wrap_PDF_setlinewidth(PyObject *self, PyObject *args)
{
    PDF *p;
    char *py_p = 0;
    double width;

    if (!PyArg_ParseTuple(args,"sd:PDF_setlinewidth", &py_p, &width))
        return NULL;
    if (py_p && SWIG_GetPtr(py_p, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle("PDF_setlinewidth");
        return NULL;
    }

    try { PDF_setlinewidth(p, width);
    } catch;

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *_wrap_PDF_setmatrix(PyObject *self, PyObject *args)
{
    PDF *p;
    char *py_p = 0;
    double a;
    double b;
    double c;
    double d;
    double e;
    double f;

    if (!PyArg_ParseTuple(args,"sdddddd:PDF_setmatrix", &py_p, &a, &b, &c, &d, &e, &f))
        return NULL;
    if (py_p && SWIG_GetPtr(py_p, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle("PDF_setmatrix");
        return NULL;
    }

    try { PDF_setmatrix(p, a, b, c, d, e, f);
    } catch;

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *_wrap_PDF_setmiterlimit(PyObject *self, PyObject *args)
{
    PDF *p;
    char *py_p = 0;
    double miter;

    if (!PyArg_ParseTuple(args,"sd:PDF_setmiterlimit", &py_p, &miter))
        return NULL;
    if (py_p && SWIG_GetPtr(py_p, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle("PDF_setmiterlimit");
        return NULL;
    }

    try { PDF_setmiterlimit(p, miter);
    } catch;

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *_wrap_PDF_setpolydash(PyObject *self, PyObject *args)
{
    PDF *p;
    char *py_p = 0;
    PyObject *parray = NULL;

    PyObject *val;
    float fval, *carray;
    int PDF_VOLATILE length, i;

    if (!PyArg_ParseTuple(args,"sO:PDF_setpolydash", &py_p, &parray))
        return NULL;
    if (py_p && SWIG_GetPtr(py_p, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle("PDF_setpolydash");
        return NULL;
    }

    length = PyTuple_Size(parray);

    carray = (float *) malloc(sizeof(float) * length);
    if (carray == NULL)
    {
        PyErr_SetString(PyExc_TypeError, "Out of memory in PDF_set_polydash");
        return NULL;
    }

    for (i = 0; i < length; i++)
    {
        val = PyTuple_GetItem(parray, i);
        if (!PyArg_Parse(val, "f:PDF_setpolydash", &fval))
        {
            PyErr_SetString(PyExc_TypeError,
                "Type error in argument 2 of PDF_setpolydash. Expected float.");
            return NULL;
        }
        carray[i] = fval;
    }

    try { PDF_setpolydash(p, carray, length);
    } catch;

    free(carray);

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *_wrap_PDF_setrgbcolor(PyObject *self, PyObject *args)
{
    PDF *p;
    char *py_p = 0;
    double red;
    double green;
    double blue;

    if (!PyArg_ParseTuple(args,"sddd:PDF_setrgbcolor", &py_p, &red, &green, &blue))
        return NULL;
    if (py_p && SWIG_GetPtr(py_p, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle("PDF_setrgbcolor");
        return NULL;
    }

    try { PDF_setrgbcolor(p, red, green, blue);
    } catch;

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *_wrap_PDF_setrgbcolor_fill(PyObject *self, PyObject *args)
{
    PDF *p;
    char *py_p = 0;
    double red;
    double green;
    double blue;

    if (!PyArg_ParseTuple(args,"sddd:PDF_setrgbcolor_fill", &py_p, &red, &green, &blue))
        return NULL;
    if (py_p && SWIG_GetPtr(py_p, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle("PDF_setrgbcolor_fill");
        return NULL;
    }

    try { PDF_setrgbcolor_fill(p, red, green, blue);
    } catch;

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *_wrap_PDF_setrgbcolor_stroke(PyObject *self, PyObject *args)
{
    PDF *p;
    char *py_p = 0;
    double red;
    double green;
    double blue;

    if (!PyArg_ParseTuple(args,"sddd:PDF_setrgbcolor_stroke", &py_p, &red, &green, &blue))
        return NULL;
    if (py_p && SWIG_GetPtr(py_p, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle("PDF_setrgbcolor_stroke");
        return NULL;
    }

    try { PDF_setrgbcolor_stroke(p, red, green, blue);
    } catch;

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *_wrap_PDF_shading(PyObject *self, PyObject *args)
{
    PDF *p;
    char *py_p = 0;
    const char *shtype;
    double x_0;
    double y_0;
    double x_1;
    double y_1;
    double c_1;
    double c_2;
    double c_3;
    double c_4;
    const char *optlist;
    int PDF_VOLATILE _result = 0;

    if (!PyArg_ParseTuple(args,"ssdddddddds:PDF_shading", &py_p, &shtype, &x_0, &y_0, &x_1, &y_1, &c_1, &c_2, &c_3, &c_4, &optlist))
        return NULL;
    if (py_p && SWIG_GetPtr(py_p, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle("PDF_shading");
        return NULL;
    }

    try { _result = (int) PDF_shading(p, shtype, x_0, y_0, x_1, y_1, c_1, c_2, c_3, c_4, optlist);
    } catch;

    return Py_BuildValue("i", _result);
}

static PyObject *_wrap_PDF_shading_pattern(PyObject *self, PyObject *args)
{
    PDF *p;
    char *py_p = 0;
    int shading;
    const char *optlist;
    int PDF_VOLATILE _result = 0;

    if (!PyArg_ParseTuple(args,"sis:PDF_shading_pattern", &py_p, &shading, &optlist))
        return NULL;
    if (py_p && SWIG_GetPtr(py_p, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle("PDF_shading_pattern");
        return NULL;
    }

    try { _result = (int) PDF_shading_pattern(p, shading, optlist);
    } catch;

    return Py_BuildValue("i", _result);
}

static PyObject *_wrap_PDF_shfill(PyObject *self, PyObject *args)
{
    PDF *p;
    char *py_p = 0;
    int shading;

    if (!PyArg_ParseTuple(args,"si:PDF_shfill", &py_p, &shading))
        return NULL;
    if (py_p && SWIG_GetPtr(py_p, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle("PDF_shfill");
        return NULL;
    }

    try { PDF_shfill(p, shading);
    } catch;

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *_wrap_PDF_show(PyObject *self, PyObject *args)
{
    PDF *p;
    char *py_p = 0;
    const char *text;
    int text_len;

    if (!PyArg_ParseTuple(args,"ss#:PDF_show", &py_p, &text, &text_len))
        return NULL;
    if (py_p && SWIG_GetPtr(py_p, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle("PDF_show");
        return NULL;
    }

    try { PDF_show2(p, text, text_len);
    } catch;

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *_wrap_PDF_show_boxed(PyObject *self, PyObject *args)
{
    PDF *p;
    char *py_p = 0;
    const char *text;
    int text_len;
    double left;
    double top;
    double width;
    double height;
    const char *hmode;
    const char *feature;
    int PDF_VOLATILE _result = 0;

    if (!PyArg_ParseTuple(args,"ss#ddddss:PDF_show_boxed", &py_p, &text, &text_len, &left, &top, &width, &height, &hmode, &feature))
        return NULL;
    if (py_p && SWIG_GetPtr(py_p, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle("PDF_show_boxed");
        return NULL;
    }

    try { _result = (int) PDF_show_boxed2(p, text, text_len, left, top, width, height, hmode, feature);
    } catch;

    return Py_BuildValue("i", _result);
}

static PyObject *_wrap_PDF_show_xy(PyObject *self, PyObject *args)
{
    PDF *p;
    char *py_p = 0;
    const char *text;
    int text_len;
    double x;
    double y;

    if (!PyArg_ParseTuple(args,"ss#dd:PDF_show_xy", &py_p, &text, &text_len, &x, &y))
        return NULL;
    if (py_p && SWIG_GetPtr(py_p, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle("PDF_show_xy");
        return NULL;
    }

    try { PDF_show_xy2(p, text, text_len, x, y);
    } catch;

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *_wrap_PDF_skew(PyObject *self, PyObject *args)
{
    PDF *p;
    char *py_p = 0;
    double alpha;
    double beta;

    if (!PyArg_ParseTuple(args,"sdd:PDF_skew", &py_p, &alpha, &beta))
        return NULL;
    if (py_p && SWIG_GetPtr(py_p, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle("PDF_skew");
        return NULL;
    }

    try { PDF_skew(p, alpha, beta);
    } catch;

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *_wrap_PDF_stringwidth(PyObject *self, PyObject *args)
{
    PDF *p;
    char *py_p = 0;
    const char *text;
    int text_len;
    int font;
    double fontsize;
    double PDF_VOLATILE _result = 0;

    if (!PyArg_ParseTuple(args,"ss#id:PDF_stringwidth", &py_p, &text, &text_len, &font, &fontsize))
        return NULL;
    if (py_p && SWIG_GetPtr(py_p, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle("PDF_stringwidth");
        return NULL;
    }

    try { _result = (double) PDF_stringwidth2(p, text, text_len, font, fontsize);
    } catch;

    return Py_BuildValue("d", _result);
}

static PyObject *_wrap_PDF_stroke(PyObject *self, PyObject *args)
{
    PDF *p;
    char *py_p = 0;

    if (!PyArg_ParseTuple(args,"s:PDF_stroke", &py_p))
        return NULL;
    if (py_p && SWIG_GetPtr(py_p, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle("PDF_stroke");
        return NULL;
    }

    try { PDF_stroke(p);
    } catch;

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *_wrap_PDF_suspend_page(PyObject *self, PyObject *args)
{
    PDF *p;
    char *py_p = 0;
    const char *optlist;

    if (!PyArg_ParseTuple(args,"ss:PDF_suspend_page", &py_p, &optlist))
        return NULL;
    if (py_p && SWIG_GetPtr(py_p, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle("PDF_suspend_page");
        return NULL;
    }

    try { PDF_suspend_page(p, optlist);
    } catch;

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *_wrap_PDF_translate(PyObject *self, PyObject *args)
{
    PDF *p;
    char *py_p = 0;
    double tx;
    double ty;

    if (!PyArg_ParseTuple(args,"sdd:PDF_translate", &py_p, &tx, &ty))
        return NULL;
    if (py_p && SWIG_GetPtr(py_p, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle("PDF_translate");
        return NULL;
    }

    try { PDF_translate(p, tx, ty);
    } catch;

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *_wrap_PDF_utf16_to_utf8(PyObject *self, PyObject *args)
{
    PDF *p;
    char *py_p = 0;
    const char *utf16string;
    int utf16string_len;
    int size;
    char PDF_VOLATILE *_result = NULL;

    if (!PyArg_ParseTuple(args,"ss#i:PDF_utf16_to_utf8", &py_p, &utf16string, &utf16string_len, &size))
        return NULL;
    if (py_p && SWIG_GetPtr(py_p, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle("PDF_utf16_to_utf8");
        return NULL;
    }

    try { _result = (char *) PDF_utf16_to_utf8(p, utf16string, utf16string_len, &size);
    } catch;

    return Py_BuildValue("s#", _result, size);
}

static PyObject *_wrap_PDF_utf8_to_utf16(PyObject *self, PyObject *args)
{
    PDF *p;
    char *py_p = 0;
    const char *utf8string;
    const char *format;
    int size;
    char PDF_VOLATILE *_result = NULL;

    if (!PyArg_ParseTuple(args,"sss:PDF_utf8_to_utf16", &py_p, &utf8string, &format))
        return NULL;
    if (py_p && SWIG_GetPtr(py_p, (void **) &p, "_PDF_p"))
    {
        PDF_WrongPDFHandle("PDF_utf8_to_utf16");
        return NULL;
    }

    try { _result = (char *) PDF_utf8_to_utf16(p, utf8string, format, &size);
    } catch;

    return Py_BuildValue("s#", _result, size);
}

static PyMethodDef pdflibMethods[] =
{
    { "PDF_activate_item", _wrap_PDF_activate_item, 1 },
    { "PDF_add_bookmark", _wrap_PDF_add_bookmark, 1 },
    { "PDF_add_launchlink", _wrap_PDF_add_launchlink, 1 },
    { "PDF_add_locallink", _wrap_PDF_add_locallink, 1 },
    { "PDF_add_nameddest", _wrap_PDF_add_nameddest, 1 },
    { "PDF_add_note", _wrap_PDF_add_note, 1 },
    { "PDF_add_pdflink", _wrap_PDF_add_pdflink, 1 },
    { "PDF_add_thumbnail", _wrap_PDF_add_thumbnail, 1 },
    { "PDF_add_weblink", _wrap_PDF_add_weblink, 1 },
    { "PDF_arc", _wrap_PDF_arc, 1 },
    { "PDF_arcn", _wrap_PDF_arcn, 1 },
    { "PDF_attach_file", _wrap_PDF_attach_file, 1 },
    { "PDF_begin_document", _wrap_PDF_begin_document, 1 },
    { "PDF_begin_font", _wrap_PDF_begin_font, 1 },
    { "PDF_begin_glyph", _wrap_PDF_begin_glyph, 1 },
    { "PDF_begin_item", _wrap_PDF_begin_item, 1 },
    { "PDF_begin_layer", _wrap_PDF_begin_layer, 1 },
    { "PDF_begin_mc", _wrap_PDF_begin_mc, 1 },
    { "PDF_begin_page", _wrap_PDF_begin_page, 1 },
    { "PDF_begin_page_ext", _wrap_PDF_begin_page_ext, 1 },
    { "PDF_begin_pattern", _wrap_PDF_begin_pattern, 1 },
    { "PDF_begin_template", _wrap_PDF_begin_template, 1 },
    { "PDF_circle", _wrap_PDF_circle, 1 },
    { "PDF_clip", _wrap_PDF_clip, 1 },
    { "PDF_close", _wrap_PDF_close, 1 },
    { "PDF_close_image", _wrap_PDF_close_image, 1 },
    { "PDF_close_pdi", _wrap_PDF_close_pdi, 1 },
    { "PDF_close_pdi_page", _wrap_PDF_close_pdi_page, 1 },
    { "PDF_closepath", _wrap_PDF_closepath, 1 },
    { "PDF_closepath_fill_stroke", _wrap_PDF_closepath_fill_stroke, 1 },
    { "PDF_closepath_stroke", _wrap_PDF_closepath_stroke, 1 },
    { "PDF_concat", _wrap_PDF_concat, 1 },
    { "PDF_continue_text", _wrap_PDF_continue_text, 1 },
    { "PDF_create_action", _wrap_PDF_create_action, 1 },
    { "PDF_create_annotation", _wrap_PDF_create_annotation, 1 },
    { "PDF_create_bookmark", _wrap_PDF_create_bookmark, 1 },
    { "PDF_create_field", _wrap_PDF_create_field, 1 },
    { "PDF_create_fieldgroup", _wrap_PDF_create_fieldgroup, 1 },
    { "PDF_create_gstate", _wrap_PDF_create_gstate, 1 },
    { "PDF_create_pvf", _wrap_PDF_create_pvf, 1 },
    { "PDF_create_textflow", _wrap_PDF_create_textflow, 1 },
    { "PDF_curveto", _wrap_PDF_curveto, 1 },
    { "PDF_define_layer", _wrap_PDF_define_layer, 1 },
    { "PDF_delete", _wrap_PDF_delete, 1 },
    { "PDF_delete_pvf", _wrap_PDF_delete_pvf, 1 },
    { "PDF_delete_textflow", _wrap_PDF_delete_textflow, 1 },
    { "PDF_encoding_set_char", _wrap_PDF_encoding_set_char, 1 },
    { "PDF_end_document", _wrap_PDF_end_document, 1 },
    { "PDF_end_font", _wrap_PDF_end_font, 1 },
    { "PDF_end_glyph", _wrap_PDF_end_glyph, 1 },
    { "PDF_end_item", _wrap_PDF_end_item, 1 },
    { "PDF_end_layer", _wrap_PDF_end_layer, 1 },
    { "PDF_end_mc", _wrap_PDF_end_mc, 1 },
    { "PDF_end_page", _wrap_PDF_end_page, 1 },
    { "PDF_end_page_ext", _wrap_PDF_end_page_ext, 1 },
    { "PDF_end_pattern", _wrap_PDF_end_pattern, 1 },
    { "PDF_end_template", _wrap_PDF_end_template, 1 },
    { "PDF_endpath", _wrap_PDF_endpath, 1 },
    { "PDF_fill", _wrap_PDF_fill, 1 },
    { "PDF_fill_imageblock", _wrap_PDF_fill_imageblock, 1 },
    { "PDF_fill_pdfblock", _wrap_PDF_fill_pdfblock, 1 },
    { "PDF_fill_stroke", _wrap_PDF_fill_stroke, 1 },
    { "PDF_fill_textblock", _wrap_PDF_fill_textblock, 1 },
    { "PDF_findfont", _wrap_PDF_findfont, 1 },
    { "PDF_fit_image", _wrap_PDF_fit_image, 1 },
    { "PDF_fit_pdi_page", _wrap_PDF_fit_pdi_page, 1 },
    { "PDF_fit_textflow", _wrap_PDF_fit_textflow, 1 },
    { "PDF_fit_textline", _wrap_PDF_fit_textline, 1 },
    { "PDF_get_apiname", _wrap_PDF_get_apiname, 1 },
    { "PDF_get_buffer", _wrap_PDF_get_buffer, 1 },
    { "PDF_get_errmsg", _wrap_PDF_get_errmsg, 1 },
    { "PDF_get_errnum", _wrap_PDF_get_errnum, 1 },
    { "PDF_get_parameter", _wrap_PDF_get_parameter, 1 },
    { "PDF_get_pdi_parameter", _wrap_PDF_get_pdi_parameter, 1 },
    { "PDF_get_pdi_value", _wrap_PDF_get_pdi_value, 1 },
    { "PDF_get_value", _wrap_PDF_get_value, 1 },
    { "PDF_info_textflow", _wrap_PDF_info_textflow, 1 },
    { "PDF_initgraphics", _wrap_PDF_initgraphics, 1 },
    { "PDF_lineto", _wrap_PDF_lineto, 1 },
    { "PDF_load_font", _wrap_PDF_load_font, 1 },
    { "PDF_load_iccprofile", _wrap_PDF_load_iccprofile, 1 },
    { "PDF_load_image", _wrap_PDF_load_image, 1 },
    { "PDF_makespotcolor", _wrap_PDF_makespotcolor, 1 },
    { "PDF_mc_point", _wrap_PDF_mc_point, 1 },
    { "PDF_moveto", _wrap_PDF_moveto, 1 },
    { "PDF_new", _wrap_PDF_new, 1 },
    { "PDF_open_CCITT", _wrap_PDF_open_CCITT, 1 },
    { "PDF_open_file", _wrap_PDF_open_file, 1 },
    { "PDF_open_image", _wrap_PDF_open_image, 1 },
    { "PDF_open_image_file", _wrap_PDF_open_image_file, 1 },
    { "PDF_open_pdi", _wrap_PDF_open_pdi, 1 },
    { "PDF_open_pdi_page", _wrap_PDF_open_pdi_page, 1 },
    { "PDF_place_image", _wrap_PDF_place_image, 1 },
    { "PDF_place_pdi_page", _wrap_PDF_place_pdi_page, 1 },
    { "PDF_process_pdi", _wrap_PDF_process_pdi, 1 },
    { "PDF_rect", _wrap_PDF_rect, 1 },
    { "PDF_restore", _wrap_PDF_restore, 1 },
    { "PDF_resume_page", _wrap_PDF_resume_page, 1 },
    { "PDF_rotate", _wrap_PDF_rotate, 1 },
    { "PDF_save", _wrap_PDF_save, 1 },
    { "PDF_scale", _wrap_PDF_scale, 1 },
    { "PDF_set_border_color", _wrap_PDF_set_border_color, 1 },
    { "PDF_set_border_dash", _wrap_PDF_set_border_dash, 1 },
    { "PDF_set_border_style", _wrap_PDF_set_border_style, 1 },
    { "PDF_set_gstate", _wrap_PDF_set_gstate, 1 },
    { "PDF_set_info", _wrap_PDF_set_info, 1 },
    { "PDF_set_layer_dependency", _wrap_PDF_set_layer_dependency, 1 },
    { "PDF_set_parameter", _wrap_PDF_set_parameter, 1 },
    { "PDF_set_text_pos", _wrap_PDF_set_text_pos, 1 },
    { "PDF_set_value", _wrap_PDF_set_value, 1 },
    { "PDF_setcolor", _wrap_PDF_setcolor, 1 },
    { "PDF_setdash", _wrap_PDF_setdash, 1 },
    { "PDF_setdashpattern", _wrap_PDF_setdashpattern, 1 },
    { "PDF_setflat", _wrap_PDF_setflat, 1 },
    { "PDF_setfont", _wrap_PDF_setfont, 1 },
    { "PDF_setgray", _wrap_PDF_setgray, 1 },
    { "PDF_setgray_stroke", _wrap_PDF_setgray_stroke, 1 },
    { "PDF_setgray_fill", _wrap_PDF_setgray_fill, 1 },
    { "PDF_setlinecap", _wrap_PDF_setlinecap, 1 },
    { "PDF_setlinejoin", _wrap_PDF_setlinejoin, 1 },
    { "PDF_setlinewidth", _wrap_PDF_setlinewidth, 1 },
    { "PDF_setmatrix", _wrap_PDF_setmatrix, 1 },
    { "PDF_setmiterlimit", _wrap_PDF_setmiterlimit, 1 },
    { "PDF_setpolydash", _wrap_PDF_setpolydash, 1 },
    { "PDF_setrgbcolor", _wrap_PDF_setrgbcolor, 1 },
    { "PDF_setrgbcolor_fill", _wrap_PDF_setrgbcolor_fill, 1 },
    { "PDF_setrgbcolor_stroke", _wrap_PDF_setrgbcolor_stroke, 1 },
    { "PDF_shading", _wrap_PDF_shading, 1 },
    { "PDF_shading_pattern", _wrap_PDF_shading_pattern, 1 },
    { "PDF_shfill", _wrap_PDF_shfill, 1 },
    { "PDF_show", _wrap_PDF_show, 1 },
    { "PDF_show_boxed", _wrap_PDF_show_boxed, 1 },
    { "PDF_show_xy", _wrap_PDF_show_xy, 1 },
    { "PDF_skew", _wrap_PDF_skew, 1 },
    { "PDF_stringwidth", _wrap_PDF_stringwidth, 1 },
    { "PDF_stroke", _wrap_PDF_stroke, 1 },
    { "PDF_suspend_page", _wrap_PDF_suspend_page, 1 },
    { "PDF_translate", _wrap_PDF_translate, 1 },
    { "PDF_utf16_to_utf8", _wrap_PDF_utf16_to_utf8, 1 },
    { "PDF_utf8_to_utf16", _wrap_PDF_utf8_to_utf16, 1 },
    { NULL, NULL },
};


static PyObject *SWIG_globals;
#ifdef __cplusplus
extern "C"
#endif
SWIGEXPORT(void,initpdflib_py)() {
	 PyObject *m, *d;
	 SWIG_globals = SWIG_newvarlink();
	 m = Py_InitModule("pdflib_py", pdflibMethods);
	 d = PyModule_GetDict(m);

	/* Boot the PDFlib core */
	PDF_boot();
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
}
