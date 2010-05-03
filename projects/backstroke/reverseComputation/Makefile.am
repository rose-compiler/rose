include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

# SUBDIRS = eventDetection

# This file does not compile using ROSE (skip testing it for now)
if !ROSE_USING_ROSE_ANALYSIS
   bin_PROGRAMS = reverse
   reverse_SOURCES = reverse.cpp
endif

INCLUDES = $(ROSE_INCLUDES)
LDADD    = $(LIBS_WITH_RPATH) $(ROSE_LIBS)

EXTRA_DIST = reverse.h test

CLEANFILES = 

include_HEADERS =

ALL_TESTCODES = \
test0.c \
test1.c \
test2.c \
test3.c \
test4.c \
test5.c \
test6.c \
test7.c \
test8.c \
test9.c \
test10.c \
test11.c \
test12.c \
test13.c \
test14.c \
test15.c \
test16.c \
test17.c \
test18.c

TEST_Objects = ${ALL_TESTCODES:.c=.o}

$(TEST_Objects): reverse
	@cp $(srcdir)/test/$(@:.o=.c) $(@:.o=.c)
	./reverse -I$(srcdir)/test -c $(@:.o=.c) -rose:C99 -DROSE_LANGUAGE_MODE=0       		
	@g++ -I$(srcdir)/test rose_$(@:.o=.c) -o $(@:.o=.c).o
	./$(@:.o=.c).o		

check-local:
	@$(MAKE) $(TEST_Objects)
	@echo "*********************************************************************************************************"
	@echo "****** ROSE/projects/backstroke/reverseComputation: make check rule complete (terminated normally) ******"
	@echo "*********************************************************************************************************"
