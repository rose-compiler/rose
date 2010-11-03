# Makefile for an x86 simulator called "sandbox". At this time the simulator is highly Linux-specific.  On platforms other
# than Linux, this simulator is simply a main() that prints an error message and exits.

include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

EXTRA_DIST = README
MOSTLYCLEANFILES=
include_HEADERS=
noinst_PROGRAMS=

###############################################################################################################################
#  PROGRAMS
###############################################################################################################################

# The old simulator, which uses ROSE's disassembler, but not ROSE's ability to parse ELF containers. This is built but
# not tested.  It is being replaced by the new simulator, x86sim.
noinst_PROGRAMS += sandbox
include_HEADERS += mymemory.h simulate.h
sandbox_SOURCES = emulator.C simulate.C linuxcalls.C elfload.C
sandbox_CPPFLAGS = $(ROSE_INCLUDES) -UDEBUG
sandbox_LDADD = $(LIBS_WITH_RPATH) $(ROSE_LIBS)

# The new simulator, which uses ROSE interfaces where possible.  The rest of this makefile deals only with this new simulator.
noinst_PROGRAMS += x86sim
include_HEADERS += x86print.h
x86sim_SOURCES = x86sim.C x86print.C
x86sim_CPPFLAGS = $(ROSE_INCLUDES) -DX86_VDSO_PATH_1='"$(srcdir)"' -DX86_VDSO_PATH_2='"$(include_dir)"'
x86sim_LDADD = $(LIBS_WITH_RPATH) $(ROSE_LIBS)

###############################################################################################################################
#  TEST INPUTS
###############################################################################################################################

# Source and executables for test inputs. The inputs are 32-bit executables. They come in three flavors depending
# on how they're compiled and linked:
#    TEST_INPUTS_SA  -- stand-alone, no additional source required
#    TEST_INPUTS_RT  -- require linking with librt
#    TEST_INPUTS_TS  -- use the TEST_SUPPORT library
TEST_SUPPORT_SRC = $(srcdir)/tests/parse_opts.c \
                   $(srcdir)/tests/tst_res.c \
                   $(srcdir)/tests/tst_sig.c \
                   $(srcdir)/tests/tst_tmpdir.c \
                   $(srcdir)/tests/rmobj.c \
                   $(srcdir)/tests/simple_test_looping.c \
                   $(srcdir)/tests/audit_parse_verify.c \
                   $(srcdir)/tests/audit_parse_verify_utils.c \
                   $(srcdir)/tests/audit_utils.c

TEST_INPUTS_SA_SRC = $(wildcard $(srcdir)/tests/syscall_sa*.c)
TEST_INPUTS_RT_SRC = $(wildcard $(srcdir)/tests/syscall_rt*.c)
TEST_INPUTS_TS_SRC = $(wildcard $(srcdir)/tests/syscall_ts*.c)
TEST_INPUTS_SRC    = $(TEST_INPUTS_SA_SRC) $(TEST_INPUTS_RT_SRC) $(TEST_INPUTS_TS_SRC)

TEST_INPUTS_SA = $(basename $(notdir $(TEST_INPUTS_SA_SRC)))
TEST_INPUTS_RT = $(basename $(notdir $(TEST_INPUTS_RT_SRC)))
TEST_INPUTS_TS = $(basename $(notdir $(TEST_INPUTS_TS_SRC)))
TEST_INPUTS    = $(TEST_INPUTS_SA) $(TEST_INPUTS_RT) $(TEST_INPUTS_TS)

MOSTLYCLEANFILES += $(TEST_INPUTS) $(addsuffix .o, $(TEST_INPUTS)) \
                    $(addsuffix .o, $(basename $(notdir $(TEST_SUPPORT_SRC))))

$(TEST_INPUTS_SA): %: $(srcdir)/tests/%.c
	@echo "  CC32    $@"
	@$(CC) -m32 -I$(srcdir)/tests -o $@ $^
$(TEST_INPUTS_RT): %: $(srcdir)/tests/%.c
	@echo "  CC32    $@"
	@$(CC) -m32 -I$(srcdir)/tests -o $@ $^ -lrt
$(TEST_INPUTS_TS): %: $(srcdir)/tests/%.c $(TEST_SUPPORT_SRC)
	@echo "  CC32    $@"
	@$(CC) -m32 -I$(srcdir)/tests -o $@ $^ -lrt

# Target to manually build the test inputs. (Not used internally)
.PHONY: test_inputs
test_inputs: $(TEST_INPUTS)


###############################################################################################################################
#  TESTS
###############################################################################################################################

# Test targets are test names suffixed with ".passed" (but when a test fails it produces ".failed" instead).
# Each target runs "x86sim" on one of the TEST_INPUT files. Exit status determines success or failure.
TEST_TARGETS = $(addsuffix .passed, $(TEST_INPUTS))
MOSTLYCLEANFILES += $(TEST_TARGETS) $(patsubst %.passed, %.failed, $(TEST_TARGETS))
$(TEST_TARGETS): %.passed: % $(srcdir)/x86sim.conf
	@$(TH) INPUT=./$< $(srcdir)/x86sim.conf $@

check-local: $(TEST_TARGETS)
