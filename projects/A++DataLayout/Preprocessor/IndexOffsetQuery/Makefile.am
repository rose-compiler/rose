include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

INCLUDES = \
     $(ROSE_INCLUDES) \
     -I$(top_srcdir)/projects/A++DataLayout \
     -I$(top_srcdir)/projects/A++DataLayout/include \
     -I$(top_srcdir)/projects/A++DataLayout/Preprocessor \
     -I$(top_srcdir)/projects/A++DataLayout/Preprocessor/ArrayAssignmentStatementQuery \
     -I$(top_srcdir)/projects/A++DataLayout/Preprocessor/IndexOffsetQuery

## The grammar generator (ROSETTA) should use its own template repository
CXX_TEMPLATE_REPOSITORY_PATH = .

noinst_LIBRARIES = libindexoffsetquery.a

libindexoffsetquery_a_SOURCES = indexOffsetQuery.C

libindexoffsetquery_a_includedir = $(includedir)/IndexOffsetQuery
libindexoffsetquery_a_include_HEADERS = indexOffsetQuery.h









