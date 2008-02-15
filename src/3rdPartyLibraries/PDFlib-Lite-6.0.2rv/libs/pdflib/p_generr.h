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

/* $Id: p_generr.h,v 1.1 2005/10/11 17:18:14 vuduc2 Exp $
 *
 * PDFlib error messages
 *
 */

#define P_GENERR_H

#if	pdf_genNames
#define gen(n, num, nam, msg)	PDF_E_##nam = num,
#elif	pdf_genInfo
#define gen(n, num, nam, msg)	{ n, num, msg, (const char *) 0 },

#else
#error	invalid inclusion of generator file
#endif


/* -------------------------------------------------------------------- */
/* Configuration 					(20xx)		*/
/* -------------------------------------------------------------------- */

gen(0, 2000, UNSUPP_CRYPT, "Encryption not supported in PDFlib Lite")

gen(0, 2002, UNSUPP_KERNING, "Kerning not supported in PDFlib Lite")

gen(0, 2004, UNSUPP_SUBSET, "Subsetting not supported in PDFlib Lite")

gen(0, 2006, UNSUPP_PDFX, "PDF/X not supported in PDFlib Lite")

gen(1, 2008, UNSUPP_IMAGE, "$1 images not supported in this configuration")

gen(0, 2010, UNSUPP_ICC, "ICC profiles not supported in PDFlib Lite")

gen(0, 2012, UNSUPP_UNICODE,
    "Unicode and glyph id addressing not supported in PDFlib Lite")

gen(0, 2014, UNSUPP_SPOTCOLOR, "Spot colors not supported in PDFlib Lite")

gen(0, 2016, UNSUPP_PDI, "PDF import (PDI) not supported in PDFlib Lite")

gen(0, 2018, UNSUPP_BLOCK,
    "Personalization with blocks not supported in PDFlib Lite")

gen(0, 2020, UNSUPP_FORMFIELDS, "Form fields not supported in PDFlib Lite")

gen(0, 2021, UNSUPP_JAVASCRIPT, "JavaScript not supported in PDFlib Lite")

gen(0, 2022, UNSUPP_MC, "Marked content not supported in PDFlib Lite")

gen(0, 2024, UNSUPP_TAGGED, "Tagged PDF not supported in PDFlib Lite")

gen(0, 2026, UNSUPP_LAYER,
    "Optional content (layers) not supported in PDFlib Lite")

gen(0, 2028, UNSUPP_TEXTFLOWS, "Textflow not supported in PDFlib Lite")

gen(0, 2030, UNSUPP_CHARREF,
    "Character references not supported in PDFlib Lite")

gen(0, 2032, UNSUPP_JPEG2000,
    "JPEG2000 images not supported in PDFlib Lite")

gen(0, 2098, BETA_EXPIRED,
    "PDFlib 6 beta expired -- get latest version at www.pdflib.com")


/* -------------------------------------------------------------------- */
/* Document, page, scoping and resource                 (21xx)          */
/* -------------------------------------------------------------------- */

gen(1, 2100, DOC_SCOPE,	"Function must not be called in '$1' scope")

gen(1, 2102, DOC_FUNCUNSUPP, "Function not supported for '$1'")

gen(2, 2104, DOC_PDFVERSION, "$1 not supported in PDF $2")

gen(0, 2106, DOC_EMPTY,	"Generated document doesn't contain any pages")

gen(0, 2110, PAGE_SIZE_ACRO, "Page size incompatible with Acrobat")

gen(2, 2112, PAGE_BADBOX, "Illegal $1 [$2]")

gen(0, 2114, PAGE_ILLCHGSIZE,
    "Page size cannot be changed in top-down coordinate system")

gen(2, 2120, RES_BADRES, "Bad resource specification '$1' for category '$2'")

gen(2, 2122, DOC_SCOPE_GET, "Can't get parameter '$1' in '$2' scope")

gen(2, 2124, DOC_SCOPE_SET, "Can't set parameter '$1' in '$2' scope")

gen(1, 2126, PAGE_NOSUSPEND, "Page number $1 has not been suspended")

gen(2, 2128, PAGE_NOSUSPEND2,
    "Page number $1 in group '$2' has not been suspended")

gen(1, 2130, PAGE_ILLNUMBER, "Illegal page number $1")

gen(1, 2132, PAGE_NOTEXIST, "Page number $1 does not exist")

