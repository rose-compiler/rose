# Makefile for an x86 simulator called "sandbox". At this time the simulator is highly Linux-specific.  On platforms other
# than Linux, this simulator is simply a main() that prints an error message and exits.

include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

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

lib_LIBRARIES = libipc.a
libipc_a_SOURCES = $(srcdir)/tests/libipc.c
libipc_a_CFLAGS = -m32
libipc_a_LDFLAGS = 

include_HEADERS += $(srcdir)/tests/ipcmsg.h  $(srcdir)/tests/ipcsem.h  $(srcdir)/tests/ipcshm.h

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
                   $(srcdir)/tests/cleanup.c \
                   $(srcdir)/tests/libtestsuite.c \
                   $(srcdir)/tests/tst_is_cwd.c \
                   $(srcdir)/tests/tst_kvercmp.c \
                   $(srcdir)/tests/tst_res.c \
                   $(srcdir)/tests/tst_sig.c \
                   $(srcdir)/tests/tst_tmpdir.c \
                   $(srcdir)/tests/rmobj.c \
                   $(srcdir)/tests/search_path.c \
                   $(srcdir)/tests/simple_test_looping.c \
                   $(srcdir)/tests/system_specific_process_info.c \
                   $(srcdir)/tests/audit_parse_verify.c \
                   $(srcdir)/tests/get_high_address.c \
                   $(srcdir)/tests/audit_parse_verify_utils.c \
                   $(srcdir)/tests/audit_utils.c

# TEST_INPUTS_SA_SRC = $(wildcard $(srcdir)/tests/syscall_sa*.c)
TEST_INPUTS_SA_SRC = \
$(srcdir)/tests/syscall_sa.114.c \
$(srcdir)/tests/syscall_sa.122.01.c \
$(srcdir)/tests/syscall_sa.122.02.c \
$(srcdir)/tests/syscall_sa.122.03.c \
$(srcdir)/tests/syscall_sa.13.c \
$(srcdir)/tests/syscall_sa.197.1.c \
$(srcdir)/tests/syscall_sa.197.3.c \
$(srcdir)/tests/syscall_sa.199.c \
$(srcdir)/tests/syscall_sa.200.c \
$(srcdir)/tests/syscall_sa.201.c \
$(srcdir)/tests/syscall_sa.202.c \
$(srcdir)/tests/syscall_sa.20.c \
$(srcdir)/tests/syscall_sa.224.c \
$(srcdir)/tests/syscall_sa.24.c \
$(srcdir)/tests/syscall_sa.3.c \
$(srcdir)/tests/syscall_sa.4.1.c \
$(srcdir)/tests/syscall_sa.41.c \
$(srcdir)/tests/syscall_sa.4.2.c \
$(srcdir)/tests/syscall_sa.47.c \
$(srcdir)/tests/syscall_sa.49.c \
$(srcdir)/tests/syscall_sa.50.c \
$(srcdir)/tests/syscall_sa.5.1.c \
$(srcdir)/tests/syscall_sa.5.2.c \
$(srcdir)/tests/syscall_sa.5.4.c \
$(srcdir)/tests/syscall_sa.5.6.c \
$(srcdir)/tests/syscall_sa.57.c \
$(srcdir)/tests/syscall_sa.5.9.c \
$(srcdir)/tests/syscall_sa.64.c \
$(srcdir)/tests/syscall_sa.65.c \
$(srcdir)/tests/syscall_sa.78.c \
$(srcdir)/tests/syscall_sa.83.c \
$(srcdir)/tests/syscall_sa.85.c 

#TEST_INPUTS_RT_SRC = $(wildcard $(srcdir)/tests/syscall_rt*.c)
TEST_INPUTS_RT_SRC = \
$(srcdir)/tests/syscall_rt.18.c


