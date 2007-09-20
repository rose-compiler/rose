## Process this file with automake to create Makefile.in.

INCLUDES = -I$(top_srcdir)/src/termite/satire \
	   -I$(top_srcdir)/src/analyzer/annotation/satire \
	   -I$(top_srcdir)/src/analyzer/icfgbuilder/satire \
	   -I$(top_srcdir)/src/analyzer/astaccess/satire \
	   -I$(ROSE_INCLUDE_DIR) \
	   -I$(PAG_INCLUDE_DIR)/pag \
           -I$(PAG_INCLUDE_DIR)/pag/analyzer

AM_CXXFLAGS=-I$(ROSE_INCLUDE_DIR)
AM_LDFLAGS=-L$(ROSE_LIB_DIR)

bin_PROGRAMS = termpretty rose2term term2rose
bin_SCRIPTS = c2term term2c

BUILT_SOURCES = termparser.tab.c++ termparser.tab.h++
AM_YFLAGS = -d

lib_LIBRARIES = libtermite.a
libtermite_a_SOURCES = \
	PrologVariable.cpp PrologList.cpp PrologAtom.cpp PrologString.cpp \
	PrologInt.cpp PrologCompTerm.cpp PrologTerm.cpp PrologSupport.C \
	\
	termparser.y++ termparser.tab.h++ termlexer.l++ \
	PrologToRose.C \
	PrologVariable.cpp PrologList.cpp PrologAtom.cpp PrologString.cpp \
	PrologInt.cpp PrologCompTerm.cpp PrologTerm.cpp PrologSupport.C 


termparser.tab.h++ termparser.tab.c++: $(srcdir)/termparser.y++ 
	$(YACC) -d $(srcdir)/termparser.y++
	mv y.tab.c termparser.tab.c++
	mv y.tab.h termparser.tab.h++

termpretty_SOURCES = termpretty.l++

rose2term_SOURCES = toProlog.C 
term2rose_SOURCES = main.C 

term2rose_LDADD = libtermite.a -lrose -lrt
rose2term_LDADD = libtermite.a -lrose -lrt

CLEANFILES=termparser.tab.c++ termparser.tab.h++

nobase_include_HEADERS = \
	satire/PrologList.h      satire/PrologToRose.h   satire/termparser.h \
	satire/PrologAtom.h      satire/PrologString.h   satire/TermPrinter.h \
	satire/PrologCompTerm.h  satire/PrologSupport.h  satire/PrologVariable.h \
	satire/PrologInt.h       satire/PrologTerm.h     satire/TermRep.h 


EXTRA_DIST = README LICENCE doc Makefile.original tests \
	c2term term2c

