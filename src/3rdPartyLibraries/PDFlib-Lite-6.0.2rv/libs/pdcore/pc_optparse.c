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

/* $Id: pc_optparse.c,v 1.1 2005/10/11 17:18:13 vuduc2 Exp $
 *
 * Parser options routines
 *
 */

#include "pc_util.h"
#include "pc_geom.h"

/* result of an option */
struct pdc_resopt_s
{
    int               numdef;  /* number of definitions */
    const pdc_defopt *defopt;  /* pointer to option definition */
    int               num;     /* number of parsed values */
    void             *val;     /* list of parsed values */
    char             *origval; /* original value as string */
    int               flags;   /* flags */
    int               pcmask;  /* percentage mask */
    int               currind; /* index of current option */
    int               lastind; /* index of last option */
    pdc_bool          isutf8;  /* optionlist UTF-8 encoded */
};

/* sizes of option types. must be parallel to pdc_opttype */
static const size_t pdc_typesizes[] =
{
    sizeof (pdc_bool),
    sizeof (char *),
    sizeof (int),
    sizeof (int),
    sizeof (float),
    sizeof (double),
    sizeof (pdc_scalar),
    sizeof (pdc_ushort),
    sizeof (int),
    sizeof (int),
    sizeof (int),
    sizeof (int),
    sizeof (int),
    sizeof (int),
    sizeof (int),
    sizeof (int),
    sizeof (int),
    sizeof (int),
    sizeof (int),
    sizeof (int),
    sizeof (int),
    sizeof (int),
};

static const pdc_keyconn pdc_handletypes[] =
{
    {"action",     pdc_actionhandle},
    {"bookmark",   pdc_bookmarkhandle},
    {"color",      pdc_colorhandle},
    {"document",   pdc_documenthandle},
    {"font",       pdc_fonthandle},
    {"gstate",     pdc_gstatehandle},
    {"iccprofile", pdc_iccprofilehandle},
    {"image",      pdc_imagehandle},
    {"layer",      pdc_layerhandle},
    {"page",       pdc_pagehandle},
    {"pattern",    pdc_patternhandle},
    {"shading",    pdc_shadinghandle},
    {"template",   pdc_templatehandle},
    {"textflow",   pdc_textflowhandle},
};

int
pdc_get_keycode(const char *keyword, const pdc_keyconn *keyconn)
{
    int i;
    for (i = 0; keyconn[i].word != 0; i++)
    {
        if (!strcmp(keyword, keyconn[i].word))
            return keyconn[i].code;
    }
    return PDC_KEY_NOTFOUND;
}

int
pdc_get_keycode_ci(const char *keyword, const pdc_keyconn *keyconn)
{
    int i;
    for (i = 0; keyconn[i].word != 0; i++)
    {
        if (!pdc_stricmp(keyword, keyconn[i].word))
            return keyconn[i].code;
    }
    return PDC_KEY_NOTFOUND;
}

int
pdc_get_keymask_ci(pdc_core *pdc, const char *option,
                   const char *keywordlist, const pdc_keyconn *keyconn)
{
    char **keys = NULL;
    int nkeys, i, j, k = 0;

    nkeys = pdc_split_stringlist(pdc, keywordlist, NULL, &keys);

    for (j = 0; j < nkeys; j++)
    {
        for (i = 0; keyconn[i].word != NULL; i++)
            if (!pdc_stricmp(keys[j], keyconn[i].word))
                break;

        if (keyconn[i].word == NULL)
        {
            const char *stemp = pdc_errprintf(pdc, "%.*s",
                                              PDC_ET_MAXSTRLEN, keys[j]);
            pdc_cleanup_stringlist(pdc, keys);
            pdc_set_errmsg(pdc, PDC_E_OPT_ILLKEYWORD, option, stemp, 0, 0);
            return PDC_KEY_NOTFOUND;
        }

        k |= keyconn[i].code;
    }

    pdc_cleanup_stringlist(pdc, keys);
    return k;
}

const char *
pdc_get_keyword(int keycode, const pdc_keyconn *keyconn)
{
    int i;
    for (i = 0; keyconn[i].word != 0; i++)
    {
        if (keycode == keyconn[i].code)
            return keyconn[i].word;
    }
    return NULL;
}

const char *
pdc_get_int_keyword(const char *keyword, const pdc_keyconn *keyconn)
{
    int i;
    for (i = 0; keyconn[i].word != 0; i++)
    {
        if (!pdc_stricmp(keyword, keyconn[i].word))
            return keyconn[i].word;
    }
    return NULL;
}

