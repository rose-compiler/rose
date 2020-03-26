################################################################################
# How to install ROSE without EDG source code on a machine with no network:
#  See http://rosecompiler.org/ROSE_HTML_Reference/group__installing__edg.html
################################################################################

include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

MOSTLYCLEANFILES =
PHONIES =

SUBDIRS = Clang
DIST_SUBDIRS = Clang

#If we have Clang, then we don't need EDG at all.
if !ROSE_USE_CLANG_FRONTEND

# Who to contact if something goes wrong with these scripts
CONTACT_INFO = "Please contact rose-public@nersc.gov to troubleshoot this error."

# The script for computing the EDG hash
EDG_GENERATE_SIG = $(top_srcdir)/scripts/edg-generate-sig

# This is where EDG binaries are stored
ROSE_WEB_SERVER         = http://rosecompiler.org
EDGBIN_LOCAL_TARBALLS   = /nfs/casc/overture/ROSE/git/ROSE_EDG_Binaries
EDGBIN_REMOTE_TARBALLS  = http://edg-binaries.rosecompiler.org
EDGBIN_STAGING_TARBALLS = /nfs/casc/overture/ROSE/git/ROSE_EDG_Binaries-staging

# The EDG hash is stored in a file, which gets around problems of using $(shell...), but means that we shouldn't use
# EDGBIN_SIGNATURE as a target or dependency.
EDGBIN_SIGNATURE_FILE = edg-generate-sig.output
EDGBIN_SIGNATURE      = $(shell cat $(EDGBIN_SIGNATURE_FILE) 2>/dev/null)

# The name of the EDG binary is a bit complex.  Most of these variables come from the configure script. Be careful
# not to use EDGBIN_NAME as a target or dependency in this makefile since make needs to parse those before the
# $(EDGBIN_SIGNATURE_FILE) can be created.
EDGBIN_NAME_PREFIX = roseBinaryEDG-@ROSE_EDG_MAJOR_VERSION_NUMBER@-@ROSE_EDG_MINOR_VERSION_NUMBER@
if ROSE_USE_MICROSOFT_EXTENSIONS
EDGBIN_NAME_SUFFIX = $(build_triplet_without_redhat)-$(FRONTEND_CXX_VENDOR_AND_VERSION2)-microsoft
else
EDGBIN_NAME_SUFFIX = $(build_triplet_without_redhat)-$(FRONTEND_CXX_VENDOR_AND_VERSION2)
endif
EDGBIN_NAME        = $(EDGBIN_NAME_PREFIX)-$(EDGBIN_NAME_SUFFIX)-$(EDGBIN_SIGNATURE)

# All the .PHONY's need to be outside any automake "if" statements otherwise automake complains.
PHONIES += edg-prerequisites binary_edg_tarball upload_edg_binary check-network download-edg show-edg-name show-edg-url show-edg-hash

# Phony target to build EDG dependencies. We have to build EDG's prerequisites in order to compute the EDG hash.
#(see above): .PHONY: edg-prerequisites
edg-prerequisites:
	$(MAKE) -C ../../util
	$(MAKE) -C ../../ROSETTA/src

# Run the edg-generate-sig script to find an appropriate version number.  We don't use make's $(shell...) function
# because it sometimes discards standard error and doesn't cause make to fail if edg-generate-sig fails.  If
# edg-generate-sig fails then remove the edg-generate-sig.output file so that re-running make will fail the same way
# next time.  Also, we don't want to change the modification time of this file if not necessary, since doing so would
# cause targets that depend on it to become out of date.
MOSTLYCLEANFILES += $(EDGBIN_SIGNATURE_FILE)
$(EDGBIN_SIGNATURE_FILE): edg-prerequisites
	$(EDG_GENERATE_SIG) $(top_srcdir) $(top_builddir) >$@.tmp
	diff -q $@ $@.tmp || mv $@.tmp $@

# Phony targets to show useful information. See .PHONY above.
show-edg-hash: $(EDGBIN_SIGNATURE_FILE)
	@cat $(EDGBIN_SIGNATURE_FILE)
show-edg-name: $(EDGBIN_SIGNATURE_FILE)
	@echo $(EDGBIN_NAME).tar.gz
