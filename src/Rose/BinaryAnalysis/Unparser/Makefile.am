include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs
AUTOMAKE_OPTIONS = nostdinc
AM_CPPFLAGS = $(ROSE_INCLUDES)

noinst_LTLIBRARIES = librose_partial.la

librose_partial_la_SOURCES =			\
    EdgeArrows.C				\
    Aarch32.C					\
    Aarch64.C					\
    Base.C					\
    M68k.C					\
    Mips.C					\
    Null.C					\
    Powerpc.C					\
    X86.C

# Header files are installed from parent makefiles.
