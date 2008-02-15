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

/* $Id: p_params.h,v 1.1 2005/10/11 17:18:15 vuduc2 Exp $
 *
 * PDFlib parameter table
 *
 */

#if	pdf_gen_parm_enum
#define pdf_gen1(code, name, zero, scope)		PDF_PARAMETER_##code,
#define pdf_gen2(code, name, zero, gscope, sscope)	PDF_PARAMETER_##code,
#elif	pdf_gen_parm_descr
#define pdf_gen1(code, name, zero, scope)		\
	{ name, zero, scope, scope },
#define pdf_gen2(code, name, zero, gscope, sscope)	\
	{ name, zero, gscope, sscope },
#else
#error	invalid inclusion of generator file
#endif

/*
   List of unsupported control characters for the "debug" parameter:
   2	disable the search for Windows 2000/XP's color directory via mscms.dll
   e	extract embedded ICC profiles from image files
   F    throw an exception if a font is unavailable (instead of returning -1);
   g    throw an exception if a glyph cannot be shown;
   h    disable host font processing
   i    throw an exception when an image is unavailable
   I    throw an exception if a ICC is unavailable (instead of returning -1);
   o    throw an exception if the PDF output file can't be opened
   P    disable pass-through mode for TIFF images
   p    throw an exception if an imported PDF is unavailable
   t	API function name and parameter logging to tracefile enabled
   T    enable TIFFlib warnings

   On by default:  e F I
*/

/*
 * Deprecated and unsupported parameters:
 * dep6		Deprecated since PDFlib 6
 * dep5		Deprecated since PDFlib 5
 * unsupp	Unsupported (internal use, dysfunctional, or other)
 */


/*
 * ----------------------------------------------------------------------
 * Setup
 * ----------------------------------------------------------------------
 */

pdf_gen1(OPENWARNING,   "openwarning",  1, pdf_state_all)	/* dep6 */
pdf_gen1(COMPRESS,	"compress",	1, pdf_state_page | pdf_state_document)
pdf_gen1(FLUSH,		"flush",	1, pdf_state_all)	/* dep6 */
pdf_gen1(RESOURCEFILE,	"resourcefile",	1, pdf_state_all)
pdf_gen1(COMPATIBILITY, "compatibility",1, pdf_state_object)	/* dep6 */
pdf_gen1(PDFX,          "pdfx",         1, pdf_state_object)	/* dep6 */
pdf_gen1(PREFIX,	"prefix",	1, pdf_state_all)	/* dep5 */
pdf_gen1(SEARCHPATH,    "SearchPath",   1, pdf_state_all)
pdf_gen1(ASCIIFILE,     "asciifile",    1, pdf_state_all)
pdf_gen1(WARNING,	"warning",	1, pdf_state_all)
pdf_gen1(LICENSE,	"license",	1, pdf_state_object)
pdf_gen1(LICENCE,	"licence",	1, pdf_state_object)	/* unsupp */
pdf_gen1(LICENSEFILE,	"licensefile",	1, pdf_state_object)
pdf_gen1(LICENCEFILE,	"licencefile",	1, pdf_state_object)	/* unsupp */
pdf_gen1(TRACE,         "trace",        1, pdf_state_all)       /* dep7 */
pdf_gen1(TRACEFILE,     "tracefile",    1, pdf_state_all)       /* dep7 */
pdf_gen1(TRACEMSG,      "tracemsg",     1, pdf_state_all)       /* dep7 */
pdf_gen1(LOGGING,       "logging",      1, pdf_state_all)
pdf_gen1(LOGMSG,        "logmsg",       1, pdf_state_all)
pdf_gen1(CHARREF,       "charref",      1, pdf_state_all)

