include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

# SUBDIRS = eventDetection

bin_PROGRAMS = reverse
reverse_SOURCES = reverse.cpp

INCLUDES = $(ROSE_INCLUDES)
LDADD    = $(LIBS_WITH_RPATH) $(ROSE_LIBS)

EXTRA_DIST = 

CLEANFILES = 

include_HEADERS =

ALL_TESTCODES = \
test.C		

TEST_Objects = ${ALL_TESTCODES:.C=.o}

$(TEST_Objects): reverse
	@cp $(srcdir)/test/$(@:.o=.C) $(@:.o=.C)
	./reverse -I$(srcdir)/test -c $(@:.o=.C)       		
	g++ -I$(srcdir)/test rose_$(@:.o=.C) -o $(@:.o=.C).o
	./$(@:.o=.C).o		

check-local:
	@$(MAKE) $(TEST_Objects)
	@echo "*********************************************************************************************************"
	@echo "****** ROSE/projects/backstroke/reverseComputation: make check rule complete (terminated normally) ******"
	@echo "*********************************************************************************************************"
