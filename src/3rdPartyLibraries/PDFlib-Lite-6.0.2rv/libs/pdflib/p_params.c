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

/* $Id: p_params.c,v 1.1 2005/10/11 17:18:15 vuduc2 Exp $
 *
 * PDFlib parameter handling
 *
 */

#define P_PARAMS_C

#include "p_intern.h"
#include "p_color.h"
#include "p_font.h"
#include "p_image.h"
#include "p_page.h"
#include "p_tagged.h"

/*
 * PDF_get_parameter() and PDF_set_parameter() deal with strings,
 * PDF_get_value() and PDF_set_value() deal with numerical values.
 */

typedef struct
{
    char *	name;		/* parameter name */
    pdc_bool	mod_zero;	/* PDF_get_() modifier argument must be 0 */
    int		get_scope;	/* bit mask of legal states for PDF_get_() */
    int		set_scope;	/* bit mask of legal states for PDF_set_() */
} pdf_parm_descr;

static pdf_parm_descr parms[] =
{
#define pdf_gen_parm_descr	1
#include "p_params.h"
#undef	pdf_gen_parm_descr

    { "", 0, 0, 0 }
};

enum
{
#define pdf_gen_parm_enum	1
#include "p_params.h"
#undef	pdf_gen_parm_enum

    PDF_PARAMETER_LIMIT
};

static int
pdf_get_index(PDF *p, const char *key)
{
    int i;

    if (key == NULL || !*key)
        pdc_error(p->pdc, PDC_E_ILLARG_EMPTY, "key", 0, 0, 0);

    for (i = 0; i < PDF_PARAMETER_LIMIT; ++i)
    {
        if (pdc_stricmp(key, parms[i].name) == 0)
        {
            if ((p->state_stack[p->state_sp] & parms[i].set_scope) == 0)
                pdc_error(p->pdc, PDF_E_DOC_SCOPE_SET, key,
                          pdf_current_scope(p), 0, 0);
	    return i;
        }
    }

    if (i == PDF_PARAMETER_LIMIT)
        pdc_error(p->pdc, PDC_E_PAR_UNKNOWNKEY, key, 0, 0, 0);

    return -1;
}

static pdc_bool
pdf_bool_value(PDF *p, const char *key, const char *value)
{
    if (!pdc_stricmp(value, "true"))
	return pdc_true;

    if (!pdc_stricmp(value, "false"))
	return pdc_false;

    pdc_error(p->pdc, PDC_E_ILLARG_BOOL, key, value, 0, 0);

    return pdc_false;		/* compilers love it */
}

