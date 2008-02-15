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

/* $Id: pdflib.c,v 1.1 2005/10/11 17:18:15 vuduc2 Exp $
 *
 * PDFlib API functions
 *
 */

#define PDFLIB_C

#include "p_intern.h"


static const PDFlib_api PDFlib = {
    /* version numbers for checking the DLL against client code */
    sizeof(PDFlib_api),         /* size of this structure */

    PDFLIB_MAJORVERSION,        /* PDFlib major version number */
    PDFLIB_MINORVERSION,        /* PDFlib minor version number */
    PDFLIB_REVISION,            /* PDFlib revision number */

    0,                          /* reserved */

    PDF_activate_item,
    PDF_add_bookmark,
    PDF_add_bookmark2,
    PDF_add_launchlink,
    PDF_add_locallink,
    PDF_add_nameddest,
    PDF_add_note,
    PDF_add_note2,
    PDF_add_pdflink,
    PDF_add_thumbnail,
    PDF_add_weblink,
    PDF_arc,
    PDF_arcn,
    PDF_attach_file,
    PDF_attach_file2,
    PDF_begin_document,
    PDF_begin_document_callback,
    PDF_begin_font,
    PDF_begin_glyph,
    PDF_begin_item,
    PDF_begin_layer,
    PDF_begin_mc,
    PDF_begin_page,
    PDF_begin_page_ext,
    PDF_begin_pattern,
    PDF_begin_template,
    PDF_boot,
    PDF_circle,
    PDF_clip,
    PDF_close,
    PDF_close_image,
    PDF_close_pdi,
    PDF_close_pdi_page,
    PDF_closepath,
    PDF_closepath_fill_stroke,
    PDF_closepath_stroke,
    PDF_concat,
    PDF_continue_text,
    PDF_continue_text2,
    PDF_create_action,
    PDF_create_annotation,
    PDF_create_bookmark,
    PDF_create_field,
    PDF_create_fieldgroup,
    PDF_create_gstate,
    PDF_create_pvf,
    PDF_create_textflow,
    PDF_curveto,
    PDF_define_layer,
    PDF_delete,
    PDF_delete_pvf,
    PDF_delete_textflow,
    PDF_encoding_set_char,
    PDF_end_document,
    PDF_end_font,
    PDF_end_glyph,
    PDF_end_item,
    PDF_end_layer,
    PDF_end_mc,
    PDF_end_page,
    PDF_end_page_ext,
    PDF_end_pattern,
    PDF_end_template,
    PDF_endpath,
    PDF_fill,
    PDF_fill_imageblock,
    PDF_fill_pdfblock,
    PDF_fill_stroke,
    PDF_fill_textblock,
    PDF_findfont,
    PDF_fit_image,
    PDF_fit_pdi_page,
    PDF_fit_textflow,
    PDF_fit_textline,
    PDF_get_api,
    PDF_get_apiname,
    PDF_get_buffer,
    PDF_get_errmsg,
    PDF_get_errnum,
    PDF_get_majorversion,
    PDF_get_minorversion,
    PDF_get_opaque,
    PDF_get_parameter,
    PDF_get_pdi_parameter,
    PDF_get_pdi_value,
    PDF_get_value,
    PDF_info_textflow,
    PDF_initgraphics,
    PDF_lineto,
    PDF_load_font,
    PDF_load_iccprofile,
    PDF_load_image,
    PDF_makespotcolor,
    PDF_mc_point,
    PDF_moveto,
    PDF_new,
    PDF_new2,
    PDF_open_CCITT,
    PDF_open_file,
    PDF_open_image,
    PDF_open_image_file,
    PDF_open_mem,
    PDF_open_pdi,
    PDF_open_pdi_callback,
    PDF_open_pdi_page,
    PDF_place_image,
    PDF_place_pdi_page,
    PDF_process_pdi,
    PDF_rect,
    PDF_restore,
    PDF_resume_page,
    PDF_rotate,
    PDF_save,
    PDF_scale,
    PDF_set_border_color,
    PDF_set_border_dash,
    PDF_set_border_style,
    PDF_set_gstate,
    PDF_set_info,
    PDF_set_info2,
    PDF_set_layer_dependency,
    PDF_set_parameter,
    PDF_set_text_pos,
    PDF_set_value,
    PDF_setcolor,
    PDF_setdash,
    PDF_setdashpattern,
    PDF_setflat,
    PDF_setfont,
    PDF_setgray,
    PDF_setgray_fill,
    PDF_setgray_stroke,
    PDF_setlinecap,
    PDF_setlinejoin,
    PDF_setlinewidth,
    PDF_setmatrix,
    PDF_setmiterlimit,
    PDF_setpolydash,
    PDF_setrgbcolor,
    PDF_setrgbcolor_fill,
    PDF_setrgbcolor_stroke,
    PDF_shading,
    PDF_shading_pattern,
    PDF_shfill,
    PDF_show,
    PDF_show2,
    PDF_show_boxed,
    PDF_show_boxed2,
    PDF_show_xy,
    PDF_show_xy2,
    PDF_shutdown,
    PDF_skew,
    PDF_stringwidth,
    PDF_stringwidth2,
    PDF_stroke,
    PDF_suspend_page,
    PDF_translate,
    PDF_utf16_to_utf8,
    PDF_utf8_to_utf16,
    PDF_xshow,
    pdf_catch,
    pdf_exit_try,
    pdf_jbuf,
    pdf_rethrow
};

static pdc_bool
pdf_enter_api(PDF *p, const char *funame, pdf_state s, const char *fmt, ...)
{
    va_list args;
    int flags = 0;

    /* check whether the client completely screwed up */
    if (p == NULL || p->magic != PDC_MAGIC)
        return pdc_false;

    va_start(args, fmt);
    pdc_trace_enter_api(p->pdc, funame, fmt, args);
    va_end(args);

    /* pdc_enter_api() will return pdc_false if the core is in error state.
    */
    if (p->binding)
        flags |= PDC_ET_NOSUFFIX;
    if (p->objorient)
        flags |= PDC_ET_NOPREFIX;
    if (!pdc_enter_api(p->pdc, funame,  flags))
        return pdc_false;

    /* check whether we are in a valid scope */
    if ((p->state_stack[p->state_sp] & s) == 0)
    {
        /* pdc_error() will NOT throw an exception (and simply return instead)
        ** if the core is already in error state.
        */
        pdc_error(p->pdc, PDF_E_DOC_SCOPE, pdf_current_scope(p), 0, 0, 0);

        return pdc_false;
    }

    return pdc_true;
}

static int
pdf_exit_boolean_api(PDF *p, int retval)
{
    if (p->hastobepos && retval == -1)
        retval += 1;
    pdc_trace_exit_api(p->pdc, pdc_true, "[%d]\n", retval);
    return retval;
}

static int
pdf_exit_handle_api(PDF *p, int retval)
{
    if (p->hastobepos)
        retval += 1;
    pdc_trace_exit_api(p->pdc, pdc_true, "[%d]\n", retval);
    return retval;
}

static pdc_bool
pdf_enter_api2(PDF *p, const char *funame, const char *fmt, ...)
{
    va_list args;

    /* check whether the client completely screwed up */
    if (p == NULL || p->magic != PDC_MAGIC)
        return pdc_false;

    va_start(args, fmt);
    pdc_trace_enter_api(p->pdc, funame, fmt, args);
    va_end(args);

    return pdc_true;
}

/***************************
 *
 *  external API functions
 *
 ***************************/

PDFLIB_API void PDFLIB_CALL
PDF_boot(void)
{
    /* nothing yet */
}

PDFLIB_API void PDFLIB_CALL
PDF_shutdown(void)
{
    /* nothing yet */
}

PDFLIB_API const PDFlib_api * PDFLIB_CALL
PDF_get_api(void)
{
    return &PDFlib;
}

PDFLIB_API int PDFLIB_CALL
PDF_get_majorversion(void)
{
    return PDFLIB_MAJORVERSION;
}

PDFLIB_API int PDFLIB_CALL
PDF_get_minorversion(void)
{
    return PDFLIB_MINORVERSION;
}


#if (defined(WIN32) || defined(__CYGWIN)) && defined(PDFLIB_EXPORTS)

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <winbase.h>
#undef WIN32_LEAN_AND_MEAN

/*
 * DLL entry function as required by Visual C++.
 * It is currently not necessary on Windows, but will eventually
 * be used to boot thread-global resources for PDFlib
 * (mainly font-related stuff).
 */
BOOL WINAPI
DllMain(HANDLE hModule, DWORD ul_reason_for_call, LPVOID lpReserved);

BOOL WINAPI
DllMain(HANDLE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
    (void) hModule;
    (void) lpReserved;

    switch (ul_reason_for_call)
    {
        case DLL_PROCESS_ATTACH:
            PDF_boot();
            break;
        case DLL_THREAD_ATTACH:
            break;
        case DLL_THREAD_DETACH:
            break;
        case DLL_PROCESS_DETACH:
            PDF_shutdown();
            break;
    }

    return TRUE;
}
#endif  /* WIN32 && PDFLIB_EXPORT */


/*************************
 *
 *  general API functions
 *
 *************************/

PDFLIB_API int PDFLIB_CALL
PDF_get_errnum(PDF *p)
{
    static const char fn[] = "PDF_get_errnum";
    int ret;

    if (!pdf_enter_api2(p, fn, "(p[%p])", (void *) p))
    {
        pdc_trace_exit_api(p->pdc, pdc_false, "[0]\n");
        return 0;
    }

    ret = pdc_get_errnum(p->pdc);
    pdc_trace_exit_api(p->pdc, pdc_false, "[%d]\n", ret);
    return ret;
}

PDFLIB_API const char * PDFLIB_CALL
PDF_get_errmsg(PDF *p)
{
    static const char fn[] = "PDF_get_errmsg";
    const char *ret;

    if (!pdf_enter_api2(p, fn, "(p[%p])", (void *) p))
    {
        pdc_trace_exit_api(p->pdc, pdc_false, "[NULL]\n");
        return (char *) 0;
    }

    ret = pdc_get_errmsg(p->pdc);
    pdc_trace_exit_api(p->pdc, pdc_false, "[\"%T\"]\n", ret, 0);
    return ret;
}

PDFLIB_API const char * PDFLIB_CALL
PDF_get_apiname(PDF *p)
{
    static const char fn[] = "PDF_get_apiname";
    const char *ret;

    if (!pdf_enter_api2(p, fn, "(p[%p])", (void *) p))
    {
        pdc_trace_exit_api(p->pdc, pdc_false, "[NULL]\n");
        return (char *) 0;
    }

    ret = pdc_get_apiname(p->pdc);
    pdc_trace_exit_api(p->pdc, pdc_false, "[\"%T\"]\n", ret, 0);
    return ret;
}

PDFLIB_API void * PDFLIB_CALL
PDF_get_opaque(PDF *p)
{
    static const char fn[] = "PDF_get_opaque";

    if (p == NULL || p->magic != PDC_MAGIC)
        return ((void *) NULL);

    pdc_trace_protocol(p->pdc, 1, trc_api, "/* ");

    if (!pdf_enter_api2(p, fn, "(p[%p])", (void *) p))
    {
        return ((void *) NULL);
    }

    pdc_trace_exit_api(p->pdc, pdc_false, "[%p] */\n", p->opaque);
    return p->opaque;
}

PDFLIB_API pdf_jmpbuf * PDFLIB_CALL
pdf_jbuf(PDF *p)
{
    return (pdf_jmpbuf *) pdc_jbuf(p->pdc);
}

PDFLIB_API void PDFLIB_CALL
pdf_exit_try(PDF *p)
{
    if (p == NULL || p->magic != PDC_MAGIC)
        return;

    pdc_exit_try(p->pdc);
}

PDFLIB_API int PDFLIB_CALL
pdf_catch(PDF *p)
{
    if (p == NULL || p->magic != PDC_MAGIC)
        return pdc_false;

    return pdc_catch_extern(p->pdc);
}

PDFLIB_API void PDFLIB_CALL
pdf_rethrow(PDF *p)
{
    if (p == NULL || p->magic != PDC_MAGIC)
        return;

    pdc_rethrow(p->pdc);
}

PDFLIB_API void PDFLIB_CALL
pdf_throw(PDF *p, const char *parm1, const char *parm2, const char *parm3)
{
    pdc_enter_api(p->pdc, "pdf_throw", 0);

    pdc_error(p->pdc, PDF_E_INT_WRAPPER, parm1, parm2, parm3, NULL);
}


/**********************
 *
 *  p_actions.c
 *
 **********************/

PDFLIB_API int PDFLIB_CALL
PDF_create_action(
    PDF *p,
    const char *type,
    const char *optlist)
{
    static const char fn[] = "PDF_create_action";
    int retval = -1;

    if (pdf_enter_api(p, fn,
        (pdf_state) (pdf_state_page | pdf_state_document),
        "(p[%p], \"%s\", \"%T\")",
        (void *) p, type ? type : "", optlist, 0))
    {
        retval = pdf__create_action(p, type, optlist);
    }

    return pdf_exit_handle_api(p, retval);
}

/**********************
 *
 *  p_annots.c
 *
 **********************/

PDFLIB_API void PDFLIB_CALL
PDF_attach_file(
    PDF *p,
    double llx,
    double lly,
    double urx,
    double ury,
    const char *filename,
    const char *description,
    const char *author,
    const char *mimetype,
    const char *icon)
{
    static const char fn[] = "PDF_attach_file";

    if (pdf_enter_api(p, fn, pdf_state_page,
        "(p[%p], %f, %f, %f, %f, \"%T\", \"%T\", \"%T\", \"%s\", \"%s\")",
        (void *) p, llx, lly, urx, ury, filename, 0, description, 0,
        author, 0, mimetype ? mimetype : "", icon ? icon : ""))
    {
        int len_descr = description ? (int) pdc_strlen(description) : 0;
        int len_auth = author ? (int) pdc_strlen(author) : 0;
        pdf__attach_file(p, llx, lly, urx, ury, filename, 0,
            description, len_descr, author, len_auth, mimetype, icon) ;
    }

    pdc_trace_exit_api(p->pdc, pdc_true, "\n");
}

