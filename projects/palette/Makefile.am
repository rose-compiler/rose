include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs
include $(top_srcdir)/projects/compass/src/compassSupport/compass_dirs.inc

INCLUDES = $(ROSE_INCLUDES) -I$(compass_support_dir)  -I$(compass_prereqs_dir) -I$(compass_support_bdir)
LDADD    = $(LIBS_WITH_RPATH) $(ROSE_LIBS)

PALETTEFLAGS = 

BUILT_SOURCES = palette2cpp$(EXEEXT) $(srcdir)/rose.P

bin_PROGRAMS = palette2cpp

palette2cpp_SOURCES = palette2cpp.C prologLexer.C prologParser.C prologParser.h prologAst.h

dist_noinst_DATA = palette-talk.ppt

dist_data_DATA = rose.P arith.P io.P basic.tmpl compass.tmpl \
		 __eq___pp.itmpl iterateChildren_pm.itmpl cond_p.itmpl \
		 iterMemoryPools_mp.itmpl let_mp.itmpl do_p.itmpl \
		 iterCfgEdges_pppm.itmpl iterateAttributes_pm.itmpl

include_HEADERS = paletteCommon.h

noinst_PROGRAMS = qsolve simple_example null_deref switch_without_default sortTest
qsolve_SOURCES = qsolve.P
qsolve_DEPENDENCIES = $(dist_data_DATA) palette2cpp$(EXEEXT)
simple_example_SOURCES = simple_example.P
simple_example_DEPENDENCIES = $(dist_data_DATA) palette2cpp$(EXEEXT)
null_deref_SOURCES = null_deref.P
null_deref_DEPENDENCIES = $(dist_data_DATA) palette2cpp$(EXEEXT)
switch_without_default_SOURCES = switch_without_default.P
switch_without_default_DEPENDENCIES = $(dist_data_DATA) palette2cpp$(EXEEXT)
sortTest_SOURCES = sortTest.P
sortTest_DEPENDENCIES = $(dist_data_DATA) palette2cpp$(EXEEXT)

.P.C: palette2cpp$(EXEEXT)
	./palette2cpp$(EXEEXT) $(PALETTEFLAGS) -- $< > $@ || (rm -f $@ && exit 1)

SUFFIXES = .P

$(srcdir)/rose.P: $(srcdir)/gen_rose_tables.tcl $(top_srcdir)/src/ROSETTA/src/*.C
	if test \! -e $(srcdir)/rose.P -o -w $(srcdir)/rose.P; then ${SHELL} $(top_srcdir)/config/missing --run tclsh $(srcdir)/gen_rose_tables.tcl $(top_builddir)/src/frontend/SageIII/Cxx_Grammar.h $(srcdir)/rose.P; fi

check-local: switch_without_default null_deref qsolve simple_example sortTest
#	@if [ -e compass_parameters ]; then echo "Please remove compass_parameters before running \"make check\"" 1>&2; exit 1; fi
	@rm -rf compass_parameters
	@touch compass_parameters
	@./switch_without_default -edg:w $(top_srcdir)/projects/DatalogAnalysis/tests/test.cc 2>current_answers
	@echo '*****' >> current_answers
	@./null_deref -edg:w $(top_srcdir)/projects/DatalogAnalysis/tests/test.cc 2>> current_answers
	@echo '*****' >> current_answers
	@./qsolve 2>/dev/null >> current_answers
	@echo '*****' >> current_answers
	@./simple_example 2>/dev/null >> current_answers
	@echo '*****' >> current_answers
	@./sortTest 2>/dev/null >> current_answers
	@mv current_answers current_answers.temp
	@sed -e'/^Query took/d' -e's^/.*/projects/DatalogAnalysis/tests/^^g' current_answers.temp > current_answers
	@diff -U1000 $(srcdir)/check_correct_answers current_answers
	@rm compass_parameters current_answers current_answers.temp
	@echo "Test passed"

EXTRA_DIST = check_correct_answers gen_rose_tables.tcl rose.P
clean-local:
	rm -rf compass_parameters current_answers