pdc_bool
pdc_get_integerkeycode(const char *string, const pdc_keyconn *keyconn,
                       int flags, void *o_iz)
{
    int iz = PDC_KEY_NOTFOUND;
    pdc_bool retval;

    if (keyconn)
    {
        if (flags & PDC_INT_CASESENS)
            iz = pdc_get_keycode(string, keyconn);
        else
            iz = pdc_get_keycode_ci(string, keyconn);
    }

    if (iz != PDC_KEY_NOTFOUND)
    {
        if (flags & PDC_INT_CHAR)
        {
            pdc_char c = (pdc_char) iz;
            memcpy(o_iz, &c, sizeof(pdc_char));
        }
        else if (flags & PDC_INT_SHORT)
        {
            pdc_short si = (pdc_short) iz;
            memcpy(o_iz, &si, sizeof(pdc_short));
        }
        else
        {
            memcpy(o_iz, &iz, sizeof(pdc_sint32));
        }
        retval = pdc_true;
    }
    else
    {
        retval = pdc_str2integer(string, flags, o_iz);
    }

    return retval;
}

void
pdc_cleanup_optstringlist(pdc_core *pdc, char **stringlist, int ns)
{
    int j;

    for (j = 0; j < ns; j++)
    {
        if (stringlist[j] != NULL)
            pdc_free(pdc, stringlist[j]);
    }
    pdc_free(pdc, stringlist);
}

static void
pdc_delete_optvalue(pdc_core *pdc, pdc_resopt *resopt)
{
    if (resopt->val && !(resopt->flags & PDC_OPT_SAVEALL))
    {
        if (resopt->defopt->type == pdc_stringlist)
        {
            int j;
            int ja = (resopt->flags & PDC_OPT_SAVE1ELEM) ? 1 : 0;
            char **s = (char **) resopt->val;
            for (j = ja; j < resopt->num; j++)
                if (s[j])
                    pdc_free(pdc, s[j]);
        }
        pdc_free(pdc, resopt->val);
        resopt->val = NULL;
    }
    if (resopt->origval && !(resopt->flags & PDC_OPT_SAVEORIG))
    {
        pdc_free(pdc, resopt->origval);
        resopt->origval = NULL;
    }
    resopt->num = 0;
}

static int
pdc_optname_compare(const void *a, const void *b)
{
    return (strcmp(((pdc_resopt *)a)->defopt->name,
                   ((pdc_resopt *)b)->defopt->name));
}

/* destructor function for freeing temporary memory */
static void
pdc_cleanup_optionlist_tmp(void *opaque, void *mem)
{
    if (mem)
    {
        pdc_core *pdc = (pdc_core *) opaque;
        pdc_resopt *resopt = (pdc_resopt *) mem;
        int i;

        for (i = 0; i < resopt[0].numdef; i++)
            pdc_delete_optvalue(pdc, &resopt[i]);
    }
}

