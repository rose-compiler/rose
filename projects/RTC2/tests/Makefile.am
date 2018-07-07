include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

##
## Specified tests below
##
## tests are suffixed with either ".fail" or ".good"
## indicating whether a test should run w/ or w/o error.
##
## notes: c_A_10_d.c is an RTED pointer test (c/pointer). Not sure what is
##                   wrong with it. Tagged as good.

PASSING_TESTS = \
	hello.rtc.bin.good \
	late_forward_decl.rtc.bin.good \
	pointer_example.rtc.bin.good \
	pointer_example_plain.rtc.bin.fail \
	pointer_example_simple_int.rtc.bin.fail \
	pointer_example_simple_int_ok.rtc.bin.good \
	pointer_example_struct2.rtc.bin.fail \
	pointer_example_struct2_ok.rtc.bin.good \
	pointer_example_struct2_trans.rtc.bin.fail \
	pointer_example_struct2_trans_ok.rtc.bin.good \
	pointer_example2.rtc.bin.fail \
	pointer_example2_ok.rtc.bin.good \
	pointer_example2_trans.rtc.bin.fail \
	pointer_example2b.rtc.bin.good \
	pointer_example2b_trans.rtc.bin.fail \
	pointer_example3.rtc.bin.good   \
	pointer_example3_trans2.rtc.bin.good \
	pointer_example7_simplified.rtc.bin.good \
	pointer_example4.rtc.bin.good  \
	pointer_example5.rtc.bin.good \
	pointer_example8_new_ok.rtc.bin.good \
	pointer_example8_trans.rtc.bin.good \
	pointer_example8_simplified.rtc.bin.fail \
	pointer_example8_simplified_ok.rtc.bin.good \
	pointer_example9.rtc.bin.good \
	pointer_example10_simplified.rtc.bin.fail \
	pointer_example10_simplified_ok.rtc.bin.good \
	pointer_example11_simplified.rtc.bin.fail \
	pointer_example11_simplified_ok.rtc.bin.good \
	pointer_example12_simplified.rtc.bin.good \
	pointer_example13_simplified.rtc.bin.fail \
	pointer_example13_simplified_ok.rtc.bin.good \
	pointer_example14_simplified.rtc.bin.good \
	pointer_example15_simplified.rtc.bin.fail \
	pointer_example15_simplified_ok.rtc.bin.good \
	pointer_example16_simplified.rtc.bin.good \
	pointer_example17_array_test.rtc.bin.fail \
	heap_array.rtc.bin.fail \
	stack_array.rtc.bin.fail \
	stack_array_in_struct.rtc.bin.fail \
	arraytest.rtc.bin.fail \
	c_A_10_d.rtc.bin.good

## BUGS where instrumentation fails

BUGGY_AGGREGATE_INIT = \
	aggregate_init_ptr.rtc.bin \
	pointer_example_struct.rtc.bin \
	pointer_example_struct1.rtc.bin

BUGGY_MALLOC_FOR_INIT = \
	malloc_for_init.rtc.bin \
	pointer_example_int.rtc.bin \
	pointer_example6.rtc.bin  \
	pointer_example7.rtc.bin \
	pointer_example7b.rtc.bin

BUGGY_OTHER = \
	pointer_example2_trans_ok.rtc.bin.good \
	heap_array_in_struct_in_array.rtc.bin \
	heap_array_in_struct.rtc.bin \
	stack_array_in_struct_in_array.rtc.bin \
	plus_equal.rtc.bin \
	union.rtc.bin

## BUGS where runtime yields false positives or false negatives

BUGGY_OUT_OF_BOUNDS_CHECK = \
	pointer_example2b_trans_ok.rtc.bin.good \
	pointer_example8_new.rtc.bin.fail

BUGGY_UNDETECTED = \
	assignment_to_fncall_multivar.rtc.bin.fail

