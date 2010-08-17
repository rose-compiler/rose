
include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

SUBDIRS = PMDAtoMDA

if ROSE_USE_FADALIB

SUBDIRS += Common RoseToFada

if ROSE_USE_PPL

SUBDIRS += RoseToPPL Schedule CodeGenerator

endif

endif

EXTRA_DIST = README Makefile.external

