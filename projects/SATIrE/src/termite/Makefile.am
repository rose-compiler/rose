
## Process this file with automake to create Makefile.in.

INCLUDES = -I$(top_srcdir)/src/satire \
	   -I$(top_srcdir)/src/termite/satire \
	   -I$(top_builddir)/src/termite/satire \
	   -I$(top_srcdir)/src/analyzer/annotation/satire \
	   -I$(top_srcdir)/src/analyzer/icfgbuilder/satire \
           -I$(top_srcdir)/src/analyzer/astaccess \
	   -I$(top_srcdir)/src/analyzer/astaccess/satire \
	   -I$(top_srcdir)/src/analyzer/astnumbering/satire \
           -I$(top_srcdir)/src/analyzer/annotation/satire \
	   -I$(top_srcdir)/src/analyzer/commandlineinterface/satire \
	   -I$(top_srcdir)/src/analyzer/support \
	   -I$(top_srcdir)/src/analyzer/support/satire \
	   -I$(ROSE_INCLUDE_DIR) \
	   -I$(BOOST_INCLUDE_DIR) \
	   -I$(PAG_INCLUDE_DIR)/pag \
           -I$(PAG_INCLUDE_DIR)/pag/analyzer

AM_LDFLAGS=-L$(ROSE_LIB_DIR) -Wl,-rpath $(ROSE_LIB_DIR) -L$(BOOST_LIB_DIR) -Wl,-rpath $(BOOST_LIB_DIR) 

bin_PROGRAMS = termpretty rose2term term2rose
bin_SCRIPTS = c2term term2c

AM_YFLAGS = -d

lib_LIBRARIES = libtermite.a
libtermite_a_SOURCES = \
	PrologSupport.C \
	dfiparser.tab.c++ dfiparser.tab.h++ dfiscanner.c++ \
	PrologToRose.C PrologTerm.C

termparser.tab.c++: $(srcdir)/termparser.y++ Makefile
	$(YACC) -d $(srcdir)/termparser.y++ && \
	mv y.tab.c termparser.tab.c++ && \
	mv y.tab.h termparser.tab.h++

dfiparser.tab.c++: $(srcdir)/dfiparser.y++ Makefile
	bison -p dfi -d $(srcdir)/dfiparser.y++

# dummy dependendcy for make -j
termparser.tab.h++: termparser.tab.c++
dfiparser.tab.h++: dfiparser.tab.c++

dfiscanner.c++: $(srcdir)/dfiscanner.l++ Makefile
	flex -Pdfi $(srcdir)/dfiscanner.l++ && \
	mv lex.dfi.c dfiscanner.c++


termpretty_SOURCES = termpretty.l++

rose2term_SOURCES = toProlog.C 
term2rose_SOURCES = toC.C 

term2rose_LDADD = libtermite.a -lrose
rose2term_LDADD = libtermite.a -lrose

CLEANFILES = termparser.c++ termparser.h termlexer.c++ termpretty.c++ \
	     termparser.tab.c++ termparser.tab.h++ \
	     dfiparser.c++ dfiparser.h dfiscanner.c++ \
             dfiparser.tab.c++ dfiparser.tab.h++

nobase_include_HEADERS = \
	satire/termite.h \
	satire/TermPrinter.h \
	\
	satire/PrologToRose.h \
	satire/termparser.h \
	satire/PrologTerm.h \
	satire/PrologCompTerm.h	\
	satire/PrologList.h \
	satire/PrologAtom.h \
	satire/PrologInt.h \
	satire/PrologVariable.h \
	satire/PrologSupport.h

if WITH_SWI_PROLOG
  INCLUDES += -I$(SWI_PROLOG_INCLUDE_DIR) 
  AM_LDFLAGS += $(SWI_PROLOG_LDADD)

else
  BUILT_SOURCES = termparser.tab.c++ termparser.tab.h++ \
                  dfiparser.tab.c++ dfiparser.tab.h++
  libtermite_a_SOURCES += termparser.y++ termparser.tab.h++ termlexer.l++
  nobase_include_HEADERS += satire/termparser.h 
endif

EXTRA_DIST = \
  README doc tests \
  c2term term2c \
  dfiparser.y++ dfiparser.tab.h++ dfiscanner.l++ \
  clang/README clang/INSTALL clang/BUILD-clang.sh clang/clang-termite.patch