void
pdf__set_parameter(PDF *p, const char *key, const char *value)
{
    pdc_usebox usebox = use_none;
    pdc_text_format textformat = pdc_auto;
    char optlist[64];
    pdf_ppt *ppt;
    int i, k;

    i = pdf_get_index(p, key);

    if (value == NULL) value = "";

    ppt = p->curr_ppt;

    switch (i)
    {
        case PDF_PARAMETER_PDIUSEBOX:
        case PDF_PARAMETER_VIEWAREA:
        case PDF_PARAMETER_VIEWCLIP:
        case PDF_PARAMETER_PRINTAREA:
        case PDF_PARAMETER_PRINTCLIP:
            k = pdc_get_keycode_ci(value, pdf_usebox_keylist);
            if (k == PDC_KEY_NOTFOUND)
                pdc_error(p->pdc, PDC_E_PAR_ILLPARAM, value, key, 0, 0);
            usebox = (pdc_usebox) k;
            strcpy(optlist, key);
            strcat(optlist, " ");
            strcat(optlist, value);
            break;

        case PDF_PARAMETER_TEXTFORMAT:
        case PDF_PARAMETER_HYPERTEXTFORMAT:
            k = pdc_get_keycode_ci(value, pdf_textformat_keylist);
            if (k == PDC_KEY_NOTFOUND)
                pdc_error(p->pdc, PDC_E_PAR_ILLPARAM, value, key, 0, 0);
            textformat = (pdc_text_format) k;
            break;
    }

    switch (i)
    {
        case PDF_PARAMETER_SEARCHPATH:
        case PDF_PARAMETER_FONTAFM:
	case PDF_PARAMETER_FONTPFM:
        case PDF_PARAMETER_FONTOUTLINE:
        case PDF_PARAMETER_HOSTFONT:
        case PDF_PARAMETER_ENCODING:
        case PDF_PARAMETER_ICCPROFILE:
        case PDF_PARAMETER_STANDARDOUTPUTINTENT:
	{
            pdf_add_resource(p, key, value);
            break;
        }

	case PDF_PARAMETER_DEBUG:
	{
	    const unsigned char *c;

	    for (c = (const unsigned char *) value; *c; c++) {
		p->debug[(int) *c] = 1;

		if (*c == 't') {
                    pdf_set_trace_options(p, "enable");
		}
	    }
	    break;
	}

	case PDF_PARAMETER_NODEBUG:
	{
	    const unsigned char *c;

	    for (c = (const unsigned char *) value; *c; c++) {
		if (*c == 't')
                    pdf_set_trace_options(p, "disable");

		p->debug[(int) *c] = 0;
	    }
	    break;
	}

        case PDF_PARAMETER_BINDING:
            if (!p->binding)
                p->binding = pdc_strdup(p->pdc, value);
            break;

        case PDF_PARAMETER_OBJORIENT:
            p->objorient = pdf_bool_value(p, key, value);
            break;

        case PDF_PARAMETER_HASTOBEPOS:
            p->hastobepos = pdf_bool_value(p, key, value);
            break;

        case PDF_PARAMETER_PTFRUN:
            p->ptfrun = pdf_bool_value(p, key, value);
            break;

	case PDF_PARAMETER_UNDERLINE:
            pdf_set_tstate(p, (double) pdf_bool_value(p, key, value),
                           to_underline);
	    break;

	case PDF_PARAMETER_OVERLINE:
            pdf_set_tstate(p, (double) pdf_bool_value(p, key, value),
                           to_overline);
	    break;

	case PDF_PARAMETER_STRIKEOUT:
            pdf_set_tstate(p, (double) pdf_bool_value(p, key, value),
                           to_strikeout);
	    break;

        case PDF_PARAMETER_KERNING:
            pdc_warning(p->pdc, PDF_E_UNSUPP_KERNING, 0, 0, 0, 0);
            break;

        case PDF_PARAMETER_AUTOSUBSETTING:
        case PDF_PARAMETER_AUTOCIDFONT:
        case PDF_PARAMETER_UNICODEMAP:
            pdc_warning(p->pdc, PDF_E_UNSUPP_UNICODE, 0, 0, 0, 0);
            break;


        case PDF_PARAMETER_RESOURCEFILE:
            if (p->resourcefilename)
            {
                pdc_free(p->pdc, p->resourcefilename);
                p->resourcefilename = NULL;
            }
            p->resourcefilename = pdc_strdup(p->pdc, value);
            p->resfilepending = pdc_true;
            break;

        case PDF_PARAMETER_WARNING:
            pdc_set_warnings(p->pdc, pdf_bool_value(p, key, value));
            break;

        case PDF_PARAMETER_OPENWARNING:
            p->debug[(int) 'o'] = (char) pdf_bool_value(p, key, value);
            break;

        case PDF_PARAMETER_FONTWARNING:
            p->debug[(int) 'F'] = (char) pdf_bool_value(p, key, value);
            break;

        case PDF_PARAMETER_ICCWARNING:
            p->debug[(int) 'I'] = (char) pdf_bool_value(p, key, value);
            break;

        case PDF_PARAMETER_IMAGEWARNING:
            p->debug[(int) 'i'] = (char) pdf_bool_value(p, key, value);
            break;

        case PDF_PARAMETER_PDIWARNING:
            p->debug[(int) 'p'] = (char) pdf_bool_value(p, key, value);
            break;

        case PDF_PARAMETER_HONORICCPROFILE:
            p->debug[(int) 'e'] = (char) pdf_bool_value(p, key, value);
            break;

        case PDF_PARAMETER_GLYPHWARNING:
            p->debug[(int) 'g'] = (char) pdf_bool_value(p, key, value);
            if (p->curr_ppt)
                pdf_set_tstate(p, (double) pdf_bool_value(p, key, value),
                               to_glyphwarning);
            break;

        case PDF_PARAMETER_PASSTHROUGH:
            p->debug[(int) 'P'] = (char) !pdf_bool_value(p, key, value);
            break;

        case PDF_PARAMETER_RENDERINGINTENT:
            k = pdc_get_keycode_ci(value, pdf_renderingintent_pdfkeylist);
            if (k == PDC_KEY_NOTFOUND)
                pdc_error(p->pdc, PDC_E_PAR_ILLPARAM, value, key, 0, 0);
            p->rendintent = (pdf_renderingintent) k;
            break;

        case PDF_PARAMETER_PRESERVEOLDPANTONENAMES:
            p->preserveoldpantonenames = pdf_bool_value(p, key, value);
            break;

        case PDF_PARAMETER_SPOTCOLORLOOKUP:
            p->spotcolorlookup = pdf_bool_value(p, key, value);
            break;

        case PDF_PARAMETER_PDISTRICT:
            p->pdi_strict = pdf_bool_value(p, key, value);
            break;

        case PDF_PARAMETER_PDIUSEBOX:
            p->pdi_usebox = usebox;
            break;

        case PDF_PARAMETER_TOPDOWN:
            if (pdf_bool_value(p, key, value))
                p->ydirection = -1.0;
            else
                p->ydirection = 1.0;
            break;

        case PDF_PARAMETER_USERCOORDINATES:
            p->usercoordinates = pdf_bool_value(p, key, value);
            break;

        case PDF_PARAMETER_TEXTFORMAT:
            pdf_check_textformat(p, textformat);
            p->textformat = textformat;
            if (p->curr_ppt)
                pdf_set_tstate(p, (double) textformat, to_textformat);
            break;

        case PDF_PARAMETER_HYPERTEXTFORMAT:
            pdf_check_hypertextformat(p, textformat);
            p->hypertextformat = textformat;
            break;

        case PDF_PARAMETER_HYPERTEXTENCODING:
        {
            p->hypertextencoding =
                pdf_get_hypertextencoding(p, value, &p->hypertextcodepage,
                                          pdc_true);
            pdf_check_hypertextencoding(p, p->hypertextencoding);
            break;
        }

        case PDF_PARAMETER_CHARREF:
            pdc_warning(p->pdc, PDF_E_UNSUPP_CHARREF, 0, 0, 0, 0);
            break;

        case PDF_PARAMETER_FILLRULE:
            if (!pdc_stricmp(value, "winding")) {
                ppt->fillrule = pdf_fill_winding;
            } else if (!pdc_stricmp(value, "evenodd")) {
                ppt->fillrule = pdf_fill_evenodd;
            } else {
                pdc_error(p->pdc, PDC_E_PAR_ILLPARAM, value, key, 0, 0);
            }
            break;

        case PDF_PARAMETER_LOGGING:
            pdf_set_trace_options(p, value);
            break;

        /* deprecated */
        case PDF_PARAMETER_TRACE:
        {
            pdc_bool bv = pdf_bool_value(p, key, value);
            if (bv)
                pdf_set_trace_options(p, "");
            else
                pdf_set_trace_options(p, "disable");
            break;
        }

        /* deprecated */
        case PDF_PARAMETER_TRACEFILE:
            pdc_set_tracefile(p->pdc, value);
            break;

        case PDF_PARAMETER_LOGMSG:
        case PDF_PARAMETER_TRACEMSG:
            /* do nothing -- client-supplied string will show up
             * in the log file
             */
            break;

        case PDF_PARAMETER_SERIAL:
        case PDF_PARAMETER_LICENCE:
        case PDF_PARAMETER_LICENSE:
            break;

        case PDF_PARAMETER_LICENCEFILE:
        case PDF_PARAMETER_LICENSEFILE:
            break;

        case PDF_PARAMETER_AUTOSPACE:
            pdc_warning(p->pdc, PDF_E_UNSUPP_TAGGED, 0, 0, 0, 0);
            break;


/*****************************************************************************/
/**                   deprecated historical parameters                      **/
/*****************************************************************************/

	case PDF_PARAMETER_MASTERPASSWORD:
	    pdc_warning(p->pdc, PDF_E_UNSUPP_CRYPT, 0, 0, 0, 0);
            break;

	case PDF_PARAMETER_USERPASSWORD:
            pdc_warning(p->pdc, PDF_E_UNSUPP_CRYPT, 0, 0, 0, 0);
            break;

	case PDF_PARAMETER_PERMISSIONS:
            pdc_warning(p->pdc, PDF_E_UNSUPP_CRYPT, 0, 0, 0, 0);
            break;

	case PDF_PARAMETER_COMPATIBILITY:
            pdf_set_compatibility(p, value);
	    break;

        case PDF_PARAMETER_FLUSH:
            pdf_set_flush(p, value);
            break;

	case PDF_PARAMETER_PDFX:
	    pdc_warning(p->pdc, PDF_E_UNSUPP_PDFX, 0, 0, 0, 0);
	    break;

        case PDF_PARAMETER_PREFIX:
            if (p->prefix)
            {
                pdc_free(p->pdc, p->prefix);
                p->prefix = NULL;
            }
            /* because of downward compatibility */
            p->prefix = pdc_strdup(p->pdc, &value[value[0] == '/' ? 1 : 0]);
            break;

	case PDF_PARAMETER_HIDETOOLBAR:
        case PDF_PARAMETER_HIDEMENUBAR:
        case PDF_PARAMETER_HIDEWINDOWUI:
        case PDF_PARAMETER_FITWINDOW:
        case PDF_PARAMETER_CENTERWINDOW:
        case PDF_PARAMETER_DISPLAYDOCTITLE:
	    if (pdf_bool_value(p, key, value))
                pdf_set_viewerpreference(p, key);
	    break;

	case PDF_PARAMETER_NONFULLSCREENPAGEMODE:
            if (!pdc_stricmp(value, "useoutlines"))
                pdf_set_viewerpreference(p, "nonfullscreenpagemode bookmarks");
            else if (!pdc_stricmp(value, "usethumbs"))
                pdf_set_viewerpreference(p, "nonfullscreenpagemode thumbnails");
            else if (!pdc_stricmp(value, "usenone"))
                pdf_set_viewerpreference(p, "nonfullscreenpagemode none");
	    else
		pdc_error(p->pdc, PDC_E_PAR_ILLPARAM, value, key, 0, 0);
	    break;

	case PDF_PARAMETER_DIRECTION:
            if (!pdc_stricmp(value, "r2l"))
                pdf_set_viewerpreference(p, "direction r2l");
            else if (!pdc_stricmp(value, "l2r"))
                pdf_set_viewerpreference(p, "direction l2r");
	    else
		pdc_error(p->pdc, PDC_E_PAR_ILLPARAM, value, key, 0, 0);
	    break;

	case PDF_PARAMETER_VIEWAREA:
        case PDF_PARAMETER_VIEWCLIP:
        case PDF_PARAMETER_PRINTAREA:
        case PDF_PARAMETER_PRINTCLIP:
            pdf_set_viewerpreference(p, optlist);
	    break;

	case PDF_PARAMETER_OPENACTION:
            pdf_set_openaction(p, value);
	    break;

	case PDF_PARAMETER_OPENMODE:
            pdf_set_openmode(p, value);
	    break;

	case PDF_PARAMETER_BOOKMARKDEST:
	    pdf_cleanup_destination(p, p->bookmark_dest);
            p->bookmark_dest =
                pdf_parse_destination_optlist(p, value, 0, pdf_bookmark,
                                              pdc_true);
	    break;

	case PDF_PARAMETER_NATIVEUNICODE:
        case PDF_PARAMETER_INHERITGSTATE:
	    (void) pdf_bool_value(p, key, value);
	    break;

	case PDF_PARAMETER_TRANSITION:
	    pdf_set_transition(p, value);
	    break;

	case PDF_PARAMETER_BASE:
            pdf_set_uri(p, value);
	    break;

	case PDF_PARAMETER_LAUNCHLINK_PARAMETERS:
	    if (p->launchlink_parameters) {
		pdc_free(p->pdc, p->launchlink_parameters);
		p->launchlink_parameters = NULL;
	    }
	    p->launchlink_parameters = pdc_strdup(p->pdc, value);
	    break;

	case PDF_PARAMETER_LAUNCHLINK_OPERATION:
	    if (p->launchlink_operation) {
		pdc_free(p->pdc, p->launchlink_operation);
		p->launchlink_operation = NULL;
	    }
	    p->launchlink_operation = pdc_strdup(p->pdc, value);
	    break;

	case PDF_PARAMETER_LAUNCHLINK_DEFAULTDIR:
	    if (p->launchlink_defaultdir) {
		pdc_free(p->pdc, p->launchlink_defaultdir);
		p->launchlink_defaultdir = NULL;
	    }
	    p->launchlink_defaultdir = pdc_strdup(p->pdc, value);
	    break;

	default:
	    pdc_error(p->pdc, PDC_E_PAR_UNKNOWNKEY, key, 0, 0, 0);
	    break;
    } /* switch */
} /* pdf__set_parameter */

