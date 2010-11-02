include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

INCLUDES = $(ROSE_INCLUDES)

unparseLanguageIndependenceSupport_headers = unparseLanguageIndependentConstructs.h name_qualification_support.h unparser_opt.h modified_sage.h unparse_debug.h 
# unparseLanguageIndependenceSupport_sources = $(unparseOpt_headers:.h=.C) modified_sage_isUnaryOp.C
unparseLanguageIndependenceSupport_sources = unparseLanguageIndependentConstructs.C name_qualification_support.C unparser_opt.C modified_sage.C unparse_debug.C modified_sage_isUnaryOp.C 

include_HEADERS = $(unparseLanguageIndependenceSupport_headers)
noinst_LTLIBRARIES = libunparseLanguageIndependenceSupport.la

libunparseLanguageIndependenceSupport_la_SOURCES = $(unparseLanguageIndependenceSupport_sources)

clean-local:
	rm -rf ii_files ti_files core


EXTRA_DIST = CMakeLists.txt
