include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

release_binary_compatibility_signature=$(shell ${top_srcdir}/scripts/bincompat-sig)
BINARY_EDG_DIRNAME=roseBinaryEDG-${build_triplet_without_redhat}-${GENERATED_COMPILER_NAME_AND_VERSION_SUFFIX}-${release_binary_compatibility_signature}
BINARY_PATH=/usr/casc/overture/ROSE/git/ROSE_EDG_Binaries
BINARY_URL=file://$(BINARY_PATH)

ALL_ARCHS = i686-pc-linux-gnu x86_64-pc-linux-gnu
ALL_COMPILERS = GNU-3.4 GNU-4.0 GNU-4.1 GNU-4.2 GNU-4.3 GNU-4.4

#fetch tar-ball 
%.tar.gz:
	curl ${BINARY_URL}/$@  -o $@

# This line includes different EDG tarballs depending on the value of the environment variable DIST_EDG
# Default
#    DIST_EDG == current
DIST_EDG = current

# if DIST_EDG == current:
#   include only binaries for the current architecture and current compiler version
# if DIST_EDG == all:
#   include all EDG binaries for all platforms and compiler versions 
# if DIST_EDG == none:
#   no binaries distributed as EXTRA_DIST
EXTRA_DIST = $(if $(findstring all,$(DIST_EDG)), $(foreach ARCH,$(ALL_ARCHS),$(foreach COMPILER,$(ALL_COMPILERS),roseBinaryEDG-$(ARCH)-$(COMPILER)-$(release_binary_compatibility_signature).tar.gz)), $(if $(findstring current,$(DIST_EDG)), $(BINARY_EDG_DIRNAME).tar.gz))

DIST_SUBDIRS= 

if ROSE_HAS_EDG_SOURCE

SUBDIRS = EDG

EDG/libroseEDG.la:
	$(MAKE) -CEDG libroseEDG.la

#create tar-ball
$(BINARY_EDG_DIRNAME).tar.gz: EDG/libroseEDG.la
	rm -rf $(BINARY_EDG_DIRNAME)
	mkdir $(BINARY_EDG_DIRNAME)
#old script for creating binary tarball replaced dependency_libs with an empty list. We have not observed this to be non-empty so leave
#it as-is for now.
#	sed "s@^dependency_libs.*@dependency_libs=''@" src/frontend/CxxFrontend/EDG/libroseEDG.la > $${BINARY_EDG_DIRNAME}/libroseEDG.la  
	cp -r EDG/libroseEDG.* EDG/.libs $(BINARY_EDG_DIRNAME)/
	tar czf $(BINARY_EDG_DIRNAME).tar.gz $(BINARY_EDG_DIRNAME)
	rm -rf $(BINARY_EDG_DIRNAME)

#generate a tar-ball, but don't upload it
tarball: $(BINARY_EDG_DIRNAME).tar.gz

#upload tarball to a central location
upload: $(BINARY_EDG_DIRNAME).tar.gz
	cp $< $(BINARY_PATH)

else 
#------------------- no EDG sources

SUBDIRS= 


all: EDG/libroseEDG.la


#extract EDG tar-balls for the current architecture if that has not already been done
EDG/libroseEDG.la: $(BINARY_EDG_DIRNAME).tar.gz
	if test "$(am__untar)" != "false"; then \
	    echo "Found $<: proceeding to untar file $(BINARY_EDG_DIRNAME).tar.gz" >&2 && \
	    GZIP=$(GZIP_ENV) gunzip -c $< | $(am__untar) && \
	    rm -rf EDG && \
	    mkdir -p EDG && \
	    mv $(BINARY_EDG_DIRNAME)/libroseEDG.* EDG/ && \
	    mv $(BINARY_EDG_DIRNAME)/.libs EDG/ && \
	    rmdir $(BINARY_EDG_DIRNAME)/ && \
            touch EDG/libroseEDG.la; \
	else \
	  echo "ERROR: am__untar set to false! (am__untar == $(am__untar)) Can't untar $(BINARY_EDG_DIRNAME).tar.gz) -- this should have been caught by configure" >&2 && \
	  exit 1; \
	fi

CLEANFILES = EDG/libroseEDG.la
clean-local:
	-rm -rf EDG/.libs/
endif
