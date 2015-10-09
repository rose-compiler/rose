include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

# Automake conditional to control use of ROSE with BDDBDDB
if ROSE_USE_BDDBDDB

noinst_LTLIBRARIES = libastBinaryRelations.la

# Bug fix suggested by Rich which is required for automake 1.9.2, but not for version 1.9.6
# libastBinaryRelations_la_SOURCES = output/RelationVisitor.cc
nodist_libastBinaryRelations_la_SOURCES = output/RelationVisitor.cc

# libastBinaryRelations_la_LIBADD = RelationVisitor.lo ChildVisitor.lo
# libastBinaryRelations_la_LIBADD = RelationVisitor.lo

# RelationVisitor.lo: output/RelationVisitor.cc output/RelationVisitor.hh
# ChildVisitor.lo: output/ChildVisitor.cc

AM_CPPFLAGS = -I$(top_builddir) $(ROSE_INCLUDES) -I$(srcdir)/../include

#RelationVisitor.lo: $(generated_relation_source_code)
#	$(CXX) $(AM_CPPFLAGS) -Ioutput -c output/RelationVisitor.cc

include_HEADERS = 
###########################

# generated_relation_source_code = ../src/RelationVisitor.cc  ../src/RelationVisitor.hh
generated_relation_source_code = output/RelationVisitor.cc  output/RelationVisitor.hh
# Python must be version 2.4
$(generated_relation_source_code): main.py Relations.py  Visitor.py
	mkdir -p output
	@echo "NOTE: Must be using Python version 2.4 or greater"
	env PYTHONPATH=$(srcdir) $(python_path) $(srcdir)/main.py
#	cp -f output/* .
#	cp -f output/* ../src

all: $(generated_relation_source_code)

check-local:
	@echo "********************************************************************************************"
	@echo "*** ROSE/projects/DocumentationGenerator: make check rule complete (terminated normally) ***"
	@echo "********************************************************************************************"
endif

EXTRA_DIST = main.py  Relations.py  Visitor.py

distclean-local:
	rm -rf output

clean-local:
	rm -rf output/*
	rm -rf $(CXX_TEMPLATE_OBJECTS) Templates.DB ii_files ti_files
	rm -f *.o rose_*.C *.sage.C doxygenComment.C \
              *.C.pdf *.C.edg.pdf *.pdf *.dot *.sidl *.int.C rose_directives_list.txt core
	rm -rf ti_files
	rm -f *.docs
