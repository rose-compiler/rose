include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

INCLUDES = $(ROSE_INCLUDES)

if ROSE_USE_BOOST_WAVE

# DQ (10/27/2013): Commented out for now from this Makefile.am (experimenting with replacing with new implementation independent of WAVE).
# libastTokenStream_la_SOURCES      = createMap.C  doCompleteMapping.C  linearizeAST.C unparseMacro.C
# libastTokenStream_la_LIBADD       = 
# libastTokenStream_la_DEPENDENCIES = 
# include_HEADERS = createMap.h  doCompleteMapping.h  linearizeAST.h unparseMacro.h

endif


# DQ (10/27/2013): This is a new definition of libastTokenStream
# noinst_LTLIBRARIES = libastTokenStream.la
noinst_LTLIBRARIES = libastTokenStream.la

# DQ (10/27/2013): This is the new support for token stream mapping.
libastTokenStream_la_SOURCES = tokenStreamMapping.C frontierDetection.C simpleFrontierDetection.C previousAndNextNode.C

# DQ (10/27/2013): These are required for the older tests in tests/roseTests/astTokenStreamTests
libastTokenStream_la_SOURCES += createMap.C  doCompleteMapping.C  linearizeAST.C unparseMacro.C

libastTokenStream_la_LIBADD       = 
libastTokenStream_la_DEPENDENCIES = 

# Leave these header file reference in the Makefile.am (so we will pass the policy tests).
pkginclude_HEADERS = createMap.h  doCompleteMapping.h  linearizeAST.h unparseMacro.h \
   tokenStreamMapping.h frontierDetection.h simpleFrontierDetection.h previousAndNextNode.h



EXTRA_DIST = CMakeLists.txt

