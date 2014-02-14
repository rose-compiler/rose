include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs
noinst_PROGRAMS =
TEST_TARGETS =
EXTRA_DIST =

TEST_EXIT_STATUS = $(top_srcdir)/scripts/test_exit_status

#------------------------------------------------------------------------------------------------------------------------
# inlineEverything

noinst_PROGRAMS += inlineEverything
inlineEverything_SOURCES = inlineEverything.C
inlineEverything_CPPFLAGS = $(ROSE_INCLUDES)
inlineEverything_LDADD = $(LIBS_WITH_RPATH) $(ROSE_SEPARATE_LIBS)

inlineEverything_specimens = 									\
   pass16.C pass1.C pass2.C pass3.C pass4.C pass5.C pass6.C pass7.C pass8.C pass9.C pass10.C	\
   pass11.C pass12.C pass13.C pass14.C pass15.C pass17.C pass18.C pass19.C pass20.C pass21.C
EXTRA_DIST += $(inlineEverything_specimens)

inlineEverything_test_targets = $(addprefix inlineEverything_, $(addsuffix .passed, $(inlineEverything_specimens)))
TEST_TARGETS += $(inlineEverything_test_targets)
$(inlineEverything_test_targets): inlineEverything_%.passed: % inlineEverything
	@$(RTH_RUN) \
		USE_SUBDIR=yes \
		CMD="$$(pwd)/inlineEverything -rose:verbose 0 $(abspath $<) -o a.out && ./a.out" \
		$(TEST_EXIT_STATUS) $@

#------------------------------------------------------------------------------------------------------------------------
# additional tests not executed by "make check"

EXTRA_DIST += test1.conf test1A.C test1B.C
test1.passed: test1A.C test1B.C
	@$(RTH_RUN) $(srcdir)/test1.conf $@

#------------------------------------------------------------------------------------------------------------------------
# automake boilerplate

check-local: $(TEST_TARGETS)

clean-local:
	rm -f $(TEST_TARGETS)
	rm -f $(TEST_TARGETS:.passed=.failed)