pdf_gen1(SERIAL,	"serial",	1, pdf_state_object)	/* unsupp */
pdf_gen1(FLOATDIGITS,	"floatdigits",	1, pdf_state_all)	/* unsupp */
pdf_gen1(BINDING,       "binding",      1, pdf_state_all)	/* unsupp */
pdf_gen1(OBJORIENT,     "objorient",    1, pdf_state_all)       /* unsupp */
pdf_gen1(HASTOBEPOS,    "hastobepos",   1, pdf_state_all)       /* unsupp */
pdf_gen1(DEBUG,		"debug",	1, pdf_state_all)	/* unsupp */
pdf_gen1(NODEBUG,       "nodebug",      1, pdf_state_all)       /* unsupp */
pdf_gen1(PTFRUN,        "ptfrun",       1, pdf_state_all)       /* unsupp */


/*
 * ----------------------------------------------------------------------
 * Versioning (cf. pdflib.c)
 * ----------------------------------------------------------------------
 */

pdf_gen1(MAJOR,		"major",	1, pdf_state_all)
pdf_gen1(MINOR,		"minor",	1, pdf_state_all)
pdf_gen1(REVISION,	"revision",	1, pdf_state_all)
pdf_gen1(VERSION,	"version",	1, pdf_state_all)


/*
 * ----------------------------------------------------------------------
 * Page
 * ----------------------------------------------------------------------
 */

pdf_gen1(PAGEWIDTH,     "pagewidth",    1, pdf_state_page | pdf_state_path)
pdf_gen1(PAGEHEIGHT,    "pageheight",   1, pdf_state_page | pdf_state_path)


/* all of the following group are dep6 */

pdf_gen1(CROPBOX_LLX,   "CropBox/llx",  1, pdf_state_page | pdf_state_path)
pdf_gen1(CROPBOX_LLY,   "CropBox/lly",  1, pdf_state_page | pdf_state_path)
pdf_gen1(CROPBOX_URX,   "CropBox/urx",  1, pdf_state_page | pdf_state_path)
pdf_gen1(CROPBOX_URY,   "CropBox/ury",  1, pdf_state_page | pdf_state_path)
pdf_gen1(BLEEDBOX_LLX,  "BleedBox/llx", 1, pdf_state_page | pdf_state_path)
pdf_gen1(BLEEDBOX_LLY,  "BleedBox/lly", 1, pdf_state_page | pdf_state_path)
pdf_gen1(BLEEDBOX_URX,  "BleedBox/urx", 1, pdf_state_page | pdf_state_path)
pdf_gen1(BLEEDBOX_URY,  "BleedBox/ury", 1, pdf_state_page | pdf_state_path)
pdf_gen1(TRIMBOX_LLX,   "TrimBox/llx",  1, pdf_state_page | pdf_state_path)
pdf_gen1(TRIMBOX_LLY,   "TrimBox/lly",  1, pdf_state_page | pdf_state_path)
pdf_gen1(TRIMBOX_URX,   "TrimBox/urx",  1, pdf_state_page | pdf_state_path)
pdf_gen1(TRIMBOX_URY,   "TrimBox/ury",  1, pdf_state_page | pdf_state_path)
pdf_gen1(ARTBOX_LLX,    "ArtBox/llx",   1, pdf_state_page | pdf_state_path)
pdf_gen1(ARTBOX_LLY,    "ArtBox/lly",   1, pdf_state_page | pdf_state_path)
pdf_gen1(ARTBOX_URX,    "ArtBox/urx",   1, pdf_state_page | pdf_state_path)
pdf_gen1(ARTBOX_URY,    "ArtBox/ury",   1, pdf_state_page | pdf_state_path)


/*
 * ----------------------------------------------------------------------
 * Font
 * ----------------------------------------------------------------------
 */

