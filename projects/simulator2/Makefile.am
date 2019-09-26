# Makefile for an x86 simulator called "x86sim". At this time the simulator is highly Linux-specific.  On platforms other
# than Linux, this simulator is simply a main() that prints an error message and exits.

include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

if ROSE_BUILD_BINARY_ANALYSIS_SUPPORT

# Forward declarations for "+="
MOSTLYCLEANFILES=
EXTRA_DIST=
include_HEADERS=
noinst_HEADERS=
bin_PROGRAMS=
noinst_PROGRAMS=
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

lib_LTLIBRARIES = libRSIM.la
libRSIM_la_CPPFLAGS = $(ROSE_INCLUDES)
libRSIM_la_SOURCES =				\
	RSIM_Adapter.C				\
	RSIM_Callbacks.C			\
	RSIM_ColdFire.C				\
	RSIM_Common.C				\
	RSIM_Debugger.C				\
	RSIM_Futex.C				\
	RSIM_Linux.C				\
	RSIM_Linux32.C				\
	RSIM_Linux64.C				\
	RSIM_Pentium4.C				\
	RSIM_Printer.C				\
	RSIM_Process.C				\
	RSIM_Semantics.C			\
	RSIM_SignalHandling.C			\
	RSIM_Simulator.C			\
	RSIM_Thread.C				\
	RSIM_Tools.C
include_HEADERS +=				\
	RSIM_Adapter.h				\
	RSIM_Callbacks.h			\
	RSIM_ColdFire.h				\
	RSIM_Common.h				\
	RSIM_Debugger.h				\
	RSIM_Futex.h				\
	RSIM_Linux.h				\
	RSIM_Linux32.h				\
	RSIM_Linux64.h				\
	RSIM_Pentium4.h				\
	RSIM_Printer.h				\
	RSIM_Process.h				\
	RSIM_Semantics.h			\
	RSIM_SignalHandling.h			\
	RSIM_Simulator.h			\
	RSIM_Thread.h				\
	RSIM_Tools.h

$(libRSIM_la_SOURCES:.C=.o): %.o: %.h
$(libRSIM_la_SOURCES:.C=.lo): %.lo: %.h

###############################################################################################################################
#  PROGRAMS
###############################################################################################################################

# Do not install RSIM_Private.h -- it is only used during compilation.
EXTRA_DIST += RSIM_Private.h

# The simulator, which uses ROSE interfaces where possible.
bin_PROGRAMS += rsim-x86sim
rsim_x86sim_SOURCES = x86sim.C
rsim_x86sim_CPPFLAGS = $(ROSE_INCLUDES)
rsim_x86sim_LDFLAGS = $(ROSE_RPATHS)
rsim_x86sim_LDADD = $(BOOST_LDFLAGS) libRSIM.la $(ROSE_LIBS)

# help debug the initial stack
noinst_PROGRAMS += rsim-show-initial-stack
rsim_show_initial_stack_SOURCES = show-initial-stack.C
rsim_show_initial_stack_CPPFLAGS = $(ROSE_INCLUDES)
rsim_show_initial_stack_LDFLAGS = $(ROSE_RPATHS)
rsim_show_initial_stack_LDADD = $(ROSE_LIBS)

# demo to show how to monitor some file-related system calls
noinst_PROGRAMS += rsim-ports-opened
rsim_ports_opened_SOURCES = demos/ports_opened.C
rsim_ports_opened_CPPFLAGS = $(ROSE_INCLUDES)
rsim_ports_opened_LDFLAGS = $(ROSE_RPATHS)
rsim_ports_opened_LDADD = $(BOOST_LDFLAGS) libRSIM.la $(ROSE_LIBS)

###############################################################################################################################
#  INTERNAL SIMULATOR TESTS
###############################################################################################################################

SimulatorFlags = --arch=linux-amd64 --native-load --trace=syscall,mmap,progress

###############################################################################################################################
#  SYSTEM CALL TESTS
###############################################################################################################################

# Sources to generate the 32- or 64-bit specimen executables.  These come in three flavors depending on how they're linked:
#    SyscallTests_Standalone   -- stand-alone, no additional source required
#    SyscallTests_WithTestLib  -- use the SYS_TEST_SUPPORT library
#    SyscallTests_WithRtLib    -- require linking with librt

#-----------------------------------------------------------------------

