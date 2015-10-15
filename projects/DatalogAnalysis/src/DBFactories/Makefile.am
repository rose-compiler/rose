include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

# Automake conditional to control use of ROSE with BDDBDDB
if ROSE_USE_BDDBDDB

AM_CPPFLAGS = -I$(top_builddir) $(ROSE_INCLUDES) -I$(srcdir)/../../include

noinst_LTLIBRARIES = libDBFactories.la
libDBFactories_la_SOURCES = CommonDBFactory.cc  TupleDomain.cc  TupleFactory.cc  TupleRelation.cc

noinst_HEADERS = CommonDBFactory.hh  TupleDomain.hh  TupleFactory.hh  TupleRelation.hh

LDADD = $(LIBS_WITH_RPATH) $(ROSE_LIBS)

check-local:
	@echo "********************************************************************************************"
	@echo "*** ROSE/projects/DocumentationGenerator: make check rule complete (terminated normally) ***"
	@echo "********************************************************************************************"
endif

EXTRA_DIST = CommonDBFactory.cc  TupleDomain.cc  TupleFactory.cc  TupleRelation.cc CommonDBFactory.hh  TupleDomain.hh  TupleFactory.hh  TupleRelation.hh

distclean-local:
	rm -rf testAutoDocumentation_01.docs

clean-local:
	rm -rf $(CXX_TEMPLATE_OBJECTS) Templates.DB ii_files ti_files
	rm -f *.o rose_*.C *.sage.C doxygenComment.C \
              *.C.pdf *.C.edg.pdf *.pdf *.dot *.sidl *.int.C rose_directives_list.txt core
	rm -rf ti_files
	rm -f *.docs
