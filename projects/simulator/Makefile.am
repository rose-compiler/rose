# Makefile for an x86 simulator called "x86sim". At this time the simulator is highly Linux-specific.  On platforms other
# than Linux, this simulator is simply a main() that prints an error message and exits.

include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

# Forward declarations for "+="
MOSTLYCLEANFILES=
EXTRA_DIST=
include_HEADERS=
noinst_PROGRAMS=
lib_LIBRARIES=
TEST_TARGETS = 

warn_no_tests:
	@echo "  NOTE    simulator testing is disabled (no i386 support)"

###############################################################################################################################
#  DOCUMENTATION, CONFIGURATION, ETC
###############################################################################################################################

EXTRA_DIST += README test_status.txt internal_tests.conf syscall_tests.conf pthread_tests.conf RSIM.dxy \
	USED_HEADER_FILES.C demos/MultiDomainDemo.h demos/RiscTracingPolicy.h

###############################################################################################################################
#  SIMULATOR LIBRARY
###############################################################################################################################

noinst_LTLIBRARIES = libRSIM.la
libRSIM_la_CPPFLAGS = $(ROSE_INCLUDES)
libRSIM_la_SOURCES =				\
	x86print.C				\
	RSIM_Adapter.C				\
	RSIM_Callbacks.C			\
	RSIM_Common.C				\
	RSIM_Futex.C				\
	RSIM_Linux32.C				\
	RSIM_Process.C				\
	RSIM_SignalHandling.C			\
	RSIM_Simulator.C			\
	RSIM_Thread.C				\
	RSIM_Tools.C
include_HEADERS +=				\
	x86print.h				\
	RSIM_Adapter.h				\
	RSIM_Callbacks.h			\
	RSIM_Common.h				\
	RSIM_Futex.h				\
	RSIM_Linux32.h				\
	RSIM_Process.h				\
	RSIM_SemanticsSettings.h		\
	RSIM_SemanticsInner.h			\
	RSIM_SemanticsOuter.h			\
	RSIM_SignalHandling.h			\
	RSIM_Simulator.h			\
	RSIM_Templates.h			\
	RSIM_Thread.h				\
	RSIM_Tools.h

$(libRSIM_la_SOURCES:.C=.o): %.o: %.h
$(libRSIM_la_SOURCES:.C=.lo): %.lo: %.h

###############################################################################################################################
# CLIENT LIBRARY
###############################################################################################################################

EXTRA_DIST += RSIM_Client.c RSIM_Client.h
MOSTLYCLEANFILES += librsim_client.a
librsim_client.a: RSIM_Client.o
	@echo "  LINK32  $@"
	$(AR) rcs $@ $^

MOSTLYCLEANFILES += RSIM_Client.o
RSIM_Client.o: RSIM_Client.c RSIM_Client.h
	@echo "  CC32   $@"
	: ; $(CC) $(CFLAGS32) -Wall -o $@ -c $<

###############################################################################################################################
#  PROGRAMS
###############################################################################################################################

# The new simulator, which uses ROSE interfaces where possible.  The rest of this makefile deals only with this new simulator.
# Do not install RSIM_Private.h -- it is only used during compilation.
EXTRA_DIST += RSIM_Private.h
noinst_PROGRAMS += x86sim
x86sim_SOURCES = x86sim.C
x86sim_CPPFLAGS = $(ROSE_INCLUDES) -DX86_VDSO_PATH_1='"$(srcdir)"' -DX86_VDSO_PATH_2='"$(include_dir)"'
x86sim_LDADD = $(BOOST_LDFLAGS) libRSIM.la $(LIBS_WITH_RPATH) $(ROSE_LIBS) 

# Simulator for windows programs
noinst_PROGRAMS += x86win
x86win_SOURCES = x86win.C
x86win_CPPFLAGS = $(ROSE_INCLUDES)
x86win_LDADD = $(BOOST_LDFLAGS) libRSIM.la $(LIBS_WITH_RPATH) $(ROSE_LIBS)

# Robb's debugging area so he doesn't need to monkey with x86sim all the time
noinst_PROGRAMS += x86robb
x86robb_SOURCES = x86robb.C
x86robb_CPPFLAGS = $(ROSE_INCLUDES)
x86robb_LDADD = $(BOOST_LDFLAGS) libRSIM.la $(LIBS_WITH_RPATH) $(ROSE_LIBS)

# Dump contents of an RSIM_FutexTable
noinst_PROGRAMS += x86futex
x86futex_SOURCES = x86futex.C
x86futex_CPPFLAGS = $(ROSE_INCLUDES)
x86futex_LDADD = $(BOOST_LDFLAGS) libRSIM.la $(LIBS_WITH_RPATH) $(ROSE_LIBS)

# Disassemble a process
noinst_PROGRAMS += x86dis
x86dis_SOURCES = x86dis.C
x86dis_CPPFLAGS = $(ROSE_INCLUDES)
x86dis_LDADD = $(BOOST_LDFLAGS) libRSIM.la $(LIBS_WITH_RPATH) $(ROSE_LIBS)


###############################################################################################################################
#  DEMOS
###############################################################################################################################

EXTRA_DIST += demos/README demos/SignAnalysisExample.h

#------------------------------------------------------------------------------------------------------------------------------
# demo 1: branching to an uncalled function
noinst_PROGRAMS += demo1
demo1_SOURCES = demos/demo1.C
demo1_CPPFLAGS = $(ROSE_INCLUDES)
demo1_LDADD = $(BOOST_LDFLAGS) libRSIM.la $(LIBS_WITH_RPATH) $(ROSE_LIBS)

#------------------------------------------------------------------------------------------------------------------------------
# demo 2: synchronous sign analysis
noinst_PROGRAMS += demo2
demo2_SOURCES = demos/demo2.C
demo2_CPPFLAGS = $(ROSE_INCLUDES)
demo2_LDADD = $(BOOST_LDFLAGS) libRSIM.la $(LIBS_WITH_RPATH) $(ROSE_LIBS)

#------------------------------------------------------------------------------------------------------------------------------
# demo 3: symbolic semantics for an uncalled function using the Yices SMT solver.
#         don't try to run this demo unless you've configured ROSE using "--with-yices"
noinst_PROGRAMS += demo3
demo3_SOURCES = demos/demo3.C
demo3_CPPFLAGS = $(ROSE_INCLUDES)
demo3_LDADD = $(BOOST_LDFLAGS) libRSIM.la $(LIBS_WITH_RPATH) $(ROSE_LIBS)

EXTRA_DIST += demos/demo3input.c
MOSTLYCLEANFILES += demo3input
demo3input: demos/demo3input.c
	@echo "  CC32    $@"
	@$(CC) $(CFLAGS32) -Wall -o $@ $^

.PHONY: run_demo3
run_demo3: demo3 demo3input
	./demo3 ./demo3input

#------------------------------------------------------------------------------------------------------------------------------
# demo 4: combined symbolic semantics and sign analysis
noinst_PROGRAMS += demo4
demo4_SOURCES = demos/demo4.C
demo4_CPPFLAGS = $(ROSE_INCLUDES)
demo4_LDADD = $(BOOST_LDFLAGS) libRSIM.la $(LIBS_WITH_RPATH) $(ROSE_LIBS)

MOSTLYCLEANFILES += demo4input
demo4input: demo3input
	cp $^ $@

.PHONY: run_demo4
run_demo4: demo4 demo4input
	./demo4 ./demo4input

#------------------------------------------------------------------------------------------------------------------------------
# demo 5: watch for executing instructions on the stack
noinst_PROGRAMS += demo5
demo5_SOURCES = demos/demo5.C
demo5_CPPFLAGS = $(ROSE_INCLUDES)
demo5_LDADD = $(BOOST_LDFLAGS) libRSIM.la $(LIBS_WITH_RPATH) $(ROSE_LIBS)

EXTRA_DIST += demos/demo5input.c
MOSTLYCLEANFILES += demo5input
demo5input: demos/demo5input.c
	@echo "  CC32    $@"
	@$(CC) $(CFLAGS32) -Wall -o $@ $^

.PHONY: run_demo5
run_demo5: demo5 demo5input
	./demo5 ./demo5input

#------------------------------------------------------------------------------------------------------------------------------
# demo 6: instruction tracing with function call stacks
noinst_PROGRAMS += demo6
demo6_SOURCES = demos/demo6.C
demo6_CPPFLAGS = $(ROSE_INCLUDES)
demo6_LDADD = $(BOOST_LDFLAGS) libRSIM.la $(LIBS_WITH_RPATH) $(ROSE_LIBS)

.PHONY: run_demo6
run_demo6: demo6
	./demo6 --debug=insn $(top_srcdir)/binaries/samples/buffer2.bin

#------------------------------------------------------------------------------------------------------------------------------
# demo 7: trace instructions, printing only the name of the instruction's function.
noinst_PROGRAMS += demo7
demo7_SOURCES = demos/demo7.C
demo7_CPPFLAGS = $(ROSE_INCLUDES)
demo7_LDADD = $(BOOST_LDFLAGS) libRSIM.la $(LIBS_WITH_RPATH) $(ROSE_LIBS)

.PHONY: run_demo7
run_demo7: demo7
	./demo7 --debug $(top_srcdir)/binaries/samples/buffer2.bin

#------------------------------------------------------------------------------------------------------------------------------
# demo 7b: generate a disassembly listing after dynamic linking
noinst_PROGRAMS += demo7b
demo7b_SOURCES = demos/demo7b.C
demo7b_CPPFLAGS = $(ROSE_INCLUDES)
demo7b_LDADD = $(BOOST_LDFLAGS) libRSIM.la $(LIBS_WITH_RPATH) $(ROSE_LIBS)

.PHONY: run_demo7b
run_demo7b: demo7b
	./demo7b --debug $(top_srcdir)/binaries/samples/buffer2.bin

#------------------------------------------------------------------------------------------------------------------------------
# demo 8: report statistics about number of instructions and system calls
noinst_PROGRAMS += demo8
demo8_SOURCES = demos/demo8.C
demo8_CPPFLAGS = $(ROSE_INCLUDES)
demo8_LDADD = $(BOOST_LDFLAGS) libRSIM.la $(LIBS_WITH_RPATH) $(ROSE_LIBS)

#------------------------------------------------------------------------------------------------------------------------------
# Example of running a multi-domain semantic analysis to find places where symbolic semantics becomes too complex.
noinst_PROGRAMS += SymbolicBomb
SymbolicBomb_SOURCES = demos/SymbolicBomb.C
SymbolicBomb_CPPFLAGS = $(ROSE_INCLUDES)
SymbolicBomb_LDADD = $(BOOST_LDFLAGS) libRSIM.la $(LIBS_WITH_RPATH) $(ROSE_LIBS)

#------------------------------------------------------------------------------------------------------------------------------
# Example of running an interval analysis in lock step with the simulator
noinst_PROGRAMS += Intervals
Intervals_SOURCES = demos/Intervals.C
Intervals_CPPFLAGS = $(ROSE_INCLUDES)
Intervals_LDADD = $(BOOST_LDFLAGS) libRSIM.la $(LIBS_WITH_RPATH) $(ROSE_LIBS)

#------------------------------------------------------------------------------------------------------------------------------
# Example of using an SMT solver in lock step with the simulator
noinst_PROGRAMS += smt_example
smt_example_SOURCES = demos/smt_example.C
smt_example_CPPFLAGS = $(ROSE_INCLUDES)
smt_example_LDADD = $(BOOST_LDFLAGS) libRSIM.la $(LIBS_WITH_RPATH) $(ROSE_LIBS)

#------------------------------------------------------------------------------------------------------------------------------
# Multi-domain demo that enables/disables sub-domains and converts values from one sub-domain to another.  This is meant to
# serve as an example for Andreas and Earl that they can modify to suit their needs.
noinst_PROGRAMS += MultiWithConversion
EXTRA_DIST += demos/MultiWithConversion.h demos/MultiWithConversionTpl.h demos/RobinTpl.h
MultiWithConversion_SOURCES = demos/MultiWithConversion.C
MultiWithConversion_CPPFLAGS = $(ROSE_INCLUDES)
MultiWithConversion_LDADD = $(BOOST_LDFLAGS) libRSIM.la $(LIBS_WITH_RPATH) $(ROSE_LIBS)

###############################################################################################################################
#  SUPPORT PROGRAMS FOR PAPERS
###############################################################################################################################

#------------------------------------------------------------------------------------------------------------------------------
# Example of concrete analysis on a gzip code sample.
noinst_PROGRAMS += concrete_1
concrete_1_SOURCES = papers/concrete_1.C
concrete_1_CPPFLAGS = $(ROSE_INCLUDES)
concrete_1_LDADD = $(BOOST_LDFLAGS) libRSIM.la $(LIBS_WITH_RPATH) $(ROSE_LIBS)

EXTRA_DIST += papers/gzip_sample_1.c
concrete_1_input: papers/gzip_sample_1.c
	@echo "  CC32    $@"
	@$(CC) $(CFLAGS32) -DINIT_CRC_32_TAB -Wall -o $@ $^

.PHONY: run_concrete_1
run_concrete_1: concrete_1 concrete_1_input
	./concrete_1 --debug ./concrete_1_input
	./concrete_1_input

#------------------------------------------------------------------------------------------------------------------------------
# Example of concrete analysis on a gzip code sample with memory initialization in the simulator
noinst_PROGRAMS += concrete_2
concrete_2_SOURCES = papers/concrete_2.C
concrete_2_CPPFLAGS = $(ROSE_INCLUDES)
concrete_2_LDADD = $(BOOST_LDFLAGS) libRSIM.la $(LIBS_WITH_RPATH) $(ROSE_LIBS)

EXTRA_DIST += papers/gzip_sample_1.c
concrete_2_input: papers/gzip_sample_1.c
	@echo "  CC32    $@"
	@$(CC) $(CFLAGS32) -UINIT_CRC_32_TAB -Wall -o $@ $^

