include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

# Don't include PetSc by default just yet
# (not sure we can support it as a test code directly within ROSE just yet)
# SUBDIRS = A++Tests P++Tests A++Code C++Code OvertureCode PetscCode
# SUBDIRS = A++Tests C++Code A++Code OvertureCode P++Tests
# SUBDIRS = ElsaTestCases

SUBDIRS = \
     C_tests C99_tests UPC_tests Cxx_tests C_subset_of_Cxx_tests \
     Fortran_tests copyAST_tests RoseExample_tests PythonExample_tests \
     colorAST_tests mergeAST_tests unparseToString_tests \
     sourcePosition_tests OpenMP_tests \
     boost_tests virtualCFG_tests uninitializedField_tests \
     A++Tests OvertureCode P++Tests A++Code ElsaTestCases \
     ExpressionTemplateExample_tests hiddenTypeAndDeclarationListTests

# DQ (12/5/2007): Removed because this takes a long time (about 1 minute)
# A++Code 

# DQ (9/3/2006): Remove C++Code (it is the older version of what is now multiple testing directories)
# C++Code 

# Note that these are not in the development test (and perhaps they should be):
#      A++Tests A++Code OvertureCode P++Tests ElsaTestCases

# The Makefile.am is not ready to be used for a distribution of ROSE
# This needs more work.
# Error reported in "make distcheck": gmake[6]: *** No rule to make target `t0001.cc', needed by `t0001.o'.  Stop.
# ElsaTestCases

check-local:
	@echo "*************************************************************************************"
	@echo "****** ROSE/tests/CompileTests: make check rule complete (terminated normally) ******"
	@echo "*************************************************************************************"

