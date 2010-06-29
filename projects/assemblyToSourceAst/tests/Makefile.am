# Warning: This simple makefile assumes certain values for compiler
# and switches without attempting to verify these values via
# configuration. This is a holdover from the original makefile.

inputs = test0 test1 test2
EXTRA_DIST=$(inputs)

all: $(inputs)

$(inputs): %: %.c
	$(CC) -nostdlib -static -m32 -O3 -s -o $@ $<

clean-local:
	rm -f $(inputs)