show-edg-url: $(EDGBIN_SIGNATURE_FILE)
	@echo $(EDGBIN_REMOTE_TARBALLS)/$(EDGBIN_NAME).tar.gz


if ROSE_HAS_EDG_SOURCE

###############################################################################################################################
# We have the EDG source code (ROSE_HAS_EDG_SOURCE)
###############################################################################################################################

SUBDIRS += EDG

# When building the library we have to make sure that it's dependencies were built first.  These are the same things that
# go into calculating the EDG binary version number (see scripts/edg-generate-sig)
EDG/libroseEDG.la: edg-prerequisites
	$(MAKE) -C ./EDG libroseEDG.la

# Create EDG binary tarball without using $(EDGBIN_NAME) which hasn't been computed when make parses this rule (but is
# computed by time this rule's commands run). The "upload-" prefix is necessary because we can't have the same target
# in the "if" and "else" part of an automake file, and we'd prefer to use "EDG.tar.gz" at the download end (the "else").
#
# Also, whenever we create the uploadable tarball (regardless of whether we actually upload it) we should also make it
# look like we just downloaded it, because Jenkins tests that the EDG upload/download functionality is working by faking
# an upload/download. It runs "make binary_edg_tarball", then deletes the EDG source code, reconfigures ROSE, and runs
# "make" again to see if we can build ROSE with the EDG binary.
MOSTLYCLEANFILES += upload-EDG.tar.gz
upload-EDG.tar.gz: EDG/libroseEDG.la $(EDGBIN_SIGNATURE_FILE)
	rm -rf $(EDGBIN_NAME)
	mkdir $(EDGBIN_NAME)
	sed "s@^dependency_libs.*@dependency_libs=''@" EDG/libroseEDG.la >$(EDGBIN_NAME)/libroseEDG.la
	cp -r EDG/.libs $(EDGBIN_NAME)
	tar czf $(EDGBIN_NAME).tar.gz $(EDGBIN_NAME)
	rm -rf $(EDGBIN_NAME)
	ln -sf $(EDGBIN_NAME).tar.gz $@             # for upload
	ln -sf $(EDGBIN_NAME).tar.gz EDG.tar.gz     # make it look like we downloaded also


# Phony target to generate a tarball but not upload it
#(see above): .PHONY: binary_edg_tarball
binary_edg_tarball: upload-EDG.tar.gz

# Phony target to upload EDG tarballs to the staging area and the local area. Do not use $(EDGBIN_NAME) in the target or
# dependencies because it hasn't been computed at the time that make parses that part of the rule. It's okay to use it
# in the commands of this rule.
#(see above): .PHONY: upload_edg_binary
upload_edg_binary: binary_edg_tarball
	cp -f $(EDGBIN_NAME).tar.gz $(EDGBIN_STAGING_TARBALLS)/$(EDGBIN_NAME).tar.gz
	cp -f $(EDGBIN_NAME).tar.gz $(EDGBIN_LOCAL_TARBALLS)/$(EDGBIN_NAME).tar.gz

else

###############################################################################################################################
# We don't have the EDG source code (!ROSE_HAS_EDG_SOURCE)
###############################################################################################################################

# Figure out how to download a binary version of EDG that the ROSE team distributes.  The ROSE team distributes EDG
# binaries by compiling EDG within particular revisions of ROSE and then uploading the EDG binaries to a public web
# server.


all-am: EDG/libroseEDG.la

EDG_HELP_URL="https://github.com/rose-compiler/rose/wiki/EDG-Installation-Instructions"

#(see above): .PHONY: check-network
check-network:
	@if ! ping -c 1 google.com 2>&1 >/dev/null; then						\
	    echo "Unable to ping google.com" >&2;							\
	    echo "ROSE requires an internet connection to download EDG binary" >&2;			\
	    echo "For more information on EDG binaries visit" >&2;			\
	    echo "$(EDG_HELP_URL)" >&2;	\
	    exit 1;											\
	fi
	@if ! wget -q --spider -t 3 --wait=1 $(ROSE_WEB_SERVER); then					\
	    echo "ROSE web server $(ROSE_WEB_SERVER) is not currently reachable" >&2;			\
	    echo "$(CONTACT_INFO)" >&2;									\
	    echo "For more information on EDG binaries visit" >&2;			\
	    echo "$(EDG_HELP_URL)" >&2;	\
	    exit 1;											\
	fi