pdc_resopt *
pdc_parse_optionlist(pdc_core *pdc, const char *optlist,
                     const pdc_defopt *defopt,
                     const pdc_clientdata *clientdata, pdc_bool verbose)
{
    static const char *fn = "pdc_parse_optionlist";
    pdc_bool logg5 = pdc_trace_protocol_is_enabled(pdc, 5, trc_optlist);
    const char *stemp1 = NULL, *stemp2 = NULL, *stemp3 = NULL;
    char **items = NULL, *keyword = NULL;
    char **values = NULL, *value = NULL, **strings = NULL;
    int i, nd, is, iss, it, iv, numdef, nitems = 0, nvalues, errcode = 0;
    void *resval;
    double dz, maxval;
    int retval, iz;
    pdc_sint32 lz = 0;
    pdc_uint32 ulz = 0;
    pdc_ushort usz = 0;
    size_t len;
    const pdc_defopt *dopt = NULL;
    pdc_resopt *resopt = NULL;
    pdc_bool ignore = pdc_false;
    pdc_bool boolval = pdc_false;
    pdc_bool tocheck = pdc_false;
    pdc_bool issorted = pdc_true;
    pdc_bool ishandle = pdc_true;
    pdc_bool hastobepos;

    /* decrement handles */
    hastobepos = clientdata && clientdata->hastobepos ? pdc_true : pdc_false;

    pdc_trace_protocol(pdc, 1, trc_optlist, "\n\tOption list: \"%s\"\n",
                       optlist ? optlist : "");

    /* split option list */
    if (optlist)
        nitems = pdc_split_stringlist(pdc, optlist, PDC_OPT_LISTSEPS, &items);
    if (nitems < 0)
    {
        keyword = (char *) optlist;
        errcode = PDC_E_OPT_NOTBAL;
        goto PDC_OPT_SYNTAXERROR;
    }

    /* initialize result list */
    for (numdef = 0; defopt[numdef].name != NULL; numdef++) {
	/* */ ;
    }

    /* allocate temporary memory for option parser result struct */
    resopt = (pdc_resopt *) pdc_calloc_tmp(pdc, numdef * sizeof(pdc_resopt),
                                    fn, pdc, pdc_cleanup_optionlist_tmp);
    for (i = 0; i < numdef; i++)
    {
        resopt[i].numdef = numdef;
        resopt[i].defopt = &defopt[i];

        if (defopt[i].flags & PDC_OPT_IGNOREIF1 ||
            defopt[i].flags & PDC_OPT_IGNOREIF2 ||
            defopt[i].flags & PDC_OPT_REQUIRIF1 ||
            defopt[i].flags & PDC_OPT_REQUIRIF2 ||
            defopt[i].flags & PDC_OPT_REQUIRED)
            tocheck = pdc_true;

        if (i && issorted)
            issorted = (strcmp(defopt[i-1].name, defopt[i].name) <= 0) ?
                       pdc_true : pdc_false;
    }

    /* loop over all option list elements */
    for (is = 0; is < nitems; is++)
    {
        /* search keyword */
        boolval = pdc_undef;
        keyword = items[is];
        for (it = 0; it < numdef; it++)
        {
            /* special handling for booleans */
            if (defopt[it].type == pdc_booleanlist)
            {
                if (!pdc_stricmp(keyword, defopt[it].name) ||
                    (keyword[1] != 0 &&
                     !pdc_stricmp(&keyword[2], defopt[it].name)))
                {
                    iss = is + 1;
                    if (iss == nitems ||
                        (pdc_stricmp(items[iss], "true") &&
                         pdc_stricmp(items[iss], "false")))
                    {
                        if (!pdc_strincmp(keyword, "no", 2))
                        {
                            boolval = pdc_false;
                            break;
                        }
                        else
                        {
                            boolval = pdc_true;
                            break;
                        }
                    }
                }
            }

            if (!pdc_stricmp(keyword, defopt[it].name)) break;
        }

        if (logg5)
            pdc_trace(pdc, "\t\t\toption \"%s\" specified: {", keyword);

        if (it == numdef)
        {
            errcode = PDC_E_OPT_UNKNOWNKEY;
            goto PDC_OPT_SYNTAXERROR;
        }

        /* initialize */
        dopt = &defopt[it];
        ignore = pdc_false;
        nvalues = 1;
        values = NULL;
        ishandle = pdc_true;

        /* compatibility */
        if (clientdata && clientdata->compatibility)
        {
            int compatibility = clientdata->compatibility;

            for (iv = PDC_1_3; iv <= PDC_X_X_LAST; iv++)
            {
                if ((dopt->flags & (1L<<iv)) && compatibility < iv)
                {
                    stemp2 = pdc_get_pdfversion(pdc, compatibility);
                    errcode = PDC_E_OPT_VERSION;
                    goto PDC_OPT_SYNTAXERROR;
                }
            }
        }

        /* not supported */
        if (dopt->flags & PDC_OPT_UNSUPP)
        {
            ignore = pdc_true;
            if (verbose)
                pdc_warning(pdc, PDC_E_OPT_UNSUPP, dopt->name, 0, 0, 0);
        }

        /* parse values */
        if (boolval == pdc_undef)
        {
            is++;
            if (is == nitems)
            {
                errcode = PDC_E_OPT_NOVALUES;
                goto PDC_OPT_SYNTAXERROR;
            }
            if (!ignore)
            {
                if (dopt->type == pdc_stringlist &&
                    pdc_is_utf8_bytecode(items[is]))
                    resopt[it].flags |= PDC_OPT_ISUTF8;

                if (dopt->type != pdc_stringlist || dopt->maxnum > 1)
                    nvalues = pdc_split_stringlist(pdc, items[is],
                                    (dopt->flags & PDC_OPT_SUBOPTLIST) ?
                                    PDC_OPT_LISTSEPS : NULL, &values);

                if (dopt->flags & PDC_OPT_DUPORIGVAL)
                    resopt[it].origval = pdc_strdup(pdc, items[is]);
            }
        }

        /* ignore */
        if (ignore) continue;

        /* number of values check */
        if (nvalues < dopt->minnum)
        {
            stemp2 = pdc_errprintf(pdc, "%d", dopt->minnum);
            errcode = PDC_E_OPT_TOOFEWVALUES;
            goto PDC_OPT_SYNTAXERROR;
        }
        else if (nvalues > dopt->maxnum)
        {
            stemp2 = pdc_errprintf(pdc, "%d", dopt->maxnum);
            errcode = PDC_E_OPT_TOOMANYVALUES;
            goto PDC_OPT_SYNTAXERROR;
        }

        /* number of values must be even */
        if (dopt->flags & PDC_OPT_EVENNUM && (nvalues % 2))
        {
            errcode = PDC_E_OPT_ODDNUM;
            goto PDC_OPT_SYNTAXERROR;
        }

        /* number of values must be even */
        if (dopt->flags & PDC_OPT_ODDNUM && !(nvalues % 2))
        {
            errcode = PDC_E_OPT_EVENNUM;
            goto PDC_OPT_SYNTAXERROR;
        }

        /* option already exists */
        if (resopt[it].num)
        {
            pdc_delete_optvalue(pdc, &resopt[it]);
        }

        /* no values */
        if (!nvalues ) continue;

        /* maximal value */
        switch (dopt->type)
        {
            case pdc_actionhandle:
            maxval = clientdata->maxaction;
            break;

            case pdc_bookmarkhandle:
            maxval = clientdata->maxbookmark;
            break;

            case pdc_colorhandle:
            maxval = clientdata->maxcolor;
            break;

            case pdc_documenthandle:
            maxval = clientdata->maxdocument;
            break;

            case pdc_fonthandle:
            maxval = clientdata->maxfont;
            break;

            case pdc_gstatehandle:
            maxval = clientdata->maxgstate;
            break;

            case pdc_iccprofilehandle:
            maxval = clientdata->maxiccprofile;
            break;

            case pdc_imagehandle:
            maxval = clientdata->maximage;
            break;

	    case pdc_layerhandle:
            maxval = clientdata->maxlayer;
            break;

            case pdc_pagehandle:
            maxval = clientdata->maxpage;
            break;

            case pdc_patternhandle:
            maxval = clientdata->maxpattern;
            break;

            case pdc_shadinghandle:
            maxval = clientdata->maxshading;
            break;

            case pdc_templatehandle:
            maxval = clientdata->maxtemplate;
            break;

            case pdc_textflowhandle:
            maxval = clientdata->maxtextflow;
            break;

            default:
            maxval = dopt->maxval;
            ishandle = pdc_false;
            break;
        }

        /* allocate value array */
        resopt[it].val = pdc_calloc(pdc,
                            (size_t) (nvalues * pdc_typesizes[dopt->type]), fn);
        resopt[it].num = nvalues;
        resopt[it].currind = it;

        /* analyze type */
        resval = resopt[it].val;
        for (iv = 0; iv < nvalues; iv++)
        {
            errcode = 0;
            if (dopt->maxnum > 1 && nvalues)
                value = values[iv];
            else
                value = items[is];
            if (logg5)
                pdc_trace(pdc, "%s%s", iv ? " " : "", value);
            switch (dopt->type)
            {
                /* boolean list */
                case pdc_booleanlist:
                if (boolval == pdc_true || !pdc_stricmp(value, "true"))
                {
                    *(pdc_bool *) resval = pdc_true;
                }
                else if (boolval == pdc_false || !pdc_stricmp(value, "false"))
                {
                    *(pdc_bool *) resval = pdc_false;
                }
                else
                {
                    errcode = PDC_E_OPT_ILLBOOLEAN;
                }
                break;

                /* string list */
                case pdc_stringlist:
                if (dopt->flags & PDC_OPT_NOSPACES)
                {
                    if (pdc_split_stringlist(pdc, value, NULL, &strings) > 1)
                        errcode = PDC_E_OPT_ILLSPACES;
                    pdc_cleanup_stringlist(pdc, strings);
                }
                if (!errcode)
                {
                    len = strlen(value);
                    dz = (double) len;
                    if (dz < dopt->minval)
                    {
                        stemp3 = pdc_errprintf(pdc, "%d", (int) dopt->minval);
                        errcode = PDC_E_OPT_TOOSHORTSTR;
                    }
                    else if (dz > maxval)
                    {
                        stemp3 = pdc_errprintf(pdc, "%d", (int) maxval);
                        errcode = PDC_E_OPT_TOOLONGSTR;
                    }
                    *((char **) resval) = pdc_strdup(pdc, value);
                }
                break;

                /* keyword list */
                case pdc_keywordlist:
                if (dopt->flags & PDC_OPT_CASESENS)
                    iz = pdc_get_keycode(value, dopt->keylist);
                else
                    iz = pdc_get_keycode_ci(value, dopt->keylist);
                if (iz == PDC_KEY_NOTFOUND)
                {
                    errcode = PDC_E_OPT_ILLKEYWORD;
                }
                else
                {
                    *(int *) resval = iz;
                }
                break;

                /* Character list */
                case pdc_unicharlist:
                usz = pdc_string2unicode(pdc, value, dopt->flags, dopt->keylist,
                                         pdc_false);
                if (usz == PDC_UNICODE_NOTCHAR)
                {
                    errcode = PDC_E_OPT_ILLCHAR;
                    break;
                }
                dz = usz;
                if (dz < dopt->minval)
                {
                    stemp3 = pdc_errprintf(pdc, "%g", dopt->minval);
                    errcode = PDC_E_OPT_TOOSMALLVAL;
                }
                else if (dz > maxval)
                {
                    stemp3 = pdc_errprintf(pdc, "%g", maxval);
                    errcode = PDC_E_OPT_TOOBIGVAL;
                }
                *(pdc_ushort *) resval = usz;
                break;

                /* number list */
                case pdc_integerlist:
                case pdc_floatlist:
                case pdc_doublelist:
                case pdc_scalarlist:

                if (dopt->keylist)
                {
                    /* optional keyword and/or allowed integer list */
                    if (dopt->flags & PDC_OPT_CASESENS)
                        iz = pdc_get_keycode(value, dopt->keylist);
                    else
                        iz = pdc_get_keycode_ci(value, dopt->keylist);
                    if (iz == PDC_KEY_NOTFOUND)
                    {
                        if (dopt->flags & PDC_OPT_INTLIST)
                        {
                            errcode = PDC_E_OPT_ILLINTEGER;
                            break;
                        }
                    }
                    else
                    {
                        switch (dopt->type)
                        {
                            default:
                            case pdc_integerlist:
                            *(int *) resval = iz;
                            break;

                            case pdc_floatlist:
                            *(float *) resval = (float) iz;
                            break;

                            case pdc_doublelist:
                            *(double *) resval = (double) iz;
                            break;

                            case pdc_scalarlist:
                            *(pdc_scalar *) resval = (pdc_scalar) iz;
                            break;
                        }
                        break;
                    }
                }

                /* percentage */
                if (dopt->flags & PDC_OPT_PERCENT)
                {
                    i = (int) strlen(value) - 1;
                    if (value[i] == '%')
                    {
                        value[i] = 0;
                        if (iv < 32)
                            resopt[it].pcmask |= (1L << iv);
                    }
                    else
                    {
                        resopt[it].pcmask &= ~(1L << iv);
                    }
                }

                case pdc_actionhandle:
                case pdc_bookmarkhandle:
                case pdc_colorhandle:
                case pdc_documenthandle:
                case pdc_fonthandle:
                case pdc_gstatehandle:
                case pdc_iccprofilehandle:
                case pdc_imagehandle:
		case pdc_layerhandle:
                case pdc_pagehandle:
                case pdc_patternhandle:
                case pdc_shadinghandle:
                case pdc_templatehandle:
                case pdc_textflowhandle:

                if (dopt->type == pdc_floatlist ||
                    dopt->type == pdc_doublelist ||
                    dopt->type == pdc_scalarlist)
                {
                    retval = pdc_str2double(value, &dz);
                }
                else
                {
                    if (dopt->minval >= 0)
                    {
                        retval = pdc_str2integer(value, PDC_INT_UNSIGNED, &ulz);
                        if (ishandle && hastobepos &&
                            dopt->type != pdc_bookmarkhandle)
                            ulz -= 1;
                        dz = ulz;
                    }
                    else
                    {
                        retval = pdc_str2integer(value, 0, &lz);
                        dz = lz;
                    }
                }
                if (!retval)
                {
                    errcode = PDC_E_OPT_ILLNUMBER;
                }
                else
                {
                    if (resopt[it].pcmask & (1L << iv))
                        dz /= 100.0;

                    if (dz < dopt->minval)
                    {
                        if (ishandle)
                        {
                            stemp3 = pdc_get_keyword(dopt->type,
                                                     pdc_handletypes);
                            errcode = PDC_E_OPT_ILLHANDLE;
                        }
                        else
                        {
                            stemp3 = pdc_errprintf(pdc, "%g", dopt->minval);
                            errcode = PDC_E_OPT_TOOSMALLVAL;
                        }
                    }
                    else if (dz > maxval)
                    {
                        if (ishandle)
                        {
                            stemp3 = pdc_get_keyword(dopt->type,
                                                     pdc_handletypes);
                            errcode = PDC_E_OPT_ILLHANDLE;
                        }
                        else
                        {
                            stemp3 = pdc_errprintf(pdc, "%g", maxval);
                            errcode = PDC_E_OPT_TOOBIGVAL;
                        }
                    }
                    else if (dopt->flags & PDC_OPT_NOZERO &&
                             fabs(dz) < PDC_FLOAT_PREC)
                    {
                        errcode = PDC_E_OPT_ZEROVAL;
                    }
                    else if (dopt->type == pdc_scalarlist)
                    {
                        *(pdc_scalar *) resval = dz;
                    }
                    else if (dopt->type == pdc_doublelist)
                    {
                        *(double *) resval = dz;
                    }
                    else if (dopt->type == pdc_floatlist)
                    {
                        *(float *) resval = (float) dz;
                    }
                    else
                    {
                        if (dopt->minval >= 0)
                            *(pdc_uint32 *) resval = ulz;
                        else
                            *(pdc_sint32 *) resval = lz;
                    }
                }
                break;
            }

            if (errcode)
            {
                stemp2 = pdc_errprintf(pdc, "%.*s", PDC_ET_MAXSTRLEN, value);
                goto PDC_OPT_SYNTAXERROR;
            }

            /* increment value pointer */
            resval = (void *) ((char *)(resval) + pdc_typesizes[dopt->type]);
        }
        pdc_cleanup_stringlist(pdc, values);
        values = NULL;

        if (logg5)
            pdc_trace(pdc, "}\n");

        /* build OR bit pattern */
        if (dopt->flags & PDC_OPT_BUILDOR && nvalues > 1)
        {
            int *bcode = (int *) resopt[it].val;
            for (iv = 1; iv < nvalues; iv++)
            {
                bcode[0] |= bcode[iv];
            }
            resopt[it].num = 1;
        }
    }
    pdc_cleanup_stringlist(pdc, items);
    items = NULL;

    /* required and to be ignored options */
    for (is = 0; tocheck && is < numdef; is++)
    {
        /* to be ignored option */
        if (resopt[is].num)
        {
            nd = 0;
            if (defopt[is].flags & PDC_OPT_IGNOREIF1) nd = 1;
            if (defopt[is].flags & PDC_OPT_IGNOREIF2) nd = 2;
            for (it = is - 1; it >= is - nd && it >= 0; it--)
            {
                if (resopt[it].num)
                {
                    pdc_delete_optvalue(pdc, &resopt[is]);
                    if (verbose)
                        pdc_warning(pdc, PDC_E_OPT_IGNORE, defopt[is].name,
                                    defopt[it].name, 0, 0);
                }
            }
        }

        /* required option */
        if (!resopt[is].num &&
            ((defopt[is].flags & PDC_OPT_REQUIRED) ||
             (defopt[is].flags & PDC_OPT_REQUIRIF1 && resopt[is-1].num) ||
             (defopt[is].flags & PDC_OPT_REQUIRIF2 &&
              (resopt[is-1].num || resopt[is-2].num))))
        {
            keyword = (char *) defopt[is].name;
            errcode = PDC_E_OPT_NOTFOUND;
            goto PDC_OPT_SYNTAXERROR;
        }
    }

    /* is no sorted */
    if (!issorted)
    {
        qsort((void *)resopt, (size_t) numdef, sizeof(pdc_resopt),
              pdc_optname_compare);
    }

    /* global UTF-8 check after sort */
    if (optlist && pdc_is_utf8_bytecode(optlist))
        resopt[0].isutf8 = pdc_true;

    /* index of last got option */
    resopt[0].lastind = -1;

    /* protocol */
    if (pdc_trace_protocol_is_enabled(pdc, 1, trc_optlist))
    {
        for (is = 0; is < numdef; is++)
        {
            if (resopt[is].num)
                pdc_trace(pdc, "\tOption \"%s\": %d value%s found\n",
                          resopt[is].defopt->name, resopt[is].num,
                          resopt[is].num == 1 ? "" : "s");
            else if (logg5)
                pdc_trace(pdc, "\t\t\toption \"%s\" not specified\n",
                          resopt[is].defopt->name);
            for (iv = 0; iv < resopt[is].num; iv++)
            {
                switch (resopt[is].defopt->type)
                {
                    case pdc_booleanlist:
                    case pdc_keywordlist:
                    case pdc_integerlist:
                    case pdc_actionhandle:
                    case pdc_bookmarkhandle:
                    case pdc_colorhandle:
                    case pdc_documenthandle:
                    case pdc_fonthandle:
                    case pdc_gstatehandle:
                    case pdc_iccprofilehandle:
                    case pdc_imagehandle:
                    case pdc_layerhandle:
                    case pdc_pagehandle:
                    case pdc_patternhandle:
                    case pdc_shadinghandle:
                    case pdc_templatehandle:
                    case pdc_textflowhandle:
                    pdc_trace(pdc, "\tValue %d: %d\n",
                              iv + 1, *((int *) resopt[is].val + iv));
                    break;

                    case pdc_stringlist:
                    pdc_trace(pdc, "\tValue %d: \"%s\"\n",
                              iv + 1, *((char **) resopt[is].val + iv));
                    break;

                    case pdc_floatlist:
                    pdc_trace(pdc, "\tValue %d: %f\n",
                              iv + 1, *((float *) resopt[is].val + iv));
                    break;

                    case pdc_doublelist:
                    pdc_trace(pdc, "\tValue %d: %f\n",
                              iv + 1, *((double *) resopt[is].val + iv));
                    break;

                    case pdc_scalarlist:
                    pdc_trace(pdc, "\tValue %d: %f\n",
                              iv + 1, *((pdc_scalar *) resopt[is].val + iv));
                    break;

                    case pdc_unicharlist:
                    pdc_trace(pdc, "\tValue %d: %d\n",
                              iv + 1, *((pdc_ushort *) resopt[is].val + iv));
                    break;
                }
            }
        }
    }

    return resopt;

    PDC_OPT_SYNTAXERROR:
    stemp1 = pdc_errprintf(pdc, "%.*s", PDC_ET_MAXSTRLEN, keyword);
    pdc_cleanup_stringlist(pdc, items);
    pdc_cleanup_stringlist(pdc, values);

    pdc_set_errmsg(pdc, errcode, stemp1, stemp2, stemp3, 0);
    if (verbose)
        pdc_error(pdc, -1, 0, 0, 0, 0);

    return NULL;
}

