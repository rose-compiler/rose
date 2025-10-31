# Liao, 11/10/2009, This is not a project here
#project(ROSETTA)


add_subdirectory(src)


########### install files ###############




#original Makefile.am contents follow:

#include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs
#
#SUBDIRS = src
#
#ACLOCAL = @ACLOCAL@ -I ../config -I ../acmacros
#
#LIBS = @LIBS@ ${ROSETTA_LIBS}
#
#EXTRA_DIST = astNodeList
#
## ***********************************************************************
##                      Documentation Support
## ***********************************************************************
## Don't build a "docs" rule just yet since the dist rule (which builds distributions)
## will call it to generate documentation and this both causes errors and we don't
## want to test the generation of doxygen just yet (too long of a process).
#
#docs:
#	@echo "Skipping the creation of documentation within the docs rule."
#
## sage-docs:
##	@echo "Generate documentation for C++ grammar (using Doxygen)"
##	mkdir -p ../docs/CxxGrammar
##	doxygen $(srcdir)/ROSE_CxxGrammar.cfg
#
#clean-docs:
#	@echo Removing generated documentation
#	rm -rf docs
#
## DQ (7/4/2004)
## We need this to copy the Grammar directory into the distribution once in
## the distribution it is linked in the compile tree to the source tree.
#dist-hook: docs
##	: dist-hook explanations:
##	: Copy non-automake-controled items (Grammar) into the distribution directory.
##	ln -s Grammar ${distdir}
#	for i in Grammar; do    \
#	  cp -rp ${top_srcdir}/src/ROSETTA/$$i ${distdir} &&        \
#	  rm -rf ${distdir}/$$i/CVS ${distdir}/$$i/*~   ;\
#	done
#
## Purify related flags
## PURIFY_USE_WINDOWS = -windows=yes
## PURIFY_FLAGS  = $(PURIFY_USE_WINDOWS) -recursion-depth-limit=40000 -chain-length=12 -first-only=yes -l -at-exit=yes -inuse-at-exit=yes -always-use-cache-dir=yes -cache-dir=/home/dquinlan/tmp
## PURIFY        = purify4.5 $(PURIFY_FLAGS)
#
## PURIFY_DIR    = `purify4.5 -print-home-dir`
## PURIFY_STUBS  = $(PURIFY_DIR)/purify_stubs.a
#
#clean-local:
#	rm -rf Templates.DB ii_files ti_files
#
#distclean-local:
#	rm -rf Templates.DB
#
## AUTOMAKE_OPTIONS = foreign
