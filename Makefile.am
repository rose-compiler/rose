include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs
include $(top_srcdir)/projects/compass/src/compassSupport/compass_dirs.inc

# Use automake conditional to build Public and Developer distributions
#    Public: includes all of ROSE and binary of EDG work and parts that
#            are more restricted.
#    Developer: This version includes all EDG source code and some parts that
#             might be more restricted.
# This next line will be removed for binary distributions, leaving the variable empty
DEVELOPER_DISTRIBUTION_DIRECTORY = developersScratchSpace proposals

# DQ (1/18/2009): Added demo directory to support showoffs :-).
# DQ "config" and "src" must preceed any other directories, and 
# "tutorial" and "exampleTranslators" must preceed "docs" since 
# they generate data that is subsequently used in the generated 
# ROSE documentation (examples, input codes, and generated output).
# SUBDIRS = config src projects tests tutorial exampleTranslators docs developersScratchSpace tools proposals scripts
SUBDIRS = libltdl config src projects tests tutorial exampleTranslators docs tools scripts demo $(DEVELOPER_DISTRIBUTION_DIRECTORY)


ACLOCAL_AMFLAGS = -I ./config -I ./acmacros -I ./libltdl -I /usr/share/aclocal

# Force rose.docs to be make by default 
# (if rose.docs.in is modified)
all-local: rose.docs 

# DQ (3/21/2009): This takes several minutes under Cygwin, so skip these test there.
if ROSE_BUILD_OS_IS_CYGWIN
  PROJECT_SOURCE_CODE_TESTS = 
else

# Liao, 6/27/2009 skip the test if PCH is not used
if ROSE_PCH 
PROJECT_SOURCE_CODE_TESTS = detect_roseh: detect_rosec
else  
PROJECT_SOURCE_CODE_TESTS = 
endif

endif

# all: detect_roseh detect_rosec
all: $(PROJECT_SOURCE_CODE_TESTS)

# tps: script that checks that no header file includes rose.h
# rpm: exclude build directories (they contain "include-staging") since ROSE itself creates some source files.
detect_roseh:
	touch $(top_builddir)/src/frontend/SageIII/rose.h.pch
	rm $(top_builddir)/src/frontend/SageIII/rose.h.pch
	@echo "***************************************************************************************"
	@echo "***** ROSE uses pre-compiled headers and needs to check integrity. ********************"
	@echo "***** ROSE ensures that no header files includes rose.h and that   ********************"
	@echo "***** the first header file in a source file is rose.h (if used)   ********************"
	@echo "***************************************************************************************"
	@echo Testing for rose.h in header files...
	@if test `find $(top_srcdir) \( -type d -exec test -d "{}/include-staging" \; -prune \) -o -type f \( -name "*.h" -o -name "*.hh" \) -exec grep -H "#include \"rose.h\"\|#include <rose.h>" {} \; |  grep -v "//#include \"rose.h\"\|//#include <rose.h>" | grep -v "// #include \"rose.h\"\|// #include <rose.h>" | wc | awk '{print \$$1}'` -eq 0; then echo "rose.h header check passed."; else echo "header files exist that include rose.h"; find $(top_srcdir) \( -type d -exec test -d "{}/include-staging" \; -prune \) -o -type f \( -name "*.h" -o -name "*.hh" \) -exec grep -H "#include \"rose.h\"\|#include <rose.h>" {} \; | grep -v "//#include \"rose.h\"\|//#include <rose.h>" | grep -v "// #include \"rose.h\"\|// #include <rose.h>" ; exit 1; fi

# tps: added script from Andreas with small modifications - find all C files that dont include rose in the top of the file 
# rpm: exclude build directories (they contain "include-staging") since ROSE itself creates some source files.
detect_rosec:
	@echo Testing for rose.h in source files...
	@if test `find $(top_srcdir) \( -type d -exec test -d "{}/include-staging" \; -prune \) -o -type f \( -name "*.C" -o -name "*.cpp" -o -name "*.cc" \) -exec grep -H "#include" {} \; | grep "\"rose.h\"\|<rose.h>" | grep -v "\/\/" | cut -d ':' -f 1 | grep -v "preproc-fortran.cc" | grep -v "preproc-fortran-fixed.cc" | uniq | xargs -n 1 grep -m 1 -HP "^#include" | grep -v "rose.h\|mpi.h" | wc | awk '{print \$$1}'` -eq 0; then echo "rose.h source file check passed.";  else echo "There are source files that dont include rose.h as the first header file."; find $(top_srcdir) \( -type d -exec test -d "{}/include-staging" \; -prune \) -o -type f \( -name "*.C" -o -name "*.cpp" -o -name "*.cc" \) -exec grep -H "#include" {} \; | grep "\"rose.h\"\|<rose.h>" | grep -v "\/\/" | cut -d ':' -f 1 | grep -v "preproc-fortran.cc" | grep -v "preproc-fortran-fixed.cc" | uniq | xargs -n 1 grep -m 1 -HP "^#include" | grep -v "rose.h\|mpi.h"; exit 1; fi


