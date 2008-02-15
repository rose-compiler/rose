include ../../../config/Makefile.for.ROSE.includes.and.libs

# INCLUDES = -I$(top_srcdir)/include -I$(top_srcdir)/src -I$(srcdir) -I$(srcdir)/.. -I$(srcdir)/../TransformBaseClass -I$(srcdir)/../ArrayGrammar -I../ArrayGrammar @SAGE_INCLUDES@ -I$(srcdir)/../ListT @STL_INCLUDES@ -I$(top_builddir)/include -I$(top_builddir)/SAGE @INCLUDES@

INCLUDES = \
   @INCLUDES@ \
   $(ROSE_INCLUDES) \
   @SAGE_INCLUDES@ \
   @STL_INCLUDES@ \
   -I$(top_builddir)/../include \
   -I$(top_builddir)/EDG/errinfo \
   -I$(srcdir)/../TransformBaseClass \
   -I$(srcdir)/../ArrayTransformationGrammar  \
   -I$(srcdir)/.. \
   -I$(srcdir)/../ListT \
   -I$(srcdir)/../ArrayGrammar \
   -I../ArrayGrammar

CXX_TEMPLATE_REPOSITORY_PATH = ..

noinst_LIBRARIES = libtransform3.a
libtransform3_a_SOURCES = \
     transformation_3.C \
     transform_3.C \
     statementDataBase.C \
     operandDataBase.C \
     grammar_3.C

noinst_HEADERS = grammar_3.h ROSE_TRANSFORMATION_SOURCE_3.h transformation_3.h supported_dimensions.h

## Make sure that the $(top_builddir)/include/grammar.h is generated 
## before compiling any of the source files in this directory
transformation_3.o : ../ArrayGrammar/grammar.h
transform_3.o      : ../ArrayGrammar/grammar.h
statementDataBase.o: ../ArrayGrammar/grammar.h
operandDataBase.o  : ../ArrayGrammar/grammar.h
grammar_3.o        : ../ArrayGrammar/grammar.h

../ArrayGrammar/grammar.h:
	cd ../ArrayGrammar; $(MAKE) ../ArrayGrammar/grammar.h

AM_CPPFLAGS = -g

# We have to explicily build this rule since we want to avoid the use of "ar cru"
# (need to use the C++ compiler so that templates will be built)
libtransform3.a: $(libtransform3_a_OBJECTS) $(libtransform3_a_DEPENDENCIES)
	-rm -f $@
	$(CXX_STATIC_LIB_UPDATE) $@ $(libtransform3_a_OBJECTS) $(libtransform3_a_LIBADD)
	test -f $@ || exit 1

# the next line was replaced in the code above  BP : 10/16/2001
#	$(CXX_STATIC_LIB_UPDATE) $@ $(INCLUDES) $(libtransform3_a_OBJECTS) $(libtransform3_a_LIBADD)

clean-local:
	rm -rf Templates.DB ii_files ti_files