PDFLIB_API void PDFLIB_CALL
PDF_attach_file2(
    PDF *p,
    double llx,
    double lly,
    double urx,
    double ury,
    const char *filename, int len_filename,
    const char *description, int len_descr,
    const char *author, int len_auth,
    const char *mimetype,
    const char *icon)
{
    static const char fn[] = "PDF_attach_file2";

    if (pdf_enter_api(p, fn, pdf_state_page,
        "(p[%p], %f, %f, %f, %f, \"%T\", /*c*/%d, \"%T\", /*c*/%d, "
        "\"%T\", /*c*/%d, \"%s\", \"%s\")",
        (void *) p, llx, lly, urx, ury,
        filename, len_filename, len_filename,
        description, len_descr, len_descr,
        author, len_auth, len_auth,
        mimetype ? mimetype : "",
        icon ? icon : ""))
    {
        pdf__attach_file(p, llx, lly, urx, ury, filename, len_filename,
            description, len_descr, author, len_auth, mimetype, icon) ;
    }

    pdc_trace_exit_api(p->pdc, pdc_true, "\n");
}

PDFLIB_API void PDFLIB_CALL
PDF_add_note(
    PDF *p,
    double llx,
    double lly,
    double urx,
    double ury,
    const char *contents,
    const char *title,
    const char *icon,
    int open)
{
    static const char fn[] = "PDF_add_note";

    if (pdf_enter_api(p, fn, pdf_state_page,
        "(p[%p], %f, %f, %f, %f, \"%T\", \"%T\", \"%s\", %d)",
        (void *) p, llx, lly, urx, ury, contents, 0, title, 0,
        icon ? icon : "", open))
    {
        int len_cont = contents ? (int) pdc_strlen(contents) : 0;
        int len_title = title ? (int) pdc_strlen(title) : 0;
        pdf__add_note(p, llx, lly, urx, ury, contents, len_cont,
                      title, len_title, icon, open);
    }

    pdc_trace_exit_api(p->pdc, pdc_true, "\n");
}

PDFLIB_API void PDFLIB_CALL
PDF_add_note2(
    PDF *p,
    double llx,
    double lly,
    double urx,
    double ury,
    const char *contents, int len_cont,
    const char *title, int len_title,
    const char *icon,
    int open)
{
    static const char fn[] = "PDF_add_note2";

    if (pdf_enter_api(p, fn, pdf_state_page,
        "(p[%p], %f, %f, %f, %f, \"%T\", /*c*/%d, \"%T\", "
        "/*c*/%d, \"%s\", %d)",
        (void *) p, llx, lly, urx, ury, contents, len_cont, len_cont,
        title, len_title, len_title, icon ? icon : "", open))
    {
        pdf__add_note(p, llx, lly, urx, ury, contents, len_cont,
                      title, len_title, icon, open);
    }

    pdc_trace_exit_api(p->pdc, pdc_true, "\n");
}

PDFLIB_API void PDFLIB_CALL
PDF_add_pdflink(
    PDF *p,
    double llx,
    double lly,
    double urx,
    double ury,
    const char *filename,
    int page,
    const char *optlist)
{
    static const char fn[] = "PDF_add_pdflink";

    if (pdf_enter_api(p, fn, pdf_state_page,
        "(p[%p], %f, %f, %f, %f, \"%s\", %d, \"%T\")",
        (void *) p, llx, lly, urx, ury, filename ? filename : "", page,
        optlist, 0))
    {
        pdf__add_pdflink(p, llx, lly, urx, ury, filename, page, optlist);
    }

    pdc_trace_exit_api(p->pdc, pdc_true, "\n");
}

PDFLIB_API void PDFLIB_CALL
PDF_add_launchlink(
    PDF *p,
    double llx,
    double lly,
    double urx,
    double ury,
    const char *filename)
{
    static const char fn[] = "PDF_add_launchlink";

    if (pdf_enter_api(p, fn, pdf_state_page,
        "(p[%p], %f, %f, %f, %f, \"%s\")",
        (void *)p, llx, lly, urx, ury, filename ? filename : ""))
    {
        pdf__add_launchlink(p, llx, lly, urx, ury, filename);
    }

    pdc_trace_exit_api(p->pdc, pdc_true, "\n");
}

PDFLIB_API void PDFLIB_CALL
PDF_add_locallink(
    PDF *p,
    double llx,
    double lly,
    double urx,
    double ury,
    int page,
    const char *optlist)
{
    static const char fn[] = "PDF_add_locallink";

    if (pdf_enter_api(p, fn, pdf_state_page,
        "(p[%p], %f, %f, %f, %f, %d, \"%T\")",
        (void *) p, llx, lly, urx, ury, page, optlist, 0))
    {
        pdf__add_locallink(p, llx, lly, urx, ury, page, optlist);
    }

    pdc_trace_exit_api(p->pdc, pdc_true, "\n");
}

PDFLIB_API void PDFLIB_CALL
PDF_add_weblink(
    PDF *p,
    double llx,
    double lly,
    double urx,
    double ury,
    const char *url)
{
    static const char fn[] = "PDF_add_weblink";

    if (pdf_enter_api(p, fn, pdf_state_page,
        "(p[%p], %f, %f, %f, %f, \"%s\")",
        (void *) p, llx, lly, urx, ury, url ? url : ""))
    {
        pdf__add_weblink(p, llx, lly, urx, ury, url);
    }

    pdc_trace_exit_api(p->pdc, pdc_true, "\n");
}

PDFLIB_API void PDFLIB_CALL
PDF_create_annotation(
    PDF *p,
    double llx,
    double lly,
    double urx,
    double ury,
    const char *type,
    const char *optlist)
{
    static const char fn[] = "PDF_create_annotation";

    if (pdf_enter_api(p, fn, pdf_state_page,
        "(p[%p], %f, %f, %f, %f, \"%s\", \"%T\")",
        (void *) p, llx, lly, urx, ury, type ? type : "",
        optlist, 0))
    {
        pdf__create_annotation(p, llx, lly, urx, ury, type, optlist);
    }

    pdc_trace_exit_api(p->pdc, pdc_true, "\n");
}

PDFLIB_API void PDFLIB_CALL
PDF_set_border_color(
    PDF *p,
    double red,
    double green,
    double blue)
{
    static const char fn[] = "PDF_set_border_color";

    if (pdf_enter_api(p, fn,
        (pdf_state) (pdf_state_document | pdf_state_page),
        "(p[%p], %f, %f, %f)", (void *) p, red, green, blue))
    {
        pdf__set_border_color(p, red, green, blue);
    }

    pdc_trace_exit_api(p->pdc, pdc_true, "\n");
}

PDFLIB_API void PDFLIB_CALL
PDF_set_border_dash(
    PDF *p,
    double b,
    double w)
{
    static const char fn[] = "PDF_set_border_dash";

    if (pdf_enter_api(p, fn,
        (pdf_state) (pdf_state_document | pdf_state_page),
        "(p[%p], %f, %f)", (void *) p, b, w))
    {
        pdf__set_border_dash(p, b, w);
    }

    pdc_trace_exit_api(p->pdc, pdc_true, "\n");
}

PDFLIB_API void PDFLIB_CALL
PDF_set_border_style(
    PDF *p,
    const char *style,
    double width)
{
    static const char fn[] = "PDF_set_border_style";

    if (pdf_enter_api(p, fn,
        (pdf_state) (pdf_state_document | pdf_state_page),
        "(p[%p], \"%s\", %f)", (void *) p, style ? style : "", width))
    {
        pdf__set_border_style(p, style, width);
    }

    pdc_trace_exit_api(p->pdc, pdc_true, "\n");
}


/**********************
 *
 *  p_block.c
 *
 **********************/

PDFLIB_API int PDFLIB_CALL
PDF_fill_imageblock(
    PDF *p,
    int page,
    const char *blockname,
    int image,
    const char *optlist)
{
    static const char fn[] = "PDF_fill_imageblock";
    int retval = -1;

    if (pdf_enter_api(p, fn,
        (pdf_state) pdf_state_content,
        "(p[%p], %d, \"%T\", %d, \"%T\")",
        (void *) p, page, blockname, 0, image, optlist, 0))
    {
        pdc_error(p->pdc, PDF_E_UNSUPP_BLOCK, 0, 0, 0, 0);
    }

    return pdf_exit_boolean_api(p, retval);
}

PDFLIB_API int PDFLIB_CALL
PDF_fill_pdfblock(
    PDF *p,
    int page,
    const char *blockname,
    int contents,
    const char *optlist)
{
    static const char fn[] = "PDF_fill_pdfblock";
    int retval = -1;

    if (pdf_enter_api(p, fn,
        (pdf_state) pdf_state_content,
        "(p[%p], %d, \"%T\", %d, \"%T\")",
        (void *) p, page, blockname, 0, contents, optlist, 0))
    {
        pdc_error(p->pdc, PDF_E_UNSUPP_BLOCK, 0, 0, 0, 0);
    }

    return pdf_exit_boolean_api(p, retval);
}

PDFLIB_API int PDFLIB_CALL
PDF_fill_textblock(
    PDF *p,
    int page,
    const char *blockname,
    const char *text, int len,
    const char *optlist)
{
    static const char fn[] = "PDF_fill_textblock";
    int retval = -1;

    if (pdf_enter_api(p, fn,
        (pdf_state) pdf_state_content,
        "(p[%p], %d, \"%T\", \"%T\", /*c*/%d, \"%T\")",
        (void *) p, page, blockname, 0, text, len, len, optlist, 0))
    {
        pdc_error(p->pdc, PDF_E_UNSUPP_BLOCK, 0, 0, 0, 0);
    }

    return pdf_exit_boolean_api(p, retval);
}


/**********************
 *
 *  p_color.c
 *
 **********************/

PDFLIB_API int PDFLIB_CALL
PDF_makespotcolor(PDF *p, const char *spotname, int len)
{
    static const char fn[] = "PDF_makespotcolor";
    int retval = -1;

    if (pdf_enter_api(p, fn,
        (pdf_state) (pdf_state_content | pdf_state_document),
        "(p[%p], \"%T\", /*c*/%d)",
        (void *) p, spotname, len, len))
    {
        pdc_error(p->pdc, PDF_E_UNSUPP_SPOTCOLOR, 0, 0, 0, 0);
    }

    return pdf_exit_handle_api(p, retval);
}

PDFLIB_API void PDFLIB_CALL
PDF_setcolor(
    PDF *p,
    const char *fstype,
    const char *colorspace,
    double c1, double c2, double c3, double c4)
{
    static const char fn[] = "PDF_setcolor";
    int legal_states;

    if (PDF_GET_STATE(p) == pdf_state_glyph && !pdf_get_t3colorized(p))
        legal_states = pdf_state_page | pdf_state_pattern | pdf_state_template;
    else
        legal_states = pdf_state_content | pdf_state_document;

    if (pdf_enter_api(p, fn, (pdf_state) legal_states,
        "(p[%p], \"%s\", \"%s\", %f, %f, %f, %f)",
        (void *) p, fstype ? fstype : "", colorspace ? colorspace : "",
        c1, c2, c3, c4))
    {
        pdf__setcolor(p, fstype, colorspace, c1, c2, c3, c4);
    }

    pdc_trace_exit_api(p->pdc, pdc_true, "\n");
}

PDFLIB_API void PDFLIB_CALL
PDF_setgray(PDF *p, double g)
{
    static const char fn[] = "PDF_setgray";

    if (pdf_enter_api(p, fn, pdf_state_content, "(p[%p], %f)",
        (void *) p, g))
    {
        pdf__setcolor(p, "fillstroke", "gray", g, 0, 0, 0);
    }

    pdc_trace_exit_api(p->pdc, pdc_true, "\n");
}

PDFLIB_API void PDFLIB_CALL
PDF_setgray_fill(PDF *p, double g)
{
    static const char fn[] = "PDF_setgray_fill";

    if (pdf_enter_api(p, fn, pdf_state_content, "(p[%p], %f)",
        (void *) p, g))
    {
        pdf__setcolor(p, "fill", "gray", g, 0, 0, 0);
    }

    pdc_trace_exit_api(p->pdc, pdc_true, "\n");
}

PDFLIB_API void PDFLIB_CALL
PDF_setgray_stroke(PDF *p, double g)
{
    static const char fn[] = "PDF_setgray_stroke";

    if (pdf_enter_api(p, fn, pdf_state_content, "(p[%p], %f)",
        (void *) p, g))
    {
        pdf__setcolor(p, "stroke", "gray", g, 0, 0, 0);
    }

    pdc_trace_exit_api(p->pdc, pdc_true, "\n");
}

PDFLIB_API void PDFLIB_CALL
PDF_setrgbcolor(PDF *p, double r, double g, double b)
{
    static const char fn[] = "PDF_setrgbcolor";

    if (pdf_enter_api(p, fn, pdf_state_content, "(p[%p], %f, %f, %f)",
        (void *) p, r, g, b))
    {
        pdf__setcolor(p, "fillstroke", "rgb", r, g, b, 0);
    }

    pdc_trace_exit_api(p->pdc, pdc_false, "\n");
}

