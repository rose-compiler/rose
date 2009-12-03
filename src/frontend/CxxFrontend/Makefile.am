include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

release_binary_compatibility_signature=$(shell ${top_srcdir}/scripts/bincompat-sig)
BINARY_EDG_DIRNAME=roseBinaryEDG-${build_triplet_without_redhat}-${GENERATED_COMPILER_NAME_AND_VERSION_SUFFIX}-${release_binary_compatibility_signature}
BINARY_URL=file:///usr/casc/overture/ROSE/git/ROSE_EDG_Binaries


if ROSE_HAS_EDG_SOURCE

SUBDIRS = EDG


$(BINARY_EDG_DIRNAME).tar.gz: EDG/libroseEDG.la
	mkdir $(BINARY_EDG_DIRNAME)
	cp -r EDG/libroseEDG.* EDG/.libs $(BINARY_EDG_DIRNAME)/
	tar czf $(BINARY_EDG_DIRNAME).tar.gz $(BINARY_EDG_DIRNAME)
	rm -rf $(BINARY_EDG_DIRNAME)

upload: $(BINARY_EDG_DIRNAME).tar.gz
	cp $< /usr/casc/overture/ROSE/git/ROSE_EDG_Binaries

else

DIST_SUBDIRS= EDG 
SUBDIRS= 


all: EDG/libroseEDG.la

# DQ (9/8/2009): Debugging gunzip broken pipe issue on NMI x86_sles_9 force echo of output from gunzip command.
# DQ (9/14/2009): When doing nmi-postmortem testing, the $(srcdir)/$(BINARY_EDG_DIRNAME).tar.gz is already unzipped

%.tar.gz:
	curl ${BINARY_URL}/$@  -o $@

EDG/libroseEDG.la: $(BINARY_EDG_DIRNAME).tar.gz
	if test "$(am__untar)" != "false"; then \
	    echo "Found $(builddir)/$(BINARY_EDG_DIRNAME).tar.gz: proceeding to untar file $(BINARY_EDG_DIRNAME).tar.gz" >&2 && \
	    GZIP=$(GZIP_ENV) gunzip -c $(BINARY_EDG_DIRNAME).tar.gz | $(am__untar) && \
	    mkdir -p EDG && \
	    mv $(BINARY_EDG_DIRNAME)/libroseEDG.* EDG/ && \
	    mv $(BINARY_EDG_DIRNAME)/.libs EDG/ && \
	    rmdir $(BINARY_EDG_DIRNAME)/; \
	else \
	  echo "ERROR: am__untar set to false! (am__untar == $(am__untar)) Can't untar $(BINARY_EDG_DIRNAME).tar.gz) -- this should have been caught by configure" >&2 && \
	  exit 1; \
	fi

CLEANFILES = EDG/libroseEDG.la
clean-local:
	-rm -rf EDG/.libs/
endif
