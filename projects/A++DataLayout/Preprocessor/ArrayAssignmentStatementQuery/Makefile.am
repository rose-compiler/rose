include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

INCLUDES = \
     $(ROSE_INCLUDES) \
     -I$(top_srcdir)/projects/A++DataLayout \
     -I$(top_srcdir)/projects/A++DataLayout/include \
     -I$(top_srcdir)/projects/A++DataLayout/Preprocessor \
     -I$(top_srcdir)/projects/A++DataLayout/Preprocessor/ArrayAssignmentStatementQuery \
     -I$(top_srcdir)/projects/A++DataLayout/Preprocessor/IndexOffsetQuery

## The grammar generator (ROSETTA) should use its own template repository
CXX_TEMPLATE_REPOSITORY_PATH = .
CXXFLAGS = -DDEBUG 

lib_LIBRARIES = libarrayassignmentquery.a

libarrayassignmentquery_a_SOURCES = \
     arrayAssignmentStatementAssembly.C \
     arrayAssignmentStatementAccumulatorAttribute.C \
     arrayAssignmentStatementInheritedAttribute.C \
     arrayAssignmentStatementSynthesizedAttribute.C \
     arrayAssignmentStatementTransformation.C

libarrayassignmentquery_a_LIBADD = $(CXX_TEMPLATE_OBJECTS)


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








