# Makefile for an x86 simulator called "sandbox". At this time the simulator is highly Linux-specific.  On platforms other
# than Linux, this simulator is simply a main() that prints an error message and exits.

include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

# Forward declarations for "+="
MOSTLYCLEANFILES=
EXTRA_DIST=
include_HEADERS=
noinst_PROGRAMS=
lib_LIBRARIES=

###############################################################################################################################
#  DOCUMENTATION, CONFIGURATION, ETC
###############################################################################################################################

EXTRA_DIST += README x86sim.conf test_status.txt

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
		   $(srcdir)/syscall_tests/syscall_tst.118.2.c \
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
		   $(srcdir)/syscall_tests/syscall_tst.183.3.c \
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
		   $(srcdir)/syscall_tests/syscall_tst.54.c \
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

# These cannot be run reliably (at least not concurrently with other tests.
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

# The test inputs
$(SYS_TEST_INPUTS_SA): %: $(srcdir)/syscall_tests/%.c
	@echo "  CC32    $@"
	@$(CC) -m32 -g -I$(srcdir)/syscall_tests -Wall -o $@ $^
$(SYS_TEST_INPUTS_RT): %: $(srcdir)/syscall_tests/%.c
	@echo "  CC32    $@"
	@$(CC) -m32 -g -I$(srcdir)/syscall_tests -Wall -o $@ $^ -lrt
$(SYS_TEST_INPUTS_TS): %: $(srcdir)/syscall_tests/%.c libtestsupport.a libipc.a
	@echo "  CC32    $@"
	@$(CC) -m32 -g -I$(srcdir)/syscall_tests -Wall -o $@ $^ -lrt 
$(SYS_TEST_INPUTS_NM): %: $(srcdir)/syscall_tests/%.c
	@echo "  CC32    $@"
	@$(CC) -m32 -g -I$(srcdir)/syscall_tests -Wall -o $@ $^ 
# Target to manually build the test inputs. (Not used internally)
.PHONY: test_inputs
test_inputs: $(SYS_TEST_INPUTS)


# The test support library
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


###############################################################################################################################
#  TESTS
###############################################################################################################################

# Test targets are test names suffixed with ".passed" (but when a test fails it produces ".failed" instead).
# Each target runs "x86sim" on one of the SYS_TEST_INPUT files. Exit status determines success or failure.
SYS_TEST_TARGETS = $(addsuffix .passed, $(SYS_TEST_INPUTS))
MOSTLYCLEANFILES += $(SYS_TEST_TARGETS) $(patsubst %.passed, %.failed, $(SYS_TEST_TARGETS))
$(SYS_TEST_TARGETS): %.passed: % $(srcdir)/x86sim.conf x86sim
	@$(RTH_RUN) INPUT=./$< $(srcdir)/x86sim.conf $@

check-syscalls:  $(SYS_TEST_TARGETS)
	@$(RTH_STATS)


check-local:check-syscalls 

# "tmp-sim" is the temp directory used to run the tests. See x86sim.conf.
clean-local:
	if [ -d tmp-sim ]; then chmod -f a+rwx -R tmp-sim; fi
	rm -rf tmp-sim x-core.rose test-file-fchown32
