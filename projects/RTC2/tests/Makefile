# Example Makefile for ROSE users
# This makefile is provided as an example of how to use ROSE when ROSE is
# installed (using "make install").  This makefile is tested as part of the
# "make distcheck" rule (run as part of tests before any CVS checkin).
# The test of this makefile can also be run by using the "make installcheck"
# rule (run as part of "make distcheck").


# Location of include directory after "make install"

ifeq ($(ROSE_ROOT),)
  echo "setting default ROSE_ROOT to ./rose"
  ROSE_ROOT=./rose
endif

ifeq ($(ROSE_HOME),)
  echo "setting default ROSE_HOME to $(ROSE_ROOT)/installtree"
  ROSE_HOME=$(ROSE_ROOT)/installtree)
endif

ifeq ($(ROSE_BUILD),)
  echo "setting default ROSE_HOME to $(ROSE_ROOT)/buildtree"
  ROSE_BUILD=$(ROSE_ROOT)/buildtree)
endif

ifeq ($(ROSE_SRC),)
  echo "setting default ROSE_SRC to $(ROSE_ROOT)"
  ROSE_SRC=$(ROSE_ROOT)
endif

ROSE_INCLUDE_DIR = $(ROSE_HOME)/include

CXXFLAGS         = -ggdb -O0 -Wall -Wextra

ROSE_INCLUDES    = \
     -I$(ROSE_INCLUDE_DIR) \
     -I$(ROSE_INCLUDE_DIR)/rose

##
## Specified tests below
##
## tests are suffixed with either good (should run w/o error)
## or fail (should terminate with error code).
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

## BUGS where runtime yields false po

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


RTC=../src/RTC
RTED_TESTS = $(PASSING_TESTS)
RTED_GOOD_BIN = $(RTED_TESTS:.bin.good=.bin)
RTED_FAIL_BIN = $(RTED_TESTS:.bin.fail=.bin)

.PHONY: metadatalib check clean purge
.PRECIOUS: $(RTED_GOOD_BIN) $(RTED_FAIL_BIN)


check: $(RTED_TESTS)

#
# tests

# RTC
$(RTC): ../src/instrumenter/RTC.C
	(cd ../src; make)

# metadata lib
metadatalib: metadata_alt.o metalib_alt.o

metadata_alt.o: ../src/metadata/metadata_alt.C ../src/metadata/metadata_alt.h ../src/metadata/LockMgr.h ../src/metadata/TrackingDB.h ../src/metadata/rtc-defines.h
	$(CXX) $(CXXFLAGS) -I../src/metadata -c $<

metalib_alt.o: ../src/metadata/metalib_alt.C ../src/metadata/metalib_alt.h ../src/metadata/LockMgr.h ../src/metadata/TrackingDB.h ../src/metadata/rtc-defines.h
	$(CXX) $(CXXFLAGS) -I../src/metadata -c $<

# instrument codes
# - depending on the location of the test files,
#   one of the following rules should trigger for every test

%.rtc.c: c/%.c $(RTC)
	$(RTC) $(ROSE_INCLUDES) -I./include -plain -rose:o $@ $<

%.rtc.c: rted/c/memoryleaks/%.c $(RTC)
	$(RTC) $(ROSE_INCLUDES) -I./include -plain -rose:o $@ $<

%.rtc.c: rted/c/memoverlap/%.c $(RTC)
	$(RTC) $(ROSE_INCLUDES) -I./include -plain -rose:o $@ $<

%.rtc.c: rted/c/pointer/%.c $(RTC)
	$(RTC) $(ROSE_INCLUDES) -I./include -plain -rose:o $@ $<

%.rtc.c: rted/c/types/%.c $(RTC)
	$(RTC) $(ROSE_INCLUDES) -I./include -plain -rose:o $@ $<


# compile instrumented codes
%.rtc.o: %.rtc.c
	$(CXX) -ggdb -Wall -Wextra -fpermissive -I./include -I../src -c -o $@ $<

# link instrumented codes
%.rtc.bin: %.rtc.o metadata_alt.o metalib_alt.o
	$(CXX) $< metadata_alt.o metalib_alt.o -o $@

%.rtc.bin.good: %.rtc.bin
	chk.sh 0 $<

%.rtc.bin.fail: %.rtc.bin
	chk.sh 134 $<


#
# clean

clean:
	rm -rf *.rtc.c *.rtc.o *.rtc.bin
