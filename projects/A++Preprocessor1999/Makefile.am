include ../../config/Makefile.for.ROSE.includes.and.libs

# INCLUDES = -I$(top_srcdir)/include -I$(top_srcdir)/src -I$(srcdir) -I$(srcdir)/.. -I$(srcdir)/../TransformBaseClass -I$(srcdir)/../ArrayGrammar -I../ArrayGrammar @SAGE_INCLUDES@ -I$(srcdir)/../ListT @STL_INCLUDES@ -I$(top_builddir)/include -I$(top_builddir)/SAGE @INCLUDES@
# INCLUDES = -I$(top_srcdir) -I$(top_srcdir)/include -I$(top_srcdir)/src -IArrayGrammar -I$(srcdir)/TransformBaseClass -I$(srcdir)/ArrayGrammar -I$(srcdir)/ArrayTransformationGrammar -I$(srcdir)/ListT @STL_INCLUDES@ -I$(top_builddir)/include -I$(top_builddir)/SAGE @INCLUDES@ @SAGE_INCLUDES@

INCLUDES = \
   @INCLUDES@ \
   $(ROSE_INCLUDES) \
   @SAGE_INCLUDES@ \
   @STL_INCLUDES@ \
   -I$(top_builddir)/include \
   -I$(top_builddir)/EDG/errinfo \
   -I$(srcdir)/TransformBaseClass \
   -I$(srcdir)/ArrayTransformationGrammar  \
   -I$(srcdir)/ListT \
   -I$(srcdir)/ArrayGrammar \
   -IArrayGrammar

# Directories containing non-automatically generated (semi-automated) grammars
SUBDIRS = TransformBaseClass ArrayGrammar  ArrayTransformationGrammar ListT

CXX_TEMPLATE_REPOSITORY_PATH = .

bin_PROGRAMS = optimizingArrayPreprocessor

optimizingArrayPreprocessor_SOURCES = optimizingArrayPreprocessor.C array_class_interface.C
optimizingArrayPreprocessor_DEPENDENCIES = \
   $(top_builddir)/src/librose.a \
   $(top_builddir)/EDG/src/libedg.a \
   ./TransformBaseClass/libtransformbaseclass.a \
   ./ArrayGrammar/libtransform2.a \
   ./ArrayTransformationGrammar/libtransform3.a

#   $(top_builddir)/SAGE/libsage3.a
#   $(top_builddir)/PDF/libpdf.a

# noinst_HEADERS = ROSE_Unparser.h
noinst_HEADERS = ROSE_Transformations.h array_class_interface.h

optimizingArrayPreprocessor_LDADD = \
  $(CXX_TEMPLATE_OBJECTS) \
  @SAGE_LIBS@ \
  $(top_builddir)/src/librose.a    \
  $(top_builddir)/EDG/src/libedg.a \
  ArrayGrammar/libtransform2.a     \
  ArrayTransformationGrammar/libtransform3.a \
  TransformBaseClass/libtransformbaseclass.a \
  $(top_builddir)/PDF/libpdf.a \
  $(top_builddir)/util/libroseutil.a \
  -lm

# /usr/local/zlib/1.1.3/lib/libz.a

## linker needs to know about STL
## By adding the -ptr. on the link line we should be able to avoid 
## specifying the Templates.DB/*.o file explicitly on the link line
LDFLAGS = @STL_INCLUDES@

## DQ (3/3/2001): I have added a dependence upon the ZLib compression library
## This is temporary to see if it works (it didn't so we can remove it shortly)
## LDADD = \
#  @SAGE_LIBS@ \
#  $(top_builddir)/src/librose.a \
#  $(top_builddir)/EDG/src/libedg.a \
#  ArrayGrammar/libtransform2.a \
#  ArrayTransformationGrammar/libtransform3.a \
#  TransformBaseClass/libtransformbaseclass.a	\
#  $(top_builddir)/PDF/libpdf.a \
#  /usr/local/zlib/1.1.3/lib/libz.a

# I think this will be required for the array preprocessor once it is using the code that
# used to be located in directories within ROSE/src/
# LDADD = \
#  Transform_2/libtransform2.a	\
#  Transform_3/libtransform3.a	\
#  TransformBaseClass/libtransformbaseclass.a	\
#  PaddingTrans/libpadtransform.a	\
#  Padding/libpadutils.a	\
#  unparser/libunparse.a	\
#  librose.a	\
#  $(top_builddir)/EDG/src/libedg.a	\
#  @SAGE_LIBS@	\
#  librose.a	\
#  $(top_builddir)/PDF/libpdf.a \
#  /usr/local/zlib/1.1.3/lib/libz.a

# If the purify linker is enabled, AUX_LINKER will be set by configure.
# These are now set in the ROSE SUPPORT PURIFY configure macro
# CCLD  = $(AUX_LINKER) $(CC)
# CXXLD = $(AUX_LINKER) $(CXX)

# EXTRA_DIST = ROSE_Transformations.h array_class_interface.h
EXTRA_DIST =

CLEANFILES = 

clean-local:
	rm -rf Templates.DB ii_files ti_files

# Compile everything with -g (compiler's debug flag)
AM_CPPFLAGS = -g

