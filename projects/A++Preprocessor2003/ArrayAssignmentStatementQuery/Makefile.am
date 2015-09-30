include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

AM_CPPFLAGS = \
     $(ROSE_INCLUDES) \
     -I$(top_srcdir)/ExamplePreprocessors/A++P++Preprocessor \
     -I$(top_srcdir)/ExamplePreprocessors/A++P++Preprocessor/ArrayAssignmentStatementQuery \
     -I$(top_srcdir)/ExamplePreprocessors/A++P++Preprocessor/ScalarIndexingStatementQuery \
     -I$(top_srcdir)/ExamplePreprocessors/A++P++Preprocessor/IndexOffsetQuery

## The grammar generator (ROSETTA) should use its own template repository
CXX_TEMPLATE_REPOSITORY_PATH = .

# I think the next line should be changed : Bobby Philip 09/27/2001
#noinst_LIBRARIES = libsage3.a
lib_LIBRARIES = libarrayassignmentquery.a

libarrayassignmentquery_a_SOURCES = \
     arrayAssignmentStatementAssembly.C \
     arrayAssignmentStatementAccumulatorAttribute.C \
     arrayAssignmentStatementInheritedAttribute.C \
     arrayAssignmentStatementSynthesizedAttribute.C \
     arrayAssignmentStatementTransformation.C

# changes to the templates in ../../AST_RewriteMechanism/rewriteTreeTraversal.C should force
# several files using those templates to be recompiled.  Express this explicitly:
TEMPLATE_DEPENDENT_CODE = \
     arrayAssignmentStatementTransformation.o \
     arrayAssignmentStatementAssembly.o

$(TEMPLATE_DEPENDENT_CODE): $(top_builddir)/AST_RewriteMechanism/rewriteTreeTraversal.o

libarrayassignmentquery_a_LIBADD = $(CXX_TEMPLATE_OBJECTS)

# libarrayassignmentquery_a_DEPENDENCIES =

noinst_HEADERS = \
     arrayAssignmentStatementTransformation.h \
     arrayAssignmentStatementInheritedAttribute.h \
     arrayAssignmentStatementSynthesizedAttribute.h \
     arrayAssignmentStatementAccumulatorAttribute.h

ACLOCAL = @ACLOCAL@ -I ../config -I ../acmacros

# We have to explicily build this rule since we want to avoid the use of "ar cru" 
# (need to use the C++ compiler so that templates will be built)
libarrayassignmentquery.a: $(libarrayassignmentquery_a_OBJECTS) $(libarrayassignmentquery_a_DEPENDENCIES)
	-rm -f $@
	$(CXX_STATIC_LIB_UPDATE) $@ $(libarrayassignmentquery_a_OBJECTS) $(libarrayassignmentquery_a_LIBADD)
	test -f $@ || exit 1

clean-local:
	rm -rf Templates.DB ii_files ti_files cxx_templates

distclean-local:
	rm -rf Templates.DB

include_HEADERS = \
     arrayAssignmentStatementAccumulatorAttribute.h \
     arrayAssignmentStatementTransformation.h \
     arrayAssignmentStatementInheritedAttribute.h \
     arrayAssignmentStatementSynthesizedAttribute.h

#dist-hook:
#	ln -s ../STL-link ${distdir}

AUTOMAKE_OPTIONS = foreign








