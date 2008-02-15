# Makefile for PDFlib's Perl binding
# $Id: Makefile,v 1.1 2005/10/11 17:18:09 vuduc2 Exp $

top_builddir	= ../../..

include $(top_builddir)/config/mkcommon.inc

INCLUDES	= $(PDFLIBINC) $(PERLBOOLDEF) $(PERLINCLUDE)
# special CFLAGS, as -ansi is not working here.
CFLAGS          = $(DEFS) $(DEFINES) $(INCLUDES)

PERLBIN		= $(PERL) -I. -I.libs
LANG_LIBDIR 	= $(PERLLIBDIR)
BINDEXTERNALLIBS= $(PERLLINK)

LIBNAME		= pdflib_pl$(LA)
OBJ		= pdflib_pl$(LO)
SRC		= pdflib_pl.c

include $(top_builddir)/config/mkbind.inc

test:: all
	-$(PERLBIN) hello.pl
	-$(PERLBIN) image.pl
	-$(PERLBIN) pdfclock.pl
	-$(PERLBIN) chartab.pl
	-$(PERLBIN) invoice.pl
	-$(PERLBIN) businesscard.pl
	-$(PERLBIN) quickreference.pl

install:: $(LIBNAME)
	$(LIBTOOL) --mode=install \
		$(INSTALL_DATA) $(LIBNAME) $(LANG_LIBDIR)
	$(INSTALL_DATA) pdflib_pl.pm $(LANG_LIBDIR)

uninstall::
	$(LIBTOOL) --mode=uninstall \
		$(RM) $(LANG_LIBDIR)/$(LIBNAME) $(LANG_LIBDIR)/pdflib_pl.pm 

clean::
	$(RM) hello.pdf image.pdf pdfclock.pdf chartab.pdf invoice.pdf
	$(RM) businesscard.pdf quickreference.pdf
	$(RM) smoke_c*.trace smoke_c*.pdf
	$(RM) perl56 perl58 perlAS56 perlAS58

distclean::

# Automatically generated dependencies
