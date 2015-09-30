include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

SUBDIRS = DBFactories

# Automake conditional to control use of ROSE with BDDBDDB
if ROSE_USE_BDDBDDB

AM_CPPFLAGS = -I$(top_builddir) $(ROSE_INCLUDES) -I$(srcdir)/../include

GENERATED_SOURCE_DIR = $(top_builddir)/projects/DatalogAnalysis/relationTranslatorGenerator
# GENERATED_SOURCE = $(GENERATED_SOURCE_DIR)/output/RelationVisitor.cc

#$(GENERATED_SOURCE):
#	@echo "Need to generated the RelationVisitor.cc and ChildVisitor.cc files (call make)"
#	cd $(GENERATED_SOURCE_DIR); $(MAKE)

#RelationVisitor.cc: ../relationTranslatorGenerator/output/RelationVisitor.cc
#	cp -f ../relationTranslatorGenerator/output/RelationVisitor.cc .

#ChildVisitor.cc:../relationTranslatorGenerator/output/ChildVisitor.cc
#	cp -f ../relationTranslatorGenerator/output/ChildVisitor.cc .

bin_PROGRAMS = relationTranslator

# Some of the source files are generated, so only use these for the distribution.
# DIST_SOURCES = CFGVisitor.cc  main.cc  misc.cc ChildVisitor.cc

# DQ (9/28/2006): Reinclude the sources for RelationVisitor.cc ChildVisitor.cc since they
# are required to make the build work!  We need to figure this problem out, since they
# have to be present for the "make" rule, but are a problem for "make dist" rule.
# DQ (9/24/2006): Don't include the generated source files
# relationTranslator_SOURCES = CFGVisitor.cc  main.cc  misc.cc  RelationVisitor.cc ChildVisitor.cc
# relationTranslator_SOURCES = CFGVisitor.cc  main.cc  misc.cc
# relationTranslator_SOURCES = CFGVisitor.cc  main.cc  misc.cc  RelationVisitor.cc ChildVisitor.cc
# relationTranslator_SOURCES = $(GENERATED_SOURCE) CFGVisitor.cc  main.cc  misc.cc
relationTranslator_SOURCES = CFGVisitor.cc  main.cc  misc.cc ChildVisitor.cc

# relationTranslator_LDADD = DBFactories/libDBFactories.a

# LDADD = $(LIBS_WITH_RPATH) $(ROSE_LIBS) DBFactories/libDBFactories.a
# LDADD = $(LIBS_WITH_RPATH) $(ROSE_LIBS) DBFactories/libDBFactories.la
# LDADD = $(GENERATED_SOURCE_DIR)/RelationVisitor.lo $(GENERATED_SOURCE_DIR)/ChildVisitor.lo $(LIBS_WITH_RPATH) $(ROSE_LIBS) DBFactories/libDBFactories.la
# LDADD = $(LIBS_WITH_RPATH) $(ROSE_LIBS) DBFactories/libDBFactories.la
# LDADD = $(LIBS_WITH_RPATH) $(ROSE_LIBS) DBFactories/libDBFactories.la $(GENERATED_SOURCE_DIR)/astBinaryRelations.la 
# LDADD = $(LIBS_WITH_RPATH) $(ROSE_LIBS) DBFactories/libDBFactories.la
LDADD = $(LIBS_WITH_RPATH) $(ROSE_LIBS) DBFactories/libDBFactories.la $(GENERATED_SOURCE_DIR)/libastBinaryRelations.la 

noinst_HEADERS = CFGVisitor.hh ChildVisitor.hh

check-local:
	@echo "********************************************************************************************"
	@echo "*** ROSE/projects/DatalogAnalysis/src: make check rule complete (terminated normally) ***"
	@echo "********************************************************************************************"
endif

EXTRA_DIST = CFGVisitor.cc  main.cc  misc.cc ChildVisitor.cc CFGVisitor.hh ChildVisitor.hh

distclean-local:

clean-local:
#	rm -f RelationVisitor.cc ChildVisitor.cc
	rm -rf $(CXX_TEMPLATE_OBJECTS) Templates.DB ii_files ti_files
	rm -f *.o rose_*.C *.sage.C doxygenComment.C \
              *.C.pdf *.C.edg.pdf *.pdf *.dot *.sidl *.int.C rose_directives_list.txt core
	rm -rf ti_files
	rm -f *.docs
