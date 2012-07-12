include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

SUBDIRS = depInfo depGraph computation outsideInterface slicing driver prepostTransformation

INCLUDES = \
   $(ROSE_INCLUDES) -I${srcdir}/driver -I${srcdir}/prepostTransformation -I$(top_srcdir)/src/util/support -I$(top_srcdir)/src/util/graphs  -I$(top_srcdir)/src/midend/astUtil/astSupport -I$(top_srcdir)/src/midend/astUtil/symbolicVal -I$(top_srcdir)/src/midend/astUtil/astInterface

## The grammar generator (ROSETTA) should use its own template repository
CXX_TEMPLATE_REPOSITORY_PATH = .

clean-local:
	rm -rf Templates.DB ii_files ti_files cxx_templates
	rm -rf rose_mm.C

distclean-local:
	rm -rf Templates.DB


EXTRA_DIST = CMakeLists.txt