SyscallTests_Standalone_Src =				\
	$(srcdir)/syscall_tests/syscall_sa.114.c	\
	$(srcdir)/syscall_tests/syscall_sa.122.01.c	\
	$(srcdir)/syscall_tests/syscall_sa.122.02.c	\
	$(srcdir)/syscall_tests/syscall_sa.122.03.c	\
	$(srcdir)/syscall_tests/syscall_sa.13.c		\
	$(srcdir)/syscall_tests/syscall_sa.197.1.c	\
	$(srcdir)/syscall_tests/syscall_sa.197.3.c	\
	$(srcdir)/syscall_tests/syscall_sa.20.c		\
	$(srcdir)/syscall_tests/syscall_sa.24.c		\
	$(srcdir)/syscall_tests/syscall_sa.3.c		\
	$(srcdir)/syscall_tests/syscall_sa.4.1.c	\
	$(srcdir)/syscall_tests/syscall_sa.41.c		\
	$(srcdir)/syscall_tests/syscall_sa.4.2.c	\
	$(srcdir)/syscall_tests/syscall_sa.47.c		\
	$(srcdir)/syscall_tests/syscall_sa.49.c		\
	$(srcdir)/syscall_tests/syscall_sa.50.c		\
	$(srcdir)/syscall_tests/syscall_sa.5.1.c	\
	$(srcdir)/syscall_tests/syscall_sa.5.2.c	\
	$(srcdir)/syscall_tests/syscall_sa.5.4.c	\
	$(srcdir)/syscall_tests/syscall_sa.5.6.c	\
	$(srcdir)/syscall_tests/syscall_sa.57.c		\
	$(srcdir)/syscall_tests/syscall_sa.5.9.c	\
	$(srcdir)/syscall_tests/syscall_sa.64.c		\
	$(srcdir)/syscall_tests/syscall_sa.65.c		\
	$(srcdir)/syscall_tests/syscall_sa.78.c		\
	$(srcdir)/syscall_tests/syscall_sa.83.c		\
	$(srcdir)/syscall_tests/syscall_sa.85.c 

SyscallTests_Standalone_Exes = $(notdir $(basename $(SyscallTests_Standalone_Src)))
MOSTLYCLEANFILES += $(SyscallTests_Standalone_Exes)

SyscallTests_Standalone_Tests = $(addsuffix .passed, $(SyscallTests_Standalone_Exes))
TEST_TARGETS += $(SyscallTests_Standalone_Tests)

$(SyscallTests_Standalone_Exes): %: $(srcdir)/syscall_tests/%.c
	$(CC) -Wall -g -o $@ $^

$(SyscallTests_Standalone_Tests): %.passed: % rsim-x86sim
	@$(RTH_RUN)										\
		TITLE="rsim-x86sim $< [$@]"							\
		USE_SUBDIR=yes									\
		CMD="x86_64 -R $(abspath .)/rsim-x86sim $(SimulatorFlags) $(abspath $<)"	\
		$(top_srcdir)/scripts/test_exit_status $@

.PHONY: check-syscalltests-standalone
check-syscalltests-standalone: $(SyscallTests_Standalone_Tests)

#-----------------------------------------------------------------------

# The test support library, statically compiled into each test
SyscallTests_LibTest_Src =					\
	$(srcdir)/syscall_tests/audit_parse_verify.c		\
        $(srcdir)/syscall_tests/audit_parse_verify_utils.c	\
        $(srcdir)/syscall_tests/audit_utils.c			\
        $(srcdir)/syscall_tests/cleanup.c			\
        $(srcdir)/syscall_tests/cloner.c			\
        $(srcdir)/syscall_tests/get_high_address.c		\
	$(srcdir)/syscall_tests/libipc.c			\
        $(srcdir)/syscall_tests/libtestsuite.c			\
        $(srcdir)/syscall_tests/parse_opts.c			\
        $(srcdir)/syscall_tests/rmobj.c				\
        $(srcdir)/syscall_tests/search_path.c			\
        $(srcdir)/syscall_tests/simple_test_looping.c		\
        $(srcdir)/syscall_tests/system_specific_process_info.c	\
        $(srcdir)/syscall_tests/tst_is_cwd.c			\
        $(srcdir)/syscall_tests/tst_kvercmp.c			\
        $(srcdir)/syscall_tests/tst_res.c			\
        $(srcdir)/syscall_tests/tst_sig.c			\
        $(srcdir)/syscall_tests/tst_tmpdir.c

