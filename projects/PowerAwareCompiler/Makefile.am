include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs


INCLUDES = $(ROSE_INCLUDES)

# Location of Thrifty simulator
THRIFTY_SIMULATOR = /home/dquinlan/PROJECTS/THRIFTY/SVN
MIPS_CROSS_COMPILER_PATH = /nfs/casc/overture/ROSE/git/sescutils-ins/bin

MIPS_CROSS_COMPILER_GNU_gcc = $(MIPS_CROSS_COMPILER_PATH)/mipseb-linux-gcc
MIPS_CROSS_COMPILER_GNU_g77 = $(MIPS_CROSS_COMPILER_PATH)/mipseb-linux-g77
MIPS_CROSS_COMPILER_GNU_cxx = $(MIPS_CROSS_COMPILER_PATH)/mipseb-linux-g++

bin_PROGRAMS = thrifty

thrifty_SOURCES = thrifty.C

LDADD = $(LIBS_WITH_RPATH) $(ROSE_LIBS)

testThrifty: thrifty
	./thrifty -rose:verbose 0 -c $(srcdir)/inputCode_thrifty_01.c

# Example of running the simulator
testSimulator:
	$(THRIFTY_SIMULATOR)/build/sesc.tst -h0x800000 -c../sesc/confs/mem.conf ../sesc/tests/crafty < ../sesc/tests/tt.in
	$(THRIFTY_SIMULATOR)/build/sesc.mem -h0x6000000 -c../sesc/confs/mem.conf ../sesc/tests/mcf ../sesc/tests/mcf.in

# Required flags to build executables for use with Thrifty simulator.
MIPS_FLAGS = -mabi=32 -Wa,-non_shared -mno-abicalls

testMipsCompiler_cc:
	$(MIPS_CROSS_COMPILER_GNU_gcc) $(MIPS_FLAGS) -o inputCode_C_thrifty_01 $(srcdir)/inputCode_C_thrifty_01.c

testMipsCompiler_f77:
	$(MIPS_CROSS_COMPILER_GNU_g77) $(srcdir)/inputCode_F77_thrifty_01.f

testMipsCompiler_cxx:
	$(MIPS_CROSS_COMPILER_GNU_cxx) $(srcdir)/inputCode_CXX_thrifty_01.C

# This can be empty for now since we don't have ROSE set up to 
# take a path to the MIPS cross-compilers so that we can test 
# this using Hudson.
check-local:

clean-local:
	rm -f *.o rose_*.c

EXTRA_DIST = inputCode_CXX_thrifty_01.C inputCode_C_thrifty_01.c inputCode_F77_thrifty_01.f

