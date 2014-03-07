include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

# Global setting for ROSE verbosity
ROSE_VERBOSE=-rose:verbose 0
CLASSPATH=-classpath ./rose-output

# Initial definition
bin_PROGRAMS =

# Use variables set in ROSE/config/Makefile.for.ROSE.includes.and.libs
INCLUDES = $(ROSE_INCLUDES) $(JAVA_JVM_INCLUDE)
LDADD    = $(ROSE_LIBS) $(LIBS_WITH_RPATH)

clean-local:
	rm -fr rose-* ecj-classes-* javac-*

check-local:
	@$(MAKE) $(UNIT_TESTS)
	@echo "***********************************************************************************************************"
	@echo "****** ROSE/tests/CompileTests/Java_tests/unit_tests: make check rule complete (terminated normally) ******"
	@echo "***********************************************************************************************************"

UNIT_TESTS = \
	test_addJavaAssignmentStmt \
	test_addJavaExpression \
	test_addJavaVariableDeclaration
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
	./addJavaTransformation $(CLASSPATH) $(ROSE_VERBOSE) "$(srcdir)/InputCode_java.java"

#---------------------------------------
# test:   addJavaSource
# author: Philippe Charles
#---------------------------------------
bin_PROGRAMS += addJavaSource
addJavaSource_SOURCES = addJavaSource.cpp
test_addJavaSource: addJavaSource
	./addJavaSource $(CLASSPATH) $(ROSE_VERBOSE) "$(srcdir)/InputCode_java.java"

#---------------------------------------
# test:   addJavaAssignmentStmt
# author: Philippe Charles
#---------------------------------------
bin_PROGRAMS += addJavaAssignmentStmt
addJavaAssignmentStmt_SOURCES = addJavaAssignmentStmt.cpp
test_addJavaAssignmentStmt: addJavaAssignmentStmt
	./addJavaAssignmentStmt $(CLASSPATH) $(ROSE_VERBOSE) $(srcdir)/InputCode_java.java

#---------------------------------------
# test:   addJavaExpression
# author: Philippe Charles
#---------------------------------------
bin_PROGRAMS += addJavaExpression
addJavaExpression_SOURCES = addJavaExpression.cpp
test_addJavaExpression: addJavaExpression
	./addJavaExpression $(CLASSPATH) $(ROSE_VERBOSE) $(srcdir)/InputCode_java.java

#---------------------------------------
# test:   addJavaVariableDeclaration
# author: Philippe Charles
#---------------------------------------
bin_PROGRAMS += addJavaVariableDeclaration
addJavaVariableDeclaration_SOURCES = addJavaVariableDeclaration.cpp
test_addJavaVariableDeclaration: addJavaVariableDeclaration
	./addJavaVariableDeclaration $(CLASSPATH) $(ROSE_VERBOSE) $(srcdir)/InputCode_java.java

