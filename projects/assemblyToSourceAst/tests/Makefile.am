#if ROSE_MPI

#else

CFLAGS = -nostdlib -static -m32 -O3 -s

# DQ (3/14/2009): Don't overwrite existing values in LDFLAGS.
# LDFLAGS = -nostdlib -static -m32 -s
LDFLAGS += -nostdlib -static -m32 -s

CCAS = $(CC)
CCASFLAGS = $(CFLAGS)



if ROSE_USE_ASSEMBLY_SEMANTICS

noinst_PROGRAMS = test0 test1 test2

test0_SOURCES = test0.c
test1_SOURCES = test1.c lib.h
test2_SOURCES = test2.c lib.h

endif

#endif
