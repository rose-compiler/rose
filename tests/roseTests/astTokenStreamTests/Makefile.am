include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs
EXTRA_DIST =  tests
bin_PROGRAMS =
MOSTLYCLEANFILES =

# if ROSE_USE_SQLITE_DATABASE

INCLUDES = $(ROSE_INCLUDES) 

EDG_VERSION = @ROSE_EDG_MAJOR_VERSION_NUMBER@

#------------------------------------------------------------------------------------------------------------------------
# Scope matching
bin_PROGRAMS += testScopeMatching
testScopeMatching_SOURCES = testScopeMatching.C  
testScopeMatching_LDADD = $(ROSE_LIBS)

# NOTE: testScopeMatching was apparently never tested.



#------------------------------------------------------------------------------------------------------------------------
# Linearization
bin_PROGRAMS += testLinearization
testLinearization_SOURCES = testLinearization.C  
testLinearization_LDADD = $(ROSE_LIBS)

LIN_SPECIMENS = testLin1.c testLin2.c
LIN_TEST_TARGETS = $(addprefix lin_, $(addsuffix .passed, $(LIN_SPECIMENS)))
LIN_TEST_CONFIG = $(srcdir)/testLinearization.conf
$(LIN_TEST_TARGETS): lin_%.passed: ${srcdir}/tests/% testLinearization $(LIN_TEST_CONFIG)
	@echo "Testing for EDG $(EDG_VERSION)"
	@$(RTH_RUN) SPECIMEN=$< EDG=$(EDG_VERSION) $(LIN_TEST_CONFIG) $@

EXTRA_DIST += $(LIN_TEST_CONFIG)

MOSTLYCLEANFILES += 										 \
	$(LIN_TEST_TARGETS) $(LIN_TEST_TARGETS:.passed=.failed) $(LIN_TEST_TARGETS:.passed=.out) \
	$(LIN_TEST_TARGETS:.passed=.err)



#------------------------------------------------------------------------------------------------------------------------
# Additional automake targets
check-local: $(LIN_TEST_TARGETS)

# endif
