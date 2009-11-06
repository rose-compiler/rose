## Process this file with automake to create Makefile.in.

INCLUDES = -I$(top_builddir)/src/satire \
	   -I$(top_srcdir)/src/termite/satire \
	   -I$(top_builddir)/src/termite/satire \
	   -I$(top_srcdir)/src/analyzer/annotation/satire \
	   -I$(top_srcdir)/src/analyzer/icfgbuilder/satire \
           -I$(top_srcdir)/src/analyzer/astaccess \
	   -I$(top_srcdir)/src/analyzer/astaccess/satire \
	   -I$(top_srcdir)/src/analyzer/astnumbering/satire \
           -I$(top_srcdir)/src/analyzer/annotation/satire \
	   -I$(top_srcdir)/src/analyzer/commandlineinterface/satire \
	   -I$(top_srcdir)/src/analyzer/provided/pointsto/satire \
	   -I$(top_srcdir)/src/analyzer/provided/aslan/satire \
	   -I$(top_srcdir)/src/analyzer/support \
	   -I$(top_srcdir)/src/analyzer/support/satire \
       -I$(top_srcdir)/src/aral/src/satire \
	   -I$(ROSE_INCLUDE_DIR) \
	   -I$(BOOST_INCLUDE_DIR)

nobase_include_HEADERS = \
	satire/AstJanitor.h \
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

ROSELDFLAGS=-L$(ROSE_LIB_DIR) -Wl,-rpath $(ROSE_LIB_DIR) -L$(BOOST_LIB_DIR) -Wl,-rpath $(BOOST_LIB_DIR)

bin_PROGRAMS = termpretty c2term term2c

if WITH_SWI_PROLOG
bin_PROGRAMS += termite_lint

termite_lint_SOURCES =

termite_lint: term_lint_main.pl termite/termite_spec.pl termite/termlint.pl
	TERMITE_LIB=$(srcdir)/termite \
	  $(SWI_PLC) -o termite_lint -c $(srcdir)/term_lint_main.pl
endif

AM_YFLAGS = -d

lib_LTLIBRARIES = libtermsupport.la
libtermsupport_la_SOURCES = \
	RoseToTerm.C \
	TermToRose.C PrologTerm.C RoseEnums.C
nodist_libtermsupport_la_SOURCES = \
	dfiparser.tab.c++ dfiparser.tab.h++ dfiscanner.c++

CLEANFILES = termpretty.c++ \
	     dfiparser.c++ dfiparser.h dfiscanner.c++ \
             dfiparser.tab.c++ dfiparser.tab.h++

BUILT_SOURCES =	#dfiparser.tab.c++ dfiparser.tab.h++ 

if WITH_SWI_PROLOG # ---------------------------------------------------

INCLUDES += -I$(SWI_PROLOG_INCLUDE_DIR) 
ROSELDFLAGS += $(SWI_PROLOG_LIBS) $(SWI_PROLOG_LDADD)
nobase_dist_data_DATA = \
  termite/astproperties.pl  termite/loops.pl \
  termite/asttransform.pl   termite/utils.pl \
  termite/astwalk.pl        termite/callgraph.pl \
  termite/termlint.pl       termite/termite_spec.pl 

else # -----------------------------------------------------------------

BUILT_SOURCES += termparser.tab.h++ 

nodist_libtermsupport_la_SOURCES += $(BUILT_SOURCES)
CLEANFILES += termparser.c++ termparser.h termlexer.c++ \
              termparser.tab.h++
libtermsupport_la_SOURCES += termparser.y++ termlexer.l++
nobase_include_HEADERS += satire/termparser.h

.PHONY: $(srcdir)/TermToRose.C
$(srcdir)/TermToRose.C: termparser.tab.h++

termparser.tab.h++: $(srcdir)/termparser.y++ Makefile
	$(YACC) -d $(srcdir)/termparser.y++ && \
	rm y.tab.c && \
	mv y.tab.h termparser.tab.h++

endif # ----------------------------------------------------------------

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

term2c_LDFLAGS = $(ROSELDFLAGS)
c2term_LDFLAGS = $(ROSELDFLAGS)

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
  term_lint_main.pl \
  clang/README clang/INSTALL clang/clang-termite.patch

