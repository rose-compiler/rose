# Makefile for testing an x86 simulator called "sandbox".

include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

# Test targets for individual syscalls with x86sim

SYSCALLDIR = $(srcdir)/

#Syscalls that can be compiled in a standalone fashion
SYSCALL_FILES_SA =  $(wildcard $(SYSCALLDIR)syscall_sa*c)

# Syscall tests that need to link with -rt
SYSCALL_FILES_RT =  $(wildcard $(SYSCALLDIR)syscall_rt*c)

# Syscall tests that needs to link with parse_opts.c, tst_res.c, tst_sig.c, tst_tmpdir.c, rmobj.c
SYSCALL_FILES_TST = $(wildcard $(SYSCALLDIR)/syscall_tst*c) 

SYSCALL_FILES_FAIL_TST = $(wildcard $(SYSCALLDIR)/syscall_fail_tst*c) 

#Syscalls that can be compiled in a standalone fashion
SYSCALL_FILES_FAIL_SA =  $(wildcard $(SYSCALLDIR)syscall_fail_sa*c)

SYSCALL_FILES_CHK = $(SYSCALL_FILES_SA)  $(SYSCALL_FILES_TST)
SYSCALL_FILES_FAIL_CHK =  $(wildcard $(SYSCALLDIR)/syscall_fail_*c)  

# Do we need this?
##noinst_PROGRAMS = $(notdir $(SYSCALL_FILES:%.c:%))

nullstring :=
space= $(nullstring) # end of the line
define compile
$(basename $(notdir $(1))): $(addprefix $(dir $(1)), $(notdir $(1)))
	$(CC) -m32 -o $$@ $(2) -I$(SYSCALLDIR) $$<  
endef

define checkFile
$(addprefix check-, $(1)): $(1)
# Ignore output of actual files. Some require non 0 exit values
	$(addprefix ./, $(1))
	../x86sim ./$(notdir $(1))
endef

TST_FILES_LINKING= $(srcdir)/parse_opts.c \
                   $(srcdir)/tst_res.c \
                   $(srcdir)/tst_sig.c \
                   $(srcdir)/tst_tmpdir.c \
                   $(srcdir)/rmobj.c \
                   $(srcdir)/simple_test_looping.c \
                   $(srcdir)/audit_parse_verify.c \
                   $(srcdir)/audit_parse_verify_utils.c \
                   $(srcdir)/audit_utils.c

$(foreach file, $(SYSCALL_FILES_SA), $(eval $(call compile, $(file))))

$(foreach file, $(SYSCALL_FILES_RT), $(eval $(call compile, $(file), -lrt  ) ))

$(foreach file, $(SYSCALL_FILES_TST), $(eval $(call compile, $(file), $(TST_FILES_LINKING) ) -lrt ))

$(foreach file, $(SYSCALL_FILES_FAIL_TST), $(eval $(call compile, $(file), $(TST_FILES_LINKING)  ) -lrt ))

$(foreach file, $(SYSCALL_FILES_FAIL_SA), $(eval $(call compile, $(file))))

# TESTING CODE BELOW

$(foreach file, $(SYSCALL_FILES_CHK), $(eval $(call checkFile, $(basename $(notdir $(file))))))
$(foreach file, $(SYSCALL_FILES_FAIL_CHK), $(eval $(call checkFile, $(basename $(notdir $(file))))))

test_file_permission: syscall_rt.18 
	mkdir -p tst-file-permission
	echo $(SYSCALL_FILES_TST)
	USER=`whoami`
	../x86sim ./syscall_rt.18 tst $(USER) $(USER) ;
	rmdir tst-file-permission

syscall_tst_fail:
	./syscall_tst.133

failing_tests: test_file_permission syscall_tst_fail  $(foreach file,$(SYSCALL_FILES_FAIL_CHK), $(addprefix check-,$(basename $(notdir $(file)))))


check-local: $(foreach file,$(SYSCALL_FILES_CHK), $(addprefix check-,$(basename $(notdir $(file)))))
#check-local: 
#	echo "I have to temporarily turn this check off since it has been merged into master without passing tests, Liao 9/30/2010"
#	echo "Please restore the tests once the bugs are fixed. "

EXTRA_DIST = $(TST_FILES_LINKING) compiler.h  rmobj.h  test.h  usctest.h parse_opts.c tst_res.c tst_sig.c tst_tmpdir.c rmobj.c 


clean-local:
	-rm -r syscall* tfile*
	-rmdir tst-file-permission
