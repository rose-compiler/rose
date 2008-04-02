# Main PDFlib Makefile
# $Id: Makefile,v 1.2 2006/09/08 11:39:28 dquinlan Exp $

top_builddir=.

include config/mkcommon.inc

SUB_DIRS = libs bind progs $(PTFDIR)

include config/mksubdirs.inc

# --------------------------- check ---------------------------------
# RV 9/13/2005 -- Dummy rule
check: all
	:

# --------------------------- install ---------------------------------
install:: all .dummy
	mkdir -p $(bindir)
	-$(INSTALL) pdflib-config $(bindir)

# RV 9/13/2005 -- Dummy rule
installcheck:
	:

# ----------------------- distclean ---------------------------------
# RV 9/13/2005 -- Added new distribution rules
#
# These rules allow the PDFlib to look like a conventional
# autoconf+automake package.
#

# Creating a distribution from the standalone package
PACKAGE=PDFlib-Lite-$(VERSION)rv
dist:
	distdir=$(top_builddir)/$(PACKAGE) ; \
		test -d $$distdir && $(RM) $$distdir ; \
		mkdir -p $$distdir && ./makedist.sh $$distdir
	$(MAKE) dist-gzip

dist-gzip:
	distdir=$(top_builddir)/$(PACKAGE) ; \
		test -d $$distdir && \
			tar cvhf - $$distdir | gzip -9c > $(PACKAGE).tar.gz

dist-bzip2:
	distdir=$(top_builddir)/$(PACKAGE) ; \
		test -d $$distdir && \
			tar cvhf - $$distdir | bzip2 -9c > $(PACKAGE).tar.bz2

distclean:: .dummy
	-$(RM) libtool pdflib-config \
		config.cache config.log config.status \
		config/mkcommon.inc autom4te.cache
	for f in `cat distfiles.txt` ; do \
		$(RM) $$f ; \
	done
	OBJS=`find bind/pdflib/. -name "*.o"` ; \
		 if test -n "$$OBJS" ; then \
		 	for f in $$OBJS ; do \
				$(RM) $$f ; \
			done ; \
		 fi

distcheck:
	:

# Copy distribution files when called from a recursive 'make dist'
distdir:
	test -d $(distdir)
	(cd $(top_builddir) ; ./makedist.sh $(distdir))

# Why is this needed?
dvi:
	:

# eof
