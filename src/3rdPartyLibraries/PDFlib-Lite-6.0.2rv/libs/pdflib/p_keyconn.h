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

/* $Id: p_keyconn.h,v 1.1 2005/10/11 17:18:14 vuduc2 Exp $
 *
 * PDFlib shared keys connection lists
 *
 */

#ifndef P_KEYCONN_H
#define P_KEYCONN_H

/*
 * global enumerations with or without key connections
 */

typedef enum
{
    pdf_state_object   = (1<<0),        /* outside any document */
    pdf_state_document = (1<<1),        /* document */
    pdf_state_page     = (1<<2),        /* page description in a document */
    pdf_state_pattern  = (1<<3),        /* pattern in a document */
    pdf_state_template = (1<<4),        /* template in a document */
    pdf_state_path     = (1<<5),        /* path in a page description */
    pdf_state_font     = (1<<6),        /* font definition */
    pdf_state_glyph    = (1<<7),        /* glyph description in a font */
    pdf_state_error    = (1<<8)         /* in error cleanup */
}
pdf_state;

typedef enum
{
    pdf_flush_none      = 0,            /* end of document */
    pdf_flush_page      = 1<<0,         /* after page */
    pdf_flush_content   = 1<<1,         /* font, xobj, annots */
    pdf_flush_reserved1 = 1<<2,         /* reserved */
    pdf_flush_reserved2 = 1<<3,         /* reserved */
    pdf_flush_heavy     = 1<<4          /* before realloc attempt */
}
pdf_flush_state;

typedef enum
{
    names_undef = 0,
    names_dests,
    names_javascript,
    names_ap
}
pdf_nametree_type;

typedef enum
{
    event_annotation,
    event_bookmark,
    event_page,
    event_document
}
pdf_event_object;

typedef enum
{
    pdf_openaction,
    pdf_bookmark,
    pdf_remotelink,
    pdf_locallink,
    pdf_nameddest
}
pdf_destuse;

typedef enum
{
    pdf_none = 0,
    pdf_fill,
    pdf_stroke,
    pdf_fillstroke
}
pdf_drawmode;

typedef enum
{
    pdf_fill_winding,
    pdf_fill_evenodd
}
pdf_fillrule;

typedef enum
{
    NoColor = -1,
    DeviceGray = 0,
    DeviceRGB,
    DeviceCMYK,
    CalGray,
    CalRGB,
    Lab,
    ICCBased,
    Indexed,
    PatternCS,
    Separation,
    DeviceN
}
pdf_colorspacetype;

typedef enum
{
    color_none = 0,
    color_gray,
    color_rgb,
    color_cmyk,
    color_spotname,
    color_spot,
    color_pattern,
    color_iccbasedgray,
    color_iccbasedrgb,
    color_iccbasedcmyk,
    color_lab
}
pdf_colortype;

typedef enum
{
    AutoIntent = 0,
    AbsoluteColorimetric,
    RelativeColorimetric,
    Saturation,
    Perceptual
}
pdf_renderingintent;

/* only up to 32 values permitted! */
typedef enum
{
    fo_autocidfont,
    fo_autosubsetting,
    fo_embedding,
    fo_encoding,
    fo_fontname,
    fo_fontstyle,
    fo_fontwarning,
    fo_kerning,
    fo_monospace,
    fo_subsetlimit,
    fo_subsetminsize,
    fo_subsetting,
    fo_unicodemap
}
pdf_font_optflags;

/* only up to 32 values permitted! */
typedef enum
{
    to_charspacing,
    to_fillcolor,
    to_font,
    to_fontsize,
    to_deffont,
    to_glyphwarning,
    to_horizscaling,
    to_italicangle,
    to_kerning,
    to_overline,
    to_strikeout,
    to_strokecolor,
    to_text,
    to_textformat,
    to_textrendering,
    to_textrise,
    to_leading,
    to_underline,
    to_wordspacing,
    to_underlinewidth,
    to_underlineposition,
    to_charref,

    to_textx,
    to_texty
}
pdf_text_optflags;

typedef enum
{
    border_solid,
    border_dashed,
    border_beveled,
    border_inset,
    border_underline
}
pdf_borderstyle;


/*
 * local enumerations with key connections
 */

#if defined(P_DOCUMENT_C)|| defined(P_PAGE_C) || defined(P_PARAMS_C)

typedef enum
{
    label_none,
    label_123,
    label_IVX,
    label_ivx,
    label_ABC,
    label_abc
}
pdf_labelstyle;

static const pdc_keyconn pdf_labelstyle_pdfkeylist[] =
{
    {"none", label_none},
    {"D",    label_123},
    {"R",    label_IVX},
    {"r",    label_ivx},
    {"A",    label_ABC},
    {"a",    label_abc},
    {NULL, 0}
};