# Only here for backward compatibility. Maybe we don't need it?
network_checking: check-network

# Try to get the EDG binary tarball if we don't have it.  Team members might be able to get it from a local directory,
# but others will need to go to a Web server.  We use wget rather than curl because curl was sometimes placing error
# messages inside the tarball. Don't use $(EDGBIN_NAME) in the target or dependencies because it hasn't been computed
# yet when make parses that part of this rule; it's okay to use it in the commands though.
EDG.tar.gz: $(EDGBIN_SIGNATURE_FILE) 
	@if [ "$(EDGBIN_SIGNATURE)" = "" ]; then							\
	    echo "error: empty EDG binary compatibility signature in $(EDGBIN_SIGNATURE_FILE)." >&2;	\
	    echo "$(CONTACT_INFO)" >&2;									\
	    exit 1;											\
	fi
	
	@if [ -r "$(EDGBIN_NAME).tar.gz" ]; then								\
	    echo "EDG binary tarball appears to have been downloaded already: $(EDGBIN_NAME).tar.gz";		\
	else													\
	    $(MAKE) check-network || exit 1;									\
	    echo "Downloading EDG binary tarball: $(EDGBIN_NAME).tar.gz";					\
	    if ! wget -q --spider -t 3 --wait=1 $(EDGBIN_REMOTE_TARBALLS)/$(EDGBIN_NAME).tar.gz; then		\
	        echo "The needed EDG binary is unavailable." >&2;						\ 
	        echo "This is likely because there is not a binary distributed for your current compiler." >&2;	\ 
	        echo "For more information on EDG binaries visit" >&2;						\
	        echo "$(EDG_HELP_URL)" >&2;									\
	        echo "$(CONTACT_INFO)" >&2;									\
	        exit 1;												\
	    fi;													\
	    wget -O "$(EDGBIN_NAME).tar.gz" "$(EDGBIN_REMOTE_TARBALLS)/$(EDGBIN_NAME).tar.gz";			\
	    touch $(EDGBIN_NAME).tar.gz;									\
	fi

	ln -sf $(EDGBIN_NAME).tar.gz $@

# This target can be used to explicitly download the EDG tarball without needing to know its name.
#(see above): .PHONY: download-edg
download-edg: EDG.tar.gz
	@echo
	@echo "The EDG tarball is $(EDGBIN_NAME).tar.gz"
	@echo "To compile on a system with no network, follow the instructions here:"
	@echo "  http://rosecompiler.org/ROSE_HTML_Reference/group__installing__edg.html"

# Unpack the EDG tarball. Note: $(am__untar) comes from configure and if it's not set then there's something wrong with configure.
EDG/libroseEDG.la: EDG.tar.gz
	GZIP=$(GZIP_ENV) gunzip -c $< |$(am__untar)
	rm -rf EDG
	mkdir -p EDG
	mv $(EDGBIN_NAME)/libroseEDG.* EDG/.
	mv $(EDGBIN_NAME)/.libs EDG/.
	rmdir $(EDGBIN_NAME)
	touch EDG/libroseEDG.la

installdir = ${datadir}

MOSTLYCLEANFILES += edg_binaries.txt EDG.tar.gz

endif

###############################################################################################################################
# Stuff that's common to both uploading and downloading EDG tarballs
###############################################################################################################################

# Cleanup
MOSTLYCLEANFILES += EDG/libroseEDG.la
endif

clean-local:
# DQ (3/1/2017): Need to be able to run make clean whenClang frontend support is defined.
#	rm -rf EDG/.libs $(EDGBIN_NAME_PREFIX)-*.tar.gz
	rm -rf EDG/.libs 
if !ROSE_USE_CLANG_FRONTEND
	rm -rf $(EDGBIN_NAME_PREFIX)-*.tar.gz
endif

.PHONY: $(PHONIES)
