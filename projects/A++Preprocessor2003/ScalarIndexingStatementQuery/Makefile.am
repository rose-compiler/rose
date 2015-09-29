include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

AM_CPPFLAGS = \
     $(ROSE_INCLUDES) \
     -I$(top_srcdir)/ExamplePreprocessors/A++P++Preprocessor \
     -I$(top_srcdir)/ExamplePreprocessors/A++P++Preprocessor/ArrayAssignmentStatementQuery \
     -I$(top_srcdir)/ExamplePreprocessors/A++P++Preprocessor/ScalarIndexingStatementQuery \
     -I$(top_srcdir)/ExamplePreprocessors/A++P++Preprocessor/IndexOffsetQuery

## The grammar generator (ROSETTA) should use its own template repository
CXX_TEMPLATE_REPOSITORY_PATH = .

lib_LIBRARIES = libscalarIndexingQuery.a

libscalarIndexingQuery_a_SOURCES = \
     scalarIndexingStatementAssembly.C \
     scalarIndexingStatementAccumulatorAttribute.C \
     scalarIndexingStatementInheritedAttribute.C \
     scalarIndexingStatementSynthesizedAttribute.C \
     scalarIndexingStatementTransformation.C

#    arrayDataBase.C
#    indexDataBase.C
#    database.C

libscalarIndexingQuery_a_LIBADD = $(CXX_TEMPLATE_OBJECTS)

# libscalarIndexingQuery_a_DEPENDENCIES =

noinst_HEADERS = \
     scalarIndexingStatementTransformation.h \
     scalarIndexingStatementInheritedAttribute.h \
     scalarIndexingStatementSynthesizedAttribute.h

ACLOCAL = @ACLOCAL@ -I ../config -I ../acmacros

# We have to explicily build this rule since we want to avoid the use of "ar cru" 
# (need to use the C++ compiler so that templates will be built)
libscalarIndexingQuery.a: $(libscalarIndexingQuery_a_OBJECTS) $(libscalarIndexingQuery_a_DEPENDENCIES)
	-rm -f $@
	$(CXX_STATIC_LIB_UPDATE) $@ $(libscalarIndexingQuery_a_OBJECTS) $(libscalarIndexingQuery_a_LIBADD)
	test -f $@ || exit 1

clean-local:
	rm -rf Templates.DB ii_files ti_files cxx_templates

distclean-local:
	rm -rf Templates.DB

include_HEADERS = \
     scalarIndexingStatementAccumulatorAttribute.h \
     scalarIndexingStatementTransformation.h \
     scalarIndexingStatementInheritedAttribute.h \
     scalarIndexingStatementSynthesizedAttribute.h

#    scalarDataBase.h
#    database.h

#dist-hook:
#	ln -s ../STL-link ${distdir}

AUTOMAKE_OPTIONS = foreign