# Each test...
SyscallTests_WithTestLib_Src =					\
	$(srcdir)/syscall_tests/syscall_tst.100.1.c		\
	$(srcdir)/syscall_tests/syscall_tst.100.2.c		\
	$(srcdir)/syscall_tests/syscall_tst.102.1.c		\
	$(srcdir)/syscall_tests/syscall_tst.102.2.c		\
	$(srcdir)/syscall_tests/syscall_tst.102.3.c		\
	$(srcdir)/syscall_tests/syscall_tst.102.4.c		\
	$(srcdir)/syscall_tests/syscall_tst.114.1.c		\
	$(srcdir)/syscall_tests/syscall_tst.114.2.c		\
	$(srcdir)/syscall_tests/syscall_tst.116.1.c		\
	$(srcdir)/syscall_tests/syscall_tst.116.2.c		\
	$(srcdir)/syscall_tests/syscall_tst.117.c		\
	$(srcdir)/syscall_tests/syscall_tst.117.msgctl.01.c	\
	$(srcdir)/syscall_tests/syscall_tst.117.msgctl.02.c	\
	$(srcdir)/syscall_tests/syscall_tst.117.msgctl.03.c	\
	$(srcdir)/syscall_tests/syscall_tst.117.msgctl.08.c	\
	$(srcdir)/syscall_tests/syscall_tst.117.msgget.01.c	\
	$(srcdir)/syscall_tests/syscall_tst.117.msgget.02.c	\
	$(srcdir)/syscall_tests/syscall_tst.117.msgrcv.01.c	\
	$(srcdir)/syscall_tests/syscall_tst.117.msgrcv.03.c	\
	$(srcdir)/syscall_tests/syscall_tst.117.msgrcv.04.c	\
	$(srcdir)/syscall_tests/syscall_tst.117.msgrcv.05.c	\
	$(srcdir)/syscall_tests/syscall_tst.117.msgrcv.06.c	\
	$(srcdir)/syscall_tests/syscall_tst.117.msgsnd.01.c	\
	$(srcdir)/syscall_tests/syscall_tst.117.msgsnd.03.c	\
	$(srcdir)/syscall_tests/syscall_tst.117.msgsnd.04.c	\
	$(srcdir)/syscall_tests/syscall_tst.117.msgsnd.05.c	\
	$(srcdir)/syscall_tests/syscall_tst.117.msgsnd.06.c	\
	$(srcdir)/syscall_tests/syscall_tst.117.semctl.01.c	\
	$(srcdir)/syscall_tests/syscall_tst.117.semctl.05.c	\
	$(srcdir)/syscall_tests/syscall_tst.117.semget.01.c	\
	$(srcdir)/syscall_tests/syscall_tst.117.semget.03.c	\
	$(srcdir)/syscall_tests/syscall_tst.117.semop.01.c	\
	$(srcdir)/syscall_tests/syscall_tst.117.semop.03.c	\
	$(srcdir)/syscall_tests/syscall_tst.117.semop.04.c	\
	$(srcdir)/syscall_tests/syscall_tst.117.semop.05.c	\
	$(srcdir)/syscall_tests/syscall_tst.117.shmat.01.c	\
	$(srcdir)/syscall_tests/syscall_tst.117.shmctl.01.c	\
	$(srcdir)/syscall_tests/syscall_tst.117.shmctl.04.c	\
	$(srcdir)/syscall_tests/syscall_tst.117.shmdt.01.c	\
	$(srcdir)/syscall_tests/syscall_tst.117.shmdt.02.c	\
	$(srcdir)/syscall_tests/syscall_tst.117.shmget.01.c	\
	$(srcdir)/syscall_tests/syscall_tst.117.shmget.02.c	\
	$(srcdir)/syscall_tests/syscall_tst.118.1.c		\
	$(srcdir)/syscall_tests/syscall_tst.118.3.c		\
	$(srcdir)/syscall_tests/syscall_tst.122.c		\
	$(srcdir)/syscall_tests/syscall_tst.125.1.c		\
	$(srcdir)/syscall_tests/syscall_tst.125.2.c		\
	$(srcdir)/syscall_tests/syscall_tst.125.3.c		\
	$(srcdir)/syscall_tests/syscall_tst.133.c		\
	$(srcdir)/syscall_tests/syscall_tst.140.2.c		\
	$(srcdir)/syscall_tests/syscall_tst.141.1.c		\
	$(srcdir)/syscall_tests/syscall_tst.141.2.c		\
	$(srcdir)/syscall_tests/syscall_tst.141.3.c		\
	$(srcdir)/syscall_tests/syscall_tst.141.4.c		\
	$(srcdir)/syscall_tests/syscall_tst.142.1.c		\
	$(srcdir)/syscall_tests/syscall_tst.142.2.c		\
	$(srcdir)/syscall_tests/syscall_tst.142.3.c		\
	$(srcdir)/syscall_tests/syscall_tst.144.1.c		\
	$(srcdir)/syscall_tests/syscall_tst.144.2.c		\
	$(srcdir)/syscall_tests/syscall_tst.144.3.c		\
	$(srcdir)/syscall_tests/syscall_tst.144.4.c		\
	$(srcdir)/syscall_tests/syscall_tst.144.5.c		\
	$(srcdir)/syscall_tests/syscall_tst.146.1.c		\
	$(srcdir)/syscall_tests/syscall_tst.146.2.c		\
	$(srcdir)/syscall_tests/syscall_tst.146.3.c		\
	$(srcdir)/syscall_tests/syscall_tst.146.4.c		\
	$(srcdir)/syscall_tests/syscall_tst.146.5.c		\
	$(srcdir)/syscall_tests/syscall_tst.146.6.c		\
	$(srcdir)/syscall_tests/syscall_tst.162.1.c		\
	$(srcdir)/syscall_tests/syscall_tst.162.2.c		\
	$(srcdir)/syscall_tests/syscall_tst.162.3.c		\
	$(srcdir)/syscall_tests/syscall_tst.162.4.c		\
	$(srcdir)/syscall_tests/syscall_tst.174.1.c		\
	$(srcdir)/syscall_tests/syscall_tst.174.2.c		\
	$(srcdir)/syscall_tests/syscall_tst.174.3.c		\
	$(srcdir)/syscall_tests/syscall_tst.175.c		\
	$(srcdir)/syscall_tests/syscall_tst.176.1.c		\
	$(srcdir)/syscall_tests/syscall_tst.179.1.c		\
	$(srcdir)/syscall_tests/syscall_tst.183.1.c		\
	$(srcdir)/syscall_tests/syscall_tst.183.2.c		\
	$(srcdir)/syscall_tests/syscall_tst.186.1.c		\
	$(srcdir)/syscall_tests/syscall_tst.186.2.c		\
	$(srcdir)/syscall_tests/syscall_tst_18.c		\
	$(srcdir)/syscall_tests/syscall_tst.192.10.c		\
	$(srcdir)/syscall_tests/syscall_tst.192.1.c		\
	$(srcdir)/syscall_tests/syscall_tst.192.2.c		\
	$(srcdir)/syscall_tests/syscall_tst.192.3.c		\
	$(srcdir)/syscall_tests/syscall_tst.192.4.c		\
	$(srcdir)/syscall_tests/syscall_tst.192.5.c		\
	$(srcdir)/syscall_tests/syscall_tst.192.6.c		\
	$(srcdir)/syscall_tests/syscall_tst.192.7.c		\
	$(srcdir)/syscall_tests/syscall_tst.192.8.c		\
	$(srcdir)/syscall_tests/syscall_tst.192.9.c		\
	$(srcdir)/syscall_tests/syscall_tst.195.5.c		\
	$(srcdir)/syscall_tests/syscall_tst.195.6.c		\
	$(srcdir)/syscall_tests/syscall_tst.196.1.c		\
	$(srcdir)/syscall_tests/syscall_tst.197.1.c		\
	$(srcdir)/syscall_tests/syscall_tst.197.2.c		\
	$(srcdir)/syscall_tests/syscall_tst.199.1.c		\
	$(srcdir)/syscall_tests/syscall_tst.199.2.c		\
	$(srcdir)/syscall_tests/syscall_tst.199.3.c		\
	$(srcdir)/syscall_tests/syscall_tst.200.1.c		\
	$(srcdir)/syscall_tests/syscall_tst.200.3.c		\
	$(srcdir)/syscall_tests/syscall_tst.201.1.c		\
	$(srcdir)/syscall_tests/syscall_tst.205.1.c		\
	$(srcdir)/syscall_tests/syscall_tst.205.2.c		\
	$(srcdir)/syscall_tests/syscall_tst.205.4.c		\
	$(srcdir)/syscall_tests/syscall_tst.207.c		\
	$(srcdir)/syscall_tests/syscall_tst.212.c		\
	$(srcdir)/syscall_tests/syscall_tst.220.1.c		\
	$(srcdir)/syscall_tests/syscall_tst.220.2.c		\
	$(srcdir)/syscall_tests/syscall_tst.220.3.c		\
	$(srcdir)/syscall_tests/syscall_tst.220.4.c		\
	$(srcdir)/syscall_tests/syscall_tst.221.01.c		\
	$(srcdir)/syscall_tests/syscall_tst.221.02.c		\
	$(srcdir)/syscall_tests/syscall_tst.221.03.c		\
	$(srcdir)/syscall_tests/syscall_tst.221.04.c		\
	$(srcdir)/syscall_tests/syscall_tst.221.05.c		\
	$(srcdir)/syscall_tests/syscall_tst.221.06.c		\
	$(srcdir)/syscall_tests/syscall_tst.221.07.c		\
	$(srcdir)/syscall_tests/syscall_tst.221.08.c		\
	$(srcdir)/syscall_tests/syscall_tst.221.09.c		\
	$(srcdir)/syscall_tests/syscall_tst.221.10.c		\
	$(srcdir)/syscall_tests/syscall_tst.221.11.c		\
	$(srcdir)/syscall_tests/syscall_tst.221.12.c		\
	$(srcdir)/syscall_tests/syscall_tst.221.13.c		\
	$(srcdir)/syscall_tests/syscall_tst.221.16.c		\
	$(srcdir)/syscall_tests/syscall_tst.221.17.c		\
	$(srcdir)/syscall_tests/syscall_tst.221.19.c		\
	$(srcdir)/syscall_tests/syscall_tst.221.20.c		\
	$(srcdir)/syscall_tests/syscall_tst.221.21.c		\
	$(srcdir)/syscall_tests/syscall_tst.221.22.c		\
	$(srcdir)/syscall_tests/syscall_tst.221.23.c		\
	$(srcdir)/syscall_tests/syscall_tst.221.24.c		\
	$(srcdir)/syscall_tests/syscall_tst.221.25.c		\
	$(srcdir)/syscall_tests/syscall_tst.221.26.c		\
	$(srcdir)/syscall_tests/syscall_tst.221.27.c		\
	$(srcdir)/syscall_tests/syscall_tst.221.28.c		\
	$(srcdir)/syscall_tests/syscall_tst.224.c		\
	$(srcdir)/syscall_tests/syscall_tst.243.1.c		\
	$(srcdir)/syscall_tests/syscall_tst.243.2.c		\
	$(srcdir)/syscall_tests/syscall_tst.252.c		\
	$(srcdir)/syscall_tests/syscall_tst.258.c		\
	$(srcdir)/syscall_tests/syscall_tst.265.c		\
	$(srcdir)/syscall_tests/syscall_tst.266.c		\
	$(srcdir)/syscall_tests/syscall_tst.306.c		\
	$(srcdir)/syscall_tests/syscall_tst.1.1.c		\
	$(srcdir)/syscall_tests/syscall_tst.1.2.c		\
	$(srcdir)/syscall_tests/syscall_tst.3.1.c		\
	$(srcdir)/syscall_tests/syscall_tst.3.2.c		\
	$(srcdir)/syscall_tests/syscall_tst.3.3.c		\
	$(srcdir)/syscall_tests/syscall_tst.3.4.c		\
	$(srcdir)/syscall_tests/syscall_tst.4.1.c		\
	$(srcdir)/syscall_tests/syscall_tst.4.2.c		\
	$(srcdir)/syscall_tests/syscall_tst.4.3.c		\
	$(srcdir)/syscall_tests/syscall_tst.4.4.c		\
	$(srcdir)/syscall_tests/syscall_tst.4.5.c		\
	$(srcdir)/syscall_tests/syscall_tst.9.1.c		\
	$(srcdir)/syscall_tests/syscall_tst.9.2.c		\
	$(srcdir)/syscall_tests/syscall_tst.9.3.c		\
	$(srcdir)/syscall_tests/syscall_tst.9.4.c		\
	$(srcdir)/syscall_tests/syscall_tst.13.1.c		\
	$(srcdir)/syscall_tests/syscall_tst.13.2.c		\
	$(srcdir)/syscall_tests/syscall_tst.19.1.c		\
	$(srcdir)/syscall_tests/syscall_tst.19.2.c		\
	$(srcdir)/syscall_tests/syscall_tst.19.3.c		\
	$(srcdir)/syscall_tests/syscall_tst.19.4.c		\
	$(srcdir)/syscall_tests/syscall_tst.19.5.c		\
	$(srcdir)/syscall_tests/syscall_tst.19.6.c		\
	$(srcdir)/syscall_tests/syscall_tst.19.7.c		\
	$(srcdir)/syscall_tests/syscall_tst.19.8.c		\
	$(srcdir)/syscall_tests/syscall_tst.19.9.c		\
	$(srcdir)/syscall_tests/syscall_tst.19.10.c		\
	$(srcdir)/syscall_tests/syscall_tst.20.1.c		\
	$(srcdir)/syscall_tests/syscall_tst.20.2.c		\
	$(srcdir)/syscall_tests/syscall_tst.29.1.c		\
	$(srcdir)/syscall_tests/syscall_tst.29.2.c		\
	$(srcdir)/syscall_tests/syscall_tst.29.3.c		\
	$(srcdir)/syscall_tests/syscall_tst.30.c		\
	$(srcdir)/syscall_tests/syscall_tst.311.c		\
	$(srcdir)/syscall_tests/syscall_tst.33.1.c		\
	$(srcdir)/syscall_tests/syscall_tst.33.2.c		\
	$(srcdir)/syscall_tests/syscall_tst.36.1.c		\
	$(srcdir)/syscall_tests/syscall_tst.36.2.c		\
	$(srcdir)/syscall_tests/syscall_tst.38.1.c		\
	$(srcdir)/syscall_tests/syscall_tst.38.2.c		\
	$(srcdir)/syscall_tests/syscall_tst.38.3.c		\
	$(srcdir)/syscall_tests/syscall_tst.38.4.c		\
	$(srcdir)/syscall_tests/syscall_tst.38.5.c		\
	$(srcdir)/syscall_tests/syscall_tst.38.6.c		\
	$(srcdir)/syscall_tests/syscall_tst.38.7.c		\
	$(srcdir)/syscall_tests/syscall_tst.38.8.c		\
	$(srcdir)/syscall_tests/syscall_tst.38.10.c		\
	$(srcdir)/syscall_tests/syscall_tst.38.13.c		\
	$(srcdir)/syscall_tests/syscall_tst.38.14.c		\
	$(srcdir)/syscall_tests/syscall_tst.24.1.c		\
	$(srcdir)/syscall_tests/syscall_tst.24.2.c		\
	$(srcdir)/syscall_tests/syscall_tst.24.3.c		\
	$(srcdir)/syscall_tests/syscall_tst.57.1.c		\
	$(srcdir)/syscall_tests/syscall_tst.57.2.c		\
	$(srcdir)/syscall_tests/syscall_tst.57.3.c		\
	$(srcdir)/syscall_tests/syscall_tst.60.1.c		\
	$(srcdir)/syscall_tests/syscall_tst.60.2.c		\
	$(srcdir)/syscall_tests/syscall_tst.60.3.c		\
	$(srcdir)/syscall_tests/syscall_tst.63.1.c		\
	$(srcdir)/syscall_tests/syscall_tst.63.2.c		\
	$(srcdir)/syscall_tests/syscall_tst.63.3.c		\
	$(srcdir)/syscall_tests/syscall_tst.63.4.c		\
	$(srcdir)/syscall_tests/syscall_tst.63.5.c		\
	$(srcdir)/syscall_tests/syscall_tst.64.1.c		\
	$(srcdir)/syscall_tests/syscall_tst.64.2.c		\
	$(srcdir)/syscall_tests/syscall_tst.76.1.c		\
	$(srcdir)/syscall_tests/syscall_tst.76.2.c		\
	$(srcdir)/syscall_tests/syscall_tst.78.1.c		\
	$(srcdir)/syscall_tests/syscall_tst.83.1.c		\
	$(srcdir)/syscall_tests/syscall_tst.83.2.c		\
	$(srcdir)/syscall_tests/syscall_tst.83.4.c		\
	$(srcdir)/syscall_tests/syscall_tst.83.5.c		\
	$(srcdir)/syscall_tests/syscall_tst.91.1.c		\
	$(srcdir)/syscall_tests/syscall_tst.91.2.c		\
	$(srcdir)/syscall_tests/syscall_tst.91.3.c		\
	$(srcdir)/syscall_tests/syscall_tst.93.1.c		\
	$(srcdir)/syscall_tests/syscall_tst.93.2.c		\
	$(srcdir)/syscall_tests/syscall_tst.93.3.c		\
	$(srcdir)/syscall_tests/syscall_tst.93.4.c		\
	$(srcdir)/syscall_tests/syscall_tst.94.1.c		\
	$(srcdir)/syscall_tests/syscall_tst.94.2.c		\
	$(srcdir)/syscall_tests/syscall_tst.99.1.c		\
	$(srcdir)/syscall_tests/syscall_tst.99.2.c

