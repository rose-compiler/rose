include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

# DQ (10/23/2013): Disable so that we can try an approach modifiying the original ATerm library to be ROSE specific.
# DQ (10/20/2013): Allow alternative support using ROSE ATerm library API.
# SUBDIRS = tests

INCLUDES = $(ROSE_INCLUDES)

bin_PROGRAMS = shiftCalculusCompiler  

# Liao, 9/14/15 C++11 has only been available since G++ 4.3. 
# In versions pre-G++ 4.7, you'll have to use -std=c++0x, for more recent versions you can use -std=c++11.
#CXX11FLAG = -std=c++0x
CXX11FLAG = -std=c++11

CPPFLAGS += $(CXX11FLAG)
 
shiftCalculusCompiler_SOURCES = shiftCalculusCompiler.C stencilAndStencilOperatorDetection.C stencilEvaluation.C stencilFiniteStateMachine.C dslSupport.C dslCodeGeneration.C lowLevelCodeGeneration.C

include_HEADERS = shiftCalculusCompiler.h stencilAndStencilOperatorDetection.h stencilEvaluation.h stencilFiniteStateMachine.h dslSupport.h dslCodeGeneration.h lowLevelCodeGeneration.h

ATERM_LIB_DIR = $(ATERM_INSTALL_PATH)/lib

LDADD = $(LIBS_WITH_RPATH) $(ROSE_LIBS)

TESTCODES_REQUIRED_TO_PASS = test2014_01.C

TESTCODE_CURRENTLY_FAILING =

# Automake's testing mechanism (which defines the "make check" rule) requires passing tests.
TESTCODES = $(TESTCODES_REQUIRED_TO_PASS)

ALL_TESTCODES = $(TESTCODES) $(TESTCODE_CURRENTLY_FAILING)

PASSING_TEST_Output = $(addprefix rose_,${TESTCODES})
TEST_Output = $(addprefix rose_,${ALL_TESTCODES})

PASSING_REGRESSION_TEST_Output = ${TESTCODES_REQUIRED_TO_PASS:.C=.C.passing}


$(PASSING_REGRESSION_TEST_Output): ./shiftCalculusCompiler
	./shiftCalculusCompiler $(ROSE_FLAGS) -I$(top_srcdir)/tests/CompileTests/Cxx_tests -c $(top_srcdir)/tests/CompileTests/Cxx_tests/$(@:.C.passing=.C)



test1: shiftCalculusCompiler
	./shiftCalculusCompiler -c $(srcdir)/identity.cpp

test2: shiftCalculusCompiler
	./shiftCalculusCompiler -c $(srcdir)/laplacian.cpp


test3: shiftCalculusCompiler
	./shiftCalculusCompiler -c $(srcdir)/identity_lite.cpp

test4: shiftCalculusCompiler
	./shiftCalculusCompiler -c $(srcdir)/laplacian_lite.cpp

test5: shiftCalculusCompiler
	./shiftCalculusCompiler -c $(srcdir)/test2014_02.C

test6: shiftCalculusCompiler
	./shiftCalculusCompiler -c $(srcdir)/test2014_03.C

test7: shiftCalculusCompiler
	./shiftCalculusCompiler -c $(srcdir)/test2014_04.C

test8: shiftCalculusCompiler
	./shiftCalculusCompiler -c $(srcdir)/test2014_05.C

test9: shiftCalculusCompiler
	./shiftCalculusCompiler -c $(srcdir)/laplacian_lite_v2.cpp

test10: shiftCalculusCompiler
	@echo "********************************************************************************************"
	@echo "******* ROSE/projects/ShiftCalculus: code generation for C++ code                   ********"
	@echo "********************************************************************************************"
	./shiftCalculusCompiler -c -std=c++11 $(srcdir)/laplacian_lite_v3.cpp
	g++ -std=c++11 rose_laplacian_lite_v3.cpp $(srcdir)/Box.cpp -I$(srcdir) -o $@ 

POLYOPT_PATH=$(top_builddir)/projects/PolyOpt2

test10poly: shiftCalculusCompiler
	./shiftCalculusCompiler -c -rose:dslcompiler:polyopt -std=c++11 $(srcdir)/laplacian_lite_v3.cpp -rose:skipfinalCompileStep -rose:output laplacian_lite_v3_polyopt.cpp
	make -C $(POLYOPT_PATH)
	@echo "********************************************************************************************"
	@echo "******* ROSE/projects/ShiftCalculus: running polyhedral parallel only               ********"
	@echo "********************************************************************************************"
	$(POLYOPT_PATH)/src/PolyOpt laplacian_lite_v3_polyopt.cpp -I$(srcdir) -std=c++11 -rose:Cxx11 -rose:skipfinalCompileStep --polyopt-parallel-only -rose:o rose_parallel_only_laplacian_lite_v3_polyopt.cpp
	@echo "********************************************************************************************"
	@echo "******* ROSE/projects/ShiftCalculus: running polyhedral tiling (tile size 8x8x8)    ********"
	@echo "********************************************************************************************"
	echo "8 8 8" >> tile.sizes
	$(POLYOPT_PATH)/src/PolyOpt laplacian_lite_v3_polyopt.cpp -I$(srcdir) -std=c++11 -rose:Cxx11 -rose:skipfinalCompileStep --polyopt-fixed-tiling -rose:o rose_fixed-tiling_laplacian_lite_v3_polyopt.cpp
        