gen(2, 2134, PAGE_NOTEXIST2, "Page number $1 in group '$2' does not exist")

gen(0, 2136, PAGE_NEEDGROUP, "No page group specified")

gen(0, 2138, PAGE_NEEDGROUP2,
    "No page group specified (use PDF_begin_page_ext)")

gen(1, 2140, DOC_UNKNOWNGROUP, "Unknown page group '$1'")

gen(1, 2142, DOC_GROUPMISSING,
    "Page group '$1' is missing in list of option 'grouporder'")

gen(0, 2144, DOC_OPTGROUPORDER,
    "Option 'grouporder' is illegal (no page groups are specified)")

gen(1, 2146, DOC_DUPLGROUP,
    "Duplicate definition of group '$1' in option 'grouporder'")

gen(1, 2148, DOC_ILL_LABELOPT,
    "Label option '$1' is illegal for this function")

gen(1, 2150, DOC_NEED_LABELOPT,
    "Option 'labels' requires suboption '$1' if used with this function")

gen(1, 2152, PAGE_TRANS_COMPAT,
    "Page transition '$1' requires PDF output version 1.5 or higher")

gen(1, 2154, PAGE_ILLROTATE, "Option 'rotate' has illegal value $1")

gen(0, 2156, PAGE_SUSPEND_TAGGED,
    "This function must not be used in Tagged PDF mode")

gen(0, 2158, PAGE_SEP_NOSPOT,
    "Option 'separationinfo' requires 'spotname' or 'spotcolor'")

gen(0, 2160, PAGE_SEP_ILLPAGES,
    "Option 'separationinfo' must not use 'pages' if not first page in group")

gen(0, 2162, PAGE_SEP_NOPAGES, "Option 'separationinfo' requires 'pages'")

gen(0, 2164, PAGE_SEP_NOINFO, "Option 'separationinfo' missing")

gen(0, 2166, DOC_SEP_INCOMPLETE, "Incomplete separation group")

gen(0, 2168, PAGE_TOPDOWN_NODIMS,
    "Must specify page dimensions with option 'topdown'")

gen(0, 2170, PAGE_NODIMS, "No dimensions specified for this page")

gen(0, 2172, DOC_GETBUF_2GB,
    "Can't process buffers larger than 2GB on this platform")

gen(1, 2174, PAGE_SUSPENDED, "Page number $1 is still suspended")


/* -------------------------------------------------------------------- */
/* Graphics and Text					(22xx)		*/
/* -------------------------------------------------------------------- */

gen(0, 2200, GSTATE_UNMATCHEDSAVE, "Unmatched save level")

gen(0, 2202, GSTATE_RESTORE, "Invalid restore (no matching save level)")

gen(1, 2204, GSTATE_SAVELEVEL, "Too many save levels (max. $1)")

/* Currently unused */
/*
gen(0, 2210, PATTERN_SELF, "Can't use a pattern within its own definition")
*/

gen(0, 2212, SHADING13, "Smooth shadings are not supported in PDF 1.3")

gen(1, 2220, TEMPLATE_SELF,
    "Can't place template handle $1 within its own definition")

gen(1, 2230, TEXT_UNICODENOTSHOW,
    "Can't show character with Unicode value U+$1")

gen(1, 2232, TEXT_GLYPHIDNOTSHOW, "Can't show character with glyph id $1")

gen(1, 2233, TEXT_BUILTINNOTSHOW,
    "Can't show 16-bit character $1 for builtin encoding")

gen(1, 2234, TEXT_TOOLONG, "Text too long (max. $1)")

gen(2, 2235, TEXT_SIZENOMATCH,
    "Size ($1) of glyphwidths list doesn't match size ($2 characters) of text")

gen(0, 2236, TEXT_TOOMANYCODES, "Too many different unicode values (> 256)")

gen(0, 2238, TEXT_NOFONT, "No font set for text")

gen(1, 2240, TEXT_NOFONT_PAR, "No font set for parameter '$1'")






/* -------------------------------------------------------------------- */
/* Color						(23xx)		*/
/* -------------------------------------------------------------------- */

gen(0, 2300, COLOR_SPOT,
"Spot color can not be based on a Pattern, Indexed, or Separation color space")

gen(2, 2302, COLOR_BADSPOT, "Color name '$1' not found in $2 table")

gen(0, 2304, COLOR_SPOTBW, "Alternate color for spot color is black or white")