static double
pdf_value(PDF *p, const char *key, double value, int minver)
{
    if (p->compatibility < minver)
	pdc_error(p->pdc, PDC_E_PAR_VERSION,
            key, pdc_get_pdfversion(p->pdc, minver), 0, 0);

    return value;
}

static double
pdf_pos_value(PDF *p, const char *key, double value, int minver)
{
    if (p->compatibility < minver)
	pdc_error(p->pdc, PDC_E_PAR_VERSION,
            key, pdc_get_pdfversion(p->pdc, minver), 0, 0);

    if (value <= 0)
	pdc_error(p->pdc, PDC_E_PAR_ILLVALUE,
	    pdc_errprintf(p->pdc, "%f", value), key, 0, 0);

    return value;
}

void
pdf__set_value(PDF *p, const char *key, double value)
{
    int i;
    int ivalue = (int) value;
    pdf_ppt *ppt;

    i = pdf_get_index(p, key);

    ppt = p->curr_ppt;

    switch (i)
    {
        case PDF_PARAMETER_COMPRESS:
	    if (ivalue < 0 || ivalue > 9)
		pdc_error(p->pdc, PDC_E_PAR_ILLVALUE,
		    pdc_errprintf(p->pdc, "%f", value), key, 0, 0);

	    if (pdc_get_compresslevel(p->out) != ivalue)
	    {
		/*
		 * We must restart the compression engine and start a new
		 * contents section if we're in the middle of a page.
		 */
		if (PDF_GET_STATE(p) == pdf_state_page) {
		    pdf_end_contents_section(p);
		    pdc_set_compresslevel(p->out, ivalue);
		    pdf_begin_contents_section(p);
		} else
		    pdc_set_compresslevel(p->out, ivalue);
	    }

	    break;

	case PDF_PARAMETER_FLOATDIGITS:
	    if (3 <= ivalue && ivalue <= 6)
		pdc_set_floatdigits(p->pdc, ivalue);
	    else
		pdc_error(p->pdc, PDC_E_PAR_ILLVALUE,
		    pdc_errprintf(p->pdc, "%d", ivalue), key, 0, 0);
	    break;

	/* TODO (york): take /CropBox into account?
	*/
	case PDF_PARAMETER_PAGEWIDTH:
	{
	    const pdc_rectangle *box = pdf_get_pagebox(p, pdf_mediabox);

            if (p->ydirection == -1)
                pdc_error(p->pdc, PDF_E_PAGE_ILLCHGSIZE, 0, 0, 0, 0);

	    if (value < PDF_ACRO_MINPAGE || value > PDF_ACRO_MAXPAGE)
		pdc_warning(p->pdc, PDF_E_PAGE_SIZE_ACRO, 0, 0, 0, 0);

	    pdf_set_pagebox_urx(p, pdf_mediabox,
		box->llx + pdf_pos_value(p, key, value, PDC_1_3));
	    break;
	}

	/* TODO (york): take /CropBox into account?
	*/
	case PDF_PARAMETER_PAGEHEIGHT:
	{
	    const pdc_rectangle *box = pdf_get_pagebox(p, pdf_mediabox);

            if (p->ydirection == -1)
                pdc_error(p->pdc, PDF_E_PAGE_ILLCHGSIZE, 0, 0, 0, 0);

	    if (value < PDF_ACRO_MINPAGE || value > PDF_ACRO_MAXPAGE)
		pdc_warning(p->pdc, PDF_E_PAGE_SIZE_ACRO, 0, 0, 0, 0);

	    pdf_set_pagebox_ury(p, pdf_mediabox,
		box->lly + pdf_pos_value(p, key, value, PDC_1_3));
	    break;
	}

	case PDF_PARAMETER_CROPBOX_LLX:
	    pdf_set_pagebox_llx(p, pdf_cropbox,
				    pdf_value(p, key, value, PDC_1_3));
	    break;

	case PDF_PARAMETER_CROPBOX_LLY:
	    pdf_set_pagebox_lly(p, pdf_cropbox,
				    pdf_value(p, key, value, PDC_1_3));
	    break;

	case PDF_PARAMETER_CROPBOX_URX:
	    pdf_set_pagebox_urx(p, pdf_cropbox,
				    pdf_value(p, key, value, PDC_1_3));
	    break;

	case PDF_PARAMETER_CROPBOX_URY:
	    pdf_set_pagebox_ury(p, pdf_cropbox,
				    pdf_value(p, key, value, PDC_1_3));
	    break;

	case PDF_PARAMETER_BLEEDBOX_LLX:
	    pdf_set_pagebox_llx(p, pdf_bleedbox,
				    pdf_value(p, key, value, PDC_1_3));
	    break;

	case PDF_PARAMETER_BLEEDBOX_LLY:
	    pdf_set_pagebox_lly(p, pdf_bleedbox,
				    pdf_value(p, key, value, PDC_1_3));
	    break;

	case PDF_PARAMETER_BLEEDBOX_URX:
	    pdf_set_pagebox_urx(p, pdf_bleedbox,
				    pdf_value(p, key, value, PDC_1_3));
	    break;

	case PDF_PARAMETER_BLEEDBOX_URY:
	    pdf_set_pagebox_ury(p, pdf_bleedbox,
				    pdf_value(p, key, value, PDC_1_3));
	    break;

	case PDF_PARAMETER_TRIMBOX_LLX:
	    pdf_set_pagebox_llx(p, pdf_trimbox,
				    pdf_value(p, key, value, PDC_1_3));
	    break;

	case PDF_PARAMETER_TRIMBOX_LLY:
	    pdf_set_pagebox_lly(p, pdf_trimbox,
				    pdf_value(p, key, value, PDC_1_3));
	    break;

	case PDF_PARAMETER_TRIMBOX_URX:
	    pdf_set_pagebox_urx(p, pdf_trimbox,
				    pdf_value(p, key, value, PDC_1_3));
	    break;

	case PDF_PARAMETER_TRIMBOX_URY:
	    pdf_set_pagebox_ury(p, pdf_trimbox,
				    pdf_value(p, key, value, PDC_1_3));
	    break;

	case PDF_PARAMETER_ARTBOX_LLX:
	    pdf_set_pagebox_llx(p, pdf_artbox,
				    pdf_value(p, key, value, PDC_1_3));
	    break;

	case PDF_PARAMETER_ARTBOX_LLY:
	    pdf_set_pagebox_lly(p, pdf_artbox,
				    pdf_value(p, key, value, PDC_1_3));
	    break;

	case PDF_PARAMETER_ARTBOX_URX:
	    pdf_set_pagebox_urx(p, pdf_artbox,
				    pdf_value(p, key, value, PDC_1_3));
	    break;

	case PDF_PARAMETER_ARTBOX_URY:
	    pdf_set_pagebox_ury(p, pdf_artbox,
				    pdf_value(p, key, value, PDC_1_3));
	    break;

	case PDF_PARAMETER_LEADING:
            pdf_set_tstate(p, value, to_leading);
	    break;

	case PDF_PARAMETER_TEXTRISE:
            pdf_set_tstate(p, value, to_textrise);
	    break;

	case PDF_PARAMETER_HORIZSCALING:
            pdf_set_tstate(p, value /100, to_horizscaling);
	    break;

        case PDF_PARAMETER_ITALICANGLE:
            pdf_set_tstate(p, value, to_italicangle);
            break;

	case PDF_PARAMETER_TEXTRENDERING:
            pdf_set_tstate(p, value, to_textrendering);
	    break;

	case PDF_PARAMETER_CHARSPACING:
            pdf_set_tstate(p, value, to_charspacing);
	    break;

        case PDF_PARAMETER_WORDSPACING:
            pdf_set_tstate(p, value, to_wordspacing);
            break;

        case PDF_PARAMETER_UNDERLINEWIDTH:
            pdf_set_tstate(p, value, to_underlinewidth);
            break;

        case PDF_PARAMETER_UNDERLINEPOSITION:
            pdf_set_tstate(p, value, to_underlineposition);
            break;

	case PDF_PARAMETER_DURATION:
	    pdf_set_duration(p, value);
	    break;

	case PDF_PARAMETER_DEFAULTGRAY:
	    break;

	case PDF_PARAMETER_DEFAULTRGB:
	    break;

	case PDF_PARAMETER_DEFAULTCMYK:
	    break;

        case PDF_PARAMETER_SETCOLOR_ICCPROFILEGRAY:
            break;

        case PDF_PARAMETER_SETCOLOR_ICCPROFILERGB:
            break;

        case PDF_PARAMETER_SETCOLOR_ICCPROFILECMYK:
            break;

        case PDF_PARAMETER_SUBSETLIMIT:
        case PDF_PARAMETER_SUBSETMINSIZE:
        {
            pdc_warning(p->pdc, PDF_E_UNSUPP_SUBSET, 0, 0, 0, 0);
            break;
        }

	default:
	    pdc_error(p->pdc, PDC_E_PAR_UNKNOWNKEY, key, 0, 0, 0);
	    break;
    } /* switch */
} /* pdf__set_value */