SyscallTests_WithTestLib_Exes = $(notdir $(basename $(SyscallTests_WithTestLib_Src)))
MOSTLYCLEANFILES += $(SyscallTests_WithTestLib_Exes)

SyscallTests_WithTestLib_Tests = $(addsuffix .passed, $(SyscallTests_WithTestLib_Exes))
TEST_TARGETS += $(SyscallTests_WithTestLib_Tests)

$(SyscallTests_WithTestLib_Exes): %: $(srcdir)/syscall_tests/%.c $(SyscallTests_LibTest_Src)
	$(CC) -Wall -g -I $(srcdir)/syscall_tests -o $@ $^

$(SyscallTests_WithTestLib_Tests): %.passed: % rsim-x86sim
	@$(RTH_RUN)										\
		TITLE="rsim-x86sim $< [$@]"							\
		USE_SUBDIR=yes									\
		CMD="x86_64 -R $(abspath .)/rsim-x86sim $(SimulatorFlags) $(abspath $<)"	\
		$(top_srcdir)/scripts/test_exit_status $@

.PHONY: check-syscalltests-withtestlib
check-syscalltests-withtestlib: $(SyscallTests_WithTestLib_Tests)

#-----------------------------------------------------------------------














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
		   $(srcdir)/syscall_tests/usctest.h		\
		   $(srcdir)/syscall_tests/syscall_tst.120.1.c	\
		   $(srcdir)/syscall_tests/syscall_tst.120.2.c	\
		   $(srcdir)/syscall_tests/syscall_tst.120.3.c	\
		   $(srcdir)/syscall_tests/syscall_tst.120.4.c	\
		   $(srcdir)/syscall_tests/syscall_tst.120.5.c	\
		   $(srcdir)/syscall_tests/syscall_tst.120.6.c	\
		   $(srcdir)/syscall_tests/syscall_tst.120.7.c	\
		   $(srcdir)/syscall_tests/syscall_tst.183.3.c	\
		   $(srcdir)/syscall_tests/syscall_tst.54.c	\
		   $(srcdir)/syscall_tests/syscall_tst.65.c