gen(1, 2306, COLOR_UNLIC_SPOTCOLOR, "$1 spot colors not licensed")

gen(0, 2308, COLOR_UNSUPP_SPOTNAME, "Unicode spot color names not supported")



/* -------------------------------------------------------------------- */
/* Image						(24xx)		*/
/* -------------------------------------------------------------------- */

gen(2, 2400, IMAGE_CORRUPT, "Corrupt $1 image file '$2'")

gen(3, 2402, IMAGE_NOPAGE, "Requested page $1 in $2 image '$3' not found")

gen(2, 2404, IMAGE_BADDEPTH,
    "Bad number of bits per pixel ($1) in image file '$2'")

gen(1, 2406, IMAGE_BADMASK,
    "Image '$1' not suitable as mask (more than one color component)")

gen(2, 2407, IMAGE_NOMATCH,
    "Image '$1' not suitable as mask for image '$2' (different orientation)")

gen(1, 2408, IMAGE_MASK1BIT13,
    "Image '$1' with more than 1 bit not supported as mask in PDF 1.3")

gen(1, 2410, IMAGE_COLORMAP, "Couldn't read colormap in image '$1'")

gen(2, 2412, IMAGE_BADCOMP,
    "Bad number of color components ($1) in image '$2'")

gen(1, 2414, IMAGE_COLORIZE,
    "Can't colorize image '$1' with more than 1 component")

gen(1, 2416, IMAGE_ICC, "Couldn't handle embedded ICC profile in image '$1'")

gen(1, 2418, IMAGE_ICC2,
    "ICC profile for image file '$1' doesn't match image data")

gen(0, 2420, IMAGE_THUMB, "More than one thumbnail for this page")

gen(1, 2422, IMAGE_THUMB_MULTISTRIP,
    "Can't use multi-strip image $1 as thumbnail")

gen(1, 2424, IMAGE_THUMB_CS,
    "Unsupported color space in thumbnail image handle $1")

gen(2, 2426, IMAGE_THUMB_SIZE, "Thumbnail image $1 larger than $2 pixels")

gen(2, 2428, IMAGE_OPTUNSUPP,
    "Option '$1' for image type '$2' not supported (ignored)")

gen(2, 2430, IMAGE_OPTUNREAS,
    "Option '$1' for image type '$2' doesn't have any effect (ignored)")

gen(2, 2432, IMAGE_OPTBADMASK, "Option '$1' has bad image mask $2")

gen(1, 2434, IMAGE_UNKNOWN, "Unknown image type in file '$1'")

gen(0, 2436, IMAGE_NOADJUST,
    "Option 'adjustpage' must not be used in top-down system")

gen(1, 2437, IMAGE_OPI_ILLRECT, "Option '$1' has bad rectangle")

gen(2, 2438, IMAGE_OPI_ILLMAPSIZE, "Option '$1': Number of values must be $2")

gen(1, 2439, IMAGE_OPI_ILLPARALL, "Option '$1' has bad parallelogram")

gen(2, 2440, RAW_ILLSIZE,
    "Size ($1 bytes) of raw image file '$2' doesn't match specified options")

gen(2, 2442, IMAGE_TYPUNSUPP, "Image type '%s' is not supported in PDF %s")

gen(1, 2444, BMP_VERSUNSUPP,
    "Version of BMP image file '$1' not supported")

gen(1, 2446, BMP_COMPUNSUPP,
    "Compression in BMP image file '$1' not supported")

gen(2, 2450, JPEG_COMPRESSION,
    "JPEG compression scheme '$1' in file '$2' not supported in PDF")

/* UNUSED
gen(1, 2452, JPEG_MULTISCAN,
    "JPEG file '$1' contains multiple scans, which is not supported in PDF")
*/

gen(1, 2454, JPEG_TRANSCODE,
    "Problems during JPEG transcoding in file '$1'")