PDFLIB_API void PDFLIB_CALL
PDF_setrgbcolor_fill(PDF *p, double r, double g, double b)
{
    static const char fn[] = "PDF_setrgbcolor_fill";

    if (pdf_enter_api(p, fn, pdf_state_content, "(p[%p], %f, %f, %f)",
        (void *) p, r, g, b))
    {
        pdf__setcolor(p, "fill", "rgb", r, g, b, 0);
    }

    pdc_trace_exit_api(p->pdc, pdc_true, "\n");
}

PDFLIB_API void PDFLIB_CALL
PDF_setrgbcolor_stroke(PDF *p, double r, double g, double b)
{
    static const char fn[] = "PDF_setrgbcolor_stroke";

    if (pdf_enter_api(p, fn, pdf_state_content, "(p[%p], %f, %f, %f)",
        (void *) p, r, g, b))
        return;
    {
        pdf__setcolor(p, "stroke", "rgb", r, g, b, 0);
    }

    pdc_trace_exit_api(p->pdc, pdc_true, "\n");
}


/**********************
 *
 *  p_document.c
 *
 **********************/

PDFLIB_API int PDFLIB_CALL
PDF_begin_document(
    PDF *p,
    const char *filename, int len,
    const char *optlist)
{
    static const char fn[] = "PDF_begin_document";
    int retval = -1;

    if (pdf_enter_api(p, fn, pdf_state_object,
        "(p[%p], \"%T\", /*c*/%d, \"%T\")",
        (void *) p, filename, len, len, optlist, 0))
    {
        if (filename && *filename && len < 0)
            pdc_error(p->pdc, PDC_E_ILLARG_INT,
                "len", pdc_errprintf(p->pdc, "%d", len), 0, 0);
        retval = pdf__begin_document(p, filename, len, optlist);
    }

    return pdf_exit_boolean_api(p, retval);
}

PDFLIB_API void PDFLIB_CALL
PDF_begin_document_callback(
    PDF *p,
    size_t (*i_writeproc)(PDF *p, void *data, size_t size),
    const char *optlist)
{
    static const char fn[] = "PDF_begin_document_callback";
    size_t (*writeproc)(PDF *, void *, size_t) = i_writeproc;

    if (pdf_enter_api(p, fn, pdf_state_object,
        "(p[%p], wp[%p]), \"%T\"", (void *) p, (void *) writeproc, optlist, 0))
    {
        pdf__begin_document_callback(p, writeproc, optlist);
    }

    pdc_trace_exit_api(p->pdc, pdc_true, "\n");
}

PDFLIB_API void PDFLIB_CALL
PDF_close(PDF *p)
{
    static const char fn[] = "PDF_close";

    if (pdf_enter_api(p, fn, pdf_state_document,
        "(p[%p])\n", (void *) p))
    {
        pdf__end_document(p, "");
    }

    pdc_trace_exit_api(p->pdc, pdc_true, NULL);
}

PDFLIB_API void PDFLIB_CALL
PDF_end_document(PDF *p, const char *optlist)
{
    static const char fn[] = "PDF_end_document";

    if (pdf_enter_api(p, fn, pdf_state_document,
        "(p[%p], \"%T\")\n", (void *) p, optlist, 0))
    {
        pdf__end_document(p, optlist);
    }

    pdc_trace_exit_api(p->pdc, pdc_true, NULL);
}

PDFLIB_API const char * PDFLIB_CALL
PDF_get_buffer(PDF *p, long *size)
{
    static const char fn[] = "PDF_get_buffer";
    const char *retval = NULL;

    if (!size)
        pdc_error(p->pdc, PDC_E_ILLARG_EMPTY, "size", 0, 0, 0);

    *size = (long) 0;

    if (pdf_enter_api(p, fn,
        (pdf_state) (pdf_state_object | pdf_state_document),
        "(p[%p], &size[%p])", (void *) p, (void *) size))
    {
        retval = pdf__get_buffer(p, size);
    }

    pdc_trace_exit_api(p->pdc, pdc_false, "[%p, size=%ld]\n",
    	(void *) (retval), *size);
    return retval;
}

PDFLIB_API int PDFLIB_CALL
PDF_open_file(PDF *p, const char *filename)
{
    static const char fn[] = "PDF_open_file";
    int retval = -1;

    if (pdf_enter_api(p, fn, pdf_state_object, "(p[%p], \"%s\")",
        (void *) p, filename ? filename : ""))
    {
        retval = pdf__begin_document(p, filename, 0, "");
    }

    return pdf_exit_boolean_api(p, retval);
}

PDFLIB_API void PDFLIB_CALL
PDF_open_mem(
    PDF *p,
    size_t (*i_writeproc)(PDF *p, void *data, size_t size))
{
    static const char fn[] = "PDF_open_mem";
    size_t (*writeproc)(PDF *, void *, size_t) = i_writeproc;

    if (pdf_enter_api(p, fn, pdf_state_object,
        "(p[%p], wp[%p])", (void *) p, (void *) writeproc))
    {
        pdf__begin_document_callback(p, writeproc, "");
    }

    pdc_trace_exit_api(p->pdc, pdc_true, "\n");
}


/**********************
 *
 *  p_draw.c
 *
 **********************/

PDFLIB_API void PDFLIB_CALL
PDF_arc(PDF *p, double x, double y, double r, double alpha, double beta)
{
    static const char fn[] = "PDF_arc";

    if (pdf_enter_api(p, fn,
        (pdf_state) (pdf_state_content | pdf_state_path),
        "(p[%p], %f, %f, %f, %f, %f)", (void *) p, x, y, r, alpha, beta))
    {
        pdf__arc(p, x, y, r, alpha, beta);
    }

    pdc_trace_exit_api(p->pdc, pdc_false, "\n");
}

PDFLIB_API void PDFLIB_CALL
PDF_arcn(PDF *p, double x, double y, double r, double alpha, double beta)
{
    static const char fn[] = "PDF_arcn";

    if (pdf_enter_api(p, fn,
        (pdf_state) (pdf_state_content | pdf_state_path),
        "(p[%p], %f, %f, %f, %f, %f)", (void *) p, x, y, r, alpha, beta))
    {
        pdf__arcn(p, x, y, r, alpha, beta);
    }

    pdc_trace_exit_api(p->pdc, pdc_false, "\n");
}

PDFLIB_API void PDFLIB_CALL
PDF_circle(PDF *p, double x, double y, double r)
{
    static const char fn[] = "PDF_circle";

    if (pdf_enter_api(p, fn,
        (pdf_state) (pdf_state_content | pdf_state_path),
        "(p[%p], %f, %f, %f)", (void *) p, x, y, r))
    {
        pdf__circle(p, x, y, r);
    }

    pdc_trace_exit_api(p->pdc, pdc_false, "\n");
}

PDFLIB_API void PDFLIB_CALL
PDF_clip(PDF *p)
{
    static const char fn[] = "PDF_clip";

    if (pdf_enter_api(p, fn, pdf_state_path, "(p[%p])", (void *) p))
    {
        pdf__clip(p);
    }

    pdc_trace_exit_api(p->pdc, pdc_false, "\n");
}

PDFLIB_API void PDFLIB_CALL
PDF_closepath(PDF *p)
{
    static const char fn[] = "PDF_closepath";

    if (pdf_enter_api(p, fn, pdf_state_path, "(p[%p])", (void *) p))
    {
        pdf__closepath(p);
    }

    pdc_trace_exit_api(p->pdc, pdc_false, "\n");
}

PDFLIB_API void PDFLIB_CALL
PDF_closepath_fill_stroke(PDF *p)
{
    static const char fn[] = "PDF_closepath_fill_stroke";

    if (pdf_enter_api(p, fn, pdf_state_path, "(p[%p])", (void *) p))
    {
        pdf__closepath_fill_stroke(p);
    }

    pdc_trace_exit_api(p->pdc, pdc_false, "\n");
}

PDFLIB_API void PDFLIB_CALL
PDF_closepath_stroke(PDF *p)
{
    static const char fn[] = "PDF_closepath_stroke";

    if (pdf_enter_api(p, fn, pdf_state_path, "(p[%p])", (void *) p))
    {
        pdf__closepath_stroke(p);
    }

    pdc_trace_exit_api(p->pdc, pdc_false, "\n");
}

PDFLIB_API void PDFLIB_CALL
PDF_curveto(PDF *p,
    double x_1, double y_1, double x_2, double y_2, double x_3, double y_3)
{
    static const char fn[] = "PDF_curveto";

    if (pdf_enter_api(p, fn, pdf_state_path,
        "(p[%p], %f, %f, %f, %f, %f, %f)",
        (void *) p, x_1, y_1, x_2, y_2, x_3, y_3))
    {
        pdf__curveto(p, x_1, y_1, x_2, y_2, x_3, y_3);
    }

    pdc_trace_exit_api(p->pdc, pdc_false, "\n");
}

PDFLIB_API void PDFLIB_CALL
PDF_endpath(PDF *p)
{
    static const char fn[] = "PDF_endpath";

    if (pdf_enter_api(p, fn, pdf_state_path, "(p[%p])", (void *) p))
    {
        pdf__endpath(p);
    }

    pdc_trace_exit_api(p->pdc, pdc_false, "\n");
}

PDFLIB_API void PDFLIB_CALL
PDF_fill(PDF *p)
{
    static const char fn[] = "PDF_fill";

    if (pdf_enter_api(p, fn, pdf_state_path, "(p[%p])", (void *) p))
    {
        pdf__fill(p);
    }

    pdc_trace_exit_api(p->pdc, pdc_false, "\n");
}

PDFLIB_API void PDFLIB_CALL
PDF_fill_stroke(PDF *p)
{
    static const char fn[] = "PDF_fill_stroke";

    if (pdf_enter_api(p, fn, pdf_state_path, "(p[%p])", (void *) p))
    {
        pdf__fill_stroke(p);
    }

    pdc_trace_exit_api(p->pdc, pdc_false, "\n");
}

PDFLIB_API void PDFLIB_CALL
PDF_lineto(PDF *p, double x, double y)
{
    static const char fn[] = "PDF_lineto";

    if (pdf_enter_api(p, fn, pdf_state_path, "(p[%p], %f, %f)",
        (void *) p, x, y))
    {
        pdf__lineto(p, x, y);
    }

    pdc_trace_exit_api(p->pdc, pdc_false, "\n");
}

PDFLIB_API void PDFLIB_CALL
PDF_moveto(PDF *p, double x, double y)
{
    static const char fn[] = "PDF_moveto";

    if (pdf_enter_api(p, fn,
        (pdf_state) (pdf_state_content | pdf_state_path),
        "(p[%p], %f, %f)", (void *) p, x, y))
    {
        pdf__moveto(p, x, y);
    }

    pdc_trace_exit_api(p->pdc, pdc_false, "\n");
}

PDFLIB_API void PDFLIB_CALL
PDF_rcurveto(PDF *p,
    double x_1, double y_1, double x_2, double y_2, double x_3, double y_3)
{
    static const char fn[] = "PDF_rcurveto";

    if (pdf_enter_api(p, fn, pdf_state_path,
        "(p[%p], %f, %f, %f, %f, %f, %f)",
        (void *) p, x_1, y_1, x_2, y_2, x_3, y_3))
    {
        pdf__rcurveto(p, x_1, y_1, x_2, y_2, x_3, y_3);
    }

    pdc_trace_exit_api(p->pdc, pdc_false, "\n");
}

PDFLIB_API void PDFLIB_CALL
PDF_rect(PDF *p, double x, double y, double width, double height)
{
    static const char fn[] = "PDF_rect";

    if (pdf_enter_api(p, fn,
        (pdf_state) (pdf_state_content | pdf_state_path),
        "(p[%p], %f, %f, %f, %f)", (void *) p, x, y, width, height))
    {
        pdf__rect(p, x, y, width, height);
    }

    pdc_trace_exit_api(p->pdc, pdc_false, "\n");
}

PDFLIB_API void PDFLIB_CALL
PDF_rlineto(PDF *p, double x, double y)
{
    static const char fn[] = "PDF_rlineto";

    if (pdf_enter_api(p, fn, pdf_state_path, "(p[%p], %f, %f)",
        (void *) p, x, y))
    {
        pdf__rlineto(p, x, y);
    }

    pdc_trace_exit_api(p->pdc, pdc_false, "\n");
}

PDFLIB_API void PDFLIB_CALL
PDF_rmoveto(PDF *p, double x, double y)
{
    static const char fn[] = "PDF_rmoveto";

    if (pdf_enter_api(p, fn,
        (pdf_state) (pdf_state_content | pdf_state_path),
        "(p[%p], %f, %f)", (void *) p, x, y))
    {
        pdf__rmoveto(p, x, y);
    }

    pdc_trace_exit_api(p->pdc, pdc_false, "\n");
}

PDFLIB_API void PDFLIB_CALL
PDF_stroke(PDF *p)
{
    static const char fn[] = "PDF_stroke";

    if (pdf_enter_api(p, fn, pdf_state_path, "(p[%p])", (void *) p))
    {
        pdf__stroke(p);
    }

    pdc_trace_exit_api(p->pdc, pdc_false, "\n");
}


/**********************
 *
 *  p_encoding.c
 *
 **********************/

PDFLIB_API const char * PDFLIB_CALL
PDF_encoding_get_glyphname(
    PDF *p,
    const char *encoding,
    int slot)
{
    static const char fn[] = "PDF_encoding_get_glyphname";
    const char *retval = NULL;

    if (pdf_enter_api(p, fn, pdf_state_all,
        "(p[%p], \"%s\", 0x%04X)", (void *) p, encoding ? encoding : "", slot))
    {
        retval = pdf__encoding_get_glyphname(p, encoding, slot);
    }

    pdc_trace_exit_api(p->pdc, pdc_true, "[\"%s\"]\n", retval);
    return retval;
}

