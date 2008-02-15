include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

# Note that relationTranslatorGenerator must be processed before 
# src because files built there are needed in src.
SUBDIRS = relationTranslatorGenerator src tests

# Automake conditional to control use of ROSE with bddbddb
if ROSE_USE_BDDBDDB

endif

EXTRA_DIST = bddbddb include jars

