# Makefile for testing an x86 simulator called "sandbox".

include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

# Test targets for individual syscalls with x86sim

SYSCALLDIR = $(srcdir)/
SYSCALL_FILES = $(wildcard $(SYSCALLDIR)syscall*c)

# Do we need this?
##noinst_PROGRAMS = $(notdir $(SYSCALL_FILES:%.c:%))

nullstring :=
space= $(nullstring) # end of the line
define compile
$(basename $(1)): $(addprefix $(SYSCALLDIR), $(1))
	$(CC) -m32 -o $$@ $$<
endef

define checkFile
$(addprefix check-, $(1)): $(1)
# Ignore output of actual files. Some require non 0 exit values
	-$(addprefix ./, $(1))
	../x86sim $(1)
endef


$(foreach file, $(SYSCALL_FILES), $(eval $(call compile, $(notdir $(file)))))

$(foreach file, $(SYSCALL_FILES), $(eval $(call checkFile, $(basename $(notdir $(file))))))

check-local: $(foreach file,$(SYSCALL_FILES), $(addprefix check-,$(basename $(notdir $(file)))))

clean-local:
	-rm syscall*
