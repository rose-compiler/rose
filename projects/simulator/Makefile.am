# Makefile for an x86 simulator called "sandbox". At this time the simulator
# is highly Linux-specific.  On platforms other than Linux, this simulator
# is simply a main() that prints an error message and exits.

include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

SUBDIRS = tests

include_HEADERS = mymemory.h simulate.h x86print.h

noinst_PROGRAMS = sandbox x86sim test1

sandbox_SOURCES = emulator.C simulate.C linuxcalls.C elfload.C
sandbox_CPPFLAGS = $(ROSE_INCLUDES) -UDEBUG
sandbox_LDADD = $(LIBS_WITH_RPATH) $(ROSE_LIBS)

x86sim_SOURCES = x86sim.C x86print.C
x86sim_CPPFLAGS = $(ROSE_INCLUDES) -DX86_VDSO_PATH_1='"$(srcdir)"' -DX86_VDSO_PATH_2='"$(include_dir)"'
x86sim_LDADD = $(LIBS_WITH_RPATH) $(ROSE_LIBS)

test1: $(srcdir)/test1.c
	$(CC) -m32 -o $@ $<