double
pdf__get_value(PDF *p, const char *key, double mod)
{
    int i = -1;
    int imod = (int) mod;
    double result = 0;
    pdf_ppt *ppt;

    i = pdf_get_index(p, key);

    if (parms[i].mod_zero && mod != 0)
	pdc_error(p->pdc, PDC_E_PAR_ILLVALUE,
	    pdc_errprintf(p->pdc, "%f", mod), key, 0, 0);

    ppt = p->curr_ppt;

    switch (i)
    {
        case PDF_PARAMETER_IMAGEWIDTH:
        case PDF_PARAMETER_IMAGEHEIGHT:
        case PDF_PARAMETER_RESX:
        case PDF_PARAMETER_RESY:
        case PDF_PARAMETER_ORIENTATION:
            if (p->hastobepos) imod -= 1;
            pdf_check_handle(p, imod, pdc_imagehandle);
            break;

        case PDF_PARAMETER_FONTMAXCODE:
        case PDF_PARAMETER_CAPHEIGHT:
        case PDF_PARAMETER_ASCENDER:
        case PDF_PARAMETER_DESCENDER:
            if (p->hastobepos) imod -= 1;
            pdf_check_handle(p, imod, pdc_fonthandle);
            break;
    }

    switch (i)
    {
	/* TODO (york): take /CropBox into account?
	*/
        case PDF_PARAMETER_PAGEWIDTH:
	{
	    const pdc_rectangle *box = pdf_get_pagebox(p, pdf_mediabox);

            result = box->urx - box->llx;
            break;
	}

	/* TODO (york): take /CropBox into account?
	*/
        case PDF_PARAMETER_PAGEHEIGHT:
	{
	    const pdc_rectangle *box = pdf_get_pagebox(p, pdf_mediabox);

            result = box->ury - box->lly;
            break;
	}

        case PDF_PARAMETER_IMAGEWIDTH:
            pdf_get_image_size(p, imod, (double *) &result, NULL);
            break;

        case PDF_PARAMETER_IMAGEHEIGHT:
            pdf_get_image_size(p, imod, NULL, (double *) &result);
            break;

        case PDF_PARAMETER_RESX:
            pdf_get_image_resolution(p, imod, (double *) &result, NULL);
            break;

        case PDF_PARAMETER_RESY:
            pdf_get_image_resolution(p, imod, NULL, (double *) &result);
            break;

        case PDF_PARAMETER_ORIENTATION:
            result = (double) (p->images[imod].orientation);
            break;

        case PDF_PARAMETER_IMAGE_ICCPROFILE:
            break;

        case PDF_PARAMETER_ICCCOMPONENTS:
            break;

        case PDF_PARAMETER_CURRENTX:
            result = (double) (ppt->gstate[ppt->sl].x);
	    break;

        case PDF_PARAMETER_CURRENTY:
            result = (double) (ppt->gstate[ppt->sl].y);
            break;

        case PDF_PARAMETER_CTM_A:
            result = (double) (ppt->gstate[ppt->sl].ctm.a);
            break;

        case PDF_PARAMETER_CTM_B:
            result = (double) (ppt->gstate[ppt->sl].ctm.b);
            break;

        case PDF_PARAMETER_CTM_C:
            result = (double) (ppt->gstate[ppt->sl].ctm.c);
            break;

        case PDF_PARAMETER_CTM_D:
            result = (double) (ppt->gstate[ppt->sl].ctm.d);
            break;

        case PDF_PARAMETER_CTM_E:
            result = (double) (ppt->gstate[ppt->sl].ctm.e);
            break;

        case PDF_PARAMETER_CTM_F:
            result = (double) (ppt->gstate[ppt->sl].ctm.f);
            break;

	case PDF_PARAMETER_TEXTX:
            result = pdf_get_tstate(p, to_textx);
	    break;

	case PDF_PARAMETER_TEXTY:
            result = pdf_get_tstate(p, to_texty);
	    break;

        case PDF_PARAMETER_UNDERLINEWIDTH:
            result = pdf_get_tstate(p, to_underlinewidth);
            break;

        case PDF_PARAMETER_UNDERLINEPOSITION:
            result = pdf_get_tstate(p, to_underlineposition);
            break;

        case PDF_PARAMETER_WORDSPACING:
            result = pdf_get_tstate(p, to_wordspacing);
            break;

	case PDF_PARAMETER_CHARSPACING:
            result = pdf_get_tstate(p, to_charspacing);
	    break;

	case PDF_PARAMETER_HORIZSCALING:
            result = 100 * pdf_get_tstate(p, to_horizscaling);
	    break;

        case PDF_PARAMETER_ITALICANGLE:
            result = pdf_get_tstate(p, to_italicangle);
            break;

	case PDF_PARAMETER_TEXTRISE:
            result = pdf_get_tstate(p, to_textrise);
	    break;

	case PDF_PARAMETER_LEADING:
            result = pdf_get_tstate(p, to_leading);
	    break;

	case PDF_PARAMETER_TEXTRENDERING:
            result = pdf_get_tstate(p, to_textrendering);
	    break;

        case PDF_PARAMETER_FONTSIZE:
            result = pdf_get_tstate(p, to_fontsize);
            break;

	case PDF_PARAMETER_FONT:
            result = pdf_get_tstate(p, to_font);
            if (p->hastobepos) result += 1;
	    break;

        case PDF_PARAMETER_MONOSPACE:
            result = pdf_get_font_float_option(p, fo_monospace);
            break;

        case PDF_PARAMETER_FONTMAXCODE:
            result = (double) (p->fonts[imod].numOfCodes - 1);
            break;

	case PDF_PARAMETER_CAPHEIGHT:
            result = (double) (p->fonts[imod].capHeight / 1000.0);
	    break;

	case PDF_PARAMETER_ASCENDER:
            result = (double) (p->fonts[imod].ascender / 1000.0);
	    break;

	case PDF_PARAMETER_DESCENDER:
            result = (double) (p->fonts[imod].descender / 1000.0);
	    break;

	default:
	    pdc_error(p->pdc, PDC_E_PAR_UNKNOWNKEY, key, 0, 0, 0);
	    break;
    } /* switch */

    return result;
} /* pdf__get_value */