.PHONY: run_concrete_2
run_concrete_2: concrete_2 concrete_2_input
	./concrete_2 --debug ./concrete_2_input
	./concrete_2_input

#------------------------------------------------------------------------------------------------------------------------------
# Example of symbolic analysis on a gzip code sample.
noinst_PROGRAMS += symbolic_1
symbolic_1_SOURCES = papers/symbolic_1.C
symbolic_1_CPPFLAGS = $(ROSE_INCLUDES)
symbolic_1_LDADD = $(BOOST_LDFLAGS) libRSIM.la $(LIBS_WITH_RPATH) $(ROSE_LIBS)

EXTRA_DIST += papers/gzip_sample_2.c
symbolic_1_input: papers/gzip_sample_2.c
	@echo "  CC32    $@"
	@$(CC) $(CFLAGS32) -Wall -o $@ $^

.PHONY: run_symbolic_1
run_symbolic_1: symbolic_1 symbolic_1_input
	./symbolic_1 --debug ./symbolic_1_input
	./symbolic_1_input

#------------------------------------------------------------------------------------------------------------------------------
# Example of symbolic analysis on:
#  grep code sample distilled
#  glibc's strcmp() function distilled
#  glibc's strcmp() inside a 32-bit grep
noinst_PROGRAMS += symbolic_2
symbolic_2_SOURCES = papers/symbolic_2.C
symbolic_2_CPPFLAGS = $(ROSE_INCLUDES)
symbolic_2_LDADD = $(BOOST_LDFLAGS) libRSIM.la $(LIBS_WITH_RPATH) $(ROSE_LIBS)

EXTRA_DIST += papers/grep_sample_1.c papers/glibc_strcmp.c
grep_sample_1: papers/grep_sample_1.c
	@echo "  CC32    $@"
	@$(CC) $(CFLAGS32) -Wall -o $@ $^
glibc_strcmp: papers/glibc_strcmp.c
	@echo "  CC32    $@"
	@$(CC) $(CFLAGS32) -Wall -o $@ $^

# Location of some 32-bit programs
32BIT_REPO=/home/matzke/junk/32bit

.PHONY: run_symbolic_2_grep
run_symbolic_2_grep: symbolic_2 grep_sample_1
	./symbolic_2 --debug --trigger=main --analysis-func=guess_type --take-branch=yyyn ./grep_sample_1
run_symbolic_2_strcmp: symbolic_2 glibc_strcmp
	./symbolic_2 --debug --trigger=main --analysis-func=glibc_strcmp --take-branch=n ./glibc_strcmp
run_symbolic_2_grep_strcmp: symbolic_2 $(32BIT_REPO)/grep
	: 0x08049508 is __libc_start_main@plt
	: 0x08049908 is strcmp@plt
	LD_BIND_NOW=y ./symbolic_2 --debug --trigger=0x08049508 --analysis-func=0x08049908 --take-branch=y $(32BIT_REPO)/grep x /dev/null
run_symbolic_2_vim_strcmp: symbolic_2 $(32BIT_REPO)/vim.tiny
	: 0x08049b50 is __libc_start_main@plt
	: 0x0804a100 is strcmp@plt
	LD_BIND_NOW=y ./symbolic_2 --debug --trigger=0x08049b50 --analysis-func=0x0804a100 --take-branch=y $(32BIT_REPO)/vim.tiny
run_symbolic_2_grep_strchr: symbolic_2 $(32BIT_REPO)/grep
	: 0x08049508 is __libc_start_main@plt
	: 0x08049488 is strchr@plt
	LD_BIND_NOW=y ./symbolic_2 --debug --trigger=0x08049508 --analysis-func=0x08049488 --take-branch=nnnnnnyy $(32BIT_REPO)/grep x /dev/null


#------------------------------------------------------------------------------------------------------------------------------
# Analysis to determine type info
noinst_PROGRAMS += types_1
types_1_SOURCES = papers/types_1.C
types_1_CPPFLAGS = $(ROSE_INCLUDES)
types_1_LDADD = $(BOOST_LDFLAGS) libRSIM.la $(LIBS_WITH_RPATH) $(ROSE_LIBS)

# Input files for types_1 analysis
EXTRA_DIST += local_tests/types.c local_tests/types_extern.c test_types_1.conf test_types_1.conf
MOSTLYCLEANFILES += types_extern.o libtypes_extern.{a,so} types_{static,shared}

types_extern.o: local_tests/types_extern.c
	@echo "  CC32    $@"
	@$(CC) $(CFLAGS32) -fPIC -o $@ -c $^

libtypes_extern.a: types_extern.o
	@echo "  AR      $@"
	@ar rcs $@ $^

libtypes_extern.so: types_extern.o
	@echo "  LINK    $@"
	@$(CC) $(CFLAGS32) -shared -fPIC -Wl,-soname,$@ -o $@ $^

types_static: local_tests/types.c libtypes_extern.a
	@echo "  LINK    $@"
	@$(CC) $(CFLAGS32) -static -o $@ $^

types_shared: local_tests/types.c libtypes_extern.so
	@echo "  LINK    $@"
	@$(CC) $(CFLAGS32) -fPIC -o $@ $^

# Names of functions to analyze in types_static and types_shared executables
TYPES_1_FUNCTIONS =simple01 simple02 simple03 simple04 simple05 simple06 simple07 simple08 simple09 simple10 \
                   simple11 simple12 simple13 simple14 simple15 simple16 simple17 simple18 simple19 simple20 \
                   simple21 simple22 simple23 simple24 simple25 \
                   flow01 flow02 flow03 flow04 flow05 flow06 flow07

# Test targets for types_1 analysis.  Done this way for parallelism, etc.  See scripts/rth_run.pl --help for details.
if ENABLE_I386
TYPES_1_STATIC_TARGETS = $(addprefix test_types_1_,$(addsuffix _static.passed,$(TYPES_1_FUNCTIONS)))
TYPES_1_SHARED_TARGETS = $(addprefix test_types_1_,$(addsuffix _shared.passed,$(TYPES_1_FUNCTIONS)))
$(TYPES_1_STATIC_TARGETS): %.passed: $(srcdir)/test_types_1.conf types_1 types_static
	@$(RTH_RUN) ANALYSIS_FUNC=$(patsubst test_types_1_%_static.passed,%,$@) SPECIMEN=./types_static \
		$(srcdir)/test_types_1.conf $@
$(TYPES_1_SHARED_TARGETS): %.passed: $(srcdir)/test_types_1.conf types_1 types_shared
	@$(RTH_RUN) ANALYSIS_FUNC=$(patsubst test_types_1_%_shared.passed,%,$@) SPECIMEN=./types_shared \
		$(srcdir)/test_types_1.conf $@
else
TYPES_1_STATIC_TARGETS = warn_no_tests
TYPES_1_SHARED_TARGETS = warn_no_tests
endif

.PHONY: test_types_1
test_types_1: $(TYPES_1_STATIC_TARGETS) $(TYPES_1_SHARED_TARGETS)

# Cleanup targets. We can't put all the generated files in MOSTLYCLEANFILES because the list gets too long to pass
# as arguments to "rm" (deficiency in automake).  Therefore we'll create a *.clean target for each test and have
# clean-local depend on it.
.PHONY: test_types_1_cleanups
TYPES_1_CLEANUPS = $(patsubst %.passed, %.clean, $(TYPES_1_STATIC_TARGETS) $(TYPES_1_SHARED_TARGETS))
.PHONY: $(TYPES_1_CLEANUPS)
$(TYPES_1_CLEANUPS):
	rm -f $(foreach ext, passed failed err out, $(patsubst %.clean, %, $@).$(ext))

#------------------------------------------------------------------------------------------------------------------------------
# Ptraced-based "run to original entry point, then branch and count how long it takes for child to crash.
noinst_PROGRAMS += ptrace_jmp
ptrace_jmp_SOURCES = papers/ptrace_jmp.C
ptrace_jmp_CPPFLAGS = $(ROSE_INCLUDES)
ptrace_jmp_LDADD = $(LIBS_WITH_RPATH) $(ROSE_LIBS)

###############################################################################################################################
#  INTERNAL SIMULATOR TESTS
###############################################################################################################################

INTERNAL_TEST_TARGETS =

#------------------------------------------------------------------------------------------------------------------------------
# Futex tests
noinst_PROGRAMS += testFutex
testFutex_SOURCES = testFutex.C
testFutex_CPPFLAGS = $(ROSE_INCLUDES)
testFutex_LDADD = $(BOOST_LDFLAGS) libRSIM.la $(LIBS_WITH_RPATH) $(ROSE_LIBS)

# Input file (specimen) for testFutex
EXTRA_DIST += local_tests/testFutexInput.c
MOSTLYCLEANFILES += testFutexInput
testFutexInput: local_tests/testFutexInput.c
	@echo "  CC32    $@"
	@$(CC) $(CFLAGS32) -o $@ $^ -pthread -lpthread -lrt

# The test, which simulates the specimen, which makes a special system call, which tests the futex layer
if ENABLE_I386
INTERNAL_TEST_TARGETS += testFutex.passed
testFutex.passed: testFutex testFutexInput $(srcdir)/internal_tests.conf
	@$(RTH_RUN) SIMULATOR=./testFutex SPECIMEN=./testFutexInput $(srcdir)/internal_tests.conf $@
endif
MOSTLYCLEANFILES += testFutex.{passed,failed,out,err}

#------------------------------------------------------------------------------------------------------------------------------
# Transaction tests

# The executable
EXTRA_DIST += local_tests/client_transactions.c
MOSTLYCLEANFILES += client_transactions
client_transactions: local_tests/client_transactions.c librsim_client.a
	@echo "  CC32    $@"
	@$(CC) $(CFLAGS32) -o $@ -I$(srcdir) $< -L. -lrsim_client

# The test
if ENABLE_I386
EXTRA_DIST += client_transactions.conf
INTERNAL_TEST_TARGETS += client_transactions.passed
client_transactions.passed: client_transactions x86sim $(srcdir)/client_transactions.conf
	@$(RTH_RUN) SIMULATOR=./x86sim SPECIMEN=./client_transactions ANSWER=$(srcdir)/local_tests/client_transactions.ans \
	    $(srcdir)/client_transactions.conf $@
endif
MOSTLYCLEANFILES += client_transactions.{passed,failed,out,err}

#------------------------------------------------------------------------------------------------------------------------------
# all internal tests

if ENABLE_I386
else
INTERNAL_TEST_TARGETS += warn_no_tests
endif

check-internal: $(INTERNAL_TEST_TARGETS)
MOSTLYCLEANFILES += $(INTERNAL_TEST_TARGETS)



###############################################################################################################################
#  SYSTEM CALL TESTS
###############################################################################################################################

# Source and executables for test inputs. The inputs are 32-bit executables. They come in three flavors depending
# on how they're compiled and linked:
#    SYS_TEST_INPUTS_SA  -- stand-alone, no additional source required
#    SYS_TEST_INPUTS_RT  -- require linking with librt
#    SYS_TEST_INPUTS_TS  -- use the SYS_TEST_SUPPORT library
SYS_TEST_SUPPORT_SRC = $(srcdir)/syscall_tests/audit_parse_verify.c \
                   $(srcdir)/syscall_tests/audit_parse_verify_utils.c \
                   $(srcdir)/syscall_tests/audit_utils.c \
                   $(srcdir)/syscall_tests/cleanup.c \
                   $(srcdir)/syscall_tests/cloner.c \
                   $(srcdir)/syscall_tests/get_high_address.c \
                   $(srcdir)/syscall_tests/libtestsuite.c \
                   $(srcdir)/syscall_tests/parse_opts.c \
                   $(srcdir)/syscall_tests/rmobj.c \
                   $(srcdir)/syscall_tests/search_path.c \
                   $(srcdir)/syscall_tests/simple_test_looping.c \
                   $(srcdir)/syscall_tests/system_specific_process_info.c \
                   $(srcdir)/syscall_tests/tst_is_cwd.c \
                   $(srcdir)/syscall_tests/tst_kvercmp.c \
                   $(srcdir)/syscall_tests/tst_res.c \
                   $(srcdir)/syscall_tests/tst_sig.c \
                   $(srcdir)/syscall_tests/tst_tmpdir.c

EXTRA_DIST +=      $(SYS_TEST_SUPPORT_SRC) \
		   $(srcdir)/syscall_tests/audit_parse_verify.h \
		   $(srcdir)/syscall_tests/audit_utils.h \
		   $(srcdir)/syscall_tests/bintest_config.h \
		   $(srcdir)/syscall_tests/clone_platform.h \
		   $(srcdir)/syscall_tests/compat_16.h \
		   $(srcdir)/syscall_tests/compat_uid.h \
		   $(srcdir)/syscall_tests/compiler.h \
		   $(srcdir)/syscall_tests/getdents.h \
		   $(srcdir)/syscall_tests/include_j_h.h \
		   $(srcdir)/syscall_tests/includes.h \
		   $(srcdir)/syscall_tests/ipcmsg.h \
		   $(srcdir)/syscall_tests/ipcsem.h \
		   $(srcdir)/syscall_tests/ipcshm.h \
		   $(srcdir)/syscall_tests/libtestsuite.h \
		   $(srcdir)/syscall_tests/linux_syscall_numbers.h \
		   $(srcdir)/syscall_tests/ltp_signal.h \
		   $(srcdir)/syscall_tests/rmobj.h \
		   $(srcdir)/syscall_tests/search_path.h \
		   $(srcdir)/syscall_tests/set_thread_area.h \
		   $(srcdir)/syscall_tests/system_specific_process_info.h \
		   $(srcdir)/syscall_tests/test.h \
		   $(srcdir)/syscall_tests/usctest.h


