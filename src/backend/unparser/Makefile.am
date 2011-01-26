include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

###############################################################################
cxxCodeGenerationPath=$(srcdir)/CxxCodeGeneration#
formatSupportPath=$(srcdir)/formatSupport#
fortranCodeGenerationPath=$(srcdir)/FortranCodeGeneration#
languageIndependenceSupportPath=$(srcdir)/languageIndependenceSupport#
phpCodeGenerationPath=$(srcdir)/PHPCodeGeneration#


# Subdirectory specifics
# TOO (01/26/11): relative paths must be updated for each propagation upward
# in the flattening of Makefiles 
#
include $(cxxCodeGenerationPath)/Makefile_variables
include $(formatSupportPath)/Makefile_variables
include $(fortranCodeGenerationPath)/Makefile_variables
include $(languageIndependenceSupportPath)/Makefile_variables
include $(phpCodeGenerationPath)/Makefile_variables


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