PDFLIB_API int PDFLIB_CALL
PDF_encoding_get_unicode(
    PDF *p,
    const char *encoding,
    int slot)
{
    static const char fn[] = "PDF_encoding_get_unicode";
    int retval = 0;

    if (pdf_enter_api(p, fn, pdf_state_all,
        "(p[%p], \"%s\", 0x%04X)", (void *) p, encoding ? encoding : "", slot))
    {
        retval = pdf__encoding_get_unicode(p, encoding, slot);
    }

    pdc_trace_exit_api(p->pdc, pdc_true, "[%d]\n", retval);
    return retval;
}

PDFLIB_API void PDFLIB_CALL
PDF_encoding_set_char(
    PDF *p,
    const char *encoding,
    int slot,
    const char *glyphname,
    int uv)
{
    static const char fn[] = "PDF_encoding_set_char";

    if (pdf_enter_api(p, fn, pdf_state_all,
        "(p[%p], \"%s\", %d, \"%s\", %d/*0x%04X*/)",
        (void *) p, encoding ? encoding : "", slot, glyphname ? glyphname : "",
        uv, uv))
    {
        pdf__encoding_set_char(p, encoding, slot, glyphname, uv);
    }

    pdc_trace_exit_api(p->pdc, pdc_true, "\n");
}


/**********************
 *
 *  p_fields.c
 *
 **********************/

PDFLIB_API void PDFLIB_CALL
PDF_create_field(
    PDF *p,
    double llx, double lly, double urx, double ury,
    const char *name, int len,
    const char *type,
    const char *optlist)
{
    static const char fn[] = "PDF_create_field";

    if (pdf_enter_api(p, fn, pdf_state_page,
        "(p[%p], %f, %f, %f, %f, \"%T\", /*c*/%d, \"%s\", \"%T\")",
        (void *) p, llx, lly, urx, ury, name, len, len,
        type ? type : "", optlist, 0))
    {
        pdc_error(p->pdc, PDF_E_UNSUPP_FORMFIELDS, 0, 0, 0, 0);
    }

    pdc_trace_exit_api(p->pdc, pdc_true, "\n");
}


PDFLIB_API void PDFLIB_CALL
PDF_create_fieldgroup(
    PDF *p,
    const char *name, int len,
    const char *optlist)
{
    static const char fn[] = "PDF_create_fieldgroup";

    if (pdf_enter_api(p, fn, pdf_state_all,
        "(p[%p], \"%T\", /*c*/%d, \"%T\")",
        (void *) p, name, len, len, optlist, 0))
    {
        pdc_error(p->pdc, PDF_E_UNSUPP_FORMFIELDS, 0, 0, 0, 0);
    }

    pdc_trace_exit_api(p->pdc, pdc_true, "\n");
}


/**********************
 *
 *  p_font.c
 *
 **********************/

PDFLIB_API int PDFLIB_CALL
PDF_findfont(
    PDF *p,
    const char *fontname,
    const char *encoding,
    int embed)
{
    static const char fn[] = "PDF_findfont";
    int retval = -1;

    if (pdf_enter_api(p, fn,
        (pdf_state) (pdf_state_document | pdf_state_content),
        "(p[%p], \"%s\", \"%s\", %d)",
        (void *) p, fontname ? fontname : "", encoding ? encoding : "", embed))
    {
        if (embed < 0 || embed > 1)
            pdc_error(p->pdc, PDC_E_ILLARG_INT,
                "embed", pdc_errprintf(p->pdc, "%d", embed), 0, 0);

        retval = pdf__load_font(p, fontname, 0, encoding,
                                (embed > 0) ? "embedding" : "");
    }

    return pdf_exit_handle_api(p, retval);
}

PDFLIB_API int PDFLIB_CALL
PDF_get_glyphid(PDF *p, int font, int code)
{
    static const char fn[] = "PDF_get_glyphid";
    int retval = 0;

    if (pdf_enter_api(p, fn,
        (pdf_state) (pdf_state_document | pdf_state_content | pdf_state_path),
        "(p[%p], %d, %04X)", (void *) p, font, code))
    {
        if (p->hastobepos) font -= 1;
        retval = pdf__get_glyphid(p, font, code);
    }

    pdc_trace_exit_api(p->pdc, pdc_true, "[%d]\n", retval);
    return retval;
}

PDFLIB_API int PDFLIB_CALL
PDF_load_font(
    PDF *p,
    const char *fontname, int len,
    const char *encoding,
    const char *optlist)
{
    static const char fn[] = "PDF_load_font";
    int retval = -1;

    if (pdf_enter_api(p, fn,
        (pdf_state) (pdf_state_document | pdf_state_content),
        "(p[%p], \"%T\", /*c*/%d, \"%s\", \"%T\")",
        (void *) p, fontname, len, len, encoding ? encoding : "", optlist, 0))
    {
        retval = pdf__load_font(p, fontname, len, encoding, optlist);
    }

    return pdf_exit_handle_api(p, retval);
}


/**********************
 *
 *  p_gstate.c
 *
 **********************/


PDFLIB_API void PDFLIB_CALL
PDF_concat(PDF *p, double a, double b, double c, double d, double e, double f)
{
    static const char fn[] = "PDF_concat";

    if (pdf_enter_api(p, fn, pdf_state_content,
        "(p[%p], %f, %f, %f, %f, %f, %f)", (void *) p, a, b, c, d, e, f))
    {
        pdf__concat(p, a, b, c, d, e, f);
    }

    pdc_trace_exit_api(p->pdc, pdc_false, "\n");
}

PDFLIB_API void PDFLIB_CALL
PDF_initgraphics(PDF *p)
{
    static const char fn[] = "PDF_initgraphics";

    if (pdf_enter_api(p, fn, pdf_state_content, "(p[%p])", (void *) p))
    {
        pdf__initgraphics(p);
    }

    pdc_trace_exit_api(p->pdc, pdc_false, "\n");
}

PDFLIB_API void PDFLIB_CALL
PDF_restore(PDF *p)
{
    static const char fn[] = "PDF_restore";

    if (pdf_enter_api(p, fn, pdf_state_content, "(p[%p])", (void *) p))
    {
        pdf__restore(p);
    }

    pdc_trace_exit_api(p->pdc, pdc_false, "\n");
}

PDFLIB_API void PDFLIB_CALL
PDF_rotate(PDF *p, double phi)
{
    static const char fn[] = "PDF_rotate";

    if (pdf_enter_api(p, fn, pdf_state_content, "(p[%p], %f)",
        (void *) p, phi))
    {
        pdf__rotate(p, phi);
    }

    pdc_trace_exit_api(p->pdc, pdc_false, "\n");
}

PDFLIB_API void PDFLIB_CALL
PDF_save(PDF *p)
{
    static const char fn[] = "PDF_save";

    if (pdf_enter_api(p, fn, pdf_state_content, "(p[%p])", (void *) p))
    {
        pdf__save(p);
    }

    pdc_trace_exit_api(p->pdc, pdc_false, "\n");
}

PDFLIB_API void PDFLIB_CALL
PDF_scale(PDF *p, double sx, double sy)
{
    static const char fn[] = "PDF_scale";

    if (pdf_enter_api(p, fn, pdf_state_content, "(p[%p], %f, %f)",
        (void *) p, sx, sy))
    {
        pdf__scale(p, sx, sy);
    }

    pdc_trace_exit_api(p->pdc, pdc_false, "\n");
}

PDFLIB_API void PDFLIB_CALL
PDF_setdash(PDF *p, double b, double w)
{
    static const char fn[] = "PDF_setdash";

    if (pdf_enter_api(p, fn, pdf_state_content, "(p[%p], %f, %f)",
        (void *) p, b, w))
    {
        pdf__setdash(p, b, w);
    }

    pdc_trace_exit_api(p->pdc, pdc_false, "\n");
}

PDFLIB_API void PDFLIB_CALL
PDF_setdashpattern(PDF *p, const char *optlist)
{
    static const char fn[] = "PDF_setdashpattern";

    if (pdf_enter_api(p, fn, pdf_state_content,
        "(p[%p], \"%T\")", (void *) p, optlist, 0))
    {
        pdf__setdashpattern(p, optlist);
    }

    pdc_trace_exit_api(p->pdc, pdc_true, "\n");
}

PDFLIB_API void PDFLIB_CALL
PDF_setflat(PDF *p, double flat)
{
    static const char fn[] = "PDF_setflat";

    if (pdf_enter_api(p, fn, pdf_state_content, "(p[%p], %f)",
        (void *) p, flat))
    {
        pdf__setflat(p, flat);
    }

    pdc_trace_exit_api(p->pdc, pdc_false, "\n");
}

PDFLIB_API void PDFLIB_CALL
PDF_setlinecap(PDF *p, int cap)
{
    static const char fn[] = "PDF_setlinecap";

    if (pdf_enter_api(p, fn, pdf_state_content, "(p[%p], %d)",
        (void *) p, cap))
    {
        pdf__setlinecap(p, cap);
    }

    pdc_trace_exit_api(p->pdc, pdc_false, "\n");
}

PDFLIB_API void PDFLIB_CALL
PDF_setlinejoin(PDF *p, int join)
{
    static const char fn[] = "PDF_setlinejoin";

    if (pdf_enter_api(p, fn, pdf_state_content, "(p[%p], %d)",
        (void *) p, join))
    {
        pdf__setlinejoin(p, join);
    }

    pdc_trace_exit_api(p->pdc, pdc_false, "\n");
}

PDFLIB_API void PDFLIB_CALL
PDF_setlinewidth(PDF *p, double width)
{
    static const char fn[] = "PDF_setlinewidth";

    if (pdf_enter_api(p, fn, pdf_state_content, "(p[%p], %f)",
        (void *) p, width))
    {
        pdf__setlinewidth(p, width);
    }

    pdc_trace_exit_api(p->pdc, pdc_false, "\n");
}

PDFLIB_API void PDFLIB_CALL
PDF_setmatrix(PDF *p, double a, double b, double c, double d,
              double e, double f)
{
    static const char fn[] = "PDF_setmatrix";

    if (pdf_enter_api(p, fn, pdf_state_content,
        "(p[%p], %f, %f, %f, %f, %f, %f)", (void *) p, a, b, c, d, e, f))
    {
        pdf__setmatrix(p, a, b, c, d, e, f);
    }

    pdc_trace_exit_api(p->pdc, pdc_false, "\n");
}

PDFLIB_API void PDFLIB_CALL
PDF_setmiterlimit(PDF *p, double miter)
{
    static const char fn[] = "PDF_setmiterlimit";

    if (pdf_enter_api(p, fn, pdf_state_content, "(p[%p], %f)",
        (void *) p, miter))
    {
        pdf__setmiterlimit(p, miter);
    }

    pdc_trace_exit_api(p->pdc, pdc_false, "\n");
}

PDFLIB_API void PDFLIB_CALL
PDF_setpolydash(PDF *p, float *darray, int length)
{
    static const char fn[] = "PDF_setpolydash";
    int i;

    if (!darray)
        pdc_error(p->pdc, PDC_E_ILLARG_EMPTY, "darray", 0, 0, 0);

    for (i = 0; i < length; i++)
        pdc_trace_protocol(p->pdc, 1, trc_api,
                           "/* *(darray+%d) = %f; */\n", i, darray[i]);

    if (pdf_enter_api(p, fn, pdf_state_content,
        "(p[%p], darray[%p], /*c*/%d)", (void *) p, (void *) darray, length))
    {
        char optlist[1024], *sopt;

        sopt = optlist;
        sopt += pdc_sprintf(p->pdc, optlist, "dasharray {");
        for (i = 0; i < length; i++)
        {
            if (darray[i] < 0)
                pdc_error(p->pdc, PDC_E_ILLARG_FLOAT, "darray[i]",
                          pdc_errprintf(p->pdc, "%f", darray[i]), 0, 0);
            sopt += pdc_sprintf(p->pdc, sopt, "%f ", darray[i]);
        }
        pdc_sprintf(p->pdc, sopt, "}");

        pdf__setdashpattern(p, optlist);
    }

    pdc_trace_exit_api(p->pdc, pdc_true, "\n");
}

PDFLIB_API void PDFLIB_CALL
PDF_skew(PDF *p, double alpha, double beta)
{
    static const char fn[] = "PDF_skew";

    if (pdf_enter_api(p, fn, pdf_state_content, "(p[%p], %f, %f)",
        (void *) p, alpha, beta))
    {
        pdf__skew(p, alpha, beta);
    }

    pdc_trace_exit_api(p->pdc, pdc_false, "\n");
}

PDFLIB_API void PDFLIB_CALL
PDF_translate(PDF *p, double tx, double ty)
{
    static const char fn[] = "PDF_translate";

    if (pdf_enter_api(p, fn, pdf_state_content, "(p[%p], %f, %f)",
        (void *) p, tx, ty))
    {
        pdf__translate(p, tx, ty);
    }

    pdc_trace_exit_api(p->pdc, pdc_false, "\n");
}


/**********************
 *
 *  p_hyper.c
 *
 **********************/

PDFLIB_API int PDFLIB_CALL
PDF_add_bookmark(
    PDF *p,
    const char *text,
    int parent,
    int open)
{
    static const char fn[] = "PDF_add_bookmark";
    int retval = 0;

    if (pdf_enter_api(p, fn,
        (pdf_state) (pdf_state_page | pdf_state_document),
        "(p[%p], \"%T\", %d, %d)", (void *) p, text, 0, parent, open))
    {
        int len = text ? (int) pdc_strlen(text) : 0;
        retval = pdf__add_bookmark(p, text, len, parent, open);
    }
    pdc_trace_exit_api(p->pdc, pdc_true, "[%d]\n", retval);
    return retval;
}