# TEST_INPUTS_TS_SRC = $(wildcard $(srcdir)/tests/syscall_ts*.c)
TEST_INPUTS_TS_SRC = \
$(srcdir)/tests/syscall_tst.102.1.c \
$(srcdir)/tests/syscall_tst.102.2.c \
$(srcdir)/tests/syscall_tst.102.3.c \
$(srcdir)/tests/syscall_tst.102.4.c \
$(srcdir)/tests/syscall_tst.114.1.c \
$(srcdir)/tests/syscall_tst.114.2.c \
$(srcdir)/tests/syscall_tst.116.1.c \
$(srcdir)/tests/syscall_tst.116.2.c \
$(srcdir)/tests/syscall_tst.117.c \
$(srcdir)/tests/syscall_tst.117.msgctl.01.c \
$(srcdir)/tests/syscall_tst.117.msgctl.02.c \
$(srcdir)/tests/syscall_tst.117.msgctl.03.c \
$(srcdir)/tests/syscall_tst.117.msgctl.06.c \
$(srcdir)/tests/syscall_tst.117.msgctl.07.c \
$(srcdir)/tests/syscall_tst.117.msgctl.08.c \
$(srcdir)/tests/syscall_tst.117.msgctl.09.c \
$(srcdir)/tests/syscall_tst.117.msgctl.10.c \
$(srcdir)/tests/syscall_tst.117.msgctl.11.c \
$(srcdir)/tests/syscall_tst.117.msgget.01.c \
$(srcdir)/tests/syscall_tst.117.msgget.02.c \
$(srcdir)/tests/syscall_tst.117.msgget.03.c \
$(srcdir)/tests/syscall_tst.117.msgget.04.c \
$(srcdir)/tests/syscall_tst.117.msgrcv.01.c \
$(srcdir)/tests/syscall_tst.117.msgrcv.02.c \
$(srcdir)/tests/syscall_tst.117.msgrcv.03.c \
$(srcdir)/tests/syscall_tst.117.msgrcv.04.c \
$(srcdir)/tests/syscall_tst.117.msgrcv.05.c \
$(srcdir)/tests/syscall_tst.117.msgrcv.06.c \
$(srcdir)/tests/syscall_tst.117.msgsnd.01.c \
$(srcdir)/tests/syscall_tst.117.msgsnd.02.c \
$(srcdir)/tests/syscall_tst.117.msgsnd.03.c \
$(srcdir)/tests/syscall_tst.117.msgsnd.04.c \
$(srcdir)/tests/syscall_tst.117.msgsnd.05.c \
$(srcdir)/tests/syscall_tst.117.msgsnd.06.c \
$(srcdir)/tests/syscall_tst.117.semctl.01.c \
$(srcdir)/tests/syscall_tst.117.semctl.02.c \
$(srcdir)/tests/syscall_tst.117.semctl.03.c \
$(srcdir)/tests/syscall_tst.117.semctl.04.c \
$(srcdir)/tests/syscall_tst.117.semctl.05.c \
$(srcdir)/tests/syscall_tst.117.semctl.06.c \
$(srcdir)/tests/syscall_tst.117.semget.01.c \
$(srcdir)/tests/syscall_tst.117.semget.02.c \
$(srcdir)/tests/syscall_tst.117.semget.03.c \
$(srcdir)/tests/syscall_tst.117.semop.01.c \
$(srcdir)/tests/syscall_tst.117.semop.02.c \
$(srcdir)/tests/syscall_tst.117.semop.03.c \
$(srcdir)/tests/syscall_tst.117.semop.04.c \
$(srcdir)/tests/syscall_tst.117.semop.05.c \
$(srcdir)/tests/syscall_tst.117.shmat.01.c \
$(srcdir)/tests/syscall_tst.117.shmat.02.c \
$(srcdir)/tests/syscall_tst.117.shmat.03.c \
$(srcdir)/tests/syscall_tst.117.shmctl.01.c \
$(srcdir)/tests/syscall_tst.117.shmctl.02.c \
$(srcdir)/tests/syscall_tst.117.shmctl.03.c \
$(srcdir)/tests/syscall_tst.117.shmctl.04.c \
$(srcdir)/tests/syscall_tst.117.shmdt.01.c \
$(srcdir)/tests/syscall_tst.117.shmdt.02.c \
$(srcdir)/tests/syscall_tst.117.shmget.01.c \
$(srcdir)/tests/syscall_tst.117.shmget.02.c \
$(srcdir)/tests/syscall_tst.117.shmget.03.c \
$(srcdir)/tests/syscall_tst.117.shmget.04.c \
$(srcdir)/tests/syscall_tst.117.shmget.05.c \
$(srcdir)/tests/syscall_tst.122.c \
$(srcdir)/tests/syscall_tst.125.1.c \
$(srcdir)/tests/syscall_tst.125.2.c \
$(srcdir)/tests/syscall_tst.125.3.c \
$(srcdir)/tests/syscall_tst.133.c \
$(srcdir)/tests/syscall_tst.140.1.c \
$(srcdir)/tests/syscall_tst.140.2.c \
$(srcdir)/tests/syscall_tst.141.1.c \
$(srcdir)/tests/syscall_tst.141.2.c \
$(srcdir)/tests/syscall_tst.141.3.c \
$(srcdir)/tests/syscall_tst.141.4.c \
$(srcdir)/tests/syscall_tst.142.1.c \
$(srcdir)/tests/syscall_tst.142.2.c \
$(srcdir)/tests/syscall_tst.142.3.c \
$(srcdir)/tests/syscall_tst.146.1.c \
$(srcdir)/tests/syscall_tst.146.2.c \
$(srcdir)/tests/syscall_tst.146.3.c \
$(srcdir)/tests/syscall_tst.146.4.c \
$(srcdir)/tests/syscall_tst.146.5.c \
$(srcdir)/tests/syscall_tst.146.6.c \
$(srcdir)/tests/syscall_tst.174.1.c \
$(srcdir)/tests/syscall_tst.174.2.c \
$(srcdir)/tests/syscall_tst.174.3.c \
$(srcdir)/tests/syscall_tst.175.c \
$(srcdir)/tests/syscall_tst.183.1.c \
$(srcdir)/tests/syscall_tst.183.2.c \
$(srcdir)/tests/syscall_tst.183.3.c \
$(srcdir)/tests/syscall_tst.186.1.c \
$(srcdir)/tests/syscall_tst.186.2.c \
$(srcdir)/tests/syscall_tst_18.c \
$(srcdir)/tests/syscall_tst.192.10.c \
$(srcdir)/tests/syscall_tst.192.1.c \
$(srcdir)/tests/syscall_tst.192.2.c \
$(srcdir)/tests/syscall_tst.192.3.c \
$(srcdir)/tests/syscall_tst.192.4.c \
$(srcdir)/tests/syscall_tst.192.5.c \
$(srcdir)/tests/syscall_tst.192.6.c \
$(srcdir)/tests/syscall_tst.192.7.c \
$(srcdir)/tests/syscall_tst.192.8.c \
$(srcdir)/tests/syscall_tst.192.9.c \
$(srcdir)/tests/syscall_tst.195.1.c \
$(srcdir)/tests/syscall_tst.195.2.c \
$(srcdir)/tests/syscall_tst.195.3.c \
$(srcdir)/tests/syscall_tst.195.5.c \
$(srcdir)/tests/syscall_tst.195.6.c \
$(srcdir)/tests/syscall_tst.196.1.c \
$(srcdir)/tests/syscall_tst.196.2.c \
$(srcdir)/tests/syscall_tst.196.3.c \
$(srcdir)/tests/syscall_tst.197.1.c \
$(srcdir)/tests/syscall_tst.197.2.c \
$(srcdir)/tests/syscall_tst.199.1.c \
$(srcdir)/tests/syscall_tst.199.2.c \
$(srcdir)/tests/syscall_tst.199.3.c \
$(srcdir)/tests/syscall_tst.200.1.c \
$(srcdir)/tests/syscall_tst.200.3.c \
$(srcdir)/tests/syscall_tst.201.1.c \
$(srcdir)/tests/syscall_tst.207.c \
$(srcdir)/tests/syscall_tst.212.c \
$(srcdir)/tests/syscall_tst.220.1.c \
$(srcdir)/tests/syscall_tst.220.2.c \
$(srcdir)/tests/syscall_tst.220.3.c \
$(srcdir)/tests/syscall_tst.220.4.c \
$(srcdir)/tests/syscall_tst.221.01.c \
$(srcdir)/tests/syscall_tst.221.02.c \
$(srcdir)/tests/syscall_tst.221.03.c \
$(srcdir)/tests/syscall_tst.221.04.c \
$(srcdir)/tests/syscall_tst.221.05.c \
$(srcdir)/tests/syscall_tst.221.06.c \
$(srcdir)/tests/syscall_tst.221.07.c \
$(srcdir)/tests/syscall_tst.221.08.c \
$(srcdir)/tests/syscall_tst.221.09.c \
$(srcdir)/tests/syscall_tst.221.10.c \
$(srcdir)/tests/syscall_tst.221.11.c \
$(srcdir)/tests/syscall_tst.221.12.c \
$(srcdir)/tests/syscall_tst.221.13.c \
$(srcdir)/tests/syscall_tst.221.14.c \
$(srcdir)/tests/syscall_tst.221.15.c \
$(srcdir)/tests/syscall_tst.221.16.c \
$(srcdir)/tests/syscall_tst.221.17.c \
$(srcdir)/tests/syscall_tst.221.18.c \
$(srcdir)/tests/syscall_tst.221.19.c \
$(srcdir)/tests/syscall_tst.221.20.c \
$(srcdir)/tests/syscall_tst.221.21.c \
$(srcdir)/tests/syscall_tst.221.22.c \
$(srcdir)/tests/syscall_tst.221.23.c \
$(srcdir)/tests/syscall_tst.221.24.c \
$(srcdir)/tests/syscall_tst.221.25.c \
$(srcdir)/tests/syscall_tst.221.26.c \
$(srcdir)/tests/syscall_tst.221.27.c \
$(srcdir)/tests/syscall_tst.221.28.c \
$(srcdir)/tests/syscall_tst.224.c \
$(srcdir)/tests/syscall_tst.243.1.c \
$(srcdir)/tests/syscall_tst.243.2.c \
$(srcdir)/tests/syscall_tst.252.c \
$(srcdir)/tests/syscall_tst.258.c \
$(srcdir)/tests/syscall_tst.265.c \
$(srcdir)/tests/syscall_tst.266.c \
$(srcdir)/tests/syscall_tst.306.c \
$(srcdir)/tests/syscall_tst.3.1.c \
$(srcdir)/tests/syscall_tst.3.2.c \
$(srcdir)/tests/syscall_tst.3.3.c \
$(srcdir)/tests/syscall_tst.3.4.c \
$(srcdir)/tests/syscall_tst.4.1.c \
$(srcdir)/tests/syscall_tst.4.2.c \
$(srcdir)/tests/syscall_tst.4.3.c \
$(srcdir)/tests/syscall_tst.4.4.c \
$(srcdir)/tests/syscall_tst.4.5.c \
$(srcdir)/tests/syscall_tst.13.1.c \
$(srcdir)/tests/syscall_tst.13.2.c \
$(srcdir)/tests/syscall_tst.20.1.c \
$(srcdir)/tests/syscall_tst.20.2.c \
$(srcdir)/tests/syscall_tst.30.c \
$(srcdir)/tests/syscall_tst.311.c \
$(srcdir)/tests/syscall_tst.33.1.c \
$(srcdir)/tests/syscall_tst.33.2.c \
$(srcdir)/tests/syscall_tst.54.c \
$(srcdir)/tests/syscall_tst.24.1.c \
$(srcdir)/tests/syscall_tst.24.2.c \
$(srcdir)/tests/syscall_tst.24.3.c \
$(srcdir)/tests/syscall_tst.57.1.c \
$(srcdir)/tests/syscall_tst.57.2.c \
$(srcdir)/tests/syscall_tst.57.3.c \
$(srcdir)/tests/syscall_tst.60.1.c \
$(srcdir)/tests/syscall_tst.60.2.c \
$(srcdir)/tests/syscall_tst.60.3.c \
$(srcdir)/tests/syscall_tst.64.1.c \
$(srcdir)/tests/syscall_tst.64.2.c \
$(srcdir)/tests/syscall_tst.76.1.c \
$(srcdir)/tests/syscall_tst.76.2.c \
$(srcdir)/tests/syscall_tst.78.1.c \
$(srcdir)/tests/syscall_tst.78.2.c \
$(srcdir)/tests/syscall_tst.83.1.c \
$(srcdir)/tests/syscall_tst.83.2.c \
$(srcdir)/tests/syscall_tst.83.3.c \
$(srcdir)/tests/syscall_tst.83.4.c \
$(srcdir)/tests/syscall_tst.83.5.c \
$(srcdir)/tests/syscall_tst.91.1.c \
$(srcdir)/tests/syscall_tst.91.2.c \
$(srcdir)/tests/syscall_tst.91.3.c \
$(srcdir)/tests/syscall_tst.94.1.c \
$(srcdir)/tests/syscall_tst.94.2.c \
$(srcdir)/tests/syscall_tst.95.c 

