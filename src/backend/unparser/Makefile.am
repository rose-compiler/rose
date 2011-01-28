include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

###############################################################################
# Subdirectory specifics
# TOO (01/26/11): relative paths must be updated for each propagation upward
# in the flattening of Makefiles 
#
include $(srcdir)/CxxCodeGeneration/Makefile_variables
include $(srcdir)/formatSupport/Makefile_variables
include $(srcdir)/FortranCodeGeneration/Makefile_variables
include $(srcdir)/languageIndependenceSupport/Makefile_variables
include $(srcdir)/PHPCodeGeneration/Makefile_variables


cxxCodeGenerationPath=$(srcdir)/CxxCodeGeneration
formatSupportPath=$(srcdir)/formatSupport
fortranCodeGenerationPath=$(srcdir)/FortranCodeGeneration
languageIndependenceSupportPath=$(srcdir)/languageIndependenceSupport
phpCodeGenerationPath=$(srcdir)/PHPCodeGeneration

###############################################################################

INCLUDES=\
	$(ROSE_INCLUDES)


noinst_LTLIBRARIES = libunparse.la


# unparser_headers = copy_unparser.h unparser.h unparse_sym.h
unparser_headers = unparser.h unparse_sym.h astUnparseAttribute.h
unparser_sources = $(unparser_headers:.h=.C)


##################################################################

libunparse_la_SOURCES=\
	$(unparser_sources) \
	$(unparser_headers) \
	$(unparseCxxCodeGeneration_la_sources) \
	$(unparseFormatSupport_la_sources) \
	$(unparseFortran_la_sources) \
	$(unparseLanguageIndependenceSupport_la_sources) \
	$(unparsePHPCodeGeneration_la_sources)


libunparse_la_LIBADD = # 


include_HEADERS=\
	$(unparser_headers) \
	$(unparseCxxCodeGeneration_includeHeaders) \
	$(unparseFormat_includeHeaders) \
	$(unparseFortran_includeHeaders) \
	$(unparseLanguageIndependenceSupport_includeHeaders) \
	$(unparsePHPCodeGeneration_includeHeaders)



EXTRA_DIST=\
	CMakeLists.txt \
	unparser.docs \
	$(unparseCxxCodeGeneration_extraDist) \
	$(unparseFormatSupport_extraDist) \
	$(unparseFortran_extraDist) \
	$(unparseLanguageIndependenceSupport_extraDist) \
	$(unparsePHPCodeGeneration_extraDist)


clean-local:
	rm -rf Templates.DB ii_files ti_files core
	$(unparseCxxCodeGeneration_cleanLocal)
	$(unparseFormatSupport_cleanLocal)
	$(unparseFortran_cleanLocal)
	$(unparseLanguageIndependenceSupport_cleanLocal)
	$(unparsePHPCodeGeneration_cleanLocal)
