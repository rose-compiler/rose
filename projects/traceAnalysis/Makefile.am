include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

INCLUDES = $(ROSE_INCLUDES)

bin_PROGRAMS = read_etrace

read_etrace_SOURCES = dummy_file.c

if ROSE_USE_TRACE_ANALYSIS
#  read_etrace_SOURCES += read_etrace.c
   TRACE_FILES = vmp-hw-part.trace
else
   TRACE_FILES = 
endif

LDADD = $(LIBS_WITH_RPATH) $(ROSE_LIBS)

test: read_etrace
	./read_etrace $(srcdir)/vmp-hw-part.trace

check-local:

clean-local:
	rm -f *.o rose_*.c

EXTRA_DIST = $(TRACE_FILES)