TEST_INPUTS_NM_SRC = \
$(srcdir)/tests/syscall.114.c \
$(srcdir)/tests/syscall.122.01.c \
$(srcdir)/tests/syscall.122.02.c \
$(srcdir)/tests/syscall.122.03.c \
$(srcdir)/tests/syscall.13.c \
$(srcdir)/tests/syscall.197.1.c \
$(srcdir)/tests/syscall.197.3.c \
$(srcdir)/tests/syscall.199.c \
$(srcdir)/tests/syscall.200.c \
$(srcdir)/tests/syscall.201.c \
$(srcdir)/tests/syscall.202.c \
$(srcdir)/tests/syscall.20.c \
$(srcdir)/tests/syscall.224.c \
$(srcdir)/tests/syscall.3.c \
$(srcdir)/tests/syscall.4.1.c \
$(srcdir)/tests/syscall.41.c \
$(srcdir)/tests/syscall.4.2.c \
$(srcdir)/tests/syscall.47.c \
$(srcdir)/tests/syscall.49.c \
$(srcdir)/tests/syscall.50.c \
$(srcdir)/tests/syscall.5.1.c \
$(srcdir)/tests/syscall.5.2.c \
$(srcdir)/tests/syscall.5.4.c \
$(srcdir)/tests/syscall.5.6.c \
$(srcdir)/tests/syscall.57.c \
$(srcdir)/tests/syscall.5.9.c \
$(srcdir)/tests/syscall.64.c \
$(srcdir)/tests/syscall.65.c \
$(srcdir)/tests/syscall.78.c \
$(srcdir)/tests/syscall.83.c \
$(srcdir)/tests/syscall.85.c 