# SYS_TEST_INPUTS_SA_SRC = $(wildcard $(srcdir)/syscall_tests/syscall_sa*.c)
SYS_TEST_INPUTS_SA_SRC = \
		   $(srcdir)/syscall_tests/syscall_sa.114.c \
		   $(srcdir)/syscall_tests/syscall_sa.122.01.c \
		   $(srcdir)/syscall_tests/syscall_sa.122.02.c \
		   $(srcdir)/syscall_tests/syscall_sa.122.03.c \
		   $(srcdir)/syscall_tests/syscall_sa.13.c \
		   $(srcdir)/syscall_tests/syscall_sa.197.1.c \
		   $(srcdir)/syscall_tests/syscall_sa.197.3.c \
		   $(srcdir)/syscall_tests/syscall_sa.199.c \
		   $(srcdir)/syscall_tests/syscall_sa.200.c \
		   $(srcdir)/syscall_tests/syscall_sa.201.c \
		   $(srcdir)/syscall_tests/syscall_sa.202.c \
		   $(srcdir)/syscall_tests/syscall_sa.20.c \
		   $(srcdir)/syscall_tests/syscall_sa.224.c \
		   $(srcdir)/syscall_tests/syscall_sa.24.c \
		   $(srcdir)/syscall_tests/syscall_sa.3.c \
		   $(srcdir)/syscall_tests/syscall_sa.4.1.c \
		   $(srcdir)/syscall_tests/syscall_sa.41.c \
		   $(srcdir)/syscall_tests/syscall_sa.4.2.c \
		   $(srcdir)/syscall_tests/syscall_sa.47.c \
		   $(srcdir)/syscall_tests/syscall_sa.49.c \
		   $(srcdir)/syscall_tests/syscall_sa.50.c \
		   $(srcdir)/syscall_tests/syscall_sa.5.1.c \
		   $(srcdir)/syscall_tests/syscall_sa.5.2.c \
		   $(srcdir)/syscall_tests/syscall_sa.5.4.c \
		   $(srcdir)/syscall_tests/syscall_sa.5.6.c \
		   $(srcdir)/syscall_tests/syscall_sa.57.c \
		   $(srcdir)/syscall_tests/syscall_sa.5.9.c \
		   $(srcdir)/syscall_tests/syscall_sa.64.c \
		   $(srcdir)/syscall_tests/syscall_sa.65.c \
		   $(srcdir)/syscall_tests/syscall_sa.78.c \
		   $(srcdir)/syscall_tests/syscall_sa.83.c \
		   $(srcdir)/syscall_tests/syscall_sa.85.c 

#SYS_TEST_INPUTS_RT_SRC = $(wildcard $(srcdir)/syscall_tests/syscall_rt*.c)
SYS_TEST_INPUTS_RT_SRC = \
                   $(srcdir)/syscall_tests/syscall_rt.18.c


# SYS_TEST_INPUTS_TS_SRC = $(wildcard $(srcdir)/syscall_tests/syscall_ts*.c)
SYS_TEST_INPUTS_TS_SRC = \
		   $(srcdir)/syscall_tests/syscall_tst.100.1.c \
		   $(srcdir)/syscall_tests/syscall_tst.100.2.c \
		   $(srcdir)/syscall_tests/syscall_tst.102.1.c \
		   $(srcdir)/syscall_tests/syscall_tst.102.2.c \
		   $(srcdir)/syscall_tests/syscall_tst.102.3.c \
		   $(srcdir)/syscall_tests/syscall_tst.102.4.c \
		   $(srcdir)/syscall_tests/syscall_tst.114.1.c \
		   $(srcdir)/syscall_tests/syscall_tst.114.2.c \
		   $(srcdir)/syscall_tests/syscall_tst.116.1.c \
		   $(srcdir)/syscall_tests/syscall_tst.116.2.c \
		   $(srcdir)/syscall_tests/syscall_tst.117.c \
		   $(srcdir)/syscall_tests/syscall_tst.117.msgctl.01.c \
		   $(srcdir)/syscall_tests/syscall_tst.117.msgctl.02.c \
		   $(srcdir)/syscall_tests/syscall_tst.117.msgctl.03.c \
		   $(srcdir)/syscall_tests/syscall_tst.117.msgctl.08.c \
		   $(srcdir)/syscall_tests/syscall_tst.117.msgget.01.c \
		   $(srcdir)/syscall_tests/syscall_tst.117.msgget.02.c \
		   $(srcdir)/syscall_tests/syscall_tst.117.msgrcv.01.c \
		   $(srcdir)/syscall_tests/syscall_tst.117.msgrcv.03.c \
		   $(srcdir)/syscall_tests/syscall_tst.117.msgrcv.04.c \
		   $(srcdir)/syscall_tests/syscall_tst.117.msgrcv.05.c \
		   $(srcdir)/syscall_tests/syscall_tst.117.msgrcv.06.c \
		   $(srcdir)/syscall_tests/syscall_tst.117.msgsnd.01.c \
		   $(srcdir)/syscall_tests/syscall_tst.117.msgsnd.03.c \
		   $(srcdir)/syscall_tests/syscall_tst.117.msgsnd.04.c \
		   $(srcdir)/syscall_tests/syscall_tst.117.msgsnd.05.c \
		   $(srcdir)/syscall_tests/syscall_tst.117.msgsnd.06.c \
		   $(srcdir)/syscall_tests/syscall_tst.117.semctl.01.c \
		   $(srcdir)/syscall_tests/syscall_tst.117.semctl.05.c \
		   $(srcdir)/syscall_tests/syscall_tst.117.semget.01.c \
		   $(srcdir)/syscall_tests/syscall_tst.117.semget.03.c \
		   $(srcdir)/syscall_tests/syscall_tst.117.semop.01.c \
		   $(srcdir)/syscall_tests/syscall_tst.117.semop.03.c \
		   $(srcdir)/syscall_tests/syscall_tst.117.semop.04.c \
		   $(srcdir)/syscall_tests/syscall_tst.117.semop.05.c \
		   $(srcdir)/syscall_tests/syscall_tst.117.shmat.01.c \
		   $(srcdir)/syscall_tests/syscall_tst.117.shmctl.01.c \
		   $(srcdir)/syscall_tests/syscall_tst.117.shmctl.04.c \
		   $(srcdir)/syscall_tests/syscall_tst.117.shmdt.01.c \
		   $(srcdir)/syscall_tests/syscall_tst.117.shmdt.02.c \
		   $(srcdir)/syscall_tests/syscall_tst.117.shmget.01.c \
		   $(srcdir)/syscall_tests/syscall_tst.117.shmget.02.c \
		   $(srcdir)/syscall_tests/syscall_tst.118.1.c \
		   $(srcdir)/syscall_tests/syscall_tst.118.3.c \
		   $(srcdir)/syscall_tests/syscall_tst.122.c \
		   $(srcdir)/syscall_tests/syscall_tst.125.1.c \
		   $(srcdir)/syscall_tests/syscall_tst.125.2.c \
		   $(srcdir)/syscall_tests/syscall_tst.125.3.c \
		   $(srcdir)/syscall_tests/syscall_tst.133.c \
		   $(srcdir)/syscall_tests/syscall_tst.140.2.c \
		   $(srcdir)/syscall_tests/syscall_tst.141.1.c \
		   $(srcdir)/syscall_tests/syscall_tst.141.2.c \
		   $(srcdir)/syscall_tests/syscall_tst.141.3.c \
		   $(srcdir)/syscall_tests/syscall_tst.141.4.c \
		   $(srcdir)/syscall_tests/syscall_tst.142.1.c \
		   $(srcdir)/syscall_tests/syscall_tst.142.2.c \
		   $(srcdir)/syscall_tests/syscall_tst.142.3.c \
		   $(srcdir)/syscall_tests/syscall_tst.144.1.c \
		   $(srcdir)/syscall_tests/syscall_tst.144.2.c \
		   $(srcdir)/syscall_tests/syscall_tst.144.3.c \
		   $(srcdir)/syscall_tests/syscall_tst.144.4.c \
		   $(srcdir)/syscall_tests/syscall_tst.144.5.c \
		   $(srcdir)/syscall_tests/syscall_tst.146.1.c \
		   $(srcdir)/syscall_tests/syscall_tst.146.2.c \
		   $(srcdir)/syscall_tests/syscall_tst.146.3.c \
		   $(srcdir)/syscall_tests/syscall_tst.146.4.c \
		   $(srcdir)/syscall_tests/syscall_tst.146.5.c \
		   $(srcdir)/syscall_tests/syscall_tst.146.6.c \
		   $(srcdir)/syscall_tests/syscall_tst.162.1.c \
		   $(srcdir)/syscall_tests/syscall_tst.162.2.c \
		   $(srcdir)/syscall_tests/syscall_tst.162.3.c \
		   $(srcdir)/syscall_tests/syscall_tst.162.4.c \
		   $(srcdir)/syscall_tests/syscall_tst.174.1.c \
		   $(srcdir)/syscall_tests/syscall_tst.174.2.c \
		   $(srcdir)/syscall_tests/syscall_tst.174.3.c \
		   $(srcdir)/syscall_tests/syscall_tst.175.c \
		   $(srcdir)/syscall_tests/syscall_tst.176.1.c \
		   $(srcdir)/syscall_tests/syscall_tst.179.1.c \
		   $(srcdir)/syscall_tests/syscall_tst.183.1.c \
		   $(srcdir)/syscall_tests/syscall_tst.183.2.c \
		   $(srcdir)/syscall_tests/syscall_tst.186.1.c \
		   $(srcdir)/syscall_tests/syscall_tst.186.2.c \
		   $(srcdir)/syscall_tests/syscall_tst_18.c \
		   $(srcdir)/syscall_tests/syscall_tst.192.10.c \
		   $(srcdir)/syscall_tests/syscall_tst.192.1.c \
		   $(srcdir)/syscall_tests/syscall_tst.192.2.c \
		   $(srcdir)/syscall_tests/syscall_tst.192.3.c \
		   $(srcdir)/syscall_tests/syscall_tst.192.4.c \
		   $(srcdir)/syscall_tests/syscall_tst.192.5.c \
		   $(srcdir)/syscall_tests/syscall_tst.192.6.c \
		   $(srcdir)/syscall_tests/syscall_tst.192.7.c \
		   $(srcdir)/syscall_tests/syscall_tst.192.8.c \
		   $(srcdir)/syscall_tests/syscall_tst.192.9.c \
		   $(srcdir)/syscall_tests/syscall_tst.195.5.c \
		   $(srcdir)/syscall_tests/syscall_tst.195.6.c \
		   $(srcdir)/syscall_tests/syscall_tst.196.1.c \
		   $(srcdir)/syscall_tests/syscall_tst.197.1.c \
		   $(srcdir)/syscall_tests/syscall_tst.197.2.c \
		   $(srcdir)/syscall_tests/syscall_tst.199.1.c \
		   $(srcdir)/syscall_tests/syscall_tst.199.2.c \
		   $(srcdir)/syscall_tests/syscall_tst.199.3.c \
		   $(srcdir)/syscall_tests/syscall_tst.200.1.c \
		   $(srcdir)/syscall_tests/syscall_tst.200.3.c \
		   $(srcdir)/syscall_tests/syscall_tst.201.1.c \
		   $(srcdir)/syscall_tests/syscall_tst.205.1.c \
		   $(srcdir)/syscall_tests/syscall_tst.205.2.c \
		   $(srcdir)/syscall_tests/syscall_tst.205.4.c \
		   $(srcdir)/syscall_tests/syscall_tst.207.c \
		   $(srcdir)/syscall_tests/syscall_tst.212.c \
		   $(srcdir)/syscall_tests/syscall_tst.220.1.c \
		   $(srcdir)/syscall_tests/syscall_tst.220.2.c \
		   $(srcdir)/syscall_tests/syscall_tst.220.3.c \
		   $(srcdir)/syscall_tests/syscall_tst.220.4.c \
		   $(srcdir)/syscall_tests/syscall_tst.221.01.c \
		   $(srcdir)/syscall_tests/syscall_tst.221.02.c \
		   $(srcdir)/syscall_tests/syscall_tst.221.03.c \
		   $(srcdir)/syscall_tests/syscall_tst.221.04.c \
		   $(srcdir)/syscall_tests/syscall_tst.221.05.c \
		   $(srcdir)/syscall_tests/syscall_tst.221.06.c \
		   $(srcdir)/syscall_tests/syscall_tst.221.07.c \
		   $(srcdir)/syscall_tests/syscall_tst.221.08.c \
		   $(srcdir)/syscall_tests/syscall_tst.221.09.c \
		   $(srcdir)/syscall_tests/syscall_tst.221.10.c \
		   $(srcdir)/syscall_tests/syscall_tst.221.11.c \
		   $(srcdir)/syscall_tests/syscall_tst.221.12.c \
		   $(srcdir)/syscall_tests/syscall_tst.221.13.c \
		   $(srcdir)/syscall_tests/syscall_tst.221.16.c \
		   $(srcdir)/syscall_tests/syscall_tst.221.17.c \
		   $(srcdir)/syscall_tests/syscall_tst.221.19.c \
		   $(srcdir)/syscall_tests/syscall_tst.221.20.c \
		   $(srcdir)/syscall_tests/syscall_tst.221.21.c \
		   $(srcdir)/syscall_tests/syscall_tst.221.22.c \
		   $(srcdir)/syscall_tests/syscall_tst.221.23.c \
		   $(srcdir)/syscall_tests/syscall_tst.221.24.c \
		   $(srcdir)/syscall_tests/syscall_tst.221.25.c \
		   $(srcdir)/syscall_tests/syscall_tst.221.26.c \
		   $(srcdir)/syscall_tests/syscall_tst.221.27.c \
		   $(srcdir)/syscall_tests/syscall_tst.221.28.c \
		   $(srcdir)/syscall_tests/syscall_tst.224.c \
		   $(srcdir)/syscall_tests/syscall_tst.243.1.c \
		   $(srcdir)/syscall_tests/syscall_tst.243.2.c \
		   $(srcdir)/syscall_tests/syscall_tst.252.c \
		   $(srcdir)/syscall_tests/syscall_tst.258.c \
		   $(srcdir)/syscall_tests/syscall_tst.265.c \
		   $(srcdir)/syscall_tests/syscall_tst.266.c \
		   $(srcdir)/syscall_tests/syscall_tst.306.c \
		   $(srcdir)/syscall_tests/syscall_tst.1.1.c \
		   $(srcdir)/syscall_tests/syscall_tst.1.2.c \
		   $(srcdir)/syscall_tests/syscall_tst.3.1.c \
		   $(srcdir)/syscall_tests/syscall_tst.3.2.c \
		   $(srcdir)/syscall_tests/syscall_tst.3.3.c \
		   $(srcdir)/syscall_tests/syscall_tst.3.4.c \
		   $(srcdir)/syscall_tests/syscall_tst.4.1.c \
		   $(srcdir)/syscall_tests/syscall_tst.4.2.c \
		   $(srcdir)/syscall_tests/syscall_tst.4.3.c \
		   $(srcdir)/syscall_tests/syscall_tst.4.4.c \
		   $(srcdir)/syscall_tests/syscall_tst.4.5.c \
		   $(srcdir)/syscall_tests/syscall_tst.9.1.c \
		   $(srcdir)/syscall_tests/syscall_tst.9.2.c \
		   $(srcdir)/syscall_tests/syscall_tst.9.3.c \
		   $(srcdir)/syscall_tests/syscall_tst.9.4.c \
		   $(srcdir)/syscall_tests/syscall_tst.13.1.c \
		   $(srcdir)/syscall_tests/syscall_tst.13.2.c \
		   $(srcdir)/syscall_tests/syscall_tst.19.1.c \
		   $(srcdir)/syscall_tests/syscall_tst.19.2.c \
		   $(srcdir)/syscall_tests/syscall_tst.19.3.c \
		   $(srcdir)/syscall_tests/syscall_tst.19.4.c \
		   $(srcdir)/syscall_tests/syscall_tst.19.5.c \
		   $(srcdir)/syscall_tests/syscall_tst.19.6.c \
		   $(srcdir)/syscall_tests/syscall_tst.19.7.c \
		   $(srcdir)/syscall_tests/syscall_tst.19.8.c \
		   $(srcdir)/syscall_tests/syscall_tst.19.9.c \
		   $(srcdir)/syscall_tests/syscall_tst.19.10.c \
		   $(srcdir)/syscall_tests/syscall_tst.20.1.c \
		   $(srcdir)/syscall_tests/syscall_tst.20.2.c \
		   $(srcdir)/syscall_tests/syscall_tst.29.1.c \
		   $(srcdir)/syscall_tests/syscall_tst.29.2.c \
		   $(srcdir)/syscall_tests/syscall_tst.29.3.c \
		   $(srcdir)/syscall_tests/syscall_tst.30.c \
		   $(srcdir)/syscall_tests/syscall_tst.311.c \
		   $(srcdir)/syscall_tests/syscall_tst.33.1.c \
		   $(srcdir)/syscall_tests/syscall_tst.33.2.c \
		   $(srcdir)/syscall_tests/syscall_tst.36.1.c \
		   $(srcdir)/syscall_tests/syscall_tst.36.2.c \
		   $(srcdir)/syscall_tests/syscall_tst.38.1.c \
		   $(srcdir)/syscall_tests/syscall_tst.38.2.c \
		   $(srcdir)/syscall_tests/syscall_tst.38.3.c \
		   $(srcdir)/syscall_tests/syscall_tst.38.4.c \
		   $(srcdir)/syscall_tests/syscall_tst.38.5.c \
		   $(srcdir)/syscall_tests/syscall_tst.38.6.c \
		   $(srcdir)/syscall_tests/syscall_tst.38.7.c \
		   $(srcdir)/syscall_tests/syscall_tst.38.8.c \
		   $(srcdir)/syscall_tests/syscall_tst.38.10.c \
		   $(srcdir)/syscall_tests/syscall_tst.38.13.c \
		   $(srcdir)/syscall_tests/syscall_tst.38.14.c \
		   $(srcdir)/syscall_tests/syscall_tst.24.1.c \
		   $(srcdir)/syscall_tests/syscall_tst.24.2.c \
		   $(srcdir)/syscall_tests/syscall_tst.24.3.c \
		   $(srcdir)/syscall_tests/syscall_tst.57.1.c \
		   $(srcdir)/syscall_tests/syscall_tst.57.2.c \
		   $(srcdir)/syscall_tests/syscall_tst.57.3.c \
		   $(srcdir)/syscall_tests/syscall_tst.60.1.c \
		   $(srcdir)/syscall_tests/syscall_tst.60.2.c \
		   $(srcdir)/syscall_tests/syscall_tst.60.3.c \
		   $(srcdir)/syscall_tests/syscall_tst.63.1.c \
		   $(srcdir)/syscall_tests/syscall_tst.63.2.c \
		   $(srcdir)/syscall_tests/syscall_tst.63.3.c \
		   $(srcdir)/syscall_tests/syscall_tst.63.4.c \
		   $(srcdir)/syscall_tests/syscall_tst.63.5.c \
		   $(srcdir)/syscall_tests/syscall_tst.64.1.c \
		   $(srcdir)/syscall_tests/syscall_tst.64.2.c \
		   $(srcdir)/syscall_tests/syscall_tst.76.1.c \
		   $(srcdir)/syscall_tests/syscall_tst.76.2.c \
		   $(srcdir)/syscall_tests/syscall_tst.78.1.c \
		   $(srcdir)/syscall_tests/syscall_tst.83.1.c \
		   $(srcdir)/syscall_tests/syscall_tst.83.2.c \
		   $(srcdir)/syscall_tests/syscall_tst.83.4.c \
		   $(srcdir)/syscall_tests/syscall_tst.83.5.c \
		   $(srcdir)/syscall_tests/syscall_tst.91.1.c \
		   $(srcdir)/syscall_tests/syscall_tst.91.2.c \
		   $(srcdir)/syscall_tests/syscall_tst.91.3.c \
		   $(srcdir)/syscall_tests/syscall_tst.93.1.c \
		   $(srcdir)/syscall_tests/syscall_tst.93.2.c \
		   $(srcdir)/syscall_tests/syscall_tst.93.3.c \
		   $(srcdir)/syscall_tests/syscall_tst.93.4.c \
		   $(srcdir)/syscall_tests/syscall_tst.94.1.c \
		   $(srcdir)/syscall_tests/syscall_tst.94.2.c \
		   $(srcdir)/syscall_tests/syscall_tst.99.1.c \
		   $(srcdir)/syscall_tests/syscall_tst.99.2.c