PDFLIB_API int PDFLIB_CALL
PDF_add_bookmark2(
    PDF *p,
    const char *text, int len,
    int parent,
    int open)
{
    static const char fn[] = "PDF_add_bookmark2";
    int retval = 0;

    if (pdf_enter_api(p, fn,
        (pdf_state) (pdf_state_page | pdf_state_document),
        "(p[%p], \"%T\", /*c*/%d, %d, %d)",
        (void *) p, text, len, len, parent, open))
    {
        retval = pdf__add_bookmark(p, text, len, parent, open);
    }
    pdc_trace_exit_api(p->pdc, pdc_true, "[%d]\n", retval);
    return retval;
}

PDFLIB_API void PDFLIB_CALL
PDF_add_nameddest(
    PDF *p,
    const char *name, int len,
    const char *optlist)
{
    static const char fn[] = "PDF_add_nameddest";

    if (pdf_enter_api(p, fn,
        (pdf_state) (pdf_state_page | pdf_state_document),
        "(p[%p], \"%T\", /*c*/%d, \"%T\")",
        (void *) p, name, len, len, optlist, 0))
    {
        pdf__add_nameddest(p, name, len, optlist);
    }

    pdc_trace_exit_api(p->pdc, pdc_true, "\n");
}

PDFLIB_API int PDFLIB_CALL
PDF_create_bookmark(
    PDF *p,
    const char *text, int len,
    const char *optlist)
{
    static const char fn[] = "PDF_create_bookmark";
    int retval = 0;

    if (pdf_enter_api(p, fn,
        (pdf_state) (pdf_state_page | pdf_state_document),
        "(p[%p], \"%T\", /*c*/%d, \"%T\")",
        (void *) p, text, len, len, optlist, 0))
    {
        retval = pdf__create_bookmark(p, text, len, optlist);
    }

    pdc_trace_exit_api(p->pdc, pdc_true, "[%d]\n", retval);
    return retval;
}

PDFLIB_API void PDFLIB_CALL
PDF_set_info(PDF *p, const char *key, const char *value)
{
    static const char fn[] = "PDF_set_info";

    if (pdf_enter_api(p, fn,
        (pdf_state) (pdf_state_object | pdf_state_document | pdf_state_page),
        "(p[%p], \"%T\", \"%T\")",
        (void *) p, key, 0, value, 0))
    {
        int len = value ? (int) pdc_strlen(value) : 0;
        pdf__set_info(p, key, value, len);
    }

    pdc_trace_exit_api(p->pdc, pdc_true, "\n");
}

PDFLIB_API void PDFLIB_CALL
PDF_set_info2(PDF *p, const char *key, const char *value, int len)
{
    static const char fn[] = "PDF_set_info2";

    if (pdf_enter_api(p, fn,
        (pdf_state) (pdf_state_object | pdf_state_document | pdf_state_page),
        "(p[%p], \"%T\", \"%T\", /*c*/%d)",
        (void *) p, key, 0, value, len, len))
    {
        pdf__set_info(p, key, value, len);
    }

    pdc_trace_exit_api(p->pdc, pdc_true, "\n");
}


/**********************
 *
 *  p_icc.c
 *
 **********************/

PDFLIB_API int PDFLIB_CALL
PDF_load_iccprofile(
    PDF *p,
    const char *profilename, int len,
    const char *optlist)
{
    static const char fn[] = "PDF_load_iccprofile";
    int retval = -1;

    if (pdf_enter_api(p, fn,
        (pdf_state) (pdf_state_document | pdf_state_content),
        "(p[%p], \"%T\", /*c*/%d, \"%T\")",
        (void *) p, profilename, len, len, optlist, 0))
    {
        pdc_error(p->pdc, PDF_E_UNSUPP_ICC, 0, 0, 0, 0);
    }

    return pdf_exit_handle_api(p, retval);
}


/**********************
 *
 *  p_image.c
 *
 **********************/

PDFLIB_API void PDFLIB_CALL
PDF_add_thumbnail(PDF *p, int image)
{
    static const char fn[] = "PDF_add_thumbnail";

    if (pdf_enter_api(p, fn, pdf_state_page,
        "(p[%p], %d)", (void *) p, image))
    {
        if (p->hastobepos) image -= 1;
        pdf__add_thumbnail(p, image);
    }

    pdc_trace_exit_api(p->pdc, pdc_true, "\n");
}

PDFLIB_API void PDFLIB_CALL
PDF_close_image(PDF *p, int image)
{
    static const char fn[] = "PDF_close_image";

    if (pdf_enter_api(p, fn,
        (pdf_state) (pdf_state_document | pdf_state_page | pdf_state_font),
        "(p[%p], %d)", (void *) p, image))
    {
        if (p->hastobepos) image -= 1;
        pdf__close_image(p, image);
    }

    pdc_trace_exit_api(p->pdc, pdc_true, "\n");
}

PDFLIB_API void PDFLIB_CALL
PDF_fit_image(
    PDF *p,
    int image,
    double x,
    double y,
    const char *optlist)
{
    static const char fn[] = "PDF_fit_image";

    /* scope check dependent on image type in kernel function */
    if (pdf_enter_api(p, fn, pdf_state_all,
        "(p[%p], %d, %f, %f, \"%T\")", (void *) p, image, x, y, optlist, 0))
    {
        if (p->hastobepos) image -= 1;
        pdf__fit_image(p, image, x, y, optlist);
    }

    pdc_trace_exit_api(p->pdc, pdc_true, "\n");
}

PDFLIB_API int PDFLIB_CALL
PDF_load_image(
    PDF *p,
    const char *type,
    const char *filename,
    int len,
    const char *optlist)
{
    static const char fn[] = "PDF_load_image";
    int retval = -1;

    /* scope check dependent on image type in kernel function */
    if (pdf_enter_api(p, fn,
        (pdf_state) (pdf_state_document | pdf_state_page |
                     pdf_state_font | pdf_state_content),
        "(p[%p], \"%s\", \"%T\", /*c*/%d , \"%T\")",
        (void *) p, type ? type : "", filename, len, len, optlist, 0))
    {
        filename = pdf_convert_filename(p, filename, len, "filename", pdc_true);
        retval = pdf__load_image(p, type, filename, optlist);
    }

    return pdf_exit_handle_api(p, retval);
}

PDFLIB_API int PDFLIB_CALL
PDF_open_CCITT(
    PDF *p,
    const char *filename,
    int width,
    int height,
    int BitReverse, int K, int BlackIs1)
{
    static const char fn[] = "PDF_open_CCITT";
    int retval = -1;

    /* scope check dependent on image type in kernel function */
    if (pdf_enter_api(p, fn,
        (pdf_state) (pdf_state_document | pdf_state_page |
                     pdf_state_font | pdf_state_content),
        "(p[%p], \"%s\", %d, %d, %d, %d, %d)",
        (void *) p, filename ? filename : "", width, height,
        BitReverse, K, BlackIs1))
    {
        char optlist[128];

        pdc_sprintf(p->pdc, optlist,
            "width %d  height %d  bitreverse %s  K %d  invert %s",
            width, height, PDC_BOOLSTR(BitReverse), K, PDC_BOOLSTR(BlackIs1));
        filename = pdf_convert_filename(p, filename, 0, "filename", pdc_true);
        retval = pdf__load_image(p, "CCITT", filename, optlist);
    }

    return pdf_exit_handle_api(p, retval);
}

PDFLIB_API int PDFLIB_CALL
PDF_open_image(
    PDF *p,
    const char *type,
    const char *source,
    const char *data,
    long length,
    int width,
    int height,
    int components,
    int bpc,
    const char *params)
{
    static const char fn[] = "PDF_open_image";
    int retval = -1;

    /* scope check dependent on image type in kernel function */
    if (pdf_enter_api(p, fn,
        (pdf_state) (pdf_state_document | pdf_state_page |
                     pdf_state_font | pdf_state_content),
        "(p[%p], \"%s\", \"%s\", idata[%p], %ld, %d, %d, %d, %d, \"%s\")",
        (void *) p, type ? type : "", source, (void *) data, length,
        width, height, components, bpc, params ? params : ""))
    {
        const char *filename = data;
        char optlist[512];
        pdc_bool memory = pdc_false;

        if (type == NULL || *type == '\0')
            pdc_error(p->pdc, PDC_E_ILLARG_EMPTY, "type", 0, 0, 0);

        if (source == NULL || *source == '\0')
            pdc_error(p->pdc, PDC_E_ILLARG_EMPTY, "source", 0, 0, 0);

        if (!strcmp(type, "raw") && data == NULL)
            pdc_error(p->pdc, PDC_E_ILLARG_EMPTY, "data", 0, 0, 0);

        /* create optlist */
        optlist[0] = 0;
        if (!strcmp(type, "raw") || !strcmp(type, "ccitt"))
            pdc_sprintf(p->pdc, optlist,
                "width %d  height %d  components %d  bpc %d ",
                width, height, components, bpc);

        if (length < 0L)
        {
            strcat(optlist, "bitreverse true ");
            length = -length;
        }

        strcat(optlist, "reftype ");
        if (!strcmp(source, "fileref"))
            strcat(optlist, "fileref ");
        else if (!strcmp(source, "memory"))
        {
            memory = pdc_true;
            strcat(optlist, "direct ");
        }
        else if (!strcmp(source, "url"))
            strcat(optlist, "url ");

        if (params != NULL && *params != '\0')
        {
            char **items;
            int i, nitems;

            /* separator characters because of compatibility */
            nitems = pdc_split_stringlist(p->pdc, params, "\t :", &items);
            for (i = 0; i < nitems; i++)
            {
                if (!strcmp(items[i], "invert"))
                    strcat(optlist, "invert true ");
                else if (!strcmp(items[i], "ignoremask"))
                    strcat(optlist, "ignoremask true ");
                else if (!strcmp(items[i], "inline"))
                    strcat(optlist, "inline true ");
                else if (!strcmp(items[i], "interpolate"))
                    strcat(optlist, "interpolate true ");
                else if (!strcmp(items[i], "mask"))
                    strcat(optlist, "mask true ");
                else if (!strcmp(items[i], "/K"))
                    strcat(optlist, "K ");
                else if (!strcmp(items[i], "/BlackIs1"))
                    strcat(optlist, "invert ");
                else
                    strcat(optlist, items[i]);
            }
            pdc_cleanup_stringlist(p->pdc, items);
        }

        /* create virtual file */
        if (memory)
        {
            filename = "__raw__image__data__";
            pdf__create_pvf(p, filename, data, (size_t) length, "");
        }

        filename = pdf_convert_filename(p, filename, 0, "filename", pdc_true);
        retval = pdf__load_image(p, type, filename, (const char *) optlist);

        if (memory)
            (void) pdf__delete_pvf(p, filename);
    }

    return pdf_exit_handle_api(p, retval);
}

PDFLIB_API int PDFLIB_CALL
PDF_open_image_file(
    PDF *p,
    const char *type,
    const char *filename,
    const char *stringparam,
    int intparam)
{
    static const char fn[] = "PDF_open_image_file";
    int retval = -1;

    /* scope check dependent on image type in kernel function */
    if (pdf_enter_api(p, fn,
        (pdf_state) (pdf_state_document | pdf_state_page |
                     pdf_state_font | pdf_state_content),
        "(p[%p], \"%s\", \"%s\", \"%s\", %d)",
        (void *) p, type ? type : "", filename ? filename : "",
        stringparam ? stringparam : "", intparam))
    {
        char optlist[128];

        optlist[0] = 0;
        if (stringparam != NULL && *stringparam != '\0')
        {
            if (!strcmp(stringparam, "invert"))
                strcpy(optlist, "invert true ");
            else if (!strcmp(stringparam, "inline"))
                strcpy(optlist, "inline true ");
            else if (!strcmp(stringparam, "ignoremask"))
                strcpy(optlist, "ignoremask true ");
            else if (!strcmp(stringparam, "mask"))
                strcpy(optlist, "mask true ");
            else if (!strcmp(stringparam, "masked"))
                pdc_sprintf(p->pdc, optlist, "masked %d ", intparam);
            else if (!strcmp(stringparam, "colorize"))
                pdc_sprintf(p->pdc, optlist, "colorize %d ", intparam);
            else if (!strcmp(stringparam, "page"))
                pdc_sprintf(p->pdc, optlist, "page %d ", intparam);
            else if (!strcmp(stringparam, "iccprofile"))
                pdc_sprintf(p->pdc, optlist, "iccprofile %d ", intparam);
        }
        filename = pdf_convert_filename(p, filename, 0, "filename", pdc_true);
        retval = pdf__load_image(p, type, filename, optlist);
    }

    return pdf_exit_handle_api(p, retval);
}

PDFLIB_API void PDFLIB_CALL
PDF_place_image(
    PDF *p,
    int image,
    double x,
    double y,
    double scale)
{
    static const char fn[] = "PDF_place_image";

    /* scope check dependent on image type in kernel function */
    if (pdf_enter_api(p, fn, pdf_state_all,
        "(p[%p], %d, %f, %f, %f)", (void *) p, image, x, y, scale))
    {
        char optlist[128];

        pdc_sprintf(p->pdc, optlist, "dpi none  scale %f", scale);
        if (p->hastobepos) image -= 1;
        pdf__fit_image(p, image, x, y, optlist);
    }

    pdc_trace_exit_api(p->pdc, pdc_true, "\n");
}


/**********************
 *
 *  p_kerning.c
 *
 **********************/

PDFLIB_API double PDFLIB_CALL
PDF_get_kern_amount(
    PDF *p,
    int font,
    int firstchar,
    int secondchar)
{
    static const char fn[] = "PDF_get_kern_amount";
    double retval = 0;

    if (pdf_enter_api(p, fn,
        (pdf_state) (pdf_state_document | pdf_state_content | pdf_state_path),
        "(p[%p], %d, %d, %d)", (void *) p, font, firstchar, secondchar))
    {
        if (p->hastobepos) font -= 1;
        pdc_error(p->pdc, PDF_E_UNSUPP_KERNING, 0, 0, 0, 0);
    }

    pdc_trace_exit_api(p->pdc, pdc_true, "[%f]\n", retval);
    return retval;
}


