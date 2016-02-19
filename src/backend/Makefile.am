# Non-recursive Makefile

include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

###############################################################################
# Subdirectory specifics
# TOO (01/26/11): relative paths must be updated for each propagation upward
# in the flattening of Makefiles
#
# Caution: include path cannot contain variable,
#	e.g. include $(cxxCodeGenerationPath)/Makefile_variables
#
include $(srcdir)/unparser/Makefile_variables
include $(srcdir)/unparser/CxxCodeGeneration/Makefile_variables
include $(srcdir)/unparser/formatSupport/Makefile_variables
include $(srcdir)/unparser/FortranCodeGeneration/Makefile_variables
include $(srcdir)/unparser/languageIndependenceSupport/Makefile_variables
include $(srcdir)/unparser/PHPCodeGeneration/Makefile_variables
include $(srcdir)/unparser/PythonCodeGeneration/Makefile_variables
include $(srcdir)/asmUnparser/Makefile_variables
include $(srcdir)/unparser/JavaCodeGeneration/Makefile_variables
include $(srcdir)/unparser/X10CodeGeneration/Makefile_variables

# if ROSE_BUILD_MATLAB_LANGUAGE_SUPPORT
# include $(srcdir)/unparser/MatlabCodeGeneration/Makefile_variables
# matlabCodeGenerationPath=$(srcdir)/unparser/MatlabCodeGeneration#
# else
# matlabCodeGenerationPath=
# endif

unparserPath=$(srcdir)/unparser#
cxxCodeGenerationPath=$(srcdir)/unparser/CxxCodeGeneration#
formatSupportPath=$(srcdir)/unparser/formatSupport#
fortranCodeGenerationPath=$(srcdir)/unparser/FortranCodeGeneration#
languageIndependenceSupportPath=$(srcdir)/unparser/languageIndependenceSupport#
phpCodeGenerationPath=$(srcdir)/unparser/PHPCodeGeneration#
pythonCodeGenerationPath=$(srcdir)/unparser/PythonCodeGeneration#
asmUnparserPath=$(srcdir)/asmUnparser#
javaCodeGenerationPath=$(srcdir)/unparser/JavaCodeGeneration#
x10CodeGenerationPath=$(srcdir)/unparser/X10CodeGeneration#


###############################################################################

AM_CPPFLAGS=\
	$(ROSE_INCLUDES)

noinst_LTLIBRARIES = libbackend.la

##################################################################

libbackend_la_SOURCES=\
	$(unparser_la_sources) \
	$(asmUnparser_la_sources)


pkginclude_HEADERS=\
	$(unparser_includeHeaders)


dist_pkginclude_HEADERS=\
	$(asmUnparser_distIncludeHeaders)


EXTRA_DIST=\
	CMakeLists.txt \
	$(unparser_extraDist) \
	$(asmUnparser_extraDist)


clean-local:
	$(unparser_cleanLocal)


# DQ (10/29/2010): We have to include "asmUnparser" directory so that its header files 
# will be installed by "make install", even with language only options are used.
#SUBDIRS = unparser asmUnparser