# DQ (2/13/2011): This test fails to terminate when run on Hudson 
# (failed on hudson-rose-07, email with error details sent morning of 2/13/2011 to Robb and Andreas).
#		   $(srcdir)/syscall_tests/syscall_tst.183.3.c
# Liao 2/16/2011: I have to comment out this since  ERROR: Cannot create file /tmp/notty		   
#		   $(srcdir)/syscall_tests/syscall_tst.54.c


# These cannot be run reliably (at least not concurrently with other tests.
# This variable is unused, but the scripts/policies/UnusedSources policy picks up these names to make these
# otherwise unused fails as used.
TEST_INPUTS_FAILING = \
		   $(srcdir)/tests/syscall_tst.117.msgget.03.c \
		   $(srcdir)/tests/syscall_tst.117.shmget.03.c \
		   $(srcdir)/tests/syscall_tst.78.2.c          \
		   $(srcdir)/tests/syscall_tst.95.c 	       \
		   $(srcdir)/tests/syscall_tst.117.msgctl.06.c \
		   $(srcdir)/tests/syscall_tst.117.msgctl.07.c \
		   $(srcdir)/tests/syscall_tst.117.msgget.03.c \
		   $(srcdir)/tests/syscall_tst.117.msgget.04.c \
		   $(srcdir)/tests/syscall_tst.117.msgrcv.02.c \
		   $(srcdir)/tests/syscall_tst.117.msgsnd.02.c \
		   $(srcdir)/tests/syscall_tst.117.semctl.02.c \
		   $(srcdir)/tests/syscall_tst.117.semctl.03.c \
		   $(srcdir)/tests/syscall_tst.117.semctl.04.c \
		   $(srcdir)/tests/syscall_tst.117.semget.02.c \
		   $(srcdir)/tests/syscall_tst.117.semop.02.c  \
		   $(srcdir)/tests/syscall_tst.117.shmat.02.c  \
		   $(srcdir)/tests/syscall_tst.117.shmat.03.c  \
		   $(srcdir)/tests/syscall_tst.117.shmctl.02.c \
		   $(srcdir)/tests/syscall_tst.117.shmctl.03.c \
		   $(srcdir)/tests/syscall_tst.117.shmget.03.c \
		   $(srcdir)/tests/syscall_tst.117.shmget.04.c \
		   $(srcdir)/tests/syscall_tst.117.shmget.05.c \
		   $(srcdir)/syscall_tests/syscall_tst.118.2.c \
		   $(srcdir)/tests/syscall_tst.195.1.c	       \
		   $(srcdir)/tests/syscall_tst.195.2.c	       \
		   $(srcdir)/tests/syscall_tst.195.3.c	       \
		   $(srcdir)/tests/syscall_tst.196.2.c	       \
		   $(srcdir)/tests/syscall_tst.196.3.c	       \
		   $(srcdir)/tests/syscall_tst.221.18.c	       \
		   $(srcdir)/tests/syscall_tst.83.3.c	       \
		   $(srcdir)/tests/syscall_tst.99.3.c	       \
		   $(srcdir)/tests/syscall_tst.117.msgctl.09.c \
		   $(srcdir)/tests/syscall_tst.117.msgctl.10.c \
		   $(srcdir)/tests/syscall_tst.117.msgctl.11.c \
		   $(srcdir)/tests/syscall_tst.117.semctl.06.c \
		   $(srcdir)/tests/syscall_tst.221.14.c	       \
		   $(srcdir)/tests/syscall_tst.221.15.c	       \
		   $(srcdir)/tests/syscall_tst.140.1.c

#	Comments about $(TEST_INPUTS_FAILING) and why they're commented out.
#		   $(srcdir)/syscall_tests/syscall_tst.117.msgget.03.c
#		   $(srcdir)/syscall_tests/syscall_tst.117.shmget.03.c
#  			Tries to create message queues or shared memory segments until all resources are exhausted in order
#                       to see what kind of error number is returned. Unfortunately, other tests are also requiring these
#			resources at the same time.
#		   $(srcdir)/syscall_tests/syscall_tst.78.2.c
#			Calls gettimeofday() in rapid succession for 30 seconds to make sure the value is monotonically
#			increasing. However, some versions of the kernel have a bug in the way they interact with the RTC
#			which can cause the clock to go backward by small amounts every so often.  See this discussion
#			for an example: http://www.gossamer-threads.com/lists/linux/kernel/813344
#		   $(srcdir)/syscall_tests/syscall_tst.95.c 
#			This test does not pass natively on some systems, therefore we shouldn't expect it to pass
#			under x86sim either.
#		   $(srcdir)/syscall_tests/syscall_tst.117.msgctl.06.c
#		   $(srcdir)/syscall_tests/syscall_tst.117.msgctl.07.c
#		   $(srcdir)/syscall_tests/syscall_tst.117.msgget.04.c
#		   $(srcdir)/syscall_tests/syscall_tst.117.msgrcv.02.c
#		   $(srcdir)/syscall_tests/syscall_tst.117.msgsnd.02.c
#		   $(srcdir)/syscall_tests/syscall_tst.117.semctl.02.c
#		   $(srcdir)/syscall_tests/syscall_tst.117.semctl.03.c
#		   $(srcdir)/syscall_tests/syscall_tst.117.semctl.04.c
#		   $(srcdir)/syscall_tests/syscall_tst.117.semget.02.c
#		   $(srcdir)/syscall_tests/syscall_tst.117.semop.02.c
#		   $(srcdir)/syscall_tests/syscall_tst.117.shmat.02.c
#		   $(srcdir)/syscall_tests/syscall_tst.117.shmat.03.c
#		   $(srcdir)/syscall_tests/syscall_tst.117.shmctl.02.c
#		   $(srcdir)/syscall_tests/syscall_tst.117.shmctl.03.c
#		   $(srcdir)/syscall_tests/syscall_tst.117.shmget.04.c
#		   $(srcdir)/syscall_tests/syscall_tst.117.shmget.05.c
#		   $(srcdir)/syscall_tests/syscall_tst.195.1.c
#		   $(srcdir)/syscall_tests/syscall_tst.195.2.c
#		   $(srcdir)/syscall_tests/syscall_tst.195.3.c
#		   $(srcdir)/syscall_tests/syscall_tst.196.2.c
#		   $(srcdir)/syscall_tests/syscall_tst.196.3.c
#		   $(srcdir)/syscall_tests/syscall_tst.221.18.c  (tries to setreud in a privileged way)
#		   $(srcdir)/syscall_tests/syscall_tst.83.3.c
#		   $(srcdir)/syscall_tests/syscall_tst.99.3.c
#			These tests either explicitly check for root effective uid, or the fail with EACCESS on some
#			kernels. We can't be testing ROSE as root!
#		   $(srcdir)/syscall_tests/syscall_tst.117.msgctl.09.c
#		   $(srcdir)/syscall_tests/syscall_tst.117.msgctl.10.c
#		   $(srcdir)/syscall_tests/syscall_tst.117.msgctl.11.c
#		   $(srcdir)/syscall_tests/syscall_tst.117.semctl.06.c
#		   $(srcdir)/syscall_tests/syscall_tst.221.14.c
#		   $(srcdir)/syscall_tests/syscall_tst.221.15.c
#			Disabled because of deadlocks in simulated futex() call.  These need more work and should be enabled
#			when possible. [FIXME RPM 2011-01-07]
#		   $(srcdir)/syscall_tests/syscall_tst.140.1.c
#			This test cannot be run under the simulator with stderr redirected to a file (it enters an infinite loop
#			somewhere in the setrlimit() syscall emulation). Since "make check" runs tests with their stderr
#			redirected to a file, we will skip this test.
#		   $(srcdir)/syscall_tests/syscall_tst.118.2.c
#			Uses floating point instructions



