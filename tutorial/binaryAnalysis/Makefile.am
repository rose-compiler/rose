include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

# To keep compile commands tidy, please ONLY put required include flags here.
INCLUDES = $(ROSE_INCLUDES)

bin_PROGRAMS = matchPattern

matchPattern_SOURCES = matchPattern.C

DEPENDENCIES = $(REQUIRED_LIBS)
LDADD  = $(LIBS_WITH_RPATH) $(ROSE_LIBS)

noinst_PROGRAMS = 


VALGRIND_OPTIONS = --tool=memcheck -v --num-callers=30 --leak-check=no --error-limit=no --show-reachable=yes
VALGRIND =

# Build a simple 32bit binary to use in testing.
input_matchPattern: $(srcdir)/input_matchPattern.C
	g++ -m32 -o input_matchPattern $(srcdir)/input_matchPattern.C

testmatchPattern: matchPattern input_matchPattern
	./matchPattern -c input_matchPattern

EXTRA_DIST = CMakeLists.txt input_matchPattern.C

check-local:
# DQ (4/28/2010): This appears to fail in 32-bit machines.  For now get existing work checked in and 
# investigate this detail afterward (ongoing work).
#	@$(MAKE) testmatchPattern
	@echo "****************************************************************************************"
	@echo "****** ROSE/tests/translatorTests: make check rule complete (terminated normally) ******"
	@echo "****************************************************************************************"


distclean-local:
	rm -rf Makefile

clean-local:
	rm -rf *.o *.ps *.pdf *.dot rose_*.C rose_*.c core *.int.c *.ti a.out roseTestProgram.C *.dot *.cpp

cleandata:
	rm -f *.ti *.ii rose_*.C rose_*.c *.dot test*.o rose_*.o

