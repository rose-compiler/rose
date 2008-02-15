include ../../../config/Makefile.for.ROSE.includes.and.libs

# INCLUDES = -I$(top_srcdir)/src -I$(top_srcdir)/include -I$(srcdir) -I$(srcdir)/.. -I$(srcdir)/../ListT @SAGE_INCLUDES@ @STL_INCLUDES@ -I$(top_builddir)/include -I$(top_builddir)/SAGE @INCLUDES@

INCLUDES = \
   @INCLUDES@ \
   $(ROSE_INCLUDES) \
   @SAGE_INCLUDES@ \
   @STL_INCLUDES@ \
   -I$(top_builddir)/../include \
   -I$(top_builddir)/EDG/errinfo

CXX_TEMPLATE_REPOSITORY_PATH = ..

noinst_LIBRARIES = libtransformbaseclass.a
libtransformbaseclass_a_SOURCES = \
     TransformationBase.C \
     copy.C \
     edit.C \
     transformationTemplate.C \
     targetTemplateFunction.C \
     transformationTemplateFunction.C

noinst_HEADERS = TransformationBase.h

AM_CPPFLAGS = -g

# We have to explicily build this rule since we want to avoid the use of "ar cru"
# (need to use the C++ compiler so that templates will be built)
libtransformbaseclass.a: $(libtransformbaseclass_a_OBJECTS) $(libtransformbaseclass_a_DEPENDENCIES)
	-rm -f $@
	$(CXX_STATIC_LIB_UPDATE) $@ $(libtransformbaseclass_a_OBJECTS) $(libtransformbaseclass_a_LIBADD)
	test -f $@ || exit 1

clean-local:
	rm -rf Templates.DB ii_files ti_files