/**********************
 *
 *  p_layer.c
 *
 **********************/

PDFLIB_API void PDFLIB_CALL
PDF_begin_layer(PDF *p, int layer)
{
    static const char fn[] = "PDF_begin_layer";

    if (pdf_enter_api(p, fn, pdf_state_page,
        "(p[%p], %d)", (void *) p, layer))
    {
        pdc_error(p->pdc, PDF_E_UNSUPP_LAYER, 0, 0, 0, 0);
    }

    pdc_trace_exit_api(p->pdc, pdc_true, "\n");
}

PDFLIB_API int PDFLIB_CALL
PDF_define_layer(
    PDF *p,
    const char *name, int len,
    const char *optlist)
{
    static const char fn[] = "PDF_define_layer";
    int retval = -1;

    if (pdf_enter_api(p, fn, (pdf_state) (pdf_state_document | pdf_state_page),
        "(p[%p], \"%T\", /*c*/%d, \"%T\")",
         (void *) p, name, len, len, optlist, 0))
    {
        pdc_error(p->pdc, PDF_E_UNSUPP_LAYER, 0, 0, 0, 0);
    }

    return pdf_exit_handle_api(p, retval);
}

PDFLIB_API void PDFLIB_CALL
PDF_end_layer(PDF *p)
{
    static const char fn[] = "PDF_end_layer";

    if (pdf_enter_api(p, fn, pdf_state_page,
        "(p[%p])", (void *) p))
    {
        pdc_error(p->pdc, PDF_E_UNSUPP_LAYER, 0, 0, 0, 0);
    }

    pdc_trace_exit_api(p->pdc, pdc_true, "\n");
}

PDFLIB_API void PDFLIB_CALL
PDF_set_layer_dependency(
    PDF *p,
    const char *type,
    const char *optlist)
{
    static const char fn[] = "PDF_set_layer_dependency";


    if (pdf_enter_api(p, fn, (pdf_state) (pdf_state_document | pdf_state_page),
        "(p[%p], \"%s\", \"%T\")", (void *) p, type ? type : "", optlist, 0))
    {
        pdc_error(p->pdc, PDF_E_UNSUPP_LAYER, 0, 0, 0, 0);
    }

    pdc_trace_exit_api(p->pdc, pdc_true, "\n");
}


/**********************
 *
 *  p_object.c
 *
 **********************/

PDFLIB_API void PDFLIB_CALL
PDF_delete(PDF *p)
{
    static const char fn[] = "PDF_delete";

    if (pdf_enter_api2(p, fn, "(p[%p])\n", (void *) p))
    {
        pdf__delete(p);
    }
}

PDFLIB_API PDF * PDFLIB_CALL
PDF_new(void)
{
    return pdf__new(NULL, NULL, NULL, NULL, NULL);
}

PDFLIB_API PDF * PDFLIB_CALL
PDF_new2(
    void  (*errorhandler)(PDF *p, int type, const char *msg),
    void* (*allocproc)(PDF *p, size_t size, const char *caller),
    void* (*reallocproc)(PDF *p, void *mem, size_t size, const char *caller),
    void  (*freeproc)(PDF *p, void *mem),
    void   *opaque)
{
    return pdf__new(errorhandler, allocproc, reallocproc, freeproc, opaque);
}


/**********************
 *
 *  p_page.c
 *
 **********************/

PDFLIB_API void PDFLIB_CALL
PDF_begin_page(PDF *p, double width, double height)
{
    static const char fn[] = "PDF_begin_page";

    if (pdf_enter_api(p, fn, pdf_state_document, "(p[%p], %f, %f)",
       (void *) p, width, height))
    {
        pdf__begin_page(p, width, height);
    }

    pdc_trace_exit_api(p->pdc, pdc_true, "\n");
}

PDFLIB_API void PDFLIB_CALL
PDF_begin_page_ext(PDF *p, double width, double height, const char *optlist)
{
    static const char fn[] = "PDF_begin_page_ext";

    if (pdf_enter_api(p, fn, pdf_state_document, "(p[%p], %f, %f, \"%T\")",
       (void *) p, width, height, optlist, 0))
    {
        pdf__begin_page_ext(p, width, height, optlist);
    }

    pdc_trace_exit_api(p->pdc, pdc_true, "\n");
}

PDFLIB_API void PDFLIB_CALL
PDF_end_page(PDF *p)
{
    static const char fn[] = "PDF_end_page";

    if (pdf_enter_api(p, fn, pdf_state_page, "(p[%p])\n", (void *) p))
    {
        pdf__end_page_ext(p, "");
    }

    pdc_trace_exit_api(p->pdc, pdc_true, NULL);
}

PDFLIB_API void PDFLIB_CALL
PDF_end_page_ext(PDF *p, const char *optlist)
{
    static const char fn[] = "PDF_end_page_ext";

    if (pdf_enter_api(p, fn, pdf_state_page, "(p[%p], \"%T\")\n",
        (void *) p, optlist, 0))
    {
        pdf__end_page_ext(p, optlist);
    }

    pdc_trace_exit_api(p->pdc, pdc_true, NULL);
}

PDFLIB_API void PDFLIB_CALL
PDF_resume_page(PDF *p, const char *optlist)
{
    static const char fn[] = "PDF_resume_page";

    if (pdf_enter_api(p, fn, pdf_state_document, "(p[%p], \"%T\")",
       (void *) p, optlist, 0))
    {
        pdf__resume_page(p, optlist);
    }

    pdc_trace_exit_api(p->pdc, pdc_true, "\n");
}

PDFLIB_API void PDFLIB_CALL
PDF_suspend_page(PDF *p, const char *optlist)
{
    static const char fn[] = "PDF_suspend_page";

    if (pdf_enter_api(p, fn, pdf_state_page, "(p[%p], \"%T\")",
       (void *) p, optlist, 0))
    {
        pdf__suspend_page(p, optlist);
    }

    pdc_trace_exit_api(p->pdc, pdc_true, "\n");
}


/**********************
 *
 *  p_parameter.c
 *
 **********************/

PDFLIB_API const char * PDFLIB_CALL
PDF_get_parameter(PDF *p, const char *key, double modifier)
{
    static const char fn[] = "PDF_get_parameter";
    const char *retval = "";

    if (!pdc_stricmp(key, "version"))
    {
        retval = PDFLIB_VERSIONSTRING;
    }
    else if (!pdc_stricmp(key, "pdi"))
    {
        retval = "false";
    }
    else if (pdf_enter_api(p, fn, (pdf_state) pdf_state_all,
        "(p[%p], \"%s\", %f);", (void *) p, key ? key : "", modifier))
    {
        retval = pdf__get_parameter(p, key, modifier);
        pdc_trace_exit_api(p->pdc, pdc_true, "[\"%T\"]\n", retval, 0);
    }

    return retval;
}

PDFLIB_API double PDFLIB_CALL
PDF_get_value(PDF *p, const char *key, double modifier)
{
    static const char fn[] = "PDF_get_value";
    double retval = 0;

    if (!pdc_stricmp(key, "major"))
    {
        retval = PDFLIB_MAJORVERSION;
    }
    else if (!pdc_stricmp(key, "minor"))
    {
        retval = PDFLIB_MINORVERSION;
    }
    else if (!pdc_stricmp(key, "revision"))
    {
        retval = PDFLIB_REVISION;
    }
    else if (pdf_enter_api(p, fn, (pdf_state) pdf_state_all,
        "(p[%p], \"%s\", %f)", (void *) p, key ? key : "", modifier))
    {
        retval = pdf__get_value(p, key, modifier);
                     pdc_trace_exit_api(p->pdc, pdc_true, "[%f]\n", retval);
    }

    return retval;
}

PDFLIB_API void PDFLIB_CALL
PDF_set_parameter(PDF *p, const char *key, const char *value)
{
    static const char fn[] = "PDF_set_parameter";

    if (pdf_enter_api(p, fn, (pdf_state) pdf_state_all,
        "(p[%p], \"%s\", \"%T\")",
        (void *) p, key ? key : "", value, 0))
    {
        pdf__set_parameter(p, key, value);
    }

    pdc_trace_exit_api(p->pdc, pdc_false, "\n");
}

PDFLIB_API void PDFLIB_CALL
PDF_set_value(PDF *p, const char *key, double value)
{
    static const char fn[] = "PDF_set_value";

    if (pdf_enter_api(p, fn, (pdf_state) pdf_state_all,
        "(p[%p], \"%s\", %f)", (void *) p, key, value))
    {
        pdf__set_value(p, key ? key : "", value);
    }

    pdc_trace_exit_api(p->pdc, pdc_false, "\n");
}


/**********************
 *
 *  p_pattern.c
 *
 **********************/

PDFLIB_API int PDFLIB_CALL
PDF_begin_pattern(
    PDF *p,
    double width,
    double height,
    double xstep,
    double ystep,
    int painttype)
{
    static const char fn[] = "PDF_begin_pattern";
    int retval = -1;

    if (pdf_enter_api(p, fn, (pdf_state) (pdf_state_document | pdf_state_page),
        "(p[%p], %f, %f, %f, %f, %d)",
        (void *) p, width, height, xstep, ystep, painttype))
    {
        retval = pdf__begin_pattern(p, width, height, xstep, ystep, painttype);
    }

    return pdf_exit_handle_api(p, retval);
}

PDFLIB_API void PDFLIB_CALL
PDF_end_pattern(PDF *p)
{
    static const char fn[] = "PDF_end_pattern";

    if (pdf_enter_api(p, fn, pdf_state_pattern, "(p[%p])", (void *) p))
    {
        pdf__end_pattern(p);
    }

    pdc_trace_exit_api(p->pdc, pdc_true, "\n");
}


/**********************
 *
 *  p_pdi.c
 *
 **********************/

PDFLIB_API void PDFLIB_CALL
PDF_close_pdi(PDF *p, int doc)
{
    static const char fn[] = "PDF_close_pdi";

    if (pdf_enter_api(p, fn,
        (pdf_state) (pdf_state_object | pdf_state_document | pdf_state_page),
        "(p[%p], %d)", (void *) p, doc))
    {
        pdc_error(p->pdc, PDF_E_UNSUPP_PDI, 0, 0, 0, 0);
    }

    pdc_trace_exit_api(p->pdc, pdc_true, "\n");
}

PDFLIB_API void PDFLIB_CALL
PDF_close_pdi_page(PDF *p, int page)
{
    static const char fn[] = "PDF_close_pdi_page";

    if (pdf_enter_api(p, fn,
        (pdf_state) (pdf_state_document | pdf_state_page),
        "(p[%p], %d)", (void *) p, page))
    {
        pdc_error(p->pdc, PDF_E_UNSUPP_PDI, 0, 0, 0, 0);
    }

    pdc_trace_exit_api(p->pdc, pdc_true, "\n");
}

PDFLIB_API void PDFLIB_CALL
PDF_fit_pdi_page(PDF *p, int page, double x, double y, const char *optlist)
{
    static const char fn[] = "PDF_fit_pdi_page";

    if (pdf_enter_api(p, fn, pdf_state_content,
        "(p[%p], %d, %f, %f, \"%T\")", (void *) p, page, x, y, optlist, 0))
    {
        pdc_error(p->pdc, PDF_E_UNSUPP_PDI, 0, 0, 0, 0);
    }

    pdc_trace_exit_api(p->pdc, pdc_true, "\n");
}

PDFLIB_API const char *PDFLIB_CALL
PDF_get_pdi_parameter(
    PDF *p,
    const char *key,
    int doc,
    int page,
    int reserved,
    int *len)
{
    static const char fn[] = "PDF_get_pdi_parameter";
    const char *retval = "";

    if (len)
        *len = 0;

    if (pdf_enter_api(p, fn, pdf_state_all,
        len ? "(p[%p], \"%s\", %d, %d, %d, /*c*/&len[%p])" :
              "(p[%p], \"%s\", %d, %d, %d, /*c*/NULL)",
        (void *) p, key ? key : "", doc, page, reserved, len))
    {
        pdc_error(p->pdc, PDF_E_UNSUPP_PDI, 0, 0, 0, 0);
    }

    pdc_trace_exit_api(p->pdc, pdc_true, "[\"%T\"]\n", retval, 0);
    return retval;
}

PDFLIB_API double PDFLIB_CALL
PDF_get_pdi_value(
    PDF *p,
    const char *key,
    int doc,
    int page,
    int reserved)
{
    static const char fn[] = "PDF_get_pdi_value";
    double retval = 0;

    if (pdf_enter_api(p, fn, pdf_state_all,
        "(p[%p], \"%s\", %d, %d, %d)",
        (void *) p, key ? key : "", doc, page, reserved))
    {
        pdc_error(p->pdc, PDF_E_UNSUPP_PDI, 0, 0, 0, 0);
    }

    pdc_trace_exit_api(p->pdc, pdc_true, "[%f]\n", retval);
    return retval;
}

PDFLIB_API int PDFLIB_CALL
PDF_open_pdi(
    PDF *p,
    const char *filename,
    const char *optlist,
    int len)
{
    static const char fn[] = "PDF_open_pdi";
    int retval = -1;

    /* state "object" doesn't make sense until PDFlib can handle this,
    ** but is allowed here for version compatibility
    */
    if (pdf_enter_api(p, fn,
        (pdf_state) (pdf_state_object | pdf_state_document | pdf_state_page),
        "(p[%p], \"%T\", \"%T\", %d)",
        (void *) p, filename, len, optlist, 0, len))
    {
        pdc_error(p->pdc, PDF_E_UNSUPP_PDI, 0, 0, 0, 0);
    }

    return pdf_exit_handle_api(p, retval);
}