pdf_gen1(FONTAFM,	"FontAFM",	1, pdf_state_all)
pdf_gen1(FONTPFM,	"FontPFM",	1, pdf_state_all)
pdf_gen1(FONTOUTLINE,	"FontOutline",	1, pdf_state_all)
pdf_gen1(HOSTFONT,      "HostFont",     1, pdf_state_all)
pdf_gen1(ENCODING,      "Encoding",     1, pdf_state_all)
pdf_gen1(FONTWARNING,	"fontwarning",	1, pdf_state_all)
pdf_gen1(SUBSETLIMIT,   "subsetlimit",  1, pdf_state_all)
pdf_gen1(SUBSETMINSIZE, "subsetminsize",1, pdf_state_all)
pdf_gen1(AUTOSUBSETTING,"autosubsetting",1, pdf_state_all)
pdf_gen1(AUTOCIDFONT,   "autocidfont",  1, pdf_state_all)
pdf_gen1(FONT,		"font",		1, pdf_state_content)
pdf_gen1(FONTSIZE,	"fontsize",	1, pdf_state_content)
pdf_gen1(FONTNAME,	"fontname",	1, pdf_state_content)
pdf_gen1(FONTSTYLE,     "fontstyle",    1, pdf_state_content)
pdf_gen1(FONTENCODING,  "fontencoding", 1, pdf_state_content)
pdf_gen1(MONOSPACE,     "monospace",    1, pdf_state_content)
pdf_gen1(FONTMAXCODE,   "fontmaxcode",  0, pdf_state_all)
pdf_gen1(CAPHEIGHT,	"capheight",	0, pdf_state_all)
pdf_gen1(ASCENDER,	"ascender",	0, pdf_state_all)
pdf_gen1(DESCENDER,	"descender",	0, pdf_state_all)
pdf_gen1(UNICODEMAP,    "unicodemap",   1, pdf_state_all)

pdf_gen1(NATIVEUNICODE,	"nativeunicode",1, pdf_state_all)	/* dep5 */


/*
 * ----------------------------------------------------------------------
 * Text
 * ----------------------------------------------------------------------
 */

pdf_gen1(TEXTX,         "textx",        1, pdf_state_content)
pdf_gen1(TEXTY,		"texty",	1, pdf_state_content)
pdf_gen1(LEADING,	"leading",	1, pdf_state_content)
pdf_gen1(TEXTRISE,	"textrise",	1, pdf_state_content)
pdf_gen1(HORIZSCALING,	"horizscaling",	1,
    pdf_state_content | pdf_state_document)
pdf_gen1(TEXTRENDERING,	"textrendering",1, pdf_state_content)
pdf_gen1(CHARSPACING,	"charspacing",	1,
    pdf_state_content | pdf_state_document)
pdf_gen1(WORDSPACING,   "wordspacing",  1,
    pdf_state_content | pdf_state_document)
pdf_gen1(ITALICANGLE,   "italicangle",  1,
    pdf_state_content | pdf_state_document)
pdf_gen1(UNDERLINEWIDTH,"underlinewidth",1,
    pdf_state_content | pdf_state_document)
pdf_gen1(UNDERLINEPOSITION,"underlineposition",1,
    pdf_state_content | pdf_state_document)
pdf_gen1(UNDERLINE,     "underline",    1, pdf_state_content)
pdf_gen1(OVERLINE,	"overline",	1, pdf_state_content)
pdf_gen1(STRIKEOUT,	"strikeout",	1, pdf_state_content)
pdf_gen1(KERNING,       "kerning",      1, pdf_state_all)
pdf_gen1(TEXTFORMAT,    "textformat",   1, pdf_state_all)
pdf_gen1(GLYPHWARNING,  "glyphwarning", 1, pdf_state_all)


/*
 * ----------------------------------------------------------------------
 * Graphics
 * ----------------------------------------------------------------------
 */

pdf_gen1(CURRENTX,	"currentx",	1, pdf_state_content | pdf_state_path)
pdf_gen1(CURRENTY,	"currenty",	1, pdf_state_content | pdf_state_path)
pdf_gen1(FILLRULE,	"fillrule",	1, pdf_state_content)
pdf_gen1(SCOPE,		"scope",        1, pdf_state_all)
pdf_gen1(TOPDOWN,       "topdown",      1, pdf_state_document)
pdf_gen1(CTM_A,         "ctm_a",        1, pdf_state_content)
pdf_gen1(CTM_B,         "ctm_b",        1, pdf_state_content)
pdf_gen1(CTM_C,         "ctm_c",        1, pdf_state_content)
pdf_gen1(CTM_D,         "ctm_d",        1, pdf_state_content)
pdf_gen1(CTM_E,         "ctm_e",        1, pdf_state_content)
pdf_gen1(CTM_F,         "ctm_f",        1, pdf_state_content)


