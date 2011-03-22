include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

# SUBDIRS = CxxCodeGeneration languageIndependenceSupport formatSupport
SUBDIRS = CxxCodeGeneration FortranCodeGeneration PHPCodeGeneration languageIndependenceSupport formatSupport

INCLUDES = $(ROSE_INCLUDES)

# unparser_headers = copy_unparser.h unparser.h unparse_sym.h
unparser_headers = unparser.h unparse_sym.h astUnparseAttribute.h
unparser_sources = $(unparser_headers:.h=.C)

noinst_LTLIBRARIES = libunparse.la

libunparse_la_SOURCES = $(unparser_sources) $(unparser_headers)
libunparse_la_LIBADD = 

#     formatSupport/libunparseFormatSupport.la 
#     languageIndependenceSupport/libunparseLanguageIndependenceSupport.la 
#     CxxCodeGeneration/libunparseCxx.la 
#     FortranCodeGeneration/libunparseFortran.la

include_HEADERS = $(unparser_headers)

clean-local:
	rm -rf Templates.DB ii_files ti_files core

EXTRA_DIST = CMakeLists.txt unparser.docs

# eof
