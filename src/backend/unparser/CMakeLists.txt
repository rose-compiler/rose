
include_directories(${KDE4_INCLUDES} ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} )

add_subdirectory(CxxCodeGeneration)
add_subdirectory(FortranCodeGeneration)
add_subdirectory(PHPCodeGeneration)
add_subdirectory(languageIndependenceSupport)
add_subdirectory(formatSupport)


########### next target ###############

set( unparser_SRC
      ./FortranCodeGeneration/unparseFortran_expressions.C
      ./FortranCodeGeneration/unparseFortran_modfile.C
      ./FortranCodeGeneration/unparseFortran_statements.C
      ./FortranCodeGeneration/unparseFortran_types.C
      ./CxxCodeGeneration/unparseCxx_types.C
      ./CxxCodeGeneration/unparseCxx_expressions.C
      ./CxxCodeGeneration/unparseCxx_statements.C
#      ./copy_unparser.C
      ./astUnparseAttribute.C
      ./unparser.C
      ./unparse_sym.C
      ./formatSupport/unparseFormatHelp.C
      ./formatSupport/unparse_format.C
      ./PHPCodeGeneration/unparsePHP.C
      ./languageIndependenceSupport/modified_sage_isUnaryOp.C
      ./languageIndependenceSupport/unparser_opt.C
      ./languageIndependenceSupport/modified_sage.C
      ./languageIndependenceSupport/name_qualification_support.C
      ./languageIndependenceSupport/unparse_debug.C
      ./languageIndependenceSupport/unparseLanguageIndependentConstructs.C
  )

add_library(unparser ${unparser_SRC})
add_dependencies( unparser ROSETTA )
########### install files ###############

install(FILES  $(unparser_headers) DESTINATION ${INCLUDE_INSTALL_DIR})



#original Makefile.am contents follow:

#include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs
#
## SUBDIRS = CxxCodeGeneration languageIndependenceSupport formatSupport
#SUBDIRS = CxxCodeGeneration FortranCodeGeneration PHPCodeGeneration languageIndependenceSupport formatSupport
#
#INCLUDES = $(ROSE_INCLUDES)
#
## unparser_headers = copy_unparser.h unparser.h unparse_sym.h
#unparser_headers = unparser.h unparse_sym.h astUnparseAttribute.h
#unparser_sources = $(unparser_headers:.h=.C)
#
#noinst_LTLIBRARIES = libunparse.la
#
#libunparse_la_SOURCES = $(unparser_sources) $(unparser_headers)
#libunparse_la_LIBADD = 
#
##     formatSupport/libunparseFormatSupport.la 
##     languageIndependenceSupport/libunparseLanguageIndependenceSupport.la 
##     CxxCodeGeneration/libunparseCxx.la 
##     FortranCodeGeneration/libunparseFortran.la
#
#include_HEADERS = $(unparser_headers)
#
#clean-local:
#	rm -rf Templates.DB ii_files ti_files core
#
#EXTRA_DIST = unparser.docs
#
## eof
