include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

EXTRA_DIST = 

bin_PROGRAMS = typeforge
noinst_PROGRAMS = 

CLEANFILES = 

typeforge_YFLAGS = -p ltl_ # yacc flags for specification parser (not used yet)
typeforge_CXXFLAGS = -Wall -O3 -march=native -ftree-vectorize
typeforge_DEPENDENCIES=

AM_LDFLAGS = $(ROSE_LIBS) $(LIBS_WITH_RPATH) -fopenmp
AM_CPPFLAGS = $(ROSE_INCLUDES) -I$(top_srcdir)/projects/typeforge/src -g -rdynamic

typeforge_LDADD = $(LIBS_WITH_RPATH) $(ROSE_LIBS)
typeforge_DEPENDENCIES=
typeforge_LDADD = $(LIBS_WITH_RPATH) $(ROSE_LIBS)
typeforge__CXXFLAGS = -Wall -O3 -march=native -ftree-vectorize
typeforge_SOURCES = typeforge.C CommandLineOptions.C CastStats.C CastTransformer.C TFTypeTransformer.C CastGraphVis.C CppStdUtilities.C TFTransformation.C TFSpecFrontEnd.C ToolConfig.cpp TFCommandList.C

CHECK_DEFAULT=check-typeforge

check-local: $(CHECK_DEFAULT)

check-typeforge: typeforge
	@echo ================================================================
	@echo RUNNING TYPEFORGE TESTS
	@echo ================================================================
	rm -f typegraph.dot
	rm -f rose_casttest3.C
#	./typeforge -DSKIP_ROSE_BUILTIN_DECLARATIONS --dot-type-graph $(srcdir)/tests/casttest1.C 
#	dot -Tpdf typegraph.dot -o type-graph.pdf
#	./typeforge --explicit $(srcdir)/tests/casttest1.C
	./typeforge --explicit --float-var=f1,d1 $(srcdir)/tests/casttest3.C --compile
	./typeforge --spec-file=$(srcdir)/tests/casttest3.tf --trace $(srcdir)/tests/casttest3.C --compile
	g++ -Wall rose_casttest3.C
	./typeforge --spec-file=$(srcdir)/tests/type_replace_test1.tf --trace $(srcdir)/tests/type_replace_test1.C
	./typeforge --spec-file=$(srcdir)/tests/basetype_replace_test1.tf --trace $(srcdir)/tests/basetype_replace_test1.C
	./typeforge --spec-file=$(srcdir)/tests/json_replace_test.json --trace $(srcdir)/tests/json_replace_test.C 
	./typeforge --spec-file=$(srcdir)/tests/json_out_test.json --trace $(srcdir)/tests/json_out_test.C --compile
	./typeforge --spec-file=craft_initial.json --trace $(srcdir)/tests/json_out_test.C --compile
	./typeforge --spec-file=$(srcdir)/tests/ad_transformation_test.tf $(srcdir)/tests/casttest1.C
	./typeforge --spec-file=$(srcdir)/tests/ad_transformation_test.tf $(srcdir)/tests/ad_compound_test.C
	./typeforge --spec-file=$(srcdir)/tests/ad_transformation_test.tf $(srcdir)/tests/ad_array_test.C
	./typeforge --spec-file=$(srcdir)/tests/ad_transformation_test.tf $(srcdir)/tests/ad_global_test.C
	./typeforge --spec-file=$(srcdir)/tests/ad_include_pragma.json $(srcdir)/tests/include_pragma_test1.C
	./typeforge --spec-file=$(srcdir)/tests/ad_include_pragma.json $(srcdir)/tests/include_pragma_test2.C
	./typeforge --spec-file=$(srcdir)/tests/ad_all.json $(srcdir)/tests/ad_all.C
	rm -f a.out craft_initial.json

#check-typeforge:
#	@echo ================================================================
#	@echo RUNNING DEADCODE TESTS
#	@echo ================================================================
#	$(srcdir)/scripts/runDeadCodeTests $(srcdir)/tests

docs:
	cd "$(srcdir)" && doxygen

clean-local:
	rm -f *.dot
	rm -f *.ps
	rm -f *.jpg

distclean-local: clean
	rm -f *.tgz
	rm -f *~
	rm -rf ../docs/doxygen