SYS_TEST_INPUTS_NM_SRC = \
		   $(srcdir)/syscall_tests/syscall.114.c \
		   $(srcdir)/syscall_tests/syscall.122.01.c \
		   $(srcdir)/syscall_tests/syscall.122.02.c \
		   $(srcdir)/syscall_tests/syscall.122.03.c \
		   $(srcdir)/syscall_tests/syscall.13.c \
		   $(srcdir)/syscall_tests/syscall.197.1.c \
		   $(srcdir)/syscall_tests/syscall.197.3.c \
		   $(srcdir)/syscall_tests/syscall.199.c \
		   $(srcdir)/syscall_tests/syscall.200.c \
		   $(srcdir)/syscall_tests/syscall.201.c \
		   $(srcdir)/syscall_tests/syscall.202.c \
		   $(srcdir)/syscall_tests/syscall.20.c \
		   $(srcdir)/syscall_tests/syscall.224.c \
		   $(srcdir)/syscall_tests/syscall.3.c \
		   $(srcdir)/syscall_tests/syscall.4.1.c \
		   $(srcdir)/syscall_tests/syscall.41.c \
		   $(srcdir)/syscall_tests/syscall.4.2.c \
		   $(srcdir)/syscall_tests/syscall.47.c \
		   $(srcdir)/syscall_tests/syscall.49.c \
		   $(srcdir)/syscall_tests/syscall.50.c \
		   $(srcdir)/syscall_tests/syscall.5.1.c \
		   $(srcdir)/syscall_tests/syscall.5.2.c \
		   $(srcdir)/syscall_tests/syscall.5.4.c \
		   $(srcdir)/syscall_tests/syscall.5.6.c \
		   $(srcdir)/syscall_tests/syscall.57.c \
		   $(srcdir)/syscall_tests/syscall.5.9.c \
		   $(srcdir)/syscall_tests/syscall.64.c \
		   $(srcdir)/syscall_tests/syscall.65.c \
		   $(srcdir)/syscall_tests/syscall.78.c \
		   $(srcdir)/syscall_tests/syscall.83.c \
		   $(srcdir)/syscall_tests/syscall.85.c 

SYS_TEST_INPUTS_SRC    = $(SYS_TEST_INPUTS_SA_SRC) $(SYS_TEST_INPUTS_RT_SRC) $(SYS_TEST_INPUTS_TS_SRC) $(SYS_TEST_INPUTS_NM_SRC)
EXTRA_DIST += $(SYS_TEST_INPUTS_SRC)

SYS_TEST_INPUTS_SA = $(basename $(notdir $(SYS_TEST_INPUTS_SA_SRC)))
SYS_TEST_INPUTS_RT = $(basename $(notdir $(SYS_TEST_INPUTS_RT_SRC)))
SYS_TEST_INPUTS_TS = $(basename $(notdir $(SYS_TEST_INPUTS_TS_SRC)))
SYS_TEST_INPUTS_NM = $(basename $(notdir $(SYS_TEST_INPUTS_NM_SRC)))
SYS_TEST_INPUTS    = $(SYS_TEST_INPUTS_SA) $(SYS_TEST_INPUTS_RT) $(SYS_TEST_INPUTS_TS) $(SYS_TEST_INPUTS_NM)

MOSTLYCLEANFILES += $(SYS_TEST_INPUTS) $(addsuffix .o, $(SYS_TEST_INPUTS)) \
                    $(addsuffix .o, $(basename $(notdir $(SYS_TEST_SUPPORT_SRC))))

# The test inputs. These will not be generated if ENABLE_I386 is not defined.
$(SYS_TEST_INPUTS_SA): %: $(srcdir)/syscall_tests/%.c
	@echo "  CC32    $@"
	@$(CC) $(CFLAGS32) -g -I$(srcdir)/syscall_tests -Wall -o $@ $^
$(SYS_TEST_INPUTS_RT): %: $(srcdir)/syscall_tests/%.c
	@echo "  CC32    $@"
	@$(CC) $(CFLAGS32) -g -I$(srcdir)/syscall_tests -Wall -o $@ $^ -lrt
$(SYS_TEST_INPUTS_TS): %: $(srcdir)/syscall_tests/%.c libtestsupport.a libipc.a
	@echo "  CC32    $@"
	@$(CC) $(CFLAGS32) -g -I$(srcdir)/syscall_tests -Wall -o $@ $^ -lrt 
$(SYS_TEST_INPUTS_NM): %: $(srcdir)/syscall_tests/%.c
	@echo "  CC32    $@"
	@$(CC) $(CFLAGS32) -g -I$(srcdir)/syscall_tests -Wall -o $@ $^ 

# Target to manually build the syscall test inputs. (Not used internally)
.PHONY: sys_test_inputs
sys_test_inputs: $(SYS_TEST_INPUTS)


# The test support library. These will not be generated if ENABLE_I386 is not defined.
SYS_TEST_SUPPORT_OBJ = $(patsubst %.c, %.o, $(notdir $(SYS_TEST_SUPPORT_SRC)))
libtestsupport.a: $(SYS_TEST_SUPPORT_OBJ)
	@echo "  AR      $@"
	@ar -rc $@ $(SYS_TEST_SUPPORT_OBJ)
$(SYS_TEST_SUPPORT_OBJ): %.o: $(srcdir)/syscall_tests/%.c
	@echo "  CC32    $@"
	@$(CC) -m32 -g -I$(srcdir)/syscall_tests -Wall -o $@ -c $^
MOSTLYCLEANFILES += libtestsupport.a

# The ipc support library
include_HEADERS += $(srcdir)/syscall_tests/ipcmsg.h  $(srcdir)/syscall_tests/ipcsem.h  $(srcdir)/syscall_tests/ipcshm.h
IPC_SUPPORT_SRC = $(srcdir)/syscall_tests/libipc.c
IPC_SUPPORT_OBJ = $(patsubst %.c, %.o, $(notdir $(IPC_SUPPORT_SRC)))
EXTRA_DIST += $(IPC_SUPPORT_SRC)
libipc.a: $(IPC_SUPPORT_OBJ)
	@echo "  AR      $@"
	@ar -rc $@ $(IPC_SUPPORT_OBJ)
$(IPC_SUPPORT_OBJ): %.o: $(srcdir)/syscall_tests/%.c
	@echo "  CC32    $@"
	@$(CC) -m32 -g -I$(srcdir)/syscall_tests -Wall -o $@ -c $^
MOSTLYCLEANFILES += libipc.a

# Test targets are test names suffixed with ".passed" (but when a test fails it produces ".failed" instead).
# Each target runs "x86sim" on one of the TEST_INPUT files. Exit status determines success or failure.
# Test targets must be 32-bit x86; if we don't have that capability then the tests are not run.
if ENABLE_I386
SYS_TEST_TARGETS = $(addsuffix .passed, $(SYS_TEST_INPUTS))
$(SYS_TEST_TARGETS): %.passed: % $(srcdir)/syscall_tests.conf x86sim
	@$(RTH_RUN) INPUT=./$< $(srcdir)/syscall_tests.conf $@
else
SYS_TEST_TARGETS = warn_no_tests
endif

# Some of the tests create files name "tfile_%d_%d" where the two numbers are a serial number and a PID. About 5000 of
# these files are created in total and many of them are not placed in the specified temporary directories. Therefore
# we have to remove them with a wildcard instead. Sorry if you had your own files with similar names (we tried to be careful).
MOSTLYCLEANFILES += 						   \
	rm -f tfile_[0-9]_[0-9][0-9][0-9]			   \
	rm -f tfile_[0-9]_[0-9][0-9][0-9][0-9]			   \
	rm -f tfile_[0-9]_[0-9][0-9][0-9][0-9][0-9]		   \
	rm -f tfile_[0-9][0-9]_[0-9][0-9][0-9]			   \
	rm -f tfile_[0-9][0-9]_[0-9][0-9][0-9][0-9]		   \
	rm -f tfile_[0-9][0-9]_[0-9][0-9][0-9][0-9][0-9]	   \
	rm -f tfile_[0-9][0-9][0-9]_[0-9][0-9][0-9]		   \
	rm -f tfile_[0-9][0-9][0-9]_[0-9][0-9][0-9][0-9]	   \
	rm -f tfile_[0-9][0-9][0-9]_[0-9][0-9][0-9][0-9][0-9]	   \
	rm -f tfile_[0-9][0-9][0-9][0-9]_[0-9][0-9][0-9]	   \
	rm -f tfile_[0-9][0-9][0-9][0-9]_[0-9][0-9][0-9][0-9]	   \
	rm -f tfile_[0-9][0-9][0-9][0-9]_[0-9][0-9][0-9][0-9][0-9]

# Cleanup targets. We can't put all the generated files in MOSTLYCLEANFILES because the list gets too long to pass
# as arguments to "rm" (deficiency in automake).  Therefore we'll create a *.clean target for each test and have
# clean-local depend on it.
SYS_TEST_CLEANUPS = $(addsuffix .clean, $(SYS_TEST_INPUTS))
.PHONY: $(SYS_TEST_CLEANUPS)
$(SYS_TEST_CLEANUPS):
	rm -f $(foreach ext, passed failed err out, $(patsubst %.clean, %, $@).$(ext))

# Main syscall testing target, on which "make check" depends.
check-syscalls:  $(SYS_TEST_TARGETS)

###############################################################################################################################
#  PTHREAD TESTS
###############################################################################################################################

