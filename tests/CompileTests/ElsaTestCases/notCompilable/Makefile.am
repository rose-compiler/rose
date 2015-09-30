## Process this file with automake to create Makefile.in
include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

SUBDIRS = gnu kandr std
## Add bugs to this line as they are fixed so that they can be used as test problems
## At least we should not reintroduce these fixed bugs again later.
# TESTCODES = \
#      test2001_11.C \
#      test2001_01.C \
#      test2001_02.C \
#      test2001_03.C \
#      test2001_04.C \
#      test2001_05.C \
#      test2001_06.C \
#      test2001_07.C \
#      test2001_08.C \
#      test2001_09.C \
#      test2001_10.C \
#      test2001_14.C \
#      test2001_15.C \
#      test2001_17.C \
#      test2001_18.C \
#      test2001_19.C \
#      test2001_20.C \
#      test2001_21.C \
#      test2001_22.C \
#      test_treeTrav_MK_01.C \
#      test_treeTrav_MK_02.C

TESTCODES = \
t0147.cc\
t0148.cc\
t0157.cc\
t0159.cc\
t0162.cc\
t0163.cc\
t0177.cc\
t0178.cc\
t0196.cc\
t0209.cc\
t0215.cc\
t0216.cc\
t0221.cc\
t0224.cc\
t0228.cc\
t0230.cc\
t0242.cc\
t0243.cc\
t0244.cc\
t0249.cc\
t0251.cc\
t0258.cc\
t0261.cc\
t0263.cc\
t0264.cc\
t0265.cc\
t0267.cc\
t0279.cc\
t0282.cc\
t0287.cc\
t0288.cc\
t0297.cc\
t0299.cc\
t0309.cc\
t0310.cc\
t0311.cc\
t0339.cc\
t0342.cc\
t0345.cc\
t0346.cc\
t0347.cc\
t0354.cc\
t0371.cc\
t0380.cc\
t0383.cc\
t0385.cc\
t0401.cc\
t0402.cc\
t0403.cc\
t0408.cc\
t0416.cc\
t0423.cc\
t0427.cc\
t0429.cc\
t0431.cc\
t0452.cc\
t0466.cc\
t0509.cc\
t0511.cc\
t0516.cc\
t0518.cc\
t0519.cc\
t0520.cc\
t0529.cc\
t0533.cc\
t0561.cc\
t0576.cc\
t0578.cc\
d0041.cc\
d0050elab.cc\
d0053.cc\
d0062.cc\
d0063.cc\
d0103.cc\
d0110.cc\
d0112.cc\
d0115.cc\
d0123.cc\
k0044.cc\
k0053.cc\
t0006.cc\
t0007.cc\
t0012.cc\
t0014.cc\
t0030.cc\
t0031.cc\
t0036.cc\
t0040.cc\
t0054.cc\
t0062.cc\
t0068.cc\
t0069.cc\
t0072.cc\
t0074.cc\
t0078.cc\
t0079.cc\
t0082.cc\
t0091.cc\
t0093.cc\
t0099.cc\
t0109.cc\
t0116.cc\
t0117.cc\
t0118.cc\
t0120.cc\
t0123.cc\
t0126.cc\
t0134.cc\
t0136.cc\
t0137.cc\
t0138.cc\
t0139.cc\
t0140.cc\
t0141.cc\
t0142.cc\
t0143.cc\
t0144.cc\
t0145.cc\
t0146.cc
t0272.cc\
t0271.cc\
t0449.cc\
t0465.cc\
t0539.cc\
t0539_9.cc\
d0118.cc\
d0120.cc\
t0129.cc\
k0028.cc\
k0034.cc

# DQ (9/4/2005): Generated code with name qualification 
# "namespace std { template<> struct std::X < int > }" 
# not allowed in g++ 3.4.3 (test commented out for now).
# There are still several cases like this so that the 
# code generation within ROSE will not pass with 3.4.x
# at present.
#    test2005_81.C
#    test2005_93.C

# DQ (7/27/2005): Example generates instanatiations for template static data members (redifinition error)
#    test2005_69.C

# DQ (9/10/2005): This code fails with 3.4.x, but passes with 3.3.x
#    test2005_98.C

# DQ (7/25/2005): Example which breaks g++ 3.4.3 (from Peter), also breaks ROSE :-)
#    test2005_115.C

# This demonstrates a bug in g++ (ROSE can't handle it until g++ can handle it)
#    test2005_151.C

# DQ (6/30/2005): Removed to compile Kull (submitted by Jeremiah Willcock)
#    test2005_85.C (Duff's Device)

# Moved to explicit rule below so that we could include -I<pythonDir>
#     test2005_18.C
#     test2005_54.C
#     test2005_24.C
#     test2004_62.C 
#     test2004_63.C 
#     test2004_41.C

# This is now an explicit makefile rule (so that we can use the -rose:C_only option)
#     test2004_21.C
#     test2004_21.c 

# This fails since it uses old style C and must be compiled in a file with *.c (instead of *.C)
#      test2004_20.C

# These require special options and have specific makefile rules
#     test2004_22.C
#     test2004_30.C
#     test2004_41.C

# DQ (8/8/2005): requires use od Python.h not included in ROSE distribution
#     test2004_92.C

# Build the list of object files
TEST_Objects = ${TESTCODES:.cc=.o}

# Restrict to level 6 testing for the moment
#ROSE_FLAGS = -rose:test 6
ROSE_FLAGS = 

ROSE_VERBOSE = -rose:verbose
# ROSE_VERBOSE =

C_ONLY = -rose:C_only
# C_ONLY =

TRANSLATOR_EXECUTABLE = $(top_builddir)/tests/testTranslator 
TEST_TRANSLATOR = $(TRANSLATOR_EXECUTABLE) $(ROSE_FLAGS)

## Include the path to A++ and the transformation specification
TESTCODE_INCLUDES = -I$(top_srcdir)/tests/CompileTests/A++Code -I$(srcdir) 
# check-local:
#	@echo "ROSE Test Codes."
#  Run this test explicitly since it has to be run using a specific rule and can't be lumped with the rest
#	@$(MAKE) test2004_21.o
#	@$(MAKE) test2004_22.o
#	@$(MAKE) test2004_30.o
#	@$(MAKE) test2004_41.o
#	@$(MAKE) $(TEST_Objects)

$(TEST_Objects): $(TESTCODES)
	$(TEST_TRANSLATOR) $(AM_CPPFLAGS) $(TESTCODE_INCLUDES) -c $(srcdir)/$(@:.o=.cc)

EXTRA_DIST = $(TESTCODES) c c99 msvc

clean-local:
	rm -f C/*.o *.o C/rose_*.c rose_*.C rose_*.c rose_*.cc *.sage.C test2001_??.C.show *.dot *.ps \
              *.C.pdf *.C.edg.pdf *.pdf *.int.C core test2005_56
	rm -rf ti_files *.ti *.et

check-local: $(TRANSLATOR_EXECUTABLE) 
	@$(MAKE) $(TEST_Objects)
	@$(MAKE) $(TEST2_Objects)
	@echo "*********************************************************************************************"
	@echo "****** ROSE/tests/CompileTests/C++Code: make check rule complete (terminated normally) ******"
	@echo "*********************************************************************************************"



