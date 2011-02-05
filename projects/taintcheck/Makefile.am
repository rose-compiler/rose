include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

INCLUDES = \
     $(ROSE_INCLUDES) 

bin_PROGRAMS = taintcheck

LDADD = $(LIBS_WITH_RPATH) $(ROSE_LIBS) -ldl $(LIBLTDL)

taintcheck_SOURCES = taintcheck.C

EXTRA_DIST = tests

CLEANFILES = *.dot 

clean:
	rm -f *.dot *.gml *.lo *.la *.s *.o

check-local:
	@echo "********************************************************************************"
	@echo "*** ROSE/projects/taintcheck: make check rule complete (terminated normally) ***"
	@echo "********************************************************************************"