/* UNUSED
gen(1, 2460, GIF_LZWOVERFLOW, "LZW code size overflow in GIF file '$1'")

gen(1, 2462, GIF_LZWSIZE,
    "Color palette in GIF file '$1' with fewer than 128 colors not supported")

gen(1, 2464, GIF_INTERLACED, "Interlaced GIF image '$1' not supported")

gen(2, 2470, TIFF_UNSUPP_CS,
    "Couldn't open TIFF image '$1' (unsupported color space; photometric $2)")

gen(2, 2472, TIFF_UNSUPP_PREDICT,
    "Couldn't open TIFF image '$1' (unsupported predictor tag $2)")

gen(1, 2474, TIFF_UNSUPP_LZW, "Couldn't open LZW-compressed TIFF image '$1')")

gen(1, 2476, TIFF_UNSUPP_LZW_PLANES,
    "Couldn't open TIFF image '$1' (separate planes with LZW compression)")

gen(1, 2478, TIFF_UNSUPP_LZW_ALPHA,
    "Couldn't open TIFF image '$1' (alpha channel with LZW compression)")

gen(2, 2480, TIFF_UNSUPP_JPEG,
    "Couldn't open TIFF image '$1' (JPEG compression scheme $2)")

gen(1, 2482, TIFF_UNSUPP_JPEG_TILED,
    "Couldn't open TIFF image '$1' (tiled image with JPEG compression)")
*/

gen(1, 2484, TIFF_UNSUPP_JPEG_SEPARATE,
    "Couldn't open TIFF image '$1' (JPEG with separate image planes)")

gen(2, 2486, TIFF_UNSUPP_SEP_NONCMYK,
    "Couldn't open TIFF image '$1' (unsupported inkset tag $2)")

gen(1, 2488, TIFF_MASK_MULTISTRIP, "Can't mask multistrip TIFF image '$1'")

/* Unused
gen(1, 2490, TIFF_MULTISTRIP_MASK,
    "Can't use multistrip TIFF image '$1' as mask")
*/

gen(1, 2492, TIFF_CMYK_MASK, "Couldn't open TIFF image '$1' (CMYK with mask)")

gen(1, 2494, JPX_FORMATUNSUPP,
    "JPEG2000 flavor in image file '$1' not supported")

gen(1, 2496, JPX_RAWDATAUNSUPP,
    "Raw JPEG2000 code stream in image file '$1' not supported in PDF")

gen(1, 2498, JPX_COMPOUNDUNSUPP,
    "Compound JPEG2000 (JPM) image file '$1' not supported")


/* -------------------------------------------------------------------- */
/* Font							(25xx)		*/
/* -------------------------------------------------------------------- */

gen(2, 2500, FONT_CORRUPT, "Corrupt $1 font file '$2'")

gen(2, 2502, FONT_BADENC, "Font '$1' doesn't support '$2' encoding")

gen(3, 2504, FONT_FORCEENC, "Use '$1' encoding instead of '$2' for font '$3'")

gen(2, 2505, FONT_NEEDUCS2,
    "Font '$2' requires Unicode-compatible CMap instead of '$1'")

gen(2, 2506, FONT_FORCEEMBED, "Encoding '$1' for font '$2' requires embedding")

/* UNUSED
gen(1, 2508, FONT_BADTEXTFORM,
    "Current text format not allowed for builtin encoding")
*/

gen(1, 2510, FONT_HOSTNOTFOUND, "Host font '$1' not found")

gen(1, 2512, FONT_TTHOSTNOTFOUND, "TrueType host font '$1' not found")

gen(1, 2514, FONT_EMBEDMM, "Multiple Master font '$1' cannot be embedded")

gen(1, 2516, FONT_NOMETRICS, "Metrics data for font '$1' not found")

gen(1, 2518, FONT_NOOUTLINE,
    "No file specified with outline data for font '$1'")

gen(1, 2520, FONT_NOGLYPHID, "Font '$1' does not contain glyph IDs")

/* UNUSED
gen(1, 2530, CJK_NOSTANDARD, "Predefined CMap not supported for font '$1'")
*/

gen(2, 2531, CJK_UNSUPP_CHARCOLL,
    "CJK font '$1' doesn't support predefined CMap '$2'")

gen(1, 2532, FONT_EMBEDCMAP,
    "Standard CJK font with predefined CMap '$1' cannot be embedded")

gen(2, 2533, FONT_ONLY_CMAP,
     "CJK font '$1' doesn't support '$2' encoding "
     "(must use standard CMap or Unicode encoding)")

gen(0, 2534, FONT_EMBEDSTYLE,
    "'fontstyle' specification not allowed for embedded fonts")

gen(2, 2536, FONT_ILLFONTSTYLE, "Illegal fontstyle '$1' in fontname '$2'")

gen(1, 2538, FONT_UNSUPP_FORMAT, "Font format '$1' not supported")

gen(1, 2540, T3_BADBBOX,
    "Bounding box values must be 0 for colorized Type 3 font '$1'")

