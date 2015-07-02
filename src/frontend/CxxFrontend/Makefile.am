################################################################################
# How to install ROSE without EDG source code on a machine with no network:
#   On a machine with a network connection:
#     1. git-clone the ROSE sources
#     2. copy the ROSE source tree to the other machine
#     3. create a build directory and configure (minimal config is okay)
#     4. in $BUILD/src/frontend/CxxFrontend run "make download-edg"
#     5. follow the instructions to copy some files to the other machine
# 
#   On the non-networked machine:
#     1. create a build directory and configure like usual
#     2. copy the two files and symlink from step 5 above (*)
#     3. run make
# 
#   *Note: the files must be copied such that the mtime for the
#    tarball is not earlier than the other file, or else make will try
#    to download on the non-networked machine.  The easiest way to
#    ensure proper mtimes is to touch(1) the tarball after copying
#    the files.
################################################################################

include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs
MOSTLYCLEANFILES =

SUBDIRS = Clang
DIST_SUBDIRS = Clang

#If we have Clang, then we don't need EDG at all.
if !ROSE_USE_CLANG_FRONTEND

# This is where EDG binaries are stored
EDGBIN_LOCAL_TARBALLS=/nfs/casc/overture/ROSE/git/ROSE_EDG_Binaries
EDGBIN_REMOTE_TARBALLS=http://www.rosecompiler.org/edg_binaries
EDGBIN_STAGING_TARBALLS=/nfs/casc/overture/ROSE/git/ROSE_EDG_Binaries-staging

# The name of the EDG binary is a bit complex.  Most of these variables come from the configure script.  The version
# number is different depending on whether we're building an EDG binary, or trying to download one: when building the
# EDG binary, the version number is based on the version of ROSE in which EDG is being built, but when downloading an
# EDG binary the version number is computed as a function of this revision of ROSE and the EDG versions that are
# available on the EDG binary web site.  So we'll put all the parts together in the 'if' and 'else' cases.
EDGBIN_NAME_PREFIX=roseBinaryEDG-@ROSE_EDG_MAJOR_VERSION_NUMBER@-@ROSE_EDG_MINOR_VERSION_NUMBER@
EDGBIN_NAME_SUFFIX=$(build_triplet_without_redhat)-$(GENERATED_COMPILER_NAME_AND_VERSION_SUFFIX)

if ROSE_HAS_EDG_SOURCE

###############################################################################################################################
# We have the EDG source code (ROSE_HAS_EDG_SOURCE)
###############################################################################################################################
SUBDIRS += EDG

# The name that we use when building EDG is always based on the version of ROSE where we're building (i.e., "this" ROSE)
EDGBIN_THIS_NAME:=$(EDGBIN_NAME_PREFIX)-$(EDGBIN_NAME_SUFFIX)-$(shell $(top_srcdir)/scripts/edg-generate-sig)

# When building the library we have to make sure that it's dependencies were built first.  These are the same things that
# go into calculating the EDG binary version number (see scripts/edg-generate-sig)
EDG/libroseEDG.la:
	$(MAKE) -C ../../util
	$(MAKE) -C ../../ROSETTA/src
	$(MAKE) -C ./EDG libroseEDG.la

# Create EDG binary tarball.
$(EDGBIN_THIS_NAME).tar.gz: EDG/libroseEDG.la
	rm -rf $(EDGBIN_THIS_NAME)
	mkdir $(EDGBIN_THIS_NAME)
	sed "s@^dependency_libs.*@dependency_libs=''@" EDG/libroseEDG.la >$(EDGBIN_THIS_NAME)/libroseEDG.la
	cp -r EDG/.libs $(EDGBIN_THIS_NAME)
	tar czf $(EDGBIN_THIS_NAME).tar.gz $(EDGBIN_THIS_NAME)
	rm -rf $(EDGBIN_THIS_NAME)

# Upload EDG tarballs to the staging area and the local area
EDGBIN_DESTINATIONS=					      \
	$(EDGBIN_STAGING_TARBALLS)/$(EDGBIN_THIS_NAME).tar.gz \
	$(EDGBIN_LOCAL_TARBALLS)/$(EDGBIN_THIS_NAME).tar.gz
$(EDGBIN_DESTINATIONS): $(EDGBIN_THIS_NAME).tar.gz
	cp $< $@

# Phony target to generate a tarball but not upload it
.PHONY: binary_edg_tarball
binary_edg_tarball: $(EDGBIN_THIS_NAME).tar.gz

# Phony target to upload the tarball
.PHONY: upload_edg_binary
upload_edg_binary: $(EDGBIN_DESTINATIONS)

else

###############################################################################################################################
# We don't have the EDG source code (!ROSE_HAS_EDG_SOURCE)
###############################################################################################################################
# Figure out how to download a binary version o EDG that the ROSE team distributes.  The ROSE team distributes EDG
# binaries by compiling EDG within particular revisions of ROSE and then uploading the EDG binaries to a public web
# server.  However, our revision of ROSE might not exactly match the revision that was used to generate the EDG binary.
# We use the edg-generate-sig script to choose the best version from a list of available versions.  If there's a version
# number (SHA1) already stored in the bincompat-sig file in the top of the source tree then edg-generate-sig will use
# that rather than doing a bunch of git commands.

# Be sure to use '=' and not ':=' because these need to expand after we've created edg-generate-sig.output
EDGBIN_SIGNATURE = $(shell cat edg-generate-sig.output 2>/dev/null)
EDGBIN_BEST_NAME = $(EDGBIN_NAME_PREFIX)-$(EDGBIN_NAME_SUFFIX)-$(EDGBIN_SIGNATURE)