int
pdc_get_optvalues(const char *keyword, pdc_resopt *resopt,
                  void *lvalues, char ***mvalues)
{
    pdc_resopt *ropt = NULL;
    void *values = NULL;
    int nvalues = 0;
    size_t nbytes;
    if (mvalues) *mvalues = NULL;

    if (resopt)
    {
        int i, cmp;
        int lo = 0;
        int hi = resopt[0].numdef;

        while (lo < hi)
        {
            i = (lo + hi) / 2;
            cmp = strcmp(keyword, resopt[i].defopt->name);

            /* keyword found */
            if (cmp == 0)
            {
                ropt = &resopt[i];
                nvalues = ropt->num;
                values = ropt->val;
                resopt[0].lastind = i;
                break;
            }

            if (cmp < 0)
                hi = i;
            else
                lo = i + 1;
        }
    }

    if (nvalues)
    {
        /* copy values */
        if (lvalues)
        {
            if (ropt->defopt->type == pdc_stringlist &&
                ropt->defopt->maxnum == 1)
            {
                strcpy((char *)lvalues, *((char **) values));
            }
            else
            {
                nbytes = (size_t) (nvalues * pdc_typesizes[ropt->defopt->type]);
                memcpy(lvalues, values, nbytes);
            }
        }

        /* copy pointer */
        if (mvalues)
        {
            *mvalues = (char **) values;
        }
    }

    return nvalues;
}

