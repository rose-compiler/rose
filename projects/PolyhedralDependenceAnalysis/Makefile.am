include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

# . is necessary to ensure building the translator before building the tests
SUBDIRS = PMDAtoMDA

if ROSE_USE_FADALIB

SUBDIRS += Common RoseToFada

if ROSE_USE_PPL

SUBDIRS += RoseToPPL Schedule

endif

endif

EXTRA_DIST = README Makefile.external

