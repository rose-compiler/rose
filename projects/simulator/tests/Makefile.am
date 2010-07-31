# Makefile for testing an x86 simulator called "sandbox".

include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

# Test targets for individual syscalls with x86sim

SYSCALLDIR = $(srcdir)/
SYSCALL_FILES = $(wildcard $(SYSCALLDIR)syscall*c))

##noinst_PROGRAMS = $(notdir $(SYSCALL_FILES:%.c:%))

nullstring :=
space= $(nullstring) # end of the line
define compileSyscall
$(basename $(1)): $(subst $(space),, $(SYSCALLDIR)$(1))
	$(CC) -m32 -o $$@ $$<
endef   

$(foreach file, $(SYSCALL_FILES), $(eval $(call compileSyscall, $(notdir $(file)))))

clean-local:
	-rm syscall*
