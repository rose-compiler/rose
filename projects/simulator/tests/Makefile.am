# Makefile for testing an x86 simulator called "sandbox".

include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

# Test targets for individual syscalls with x86sim

SYSCALLDIR = $(srcdir)/

#Syscalls that can be compiled in a standalone fashion
SYSCALL_FILES_SA = $(wildcard $(SYSCALLDIR)syscall_sa*c)

# Syscall tests that need to link with -rt
SYSCALL_FILES_RT = $(wildcard $(SYSCALLDIR)syscall_rt*c)

# Syscall tests that needs to link with parse_opts.c, tst_res.c, tst_sig.c, tst_tmpdir.c, rmobj.c
SYSCALL_FILES_TST = $(wildcard $(SYSCALLDIR)syscall_tst*c)

SYSCALL_FILES_CHK = $(SYSCALL_FILES_SA) 

# Do we need this?
##noinst_PROGRAMS = $(notdir $(SYSCALL_FILES:%.c:%))

nullstring :=
space= $(nullstring) # end of the line
define compile
$(basename $(1)): $(addprefix $(SYSCALLDIR), $(1))
	$(CC) -m32 -o $$@ $(2) $$<  
endef

define checkFile
$(addprefix check-, $(1)): $(1)
# Ignore output of actual files. Some require non 0 exit values
	$(addprefix ./, $(1))
	../x86sim $(1)
endef


$(foreach file, $(SYSCALL_FILES_SA), $(eval $(call compile, $(notdir $(file)))))

$(foreach file, $(SYSCALL_FILES_RT), $(eval $(call compile, $(notdir $(file)), -lrt  ) ))

$(foreach file, $(SYSCALL_FILES_TST), $(eval $(call compile, $(notdir $(file)), $(srcdir)/parse_opts.c $(srcdir)/tst_res.c $(srcdir)/tst_sig.c $(srcdir)/tst_tmpdir.c $(srcdir)/rmobj.c )))

# TESTING CODE BELOW

$(foreach file, $(SYSCALL_FILES_CHK), $(eval $(call checkFile, $(basename $(notdir $(file))))))

test_file_permission: syscall_rt.18 
	mkdir -p tst-file-permission
	USER=`whoami`
	../x86sim ./syscall_rt.18 tst $(USER) $(USER) ;
	rmdir tst-file-permission

syscall_tst_fail:
	./syscall_tst.133

failing_tests: test_file_permission syscall_tst_fail

check-local: $(foreach file,$(SYSCALL_FILES_CHK), $(addprefix check-,$(basename $(notdir $(file)))))

EXTRA_DIST = compiler.h  rmobj.h  test.h  usctest.h parse_opts.c tst_res.c tst_sig.c tst_tmpdir.c rmobj.c 


clean-local:
	-rm -r syscall* tfile*
	-rmdir tst-file-permission