#endif /* P_DOCUMENT_C || P_PAGE_C || P_PARAMS_C */


#if defined(P_IMAGE_C) || defined(P_PARAMS_C) || defined(P_XGSTATE_C)
static const pdc_keyconn pdf_renderingintent_pdfkeylist[] =
{
    {"Auto",                 AutoIntent},
    {"AbsoluteColorimetric", AbsoluteColorimetric},
    {"RelativeColorimetric", RelativeColorimetric},
    {"Saturation",           Saturation},
    {"Perceptual",           Perceptual},
    {NULL, 0}
};
#endif /* P_IMAGE_C || P_PARAMS_C || P_XGSTATE_C */


#if defined(P_XGSTATE_C)
typedef enum {
    BM_None             = 0,
    BM_Normal           = (1<<0),
    BM_Multiply         = (1<<1),
    BM_Screen           = (1<<2),
    BM_Overlay          = (1<<3),
    BM_Darken           = (1<<4),
    BM_Lighten          = (1<<5),
    BM_ColorDodge       = (1<<6),
    BM_ColorBurn        = (1<<7),
    BM_HardLight        = (1<<8),
    BM_SoftLight        = (1<<9),
    BM_Difference       = (1<<10),
    BM_Exclusion        = (1<<11),
    BM_Hue              = (1<<12),
    BM_Saturation       = (1<<13),
    BM_Color            = (1<<14),
    BM_Luminosity       = (1<<15)
}
pdf_blendmode;

static const pdc_keyconn pdf_blendmode_pdfkeylist[] =
{
    {"Normal",          BM_Normal},
    {"Multiply",        BM_Multiply},
    {"Screen",          BM_Screen},
    {"Overlay",         BM_Overlay},
    {"Darken",          BM_Darken},
    {"Lighten",         BM_Lighten},
    {"ColorDodge",      BM_ColorDodge},
    {"ColorBurn",       BM_ColorBurn},
    {"HardLight",       BM_HardLight},
    {"SoftLight",       BM_SoftLight},
    {"Difference",      BM_Difference},
    {"Exclusion",       BM_Exclusion},
    {"Hue",             BM_Hue},
    {"Saturation",      BM_Saturation},
    {"Color",           BM_Color},
    {"Luminosity",      BM_Luminosity},
    {NULL, 0}
};
#endif /* P_XGSTATE_C */

#if defined(P_OBJECT_C)
static const pdc_keyconn pdf_scope_keylist[] =
{
    {"object",    pdf_state_object},
    {"document",  pdf_state_document},
    {"page",      pdf_state_page},
    {"pattern",   pdf_state_pattern},
    {"template",  pdf_state_template},
    {"path",      pdf_state_path},
    {"font",      pdf_state_font},
    {"glyph",     pdf_state_glyph},
    {"error",     pdf_state_error},
    {NULL, 0}
};
#endif /* P_OBJECT_C */

#if defined(P_DOCUMENT_C) || defined(P_PARAMS_C) || defined(P_PDI_C)
static const pdc_keyconn pdf_usebox_keylist[] =
{
    {"art",   use_art},
    {"bleed", use_bleed},
    {"crop",  use_crop},
    {"media", use_media},
    {"trim",  use_trim},
    {NULL, 0}
};

static const pdc_keyconn pdf_usebox_pdfkeylist[] =
{
    {"/ArtBox",   use_art   },
    {"/BleedBox", use_bleed },
    {"/CropBox",  use_crop  },
    {"/MediaBox", use_media },
    {"/TrimBox",  use_trim  },
    {NULL, 0}
};
#endif /* P_DOCUMENT_C || P_PARAMS_C || P_PDI_C */


/* these values are used directly as indices into
** a page's boxes[] array.
*/
typedef enum
{
    pdf_artbox,
    pdf_bleedbox,
    pdf_cropbox,
    pdf_mediabox,
    pdf_trimbox
} pdf_pagebox;

#define PDF_N_PAGE_BOXES	5

#if defined(P_PAGE_C)
static const pdc_keyconn pdf_pagebox_keylist[] =
{
    {"artbox",   pdf_artbox   },
    {"bleedbox", pdf_bleedbox },
    {"cropbox",  pdf_cropbox  },
    {"mediabox", pdf_mediabox },
    {"trimbox",  pdf_trimbox  },
    {NULL, 0}
};
#endif /* P_PAGE_C */


#if defined(P_BLOCK_C) || defined(P_FONT_C) || defined(P_IMAGE_C) || \
    defined(P_TEXT_C) || defined(P_TEXTFLOW_C)

typedef enum
{
    text_left,
    text_center,
    text_right,
    text_justify,
    text_lastauto,
    text_fulljustify,
    text_decimal,
    text_top,
    text_bottom
}
pdf_alignment;

