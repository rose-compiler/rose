include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

# . is necessary to ensure building the translator before building the tests
SUBDIRS = . tests

AM_CPPFLAGS = $(ROSE_INCLUDES) 

bin_PROGRAMS = autoPar

autoPar_SOURCES = autoPar.C autoParSupport.C

LDADD = $(LIBS_WITH_RPATH) $(ROSE_LIBS) 

EXTRA_DIST = README makefile.external autoParSupport.h

clean-local:
	rm -rf autoPar