const char *
pdf__get_parameter(PDF *p, const char *key, double mod)
{
    int i = -1;
    const char *result = "";
    pdf_ppt *ppt;

    i = pdf_get_index(p, key);

    if (parms[i].mod_zero && mod != 0)
	pdc_error(p->pdc, PDC_E_PAR_ILLPARAM,
	    pdc_errprintf(p->pdc, "%f", mod), key, 0, 0);

    ppt = p->curr_ppt;

    switch (i)
    {
	case PDF_PARAMETER_FONTNAME:
            result = pdf_get_font_char_option(p, fo_fontname);
	    break;

        case PDF_PARAMETER_FONTENCODING:
            result = pdf_get_font_char_option(p, fo_encoding);
            break;

        case PDF_PARAMETER_FONTSTYLE:
            result = pdf_get_font_char_option(p, fo_fontstyle);
            break;


        case PDF_PARAMETER_UNDERLINE:
            result = PDC_BOOLSTR((int) pdf_get_tstate(p, to_underline));
            break;

	case PDF_PARAMETER_OVERLINE:
            result = PDC_BOOLSTR((int) pdf_get_tstate(p, to_overline));
	    break;

	case PDF_PARAMETER_STRIKEOUT:
            result = PDC_BOOLSTR((int) pdf_get_tstate(p, to_strikeout));
	    break;

        /* deprecated */
	case PDF_PARAMETER_INHERITGSTATE:
            result = PDC_BOOLSTR(pdc_false);
	    break;

	case PDF_PARAMETER_SCOPE:
	    switch (p->state_stack[p->state_sp]) {
		case pdf_state_object:	result = "object";	break;
		case pdf_state_document:result = "document";	break;
		case pdf_state_page:	result = "page";	break;
		case pdf_state_pattern:	result = "pattern";	break;
		case pdf_state_template:result = "template";	break;
		case pdf_state_path:	result = "path";	break;
		default:		result = "(unknown)";	break;
	    }
	    break;

        case PDF_PARAMETER_TEXTFORMAT:
            result = pdc_get_keyword(p->textformat, pdf_textformat_keylist);
            break;

        case PDF_PARAMETER_HYPERTEXTFORMAT:
            result = pdc_get_keyword(p->hypertextformat,pdf_textformat_keylist);
            break;

        case PDF_PARAMETER_HYPERTEXTENCODING:
            result = pdf_get_encoding_name(p, p->hypertextencoding, NULL);
            break;

	default:
	    pdc_error(p->pdc, PDC_E_PAR_UNKNOWNKEY, key, 0, 0, 0);
	    break;
    } /* switch */

    return result;
} /* pdf__get_parameter */


