include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

INCLUDES = \
     $(ROSE_INCLUDES) -I$(top_srcdir)/src/util  \
     -I$(top_srcdir)/src/util/support \
     -I$(top_srcdir)/src/util/graphs \
     -I$(top_srcdir)/src/midend/astUtil \
     -I$(top_srcdir)/src/midend/astUtil/astInterface \
     -I$(top_srcdir)/src/midend/astUtil/astSupport \
     -I$(top_srcdir)/src/midend/astUtil/symbolicVal \
     -I$(top_srcdir)/src/midend/astUtil/annotation \
     -I$(top_srcdir)/src/midend/programAnalysis \
     -I$(top_srcdir)/src/midend/programAnalysis/CFG \
     -I$(top_srcdir)/src/midend/programAnalysis/dataflowAnalysis \
     -I$(top_srcdir)/projects/containerParallelizer/CPAnalysis \
     -I$(top_srcdir)/projects/containerParallelizer/CPTransformation \
     -I$(top_srcdir)/projects/containerParallelizer

lib_LIBRARIES = libOmpTransformations1.a

libOmpTransformations1_a_SOURCES = \
     BodyTransformationParser.C OmpLoopRATransformationParser.C

libOmpTransformations1_a_DEPENDENCIES = $(ROSE_LIBS_WITH_PATH)
libOmpTransformations1_a_LIBADD = 

noinst_HEADERS = 

# ACLOCAL = @ACLOCAL@ -I ../config -I ../acmacros

# We have to explicily build this rule since we want to avoid the use of "ar cru" 
# (need to use the C++ compiler so that templates will be built)
libOmpTransformations1.a: $(libOmpTransformations1_a_OBJECTS) $(libOmpTransformations1_a_DEPENDENCIES)
	-rm -f $@
	$(CXX_STATIC_LIB_UPDATE) $@ $(libOmpTransformations1_a_OBJECTS) $(libOmpTransformations1_a_LIBADD)
	test -f $@ || exit 1

LDFLAGS = @STL_INCLUDES@

LDADD = $(ROSE_LIBS) \
        $(top_builddir)/projects/containerParallelizer/CPTransformation/libOmpTransformations1.a


clean-local:
	rm -rf Templates.DB ii_files ti_files cxx_templates

distclean-local:
	rm -rf Templates.DB

include_HEADERS = \
	BodyTransformationParser_Declarations.h \
	BodyTransformationParser.h BodyTransformationCocoCodes.h BodyTransformationCocoErrorMessages.h \
	BodyTransformation.h \
	OmpLoopRATransformationParser_Declarations.h \
	OmpLoopRATransformationParser.h OmpLoopRATransformationCocoCodes.h OmpLoopRATransformationCocoErrorMessages.h


#dist-hook:
#	ln -s ../STL-link ${distdir}

# AUTOMAKE_OPTIONS = foreign








