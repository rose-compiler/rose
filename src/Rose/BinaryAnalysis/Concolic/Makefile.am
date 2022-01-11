include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs
AUTOMAKE_OPTIONS = nostdinc
AM_CPPFLAGS = $(ROSE_INCLUDES)

noinst_LTLIBRARIES = librose_partial.la

librose_partial_la_SOURCES =			\
    Architecture.C				\
    ConcolicExecutor.C				\
    ConcreteExecutor.C				\
    Database.C					\
    ExecutionEvent.C				\
    ExecutionLocation.C				\
    ExecutionManager.C				\
    InputVariables.C				\
    LinuxExecutor.C				\
    LinuxExitStatus.C				\
    LinuxI386.C					\
    LinuxTraceExecutor.C			\
    SharedMemory.C				\
    Specimen.C					\
    SystemCall.C				\
    TestCase.C					\
    TestSuite.C					\
    Utility.C

# Headers are installed from parent makefiles.
