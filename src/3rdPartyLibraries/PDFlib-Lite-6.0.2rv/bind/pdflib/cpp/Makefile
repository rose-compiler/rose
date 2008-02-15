# Makefile for PDFlib, C++ language binding
# $Id: Makefile,v 1.1 2005/10/11 17:18:07 vuduc2 Exp $

top_builddir	= ../../..

include $(top_builddir)/config/mkcommon.inc

PDFCPEDANTIC	=
DEPLIBS		= $(PDFLIBLINK) pdflib$(LO)
LIBS		= $(DEPLIBS) $(STDCPP) $(EXTERNALLIBS)
INCLUDES	= $(PDFLIBINC)
CXXFLAGS	= $(DEFS) $(INCLUDES) $(PDFPEDANTIC) $(MYCFLAGS)
FLAGS   	= $(LDFLAGS) $(CPPFLAGS) $(CFLAGS)

# --------------------------------------------------------------------------

SRC	= \
	$(srcdir)/pdflib.cpp		\
	$(srcdir)/hello.cpp		\
	$(srcdir)/image.cpp		\
	$(srcdir)/pdfclock.cpp		\
	$(srcdir)/chartab.cpp		\
	$(srcdir)/invoice.cpp		\
	$(srcdir)/businesscard.cpp	\
	$(srcdir)/quickreference.cpp

PROGS = \
	hello$(EXE)		\
	image$(EXE)		\
	pdfclock$(EXE)		\
	chartab$(EXE)		\
	invoice$(EXE)		\
	businesscard$(EXE)	\
	quickreference$(EXE)

include $(top_builddir)/config/mkprogs.inc


test:: $(PROGS)
	-./hello
	-./image
	-./pdfclock
	-./chartab
	-./invoice
	-./businesscard
	-./quickreference

clean::
	$(RM) hello.pdf image.pdf pdfclock.pdf chartab.pdf invoice.pdf
	$(RM) businesscard.pdf quickreference.pdf


hello$(EXE): hello.cpp $(DEPLIBS)
	$(CXXPROGS_BUILD)

image$(EXE): image.cpp $(DEPLIBS)
	$(CXXPROGS_BUILD)

pdfclock$(EXE): pdfclock.cpp $(DEPLIBS)
	$(CXXPROGS_BUILD)

chartab$(EXE): chartab.cpp $(DEPLIBS)
	$(CXXPROGS_BUILD)

invoice$(EXE): invoice.cpp $(DEPLIBS)
	$(CXXPROGS_BUILD)

businesscard$(EXE): businesscard.cpp $(DEPLIBS)
	$(CXXPROGS_BUILD)

quickreference$(EXE): quickreference.cpp $(DEPLIBS)
	$(CXXPROGS_BUILD)

pdflib$(LO): ./pdflib.cpp pdflib.hpp ../../../libs/pdflib/pdflib.h
