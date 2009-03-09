
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
	   -I$(BOOST_INCLUDE_DIR)

nobase_include_HEADERS = \
	satire/termite.h \
	satire/TermPrinter.h \
	\
	satire/TermToRose.h \
	satire/PrologTerm.h \
	satire/PrologCompTerm.h	\
	satire/PrologList.h \
	satire/PrologAtom.h \
	satire/PrologInt.h \
	satire/PrologVariable.h \
	satire/RoseToTerm.h \
	satire/RoseEnums.h

if WITH_PAG
  INCLUDES += -I$(PAG_INCLUDE_DIR)/pag \
              -I$(PAG_INCLUDE_DIR)/pag/analyzer
endif

ROSE_LDFLAGS=-L$(ROSE_LIB_DIR) -Wl,-rpath $(ROSE_LIB_DIR) -L$(BOOST_LIB_DIR) -Wl,-rpath $(BOOST_LIB_DIR)

bin_PROGRAMS = termpretty c2term term2c

AM_YFLAGS = -d

lib_LTLIBRARIES = libtermsupport.la
libtermsupport_la_SOURCES = \
	RoseToTerm.C \
	dfiparser.tab.c++ dfiparser.tab.h++ dfiscanner.c++ \
	TermToRose.C PrologTerm.C RoseEnums.C

CLEANFILES = termpretty.c++ \
	     dfiparser.c++ dfiparser.h dfiscanner.c++ \
             dfiparser.tab.c++ dfiparser.tab.h++

if WITH_SWI_PROLOG
INCLUDES += -I$(SWI_PROLOG_INCLUDE_DIR) 
ROSE_LDFLAGS += $(SWI_PROLOG_LDADD)
nobase_dist_data_DATA =  termite/ast_properties.pl  termite/loops.pl \
                         termite/ast_transform.pl   termite/utils.pl \
                         termite/ast_walk.pl
else	
CLEANFILES += termparser.c++ termparser.h termlexer.c++ termparser.tab.c++ \
              termparser.tab.h++
BUILT_SOURCES = termparser.tab.c++ termparser.tab.h++ \
                dfiparser.tab.c++ dfiparser.tab.h++
libtermsupport_la_SOURCES += termparser.y++ termlexer.l++
nobase_include_HEADERS += satire/termparser.h

termparser.tab.c++: $(srcdir)/termparser.y++ Makefile
	$(YACC) -d $(srcdir)/termparser.y++ && \
	mv y.tab.c termparser.tab.c++ && \
	mv y.tab.h termparser.tab.h++

# dummy dependendcy for make -j
termparser.tab.h++: termparser.tab.c++

endif

dfiparser.tab.c++: $(srcdir)/dfiparser.y++ Makefile
	bison -p dfi -d $(srcdir)/dfiparser.y++

dfiparser.tab.h++: dfiparser.tab.c++

# need to do this explicitly to replace the "yy_" prefix with "dfi_"
dfiscanner.c++: $(srcdir)/dfiscanner.l++ Makefile
	$(LEX) -Pdfi $(srcdir)/dfiscanner.l++ && \
	mv lex.dfi.c dfiscanner.c++


termpretty_SOURCES = termpretty.l++

c2term_SOURCES = c2term.C 
term2c_SOURCES = term2c.C 

term2c_LDADD = libtermsupport.la -lrose 
c2term_LDADD = libtermsupport.la -lrose

term2c_LDFLAGS = $(ROSE_LDFLAGS)
c2term_LDFLAGS = $(ROSE_LDFLAGS)

EXTRA_DIST = \
  README doc tests \
  doc/bonitz_manual.pdf doc/thesis_bonitz.ps \
  \
  tests/test1.C tests/test4.C tests/test6.C tests/test9.C tests/test_control.C \
  tests/test_struct.C tests/test2.C tests/test5b.C tests/test7.C \
  tests/test_class1.C tests/test_enum.C tests/test_transformme.C tests/test3.C \
  tests/test5.C tests/test8.C tests/test_class2.C tests/test_minimal.C \
  tests/test_typedef.C \
  \
  term2c \
  dfiparser.y++ dfiscanner.l++ \
  clang/README clang/INSTALL clang/BUILD-clang.sh clang/clang-termite.patch