typedef enum
{
    text_nofit,
    text_clip,
    text_shrink,
    text_split,
    text_spread,
    text_auto
}
pdf_adjustmethod;

typedef enum
{
    text_relative,
    text_typewriter,
    text_ruler
}
pdf_hortabmethod;

typedef enum
{
    text_descender = -4,
    text_capheight = -3,
    text_ascender = -2,
    text_fontsize = -1,
    text_leading = 0
}
pdf_charmetric;
#endif /* P_BLOCK_C || P_FONT_C || P_IMAGE_C || P_TEXT_C || P_TEXTFLOW_C */

#if defined(P_BLOCK_C) || defined(P_TEXT_C) || defined(P_TEXTFLOW_C)
static const pdc_keyconn pdf_alignment_keylist[] =
{
    {"left",     text_left},
    {"center",   text_center},
    {"right",    text_right},
    {"justify",  text_justify},
    {"auto",     text_lastauto},
    {NULL, 0}
};

static const pdc_keyconn pdf_tabalignment_keylist[] =
{
    {"left",     text_left},
    {"center",   text_center},
    {"right",    text_right},
    {"decimal",  text_decimal},
    {NULL, 0}
};

static const pdc_keyconn pdf_adjustmethod_keylist[] =
{
    {"nofit",    text_nofit},
    {"clip",     text_clip},
    {"shrink",   text_shrink},
    {"split",    text_split},
    {"spread",   text_spread},
    {"auto",     text_auto},
    {NULL, 0}
};

static const pdc_keyconn pdf_hortabmethod_keylist[] =
{
    {"relative",   text_relative},
    {"typewriter", text_typewriter},
    {"ruler",      text_ruler},
    {NULL, 0}
};

static const pdc_keyconn pdf_verticalalign_keylist[] =
{
    {"top",       text_top},
    {"center",    text_center},
    {"bottom",    text_bottom},
    {"justify",   text_justify},
    {NULL, 0}
};

static const pdc_keyconn pdf_firstline_keylist[] =
{
    {"capheight",  text_capheight},
    {"ascender",   text_ascender},
    {"leading",    text_leading},
    {NULL, 0}
};

static const pdc_keyconn pdf_lastline_keylist[] =
{
    {"descender",  text_descender},
    {NULL, 0}
};

#define PDF_MAXLINES_AUTO PDC_USHRT_MAX
static const pdc_keyconn pdf_maxlines_keylist[] =
{
    {"auto",       PDF_MAXLINES_AUTO},
    {NULL, 0}
};

#define PDF_UNDERLINEWIDTH_AUTO 0
static const pdc_keyconn pdf_underlinewidth_keylist[] =
{
    {"auto",       PDF_UNDERLINEWIDTH_AUTO},
    {NULL, 0}
};

#define PDF_UNDERLINEPOSITION_AUTO 1000000
static const pdc_keyconn pdf_underlineposition_keylist[] =
{
    {"auto",       PDF_UNDERLINEPOSITION_AUTO},
    {NULL, 0}
};
#endif /* P_BLOCK_C || P_TEXT_C || P_TEXTFLOW_C */


#if defined(P_ANNOTS_C) || defined(P_FIELDS_C)
typedef enum
{
    text_left   = 0,
    text_center = 1,
    text_right  = 2
}
pdf_quadding;

static const pdc_keyconn pdf_quadding_keylist[] =
{
    {"left",     text_left},
    {"center",   text_center},
    {"right",    text_right},
    {NULL, 0}
};

typedef enum
{
    disp_visible = (1<<2) ,
    disp_hidden  = (1<<1),
    disp_noview  = (1<<5),
    disp_noprint = 0
}
pdf_display;

static const pdc_keyconn pdf_display_keylist[] =
{
    {"visible",   disp_visible},
    {"hidden",    disp_hidden},
    {"noview",    disp_noview},
    {"noprint",   disp_noprint},
    {NULL, 0}
};

typedef enum
{
    high_none,
    high_invert,
    high_outline,
    high_push
}
pdf_highlight;

static const pdc_keyconn pdf_highlight_keylist[] =
{
    {"none",      high_none},
    {"invert",    high_invert},
    {"outline",   high_outline},
    {"push",      high_push},
    {NULL, 0}
};

static const pdc_keyconn pdf_highlight_pdfkeylist[] =
{
    {"N",   high_none},
    {"I",   high_invert},
    {"O",   high_outline},
    {"P",   high_push},
    {NULL, 0}
};

static const pdc_keyconn pdf_borderstyle_keylist[] =
{
    {"solid",       border_solid},
    {"dashed",      border_dashed},
    {"beveled",     border_beveled},
    {"inset",       border_inset},
    {"underline",   border_underline},
    {NULL, 0}
};