# DQ (3/9/2009): Additional things that we can enforce within ROSE (written by Andreas).
# Additional rules to enforce:
#    1) Finding using declarations in header files:
#       find -name "*.h" -exec grep -nPH "^[ \t]*using[ \t]+namespace" {} \;
#    2) Finding link dependences in Makefile.am files:
#       find -name Makefile.am -exec grep -nPH "^[ \t]*[a-zA-Z]*[_]?LDADD[ \t]*" {} \;


# DQ (6/29/2004): I don't think we need this!
# LIBS = @LIBS@ ${SAGE_LIBS}

# It seems that it is better to handle the config directory with its own Makefile.am
# This allows us to hide autoconf accessory files (config.guess, etc.)
# Copy the config directory to the distribution: why?
# (because it has the file: config/Makefile.for.ROSE.includes.and.libs)
# EXTRA_DIST = stamp-h.in stamp-h1.in GNU_HEADERS config
# EXTRA_DIST = stamp-h.in stamp-h1.in rose.docs.in GNU_HEADERS COPYWRITE ROSE_ResearchPapers
EXTRA_DIST = stamp-h.in stamp-h1.in rose.docs.in COPYRIGHT ROSE_ResearchPapers README.OSX README.Cygwin build ChangeLog2

# At some point we want to put tals into the distribution (likely just PDF files, but not yet).
# ROSE_Talks

# clean-local explanation:
# Remove template repositories.  No source code is compiled, but configuration
#   tests may have created template repositories here.
clean-local:
	rm -rf Templates.DB ii_files ti_files *.csv