OTHER = \
	void_pointer_cast.rtc.bin.good \
	c_A_8_b_in.rtc.bin.fail \
	assignment_fncall_null.rtc.bin.fail \
	assignment_scope_switch.rtc.bin.fail \
	assignment_to_fn_malloc_novar.rtc.bin.fail \
	assignment_scope_dowhile.rtc.bin.fail \
	assignment_scope_while.rtc.bin.fail \
	assignment_to_locals_then_globals_then_locals.rtc.bin.fail \
	assignment_scope_for.rtc.bin.fail \
	assignment_to_fncall.rtc.bin.fail \
	assignment_to_multi_fncall.rtc.bin.fail \
	assignment_scope_if.rtc.bin.fail


ACTIVE_TESTS = \
	$(PASSING_TESTS)

#~ RTC_TEST_OBJECTS = \
#~ 	$(patsubst %.m,output/rose_%.cc,$(ACTIVE_TESTS))

RTED_GOOD_BIN = $(RTED_TESTS:.bin.good=.bin)
RTED_FAIL_BIN = $(RTED_TESTS:.bin.fail=.bin)
RTC = ../src/rtc

.PHONY: metadatalib check clean purge
.PRECIOUS: $(RTED_GOOD_BIN) $(RTED_FAIL_BIN)

$(RTC):
	$(MAKE) -C ../src rtc

metadata_alt.o: $(srcdir)/../src/metadata/metadata_alt.C $(srcdir)/../src/metadata/metadata_alt.h $(srcdir)/../src/metadata/LockMgr.h $(srcdir)/../src/metadata/TrackingDB.h $(srcdir)/../src/metadata/rtc-defines.h
	$(CXX) $(CXXFLAGS) -I../src/metadata -c $<

metalib_alt.o: $(srcdir)/../src/metadata/metalib_alt.C $(srcdir)/../src/metadata/metalib_alt.h $(srcdir)/../src/metadata/LockMgr.h $(srcdir)/../src/metadata/TrackingDB.h $(srcdir)/../src/metadata/rtc-defines.h
	$(CXX) $(CXXFLAGS) -I$(srcdir)/../src/metadata -c $<

# instrument codes
# - depending on the location of the test files,
#   one of the following rules should trigger for every test

%.rtc.c: $(srcdir)/c/%.c $(RTC)
	$(RTC) $(ROSE_INCLUDES) -I$(srcdir)/include -plain -rose:o $@ $<

%.rtc.c: $(srcdir)/rted/c/memoryleaks/%.c $(RTC)
	$(RTC) $(ROSE_INCLUDES) -I$(srcdir)/include -plain -rose:o $@ $<

%.rtc.c: $(srcdir)/rted/c/memoverlap/%.c $(RTC)
	$(RTC) $(ROSE_INCLUDES) -I$(srcdir)/include -plain -rose:o $@ $<

%.rtc.c: $(srcdir)/rted/c/pointer/%.c $(RTC)
	$(RTC) $(ROSE_INCLUDES) -I$(srcdir)/include -plain -rose:o $@ $<

%.rtc.c: $(srcdir)/rted/c/types/%.c $(RTC)
	$(RTC) $(ROSE_INCLUDES) -I$(srcdir)/include -plain -rose:o $@ $<


# compile instrumented codes
%.rtc.o: %.rtc.c
	$(CXX) -ggdb -Wall -Wextra -fpermissive -I$(srcdir)/include -I$(srcdir)/../src -c -o $@ $<

# link instrumented codes
%.rtc.bin: %.rtc.o metadata_alt.o metalib_alt.o
	$(CXX) $< metadata_alt.o metalib_alt.o -o $@

%.rtc.bin.good: %.rtc.bin
	chk.sh 0 $< >$@

%.rtc.bin.fail: %.rtc.bin
	chk.sh 134 $< 2>$@


# general things

conditional-check-local: $(ACTIVE_TESTS)
	@echo "MatlabTranslation tests."
	@$(MAKE) $(MATLAB_TEST_OBJECTS)
	@echo "======== make check rule for MatlabTranslation terminated normally ========"

check-local: conditional-check-local

clean-local:
	rm -rf *.rtc.c *.rtc.o *.rtc.bin *.fail *.good

EXTRA_DIST = $(ALL_TESTCODES)
