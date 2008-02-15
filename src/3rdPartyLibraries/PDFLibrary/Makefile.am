# This should not depend on ROSE in any way so I don't think we need this
include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

## Specifications for building the pdf library.
libpdfSources = \
   p_afm.c p_color.c p_gif.c p_jpeg.c p_text.c \
   p_annots.c p_draw.c p_gstate.c p_pfm.c p_tiff.c \
   p_basic.c p_filter.c p_hyper.c p_png.c p_util.c \
   p_ccitt.c p_font.c p_image.c p_stream.c

# lib_LIBRARIES = libpdf.a
# libpdf_a_SOURCES  = $(libpdfSources)

lib_LIBRARIES = libpdf.so
libpdf_so_SOURCES = $(libpdfSources)

libpdf.a: $(libpdf_a_OBJECTS)
	-rm -f $@
	$(CXX_STATIC_LIB_UPDATE) $@ $(libpdf_a_OBJECTS)
	test -f $@ || exit 1

libpdf.so: $(libpdf_so_OBJECTS)
	-rm -f $@
	$(CXX_SHARED_LIB_UPDATE) $@ $(libpdf_so_OBJECTS)
	test -f $@ || exit 1

noinst_HEADERS = \
   e_ansi.h    e_macroman.h  p_config.h  p_intern.h   pdflib.h \
   e_ebcdic.h  p_cid.h       p_font.h    p_metrics.h  resource.h

include_HEADERS = pdflib.h


