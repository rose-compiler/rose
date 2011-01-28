include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

###############################################################################
# Subdirectory specifics
# TOO (01/26/11): relative paths must be updated for each propagation upward
# in the flattening of Makefiles
#
# Caution: include path cannot contain variable,
#	e.g. include cxxCodeGenerationPath/Makefile_variables
#
include $(srcdir)/unparser/Makefile_variables
include $(srcdir)/unparser/CxxCodeGeneration/Makefile_variables
include $(srcdir)/unparser/formatSupport/Makefile_variables
include $(srcdir)/unparser/FortranCodeGeneration/Makefile_variables
include $(srcdir)/unparser/languageIndependenceSupport/Makefile_variables
include $(srcdir)/unparser/PHPCodeGeneration/Makefile_variables
include $(srcdir)/asmUnparser/Makefile_variables


unparserPath=$(srcdir)/unparser#
cxxCodeGenerationPath=$(srcdir)/unparser/CxxCodeGeneration#
formatSupportPath=$(srcdir)/unparser/formatSupport#
fortranCodeGenerationPath=$(srcdir)/unparser/FortranCodeGeneration#
languageIndependenceSupportPath=$(srcdir)/unparser/languageIndependenceSupport#
phpCodeGenerationPath=$(srcdir)/unparser/PHPCodeGeneration#
asmUnparserPath=$(srcdir)/asmUnparser#


###############################################################################

INCLUDES=\
	$(ROSE_INCLUDES)


noinst_LTLIBRARIES = libbackend.la

##################################################################

libbackend_la_SOURCES=\
	$(unparser_la_sources) \
	$(asmUnparser_la_sources)


include_HEADERS=\
	$(unparser_includeHeaders)


dist_include_HEADERS=\
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
