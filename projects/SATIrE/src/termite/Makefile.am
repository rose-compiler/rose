## Process this file with automake to create Makefile.in.

INCLUDES = -I$(top_srcdir)/src/satire \
	   -I$(top_srcdir)/src/termite/satire \
	   -I$(top_srcdir)/src/analyzer/annotation/satire \
	   -I$(top_srcdir)/src/analyzer/icfgbuilder/satire \
	   -I$(top_srcdir)/src/analyzer/astaccess/satire \
	   -I$(top_srcdir)/src/analyzer/astnumbering/satire \
	   -I$(top_srcdir)/src/analyzer/commandlineinterface/satire \
	   -I$(ROSE_INCLUDE_DIR) \
	   -I$(PAG_INCLUDE_DIR)/pag \
           -I$(PAG_INCLUDE_DIR)/pag/analyzer

AM_LDFLAGS=-L$(ROSE_LIB_DIR) -Wl,-rpath $(ROSE_LIB_DIR)

bin_PROGRAMS = termpretty rose2term term2rose
bin_SCRIPTS = c2term term2c

BUILT_SOURCES = termparser.tab.c++ termparser.tab.h++ \
                dfiparser.tab.c++ dfiparser.tab.h++
AM_YFLAGS = -d

lib_LIBRARIES = libtermite.a
libtermite_a_SOURCES = \
	PrologSupport.C \
	\
	termparser.y++ termparser.tab.h++ termlexer.l++ \
	dfiparser.tab.c++ dfiparser.tab.h++ dfiscanner.c++ \
	PrologToRose.C

termparser.tab.h++ termparser.tab.c++: $(srcdir)/termparser.y++ 
	$(YACC) -d $(srcdir)/termparser.y++
	mv y.tab.c termparser.tab.c++
	mv y.tab.h termparser.tab.h++

dfiparser.tab.h++ dfiparser.tab.c++: $(srcdir)/dfiparser.y++ 
	bison -p dfi -d $(srcdir)/dfiparser.y++

dfiscanner.c++: $(srcdir)/dfiscanner.l++ 
	flex -Pdfi $(srcdir)/dfiscanner.l++
	mv lex.dfi.c dfiscanner.c++


termpretty_SOURCES = termpretty.l++

rose2term_SOURCES = toProlog.C 
term2rose_SOURCES = toC.C 

term2rose_LDADD = libtermite.a -lrose -lrt
rose2term_LDADD = libtermite.a -lrose -lrt

CLEANFILES = termparser.c++ termparser.h termlexer.c++ termpretty.c++ \
	     termparser.tab.c++ termparser.tab.h++ \
	     dfiparser.c++ dfiparser.h dfiscanner.c++ \
             dfiparser.tab.c++ dfiparser.tab.h++

nobase_include_HEADERS = \
	satire/PrologList.h      satire/PrologToRose.h   satire/termparser.h \
	satire/PrologAtom.h      satire/PrologString.h   satire/TermPrinter.h \
	satire/PrologCompTerm.h  satire/PrologSupport.h  satire/PrologVariable.h \
	satire/PrologInt.h       satire/PrologTerm.h     satire/TermRep.h 


EXTRA_DIST = README LICENCE doc Makefile.original tests \
             c2term term2c \
             dfiparser.y++ dfiparser.tab.h++ dfiscanner.l++