static const pdc_keyconn pdf_borderstyle_pdfkeylist[] =
{
    {"S",  border_solid},
    {"D",  border_dashed},
    {"B",  border_beveled},
    {"I",  border_inset},
    {"U",  border_underline},
    {NULL, 0}
};
#endif /* P_ANNOTS_C || P_FIELDS_C */


#if defined(P_BLOCK_C) || defined(P_DOCUMENT_C) || defined(P_HYPER_C) || \
    defined(P_PARAMS_C) || defined(P_TEXT_C) || defined(P_TEXTFLOW_C) || \
    defined(P_UTIL_C) || defined(P_TAGGED_C) || defined(P_LAYER_C) || \
    defined(P_FIELDS_C)
static const pdc_keyconn pdf_textformat_keylist[] =
{
    {"auto",       pdc_auto},
    {"auto2",      pdc_auto2},
    {"bytes",      pdc_bytes},
    {"bytes2",     pdc_bytes2},
    {"utf8",       pdc_utf8},
    {"utf16",      pdc_utf16},
    {"utf16be",    pdc_utf16be},
    {"utf16le",    pdc_utf16le},
    {NULL, 0}
};
#endif /* P_BLOCK_C || P_DOCUMENT_C || P_HYPER_C ||
          P_PARAMS_C || P_TEXT_C || P_TEXTFLOW_C ||
          P_UTIL_C || P_TAGGED_C || P_LAYER_C ||
          P_FIELDS_C */


#if defined(P_BLOCK_C) || defined(P_FIELDS_C) || \
    defined(P_IMAGE_C) || defined(P_TEXT_C) || defined(P_TEXTFLOW_C)
static const pdc_keyconn pdf_position_keylist[] =
{
    {"left",     0},
    {"bottom",   0},
    {"center",  50},
    {"right",  100},
    {"top",    100},
    {NULL, 0}
};

static const pdc_keyconn pdf_orientate_keylist[] =
{
    {"north",   0},
    {"west",   90},
    {"south", 180},
    {"east",  270},
    {NULL, 0}
};

static const pdc_keyconn pdf_fitmethod_keylist[] =
{
    {"nofit",       pdc_nofit},
    {"clip",        pdc_clip},
    {"auto",        pdc_tauto},
#if !defined (P_TEXTFLOW_C)
    {"slice",       pdc_slice},
    {"meet",        pdc_meet},
    {"entire",      pdc_entire},
#endif
    {NULL, 0}
};
#endif /* P_BLOCK_C || P_FIELDS_C || P_IMAGE_C || P_TEXT_C || P_TEXTFLOW_C */

#if defined(P_BLOCK_C)  || defined(P_FONT_C) || \
    defined(P_HYPER_C) || defined(P_TEXTFLOW_C)
static const pdc_keyconn pdf_fontstyle_pdfkeylist[] =
{
    {"Normal",     pdc_Normal},
    {"Bold",       pdc_Bold},
    {"Italic",     pdc_Italic},
    {"BoldItalic", pdc_BoldItalic},
    {NULL, 0}
};
#endif /* P_BLOCK_C || P_FONT_C || P_HYPER_C || P_TEXTFLOW_C */


#if defined(P_ANNOTS_C) || defined(P_BLOCK_C) || \
    defined(P_COLOR_C) || defined(P_FIELDS_C)
static const pdc_keyconn pdf_colortype_keylist[] =
{
    {"none",         color_none},
    {"gray",         color_gray},
    {"rgb",          color_rgb},
    {"cmyk",         color_cmyk},
    {"spotname",     color_spotname},
    {"spot",         color_spot},
    {"pattern",      color_pattern},
    {"iccbasedgray", color_iccbasedgray},
    {"iccbasedrgb",  color_iccbasedrgb},
    {"iccbasedcmyk", color_iccbasedcmyk},
    {"lab",          color_lab},
    {NULL, 0}
};
#endif /* P_ANNOTS_C || P_BLOCK_C || P_COLOR_C || P_FIELDS_C */


#if defined(P_BLOCK_C) || defined(P_IMAGE_C)
typedef enum
{
    dpi_none = -999999,
    dpi_internal = 0
}
pdf_dpi_states;

static const pdc_keyconn pdf_dpi_keylist[] =
{
    {"none",     dpi_none},
    {"internal", dpi_internal},
    {NULL, 0}
};
#endif /* P_BLOCK_C || P_IMAGE_C */

#if defined(P_BLOCK_C) || defined(P_TEXTFLOW_C)
static const pdc_keyconn pdf_textlen_keylist[] =
{
    {"all",  -1},
    {NULL, 0}
};

static const pdc_keyconn pdf_charname_keylist[] =
{
    {"none", 0},
    {NULL, 0}
};

#endif /* P_BLOCK_C || P_TEXTFLOW_C */


#endif  /* P_KEYCONN_H */

