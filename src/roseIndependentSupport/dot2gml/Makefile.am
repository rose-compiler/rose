include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

AM_YFLAGS = -d

# "if" for DOT_TO_GML_TRANSLATOR (AutoMake conditional block)
if DOT_TO_GML_TRANSLATOR

bin_PROGRAMS = dot2gml

dot2gml_SOURCES = \
     main.C parseDot.ll parseDotGrammar.yy

BUILT_SOURCES = parseDotGrammar.h
DISTCLEANFILES = parseDotGrammar.h parseDotGrammar.cc parseDot.cc

# Header files that need to be included in the distribution
include_HEADERS = parse_header.h

clean-local:
	rm -f *.o *~ *.gml

# DQ (9/10/2005): This should be moved to the tests directory
#check:
#	./dot2gml $(srcdir)/onefileonly.C.dot

endif

EXTRA_DIST = CMakeLists.txt parse_header.h parseDot.ll parseDotGrammar.yy README main.C PolicyControl.txt