/*
 * ----------------------------------------------------------------------
 * Color
 * ----------------------------------------------------------------------
 */

pdf_gen1(SETCOLOR_ICCPROFILEGRAY,      "setcolor:iccprofilegray",       1,
            pdf_state_document | pdf_state_content)
pdf_gen1(SETCOLOR_ICCPROFILERGB,       "setcolor:iccprofilergb",        1,
            pdf_state_document | pdf_state_content)
pdf_gen1(SETCOLOR_ICCPROFILECMYK,      "setcolor:iccprofilecmyk",       1,
            pdf_state_document | pdf_state_content)
pdf_gen1(IMAGE_ICCPROFILE,"image:iccprofile",   0,
            pdf_state_path | pdf_state_content | pdf_state_document)
pdf_gen1(ICCWARNING,      "iccwarning",     	1, pdf_state_all)
pdf_gen1(HONORICCPROFILE, "honoriccprofile",    1, pdf_state_all)
pdf_gen1(ICCCOMPONENTS,   "icccomponents",      0, pdf_state_all)
pdf_gen1(ICCPROFILE,      "ICCProfile",     	1, pdf_state_all)
pdf_gen1(STANDARDOUTPUTINTENT, "StandardOutputIntent", 1, pdf_state_all)
pdf_gen1(RENDERINGINTENT, "renderingintent",    1, pdf_state_all)

pdf_gen1(DEFAULTRGB,    "defaultrgb",   1, pdf_state_content | pdf_state_path)
pdf_gen1(DEFAULTGRAY,   "defaultgray",  1, pdf_state_content | pdf_state_path)
pdf_gen1(DEFAULTCMYK,   "defaultcmyk",  1, pdf_state_content | pdf_state_path)

pdf_gen1(PRESERVEOLDPANTONENAMES, "preserveoldpantonenames", 1, pdf_state_all)
pdf_gen1(SPOTCOLORLOOKUP,         "spotcolorlookup",         1, pdf_state_all)

/*
 * ----------------------------------------------------------------------
 * Image
 * ----------------------------------------------------------------------
 */

pdf_gen1(IMAGEWARNING,  "imagewarning", 1, pdf_state_all)
pdf_gen1(PASSTHROUGH,   "passthrough",  1, pdf_state_all)       /* dep7 */
pdf_gen1(IMAGEWIDTH,	"imagewidth",	0,
	    pdf_state_path | pdf_state_content | pdf_state_document)
pdf_gen1(IMAGEHEIGHT,	"imageheight",	0,
	    pdf_state_path | pdf_state_content | pdf_state_document)
pdf_gen1(RESX,		"resx",		0,
	    pdf_state_path | pdf_state_content | pdf_state_document)
pdf_gen1(RESY,          "resy",         0,
            pdf_state_path | pdf_state_content | pdf_state_document)
pdf_gen1(ORIENTATION,   "orientation",  0,
            pdf_state_path | pdf_state_content | pdf_state_document)

pdf_gen1(INHERITGSTATE, "inheritgstate",1, pdf_state_all)       /* dep6 */


/*
 * ----------------------------------------------------------------------
 * PDI
 * ----------------------------------------------------------------------
 */

pdf_gen1(PDI,		"pdi",		1, pdf_state_all)
pdf_gen1(PDIWARNING,	"pdiwarning",	1, pdf_state_all)
pdf_gen1(PDIUSEBOX,	"pdiusebox",	1, pdf_state_all)	/* dep6 */
pdf_gen1(PDISTRICT,	"pdistrict",	1, pdf_state_all)	/* unsupp */


