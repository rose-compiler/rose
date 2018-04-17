include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

# DQ (12/28/2006): Later versions of automake don't seem to work when this is a directory!
# EXTRA_DIST = datalogExamples
# JJW (3/12/2008) Palette uses this example
EXTRA_DIST = test.cc datalogExamples

TEST_TARGETS =

# Automake conditional to control use of ROSE with BDDBDDB
if ROSE_USE_BDDBDDB

TEST_TARGETS += test
test:
	mkdir -p dump
	../src/relationTranslator $(srcdir)/test.cc
	$(srcdir)/../bddbddb -Dbasedir=dump -Dincludedirs=$(srcdir)/datalogExamples/:dump/ $(srcdir)/datalogExamples/switch_without_default

testDan:
	mkdir -p dump
	../src/relationTranslator $(srcdir)/test.cc
	$(srcdir)/../bddbddb -Dbasedir=dump -Dincludedirs=$(srcdir)/datalogExamples/:dump/ $(srcdir)/datalogExamples/is_child_of_parent

dist-hook:
	@echo "Copy the datalogExamples to the distribution being built"
#	cp $(srcdir)/datalogExamples $(distdir)

endif

check-local: $(TEST_TARGETS)

distclean-local:

clean-local:
	rm -rf $(CXX_TEMPLATE_OBJECTS) Templates.DB ii_files ti_files
	rm -f *.o rose_*.C *.sage.C doxygenComment.C \
              *.C.pdf *.C.edg.pdf *.pdf *.dot *.sidl *.int.C rose_directives_list.txt core
	rm -rf ti_files
	rm -f *.docs
