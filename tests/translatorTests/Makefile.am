include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

# To keep compile commands tidy, please ONLY put required include flags here.
INCLUDES = $(ROSE_INCLUDES)

# The bug numbers come from ROSE's SciDAC bug trackers
noinst_PROGRAMS = \
       bug125 

bug125_SOURCES = bug125.C
bug125_DEPENDENCIES = $(ROSE_LIBS_WITH_PATH)

LDADD = $(LIBS_WITH_RPATH) $(ROSE_SEPARATE_LIBS)

VALGRIND_OPTIONS = --tool=memcheck -v --num-callers=30 --leak-check=no --error-limit=no --show-reachable=yes

VALGRIND =

testbug125:bug125 
	./bug125 -c $(srcdir)/inputbug125.C

EXTRA_DIST =  inputbug125.C

check-local:
	@$(MAKE) testbug125 

distclean-local:
	rm -rf Makefile

clean-local:
	rm -rf *.o *.ps *.pdf *.dot rose_*.C rose_*.c core *.int.c *.ti a.out roseTestProgram.C

cleandata:
	rm -f *.ti *.ii rose_*.C rose_*.c *.dot test*.o rose_*.o