SYS_TEST_INPUTS_RT_SRC = \
                   $(srcdir)/syscall_tests/syscall_rt.18.c


# SYS_TEST_INPUTS_TS_SRC = $(wildcard $(srcdir)/syscall_tests/syscall_ts*.c)
SYS_TEST_INPUTS_TS_SRC =

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
#			for an example: http://www.gossamer-threads.com/lists/linux/kernel/813344.
#			This test can also fail if run while a leap-second is introduced since POSIX time goes backward
#			near the leap second (slim chance we're actually testing right then!)
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
noinst_HEADERS += $(srcdir)/syscall_tests/ipcmsg.h  $(srcdir)/syscall_tests/ipcsem.h  $(srcdir)/syscall_tests/ipcshm.h
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
$(SYS_TEST_TARGETS): %.passed: % $(srcdir)/syscall_tests.conf rsim-x86sim
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

PTHREAD_TEST_INCLUDES=							\
	$(srcdir)/pthread_tests/posixtest.h 	 			\
        $(srcdir)/pthread_tests/pthread_atfork.testfrmw.c		\
        $(srcdir)/pthread_tests/pthread_atfork.testfrmw.h		\
        $(srcdir)/pthread_tests/pthread_cancel.testfrmw.c		\
        $(srcdir)/pthread_tests/pthread_cancel.testfrmw.h		\
        $(srcdir)/pthread_tests/pthread_cond_broadcast.testfrmw.c	\
        $(srcdir)/pthread_tests/pthread_cond_broadcast.testfrmw.h	\
        $(srcdir)/pthread_tests/pthread_cond_destroy.testfrmw.c		\
        $(srcdir)/pthread_tests/pthread_cond_destroy.testfrmw.h		\
        $(srcdir)/pthread_tests/pthread_cond_init.testfrmw.c		\
        $(srcdir)/pthread_tests/pthread_cond_init.testfrmw.h		\
        $(srcdir)/pthread_tests/pthread_cond_signal.testfrmw.c		\
        $(srcdir)/pthread_tests/pthread_cond_signal.testfrmw.h		\
        $(srcdir)/pthread_tests/pthread_cond_timedwait.testfrmw.c	\
        $(srcdir)/pthread_tests/pthread_cond_timedwait.testfrmw.h	\
        $(srcdir)/pthread_tests/pthread_cond_wait.testfrmw.c		\
        $(srcdir)/pthread_tests/pthread_cond_wait.testfrmw.h		\
        $(srcdir)/pthread_tests/pthread_create.testfrmw.c		\
        $(srcdir)/pthread_tests/pthread_create.testfrmw.h		\
        $(srcdir)/pthread_tests/pthread_create.threads_scenarii.c	\
        $(srcdir)/pthread_tests/pthread_detach.testfrmw.c		\
        $(srcdir)/pthread_tests/pthread_detach.testfrmw.h		\
        $(srcdir)/pthread_tests/pthread_detach.threads_scenarii.c	\
        $(srcdir)/pthread_tests/pthread_equal.testfrmw.c		\
        $(srcdir)/pthread_tests/pthread_equal.testfrmw.h		\
        $(srcdir)/pthread_tests/pthread_exit.testfrmw.c			\
        $(srcdir)/pthread_tests/pthread_exit.testfrmw.h			\
        $(srcdir)/pthread_tests/pthread_exit.threads_scenarii.c		\
        $(srcdir)/pthread_tests/pthread_getschedparam.testfrmw.c	\
        $(srcdir)/pthread_tests/pthread_getschedparam.testfrmw.h	\
        $(srcdir)/pthread_tests/pthread_join.testfrmw.c			\
        $(srcdir)/pthread_tests/pthread_join.testfrmw.h			\
        $(srcdir)/pthread_tests/pthread_join.threads_scenarii.c		\
        $(srcdir)/pthread_tests/pthread_kill.testfrmw.c			\
        $(srcdir)/pthread_tests/pthread_kill.testfrmw.h			\
        $(srcdir)/pthread_tests/pthread_mutex_destroy.testfrmw.c	\
        $(srcdir)/pthread_tests/pthread_mutex_destroy.testfrmw.h	\
        $(srcdir)/pthread_tests/pthread_mutex_init.testfrmw.c		\
        $(srcdir)/pthread_tests/pthread_mutex_init.testfrmw.h		\
        $(srcdir)/pthread_tests/pthread_mutex_lock.testfrmw.c		\
        $(srcdir)/pthread_tests/pthread_mutex_lock.testfrmw.h		\
        $(srcdir)/pthread_tests/pthread_mutex_trylock.testfrmw.c	\
        $(srcdir)/pthread_tests/pthread_mutex_trylock.testfrmw.h	\
        $(srcdir)/pthread_tests/pthread_mutex_unlock.testfrmw.c		\
        $(srcdir)/pthread_tests/pthread_mutex_unlock.testfrmw.h		\
        $(srcdir)/pthread_tests/pthread_once.testfrmw.c			\
        $(srcdir)/pthread_tests/pthread_once.testfrmw.h			\
        $(srcdir)/pthread_tests/pthread_setschedparam.testfrmw.c	\
        $(srcdir)/pthread_tests/pthread_setschedparam.testfrmw.h	\
        $(srcdir)/pthread_tests/pthread_sigmask.testfrmw.c		\
        $(srcdir)/pthread_tests/pthread_sigmask.testfrmw.h		\
	$(srcdir)/pthread_tests/speculative.3-1.c			\
	$(srcdir)/pthread_tests/speculative.3-2.c			\
	$(srcdir)/pthread_tests/speculative.4-1.c			\
	$(srcdir)/pthread_tests/speculative.4-2.c			\
	$(srcdir)/pthread_tests/speculative.5-1.c			\
	$(srcdir)/pthread_tests/speculative.5-2.c			\
	$(srcdir)/pthread_tests/speculative.6-1.c

