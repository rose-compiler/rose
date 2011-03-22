include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

INCLUDES = $(ROSE_INCLUDES)

bin_PROGRAMS = trace

# Comment out to test withouth committting to ROSE yet.
trace_SOURCES = trace.C generateExecutable.C

TRACE_FILES = vmp-hw-part.trace

# DQ (11/2/2010): this automake variable is always fals at the moment.
if ROSE_USE_TRACE_ANALYSIS
#  TRACE_FILES = 
else
#  TRACE_FILES = 
endif

LDADD = $(LIBS_WITH_RPATH) $(ROSE_LIBS)

test: trace
	./trace $(srcdir)/vmp-hw-part.trace

check-local:
	$(MAKE) test > trace.out

clean-local:
	rm -f *.o rose_*.c trace.out a.out

EXTRA_DIST = $(TRACE_FILES)