PDFLIB_API int PDFLIB_CALL
PDF_open_pdi_callback(
    PDF *p,
    void *opaque,
    size_t filesize,
    size_t (*readproc)(void *opaque, void *buffer, size_t size),
    int (*seekproc)(void *opaque, long offset),
    const char *optlist)
{
    static const char fn[] = "PDF_open_pdi_callback";
    int retval = -1;

    /* state "object" doesn't make sense until PDFlib can handle this,
    ** but is allowed here for version compatibility
    */
    if (pdf_enter_api(p, fn,
        (pdf_state) (pdf_state_object | pdf_state_document | pdf_state_page),
        "(p[%p], opaque[%p], %ld, readproc[%p], seekproc[%p] \"%T\")",
        (void *)p, opaque, filesize, readproc, seekproc, optlist, 0))
    {
        pdc_error(p->pdc, PDF_E_UNSUPP_PDI, 0, 0, 0, 0);
    }

    return pdf_exit_handle_api(p, retval);
}


PDFLIB_API int PDFLIB_CALL
PDF_open_pdi_page(PDF *p, int doc, int pagenumber, const char* optlist)
{
    static const char fn[] = "PDF_open_pdi_page";
    int retval = -1;

    if (pdf_enter_api(p, fn,
        (pdf_state) (pdf_state_document | pdf_state_page),
        "(p[%p], %d, %d, \"%T\")", (void *) p, doc, pagenumber, optlist, 0))
    {
        pdc_error(p->pdc, PDF_E_UNSUPP_PDI, 0, 0, 0, 0);
    }

    return pdf_exit_handle_api(p, retval);
}

PDFLIB_API void PDFLIB_CALL
PDF_place_pdi_page(PDF *p, int page, double x, double y, double sx, double sy)
{
    static const char fn[] = "PDF_place_pdi_page";

    if (pdf_enter_api(p, fn, pdf_state_content,
        "(p[%p], %d, %f, %f, %f, %f)", (void *) p, page, x, y, sx, sy))
    {
        pdc_error(p->pdc, PDF_E_UNSUPP_PDI, 0, 0, 0, 0);
    }

    pdc_trace_exit_api(p->pdc, pdc_true, "\n");
}

PDFLIB_API int PDFLIB_CALL
PDF_process_pdi(PDF *p, int doc, int page, const char *optlist)
{
    static const char fn[] = "PDF_process_pdi";
    int retval = -1;

    if (pdf_enter_api(p, fn,
        (pdf_state) (pdf_state_document),
        "(p[%p], %d, %d, \"%T\")", (void *) p, doc, page, optlist, 0))
    {
        pdc_error(p->pdc, PDF_E_UNSUPP_PDI, 0, 0, 0, 0);
    }

    return pdf_exit_boolean_api(p, retval);
}


/**********************
 *
 *  p_resource.c
 *
 **********************/

PDFLIB_API void PDFLIB_CALL
PDF_create_pvf(
    PDF *p, const char *filename, int len,
    const void *data, size_t size,
    const char *optlist)
{
    static const char fn[] = "PDF_create_pvf";

    if (pdf_enter_api(p, fn, pdf_state_all,
        "(p[%p], \"%T\", /*c*/%d, data[%p], /*c*/%d, \"%T\")",
        (void *) p, filename, len, len, data, size, optlist, 0))
    {
        filename = pdf_convert_filename(p, filename, len, "filename",
                                        pdc_false);
        pdf__create_pvf(p, filename, data, size, optlist);
    }

    pdc_trace_exit_api(p->pdc, pdc_false, "\n");
}

PDFLIB_API int PDFLIB_CALL
PDF_delete_pvf(PDF *p, const char *filename, int len)
{
    static const char fn[] = "PDF_delete_pvf";
    int retval = 1;

    if (pdf_enter_api(p, fn, pdf_state_all,
        "(p[%p], \"%T\", /*c*/%d)",
        (void *) p, filename, len, len))
    {
        filename = pdf_convert_filename(p, filename, len, "filename",
                                        pdc_false);
        retval = pdf__delete_pvf(p, filename);
    }

    return pdf_exit_boolean_api(p, retval);
}


/**********************
 *
 *  p_shading.c
 *
 **********************/

PDFLIB_API int PDFLIB_CALL
PDF_shading(
    PDF *p,
    const char *type,
    double x_0, double y_0,
    double x_1, double y_1,
    double c_1, double c_2, double c_3, double c_4,
    const char *optlist)
{
    static const char fn[] = "PDF_shading";
    int retval = -1;

    if (pdf_enter_api(p, fn,
        (pdf_state) (pdf_state_document | pdf_state_page | pdf_state_font),
        "(p[%p], \"%s\", %f, %f, %f, %f, %f, %f, %f, %f, \"%T\")",
        (void *) p, type ? type : "", x_0, y_0, x_1, y_1, c_1, c_2, c_3, c_4,
        optlist, 0))
    {
        retval = pdf__shading(p, type, x_0, y_0, x_1, y_1,
                              c_1, c_2, c_3, c_4, optlist);
    }

    return pdf_exit_handle_api(p, retval);
}

PDFLIB_API int PDFLIB_CALL
PDF_shading_pattern(PDF *p, int shading, const char *optlist)
{
    static const char fn[] = "PDF_shading_pattern";
    int retval = -1;

    if (pdf_enter_api(p, fn,
        (pdf_state) (pdf_state_document | pdf_state_page | pdf_state_font),
        "(p[%p], %d, \"%T\")", (void *) p, shading, optlist, 0))
    {
        if (p->hastobepos) shading -= 1;
        retval = pdf__shading_pattern(p, shading, optlist);
    }

    return pdf_exit_handle_api(p, retval);
}

PDFLIB_API void PDFLIB_CALL
PDF_shfill(PDF *p, int shading)
{
    static const char fn[] = "PDF_shfill";
    int legal_states;

    if (PDF_GET_STATE(p) == pdf_state_glyph && !pdf_get_t3colorized(p))
        legal_states = pdf_state_page | pdf_state_pattern | pdf_state_template;

    else if (PDF_GET_STATE(p) == pdf_state_pattern &&
        pdf_get_shading_painttype(p) == 2)
        legal_states = pdf_state_page | pdf_state_glyph | pdf_state_template;

    else
        legal_states = pdf_state_content;

    if (pdf_enter_api(p, fn, (pdf_state) legal_states,
        "(p[%p], %d)", (void *) p, shading))
    {
        if (p->hastobepos) shading -= 1;
        pdf__shfill(p, shading);
    }

    pdc_trace_exit_api(p->pdc, pdc_true, "\n");
}


/**********************
 *
 *  p_tagged.c
 *
 **********************/

PDFLIB_API void PDFLIB_CALL
PDF_begin_mc(PDF *p, const char *tag, const char *optlist)
{
    static const char fn[] = "PDF_begin_mc";

    if (pdf_enter_api(p, fn, pdf_state_content,
        "(p[%p], \"%s\", \"%T\")", (void *) p, tag ? tag : "", optlist, 0))
    {
        pdc_error(p->pdc, PDF_E_UNSUPP_MC, 0, 0, 0, 0);
    }

    pdc_trace_exit_api(p->pdc, pdc_true, "\n");
}

PDFLIB_API void PDFLIB_CALL
PDF_end_mc(PDF *p)
{
    static const char fn[] = "PDF_end_mc";

    if (pdf_enter_api(p, fn, pdf_state_content, "(p[%p]", (void *) p))
    {
        pdc_error(p->pdc, PDF_E_UNSUPP_MC, 0, 0, 0, 0);
    }

    pdc_trace_exit_api(p->pdc, pdc_true, "\n");
}

PDFLIB_API void PDFLIB_CALL
PDF_mc_point(PDF *p, const char *tag, const char *optlist)
{
    static const char fn[] = "PDF_mc_point";

    if (pdf_enter_api(p, fn, pdf_state_content,
        "(p[%p], \"%s\", \"%T\")", (void *) p, tag ? tag : "", optlist, 0))
    {
        pdc_error(p->pdc, PDF_E_UNSUPP_MC, 0, 0, 0, 0);
    }

    pdc_trace_exit_api(p->pdc, pdc_true, "\n");
}

PDFLIB_API void PDFLIB_CALL
PDF_activate_item(PDF *p, int id)
{
    static const char fn[] = "PDF_activate_item";

    if (pdf_enter_api(p, fn, (pdf_state) (pdf_state_document | pdf_state_page),
        "(p[%p], %d)", (void *) p, id))
    {
        pdc_error(p->pdc, PDF_E_UNSUPP_TAGGED, 0, 0, 0, 0);
    }

    pdc_trace_exit_api(p->pdc, pdc_true, "\n");
}

PDFLIB_API int PDFLIB_CALL
PDF_begin_item(
    PDF *p,
    const char *tag,
    const char *optlist)
{
    static const char fn[] = "PDF_begin_item";
    int retval = -1;

    /* further check in kernel function */
    if (pdf_enter_api(p, fn, (pdf_state) (pdf_state_document | pdf_state_page),
        "(p[%p], \"%s\", \"%T\")", (void *) p, tag ? tag : "", optlist, 0))
    {
        pdc_error(p->pdc, PDF_E_UNSUPP_TAGGED, 0, 0, 0, 0);
    }

    pdc_trace_exit_api(p->pdc, pdc_true, "[%d]\n", retval);
    return retval;
}

PDFLIB_API void PDFLIB_CALL
PDF_end_item(PDF *p, int id)
{
    static const char fn[] = "PDF_end_item";

    /* further check in kernel function */
    if (pdf_enter_api(p, fn, (pdf_state) (pdf_state_document | pdf_state_page),
        "(p[%p], %d)", (void *) p, id))
    {
        pdc_error(p->pdc, PDF_E_UNSUPP_TAGGED, 0, 0, 0, 0);
    }

    pdc_trace_exit_api(p->pdc, pdc_true, "\n");
}


/**********************
 *
 *  p_template.c
 *
 **********************/

PDFLIB_API int PDFLIB_CALL
PDF_begin_template(PDF *p, double width, double height)
{
    static const char fn[] = "PDF_begin_template";
    int retval = -1;

    if (pdf_enter_api(p, fn, (pdf_state) (pdf_state_document | pdf_state_page),
        "(p[%p], %f, %f)", (void *) p, width, height))
    {
        retval = pdf__begin_template(p, width, height, "");
    }

    return pdf_exit_handle_api(p, retval);
}

PDFLIB_API int PDFLIB_CALL
PDF_begin_template2(PDF *p, double width, double height, const char *optlist)
{
    static const char fn[] = "PDF_begin_template2";
    int retval = -1;

    if (pdf_enter_api(p, fn, pdf_state_document, "(p[%p], %f, %f, \"%T\")",
        (void *) p, width, height, optlist, 0))
    {
        retval = pdf__begin_template(p, width, height, optlist);
    }

    return pdf_exit_handle_api(p, retval);
}

PDFLIB_API void PDFLIB_CALL
PDF_end_template(PDF *p)
{
    static const char fn[] = "PDF_end_template";

    if (pdf_enter_api(p, fn, pdf_state_template, "(p[%p])", (void *) p))
    {
        pdf__end_template(p);
    }

    pdc_trace_exit_api(p->pdc, pdc_false, "\n");
}


/**********************
 *
 *  p_text.c
 *
 **********************/

PDFLIB_API void PDFLIB_CALL
PDF_continue_text(PDF *p, const char *text)
{
    static const char fn[] = "PDF_continue_text";
    if (pdf_enter_api(p, fn, pdf_state_content, "(p[%p], \"%T\")",
        (void *) p, text, 0))
    {
        int len = text ? (int) strlen(text) : 0;
        pdf__show_text(p, text, len, pdc_true);
    }

    pdc_trace_exit_api(p->pdc, pdc_true, "\n");
}

PDFLIB_API void PDFLIB_CALL
PDF_continue_text2(PDF *p, const char *text, int len)
{
    static const char fn[] = "PDF_continue_text2";
    if (pdf_enter_api(p, fn, pdf_state_content,
        "(p[%p], \"%T\", /*c*/%d)", (void *) p, text, len, len))
    {
        pdf__show_text(p, text, len, pdc_true);
    }

    pdc_trace_exit_api(p->pdc, pdc_true, "\n");
}

PDFLIB_API void PDFLIB_CALL
PDF_fit_textline(PDF *p, const char *text, int len, double x, double y,
                 const char *optlist)
{
    static const char fn[] = "PDF_fit_textline";

    if (pdf_enter_api(p, fn, pdf_state_content,
        "(p[%p], \"%T\", /*c*/%d, %f, %f, \"%T\")",
        (void *) p, text, len, len, x, y, optlist, 0))
    {
        pdf__fit_textline(p, text, len, x, y, optlist);
    }

    pdc_trace_exit_api(p->pdc, pdc_true, "\n");
}

PDFLIB_API void PDFLIB_CALL
PDF_setfont(PDF *p, int font, double fontsize)
{
    static const char fn[] = "PDF_setfont";

    if (pdf_enter_api(p, fn, pdf_state_content, "(p[%p], %d, %f)",
        (void *) p, font, fontsize))
    {
        if (p->hastobepos) font -= 1;
        pdf__setfont(p, font, fontsize);
    }

    pdc_trace_exit_api(p->pdc, pdc_false, "\n");
}

