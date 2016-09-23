include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

if ROSE_ENABLE_PROJECT_CODE_PROPERTIES

# Forwards
EXTRA_DIST = 
MOSTLYCLEANFILES =
noinst_PROGRAMS =
bin_PROGRAMS =
CHECK_TARGETS =
CLEAN_TARGETS =
PHONIES =

#------------------------------------------------------------------------------------------------------------------------
# Header files, etc
EXTRA_DIST += CodeProperties.h

#------------------------------------------------------------------------------------------------------------------------
# Specimens, test inputs
SPECIMENS = test1.c
EXTRA_DIST += $(SPECIMENS)
TEST_WITH_ANSWER = $(top_srcdir)/scripts/test_with_answer

#------------------------------------------------------------------------------------------------------------------------
# findEnvironmentVars (using "fev_" as a unique test prefix)
bin_PROGRAMS += findEnvironmentVars
findEnvironmentVars_SOURCES = findEnvironmentVars.C
findEnvironmentVars_CPPFLAGS = $(ROSE_INCLUDES)
findEnvironmentVars_LDADD = $(LIBS_WITH_RPATH) $(ROSE_LIBS)

findEnvironmentVars_TESTS = $(addprefix fev_, $(addsuffix .passed, $(SPECIMENS)))
$(findEnvironmentVars_TESTS): fev_%.passed: $(srcdir)/% findEnvironmentVars $(TEST_WITH_ANSWER)
	@$(RTH_RUN) CMD="./findEnvironmentVars -c $<" ANS="$(dir $<)fev_$(notdir $<).ans" $(TEST_WITH_ANSWER) $@
EXTRA_DIST += $(findEnvironmentVars_TESTS:.passed=.ans)

CHECK_TARGETS += check-findEnvironmentVars
PHONIES += check-findEnvironmentVars
check-findEnvironmentVars: $(findEnvironmentVars_TESTS)

CLEAN_TARGETS += clean-findEnvironmentVars
PHONIES += clean-findEnvironmentVars
clean-findEnvironmentVars:
	rm -f $(findEnvironmentVars_TESTS) $(findEnvironmentVars_TESTS:.passed=.failed)

#------------------------------------------------------------------------------------------------------------------------
# findFileInput (usig "ffi_" as a unique test prefix)
bin_PROGRAMS += findFileInput
findFileInput_SOURCES = findFileInput.C
findFileInput_CPPFLAGS = $(ROSE_INCLUDES)
findFileInput_LDADD = $(LIBS_WITH_RPATH) $(ROSE_LIBS)

findFileInput_TESTS = $(addprefix ffi_, $(addsuffix .passed, $(SPECIMENS)))
$(findFileInput_TESTS): ffi_%.passed: $(srcdir)/% findFileInput $(TEST_WITH_ANSWER)
	@$(RTH_RUN) CMD="./findFileInput -c $<" ANS="$(dir $<)ffi_$(notdir $<).ans" $(TEST_WITH_ANSWER) $@
EXTRA_DIST += $(findFileInput_TESTS:.passed=.ans)

CHECK_TARGETS += check-findFileInput
PHONIES += check-findFileInput
check-findFileInput: $(findFileInput_TESTS)

CLEAN_TARGETS += clean-findFileInput
PHONIES += clean-findFileInput
clean-findFileInput:
	rm -f $(findFileInput_TESTS) $(findFileInput_TESTS:.passed=.failed)

#------------------------------------------------------------------------------------------------------------------------
# findSocketInput (using "fsi_" as a unique test prefix)
bin_PROGRAMS += findSocketInput
findSocketInput_SOURCES = findSocketInput.C
findSocketInput_CPPFLAGS = $(ROSE_INCLUDES)
findSocketInput_LDADD = $(LIBS_WITH_RPATH) $(ROSE_LIBS)

findSocketInput_TESTS = $(addprefix fsi_, $(addsuffix .passed, $(SPECIMENS)))
$(findSocketInput_TESTS): fsi_%.passed: $(srcdir)/% findSocketInput $(TEST_WITH_ANSWER)
	@$(RTH_RUN) CMD="./findSocketInput -c $<" ANS="$(dir $<)fsi_$(notdir $<).ans" $(TEST_WITH_ANSWER) $@
EXTRA_DIST += $(findSocketInput_TESTS:.passed=.ans)

CHECK_TARGETS += check-findSocketInput
PHONIES += check-findSocketInput
check-findSocketInput: $(findSocketInput_TESTS)

CLEAN_TARGETS += clean-findSocketInput
PHONIES += clean-findSocketInput
clean-findSocketInput:
	rm -f $(findSocketInput_TESTS) $(findSocketInput_TESTS:.passed=.failed)

#------------------------------------------------------------------------------------------------------------------------
# findSharedMemory (using "fsm_" as a unique test prefix)
bin_PROGRAMS += findSharedMemory
findSharedMemory_SOURCES = findSharedMemory.C
findSharedMemory_CPPFLAGS = $(ROSE_INCLUDES)
findSharedMemory_LDADD = $(LIBS_WITH_RPATH) $(ROSE_LIBS)

findSharedMemory_TESTS = $(addprefix fsm_, $(addsuffix .passed, $(SPECIMENS)))
$(findSharedMemory_TESTS): fsm_%.passed: $(srcdir)/% findSharedMemory $(TEST_WITH_ANSWER)
	@$(RTH_RUN) CMD="./findSharedMemory -c $<" ANS="$(dir $<)fsm_$(notdir $<).ans" $(TEST_WITH_ANSWER) $@
EXTRA_DIST += $(findSharedMemory_TESTS:.passed=.ans)

CHECK_TARGETS += check-findSharedMemory
PHONIES += check-findSharedMemory
check-findSharedMemory: $(findSharedMemory_TESTS)

CLEAN_TARGETS += clean-findSharedMemory
PHONIES += clean-findSharedMemory
clean-findSharedMemory:
	rm -f $(findSharedMemory_TESTS) $(findSharedMemory_TESTS:.passed=.failed)

#------------------------------------------------------------------------------------------------------------------------
# automake check and clean rules
check-local: $(CHECK_TARGETS)
clean-local: $(CLEAN_TARGETS)
	rm -f $(MOSTLYCLEANFILES)

endif

.PHONY: $(PHONIES)
