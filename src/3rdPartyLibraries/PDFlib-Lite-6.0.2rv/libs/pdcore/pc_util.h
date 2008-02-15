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

/* $Id: pc_util.h,v 1.1 2005/10/11 17:18:13 vuduc2 Exp $
 *
 * Various utility routines
 *
 */

#ifndef PC_UTIL_H
#define PC_UTIL_H

#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <math.h>
#include <ctype.h>
#include <stdio.h>
#include <stdarg.h>

#include "pc_config.h"
#include "pc_core.h"
#include "pc_ebcdic.h"
#include "pc_encoding.h"
#include "pc_output.h"
#include "pc_optparse.h"
#include "pc_unicode.h"

#define PDC_BOOLSTR(a) (a != 0 ? "true" : "false")

#ifndef MIN
#define MIN(a,b) (a <= b ? a : b)
#endif
#ifndef MAX
#define MAX(a,b) (a >= b ? a : b)
#endif

#define PDC_ROUND(x)        (((x) < 0) ? ceil((x) - 0.5) : floor((x) + 0.5))

#define PDC_FLOAT_ISNULL(x) \
    (((((x) < 0) ? -1 * (x) : (x)) < PDC_FLOAT_PREC) ? pdc_true : pdc_false)

#define PDC_INT_UNSIGNED  (1L<<0)
#define PDC_INT_CHAR      (1L<<1)
#define PDC_INT_SHORT     (1L<<2)
#define PDC_INT_HEXADEC   (1L<<4)
#define PDC_INT_DEC       (1L<<5)
#define PDC_INT_CASESENS  (1L<<6)

#define PDC_INT_CODE  (PDC_INT_UNSIGNED | PDC_INT_CHAR | PDC_INT_HEXADEC)
#define PDC_INT_UNICODE  (PDC_INT_UNSIGNED | PDC_INT_SHORT | PDC_INT_HEXADEC)

#define PDC_GET_SHORT  pdc_get_le_short
#define PDC_GET_USHORT pdc_get_le_ushort
#define PDC_GET_WORD   pdc_get_le_ushort
#define PDC_GET_DWORD  pdc_get_le_ulong
#define PDC_GET_DWORD3 pdc_get_le_ulong3
#define PDC_GET_LONG   pdc_get_le_long
#define PDC_GET_ULONG  pdc_get_le_ulong

#define PDC_TREE_INHERIT (1L<<0)
#define PDC_TREE_ISLEAF  (1L<<1)

#define PDC_NAME_SEPARSIGN '.'
#define PDC_NAME_SEPARSTRG "."

#define PDC_KIDS_CHUNKSIZE 5

/* tree error codes */
typedef enum
{
    tree_ok = 0,
    tree_notfound,
    tree_nameexists,
    tree_isleaf
}
pdc_branch_error;

typedef struct pdc_branch_s pdc_branch;

#define PDC_TIME_SBUF_SIZE	50

typedef struct
{
    int	second;
    int	minute;
    int	hour;
    int	mday;
    int	wday;
    int	month;
    int	year;
} pdc_time;

void	pdc_localtime(pdc_time *t);
void	pdc_get_timestr(char *str);

pdc_bool pdc_check_lang_code(pdc_core *pdc, const char* lang_code);

void     pdc_setbit(char *bitarr, int bit);
pdc_bool pdc_getbit(char *bitarr, int bit);
void     pdc_setbit_text(char *bitarr, const unsigned char *text,
                         int len, int nbits, int size);

pdc_short  pdc_get_le_short(pdc_byte *data);
pdc_ushort pdc_get_le_ushort(pdc_byte *data);
pdc_sint32 pdc_get_le_long(pdc_byte *data);
pdc_uint32 pdc_get_le_ulong3(pdc_byte *data);
pdc_uint32 pdc_get_le_ulong(pdc_byte *data);
pdc_short  pdc_get_be_short(pdc_byte *data);
pdc_ushort pdc_get_be_ushort(pdc_byte *data);
pdc_sint32 pdc_get_be_long(pdc_byte *data);
pdc_uint32 pdc_get_be_ulong3(pdc_byte *data);
pdc_uint32 pdc_get_be_ulong(pdc_byte *data);

size_t  pdc_strlen(const char *text);
char	*pdc_getenv(const char *name);
char    *pdc_strdup(pdc_core *pdc, const char *text);
char    *pdc_strdup_tmp(pdc_core *pdc, const char *text);
char    *pdc_strprint(pdc_core *pdc, const char *str, int leni,
                int maxchar, pdc_strform_kind strform, pdc_bool trybytes);
const char *pdc_utf8strprint(pdc_core *pdc, const char *str);
int	pdc_split_stringlist(pdc_core *pdc, const char *text,
	                     const char *i_separstr, char ***stringlist);
void    pdc_cleanup_stringlist(pdc_core *pdc, char **stringlist);
int	pdc_stricmp(const char *s1, const char *s2);
int	pdc_strincmp(const char *s1, const char *s2, int n);
char    *pdc_strtrim(char *m_str);
char    *pdc_str2trim(char *m_str);
void    pdc_swap_bytes(char *instring, int inlen, char *outstring);
void    pdc_swap_unicodes(char *instring);
char   *pdc_strdup_withbom(pdc_core *pdc, const char *text);
void    pdc_inflate_ascii(const char *instring, int inlen, char *outstring,
                          pdc_text_format textformat);
pdc_bool pdc_str2double(const char *string, double *o_dz);
pdc_bool pdc_str2integer(const char *string, int flags, void *o_iz);
int     pdc_vfprintf(pdc_core *pdc, FILE *fp, const char *fmt, va_list args);
int     pdc_fprintf(pdc_core *pdc, FILE *fp, const char *fmt, ...);
int     pdc_vsprintf(pdc_core *pdc, char *buf, const char *fmt, va_list args);
int     pdc_sprintf(pdc_core *pdc, char *buf, const char *fmt, ...);

pdc_branch *pdc_init_tree(pdc_core *pdc);
pdc_branch *pdc_create_treebranch(pdc_core *pdc, pdc_branch *root,
               const char *pathname, void *data, int flags, int size,
               pdc_branch_error *errcode, const char **name_p);
char *pdc_get_name_treebranch(pdc_branch *branch);
pdc_branch *pdc_get_parent_treebranch(pdc_branch *branch);
pdc_branch **pdc_get_kids_treebranch(pdc_branch *branch, int *nkids);
void *pdc_get_data_treebranch(pdc_branch *branch);
void pdc_cleanup_treebranch(pdc_core *pdc, pdc_branch *branch);
void pdc_deactivate_name_treebranch(pdc_core *pdc, pdc_branch *branch);

#endif	/* PC_UTIL_H */
