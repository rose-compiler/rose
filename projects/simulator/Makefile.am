include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

noinst_PROGRAMS = sandbox test1

sandbox_SOURCES = emulator.C simulate.C linuxcalls.C elfload.C
sandbox_CPPFLAGS = $(ROSE_INCLUDES)
sandbox_LDADD = $(LIBS_WITH_RPATH) $(ROSE_LIBS)

test1: $(srcdir)/test1.c
	$(CC) -m32 -o $@ $<
