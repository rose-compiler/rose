## LNP: FIXME: HEADER


#############################################################################
SUBDIRS 		=


#############################################################################

MAINTAINERCLEANFILES	= Makefile.in

EXTRA_DIST		=			\
			polyopt.pdf		\
			htmldoc.tar.gz

info_TEXINFOS		= polyopt.texi

# The following requires a fixed version of the Emacs 19.30 etags.
ETAGS_ARGS		= --lang=none \
 --regex='/^@node[ \t]+\([^,]+\)/\1/' $(srcdir)/polyopt.texi
TAGS_DEPENDENCIES	= polyopt.texi doxygen.cfg


DOCDIR			= htmldoc
DOXYFILE		= Doxyfile
DOXYGEN			= @DOXYGEN@

htmldoc.tar.gz:
	rm -rf $(top_srcdir)/doc/htmldoc
	mkdir -p $(top_srcdir)/doc/htmldoc
	$(DOXYGEN) $(DOXYFILE)
	tar czf htmldoc.tar.gz htmldoc

#############################################################################