void *
pdc_save_lastopt(pdc_resopt *resopt, int flags)
{
    int i = resopt[0].lastind;

    if (i > -1 && resopt[i].num)
    {
        if (!flags || (flags & PDC_OPT_SAVEALL))
        {
            resopt[i].flags |= PDC_OPT_SAVEALL;
            return resopt[i].val;
        }
        else if (resopt[i].defopt->type == pdc_stringlist &&
                 (flags & PDC_OPT_SAVE1ELEM))
        {
            char **s = (char **) resopt[i].val;
            resopt[i].flags |= PDC_OPT_SAVE1ELEM;
            return (void *) s[0];
        }
        else if (flags & PDC_OPT_SAVEORIG)
        {
            resopt[i].flags |= PDC_OPT_SAVEORIG;
            return (void *) resopt[i].origval;
        }
    }

    return NULL;
}

int
pdc_get_lastopt_index(pdc_resopt *resopt)
{
    int i = resopt[0].lastind;

    if (i > -1)
        return resopt[i].currind;
    else
        return -1;
}

pdc_bool
pdc_is_lastopt_percent(pdc_resopt *resopt, int ind)
{
    int i = resopt[0].lastind;

    if (i > -1 && resopt[i].num < 32)
        return (resopt[i].pcmask & (1L << ind)) ? pdc_true : pdc_false;
    else
        return pdc_false;
}