TEST_INPUTS_SRC    = $(TEST_INPUTS_SA_SRC) $(TEST_INPUTS_RT_SRC) $(TEST_INPUTS_TS_SRC) $(TEST_INPUTS_NM_SRC)

TEST_INPUTS_SA = $(basename $(notdir $(TEST_INPUTS_SA_SRC)))
TEST_INPUTS_RT = $(basename $(notdir $(TEST_INPUTS_RT_SRC)))
TEST_INPUTS_TS = $(basename $(notdir $(TEST_INPUTS_TS_SRC)))
TEST_INPUTS_NM = $(basename $(notdir $(TEST_INPUTS_NM_SRC)))
TEST_INPUTS    = $(TEST_INPUTS_SA) $(TEST_INPUTS_RT) $(TEST_INPUTS_TS) $(TEST_INPUTS_NM)

MOSTLYCLEANFILES += $(TEST_INPUTS) $(addsuffix .o, $(TEST_INPUTS)) \
                    $(addsuffix .o, $(basename $(notdir $(TEST_SUPPORT_SRC))))

$(TEST_INPUTS_SA): %: $(srcdir)/tests/%.c
	@echo "  CC32    $@"
	@$(CC) -m32 -g -I$(srcdir)/tests -o $@ $^
$(TEST_INPUTS_RT): %: $(srcdir)/tests/%.c
	@echo "  CC32    $@"
	@$(CC) -m32 -g -I$(srcdir)/tests -o $@ $^ -lrt
