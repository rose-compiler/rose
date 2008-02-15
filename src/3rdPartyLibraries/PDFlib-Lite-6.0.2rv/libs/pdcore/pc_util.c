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

/* $Id: pc_util.c,v 1.1 2005/10/11 17:18:13 vuduc2 Exp $
 *
 * PDFlib various utility routines
 *
 */

#include <errno.h>

#include "pc_util.h"

#ifdef AS400
#include <qp0z1170.h>   /* for getenv() emulation */
#endif

/* -------------------------- Time functions ------------------------------ */

#ifndef WINCE
#ifndef __USE_POSIX
#define __USE_POSIX
#endif
#include <time.h>
#else
#include <winbase.h>
#endif


/* our private localtime() function. this one circumvents platform
** quirks we found on WINCE and Solaris, and perhaps some more in
** the future.
*/
void
pdc_localtime(pdc_time *t)
{
#ifdef WINCE

    SYSTEMTIME  st;

    GetLocalTime (&st);

    t->second = st.wSecond;
    t->minute = st.wMinute;
    t->hour = st.wHour;
    t->mday = st.wDay;
    t->wday = st.wDayOfWeek;
    t->month = st.wMonth;
    t->year = st.wYear;

#else

    time_t      timer;
    struct tm	ltime;

    time(&timer);

#if defined(PDC_NEEDS_R_FUNCTIONS)

    /* the localtime() function isn't thread safe on this platform.
    ** a thread safe variant must be used instead.
    */
    (void) localtime_r(&timer, &ltime);

#else

    ltime = *localtime(&timer);

#endif /* !PDC_NEEDS_R_FUNCTIONS */

    t->second = ltime.tm_sec;
    t->minute = ltime.tm_min;
    t->hour = ltime.tm_hour;
    t->mday = ltime.tm_mday;
    t->wday = ltime.tm_wday;
    t->month = ltime.tm_mon;
    t->year = ltime.tm_year;

#endif /* !WINCE */
}

static void
pdc_localtime_r(const time_t *timer, struct tm *res)
{
#if defined(PDC_NEEDS_R_FUNCTIONS)
    (void) localtime_r(timer, res);
#else
    *res = *localtime(timer);
#endif
}

static void
pdc_gmtime_r(const time_t *timer, struct tm *res)
{
#if defined(PDC_NEEDS_R_FUNCTIONS)
    (void) gmtime_r(timer, res);
#else
    *res = *gmtime(timer);
#endif
}

