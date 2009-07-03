include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs


#TODO 2 djh: can't parallelize subdirs.  It's not actually clear to me why
#			there's even a problem, but presumably LOCAL_SUBTREE isn't
#			completely done before check-am
#TODO 3 djh: replace uses of `find` with something more portable
#TODO 2 djh:	using XFAIL is bad -- for those, expect a particular exit code 
#				(no	rtsi violation found), and not merely a failure
#				make sure segfaults are always considered failures
#TODO 2 djh: doesn't work for multifiles yet (but can just grep supportfiles & cp them)

builddir=$(shell pwd)

ROSE_FLAGS=-rose:Cxx
INCLUDES=$(ROSE_INCLUDES)  -I$(builddir)/.. -I$(top_builddir) -I$(srcdir)/.. -Winvalid-pch


CLEANFILES = *.o *.pdf *.c result.txt

VPATH=$(shell find $(srcdir) -type d)


TEST_SUBTREE = $(shell cd $(srcdir) && find . -type d ! -name '.' ! -name '..' ! -name ".svn" ! -ipath "*/.svn/*")
LOCAL_SUBTREE = $(sort $(TEST_SUBTREE:./%=$(builddir)/%))

# We expect to succeed when we find a violation, and to fail when we don't find
# a violation.
TEST_SRCS = $(shell cd $(srcdir) && find . -mindepth 2 -type f ! -ipath "*/.svn/*")
XFAIL_SRCS = $(shell cd $(srcdir) && find . -mindepth 2 -type f ! -path "*/.svn/*" -ipath "./*/no*/*")


TESTS = $(TEST_SRCS:%.c=%.translated.c.bin)
XFAIL_TESTS =  $(XFAIL_SRCS:%.c=%.translated.c.bin)


RUNTIMECHECK=$(builddir)/../runtimeCheck


$(LOCAL_SUBTREE):
	mkdir -p $@ || mkdir $@

.PRECIOUS:	%.translated.c
%.translated.c:	 %.c $(LOCAL_SUBTREE)
	src=$(word 1, $?); \
	if [ "$$src" != "" ]; then \
		sed '1i\#include "RuntimeSystem.h"' $$src > $@ ; \
	fi; \
	cd $(@D) && \
	$(RUNTIMECHECK) 1 $(@F) $(ROSE_FLAGS) -c $(INCLUDES) && \
	mv rose_$(@F) $(@F)


%.translated.c.bin:	%.translated.c
	$(CXX) -g -o $@ $< $(srcdir)/../RuntimeSystem.c $(INCLUDES);


# We need the subtree built before we check the tests, but we don't want to
# clutter the dependencies of, e.g. %.translated.c
#
# check-local would work, except that in the default check rule, it's subsequent
# to check-am
check:	$(LOCAL_SUBTREE) check-am

check-am:
	@echo temporarily disabled check-am

clean-local:
	rm -fr $(LOCAL_SUBTREE)