$(TEST_INPUTS_TS): %: $(srcdir)/tests/%.c $(TEST_SUPPORT_SRC) libipc.a
	@echo "  CC32    $@"
	@$(CC) -m32 -g -I$(srcdir)/tests -o $@ $^ libipc.a -lrt 
$(TEST_INPUTS_NM): %: $(srcdir)/tests/%.c
	@echo "  CC32    $@"
	@$(CC) -m32 -g -I$(srcdir)/tests -o $@ $^ 
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
$(TEST_TARGETS): %.passed: % $(srcdir)/x86sim.conf x86sim
	@$(TH) INPUT=./$< $(srcdir)/x86sim.conf $@

check-local: $(TEST_TARGETS)

clean-local:
	rm -rf tmp-sim

EXTRA_DIST = README x86sim.conf test_status.txt  \
$(srcdir)/tests/compat_16.h \
$(srcdir)/tests/compat_uid.h \
$(srcdir)/tests/compiler.h \
$(srcdir)/tests/bintest_config.h \
$(srcdir)/tests/ipcshm.h \
$(srcdir)/tests/usctest.h \
$(srcdir)/tests/includes.h \
$(srcdir)/tests/audit_utils.h \
$(srcdir)/tests/ltp_signal.h \
$(srcdir)/tests/system_specific_process_info.h \
$(srcdir)/tests/include_j_h.h \
$(srcdir)/tests/getdents.h \
$(srcdir)/tests/audit_parse_verify.h \
$(srcdir)/tests/ipcmsg.h \
$(srcdir)/tests/search_path.h \
$(srcdir)/tests/ipcsem.h \
$(srcdir)/tests/libtestsuite.h \
$(srcdir)/tests/rmobj.h \
$(srcdir)/tests/set_thread_area.h \
$(srcdir)/tests/test.h