# This one must be named "EDGBIN_NTRIES" because it's mentioned in an error message from the edg-generate-sig script.
# Users are directed to override this variable with a higher value (caveat emptor) if an EDG version cannot be found.
EDGBIN_NTRIES = 1
all-am: EDG/libroseEDG.la

network_checking:
	wget -q --spider -t 3 --wait=5 http://www.rosecompiler.or ; \ 
	if [ $$? -eq 0 ]; then \
	echo "ROSE WebServer is Reachable"; \
	else \
	echo "ROSE WebServer is NOT Reachable"; \
	fi


# Run the edg-generate-sig script to find an appropriate version number.  We cannot use make's $(shell...) function because
# it sometimes discards standard error and doesn't cause make to fail if the command fails.  If edg-generate-sig fails then
# remove the edg-generate-sig.output file so that re-running make will fail the same way next time.
edg-generate-sig.output: 
	@wget -q --spider -t 3 --wait=5 http://www.rosecompiler.or ; \ 
	if [ $$? -eq 0 ]; then \
	if ! $$(cd $(top_srcdir) && pwd)/scripts/edg-generate-sig --find --progress --ntries=$(EDGBIN_NTRIES) >$@; then \
	    rm -f $@ ; \
	    exit 1; \
	fi; \ 
	else \
	if ! $$(cd $(top_srcdir) && pwd)/scripts/edg-generate-sig --no-fetch --progress --ntries=$(EDGBIN_NTRIES) >$@; then \
	    rm -f $@ ; \
	    exit 1; \
	fi; \
	fi

# Try to get the EDG binary tarball if we don't have it.  Team members might be able to get it from a local directory,
# but others will need to go to a Web server.  We use wget rather than curl because curl was sometimes placing error
# messages inside the tarball.  Note, we can't have two make targets with the same name even if they are in separate parts
# of an automake 'if' statement; so we add a 'download-' prefix to this one.
EDG.tar.gz:  edg-generate-sig.output
	@if [ "$(EDGBIN_SIGNATURE)" = "" ]; then \
	    echo "Invalid EDG binary compatibility signature "$(EDGBIN_SIGNATURE)" in filename '$(EDGBIN_BEST_NAME)'." >&2; \
	    echo "Please contact rose-public@nersc.gov to troubleshoot this error message." >&2; \
	    exit 1; \
	fi
	@if [ ! -f $@ ]; then \
	echo "$@ not found, checking "$(EDGBIN_BEST_NAME).tar.gz" "; \
		if [ ! -f "$(EDGBIN_BEST_NAME).tar.gz" ]; then \
		echo ""$(EDGBIN_BEST_NAME).tar.gz" not found, download it!";\
			wget -q --spider -t 3 --wait=5 $(EDGBIN_REMOTE_TARBALLS)/$(EDGBIN_BEST_NAME).tar.gz ; \
			if [ $$? -eq 0 ]; then \
				wget -O "$(EDGBIN_BEST_NAME).tar.gz" "$(EDGBIN_REMOTE_TARBALLS)/$(EDGBIN_BEST_NAME).tar.gz"; \
			else \
				echo "Destination not reachable: download EDG binary from the following link when the network is available:"; \
				echo "$(EDGBIN_REMOTE_TARBALLS)/$(EDGBIN_BEST_NAME).tar.gz";\
			fi; \
		fi; \
		touch $(EDGBIN_BEST_NAME).tar.gz; \
		echo "Symbolic link "$(EDGBIN_BEST_NAME).tar.gz" to $@!";\
		ln -sf $(EDGBIN_BEST_NAME).tar.gz $@; \
	fi

# This target can be used to explicitly download the EDG tarball without needing to know its name.
.PHONY: download-edg
download-edg: EDG.tar.gz
	@echo
	@echo "The EDG tarball is $(EDGBIN_BEST_NAME).tar.gz"
	@echo "To compile on a system with no network, copy this EDG tarball, the EDG.tar.gz symlink, and"
	@echo "the edg-generate-sig.output file to the $$ROSE_BUILD/src/frontend/CxxFrontend directory on"
	@echo "the machine with no network."

# This target is for the user to obtain the EDG tarball name without downloading anything
.PHONY: show-edg-name show-edg-url
show-edg-name: edg-generate-sig.output
	@echo $(EDGBIN_BEST_NAME).tar.gz
show-edg-url: edg-generate-sig.output
	@echo $(EDGBIN_REMOTE_TARBALLS)/$(EDGBIN_BEST_NAME).tar.gz

# Unpack the EDG tarball. Note: $(am__untar) comes from configure and if it's not set then there's something wrong with configure.
EDG/libroseEDG.la: EDG.tar.gz
	GZIP=$(GZIP_ENV) gunzip -c $< |$(am__untar)
	rm -rf EDG
	mkdir -p EDG
	mv $(EDGBIN_BEST_NAME)/libroseEDG.* EDG/.
	mv $(EDGBIN_BEST_NAME)/.libs EDG/.
	rmdir $(EDGBIN_BEST_NAME)
	touch EDG/libroseEDG.la

installdir = ${datadir}

MOSTLYCLEANFILES += edg-generate-sig.output edg_binaries.txt EDG.tar.gz

endif

###############################################################################################################################
# Stuff that's common to both uploading and downloading EDG tarballs
###############################################################################################################################

# Cleanup
MOSTLYCLEANFILES += EDG/libroseEDG.la
clean-local:
	rm -rf EDG/.libs {download-,}$(EDGBIN_NAME_PREFIX)-*.tar.gz
endif
