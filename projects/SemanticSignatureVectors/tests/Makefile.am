# Warning: This simple makefile assumes certain values for compiler
# and switches without attempting to verify these values via
# configuration. This is a holdover from the original makefile.

inputs = test0 test1 test2

all: $(inputs)

$(inputs): %: $(srcdir)/%.c
	$(CC) -nostdlib -static -m32 -O3 -s -o $(srcdir)/$@ $<

EXTRA_DIST=test0.c test1.c test2.c

clean-local:
	rm -f $(inputs)

