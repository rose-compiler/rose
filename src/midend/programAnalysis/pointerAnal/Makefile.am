include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

AM_CPPFLAGS =  $(ROSE_INCLUDES) -I../CFG -I$(top_srcdir)/src/util/support -I$(top_srcdir)/src/util/graphs -I$(top_srcdir)/src/midend/astUtil -I$(top_srcdir)/src/midend/astUtil/astSupport -I$(top_srcdir)/src/midend/astUtil/astInterface 

## The grammar generator (ROSETTA) should use its own template repository
CXX_TEMPLATE_REPOSITORY_PATH = .

EXTRA_DIST = CMakeLists.txt steensgaard.h PtrAnal.h SteensgaardPtrAnal.h

noinst_LTLIBRARIES = libpointerAnal.la
libpointerAnal_la_SOURCES = PtrAnal.C PtrAnalCFG.C

clean-local:
	rm -rf Templates.DB ii_files ti_files cxx_templates

distclean-local:
	rm -rf Templates.DB

pkginclude_HEADERS = steensgaard.h PtrAnal.h








