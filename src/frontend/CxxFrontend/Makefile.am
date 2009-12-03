include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

if ROSE_HAS_EDG_SOURCE

SUBDIRS = EDG

else
BINARY_EDG_DIRNAME=@EXPECTED_BINARY_EDG_DIRNAME@

BINARY_URL=file:///usr/casc/overture/ROSE/git/ROSE_EDG_Binaries

all: EDG/libroseEDG.la

# DQ (9/8/2009): Debugging gunzip broken pipe issue on NMI x86_sles_9 force echo of output from gunzip command.
# DQ (9/14/2009): When doing nmi-postmortem testing, the $(srcdir)/$(BINARY_EDG_DIRNAME).tar.gz is already unzipped

%.tar.gz:
	curl ${BINARY_URL}/$@ > $@

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