PTHREAD_TEST_INPUTS_SRC =							   \
		    $(srcdir)/pthread_tests/pthread_atfork.1-1.c		   \
		    $(srcdir)/pthread_tests/pthread_atfork.1-2.c		   \
		    $(srcdir)/pthread_tests/pthread_atfork.2-1.c		   \
		    $(srcdir)/pthread_tests/pthread_atfork.2-2.c		   \
		    $(srcdir)/pthread_tests/pthread_atfork.3-2.c		   \
		    $(srcdir)/pthread_tests/pthread_atfork.3-3.c		   \
		    $(srcdir)/pthread_tests/pthread_atfork.4-1.c		   \
		    $(srcdir)/pthread_tests/pthread_attr_destroy.1-1.c		   \
		    $(srcdir)/pthread_tests/pthread_attr_destroy.2-1.c		   \
		    $(srcdir)/pthread_tests/pthread_attr_destroy.3-1.c		   \
		    $(srcdir)/pthread_tests/pthread_attr_getdetachstate.1-1.c	   \
		    $(srcdir)/pthread_tests/pthread_attr_getdetachstate.1-2.c	   \
		    $(srcdir)/pthread_tests/pthread_attr_getinheritsched.1-1.c	   \
		    $(srcdir)/pthread_tests/pthread_attr_getschedparam.1-1.c	   \
		    $(srcdir)/pthread_tests/pthread_attr_getschedpolicy.2-1.c	   \
		    $(srcdir)/pthread_tests/pthread_attr_getscope.1-1.c		   \
		    $(srcdir)/pthread_tests/pthread_attr_getstack.1-1.c		   \
		    $(srcdir)/pthread_tests/pthread_attr_getstacksize.1-1.c	   \
		    $(srcdir)/pthread_tests/pthread_attr_init.1-1.c		   \
		    $(srcdir)/pthread_tests/pthread_attr_init.2-1.c		   \
		    $(srcdir)/pthread_tests/pthread_attr_init.3-1.c		   \
		    $(srcdir)/pthread_tests/pthread_attr_init.4-1.c		   \
		    $(srcdir)/pthread_tests/pthread_attr_setdetachstate.1-1.c	   \
		    $(srcdir)/pthread_tests/pthread_attr_setdetachstate.1-2.c	   \
		    $(srcdir)/pthread_tests/pthread_attr_setdetachstate.2-1.c	   \
		    $(srcdir)/pthread_tests/pthread_attr_setdetachstate.4-1.c	   \
		    $(srcdir)/pthread_tests/pthread_attr_setinheritsched.1-1.c	   \
		    $(srcdir)/pthread_tests/pthread_attr_setinheritsched.2-1.c	   \
		    $(srcdir)/pthread_tests/pthread_attr_setinheritsched.4-1.c	   \
		    $(srcdir)/pthread_tests/pthread_attr_setschedparam.1-1.c	   \
		    $(srcdir)/pthread_tests/pthread_attr_setschedparam.1-2.c	   \
		    $(srcdir)/pthread_tests/pthread_attr_setschedpolicy.1-1.c	   \
		    $(srcdir)/pthread_tests/pthread_attr_setschedpolicy.4-1.c	   \
		    $(srcdir)/pthread_tests/pthread_attr_setscope.1-1.c		   \
		    $(srcdir)/pthread_tests/pthread_attr_setscope.4-1.c		   \
		    $(srcdir)/pthread_tests/pthread_attr_setstack.1-1.c		   \
		    $(srcdir)/pthread_tests/pthread_attr_setstack.2-1.c		   \
		    $(srcdir)/pthread_tests/pthread_attr_setstack.4-1.c		   \
		    $(srcdir)/pthread_tests/pthread_attr_setstack.6-1.c		   \
		    $(srcdir)/pthread_tests/pthread_attr_setstack.7-1.c		   \
		    $(srcdir)/pthread_tests/pthread_attr_setstacksize.1-1.c	   \
		    $(srcdir)/pthread_tests/pthread_attr_setstacksize.2-1.c	   \
		    $(srcdir)/pthread_tests/pthread_attr_setstacksize.4-1.c	   \
		    $(srcdir)/pthread_tests/pthread_barrier_destroy.1-1.c	   \
		    $(srcdir)/pthread_tests/pthread_barrier_destroy.2-1.c	   \
		    $(srcdir)/pthread_tests/pthread_barrier_init.1-1.c		   \
		    $(srcdir)/pthread_tests/pthread_barrier_init.3-1.c		   \
		    $(srcdir)/pthread_tests/pthread_barrier_init.4-1.c		   \
		    $(srcdir)/pthread_tests/pthread_barrier_wait.1-1.c		   \
		    $(srcdir)/pthread_tests/pthread_barrier_wait.2-1.c		   \
		    $(srcdir)/pthread_tests/pthread_barrier_wait.3-1.c		   \
		    $(srcdir)/pthread_tests/pthread_barrier_wait.3-2.c		   \
		    $(srcdir)/pthread_tests/pthread_barrierattr_destroy.1-1.c	   \
		    $(srcdir)/pthread_tests/pthread_barrierattr_getpshared.1-1.c   \
		    $(srcdir)/pthread_tests/pthread_barrierattr_getpshared.2-1.c   \
		    $(srcdir)/pthread_tests/pthread_barrierattr_init.1-1.c	   \
		    $(srcdir)/pthread_tests/pthread_barrierattr_init.2-1.c	   \
		    $(srcdir)/pthread_tests/pthread_barrierattr_setpshared.1-1.c   \
		    $(srcdir)/pthread_tests/pthread_barrierattr_setpshared.2-1.c   \
		    $(srcdir)/pthread_tests/pthread_cancel.1-1.c		   \
		    $(srcdir)/pthread_tests/pthread_cancel.1-2.c		   \
		    $(srcdir)/pthread_tests/pthread_cancel.1-3.c		   \
		    $(srcdir)/pthread_tests/pthread_cancel.2-1.c		   \
		    $(srcdir)/pthread_tests/pthread_cancel.2-2.c		   \
		    $(srcdir)/pthread_tests/pthread_cancel.2-3.c		   \
		    $(srcdir)/pthread_tests/pthread_cancel.3-1.c		   \
		    $(srcdir)/pthread_tests/pthread_cancel.4-1.c		   \
		    $(srcdir)/pthread_tests/pthread_cancel.5-1.c		   \
		    $(srcdir)/pthread_tests/pthread_cancel.5-2.c		   \
		    $(srcdir)/pthread_tests/pthread_cleanup_pop.1-1.c		   \
		    $(srcdir)/pthread_tests/pthread_cleanup_pop.1-2.c		   \
		    $(srcdir)/pthread_tests/pthread_cleanup_pop.1-3.c		   \
		    $(srcdir)/pthread_tests/pthread_cleanup_push.1-1.c		   \
		    $(srcdir)/pthread_tests/pthread_cleanup_push.1-2.c		   \
		    $(srcdir)/pthread_tests/pthread_cleanup_push.1-3.c		   \
		    $(srcdir)/pthread_tests/pthread_cond_broadcast.1-1.c	   \
		    $(srcdir)/pthread_tests/pthread_cond_broadcast.1-2.c	   \
		    $(srcdir)/pthread_tests/pthread_cond_broadcast.2-1.c	   \
		    $(srcdir)/pthread_tests/pthread_cond_broadcast.2-2.c	   \
		    $(srcdir)/pthread_tests/pthread_cond_broadcast.2-3.c	   \
		    $(srcdir)/pthread_tests/pthread_cond_broadcast.4-1.c	   \
		    $(srcdir)/pthread_tests/pthread_cond_broadcast.4-2.c	   \
		    $(srcdir)/pthread_tests/pthread_cond_destroy.1-1.c		   \
		    $(srcdir)/pthread_tests/pthread_cond_destroy.3-1.c		   \
		    $(srcdir)/pthread_tests/pthread_cond_init.1-1.c		   \
		    $(srcdir)/pthread_tests/pthread_cond_init.1-2.c		   \
		    $(srcdir)/pthread_tests/pthread_cond_init.2-1.c		   \
		    $(srcdir)/pthread_tests/pthread_cond_init.2-2.c		   \
		    $(srcdir)/pthread_tests/pthread_cond_init.3-1.c		   \
		    $(srcdir)/pthread_tests/pthread_cond_init.4-1.c		   \
		    $(srcdir)/pthread_tests/pthread_cond_signal.1-1.c		   \
		    $(srcdir)/pthread_tests/pthread_cond_signal.2-1.c		   \
		    $(srcdir)/pthread_tests/pthread_cond_signal.2-2.c		   \
		    $(srcdir)/pthread_tests/pthread_cond_signal.4-1.c		   \
		    $(srcdir)/pthread_tests/pthread_cond_signal.4-2.c		   \
		    $(srcdir)/pthread_tests/pthread_cond_timedwait.1-1.c	   \
		    $(srcdir)/pthread_tests/pthread_cond_timedwait.2-1.c	   \
		    $(srcdir)/pthread_tests/pthread_cond_timedwait.2-2.c	   \
		    $(srcdir)/pthread_tests/pthread_cond_timedwait.2-3.c	   \
		    $(srcdir)/pthread_tests/pthread_cond_timedwait.2-4.c	   \
		    $(srcdir)/pthread_tests/pthread_cond_timedwait.2-5.c	   \
		    $(srcdir)/pthread_tests/pthread_cond_timedwait.2-6.c	   \
		    $(srcdir)/pthread_tests/pthread_cond_timedwait.2-7.c	   \
		    $(srcdir)/pthread_tests/pthread_cond_timedwait.3-1.c	   \
		    $(srcdir)/pthread_tests/pthread_cond_timedwait.4-1.c	   \
		    $(srcdir)/pthread_tests/pthread_cond_timedwait.4-2.c	   \
		    $(srcdir)/pthread_tests/pthread_cond_timedwait.4-3.c	   \
		    $(srcdir)/pthread_tests/pthread_cond_wait.1-1.c		   \
		    $(srcdir)/pthread_tests/pthread_cond_wait.2-1.c		   \
		    $(srcdir)/pthread_tests/pthread_cond_wait.2-2.c		   \
		    $(srcdir)/pthread_tests/pthread_cond_wait.2-3.c		   \
		    $(srcdir)/pthread_tests/pthread_cond_wait.3-1.c		   \
		    $(srcdir)/pthread_tests/pthread_cond_wait.4-1.c		   \
		    $(srcdir)/pthread_tests/pthread_condattr_destroy.1-1.c	   \
		    $(srcdir)/pthread_tests/pthread_condattr_destroy.2-1.c	   \
		    $(srcdir)/pthread_tests/pthread_condattr_destroy.3-1.c	   \
		    $(srcdir)/pthread_tests/pthread_condattr_destroy.4-1.c	   \
		    $(srcdir)/pthread_tests/pthread_condattr_getclock.1-1.c	   \
		    $(srcdir)/pthread_tests/pthread_condattr_getclock.1-2.c	   \
		    $(srcdir)/pthread_tests/pthread_condattr_getpshared.1-1.c	   \
		    $(srcdir)/pthread_tests/pthread_condattr_getpshared.1-2.c	   \
		    $(srcdir)/pthread_tests/pthread_condattr_getpshared.2-1.c	   \
		    $(srcdir)/pthread_tests/pthread_condattr_init.1-1.c		   \
		    $(srcdir)/pthread_tests/pthread_condattr_init.3-1.c		   \
		    $(srcdir)/pthread_tests/pthread_condattr_setclock.1-1.c	   \
		    $(srcdir)/pthread_tests/pthread_condattr_setclock.1-2.c	   \
		    $(srcdir)/pthread_tests/pthread_condattr_setclock.1-3.c	   \
		    $(srcdir)/pthread_tests/pthread_condattr_setclock.2-1.c	   \
		    $(srcdir)/pthread_tests/pthread_condattr_setpshared.1-1.c	   \
		    $(srcdir)/pthread_tests/pthread_condattr_setpshared.1-2.c	   \
		    $(srcdir)/pthread_tests/pthread_condattr_setpshared.2-1.c	   \
		    $(srcdir)/pthread_tests/pthread_create.1-1.c		   \
		    $(srcdir)/pthread_tests/pthread_create.1-2.c		   \
		    $(srcdir)/pthread_tests/pthread_create.1-3.c		   \
		    $(srcdir)/pthread_tests/pthread_create.1-4.c		   \
		    $(srcdir)/pthread_tests/pthread_create.1-5.c		   \
		    $(srcdir)/pthread_tests/pthread_create.1-6.c		   \
		    $(srcdir)/pthread_tests/pthread_create.11-1.c		   \
		    $(srcdir)/pthread_tests/pthread_create.12-1.c		   \
		    $(srcdir)/pthread_tests/pthread_create.14-1.c		   \
		    $(srcdir)/pthread_tests/pthread_create.15-1.c		   \
		    $(srcdir)/pthread_tests/pthread_create.2-1.c		   \
		    $(srcdir)/pthread_tests/pthread_create.3-1.c		   \
		    $(srcdir)/pthread_tests/pthread_create.3-2.c		   \
		    $(srcdir)/pthread_tests/pthread_create.4-1.c		   \
		    $(srcdir)/pthread_tests/pthread_create.5-1.c		   \
		    $(srcdir)/pthread_tests/pthread_create.5-2.c		   \
		    $(srcdir)/pthread_tests/pthread_create.8-1.c		   \
		    $(srcdir)/pthread_tests/pthread_create.8-2.c		   \
		    $(srcdir)/pthread_tests/pthread_detach.1-1.c		   \
		    $(srcdir)/pthread_tests/pthread_detach.1-2.c		   \
		    $(srcdir)/pthread_tests/pthread_detach.2-1.c		   \
		    $(srcdir)/pthread_tests/pthread_detach.2-2.c		   \
		    $(srcdir)/pthread_tests/pthread_detach.3-1.c		   \
		    $(srcdir)/pthread_tests/pthread_detach.4-1.c		   \
		    $(srcdir)/pthread_tests/pthread_detach.4-2.c		   \
		    $(srcdir)/pthread_tests/pthread_detach.4-3.c		   \
		    $(srcdir)/pthread_tests/pthread_equal.1-1.c			   \
		    $(srcdir)/pthread_tests/pthread_equal.1-2.c			   \
		    $(srcdir)/pthread_tests/pthread_equal.2-1.c			   \
		    $(srcdir)/pthread_tests/pthread_exit.1-1.c			   \
		    $(srcdir)/pthread_tests/pthread_exit.1-2.c			   \
		    $(srcdir)/pthread_tests/pthread_exit.2-1.c			   \
		    $(srcdir)/pthread_tests/pthread_exit.2-2.c			   \
		    $(srcdir)/pthread_tests/pthread_exit.3-1.c			   \
		    $(srcdir)/pthread_tests/pthread_exit.3-2.c			   \
		    $(srcdir)/pthread_tests/pthread_exit.4-1.c			   \
		    $(srcdir)/pthread_tests/pthread_exit.5-1.c			   \
		    $(srcdir)/pthread_tests/pthread_exit.6-1.c			   \
		    $(srcdir)/pthread_tests/pthread_exit.6-2.c			   \
		    $(srcdir)/pthread_tests/pthread_getcpuclockid.1-1.c		   \
		    $(srcdir)/pthread_tests/pthread_getschedparam.1-1.c		   \
		    $(srcdir)/pthread_tests/pthread_getschedparam.1-2.c		   \
		    $(srcdir)/pthread_tests/pthread_getschedparam.1-3.c		   \
		    $(srcdir)/pthread_tests/pthread_getschedparam.4-1.c		   \
		    $(srcdir)/pthread_tests/pthread_getspecific.1-1.c		   \
		    $(srcdir)/pthread_tests/pthread_getspecific.3-1.c		   \
		    $(srcdir)/pthread_tests/pthread_join.1-1.c			   \
		    $(srcdir)/pthread_tests/pthread_join.1-2.c			   \
		    $(srcdir)/pthread_tests/pthread_join.2-1.c			   \
		    $(srcdir)/pthread_tests/pthread_join.3-1.c			   \
		    $(srcdir)/pthread_tests/pthread_join.4-1.c			   \
		    $(srcdir)/pthread_tests/pthread_join.5-1.c			   \
		    $(srcdir)/pthread_tests/pthread_join.6-2.c			   \
		    $(srcdir)/pthread_tests/pthread_join.6-3.c			   \
		    $(srcdir)/pthread_tests/pthread_key_create.1-1.c		   \
		    $(srcdir)/pthread_tests/pthread_key_create.1-2.c		   \
		    $(srcdir)/pthread_tests/pthread_key_create.2-1.c		   \
		    $(srcdir)/pthread_tests/pthread_key_create.3-1.c		   \
		    $(srcdir)/pthread_tests/pthread_key_delete.1-1.c		   \
		    $(srcdir)/pthread_tests/pthread_key_delete.1-2.c		   \
		    $(srcdir)/pthread_tests/pthread_key_delete.2-1.c		   \
		    $(srcdir)/pthread_tests/pthread_kill.1-1.c			   \
		    $(srcdir)/pthread_tests/pthread_kill.1-2.c			   \
		    $(srcdir)/pthread_tests/pthread_kill.2-1.c			   \
		    $(srcdir)/pthread_tests/pthread_kill.3-1.c			   \
		    $(srcdir)/pthread_tests/pthread_kill.6-1.c			   \
		    $(srcdir)/pthread_tests/pthread_kill.7-1.c			   \
		    $(srcdir)/pthread_tests/pthread_kill.8-1.c			   \
		    $(srcdir)/pthread_tests/pthread_mutex_destroy.1-1.c		   \
		    $(srcdir)/pthread_tests/pthread_mutex_destroy.2-1.c		   \
		    $(srcdir)/pthread_tests/pthread_mutex_destroy.2-2.c		   \
		    $(srcdir)/pthread_tests/pthread_mutex_destroy.3-1.c		   \
		    $(srcdir)/pthread_tests/pthread_mutex_destroy.5-1.c		   \
		    $(srcdir)/pthread_tests/pthread_mutex_destroy.5-2.c		   \
		    $(srcdir)/pthread_tests/pthread_mutex_getprioceiling.1-1.c	   \
		    $(srcdir)/pthread_tests/pthread_mutex_init.1-1.c		   \
		    $(srcdir)/pthread_tests/pthread_mutex_init.1-2.c		   \
		    $(srcdir)/pthread_tests/pthread_mutex_init.2-1.c		   \
		    $(srcdir)/pthread_tests/pthread_mutex_init.3-1.c		   \
		    $(srcdir)/pthread_tests/pthread_mutex_init.3-2.c		   \
		    $(srcdir)/pthread_tests/pthread_mutex_init.4-1.c		   \
		    $(srcdir)/pthread_tests/pthread_mutex_init.5-1.c		   \
		    $(srcdir)/pthread_tests/pthread_mutex_init.5-3.c		   \
		    $(srcdir)/pthread_tests/pthread_mutex_lock.1-1.c		   \
		    $(srcdir)/pthread_tests/pthread_mutex_lock.2-1.c		   \
		    $(srcdir)/pthread_tests/pthread_mutex_lock.3-1.c		   \
		    $(srcdir)/pthread_tests/pthread_mutex_lock.4-1.c		   \
		    $(srcdir)/pthread_tests/pthread_mutex_lock.5-1.c		   \
		    $(srcdir)/pthread_tests/pthread_mutex_timedlock.1-1.c	   \
		    $(srcdir)/pthread_tests/pthread_mutex_timedlock.2-1.c	   \
		    $(srcdir)/pthread_tests/pthread_mutex_timedlock.4-1.c	   \
		    $(srcdir)/pthread_tests/pthread_mutex_timedlock.5-1.c	   \
		    $(srcdir)/pthread_tests/pthread_mutex_timedlock.5-2.c	   \
		    $(srcdir)/pthread_tests/pthread_mutex_timedlock.5-3.c	   \
		    $(srcdir)/pthread_tests/pthread_mutex_trylock.1-1.c		   \
		    $(srcdir)/pthread_tests/pthread_mutex_trylock.1-2.c		   \
		    $(srcdir)/pthread_tests/pthread_mutex_trylock.2-1.c		   \
		    $(srcdir)/pthread_tests/pthread_mutex_trylock.3-1.c		   \
		    $(srcdir)/pthread_tests/pthread_mutex_trylock.4-1.c		   \
		    $(srcdir)/pthread_tests/pthread_mutex_trylock.4-2.c		   \
		    $(srcdir)/pthread_tests/pthread_mutex_trylock.4-3.c		   \
		    $(srcdir)/pthread_tests/pthread_mutex_unlock.1-1.c		   \
		    $(srcdir)/pthread_tests/pthread_mutex_unlock.2-1.c		   \
		    $(srcdir)/pthread_tests/pthread_mutex_unlock.3-1.c		   \
		    $(srcdir)/pthread_tests/pthread_mutex_unlock.5-1.c		   \
		    $(srcdir)/pthread_tests/pthread_mutex_unlock.5-2.c		   \
		    $(srcdir)/pthread_tests/pthread_mutexattr_destroy.1-1.c	   \
		    $(srcdir)/pthread_tests/pthread_mutexattr_destroy.2-1.c	   \
		    $(srcdir)/pthread_tests/pthread_mutexattr_destroy.3-1.c	   \
		    $(srcdir)/pthread_tests/pthread_mutexattr_destroy.4-1.c	   \
		    $(srcdir)/pthread_tests/pthread_mutexattr_getprioceiling.1-1.c \
		    $(srcdir)/pthread_tests/pthread_mutexattr_getprioceiling.1-2.c \
		    $(srcdir)/pthread_tests/pthread_mutexattr_getprioceiling.3-1.c \
		    $(srcdir)/pthread_tests/pthread_mutexattr_getprotocol.1-1.c	   \
		    $(srcdir)/pthread_tests/pthread_mutexattr_getpshared.1-1.c	   \
		    $(srcdir)/pthread_tests/pthread_mutexattr_getpshared.1-2.c	   \
		    $(srcdir)/pthread_tests/pthread_mutexattr_getpshared.1-3.c	   \
		    $(srcdir)/pthread_tests/pthread_mutexattr_getpshared.3-1.c	   \
		    $(srcdir)/pthread_tests/pthread_mutexattr_gettype.1-1.c	   \
		    $(srcdir)/pthread_tests/pthread_mutexattr_gettype.1-2.c	   \
		    $(srcdir)/pthread_tests/pthread_mutexattr_gettype.1-3.c	   \
		    $(srcdir)/pthread_tests/pthread_mutexattr_gettype.1-4.c	   \
		    $(srcdir)/pthread_tests/pthread_mutexattr_gettype.1-5.c	   \
		    $(srcdir)/pthread_tests/pthread_mutexattr_init.1-1.c	   \
		    $(srcdir)/pthread_tests/pthread_mutexattr_init.3-1.c	   \
		    $(srcdir)/pthread_tests/pthread_mutexattr_setprioceiling.1-1.c \
		    $(srcdir)/pthread_tests/pthread_mutexattr_setprioceiling.3-1.c \
		    $(srcdir)/pthread_tests/pthread_mutexattr_setprioceiling.3-2.c \
		    $(srcdir)/pthread_tests/pthread_mutexattr_setpshared.1-1.c	   \
		    $(srcdir)/pthread_tests/pthread_mutexattr_setpshared.1-2.c	   \
		    $(srcdir)/pthread_tests/pthread_mutexattr_setpshared.2-1.c	   \
		    $(srcdir)/pthread_tests/pthread_mutexattr_setpshared.2-2.c	   \
		    $(srcdir)/pthread_tests/pthread_mutexattr_setpshared.3-1.c	   \
		    $(srcdir)/pthread_tests/pthread_mutexattr_setpshared.3-2.c	   \
		    $(srcdir)/pthread_tests/pthread_mutexattr_settype.1-1.c	   \
		    $(srcdir)/pthread_tests/pthread_mutexattr_settype.3-1.c	   \
		    $(srcdir)/pthread_tests/pthread_mutexattr_settype.3-2.c	   \
		    $(srcdir)/pthread_tests/pthread_mutexattr_settype.3-3.c	   \
		    $(srcdir)/pthread_tests/pthread_mutexattr_settype.3-4.c	   \
		    $(srcdir)/pthread_tests/pthread_mutexattr_settype.7-1.c	   \
		    $(srcdir)/pthread_tests/pthread_once.1-1.c			   \
		    $(srcdir)/pthread_tests/pthread_once.1-2.c			   \
		    $(srcdir)/pthread_tests/pthread_once.1-3.c			   \
		    $(srcdir)/pthread_tests/pthread_once.2-1.c			   \
		    $(srcdir)/pthread_tests/pthread_once.3-1.c			   \
		    $(srcdir)/pthread_tests/pthread_once.6-1.c			   \
		    $(srcdir)/pthread_tests/pthread_rwlock_destroy.1-1.c	   \
		    $(srcdir)/pthread_tests/pthread_rwlock_destroy.3-1.c	   \
		    $(srcdir)/pthread_tests/pthread_rwlock_init.1-1.c		   \
		    $(srcdir)/pthread_tests/pthread_rwlock_init.2-1.c		   \
		    $(srcdir)/pthread_tests/pthread_rwlock_init.3-1.c		   \
		    $(srcdir)/pthread_tests/pthread_rwlock_init.6-1.c		   \
		    $(srcdir)/pthread_tests/pthread_rwlock_rdlock.1-1.c		   \
		    $(srcdir)/pthread_tests/pthread_rwlock_rdlock.2-1.c		   \
		    $(srcdir)/pthread_tests/pthread_rwlock_rdlock.2-2.c		   \
		    $(srcdir)/pthread_tests/pthread_rwlock_rdlock.2-3.c		   \
		    $(srcdir)/pthread_tests/pthread_rwlock_rdlock.4-1.c		   \
		    $(srcdir)/pthread_tests/pthread_rwlock_rdlock.5-1.c		   \
		    $(srcdir)/pthread_tests/pthread_rwlock_timedrdlock.1-1.c	   \
		    $(srcdir)/pthread_tests/pthread_rwlock_timedrdlock.2-1.c	   \
		    $(srcdir)/pthread_tests/pthread_rwlock_timedrdlock.3-1.c	   \
		    $(srcdir)/pthread_tests/pthread_rwlock_timedrdlock.5-1.c	   \
		    $(srcdir)/pthread_tests/pthread_rwlock_timedrdlock.6-1.c	   \
		    $(srcdir)/pthread_tests/pthread_rwlock_timedrdlock.6-2.c	   \
		    $(srcdir)/pthread_tests/pthread_rwlock_timedwrlock.1-1.c	   \
		    $(srcdir)/pthread_tests/pthread_rwlock_timedwrlock.2-1.c	   \
		    $(srcdir)/pthread_tests/pthread_rwlock_timedwrlock.3-1.c	   \
		    $(srcdir)/pthread_tests/pthread_rwlock_timedwrlock.5-1.c	   \
		    $(srcdir)/pthread_tests/pthread_rwlock_timedwrlock.6-1.c	   \
		    $(srcdir)/pthread_tests/pthread_rwlock_timedwrlock.6-2.c	   \
		    $(srcdir)/pthread_tests/pthread_rwlock_tryrdlock.1-1.c	   \
		    $(srcdir)/pthread_tests/pthread_rwlock_trywrlock.1-1.c	   \
		    $(srcdir)/pthread_tests/pthread_rwlock_unlock.1-1.c		   \
		    $(srcdir)/pthread_tests/pthread_rwlock_unlock.2-1.c		   \
		    $(srcdir)/pthread_tests/pthread_rwlock_unlock.3-1.c		   \
		    $(srcdir)/pthread_tests/pthread_rwlock_unlock.4-1.c		   \
		    $(srcdir)/pthread_tests/pthread_rwlock_unlock.4-2.c		   \
		    $(srcdir)/pthread_tests/pthread_rwlock_wrlock.1-1.c		   \
		    $(srcdir)/pthread_tests/pthread_rwlock_wrlock.2-1.c		   \
		    $(srcdir)/pthread_tests/pthread_rwlock_wrlock.3-1.c		   \
		    $(srcdir)/pthread_tests/pthread_rwlockattr_destroy.1-1.c	   \
		    $(srcdir)/pthread_tests/pthread_rwlockattr_destroy.2-1.c	   \
		    $(srcdir)/pthread_tests/pthread_rwlockattr_getpshared.1-1.c	   \
		    $(srcdir)/pthread_tests/pthread_rwlockattr_getpshared.2-1.c	   \
		    $(srcdir)/pthread_tests/pthread_rwlockattr_getpshared.4-1.c	   \
		    $(srcdir)/pthread_tests/pthread_rwlockattr_init.1-1.c	   \
		    $(srcdir)/pthread_tests/pthread_rwlockattr_init.2-1.c	   \
		    $(srcdir)/pthread_tests/pthread_rwlockattr_setpshared.1-1.c	   \
		    $(srcdir)/pthread_tests/pthread_self.1-1.c			   \
		    $(srcdir)/pthread_tests/pthread_setcancelstate.1-1.c	   \
		    $(srcdir)/pthread_tests/pthread_setcancelstate.1-2.c	   \
		    $(srcdir)/pthread_tests/pthread_setcancelstate.2-1.c	   \
		    $(srcdir)/pthread_tests/pthread_setcancelstate.3-1.c	   \
		    $(srcdir)/pthread_tests/pthread_setcanceltype.1-1.c		   \
		    $(srcdir)/pthread_tests/pthread_setcanceltype.1-2.c		   \
		    $(srcdir)/pthread_tests/pthread_setcanceltype.2-1.c		   \
		    $(srcdir)/pthread_tests/pthread_setschedparam.1-1.c		   \
		    $(srcdir)/pthread_tests/pthread_setschedparam.1-2.c		   \
		    $(srcdir)/pthread_tests/pthread_setschedparam.4-1.c		   \
		    $(srcdir)/pthread_tests/pthread_setschedparam.5-1.c		   \
		    $(srcdir)/pthread_tests/pthread_setschedprio.1-1.c		   \
		    $(srcdir)/pthread_tests/pthread_setspecific.1-1.c		   \
		    $(srcdir)/pthread_tests/pthread_setspecific.1-2.c		   \
		    $(srcdir)/pthread_tests/pthread_sigmask.10-1.c		   \
		    $(srcdir)/pthread_tests/pthread_sigmask.12-1.c		   \
		    $(srcdir)/pthread_tests/pthread_sigmask.14-1.c		   \
		    $(srcdir)/pthread_tests/pthread_sigmask.15-1.c		   \
		    $(srcdir)/pthread_tests/pthread_sigmask.16-1.c		   \
		    $(srcdir)/pthread_tests/pthread_sigmask.18-1.c		   \
		    $(srcdir)/pthread_tests/pthread_sigmask.4-1.c		   \
		    $(srcdir)/pthread_tests/pthread_sigmask.5-1.c		   \
		    $(srcdir)/pthread_tests/pthread_sigmask.6-1.c		   \
		    $(srcdir)/pthread_tests/pthread_sigmask.7-1.c		   \
		    $(srcdir)/pthread_tests/pthread_sigmask.8-1.c		   \
		    $(srcdir)/pthread_tests/pthread_sigmask.8-2.c		   \
		    $(srcdir)/pthread_tests/pthread_sigmask.8-3.c		   \
		    $(srcdir)/pthread_tests/pthread_sigmask.9-1.c		   \
		    $(srcdir)/pthread_tests/pthread_spin_destroy.1-1.c		   \
		    $(srcdir)/pthread_tests/pthread_spin_destroy.3-1.c		   \
		    $(srcdir)/pthread_tests/pthread_spin_init.1-1.c		   \
		    $(srcdir)/pthread_tests/pthread_spin_init.2-1.c		   \
		    $(srcdir)/pthread_tests/pthread_spin_init.2-2.c		   \
		    $(srcdir)/pthread_tests/pthread_spin_init.4-1.c		   \
		    $(srcdir)/pthread_tests/pthread_spin_lock.1-1.c		   \
		    $(srcdir)/pthread_tests/pthread_spin_lock.1-2.c		   \
		    $(srcdir)/pthread_tests/pthread_spin_lock.3-1.c		   \
		    $(srcdir)/pthread_tests/pthread_spin_lock.3-2.c		   \
		    $(srcdir)/pthread_tests/pthread_spin_trylock.4-1.c		   \
		    $(srcdir)/pthread_tests/pthread_spin_unlock.1-1.c		   \
		    $(srcdir)/pthread_tests/pthread_spin_unlock.1-2.c		   \
		    $(srcdir)/pthread_tests/pthread_spin_unlock.3-1.c		   \
		    $(srcdir)/pthread_tests/pthread_testcancel.1-1.c		   \
		    $(srcdir)/pthread_tests/pthread_testcancel.2-1.c

