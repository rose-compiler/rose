## Process this file with automake to create Makefile.in.

INCLUDES = -I$(ROSE_INCLUDE_DIR) -I$(PAG_INCLUDE_DIR)/pag -I$(PAG_INCLUDE_DIR)/pag/analyzer -I$(top_srcdir)/src/analyzer/astaccess  -I$(top_srcdir)/src/analyzer/astaccess/satire -I$(top_srcdir)/src/analyzer/icfgbuilder/satire -I$(top_srcdir)/src/analyzer/commandlineinterface/satire

# FIXME
EXTRA_DIST=main.C
#bin_PROGRAMS = icfgbuilder
icfgbuilder_SOURCES = main.C
icfgbuilder_LDADD = \
       -L$(ROSE_LIB_DIR) -Wl,-rpath $(ROSE_LIB_DIR) -lrose \
       -L$(PAG_LIB_DIR)  -lpag \
       ../../src/libsatire.a ../../src/analyzer/astaccess/syntree.o

installcheck-local:
	echo FIXME #icfgbuilder -I$(top_srcdir)/examples/test/...