PDFLIB_API void PDFLIB_CALL
PDF_set_text_pos(PDF *p, double x, double y)
{
    static const char fn[] = "PDF_set_text_pos";

    if (pdf_enter_api(p, fn, pdf_state_content, "(p[%p], %f, %f)",
        (void *) p, x, y))
    {
        pdf__set_text_pos(p, x, y);
    }

    pdc_trace_exit_api(p->pdc, pdc_false, "\n");
}

PDFLIB_API void PDFLIB_CALL
PDF_show(PDF *p, const char *text)
{
    static const char fn[] = "PDF_show";
    if (pdf_enter_api(p, fn, pdf_state_content, "(p[%p], \"%T\")",
        (void *) p, text, 0))
    {
        int len = text ? (int) strlen(text) : 0;
        pdf__show_text(p, text, len, pdc_false);
    }

    pdc_trace_exit_api(p->pdc, pdc_true, "\n");
}

PDFLIB_API void PDFLIB_CALL
PDF_show2(PDF *p, const char *text, int len)
{
    static const char fn[] = "PDF_show2";
    if (pdf_enter_api(p, fn, pdf_state_content,
        "(p[%p], \"%T\", /*c*/%d)", (void *) p, text, len, len))
    {
        pdf__show_text(p, text, len, pdc_false);
    }

    pdc_trace_exit_api(p->pdc, pdc_true, "\n");
}

PDFLIB_API int PDFLIB_CALL
PDF_show_boxed(
    PDF *p,
    const char *text,
    double left,
    double bottom,
    double width,
    double height,
    const char *hmode,
    const char *feature)
{
    static const char fn[] = "PDF_show_boxed";
    int retval = 0;

    if (pdf_enter_api(p, fn, pdf_state_content,
        "(p[%p], \"%T\", %f, %f, %f, %f, \"%s\", \"%s\")",
        (void *) p, text, 0, left, bottom, width, height, hmode ? hmode : "",
        feature ? feature : ""))
    {
        retval = pdf__show_boxed(p, text, 0, left, bottom, width, height,
                                 hmode, feature);
    }

    pdc_trace_exit_api(p->pdc, pdc_true, "[%d]\n", retval);
    return retval;
}

PDFLIB_API int PDFLIB_CALL
PDF_show_boxed2(
    PDF *p,
    const char *text,
    int len,
    double left,
    double bottom,
    double width,
    double height,
    const char *hmode,
    const char *feature)
{
    static const char fn[] = "PDF_show_boxed2";
    int retval = 0;

    if (pdf_enter_api(p, fn, pdf_state_content,
        "(p[%p], \"%T\", /*c*/%d, %f, %f, %f, %f, \"%s\", \"%s\")",
        (void *) p, text, len, len, left, bottom, width, height,
        hmode ? hmode : "", feature ? feature : ""))
    {
        retval = pdf__show_boxed(p, text, len, left, bottom, width, height,
                                   hmode, feature);
    }

    pdc_trace_exit_api(p->pdc, pdc_true, "[%d]\n", retval);
    return retval;
}

PDFLIB_API void PDFLIB_CALL
PDF_show_xy(PDF *p, const char *text, double x, double y)
{
    static const char fn[] = "PDF_show_xy";
    if (pdf_enter_api(p, fn, pdf_state_content, "(p[%p], \"%T\", %f, %f)",
        (void *) p, text, 0, x, y))
    {
        int len = text ? (int) strlen(text) : 0;
        pdf__set_text_pos(p, x, y);
        pdf__show_text(p, text, len, pdc_false);
    }

    pdc_trace_exit_api(p->pdc, pdc_true, "\n");
}

PDFLIB_API void PDFLIB_CALL
PDF_show_xy2(PDF *p, const char *text, int len, double x, double y)
{
    static const char fn[] = "PDF_show_xy2";
    if (pdf_enter_api(p, fn, pdf_state_content,
        "(p[%p], \"%T\", /*c*/%d, %f, %f)",
        (void *) p, text, len, len, x, y))
    {
        pdf__set_text_pos(p, x, y);
        pdf__show_text(p, text, len, pdc_false);
    }

    pdc_trace_exit_api(p->pdc, pdc_true, "\n");
}

PDFLIB_API double PDFLIB_CALL
PDF_stringwidth(PDF *p, const char *text, int font, double fontsize)
{
    static const char fn[] = "PDF_stringwidth";
    double retval = 0;

    if (pdf_enter_api(p, fn,
        (pdf_state) (pdf_state_document | pdf_state_content | pdf_state_path),
        "(p[%p], \"%T\", %d, %f)",
        (void *) p, text, 0, font, fontsize))
    {
        int len = text ? (int) strlen(text) : 0;
        if (p->hastobepos) font -= 1;
        retval = pdf__stringwidth(p, text, len, font, fontsize);
    }

    pdc_trace_exit_api(p->pdc, pdc_true, "[%f]\n", retval);
    return retval;
}

PDFLIB_API double PDFLIB_CALL
PDF_stringwidth2(PDF *p, const char *text, int len, int font, double fontsize)
{
    static const char fn[] = "PDF_stringwidth2";
    double retval = 0;

    if (pdf_enter_api(p, fn,
        (pdf_state) (pdf_state_document | pdf_state_content | pdf_state_path),
        "(p[%p], \"%T\", /*c*/%d, %d, %f)",
        (void *) p, text, len, len, font, fontsize))
    {
        if (p->hastobepos) font -= 1;
        retval = pdf__stringwidth(p, text, len, font, fontsize);
    }

    pdc_trace_exit_api(p->pdc, pdc_true, "[%f]\n", retval);
    return retval;
}

PDFLIB_API void PDFLIB_CALL
PDF_xshow(PDF *p, const char *text, int len, const double *xadvancelist)
{
    static const char fn[] = "PDF_xshow";
    if (pdf_enter_api(p, fn, pdf_state_content,
        "(p[%p], \"%T\", %d, %p)", (void *) p, text, len, len, xadvancelist))
    {
        pdf__xshow(p, text, len, xadvancelist);
    }

    pdc_trace_exit_api(p->pdc, pdc_true, "\n");
}


/**********************
 *
 *  p_textflow.c
 *
 **********************/

PDFLIB_API int PDFLIB_CALL
PDF_create_textflow(PDF *p, const char *text, int len, const char *optlist)
{
    static const char fn[] = "PDF_create_textflow";
    int retval = -1;

    if (pdf_enter_api(p, fn, pdf_state_all,
        "(p[%p], \"%T\", /*c*/%d, \"%s\")",
        (void *) p, text, len, len, optlist, 0))
    {
        pdc_error(p->pdc, PDF_E_UNSUPP_TEXTFLOWS, 0, 0, 0, 0);
    }

    return pdf_exit_handle_api(p, retval);
}

PDFLIB_API void PDFLIB_CALL
PDF_delete_textflow(PDF *p, int textflow)
{
    static const char fn[] = "PDF_delete_textflow";

    if (pdf_enter_api(p, fn, pdf_state_all,
        "(p[%p], %d)", (void *) p, textflow))
    {
        pdc_error(p->pdc, PDF_E_UNSUPP_TEXTFLOWS, 0, 0, 0, 0);
    }

    pdc_trace_exit_api(p->pdc, pdc_true, "\n");
}

PDFLIB_API const char * PDFLIB_CALL
PDF_fit_textflow(
    PDF *p,
    int textflow,
    double llx,
    double lly,
    double urx,
    double ury,
    const char *optlist)
{
    static const char fn[] = "PDF_fit_textflow";
    const char *retval = "";

    if (pdf_enter_api(p, fn, pdf_state_content,
        "(p[%p], %d, %f, %f, %f, %f, \"%T\")",
        (void *) p, textflow, llx, lly, urx, ury, optlist, 0))
    {
        pdc_error(p->pdc, PDF_E_UNSUPP_TEXTFLOWS, 0, 0, 0, 0);
    }

    pdc_trace_exit_api(p->pdc, pdc_true, "[\"%s\"]\n", retval);
    return retval;
}

PDFLIB_API double PDFLIB_CALL
PDF_info_textflow(PDF *p, int textflow, const char *keyword)
{
    static const char fn[] = "PDF_info_textflow";
    double retval = 0;

    if (pdf_enter_api(p, fn, pdf_state_all,
        "(p[%p], %d, \"%s\")", (void *) p, textflow, keyword))
    {
        pdc_error(p->pdc, PDF_E_UNSUPP_TEXTFLOWS, 0, 0, 0, 0);
    }

    pdc_trace_exit_api(p->pdc, pdc_true, "[%f]\n", retval);
    return retval;
}


/**********************
 *
 *  p_type3.c
 *
 **********************/

PDFLIB_API void PDFLIB_CALL
PDF_begin_font(
    PDF *p,
    const char *fontname, int len,
    double a, double b, double c, double d, double e, double f,
    const char *optlist)
{
    static const char fn[] = "PDF_begin_font";

    if (pdf_enter_api(p, fn, (pdf_state) (pdf_state_document | pdf_state_page),
        "(p[%p], \"%T\", /*c*/%d, %f, %f, %f, %f, %f, %f, \"%T\")",
        (void *) p, fontname, len, len, a, b, c, d, e, f, optlist, 0))
    {
        pdf__begin_font(p, fontname, len, a, b, c, d, e, f, optlist);
    }

    pdc_trace_exit_api(p->pdc, pdc_true, "\n");
}

PDFLIB_API void PDFLIB_CALL
PDF_begin_glyph(
    PDF *p,
    const char *glyphname,
    double wx, double llx, double lly, double urx, double ury)
{
    static const char fn[] = "PDF_begin_glyph";

    if (pdf_enter_api(p, fn, pdf_state_font,
        "(p[%p], \"%s\", %f, %f, %f, %f, %f)",
        (void *) p, glyphname, wx, llx, lly, urx, ury))
    {
        pdf__begin_glyph(p, glyphname, wx, llx, lly, urx, ury);
    }

    pdc_trace_exit_api(p->pdc, pdc_true, "\n");
}

PDFLIB_API void PDFLIB_CALL
PDF_end_font(PDF *p)
{
    static const char fn[] = "PDF_end_font";

    if (pdf_enter_api(p, fn, pdf_state_font, "(p[%p])", (void *) p))
    {
        pdf__end_font(p);
    }

    pdc_trace_exit_api(p->pdc, pdc_true, "\n");
}


PDFLIB_API void PDFLIB_CALL
PDF_end_glyph(PDF *p)
{
    static const char fn[] = "PDF_end_glyph";

    if (pdf_enter_api(p, fn, pdf_state_glyph, "(p[%p])", (void *) p))
    {
        pdf__end_glyph(p);
    }

    pdc_trace_exit_api(p->pdc, pdc_true, "\n");
}


/**********************
 *
 *  p_util.c
 *
 **********************/

PDFLIB_API const char * PDFLIB_CALL
PDF_utf16_to_utf8(PDF *p, const char *utf16string, int len, int *size)
{
    static const char fn[] = "PDF_utf16_to_utf8";
    const char *retval = "";

    if (p != NULL && p->magic != PDC_MAGIC && p->textformat == pdc_auto2)
    {
        retval = pdf__utf16_to_utf8(p, utf16string, len, size);
    }
    else
    {
        pdc_trace_protocol(p->pdc, 1, trc_api, "/* ");
        if (pdf_enter_api(p, fn, pdf_state_all,
            size ? "(p[%p], \"%T\", %d, &size[%p])" :
                   "(p[%p], \"%s\", %d, NULL)",
            (void *) p, utf16string, len, len, (void *) size))
        {
            retval = pdf__utf16_to_utf8(p, utf16string, len, size);
        }

        pdc_trace_exit_api(p->pdc, pdc_false, "[\"%T\", size=%d] */\n",
                           retval, 0, size ? *size : 0);
    }
    return retval;
}

PDFLIB_API const char * PDFLIB_CALL
PDF_utf8_to_utf16(PDF *p, const char *utf8string, const char *format,
   int *size)
{
    static const char fn[] = "PDF_utf8_to_utf16";
    const char *retval = "";

    if (p != NULL && p->magic != PDC_MAGIC && p->textformat == pdc_auto2)
    {
        retval = pdf__utf8_to_utf16(p, utf8string, format, size);
    }
    else
    {
        if (size == NULL)
            pdc_error(p->pdc, PDC_E_ILLARG_EMPTY, "size", 0, 0, 0);

        pdc_trace_protocol(p->pdc, 1, trc_api, "/* ");
        if (pdf_enter_api(p, fn, pdf_state_all,
            "(p[%p], \"%T\", \"%s\", &size[%p])",
            (void *) p, utf8string, 0, format, (void *) size))
        {
            retval = pdf__utf8_to_utf16(p, utf8string, format, size);
        }

        pdc_trace_exit_api(p->pdc, pdc_false, "[\"%T\", size=%d] */\n",
                           retval, *size, *size);
    }
    return retval;
}


/**********************
 *
 *  p_xgstate.c
 *
 **********************/

PDFLIB_API int PDFLIB_CALL
PDF_create_gstate(PDF *p, const char *optlist)
{
    static const char fn[] = "PDF_create_gstate";
    int retval = -1;

    if (pdf_enter_api(p, fn,
        (pdf_state) (pdf_state_document | pdf_state_content),
        "(p[%p], \"%T\")", (void *) p, optlist, 0))
    {
        retval = pdf__create_gstate(p, optlist);
    }

    return pdf_exit_handle_api(p, retval);
}

PDFLIB_API void PDFLIB_CALL
PDF_set_gstate(PDF *p, int gstate)
{
    static const char fn[] = "PDF_set_gstate";

    if (pdf_enter_api(p, fn, pdf_state_content, "(p[%p], %d)",
        (void *) p, gstate))
    {
        if (p->hastobepos) gstate -= 1;
        pdf__set_gstate(p, gstate);
    }

    pdc_trace_exit_api(p->pdc, pdc_true, "\n");
}