PTHREAD_INPUTS_FAILING =                                                        \
                    $(srcdir)/pthread_tests/pthread_attr_setinheritsched.2-2.c  \
                    $(srcdir)/pthread_tests/pthread_attr_setinheritsched.2-3.c  \
                    $(srcdir)/pthread_tests/pthread_attr_setinheritsched.2-4.c  \
                    $(srcdir)/pthread_tests/pthread_attr_setschedparam.1-3.c    \
                    $(srcdir)/pthread_tests/pthread_attr_setschedparam.1-4.c    \
		    $(srcdir)/pthread_tests/pthread_attr_setscope.5-1.c         \
                    $(srcdir)/pthread_tests/pthread_barrier_wait.6-1.c          \
                    $(srcdir)/pthread_tests/pthread_cond_destroy.2-1.c          \
                    $(srcdir)/pthread_tests/pthread_cond_init.1-3.c             \
                    $(srcdir)/pthread_tests/pthread_cond_init.4-2.c             \
                    $(srcdir)/pthread_tests/pthread_cond_signal.1-2.c           \
                    $(srcdir)/pthread_tests/pthread_create.10-1.c               \
                    $(srcdir)/pthread_tests/pthread_mutexattr_getprotocol.1-2.c \
                    $(srcdir)/pthread_tests/pthread_mutexattr_setprotocol.1-1.c \
                    $(srcdir)/pthread_tests/pthread_mutexattr_setprotocol.3-1.c \
                    $(srcdir)/pthread_tests/pthread_mutexattr_setprotocol.3-2.c \
                    $(srcdir)/pthread_tests/pthread_mutexattr_settype.2-1.c     \
                    $(srcdir)/pthread_tests/pthread_once.4-1.c                  \
                    $(srcdir)/pthread_tests/pthread_spin_trylock.1-1.c

