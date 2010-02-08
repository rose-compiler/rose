include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

# DQ (2/6/2010): Reordered a few of the first directories to have the most relevant test run first by "make check"
# This should make the testing of ROSE more meaningful for users.
SUBDIRS = \
     C_tests Cxx_tests C_subset_of_Cxx_tests C99_tests Fortran_tests UPC_tests \
     copyAST_tests RoseExample_tests PythonExample_tests \
     colorAST_tests mergeAST_tests unparseToString_tests \
     sourcePosition_tests OpenMP_tests \
     boost_tests virtualCFG_tests uninitializedField_tests \
     A++Tests OvertureCode P++Tests A++Code ElsaTestCases \
     ExpressionTemplateExample_tests hiddenTypeAndDeclarationListTests \
     sizeofOperation_tests MicrosoftWindows_tests nameQualificationAndTypeElaboration_tests \
     NewEDGInterface_C_tests

# DQ (12/5/2007): Removed A++Code because this takes a long time (about 1 minute).

check-local:
	@echo "*************************************************************************************"
	@echo "****** ROSE/tests/CompileTests: make check rule complete (terminated normally) ******"
	@echo "*************************************************************************************"