VECTOR_PATH=$(top_builddir)/projects/vectorization

test10vector: shiftCalculusCompiler
	./shiftCalculusCompiler -c -rose:dslcompiler:vectorization -std=c++11 $(srcdir)/laplacian_lite_v3_vec.cpp -rose:skipfinalCompileStep -rose:output laplacian_lite_v3_vec.cpp
	make -C $(VECTOR_PATH)
	@echo "********************************************************************************************"
	@echo "******* ROSE/projects/ShiftCalculus: code generation for vectorization              ********"
	@echo "********************************************************************************************"
	$(VECTOR_PATH)/src/vectorization laplacian_lite_v3_vec.cpp -I$(srcdir) -std=c++11 -rose:Cxx11 -rose:skipfinalCompileStep -rose:o rose_vectorization_laplacian_lite_v3.cpp

MLM_PATH=$(top_builddir)/projects/MultiLevelMemory
test10MLM: shiftCalculusCompiler
	make -C $(MLM_PATH)
	@echo "********************************************************************************************"
	@echo "******* ROSE/projects/ShiftCalculus: code generation for SST Simulator              ********"
	@echo "********************************************************************************************"
	$(MLM_PATH)/src/mlm $(srcdir)/laplacian_lite_v3_MLM.cpp -I$(srcdir) -std=c++11 -rose:Cxx11 -rose:skipfinalCompileStep -rose:o rose_MLM_laplacian_lite_v3.cpp

test11: shiftCalculusCompiler
	./shiftCalculusCompiler -std=c++11 -c $(srcdir)/simpleCNS.cpp

rose_laplacian_lite.cu: shiftCalculusCompiler
	./shiftCalculusCompiler -c -rose:dslcompiler:cuda -std=c++11 -rose:skipfinalCompileStep -rose:output rose_laplacian_lite.cu $(srcdir)/laplacian_lite.cpp
rose_laplacian_lite_v2.cu: shiftCalculusCompiler
	./shiftCalculusCompiler -c -rose:dslcompiler:cuda -std=c++11 -rose:skipfinalCompileStep -rose:output $@ $(srcdir)/laplacian_lite_v2.cpp
rose_laplacian_lite_v3.cu: shiftCalculusCompiler
	@echo "********************************************************************************************"
	@echo "******* ROSE/projects/ShiftCalculus: code generation for GPU                        ********"
	@echo "********************************************************************************************"
	./shiftCalculusCompiler -c -rose:dslcompiler:cuda -std=c++11 -rose:skipfinalCompileStep -rose:output $@  $(srcdir)/laplacian_lite_v3.cpp

rose_laplacian_lite_v3_mpi.cpp: shiftCalculusCompiler
	@echo "********************************************************************************************"
	@echo "******* ROSE/projects/ShiftCalculus: code generation for MPI ********"
	@echo "********************************************************************************************"
	./shiftCalculusCompiler -c -rose:dslcompiler:mpi -std=c++11 -rose:skipfinalCompileStep -rose:output $@  $(srcdir)/laplacian_lite_v3.cpp


rose_laplacian_lite_v3_GPU_collapsed.cu: 
	@echo "********************************************************************************************"
	@echo "******* ROSE/projects/ShiftCalculus: code generation for GPU with loop collapsing   ********"
	@echo "********************************************************************************************"
	./shiftCalculusCompiler -c -rose:dslcompiler:cuda -rose:dslcompiler:collapse -std=c++11 -rose:skipfinalCompileStep -rose:output $@  $(srcdir)/laplacian_lite_v3.cpp

test12: shiftCalculusCompiler
	./shiftCalculusCompiler -std=c++11 -c $(srcdir)/simpleCNSPhil.cpp

# Note that test1 and test3 are zero dimensional stencils that are not supported for code generation yet.
# test2 is including STL and other library code
# test4 is a version of test2 which excludes STL and library code (and it thus smaller to work with but the same API).
# test5 - test8 test subsets of the stencil specification.
localTests: test4 test2 test5 test6 test7 test8

mpiTests: rose_laplacian_lite_v3_mpi.cpp

cudaTests: rose_laplacian_lite_v3.cu rose_laplacian_lite_v3_GPU_collapsed.cu 

demo: test10 cudaTests test10poly test10vector test10MLM
demo2015: test10 cudaTests test10vector 

check-local:
	@echo "Tests for Shift Calculus examples."
#	@$(MAKE) $(PASSING_TEST_Output)
	@$(MAKE) $(PASSING_REGRESSION_TEST_Output)
	@$(MAKE) localTests cudaTests
	@echo "********************************************************************************************"
	@echo "******* ROSE/projects/ShiftCalculus: make check rule complete (terminated normally) ********"
	@echo "********************************************************************************************"


EXTRA_DIST = refCode]ls/laplacian_mpi.cpp refCode/laplacian_mpi_v1_1.cpp refCode/laplacian_mpi_v1.cpp refCode/laplacian_mpi_v2.cpp \
             refCode/laplacian_mpi_wrt.cpp refCode/laplacian_mpi_v1_2.cpp refCode/laplacian_mpi_v2_2.cpp refCode/laplacian_mpi_v4.cpp
CLEANFILES = 

clean-local:
	rm -rf rose_*.C *.pdf *.cu rose_*.cpp
	rm -f a.out *.dot 

