include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

INCLUDES = $(ROSE_INCLUDES) -I$(srcdir)/../compass/compassSupport
LDADD    = $(LIBS_WITH_RPATH) $(ROSE_LIBS)

PALETTEFLAGS = -I$(srcdir)

BUILT_SOURCES = palette2cpp

bin_PROGRAMS = palette2cpp
palette2cpp_SOURCES = palette2cpp.C prologLexer.C prologParser.C prologParser.h prologAst.h

dist_noinst_DATA = palette-talk.ppt

dist_data_DATA = rose.P arith.P io.P basic.tmpl compass.tmpl

include_HEADERS = paletteCommon.h

noinst_PROGRAMS = qsolve simple_example null_deref switch_without_default
qsolve_SOURCES = qsolve.P
qsolve_DEPENDENCIES = arith.P io.P basic.tmpl
simple_example_SOURCES = simple_example.P
simple_example_DEPENDENCIES = io.P basic.tmpl
null_deref_SOURCES = null_deref.P
null_deref_DEPENDENCIES = rose.P compass.tmpl
switch_without_default_SOURCES = switch_without_default.P
switch_without_default_DEPENDENCIES = rose.P compass.tmpl

.P.C: palette2cpp
	./palette2cpp $(PALETTEFLAGS) $< > $@

SUFFIXES = .P

rose.P: $(srcdir)/gen_rose_tables.tcl $(top_builddir)/src/frontend/SageIII/Cxx_Grammar.h
	tclsh $(srcdir)/gen_rose_tables.tcl $(top_builddir)/src/frontend/SageIII/Cxx_Grammar.h > rose.P

CLEANFILES = rose.P

check-local: switch_without_default null_deref qsolve simple_example
	@if [ -e compass_parameters ]; then echo "Please remove compass_parameters before running \"make check\"" 1>&2; exit 1; fi
	@touch compass_parameters
	@./switch_without_default -edg:w $(top_srcdir)/projects/DatalogAnalysis/tests/test.cc 2>current_answers
	@echo '*****' >> current_answers
	@./null_deref -edg:w $(top_srcdir)/projects/DatalogAnalysis/tests/test.cc 2>> current_answers
	@echo '*****' >> current_answers
	@./qsolve 2>/dev/null >> current_answers
	@echo '*****' >> current_answers
	@./simple_example 2>/dev/null >> current_answers
	@sed -i -e'/^Query took/d' -e's^/.*/projects/DatalogAnalysis/tests/^^g' current_answers
	@diff -q $(srcdir)/check_correct_answers current_answers
	@rm compass_parameters current_answers

EXTRA_DIST = check_correct_answers gen_rose_tables.tcl
