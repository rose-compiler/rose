include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

INCLUDES = $(ROSE_INCLUDES)

unparserLibSources = \
     unparser.h unparser.C \
     unparse_stmt.C \
     unparse_expr.C \
     unparse_sym.C \
     unparse_type.C \
     unparse_support.C \
     modified_sage.C \
     unparse_format.h unparse_format.C \
     \
     unparser_fort.h unparser_fort.C \
     unparse_stmt_fort.C \
     unparse_expr_fort.C \
     unparse_sym_fort.C \
     unparse_type_fort.C \
     unparse_format_fort.h unparse_format_fort.C \
     \
     copy_unparser.h copy_unparser.C \
     unparser_opt.h unparser_opt.C


# lib_LIBRARIES = libunparse.a
# libunparse_a_SOURCES  = $(unparserLibSources)

if DEV_LIBS
lib_LTLIBRARIES = libunparse.la
else
noinst_LTLIBRARIES = libunparse.la
endif
libunparse_la_SOURCES = $(unparserLibSources)

include_HEADERS = copy_unparser.h unparser.h unparser_opt.h unparse_format.h

clean-local:
	rm -rf Templates.DB ii_files ti_files core

EXTRA_DIST = unparser.docs
