include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

# Use automake conditional to build Public and Developer distributions
#    Public: includes all of ROSE and binary of EDG work and parts that
#            are more restricted.
#    Developer: This version includes all EDG source code and some parts that
#             might be more restricted.
if ROSE_USE_EDG_SOURCE
    DEVELOPER_DISTRIBUTION_DIRECTORY = developersScratchSpace proposals
endif

# DQ "config" and "src" must preceed any other directories, and 
# "tutorial" and "exampleTranslators" must preceed "docs" since 
# they generate data that is subsequently used in the generated 
# ROSE documentation (examples, input codes, and generated output).
# SUBDIRS = config src projects tests tutorial exampleTranslators docs developersScratchSpace tools proposals scripts
SUBDIRS = config src projects tests tutorial exampleTranslators docs tools scripts $(DEVELOPER_DISTRIBUTION_DIRECTORY)

ACLOCAL_AMFLAGS = -I ./config -I ./acmacros -I ./src/3rdPartyLibraries/libltdl -I /usr/share/aclocal

# Force rose.docs to be make by default 
# (if rose.docs.in is modified)
all-local: rose.docs

# DQ (6/29/2004): I don't think we need this!
# LIBS = @LIBS@ ${SAGE_LIBS}

# It seems that it is better to handle the config directory with its own Makefile.am
# This allows us to hide autoconf accessory files (config.guess, etc.)
# Copy the config directory to the distribution: why?
# (because it has the file: config/Makefile.for.ROSE.includes.and.libs)
# EXTRA_DIST = stamp-h.in stamp-h1.in GNU_HEADERS config
# EXTRA_DIST = stamp-h.in stamp-h1.in rose.docs.in GNU_HEADERS COPYWRITE ROSE_ResearchPapers
EXTRA_DIST = stamp-h.in stamp-h1.in rose.docs.in COPYRIGHT ROSE_ResearchPapers README.OSX

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

# DQ (12/3/2007): Added rose_paths.h so that it would be copied to the install tree.
nodist_include_HEADERS = rose_config.h rose_paths.h

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
DISTCHECK_CONFIGURE_FLAGS = --with-edg_source_code=$(with_edg_source_code) --with-boost=$(ac_boost_path)

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
rename_distribution:
	@echo "DISTCHECK_CONFIGURE_FLAGS = $(DISTCHECK_CONFIGURE_FLAGS)"
	@if test "$(with_edg_source_code)" = true; then \
	  echo "ROSE distribution built using EDG source code (with_edg_source_code = $(with_edg_source_code))"; \
     new_name="$(distdir)-source-with-EDG-source.tar.gz"; \
     echo "copying $(distdir).tar.gz to $$new_name"; \
     cp $(distdir).tar.gz $$new_name; \
	else \
     echo "ROSE distribution built using binaries in place of EDG source code (with_edg_source_code = $(with_edg_source_code))"; \
     new_name="$(distdir)-source-with-EDG-binaries.tar.gz"; \
     echo "copying $(distdir).tar.gz to $$new_name"; \
     cp $(distdir).tar.gz $$new_name; \
   fi

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

DISTCLEANFILES = rose_paths.h

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
	   cd ../projects/compass; $(MAKE) docs; \
   else \
      echo "Please look at the ROSE web site for documentation (www.roseCompiler.org)"; \
   fi


# DQ (7/30/2008): Copy the library to the distribution.
# binary_EDG_dist: distdir
copy_libs_for_binary_distribution: distdir
#	Call rules in the directories where binary libraries are copied (but we have to set DISTDIR)
	cd src/frontend/CxxFrontend; $(MAKE) distdir=$(distdir)/src/frontend/CxxFrontend copy_libs_for_binary_distribution;
	cd src/frontend/CxxFrontend/EDG_3.3/src; $(MAKE) distdir=$(distdir)/src/frontend/CxxFrontend/EDG_3.3/src copy_libs_for_binary_distribution;
	cd src/frontend/CxxFrontend/EDG_3.3/misc; $(MAKE) distdir=$(distdir)/src/frontend/CxxFrontend/EDG_3.3/misc copy_libs_for_binary_distribution;
	cd src/frontend/CxxFrontend/EDG_SAGE_Connection; $(MAKE) distdir=$(distdir)/src/frontend/CxxFrontend/EDG_SAGE_Connection copy_libs_for_binary_distribution;
	cd src/frontend/CxxFrontend/EDG_3.10; $(MAKE) distdir=$(distdir)/src/frontend/CxxFrontend/EDG_3.10 copy_libs_for_binary_distribution;
	cd src/frontend/CxxFrontend/EDG_3.10/src; $(MAKE) distdir=$(distdir)/src/frontend/CxxFrontend/EDG_3.10/src copy_libs_for_binary_distribution;
	cd src/frontend/CxxFrontend/EDG_3.10/src/disp; $(MAKE) distdir=$(distdir)/src/frontend/CxxFrontend/EDG_3.10/src/disp copy_libs_for_binary_distribution;
	cd src/frontend/CxxFrontend/EDG_3.10/misc; $(MAKE) distdir=$(distdir)/src/frontend/CxxFrontend/EDG_3.10/misc copy_libs_for_binary_distribution;
	tardir=$(distdir) && $(am__tar) | GZIP=$(GZIP_ENV) gzip -c >$(distdir)-source-with-EDG-binary.tar.gz
	$(am__remove_distdir)

