include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs
EXTRA_DIST =
bin_PROGRAMS =
MOSTLYCLEANFILES =
TEST_TARGETS =

if ROSE_ENABLE_PROJECT_TRACE_ANALYSIS

TEST_EXIT_STATUS = $(top_srcdir)/scripts/test_exit_status

#-------------------------------------------------------------------------------------------------------------------------------
# The main executable for this project

bin_PROGRAMS += trace
trace_SOURCES = trace.C generateExecutable.C
trace_CPPFLAGS = $(ROSE_INCLUDES)
trace_LDADD = $(LIBS_WITH_RPATH) $(ROSE_LIBS)

#-------------------------------------------------------------------------------------------------------------------------------
# Tests

trace_SPECIMEN = $(srcdir)/vmp-hw-part.trace
EXTRA_DIST += $(trace_SPECIMEN)

TEST_TARGETS += trace.passed
trace.passed: $(TEST_EXIT_STATUS) trace $(trace_SPECIMEN)
	@$(RTH_RUN) CMD="./trace $(trace_SPECIMEN)" $< $@

MOSTLYCLEANFILES += a.exe a.out trace.out
MOSTLYCLEANFILES += trace.passed trace.failed

endif

#-------------------------------------------------------------------------------------------------------------------------------
# The usual automake boilerplate

check-local: $(TEST_TARGETS)

clean-local:
	rm -rf -- $(MOSTLYCLEANFILES)