/*
 * ----------------------------------------------------------------------
 * Hypertext
 * ----------------------------------------------------------------------
 */

pdf_gen1(HYPERTEXTFORMAT,   "hypertextformat",   1, pdf_state_all)
pdf_gen1(HYPERTEXTENCODING, "hypertextencoding", 1, pdf_state_all)
pdf_gen1(USERCOORDINATES,   "usercoordinates",   1, pdf_state_all)

pdf_gen1(HIDETOOLBAR,	"hidetoolbar",	1,
		pdf_state_content | pdf_state_document)		/* dep6 */
pdf_gen1(HIDEMENUBAR,	"hidemenubar",	1,
		pdf_state_content | pdf_state_document)		/* dep6 */
pdf_gen1(HIDEWINDOWUI,	"hidewindowui",	1,
		pdf_state_content | pdf_state_document)		/* dep6 */
pdf_gen1(FITWINDOW,	"fitwindow",	1,
		pdf_state_content | pdf_state_document)		/* dep6 */
pdf_gen1(CENTERWINDOW,	"centerwindow",	1,
		pdf_state_content | pdf_state_document)		/* dep6 */
pdf_gen1(DISPLAYDOCTITLE, "displaydoctitle", 1,
		pdf_state_content | pdf_state_document)		/* dep6 */
pdf_gen1(NONFULLSCREENPAGEMODE,	"nonfullscreenpagemode", 1,
		pdf_state_content | pdf_state_document)		/* dep6 */
pdf_gen1(DIRECTION,	"direction",	1,
		pdf_state_content | pdf_state_document)		/* dep6 */

pdf_gen1(VIEWAREA,	"viewarea",	1,
		pdf_state_content | pdf_state_document)		/* dep6 */
pdf_gen1(VIEWCLIP,	"viewclip",	1,
		pdf_state_content | pdf_state_document)		/* dep6 */
pdf_gen1(PRINTAREA,	"printarea",	1,
		pdf_state_content | pdf_state_document)		/* dep6 */
pdf_gen1(PRINTCLIP,	"printclip",	1,
		pdf_state_content | pdf_state_document)		/* dep6 */

pdf_gen1(OPENACTION,	"openaction",	1,
		pdf_state_content | pdf_state_document)		/* dep6 */
pdf_gen1(OPENMODE,	"openmode",	1,
		pdf_state_content | pdf_state_document)		/* dep6 */
pdf_gen1(BOOKMARKDEST,	"bookmarkdest",	1,
		pdf_state_content | pdf_state_document)		/* dep6 */
pdf_gen1(TRANSITION,	"transition",	1, pdf_state_all)	/* dep6 */
pdf_gen1(DURATION,	"duration",	1, pdf_state_all)	/* dep6 */
pdf_gen1(BASE,		"base",		1,
		pdf_state_content | pdf_state_document)		/* dep6 */

pdf_gen1(LAUNCHLINK_PARAMETERS,		"launchlink:parameters", 1,
		pdf_state_all)					/* dep6 */
pdf_gen1(LAUNCHLINK_OPERATION,		"launchlink:operation", 1,
		pdf_state_all)					/* dep6 */
pdf_gen1(LAUNCHLINK_DEFAULTDIR,		"launchlink:defaultdir", 1,
		pdf_state_all)					/* dep6 */


/*
 * ----------------------------------------------------------------------
 * Security (all dep6)
 * ----------------------------------------------------------------------
 */

pdf_gen1(USERPASSWORD,	"userpassword",	1, pdf_state_object)	/* dep6 */
pdf_gen1(MASTERPASSWORD,"masterpassword",1, pdf_state_object)	/* dep6 */
pdf_gen1(PERMISSIONS,	"permissions",	1, pdf_state_object)	/* dep6 */


/*
 * ----------------------------------------------------------------------
 * Tagged PDF
 * ----------------------------------------------------------------------
 */

pdf_gen1(AUTOSPACE, "autospace", 1, pdf_state_all)


#undef	pdf_gen1
#undef	pdf_gen2
