include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

INCLUDES =  $(ROSE_INCLUDES) -I$(top_srcdir)/src/midend/loopProcessing

if DEV_LIBS
lib_LTLIBRARIES = libInterface.la
else
noinst_LTLIBRARIES = libInterface.la
endif

## The grammar generator (ROSETTA) should use its own template repository
CXX_TEMPLATE_REPOSITORY_PATH = .

libInterface_la_SOURCES = CPPAstInterface.C  StmtInfoAnal.C

ROSE_FLAGS =

clean-local:
	rm -rf Templates.DB ii_files ti_files cxx_templates

distclean-local:
	rm -rf Templates.DB

include_HEADERS =  CPPAstInterface.h  StmtInfoAnal.h

#dist-hook:
#	ln -s ../STL-link ${distdir}

# AUTOMAKE_OPTIONS = foreign







