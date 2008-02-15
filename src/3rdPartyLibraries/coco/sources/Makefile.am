include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

INCLUDES = $(ROSE_INCLUDES)

bin_PROGRAMS = cocor

cocor_SOURCES = cr.c cra.c crp.c crs.c crt.c crx.c crf.c collect.c set.c

# Header files that need to be included in the distribution
include_HEADERS = collect.h  cra.h  crc.h  cre.h  crf.h  crp.h  crs.h  crt.h  crx.h  set.h

clean-local:
	rm -rf *.o cocor