gen(2, 2542, T3_GLYPH, "Glyph '$1' already defined in Type 3 font '$2'")

gen(1, 2544, T3_FONTEXISTS, "Font '$1' already exists")

gen(3, 2550, T1_BADCHARSET,
    "Encoding (dfCharSet $1) in font/PFM file '$2' not supported")

gen(1, 2551, T1_UNSUPP_FORMAT, "'$1' metric files not supported")

gen(2, 2554, T1_AFMBADKEY,
    "Unknown key '$1' in AFM file '$2'")

gen(1, 2558, T1_NOFONT, "'$1' is not a PostScript Type 1 font")

gen(1, 2560, TT_BITMAP, "TrueType bitmap font '$1' not supported")

gen(1, 2562, TT_NOFONT, "Font '$1' is not a TrueType or OpenType font")

gen(1, 2564, TT_BADCMAP, "Font '$1' contains unknown encodings (cmaps) only")

gen(1, 2566, TT_SYMBOLOS2, "Symbol font '$1' does not contain OS/2 table")

gen(1, 2568, TT_EMBED,
"Couldn't embed font '$1' due to licensing restrictions in the font file")

gen(0, 2570, TT_ASSERT1, "TrueType parser error")

gen(1, 2572, TT_ASSERT2, "TrueType parser error in font '$1'")

gen(2, 2574, TTC_NOTFOUND,
    "Font '$1' not found in TrueType Collection file '$2'")

gen(1, 2576, TT_NOGLYFDESC,
    "TrueType font '$1' does not contain any character outlines")

gen(1, 2578, TT_NONAME,
    "TrueType font '$1' contains only unsupported records in 'name' table")

gen(2, 2580, OT_CHARSET,
"OpenType font '$1' with predefined charset '$2' in CFF table not supported")

gen(1, 2581, OT_MULTIFONT,
    "OpenType font '$1' with multiple font entries not supported")

gen(1, 2582, OT_CHARSTRINGS,
    "OpenType font '$1' has no CharStrings data in CFF table")

gen(1, 2583, OT_TOPDICT,
    "OpenType CID font '$1' has no Top DICT INDEX in CFF table")

gen(2, 2584, TT_GLYPHIDNOTFOUND,
    "Couldn't find glyph id for Unicode value U+$1 in TrueType font '$2'")

gen(1, 2585, OT_NO_ORDERING,
    "OpenType CID font '$1' has no ordering string in CFF table")

gen(2, 2586, OT_ILL_CHARCOLL,
    "OpenType CID font '$1' has unknown character collection '$2'")


/* -------------------------------------------------------------------- */
/* Encoding						(26xx)		*/
/* -------------------------------------------------------------------- */

gen(1, 2600, ENC_NOTFOUND, "Couldn't find encoding '$1'")

gen(1, 2602, ENC_UNSUPP, "Code page '$1' not supported")

gen(1, 2606, ENC_CANTQUERY, "Can't query encoding '$1'")

gen(1, 2608, ENC_CANTCHANGE, "Can't change encoding '$1'")

gen(1, 2610, ENC_INUSE,
    "Encoding '$1' can't be changed since it has already been used")

gen(2, 2612, ENC_TOOLONG, "Encoding name '$1' too long (max. $2)")

gen(2, 2614, ENC_BADLINE, "Syntax error in encoding file '$1' (line '$2')")

gen(0, 2616, ENC_GLYPHORCODE, "Glyph name or Unicode value required")

gen(3, 2618, ENC_BADGLYPH,
    "Glyph name '$1' for Unicode value U+$2 differs from AGL name '$3'")

gen(3, 2620, ENC_BADUNICODE,
    "Unicode value U+$1 for glyph name '$2' differs from AGL value U+$3")

gen(2, 2622, ENC_BADFONT,
    "Current font $1 wasn't specified with encoding '$2'")

gen(1, 2640, ENC_BADHYPTEXTENC, "Bad hypertext encoding '$1'")

gen(1, 2650, ENC_UNSUPPENCFORMAT,
    "Parameter or option '$1' not supported in Unicode-capable languages")




/* -------------------------------------------------------------------- */
/* Hypertext, form fields, actions, annotations		(28xx)		*/
/* -------------------------------------------------------------------- */

gen(2, 2802, HYP_OPTIGNORE_FORTYPE,
    "Option '$1' for destination type '$2' doesn't have any effect (ignored)")