void
pdc_get_timestr(char *str)
{
#ifndef WINCE
    time_t      timer, gtimer;
    struct tm   ltime;
    double      diffminutes;
    int         utcoffset;
#else
    SYSTEMTIME  st;
#endif

#ifndef WINCE
    time(&timer);

#if !defined(I370)
    pdc_gmtime_r(&timer, &ltime);
    gtimer = mktime(&ltime);
    pdc_localtime_r(&timer, &ltime);
    ltime.tm_isdst = 0;
    diffminutes = difftime(mktime(&ltime), gtimer) / 60;
    if (diffminutes >= 0)
        utcoffset = (int)(diffminutes + 0.5);
    else
        utcoffset = (int)(diffminutes - 0.5);
#else
        utcoffset = 0;
#endif

    /* Get local time again, previous data is damaged by mktime(). */
    pdc_localtime_r(&timer, &ltime);

    if (utcoffset > 0)
        sprintf(str, "D:%04d%02d%02d%02d%02d%02d+%02d'%02d'",
            ltime.tm_year + 1900, ltime.tm_mon + 1, ltime.tm_mday,
            ltime.tm_hour, ltime.tm_min, ltime.tm_sec,
            utcoffset / 60, utcoffset % 60);
    else if (utcoffset < 0)
        sprintf(str, "D:%04d%02d%02d%02d%02d%02d-%02d'%02d'",
            ltime.tm_year + 1900, ltime.tm_mon + 1, ltime.tm_mday,
            ltime.tm_hour, ltime.tm_min, ltime.tm_sec,
            abs(utcoffset) / 60, abs(utcoffset) % 60);
    else
        sprintf(str, "D:%04d%02d%02d%02d%02d%02dZ",
            ltime.tm_year + 1900, ltime.tm_mon + 1, ltime.tm_mday,
            ltime.tm_hour, ltime.tm_min, ltime.tm_sec);

#else
    GetLocalTime (&st);
    sprintf(str, "D:%04d%02d%02d%02d%02d%02d",
            st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
#endif  /* !WINCE */
}


/* -------------------------- Environment ------------------------------ */

char *
pdc_getenv(const char *name)
{
#ifdef HAVE_ENVVARS
    return getenv(name);
#else
    (void) name;

    return (char *) 0;
#endif
}


/* ------------------------ Language Code ------------------------------ */

/* ISO 639 Windows and Mac Language codes */
static const char lang_codes_ISO639[] =
    "ab aa af sq am ar hy as ay az ba eu bn dz bh bi br bg my be km ca zh co"
    "hr cs da nl en eo et fo fa fj fi fr fy gl gd gv ka de el kl gn gu ha he"
    "hi hu is id ia ie iu ik ga it ja jv kn ks kk rw ky rn ko ku lo la lv li"
    "ln lt mk mg ms ml mt mi mr mo mn na ne no oc or om ps pl pt pa qu rm ro"
    "ru sm sg sa sr sh st tn sn sd si ss sk sl so es su sw sv tl tg ta tt te"
    "th bo ti to ts tr tk tw ug uk ur uz vi vo cy wo xh yi yo zu"
    "pt-br en-gb en-us de-de de-ch";

pdc_bool
pdc_check_lang_code(pdc_core *pdc, const char* lang_code)
{
    pdc_bool valid = pdc_false;
    int     i;
    char*   country_code;
    char*   language;

    if ((lang_code != NULL) && *lang_code)
    {
        /* do not check for IANA or private languages */
        if (!(valid = ((lang_code[0] == 'i') || (lang_code[0] == 'x'))))
        {
            language = pdc_strdup(pdc, lang_code);
            for (i = 0; i < (int)strlen(language); i++)
            {
                if (isupper((int)language[i]))
                {
                    language[i] = (char)tolower((int)language[i]);
                }
            }


            country_code = strstr(lang_codes_ISO639, language);
            valid = (country_code != NULL);

            if (!valid && (strlen(language) > 2))
            {
                country_code = strchr(language, '-');
                if (country_code != NULL)
                {
                    country_code[0] = '\0';

                    country_code = strstr(lang_codes_ISO639, language);
                    valid = (country_code != NULL);

                    if (valid)
                    {
                        pdc_warning(pdc, PDC_E_ILLARG_LANG_CODE,
                                    lang_code, 0, 0, 0);
                    }
                }
            }

            pdc_free(pdc, language);
        }
    }

    return valid;
}


/* -------------------------- Bit arryas ------------------------------ */

void
pdc_setbit(char *bitarr, int bit)
{
    bitarr[bit/8] |= (char) (1<<(bit%8));
}

pdc_bool
pdc_getbit(char *bitarr, int bit)
{
    return (pdc_bool) (bitarr[bit/8] & (1<<(bit%8)));
}

void
pdc_setbit_text(char *bitarr, const pdc_byte *text, int len,
                int nbits, int size)
{
    int i, bit;
    pdc_ushort *ustext = (pdc_ushort *) text;

    for (i = 0; i < len; i += size)
    {
        if (size == sizeof(pdc_byte))
            bit = (int) text[i];
        else
            bit = ustext[i/size];
        if (bit < nbits) pdc_setbit(bitarr, bit);
    }
}


/* ---------- Get functions of integer binary data types --------------- */

pdc_short
pdc_get_le_short(pdc_byte *data)
{
    return (pdc_short) ((pdc_short) (data[1] << 8) | data[0]);
}

pdc_ushort
pdc_get_le_ushort(pdc_byte *data)
{
    return (pdc_ushort) ((data[1] << 8) | data[0]);
}

pdc_uint32
pdc_get_le_ulong3(pdc_byte *data)
{
    return (pdc_uint32) (((((data[2]) << 8) | data[1]) << 8) | data[0]);
}

pdc_sint32
pdc_get_le_long(pdc_byte *data)
{
    return ((pdc_sint32)
         (((((data[3] << 8) | data[2]) << 8) | data[1]) << 8) | data[0]);
}

pdc_uint32
pdc_get_le_ulong(pdc_byte *data)
{
    return (pdc_uint32)
         ((((((data[3] << 8) | data[2]) << 8) | data[1]) << 8) | data[0]);
}

pdc_short
pdc_get_be_short(pdc_byte *data)
{
    return (pdc_short) ((pdc_short) (data[0] << 8) | data[1]);
}

pdc_ushort
pdc_get_be_ushort(pdc_byte *data)
{
    return (pdc_ushort) ((data[0] << 8) | data[1]);
}

pdc_uint32
pdc_get_be_ulong3(pdc_byte *data)
{
    return (pdc_uint32) (((((data[0]) << 8) | data[1]) << 8) | data[2]);
}

pdc_sint32
pdc_get_be_long(pdc_byte *data)
{
    return ((pdc_sint32)
         (((((data[0] << 8) | data[1]) << 8) | data[2]) << 8) | data[3]);
}

pdc_uint32
pdc_get_be_ulong(pdc_byte *data)
{
    return (pdc_uint32)
        ((((((data[0] << 8) | data[1]) << 8) | data[2]) << 8) | data[3]);
}


/* ----------------- String handling for Unicode too ------------------- */

/* strlen() for unicode strings, which are terminated by two zero bytes.
 * wstrlen() returns the number of bytes in the Unicode string,
 * not including the two terminating null bytes.
 */
static size_t
wstrlen(const char *s)
{
    size_t len = 0;

    while(s[len] != 0 || s[len+1] != 0)
    {
        len += 2;
    }

    return len;
}

/*
 * This function returns the length in bytes for C and Unicode strings.
 * Note that unlike strlen() it returns the length _including_ the
 * terminator, which may be one or two null bytes.
 */
size_t
pdc_strlen(const char *text)
{
    if (pdc_is_utf16be_unicode(text) || pdc_is_utf16le_unicode(text))
	return wstrlen(text);
    else
	return strlen(text);
}


/* Allocate a local buffer and copy the string including
 * the terminating sentinel. If the string starts with the Unicode BOM
 * it is considered a Unicode string, and must be terminated by
 * two null bytes. Otherwise it is considered a plain C string and
 * must be terminated by a single null byte.
 * The caller is responsible for freeing the buffer.
 */
char *
pdc_strdup(pdc_core *pdc, const char *text)
{
    char *buf;
    size_t len;
    static const char fn[] = "pdc_strdup";

    if (text == NULL)
        pdc_error(pdc, PDC_E_INT_NULLARG, fn, 0, 0, 0);

    len = pdc_strlen(text) + 1;
    buf = (char *) pdc_malloc(pdc, len + 1, fn);
    memcpy(buf, text, len);
    buf[len] = 0;

    return buf;
}

char *
pdc_strdup_tmp(pdc_core *pdc, const char *text)
{
    char *buf;
    size_t len;
    static const char fn[] = "pdc_strdup_tmp";

    if (text == NULL)
        pdc_error(pdc, PDC_E_INT_NULLARG, fn, 0, 0, 0);

    len = pdc_strlen(text) + 1;
    buf = (char *) pdc_malloc_tmp(pdc, len + 1, fn, NULL, NULL);
    memcpy(buf, text, len);
    buf[len] = 0;

    return buf;
}

/* Allocate a local buffer and copy a locale UTF-8 string
 * provided with an UTF-8 BOM.
 * The caller is responsible for freeing the buffer.
 */
char *
pdc_strdup_withbom(pdc_core *pdc, const char *text)
{
    char *buf;
    size_t len;
    static const char fn[] = "pdc_strdup_withbom";

    if (text == NULL)
        pdc_error(pdc, PDC_E_INT_NULLARG, fn, 0, 0, 0);

    len = strlen(text);
    buf = (char *) pdc_malloc(pdc, len + 4, fn);

    pdc_copy_utf8_bom(buf);
    strcpy(&buf[3], text);

    return buf;
}

/*
 * Put out an arbitrary string.
 *
 * leni > 0 and even:   Unicode string is assumed.
 *
 * strform = readable:  Direct byte output with replacing no
 *                      printable bytes by their octal codes.
 *         = readable0: Like readable, but byte 0 will be displayed as space.
 *         = octal:     All bytes will be put out as octal.
 *         = hexa:      All bytes will be put out as hexadecimal value.
 *         = java:      Like readable, but Unicode strings
 *                      will be put out in Java notation \u,
 *                      if low byte picking is not completely possible
 *                      (see trybytes):
 *
 * trybytes = 1:        Try to reduce a Unicode string to byte string
 *                      (low byte picking).
 *
 * Output string is temporarily allocated.
 *
 */
char *
pdc_strprint(pdc_core *pdc, const char *str, int leni, int maxchar,
             pdc_strform_kind strform, pdc_bool trybytes)
{
    static const char fn[] = "pdc_strprint";

    if (str != NULL)
    {
        pdc_bool isunicode = pdc_false;
        int len = leni;

        if (!leni)
            len = (int) strlen(str);
        else
            isunicode = !(leni % 2);

        if (len)
        {
            char *ts, *tmpstr;
            pdc_byte c, cp = '.';
            pdc_ushort *ush = (pdc_ushort *) str;
            pdc_bool lbp = pdc_false;
            int i, im, len2 = len / 2;

            if (maxchar <= 0)
                maxchar = len;

            if (isunicode && trybytes)
            {
                for (i = 0; i < len2; i++)
                    if (ush[i] > 0x00FF)
                        break;
                if (i == len2)
                    lbp = pdc_true;
            }

            tmpstr = (char *) pdc_calloc_tmp(pdc, (size_t) (4 * (len + 4)), fn,
                                             NULL, NULL);
            ts = tmpstr;

            if (strform == strform_java && (lbp || !isunicode))
                strform = strform_readable;

            if (strform == strform_java || lbp)
                len = len2;

            im = (maxchar < len) ? maxchar : len;
            for (i = 0; i < im; i++)
            {
                c = (pdc_byte) (lbp ? ush[i] : str[i]);
                switch (strform)
                {
                    case strform_hexa:
                    ts += sprintf(ts, "\\x%02X", c);
                    break;

                    case strform_octal:
                    ts += sprintf(ts, "\\%03o", c);
                    break;

                    case strform_java:
                    ts += sprintf(ts, "\\u%04X", ush[i]);
                    break;

                    default:
                    if (c == 0x0 && strform == strform_readable0)
                        c = 0x20;
                    if (c < 0x20 || (c >= 0x7F && c <= 0xA0))
                    {
                        ts += sprintf(ts, "\\%03o", c);
                    }
                    else
                    {
                        if (c == '"')
                        {
                            *ts = '\\';
                            ts++;
                        }
                        *ts = (char) c;
                        ts++;
                    }
                }
            }

            if (maxchar < len)
            {
                switch (strform)
                {
                    case strform_hexa:
                    ts += sprintf(ts, "\\x%02X\\x%02X\\x%02X", cp, cp, cp);
                    break;

                    case strform_octal:
                    ts += sprintf(ts, "\\%03o\\%03o\\%03o", cp, cp, cp);
                    break;

                    case strform_java:
                    ts += sprintf(ts, "\\u%04X\\u%04X\\u%04X", cp, cp, cp);
                    break;

                    default:
                    ts += sprintf(ts, "%c%c%c", cp, cp, cp);
                    break;
                }
            }

            return tmpstr;
        }
    }

    return (char *) pdc_calloc_tmp(pdc, 1, fn, NULL, NULL);
}

const char *
pdc_utf8strprint(pdc_core *pdc, const char *str)
{
    int i = pdc_is_utf8_bytecode(str) ? 3 : 0;
    return pdc_errprintf(pdc, "%.*s", PDC_ET_MAXSTRLEN, &str[i]);
}

/*
 * Split a given text string into single strings which are separated by
 * arbitrary characters. This characters must be specified in a string.
 * If this string is NULL, " \f\n\r\t\v" (standard white spaces) is assumed.
 *
 * There is the convention that text inside braces {} will be taken verbatim.
 * Inside brace expressions braces must exist only in pairs. Braces are
 * masked by backslash.
 *
 * The caller is responsible for freeing the resultated string list
 * by calling the function pdc_cleanup_stringlist.
 *
 * Not for unicode strings.
 *
 * Return value: Number of strings.
 *               If braces aren't balanced the number is negative.
 *
 */
int
pdc_split_stringlist(pdc_core *pdc, const char *text, const char *i_separstr,
                     char ***stringlist)
{
    static const char fn[] = "pdc_split_stringlist";
    const char *separstr = " \f\n\r\t\v";
    const char *oldtext;
    char **strlist = NULL, *newtext;
    int i, it, len, jt = 0, jtb = 0, maxk = 0, count = 0, inside = 0;

    if (text == NULL)
	pdc_error(pdc, PDC_E_INT_NULLARG, fn, 0, 0, 0);

    if (stringlist)
        *stringlist = NULL;
    if (i_separstr)
        separstr = i_separstr;

    /* check for empty string */
    i = (int) strspn(text, separstr);
    oldtext = &text[i];
    len = (int) strlen(oldtext);
    if (!len) return 0;

    /* check for UTF-8-BOM */
    if (pdc_is_utf8_bytecode(oldtext))
    {
        oldtext = &text[i + 3];
        len -= 3;
        i = (int) strspn(oldtext, separstr);
        oldtext = &oldtext[i];
        len -= i;
        if (!len) return 0;
    }

    /* new string */
    newtext = (char *) pdc_malloc(pdc, (size_t) (len + 1), fn);
    for (it = 0; it <= len; it++)
    {
        /* check for separators */
        if (it == len)
            i = 1;
        else if (inside <= 0)
            i = (int) strspn(&oldtext[it], separstr);
        else
            i = 0;

        /* close text part */
        if (i)
        {
            newtext[jt] = 0;
            if (count == maxk)
            {
                maxk += 16;
                strlist = (strlist == NULL) ?
                    (char **) pdc_malloc(pdc, maxk * sizeof(char *), fn):
                    (char **) pdc_realloc(pdc, strlist, maxk *
                                          sizeof(char *), fn);
            }
            strlist[count] = &newtext[jtb];
            count++;

            /* Exit */
            it += i;
            if (it >= len ) break;

            /* new text part */
            jt++;
            jtb = jt;
        }

        /* open and close brace */
        if (oldtext[it] == '{')
        {
            inside++;
            if (inside == 1)
                continue;
        }
        else if (oldtext[it] == '}')
        {
            inside--;
            if (inside == 0)
                continue;
        }

        /* masked braces */
        if (oldtext[it] == '\\' &&
            (oldtext[it+1] == '{' || oldtext[it+1] == '}'))
        {
            if (it > 0 && oldtext[it-1] == '\\')
                continue;
            it++;
        }

        /* save character */
        newtext[jt] = oldtext[it];
        jt++;
    }

    if (stringlist)
        *stringlist = strlist;
    return inside ? -count : count;
}

void
pdc_cleanup_stringlist(pdc_core *pdc, char **stringlist)
{
    if(stringlist != NULL)
    {
	if(stringlist[0] != NULL)
	    pdc_free(pdc, stringlist[0]);

 	pdc_free(pdc, stringlist);
    }
}

/*
 * Compares its arguments and returns an integer less than,
 * equal to, or greater than zero, depending on whether s1
 * is lexicographically less than, equal to, or greater than s2.
 * Null pointer values for s1 and s2 are treated the same as pointers
 * to empty strings.
 *
 * Presupposition: basic character set
 *
 * Return value:  < 0  s1 <  s2;
 *		  = 0  s1 == s2;
 *		  > 0  s1 >  s2;
 *
 */
int
pdc_stricmp(const char *s1, const char *s2)
{
    char c1, c2;

    if (s1 == s2) return (0);
    if (s1 == NULL) return (-1);
    if (s2 == NULL) return (1);

    for (; *s1 != '\0' && *s2 != '\0';  s1++, s2++)
    {
	if ((c1 = *s1) == (c2 = *s2))
	    continue;

        if (isupper((int)c1)) c1 = (char) tolower((int)c1);
        if (isupper((int)c2)) c2 = (char) tolower((int)c2);
	if (c1 != c2)
	    break;
    }

    /* TODO: this function is useful for checks on (in)equality only;
    ** it does NOT compare correctly!
    */
    return (*s1 - *s2);
}


/*
 * Compares its arguments and returns an integer less than,
 * equal to, or greater than zero, depending on whether s1
 * is lexicographically less than, equal to, or greater than s2.
 * But only up to n characters compared (n less than or equal
 * to zero yields equality).Null pointer values for s1 and s2
 * are treated the same as pointers to empty strings.
 *
 * Presupposition: basic character set
 *
 * Return value:  < 0  s1 <  s2;
 *		  = 0  s1 == s2;
 *		  > 0  s1 >  s2;
 *
 */
int
pdc_strincmp(const char *s1, const char *s2, int n)
{
    char c1, c2;
    int  i;

    if (s1 == s2)   return (0);
    if (s1 == NULL) return (-1);
    if (s2 == NULL) return (1);

    for (i=0;  i < n && *s1 != '\0' && *s2 != '\0';  i++, s1++, s2++)
    {
	if ((c1 = *s1) == (c2 = *s2))
	    continue;

        if (isupper((int)c1)) c1 = (char) tolower((int)c1);
        if (isupper((int)c2)) c2 = (char) tolower((int)c2);
	if (c1 != c2)
	    break;
    }
    return ((i < n) ?  (int)(*s1 - *s2) : 0);
}

/*
 * pdc_strtrim removes trailing white space characters from an input string.
 * pdc_str2trim removes leading and trailing white space characters from an
 * input string..
 */
char *
pdc_strtrim(char *str)
{
    int i;

    for (i = (int) strlen(str) - 1; i >= 0; i--)
        if (!isspace((unsigned char) str[i])) break;
    str[i + 1] = '\0';

    return str;
}

char *
pdc_str2trim(char *str)
{
    int i;

    for (i = (int) strlen(str) - 1; i >= 0; i--)
        if (!isspace((unsigned char) str[i])) break;
    str[i + 1] = '\0';

    for (i = 0; ; i++)
        if (!isspace((unsigned char) str[i])) break;
    if (i > 0)
        memmove(str, &str[i], strlen(&str[i]) + 1);

    return str;
}

void
pdc_swap_bytes(char *instring, int inlen, char *outstring)
{
    char c;
    int i,j;

    if (instring == NULL)
        return;

    if (outstring == NULL)
        outstring = instring;

    inlen = 2 * inlen / 2;
    for (i = 0; i < inlen; i++)
    {
        j = i;
        i++;
        c = instring[j];
        outstring[j] = instring[i];
        outstring[i] = c;
    }
}

void
pdc_swap_unicodes(char *instring)
{
    if (instring &&
        ((pdc_is_utf16be_unicode(instring) && !PDC_ISBIGENDIAN) ||
         (pdc_is_utf16le_unicode(instring) &&  PDC_ISBIGENDIAN)))
        pdc_swap_bytes(&instring[2], (int) (wstrlen(instring) - 2), NULL);
}

void
pdc_inflate_ascii(const char *instring, int inlen, char *outstring,
                  pdc_text_format textformat)
{
    int i, j;
    pdc_bool is_bigendian = (textformat == pdc_utf16be) ||
                            (textformat == pdc_utf16 && PDC_ISBIGENDIAN);

    j = 0;
    for (i = 0; i < inlen; i++)
    {
        if (is_bigendian)
        {
            outstring[j] = 0;
            j++;
            outstring[j] = instring[i];
        }
        else
        {
            outstring[j] = instring[i];
            j++;
            outstring[j] = 0;
        }
        j++;
    }
}

/*
 * pdc_str2double converts a null terminated and trimed string
 * to a double precision number
 */
pdc_bool
pdc_str2double(const char *string, double *o_dz)
{
    const char *s = string;
    double dz = 0;
    int is = 1, isd = 0;

    *o_dz = 0;

    /* sign */
    if (*s == '-')
    {
        is = -1;
        s++;
    }
    else if (*s == '+')
        s++;

    if (!*s)
        return pdc_false;

    /* places before decimal point */
    isd = isdigit((int) *s);
    if (isd)
    {
        do
        {
            dz = 10 * dz + *s - '0';
            s++;
        }
        while (isdigit((int) *s));
    }

    /* decimal point */
    if (*s == '.' || *s == ',')
    {
        const char *sa;
        double adz = 0;

        s++;
        isd = isdigit((int) *s);
        if (!isd)
            return pdc_false;

        /* places after decimal point */
        sa = s;
        do
        {
            adz = 10 * adz + *s - '0';
            s++;
        }
        while (isdigit((int) *s));
        dz += adz / pow(10.0, (double)(s - sa));
    }

    /* power sign */
    if (*s == 'e' || *s == 'E')
    {
        s++;
        if (!isd)
            return pdc_false;

        /* sign */
        if (!*s)
        {
            dz *= 10;
        }
        else
        {
            int isp = 1;
            double pdz = 0, pdl = log10(dz);

            if (*s == '-')
            {
                isp = -1;
                s++;
            }
            else if (*s == '+')
                s++;

            if (!isdigit((int) *s))
                return pdc_false;
            do
            {
                pdz = 10 * pdz + *s - '0';
                s++;
            }
            while (isdigit((int) *s));


            if (*s || fabs(pdl + pdz) > 300.0)
                return pdc_false;

            dz *= pow(10.0, isp * pdz);
        }
    }
    else if(*s)
    {
        return pdc_false;
    }

    *o_dz = is * dz;
    return pdc_true;
}

/*
 * pdc_str2integer converts a null terminated and trimed string
 * to an hexadecimal or decimal integer number of arbitrary size
 */
pdc_bool
pdc_str2integer(const char *string, int flags, void *o_iz)
{
    const char *s = string;
    double dz = 0;
    pdc_char cz = 0;
    pdc_short sz = 0;
    pdc_sint32 lz = 0;
    pdc_byte ucz = 0;
    pdc_ushort usz = 0;
    pdc_uint32 ulz = 0;
    int is = 1, lzd;

    if (flags & PDC_INT_CHAR)
        memcpy(o_iz, &cz, sizeof(pdc_char));
    else if (flags & PDC_INT_SHORT)
        memcpy(o_iz, &sz, sizeof(pdc_short));
    else
        memcpy(o_iz, &lz, sizeof(pdc_sint32));

    /* sign */
    if (*s == '-')
    {
        if (flags & PDC_INT_UNSIGNED)
            return pdc_false;
        is = -1;
        s++;
    }
    else if (*s == '+')
        s++;

    if (!*s)
        return pdc_false;

    /* hexadecimal test */
    if (!(flags & PDC_INT_DEC))
    {
        if (*s == '<')
            s += 1;
        else if (*s == 'x' || *s == 'X')
            s += 1;
        else if (!strncmp(s, "0x", 2) || !strncmp(s, "0X", 2))
            s += 2;
        if (s > string)
        {
            if (!*s)
                return pdc_false;
            flags |= PDC_INT_HEXADEC;
        }
    }

    /* hexadecimal */
    if (flags & PDC_INT_HEXADEC)
    {
        while (isxdigit((int) *s))
        {
            if (isalpha(*s))
                lzd = (isupper((int) *s) ? 'A' : 'a') - 10;
            else
                lzd = '0';
            dz = 16 * dz + *s - lzd;
            s++;
        }
        if (*string == '<')
        {
            if (*s == '>')
                s += 1;
            else
                return pdc_false;
        }
    }

    /* decimal */
    else
    {
        while (isdigit((int) *s))
        {
            dz = 10 * dz + *s - '0';
            s++;
        }
    }
    if (*s)
        return pdc_false;

    dz *= is;
    if (flags & PDC_INT_CHAR)
    {
        if (flags & PDC_INT_UNSIGNED)
        {
            if (dz > PDC_UCHAR_MAX)
                return pdc_false;
            ucz = (pdc_byte) dz;
            memcpy(o_iz, &ucz, sizeof(pdc_byte));
        }
        else
        {
            if (dz < PDC_SCHAR_MIN || dz > PDC_SCHAR_MAX)
                return pdc_false;
            cz = (pdc_char) dz;
            memcpy(o_iz, &cz, sizeof(pdc_char));
        }
    }
    else if (flags & PDC_INT_SHORT)
    {
        if (flags & PDC_INT_UNSIGNED)
        {
            if (dz > PDC_USHRT_MAX)
                return pdc_false;
            usz = (pdc_ushort) dz;
            memcpy(o_iz, &usz, sizeof(pdc_ushort));
        }
        else
        {
            if (dz < PDC_SHRT_MIN || dz > PDC_SHRT_MAX)
                return pdc_false;
            sz = (pdc_short) dz;
            memcpy(o_iz, &sz, sizeof(pdc_short));
        }
    }
    else
    {
        if (flags & PDC_INT_UNSIGNED)
        {
            if (dz > PDC_UINT_MAX)
                return pdc_false;
            ulz = (pdc_uint32) dz;
            memcpy(o_iz, &ulz, sizeof(pdc_uint32));
        }
        else
        {
            if (dz < PDC_INT_MIN || dz > PDC_INT_MAX)
                return pdc_false;
            lz = (pdc_sint32) dz;
            memcpy(o_iz, &lz, sizeof(pdc_sint32));
        }
    }

    return pdc_true;
}

static const char digits[] = "0123456789ABCDEF";

static char *
pdc_ltoa(char *buf, long n, int width, char pad, int base)
{
    char        aux[100];
    int         k, i = sizeof aux;
    char *      dest = buf;
    pdc_bool	sign;

    if (n == 0)
    {
        if (width == 0)
            width = 1;

        for (k = 0; k < width; ++k)
            *(dest++) = '0';

        return dest;
    }

    if (n < 0 && base == 10)
    {
	--width;
	sign = pdc_true;
	aux[--i] = digits[- (n % base)];
	n = n / -base;
    }
    else
    {
	sign = pdc_false;
	aux[--i] = digits[n % base];
	n = n / base;
    }

    while (0 < n)
    {
        aux[--i] = digits[n % base];
        n = n / base;
    }

    width -= (int) (sizeof aux) - i;
    for (k = 0; k < width; ++k)
        *(dest++) = pad;

    if (sign)
	*(dest++) = '-';

    memcpy(dest, &aux[i], sizeof aux - i);
    return dest + sizeof aux - i;
} /* pdc_ltoa */


static char *
pdc_off_t2a(char *buf, pdc_off_t n, int width, char pad, int base)
{
    char        aux[100];
    int         k, i = sizeof aux;
    char *      dest = buf;
    pdc_bool	sign;

    if (n == 0)
    {
        if (width == 0)
            width = 1;

        for (k = 0; k < width; ++k)
            *(dest++) = '0';

        return dest;
    }

    if (n < 0 && base == 10)
    {
	--width;
	sign = pdc_true;
	aux[--i] = digits[- (n % base)];
	n = n / -base;
    }
    else
    {
	sign = pdc_false;
	aux[--i] = digits[n % base];
	n = n / base;
    }

    while (0 < n)
    {
        aux[--i] = digits[n % base];
        n = n / base;
    }

    width -= (int) (sizeof aux) - i;
    for (k = 0; k < width; ++k)
        *(dest++) = pad;

    if (sign)
	*(dest++) = '-';

    memcpy(dest, &aux[i], sizeof aux - i);
    return dest + sizeof aux - i;
} /* pdc_off_t2a */


/*
 * pdc_ftoa converts a float number to string (PDF conforming)
 */

/* Acrobat viewers have an upper limit on real and integer numbers */
#define PDF_BIGREAL             (32768.0)
#define PDF_BIGINT              (2147483647.0)

static char *
pdc_ftoa(pdc_core *pdc, char *buf, double x)
{
    static const long pow10[] = { 1, 10, 100, 1000, 10000, 100000, 1000000 };

    char *      dest = buf;
    double      integ, fract, powd;
    int         ifd = pdc_get_floatdigits(pdc);
    long        f;

    if (fabs(x) < PDF_SMALLREAL)
    {
        *dest = '0';
        return dest + 1;
    }

    if (x < 0)
    {
        x = -x;
        *(dest++) = '-';
    }

    if (x >= PDF_BIGREAL)
    {
        if (x > PDF_BIGINT)
            pdc_error(pdc, PDC_E_INT_FLOATTOOLARGE, 0, 0, 0, 0);

        return pdc_ltoa(dest, (long) (x + 0.5), 0, ' ', 10);
    }

    powd = pow10[ifd];
    fract = modf(x, &integ);
    f = (long) (fract * powd + 0.5);

    if (f == powd)
    {
        integ += 1.0;
        f = 0;
    }

    if (integ == 0 && f == 0)   /* avoid "-0" */
        dest = buf;

    dest = pdc_ltoa(dest, (long) integ, 0, ' ', 10);

    if (f != 0)
    {
        char *  aux;
        long    rem;

        *(dest++) = '.';

        do      /* avoid trailing zeros */
        {
            rem = f % 10;
            f = f / 10;
            --ifd;
        } while (rem == 0);

        aux = dest + ifd + 1;
        dest[ifd--] = digits[rem];

        for (; 0 <= ifd; --ifd)
        {
            dest[ifd] = digits[f % 10];
            f = f / 10;
        }

        return aux;
    }

    return dest;
} /* pdc_ftoa */

static int
pdc_vxprintf(
    pdc_core *pdc,
    char *cp,
    FILE *fp,
    const char *format,
    va_list args)
{
    char aux[1024];
    char *buf = cp ? cp : aux;
    char *dest = buf;

    for (/* */ ; /* */ ; /* */)
    {
        int		width = 0;
        int		prec = 0;
        char		pad = ' ';
	pdc_bool	left_justify = pdc_false;

        /* as long as there is no '%', just print.
        */
        while (*format != 0 && *format != '%')
            *(dest++) = *(format++);

        if (*format == 0)
        {
	    if (fp != (FILE *) 0)
	    {
		if (dest > buf)
		    fwrite(buf, 1, (size_t) (dest - buf), fp);
	    }
	    else
		*dest = 0;

            return (int) (dest - buf);
        }

	/* get the "flags", if any.
	*/
        if (*(++format) == '-')
        {
            left_justify = pdc_true;
            ++format;
        }

        if (*format == '0')
        {
	    if (!left_justify)
		pad = '0';

            ++format;
        }

	/* get the "width", if present.
	*/
	if (*format == '*')
	{
	    width = va_arg(args, int);	/* TODO: sign? */
	    ++format;
	}
	else
	{
	    while (isdigit((int) *format))
		width = 10 * width + *(format++) - '0';
	}

	/* get the "precision", if present.
	*/
	if (*format == '.')
	{
	    ++format;

	    if (*format == '*')
	    {
		prec = va_arg(args, int);	/* TODO: sign? */
		++format;
	    }
	    else
	    {
		while (isdigit((int) *format))
		    prec = 10 * prec + *(format++) - '0';
	    }
	}

        switch (*format)
        {
            case 'x':
            case 'X':
                dest = pdc_off_t2a(dest, va_arg(args, int), width, pad, 16);
                break;

            case 'c':
                *(dest++) = (char) va_arg(args, int);
                break;

            case 'd':
                dest = pdc_off_t2a(dest, va_arg(args, int), width, pad, 10);
                break;

            case 'g':   /* for use in pdc_trace_api() */
            case 'f':
                dest = pdc_ftoa(pdc, dest, va_arg(args, double));
                break;

            case 'l':
            {
                pdc_off_t n;

		if (format[1] == 'l')
		{
		    n = va_arg(args, pdc_off_t);
		    ++format;
		}
		else
		{
		    n = va_arg(args, long);
		}

                switch (*(++format))
                {
                    case 'x':
                    case 'X':
                        dest = pdc_off_t2a(dest, n, width, pad, 16);
                        break;

                    case 'd':
                        dest = pdc_off_t2a(dest, n, width, pad, 10);
                        break;

                    default:
                        pdc_error(pdc, PDC_E_INT_BADFORMAT,
                            pdc_errprintf(pdc, "l%c",
                                isprint((int) *format) ? *format : '?'),
                            pdc_errprintf(pdc, "0x%02X", *format),
                            0, 0);
                }

                break;
            }

            case 'p':
            {
                void *ptr = va_arg(args, void *);
                dest += sprintf(dest, "%p", ptr);
                break;
            }

            case 's':
            case 'T':
            {
                char *  str = va_arg(args, char *);
                const char *cstr = str;
                size_t  len;

                if (str == 0)
                    cstr = "(NULL)";
                len = strlen(cstr);

                if (*format == 'T')
                {
                    int l = va_arg(args, int);

                    if (str != 0)
                    {
                        cstr = pdc_print_tracestring(pdc, str, l);
                        len = strlen(cstr);
                    }
                }

		if (left_justify && len < (size_t) width)
		{
		    memset(dest, pad, width - len);
		    dest += width - len;
		}

                if (len != 0)
                {
                    if (fp != (FILE *) 0)
                    {
                        if (dest > buf)
                        {
                            fwrite(buf, 1, (size_t) (dest - buf), fp);
                            dest = buf;
                        }

                        fwrite(cstr, 1, len, fp);
                    }
                    else
                    {
                        memcpy(dest, cstr, len);
                        dest += len;
                    }
                }

		if (!left_justify && len < (size_t) width)
		{
		    memset(dest, pad, width - len);
		    dest += width - len;
		}

                break;
            }

            case '%':
                *(dest++) = '%';
                break;

            default:
                pdc_error(pdc, PDC_E_INT_BADFORMAT,
                    pdc_errprintf(pdc, "%c", isprint((int) *format) ?
                                  *format : '?'),
                    pdc_errprintf(pdc, "0x%02X", *format),
                    0, 0);
        } /* switch */

        ++format;
    } /* loop */
} /* pdc_vxprintf */


/*
 * Formatted output to file
 */
int
pdc_vfprintf(pdc_core *pdc, FILE *fp, const char *format, va_list args)
{
    return pdc_vxprintf(pdc, 0, fp, format, args);
} /* pdc_vfprintf */

int
pdc_fprintf(pdc_core *pdc, FILE *fp, const char *format, ...)
{
    int result;
    va_list ap;

    va_start(ap, format);
    result = pdc_vxprintf(pdc, 0, fp, format, ap);
    va_end(ap);

    return result;
} /* pdc_fprintf */


/*
 * Formatted output to character string
 */
int
pdc_vsprintf(pdc_core *pdc, char *buf, const char *format, va_list args)
{
    return pdc_vxprintf(pdc, buf, 0, format, args);
} /* pdc_vsprintf */

int
pdc_sprintf(pdc_core *pdc, char *buf, const char *format, ...)
{
    int result;
    va_list ap;

    va_start(ap, format);
    result = pdc_vxprintf(pdc, buf, 0, format, ap);
    va_end(ap);

    return result;
} /* pdc_sprintf */


/* --------------------- name tree handling ----------------------- */

struct pdc_branch_s
{
    char        *name;     /* name - must be allocated pointer */
    void        *data;     /* private data - must be allocated pointer */
    int          nalloc;   /* number of allocated kid structs */
    int          nkids;    /* number of kids */
    pdc_branch **kids;     /* kids */
    pdc_branch  *parent;   /* parent branch */
};

pdc_branch *
pdc_init_tree(pdc_core *pdc)
{
    return pdc_create_treebranch(pdc, NULL, "__tree__root__",
                                 NULL, 0, 0, NULL, NULL);
}

pdc_branch *
pdc_create_treebranch(pdc_core *pdc, pdc_branch *root, const char *pathname,
                      void *data, int flags, int size,
                      pdc_branch_error *errcode, const char **name_p)
{
    static const char fn[] = "pdc_create_branch";
    char *name = NULL;
    pdc_branch *branch = NULL;
    pdc_branch *kid = NULL;
    pdc_branch *parent = NULL;
    char **namelist;
    int i, j, k, nnames, nkids;

    if (errcode) *errcode = tree_ok;
    if (name_p) *name_p = "";

    if (root)
    {
        /* search for parent branch */
        parent = root;
        nnames = pdc_split_stringlist(pdc, pathname, PDC_NAME_SEPARSTRG,
                                      &namelist);
        for (i = 0; i < nnames; i++)
        {
            /* parent branch must not be a leaf branch */
            if (!parent->nalloc)
            {
                if (errcode) *errcode = tree_isleaf;
                pdc_cleanup_stringlist(pdc, namelist);
                return NULL;
            }
            if (i == nnames - 1)
                break;

            name = namelist[i];
            if (name_p)
                *name_p = pdc_errprintf(pdc, "%.*s", PDC_ET_MAXSTRLEN, name);

            nkids = parent->nkids;
            for (j = 0; j < nkids; j++)
            {
                kid = parent->kids[j];
                k = pdc_is_utf8_bytecode(kid->name) ? 3 : 0;
                if (!strcmp(&kid->name[k], name))
                {
                    parent = kid;
                    break;
                }
            }
            if (j == nkids)
            {
                if (errcode) *errcode = tree_notfound;
                pdc_cleanup_stringlist(pdc, namelist);
                return NULL;
            }
        }

        if (pdc_is_utf8_bytecode(pathname))
            name = pdc_strdup_withbom(pdc, namelist[nnames - 1]);
        else
            name = pdc_strdup(pdc, namelist[nnames - 1]);
        pdc_cleanup_stringlist(pdc, namelist);

        /* kids must have different names */
        for (j = 0; j < parent->nkids; j++)
        {
            kid = parent->kids[j];
            if (!strcmp(kid->name, name))
            {
                if (errcode) *errcode = tree_nameexists;
                if (name_p) *name_p =
                    pdc_errprintf(pdc, "%.*s", PDC_ET_MAXSTRLEN, name);
                pdc_free(pdc, name);
                return NULL;
            }
        }
    }
    else
    {
        parent = NULL;
        name = pdc_strdup(pdc, pathname);
    }

    branch = (pdc_branch *) pdc_malloc(pdc, sizeof(pdc_branch), fn);
    branch->name = name;
    branch->data = data;
    if (flags & PDC_TREE_ISLEAF)
    {
        branch->nalloc = 0;
        branch->nkids = 0;
        branch->kids = NULL;
    }
    else
    {
        branch->nalloc = PDC_KIDS_CHUNKSIZE;
        branch->nkids = 0;
        branch->kids = (pdc_branch **) pdc_malloc(pdc,
                            branch->nalloc * sizeof(pdc_branch *), fn);
    }
    branch->parent = parent;

    /* insert kid */
    if (parent)
    {
        if (parent->nkids == parent->nalloc)
        {
            parent->nalloc *= 2;
            parent->kids = (pdc_branch **) pdc_realloc(pdc, parent->kids,
                                parent->nalloc * sizeof(pdc_branch *), fn);
        }
        parent->kids[parent->nkids] = branch;
        (parent->nkids)++;

        if ((flags & PDC_TREE_INHERIT) && parent->data)
            memcpy(branch->data, parent->data, (size_t) size);
    }

    return branch;
}

void
pdc_deactivate_name_treebranch(pdc_core *pdc, pdc_branch *branch)
{
    static const char fn[] = "pdc_deactivate_name_treebranch";
    size_t len = strlen(branch->name);

    branch->name = (char *) pdc_realloc(pdc, branch->name, len + 2, fn);
    branch->name[len] = PDC_NAME_SEPARSIGN;
    branch->name[len+1] = 0;
}

char *
pdc_get_name_treebranch(pdc_branch *branch)
{
    return branch->name;
}

pdc_branch *
pdc_get_parent_treebranch(pdc_branch *branch)
{
    return branch->parent;
}

void *
pdc_get_data_treebranch(pdc_branch *branch)
{
    return branch->data;
}

pdc_branch **
pdc_get_kids_treebranch(pdc_branch *branch, int *nkids)
{
    *nkids = branch->nkids;
    return branch->kids;
}

void
pdc_cleanup_treebranch(pdc_core *pdc, pdc_branch *branch)
{
    int i;

    if (branch->name)
        pdc_free(pdc, branch->name);

    if (branch->data)
        pdc_free(pdc, branch->data);

    if (branch->kids)
    {
        for(i = 0; i < branch->nkids; i++)
            pdc_cleanup_treebranch(pdc, branch->kids[i]);
        pdc_free(pdc, branch->kids);
    }

    pdc_free(pdc, branch);
}