# DQ (9/8/2006): Modified to remove backend specific header files
uninstall-local:
	rm -rf $(DESTDIR)$(includedir)/*_HEADERS

distclean-local:
	-rm -rf $(top_builddir)/include-staging/

nodist_include_HEADERS = rose_config.h

# DQ (4/5/2009): This rule is not called when compiling this directory, it is only
# called when compiling the /src/util directory (perhaps it should be located in 
# that directory's Makefile.am.
# DQ (12/3/2007): Added rose_paths.h so that it would be copied to the install tree.
# JJW (8/25/2008): Changed to a .c file
include $(top_srcdir)/config/build_rose_paths.Makefile

check-local:
	@echo '************************************************************'
	@echo '****** make check rule complete (terminated normally) ******'
	@echo '************************************************************'

install-exec-local:
	@echo '*****************************************************************'
	@echo '***** make install-exec rule complete (terminated normally) *****'
	@echo '*****************************************************************'

install-data-local:
	-rm -rf $(DESTDIR)$(includedir)/*_HEADERS
	-mkdir $(DESTDIR)$(includedir)
	cp -R $(top_builddir)/include-staging/* $(DESTDIR)$(includedir)/
	@echo '*****************************************************************'
	@echo '***** make install-data rule complete (terminated normally) *****'
	@echo '*****************************************************************'

# DQ (4/22/2005): Set this so that make distcheck will use the same 
# --with-edg_source_code=true/false option as were used at configure.
# JJW (5/14/2008): Add the Boost flag the same way, using an internal
# variable from AX_BOOST_* to find the argument to --with-boost when
# this copy of ROSE was originally configured.
DISTCHECK_CONFIGURE_FLAGS = --with-boost=$(ac_boost_path)

# DQ (7/25/2008): If ROSE was originallly configured with Fortran (by tuning 
# on the java support) then make sure it is tested as part of the distcheck rule.
if ROSE_USE_OPEN_FORTRAN_PARSER
   DISTCHECK_CONFIGURE_FLAGS += --with-java
endif

# DQ (7/25/2008): If ROSE was originallly configured to use the HPC Toolkit
# then make sure it is tested as part of the distcheck rule.
if ROSE_BUILD_ROSEHPCT
   DISTCHECK_CONFIGURE_FLAGS += --enable-rosehpct
endif

# DQ (7/25/2008): If ROSE was originallly configured to use the javaport
# work (generation of Java interface functions for ROSE using SWIG) then 
# make sure it is tested as part of the distcheck rule.
if ENABLE_JAVAPORT
   DISTCHECK_CONFIGURE_FLAGS += --with-javaport
endif

# DQ (4/22/2005): To make clear when binaries are in the distribution as 
# opposed to EDG source code we rename the final generated distribution.
# JJW (8/3/2008): This is only used for source distributions now --
# source_with_binary_edg_dist does its own renaming
rename_distribution:
	new_name="$(distdir)-source-with-EDG-source.tar.gz"; \
	cp $(distdir).tar.gz $$new_name

# We reset distdir as shown on
# http://jcalcote.wordpress.com/2008/02/23/autoconf-macros-exposed-at3/ to add the SVN
# revision number
distdir = $(PACKAGE)-$(VERSION)$(DOT_SVNREV)

# Automake hook-ish rule to be run after dist rule (but hook runs before gzip so we can't use hook)
# We could build our own ROSE specific wrapper for dist and distcheck.
dist-after:
	@echo "Running dist hook rule ..."
	@$(MAKE) rename_distribution

# hook rule to be run after distcheck rule
distcheck-after:
	@echo "Running distcheck hook rule ..."
	@$(MAKE) rename_distribution

CLEANFILES = rose_binary_compatibility_signature rose_binary_compatibility_signature_src

find_rose_lock_files:
#	du -a | grep rose_performance_report_lockfile
	find . -name rose_performance_report_lockfile -print

# New rule to simplify generation of documentation (we want to avoid using the
# automake generated "docs" rule because the generation of documentation is
# dependent upon separate tools which the user might not have available
# (true for bothe the LaTeX and html documentation).
# DQ (7/23/2004): Since there is a "docs" subdirectory we need to force the build!
FORCE_DOCS_TO_BE_MADE:

# DQ (7/25/2008): Running "make docs" introduces dependences that are inappropriate
# for a distribution.
docs: FORCE_DOCS_TO_BE_MADE
	@if [ -f $(top_srcdir)/src/midend/midend.docs ]; then \
	   echo "    Generate all possible documentation cd docs; make documentation;"; \
	   cd tutorial; $(MAKE) docs; \
	   cd ../docs; $(MAKE) docs; \
	   cd $(compass_build_tooldir)/compass; $(MAKE) docs; \
   else \
      echo "Please look at the ROSE web site for documentation (www.roseCompiler.org)"; \
   fi


clean_binary_distribution: distdir
	rm -rf $(distdir)/src/frontend/CxxFrontend/EDG*
	rm -rf $(distdir)/src/frontend/CxxFrontend/Makefile.am # Will be replaced with another version in the binary_edg_tarball rule
	rm -rf $(distdir)/proposals
	rm -rf $(distdir)/developersScratchSpace
# Dan said this is OK 5/27/2009	
#	rm -rf $(distdir)/projects/assemblyToSourceAst
	rm -rf $(distdir)/docs/testDoxygen

if BINARY_EDG_TARBALL_ENABLED

rose_binary_compatibility_signature:
	find $(srcdir)/src/ROSETTA $(srcdir)/src/frontend/CxxFrontend \( -name \*.C -o -name \*.c -o -name \*.h -o -name \*.code -o -name \*.macro -o -name astNodeList -o -name \*.yy \) -a -print | env LC_ALL=C sort | xargs cat -ve | \
	@MD5@ | cut -f 1 -d \  > rose_binary_compatibility_signature

.PHONY : rose_binary_compatibility_signature # Always rebuild the file when needed

binary_edg_tarball: rose_binary_compatibility_signature
	cd src/ROSETTA/src; $(MAKE)
	cd src/frontend/CxxFrontend; $(MAKE)
	BINARY_EDG_DIRNAME=roseBinaryEDG-$(build_triplet_without_redhat)-`cat rose_binary_compatibility_signature` && \
	rm -rf $${BINARY_EDG_DIRNAME} && \
	mkdir $${BINARY_EDG_DIRNAME} && \
	sed "s@^dependency_libs.*@dependency_libs=''@" src/frontend/CxxFrontend/libroseEDG.la > $${BINARY_EDG_DIRNAME}/libroseEDG.la && \
	cp -Rp src/frontend/CxxFrontend/.libs $${BINARY_EDG_DIRNAME}/ && \
	tardir=$${BINARY_EDG_DIRNAME} && $(am__tar) | GZIP=$(GZIP_ENV) gzip -c >$${BINARY_EDG_DIRNAME}.tar.gz && \
	rm -rf $${BINARY_EDG_DIRNAME}

copy_binary_edg_tarball_to_source_tree: binary_edg_tarball rose_binary_compatibility_signature
	BINARY_EDG_DIRNAME=roseBinaryEDG-$(build_triplet_without_redhat)-`cat rose_binary_compatibility_signature` && \
	$(top_srcdir)/scripts/copy_binary_edg_tarball_to_source_tree $(top_srcdir) "@build_triplet_without_redhat@"

copy_binary_edg_tarball_to_source_tree_svn: binary_edg_tarball rose_binary_compatibility_signature
	BINARY_EDG_DIRNAME=roseBinaryEDG-$(build_triplet_without_redhat)-`cat rose_binary_compatibility_signature` && \
	$(top_srcdir)/scripts/copy_binary_edg_tarball_to_source_tree_svn `sed -n 5p $(top_srcdir)/.svn/entries` "@build_triplet_without_redhat@"

source_with_binary_edg_dist: rose_binary_compatibility_signature
	@ROSE_BINARY_COMPATIBILITY_SIGNATURE=`cat rose_binary_compatibility_signature` && \
	if test `ls -1 $(srcdir)/src/frontend/CxxFrontend/roseBinaryEDG-*-$${ROSE_BINARY_COMPATIBILITY_SIGNATURE}.tar.gz | wc -l` -eq 0; then echo "Error: Could not find any binary versions of EDG with signature $${ROSE_BINARY_COMPATIBILITY_SIGNATURE} in $(srcdir)/src/frontend/CxxFrontend.  This distribution cannot be made because it would not be usable on any platform." >&2; exit 1; fi && \
	if test `ls -1 $(srcdir)/src/frontend/CxxFrontend/roseBinaryEDG-*-*.tar.gz | grep -v $${ROSE_BINARY_COMPATIBILITY_SIGNATURE} | wc -l` -ne 0; then echo "Error: The following obsolete binary versions of EDG were found in $(srcdir)/src/frontend/CxxFrontend." >&2; ls -1 $(srcdir)/src/frontend/CxxFrontend/roseBinaryEDG-*-*.tar.gz | grep -v $${ROSE_BINARY_COMPATIBILITY_SIGNATURE} >&2; exit 1; fi && \
	$(MAKE) clean_binary_distribution && \
	cp $(srcdir)/src/frontend/CxxFrontend/roseBinaryEDG-*-$${ROSE_BINARY_COMPATIBILITY_SIGNATURE}.tar.gz $(distdir)/src/frontend/CxxFrontend && \
	cat $(top_srcdir)/configure.in | \
		grep -v '^developersScratchSpace/' | \
		grep -v '^proposals/' | \
		grep -v '^src/frontend/CxxFrontend/EDG' | \
		grep -v '^docs/testDoxygen/' | \
		sed s/RELEASE_BINARY_COMPATIBILITY_SIGNATURE/$${ROSE_BINARY_COMPATIBILITY_SIGNATURE}/g > \
		$(distdir)/configure.in
	cat $(top_srcdir)/Makefile.am | \
		grep -v 'DEVELOPER_DISTRIBUTION_DIRECTORY *=' > \
		$(distdir)/Makefile.am # Remove SUBDIRS references to developersScratchSpace and proposals
	cat $(top_srcdir)/docs/Makefile.am | \
		grep -v 'DEVELOPER_DISTRIBUTION_DIRECTORY *=' > \
		$(distdir)/docs/Makefile.am # Remove SUBDIRS reference to testDoxygen
	cp $(top_srcdir)/src/frontend/CxxFrontend/Makefile.am.forBinaryEDG $(distdir)/src/frontend/CxxFrontend/Makefile.am
	echo "" >> $(distdir)/src/frontend/CxxFrontend/Makefile.am
	cd $(distdir)/src/frontend/CxxFrontend; echo "EXTRA_DIST = `echo *.tar.gz`" >> Makefile.am
	cd $(distdir) && ./build
	find $(distdir)/ -name Makefile.in~ | xargs rm -rf
	find $(distdir)/ -depth -name .svn -exec rm -rf {} \;
	find $(distdir)/ -depth -name autom4te.cache -exec rm -rf {} \;
	tardir=$(distdir) && $(am__tar) | GZIP=$(GZIP_ENV) gzip -c >$(PACKAGE)-$(VERSION)-source-with-EDG-binary$(DOT_SVNREV).tar.gz
	$(am__remove_distdir)

endif
