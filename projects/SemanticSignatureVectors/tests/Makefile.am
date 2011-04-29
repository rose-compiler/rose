# Warning: This simple makefile assumes certain values for compiler
# and switches without attempting to verify these values via
# configuration. This is a holdover from the original makefile.

EXTRA_DIST=test0.c test1.c test2.c lib.h

if ENABLE_I386
inputs = test0 test1 test2
else
inputs =
endif

if ROSE_MPI

else
all: $(inputs)

$(inputs): %: $(srcdir)/%.c
	$(CC) -nostdlib -static -m32 -O3 -s -o $@ $<
endif

clean-local:
	rm -f $(inputs)