# Details about why things are listed in $(PTHREAD_INPUTS_FAILING)
#	test result is non-deterministic when run natively
#		    $(srcdir)/pthread_tests/pthread_create.10-1.c
# 	compile error in these tests with undeclared variables
#		    $(srcdir)/pthread_tests/pthread_mutexattr_getprotocol.1-2.c
#		    $(srcdir)/pthread_tests/pthread_mutexattr_setprotocol.1-1.c
#		    $(srcdir)/pthread_tests/pthread_mutexattr_setprotocol.3-1.c
#		    $(srcdir)/pthread_tests/pthread_mutexattr_setprotocol.3-2.c
# 	these tests just take a very long time
#		    $(srcdir)/pthread_tests/pthread_spin_trylock.1-1.c
#		    $(srcdir)/pthread_tests/pthread_cond_init.1-3.c
# 	these jobs doen't stop even when terminated with control c and make terminates
# 	it has to be killed by 'kill -9 '
#		    $(srcdir)/pthread_tests/pthread_mutexattr_settype.2-1.c
#		    $(srcdir)/pthread_tests/pthread_barrier_wait.6-1.c
# 	no main function: 
#		    $(srcdir)/pthread_tests/pthread_once.4-1.c
#	these tests fail when run natively, outside the simulator, within 2 minutes.
#		    $(srcdir)/pthread_tests/pthread_cond_destroy.2-1.c
#		    $(srcdir)/pthread_tests/pthread_cond_init.4-2.c
#		    $(srcdir)/pthread_tests/pthread_cond_signal.1-2.c
#       these tests require special privileges that aren't available for many users
#		    $(srcdir)/pthread_tests/pthread_attr_setinheritsched.2-2.c
#		    $(srcdir)/pthread_tests/pthread_attr_setinheritsched.2-3.c
#		    $(srcdir)/pthread_tests/pthread_attr_setinheritsched.2-4.c
#		    $(srcdir)/pthread_tests/pthread_attr_setschedparam.1-3.c
#		    $(srcdir)/pthread_tests/pthread_attr_setschedparam.1-4.c
#	these tests are hard coded to always fail
#		    $(srcdir)/pthread_tests/pthread_attr_setscope.5-1.c

PTHREAD_TEST_INCLUDES= \
                    ${srcdir}/pthread_tests/pthread_atfork.testfrmw.c \
                    ${srcdir}/pthread_tests/pthread_atfork.testfrmw.h \
                    ${srcdir}/pthread_tests/pthread_cancel.testfrmw.c \
                    ${srcdir}/pthread_tests/pthread_cancel.testfrmw.h \
                    ${srcdir}/pthread_tests/pthread_cond_broadcast.testfrmw.c \
                    ${srcdir}/pthread_tests/pthread_cond_broadcast.testfrmw.h \
                    ${srcdir}/pthread_tests/pthread_cond_destroy.testfrmw.c \
                    ${srcdir}/pthread_tests/pthread_cond_destroy.testfrmw.h \
                    ${srcdir}/pthread_tests/pthread_cond_init.testfrmw.c \
                    ${srcdir}/pthread_tests/pthread_cond_init.testfrmw.h \
                    ${srcdir}/pthread_tests/pthread_cond_signal.testfrmw.c \
                    ${srcdir}/pthread_tests/pthread_cond_signal.testfrmw.h \
                    ${srcdir}/pthread_tests/pthread_cond_timedwait.testfrmw.c \
                    ${srcdir}/pthread_tests/pthread_cond_timedwait.testfrmw.h \
                    ${srcdir}/pthread_tests/pthread_cond_wait.testfrmw.c \
                    ${srcdir}/pthread_tests/pthread_cond_wait.testfrmw.h \
                    ${srcdir}/pthread_tests/pthread_create.testfrmw.c \
                    ${srcdir}/pthread_tests/pthread_create.testfrmw.h \
                    ${srcdir}/pthread_tests/pthread_detach.testfrmw.c \
                    ${srcdir}/pthread_tests/pthread_detach.testfrmw.h \
                    ${srcdir}/pthread_tests/pthread_equal.testfrmw.c \
                    ${srcdir}/pthread_tests/pthread_equal.testfrmw.h \
                    ${srcdir}/pthread_tests/pthread_exit.testfrmw.c \
                    ${srcdir}/pthread_tests/pthread_exit.testfrmw.h \
                    ${srcdir}/pthread_tests/pthread_getschedparam.testfrmw.c \
                    ${srcdir}/pthread_tests/pthread_getschedparam.testfrmw.h \
                    ${srcdir}/pthread_tests/pthread_join.testfrmw.c \
                    ${srcdir}/pthread_tests/pthread_join.testfrmw.h \
                    ${srcdir}/pthread_tests/pthread_kill.testfrmw.c \
                    ${srcdir}/pthread_tests/pthread_kill.testfrmw.h \
                    ${srcdir}/pthread_tests/pthread_mutex_destroy.testfrmw.c \
                    ${srcdir}/pthread_tests/pthread_mutex_destroy.testfrmw.h \
                    ${srcdir}/pthread_tests/pthread_mutex_init.testfrmw.c \
                    ${srcdir}/pthread_tests/pthread_mutex_init.testfrmw.h \
                    ${srcdir}/pthread_tests/pthread_mutex_lock.testfrmw.c \
                    ${srcdir}/pthread_tests/pthread_mutex_lock.testfrmw.h \
                    ${srcdir}/pthread_tests/pthread_mutex_trylock.testfrmw.c \
                    ${srcdir}/pthread_tests/pthread_mutex_trylock.testfrmw.h \
                    ${srcdir}/pthread_tests/pthread_mutex_unlock.testfrmw.c \
                    ${srcdir}/pthread_tests/pthread_mutex_unlock.testfrmw.h \
                    ${srcdir}/pthread_tests/pthread_once.testfrmw.c \
                    ${srcdir}/pthread_tests/pthread_once.testfrmw.h \
                    ${srcdir}/pthread_tests/pthread_setschedparam.testfrmw.c \
                    ${srcdir}/pthread_tests/pthread_setschedparam.testfrmw.h \
                    ${srcdir}/pthread_tests/pthread_sigmask.testfrmw.c \
                    ${srcdir}/pthread_tests/pthread_sigmask.testfrmw.h \
                    ${srcdir}/pthread_tests/pthread_create.threads_scenarii.c \
                    ${srcdir}/pthread_tests/pthread_detach.threads_scenarii.c \
                    ${srcdir}/pthread_tests/pthread_exit.threads_scenarii.c \
                    ${srcdir}/pthread_tests/pthread_join.threads_scenarii.c \
	            ${srcdir}/pthread_tests/posixtest.h 	 

EXTRA_DIST += ${PTHREAD_TEST_INPUTS_SRC} ${PTHREAD_TEST_INCLUDES}

PTHREAD_TEST_INPUTS = $(basename $(notdir $(PTHREAD_TEST_INPUTS_SRC)))
MOSTLYCLEANFILES += $(PTHREAD_TEST_INPUTS) $(addsufix .o, $(PTHREAD_TEST_INPUTS))

$(PTHREAD_TEST_INPUTS): %: $(srcdir)/pthread_tests/%.c
	@echo "  CC32    $@"
	@$(CC) $(CFLAGS32) -g -I$(srcdir)/pthread_tests -Wall -o $@ $^ -lpthread -lrt

# Target to manually build the test inputs. (Not used internally)
.PHONY: pthread_test_inputs
pthread_test_inputs: $(PTHREAD_TEST_INPUTS)

# Liao 2/11/2011, ensure PTHREAD_TEST_TARGETS = warn_no_tests on MAC
# The assignment of PTHREAD_TEST_TARGETS must be in the same if -statement.
# Without this, the order of this statement and the following if-ENABLE_I386 may be rearranged on MAC
if ENABLE_I386
PTHREAD_TEST_TARGETS = $(addsuffix .passed, $(PTHREAD_TEST_INPUTS))
$(PTHREAD_TEST_TARGETS): %.passed: % $(srcdir)/pthread_tests.conf x86sim
	@$(RTH_RUN) INPUT=./$< $(srcdir)/pthread_tests.conf $@
else
# warn_no_tests was already defined in the "SYSTEM CALL TESTS" section of this makefile
PTHREAD_TEST_TARGETS = warn_no_tests
endif

# Cleanup targets. We can't put all the generated files in MOSTLYCLEANFILES because the list gets too long to pass
# as arguments to "rm" (deficiency in automake).  Therefore we'll create a *.clean target for each test and have
# clean-local depend on it.
PTHREAD_TEST_CLEANUPS = $(addsuffix .clean, $(PTHREAD_TEST_INPUTS))
.PHONY: $(SYS_TEST_CLEANUPS)
$(PTHREAD_TEST_CLEANUPS):
	rm -f $(foreach ext, passed failed err out, $(patsubst %.clean, %, $@).$(ext))

# Main pthread testing target, on which "make check" depends.
check-pthread:  $(PTHREAD_TEST_TARGETS)


###############################################################################################################################
# Misc targets
###############################################################################################################################

# Conveniently build all test inputs in case we want to run some manually
.PHONY: test-inputs
test-inputs: $(SYS_TEST_INPUTS) $(PTHREAD_TEST_INPUTS)

.PHONY: run-tests
run-tests: check-syscalls check-pthread check-internal test_types_1
	@$(RTH_STATS)

check-local: run-tests
#check-local:  #run-tests
#	@echo "The simulator project is excluded from make check for now due to random hanging problems."
#	@echo "Run 'make run-tests' in $$(pwd) instead."

# "tmp-sim" is the temp directory used to run the tests. See x86sim.conf.
# Rather than stuffing *everything* in MOSTLYCLEANFILES, we use cleanup targets. This is to avoid argument lists that
# are too long due.  In general, we don't want to use wild cards because some users might have some similarly named files
# that they wouldn't expect a "make clean" to delete.
clean-local: $(SYS_TEST_CLEANUPS) $(PTHREAD_TEST_CLEANUPS) $(TYPES_1_CLEANUPS)
	-chmod -f -R a+rwx tmp-sim
	rm -rf tmp-sim x-core.rose test-file-fchown32
	-chmod -f -R a+rwx fchmodattestdir* fchmodattestfile*.txt
	rm -rf             fchmodattestdir* fchmodattestfile*.txt