pdc_bool
pdc_is_lastopt_utf8(pdc_resopt *resopt)
{
    int i = resopt[0].lastind;

    if (i > -1)
        return resopt[0].isutf8 ||
               ((resopt[i].flags & PDC_OPT_ISUTF8) ? pdc_true : pdc_false);
    else
        return pdc_false;
}

void
pdc_cleanup_optionlist(pdc_core *pdc, pdc_resopt *resopt)
{
    pdc_free_tmp(pdc, resopt);
}

const char *
pdc_get_handletype(pdc_opttype type)
{
    return pdc_get_keyword(type, pdc_handletypes);
}

/*
 * Substituting a list of variables in a string by its values recursively.
 * A variable begins with the character '&' and ends before a white
 * space, the characters  = { } &  or end of string.
 * The character & is masked by backslash.
 *
 * If at least one of a variable was substituted, a new allocated null
 * terminated string is returned. Otherwise the original pointer.
 *
 * The caller is responsible for freeing the new string.
 *
 *   string     null terminated string with variables
 *   varslist   list of variable names
 *   valslist   list of variable values
 *   nvars      number of variables
 *   errind[2]  contains index and length of an unkown variable in string
 *
 */

static char *
substitute_variables(pdc_core *pdc, char *string, int ibeg, int *level,
    const char **varslist, const char **valslist, int nvars, char vchar,
    const char *separstr, int *errind)
{
    static const char fn[] = "pdc_substitue_variables";
    int i;

    for (i = ibeg; string[i] != 0; i++)
    {
        if (string[i] == vchar)
        {
            if (!i || string[i-1] != '\\')
                break;
        }
    }

    if (string[i] != 0)
    {
        int j;
        char *s = &string[i+1];
        size_t n = strcspn(s, separstr);

        for (j = 0; j < nvars; j++)
        {
            if (n == strlen(varslist[j]) && !strncmp(s, varslist[j], n))
            {
                char *newstring;
                int k = (int) (i + n + 1);
                size_t nv = strlen(valslist[j]);
                size_t nr = strlen(&string[k]);
                size_t nb = (size_t) i +  nv + nr + 1;

                newstring = (char *) pdc_malloc(pdc, nb, fn);
                strncpy(newstring, string, (size_t) i);
                strncpy(&newstring[i], valslist[j], nv);
                strcpy(&newstring[i + nv], &string[k]);

                if (*level)
                    pdc_free(pdc, string);
                (*level)++;

                string = substitute_variables(pdc, newstring, i, level,
                               varslist, valslist, nvars, vchar, separstr,
                               errind);
                break;
            }
        }
        if (j == nvars)
        {
            errind[0] = i;
            errind[1] = (int) (n + 1);
        }
    }

    return string;
}

char *
pdc_substitute_variables(pdc_core *pdc, const char *string,
    const char **varslist, const char **valslist, int nvars, int *errind)
{
    int level = 0;
    char vchar = '&';
    char separstr[16];

    separstr[0] = vchar;
    separstr[1] = 0;
    strcat(separstr, PDC_OPT_LISTSEPS);
    strcat(separstr, "{}");

    errind[0] = -1;
    errind[1] = 0;
    return substitute_variables(pdc, (char *) string, 0, &level,
                         varslist, valslist, nvars, vchar, separstr, errind);
}