gen(1, 2804, HYP_OPTIGNORE_FORELEM,
    "Option '$1' for hypertext function will be ignored")

gen(2, 2820, FF_OPTEFFLESS_FORTYPE,
    "Option '$1' for field type '$2' doesn't have any effect (ignored)")

gen(1, 2822, FF_GROUPMISSING,
    "Required field group missing for radio button field '$1'")

gen(1, 2824, FF_FONTMISSING, "Font not specified for field '$1'")

gen(1, 2826, FF_TEXTMISSING,
    "Option 'itemtextlist' not specified for field '$1'")

gen(1, 2828, FF_CIDFONT,
    "Specified font '$1' not allowed for fields (encoding not supported)")

gen(1, 2830, FF_NOEMBEDFONT,
    "Specified font '$1' not allowed for fields (must be embedded)")

gen(1, 2832, FF_SUBSETTFONT,
    "Specified font '$1' not allowed for fields (must not be subset)")

gen(1, 2834, FF_CAPTMISSING, "No caption or icon specified for field '$1'")

gen(0, 2836, FF_DIFFSTRLISTS,
    "Options 'itemnamelist' and 'itemtextlist' contain "
    "different numbers of strings")

gen(2, 2838, FF_INVALINDEX, "Option '$1' has invalid list index '$2'")

gen(2, 2840, FF_NOTFOUND,
    "Illegal field pathname '$1' (name '$2' not found)")

gen(2, 2842, FF_NAMEEXISTS,
    "Illegal field pathname '$1' (name '$2' already exists)")

gen(2, 2844, FF_NOTGROUP,
    "Illegal field pathname '$1' ('$2' is not a field group)")

gen(3, 2846, FF_TYPENOTMATCH,
    "Type '$1' of field '$2' doesn't match type '$3' of group")

gen(0, 2848, FF_ITEMNAMEORNOT,
"Either all or none of the buttons/checkboxes in a group can have item names")

gen(2, 2850, FF_OPTEFFONLY,
    "Option '$1' for field type '$2' only has an effect for highlight=push")

gen(2, 2852, FF_ILLUNINAME,
    "Illegal field name '$1' (Unicode names are not supported in PDF $2")

gen(0, 2854, FF_DEMOLIMIT,
    "No more than 10 fields can be created with the evaluation version")

gen(0, 2856, FF_RICHTEXT,
    "fontsize 0 not supported for fields with rich text")

gen(2, 2860, ACT_OPTIGNORE_FORTYPE,
    "Option '$1' for action type '$2' doesn't have any effect")

gen(2, 2862, ACT_BADACTTYPE, "Action type '$1' for event '$2' not allowed")

gen(2, 2880, ANN_OPTEFFLESS_FORTYPE,
    "Option '$1' for annotation type '$2' doesn't have any effect")

gen(1, 2882, ANN_NOSTDFONT,
    "Font '$1' not allowed for annotations (not a core or standard CJK font)")

gen(1, 2884, ANN_BADNUMCOORD, "Option '$1' has bad number of coordinates")

gen(1, 2886, ANN_OPTALRDEF,
    "Option '$1' already defined in option 'custom'")

gen(1, 2888, ANN_ILLCUSTOMKEY,
    "Option 'custom' uses illegal key '$1' (already defined in PDF)")


/* -------------------------------------------------------------------- */
/* Internal 						(29xx)		*/
/* -------------------------------------------------------------------- */

gen(1, 2900, INT_BADSCOPE, "Bad scope '$1'")

gen(1, 2902, INT_BADANNOT, "Bad annotation type '$1'")

gen(1, 2904, INT_BADCS, "Unknown color space $1")

gen(1, 2906, INT_BADALTERNATE, "Bad alternate color space $1")

gen(1, 2908, INT_BADPROFILE, "Unknown number of profile components ($1)")

gen(1, 2910, INT_SSTACK_OVER, "State stack overflow in function '$1'")

gen(1, 2912, INT_SSTACK_UNDER, "State stack underflow in function '$1'")

gen(3, 2914, INT_WRAPPER, "Error in PDFlib $1 wrapper function $2 ($3)")

gen(1, 2990, OT_UNSUPP_SID2CID,
    "OpenType font '$1' with accented characters not supported; "
    "contact support@pdflib.com")




#undef	gen
#undef	pdf_genNames
#undef	pdf_genInfo