EXTRA_DIST += $(PTHREAD_TEST_INPUTS_SRC) $(PTHREAD_TEST_INCLUDES)

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
$(PTHREAD_TEST_TARGETS): %.passed: % $(srcdir)/pthread_tests.conf rsim-x86sim
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
run-tests: check-syscalls
	@$(RTH_STATS)

# check-pthread commented out because the simulator is not designed to work with multi-threaded specimens.
#run-tests: check-syscalls check-pthread
#	@$(RTH_STATS)

check-local: run-tests
#check-local:  #run-tests
#	@echo "The simulator project is excluded from make check for now due to random hanging problems."
#	@echo "Run 'make run-tests' in $$(pwd) instead."

# "tmp-sim" is the temp directory used to run the tests. See x86sim.conf.
# Rather than stuffing *everything* in MOSTLYCLEANFILES, we use cleanup targets. This is to avoid argument lists that
# are too long due.  In general, we don't want to use wild cards because some users might have some similarly named files
# that they wouldn't expect a "make clean" to delete.
clean-local: $(SYS_TEST_CLEANUPS) $(PTHREAD_TEST_CLEANUPS) $(TYPES_1_CLEANUPS)
	rm -rf $(TEST_TARGETS) $(TEST_TARGETS:.passed=.failed)
	-chmod -f -R a+rwx tmp-sim
	rm -rf tmp-sim x-core.rose test-file-fchown32
	-chmod -f -R a+rwx fchmodattestdir* fchmodattestfile*.txt
	rm -rf             fchmodattestdir* fchmodattestfile*.txt

###############################################################################################################################
# These things are reported by the policy checkers as being unused. They should be investigated and them removed. Some of them
# likely don't work anymore anyway since we never compile them.
###############################################################################################################################

old_stuff_to_remove =				\
	local_tests/client_transactions.c	\
	local_tests/testFutexInput.c		\
	local_tests/types_extern.c		\
	papers/concrete_1.C			\
	papers/concrete_2.C			\
	papers/glibc_strcmp.c			\
	papers/grep_sample_1.c			\
	papers/gzip_sample_1.c			\
	papers/gzip_sample_2.c			\
	papers/ptrace_jmp.C			\
	papers/symbolic_1.C			\
	papers/symbolic_2.C			\
	papers/types_1.C			\
	syscall_tests/syscall_sa.199.c		\
	syscall_tests/syscall_sa.200.c		\
	syscall_tests/syscall_sa.201.c		\
	syscall_tests/syscall_sa.202.c		\
	syscall_tests/syscall_sa.224.c		\
	testFutex.C				\
	x86dis.C				\
	x86futex.C				\
	x86robb.C				\
	x86win.C

endif
