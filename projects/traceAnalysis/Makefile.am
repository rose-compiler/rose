include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

INCLUDES = $(ROSE_INCLUDES)

if ROSE_USE_TRACE_ANALYSIS
bin_PROGRAMS = read_etrace

read_etrace_SOURCES = read_etrace.c

LDADD = $(LIBS_WITH_RPATH) $(ROSE_LIBS)

test: read_etrace
	./read_etrace $(srcdir)/vmp-hw-part.trace

check-local:

clean-local:
	rm -f *.o rose_*.c

TRACE_FILES = vmp-hw-part.trace
else
TRACE_FILES = 
endif

EXTRA_DIST = $(TRACE_FILES)
