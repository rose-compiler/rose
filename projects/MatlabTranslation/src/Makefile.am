#
# Matlab to C++ Translator
#

include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

if ROSE_WITH_GNU_OCTAVE_PARSER

## Place generated object files (.o) into the same directory as their source
## files, in order to avoid collisions when non-recursive make is used.
AUTOMAKE_OPTIONS = subdir-objects

AM_CPPFLAGS = -I$(top_srcdir)/src/backend/unparser/MatlabCodeGeneration/ \
              -I$(top_srcdir)/src/frontend/Experimental_Matlab_ROSE_Connection/ \
              $(ROSE_INCLUDES)
m2cxx_LDADD = $(ROSE_LIBS) $(libdir)/libexperimentalRoseMatlab.la

bin_PROGRAMS = m2cxx

m2cxx_SOURCES = \
  m2cxx.cc \
  utility/utils.cc \
  utility/dotify.cc \
  utility/FunctionReturnAttribute.cc \
  typeInference/BottomUpTypeAnalysis.cc \
  typeInference/FastNumericsRoseSupport.cc \
  typeInference/FunctionAnalyzer.cc \
  typeInference/MatlabTypeInference.cc \
  typeInference/MatlabVariables.cc \
  typeInference/TypeAttribute.cc \
  typeInference/VariableDeclarationBuilder.cc \
  transformations/MatlabSimpleTransformer.cc \
  transformations/MatlabToCpp.cc \
  transformations/MatrixTypeTransformer.cc \
  transformations/ExpressionTransformer.cc \
  transformations/NarginTransformer.cc \
  transformations/armadillo/ArmaOptimizer.cc

#~ m2cxx_LDADD =  libroseMatlabFrontend.la

# $(ROSE_LIBS) #-ldl $(LIBLTDL)

else

##
## Project requires Matlab support
##

endif
