include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

# Global setting for ROSE verbosity
ROSE_VERBOSE=-rose:verbose 0
CLASSPATH=-classpath ./rose-output

# Initial definition
bin_PROGRAMS =

# Use variables set in ROSE/config/Makefile.for.ROSE.includes.and.libs
AM_CPPFLAGS = $(ROSE_INCLUDES) $(JAVA_JVM_INCLUDE)
AM_LDFLAGS = $(ROSE_RPATHS)
LDADD    = $(ROSE_LIBS)

clean-local:
	rm -fr rose-* ecj-classes-* javac-*
	rm -f core.*
	rm -f hs_err_pid*

check-local:
if USING_GNU_COMPILER
# DQ (2/20/2017): Don't use thiese tests on later versions of CNG 4.9 or later.
if !ROSE_USING_GCC_VERSION_LATER_4_9
	@$(MAKE) $(UNIT_TESTS)
endif
endif
	@echo "***********************************************************************************************************"
	@echo "****** ROSE/tests/nonsmoke/functional/CompileTests/Java_tests/unit_tests: make check rule complete (terminated normally) ******"
	@echo "***********************************************************************************************************"

if USING_GNU_COMPILER
# DQ (2/20/2017): Don't use thiese tests on later versions of CNG 4.9 or later.
if !ROSE_USING_GCC_VERSION_LATER_4_9
UNIT_TESTS = \
	test_addJavaAssignmentStmt \
	test_addJavaExpression \
	test_addJavaVariableDeclaration \
	test_addJavaSource \
	test_addJavaTransformation
# TOO1 (3/7/2014): Requires Java 7u51+; JVM crashes on "jni_GetStringUTFChars"
#test_addJavaSource
#test_addJavaTransformation

#-------------------------------------------------------------------------------
#-------------------------------------------------------------------------------
#                                  Unit Tests
#-------------------------------------------------------------------------------
#-------------------------------------------------------------------------------

#---------------------------------------
# test:   addJavaTransformation
# author: Philippe Charles
#---------------------------------------
bin_PROGRAMS += addJavaTransformation
addJavaTransformation_SOURCES = addJavaTransformation.cpp

test_addJavaTransformation: addJavaTransformation
	./$< $(CLASSPATH) $(ROSE_VERBOSE) "$(srcdir)/$<Testcode.java"

#---------------------------------------
# test:   addJavaSource
# author: Philippe Charles
#---------------------------------------
bin_PROGRAMS += addJavaSource
addJavaSource_SOURCES = addJavaSource.cpp
test_addJavaSource: addJavaSource
	./$< $(CLASSPATH) $(ROSE_VERBOSE) "$(srcdir)/$<Testcode.java"

#---------------------------------------
# test:   addJavaAssignmentStmt
# author: Philippe Charles
#---------------------------------------
bin_PROGRAMS += addJavaAssignmentStmt
addJavaAssignmentStmt_SOURCES = addJavaAssignmentStmt.cpp
test_addJavaAssignmentStmt: addJavaAssignmentStmt
	./$< $(CLASSPATH) $(ROSE_VERBOSE) "$(srcdir)/$<Testcode.java"

#---------------------------------------
# test:   addJavaExpression
# author: Philippe Charles
#---------------------------------------
bin_PROGRAMS += addJavaExpression
addJavaExpression_SOURCES = addJavaExpression.cpp
test_addJavaExpression: addJavaExpression
	./$< $(CLASSPATH) $(ROSE_VERBOSE) "$(srcdir)/$<Testcode.java"

#---------------------------------------
# test:   addJavaVariableDeclaration
# author: Philippe Charles
#---------------------------------------
bin_PROGRAMS += addJavaVariableDeclaration
addJavaVariableDeclaration_SOURCES = addJavaVariableDeclaration.cpp
test_addJavaVariableDeclaration: addJavaVariableDeclaration
	./$< $(CLASSPATH) $(ROSE_VERBOSE) "$(srcdir)/$<Testcode.java"

endif
else
# DQ (2/20/2017): Disabling unit tests for non-GNU compilers.
endif
