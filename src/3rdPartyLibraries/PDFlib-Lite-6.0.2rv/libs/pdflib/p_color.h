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

/* $Id: p_color.h,v 1.1 2005/10/11 17:18:14 vuduc2 Exp $
 *
 * PDFlib color definitions
 *
 */

#ifndef P_COLOR_H
#define P_COLOR_H

/*
 * These are treated specially in the global colorspace list, and are not
 * written as /ColorSpace resource since we always specify them directly.
 * Pattern colorspace with base == pdc_undef means PaintType == 1.
 */
#define PDF_SIMPLE_COLORSPACE(cs)		\
	((cs)->type == DeviceGray ||		\
	 (cs)->type == DeviceRGB ||		\
	 (cs)->type == DeviceCMYK ||		\
	 ((cs)->type == PatternCS && cs->val.pattern.base == pdc_undef))


struct pdf_pattern_s {
    pdc_id	obj_id;			/* object id of this pattern */
    int		painttype;		/* colored (1) or uncolored (2) */
    pdc_bool	used_on_current_page;	/* this pattern used on current page */
};

typedef pdc_byte pdf_colormap[256][3];

typedef struct {
    int      		cs;     	/* slot of underlying color space */

    union {
        pdc_scalar      gray;           /* DeviceGray */
        int             pattern;        /* Pattern */
        int             idx;        	/* Indexed */
        struct {                        /* DeviceRGB */
            pdc_scalar  r;
            pdc_scalar  g;
            pdc_scalar  b;
        } rgb;
        struct {                        /* DeviceCMYK */
            pdc_scalar  c;
            pdc_scalar  m;
            pdc_scalar  y;
            pdc_scalar  k;
        } cmyk;
    } val;
} pdf_color;

struct pdf_colorspace_s {
    pdf_colorspacetype type;            /* color space type */

    union {
	struct {			/* Indexed */
	    int   	base;		/* base color space */
	    pdf_colormap *colormap;	/* pointer to colormap */
	    pdc_bool	colormap_done;	/* colormap already written to output */
	    int		palette_size;	/* # of palette entries (not bytes!) */
	    pdc_id	colormap_id;	/* object id of colormap */
	} indexed;

	struct {			/* Pattern */
	    int   	base;		/* base color space for PaintType 2 */
	} pattern;

    } val;

    pdc_id      obj_id;                 /* object id of this colorspace */
    pdc_bool    used_on_current_page;   /* this resource used on current page */
};

/* "color" option */
typedef struct
{
    char name[PDF_MAX_NAMESTRING + 1];
    int type;
    pdc_scalar value[4];
}
pdf_coloropt;


pdf_color *pdf_get_cstate(PDF *p, pdf_drawmode mode);
void    pdf_get_page_colorspaces(PDF *p, pdf_reslist *rl);
void    pdf_write_function_dict(PDF *p, pdf_color *c0, pdf_color *c1,
                                pdc_scalar N);
int	pdf_add_colorspace(PDF *p, pdf_colorspace *cs, pdc_bool inuse);
void    pdf_parse_coloropt(PDF *p, const char *optname, char **optvalue, int ns,
                           int maxtype, pdf_coloropt *c);
void    pdf_set_coloropt(PDF *p, int drawmode, pdf_coloropt *c);
void    pdf_init_coloropt(pdf_coloropt *c);


#endif  /* P_COLOR_H */
