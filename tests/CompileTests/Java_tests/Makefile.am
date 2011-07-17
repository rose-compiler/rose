include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

##################################################################################
# These are C testcodes targeting aspects of C that might not be a subset of C++ #
##################################################################################

TESTCODES_REQUIRED_TO_PASS = \
     test2011_01.java \
     test2011_02.java \
     test2011_03.java \
     test2011_04.java \
     test2011_05.java \
     test2011_06.java \
     test2011_07.java \
     test2011_08.java \
     test2011_09.java \
     test2011_10.java \
     test2011_11.java \
     test2011_12.java \
     test2011_13.java \
     simple.java


# This class does not match the file name.
#    simple_fortran.java

TESTCODE_CURRENTLY_FAILING =

noinst_headers = 


# Automake's testing mechanism (which defines the "make check" rule) requires passing tests.
TESTCODES = \
$(TESTCODES_REQUIRED_TO_PASS)

# QMTest allows both passing and failing tests.
ALL_TESTCODES = \
$(TESTCODES_REQUIRED_TO_PASS) \
$(TESTCODE_CURRENTLY_FAILING)

# File option to accumulate performance information about the compilation
# PERFORMANCE_REPORT_OPTION = -rose:compilationPerformanceFile $(top_builddir)/C_ROSE_PERFORMANCE_DATA.csv

# The use of -rose:java is redundent, the source langauge can now be determined from the file suffix.
# ROSE_FLAGS = -rose:java -rose:verbose 0 -rose:skipfinalCompileStep
ROSE_FLAGS = -rose:verbose 0 -rose:skipfinalCompileStep

PASSING_TEST_Objects = ${TESTCODES:.java=.o}
TEST_Objects = ${ALL_TESTCODES:.java=.o}

if ROSE_BUILD_JAVA_LANGUAGE_SUPPORT

# testTranslator=../../testTranslator
testTranslator=../../testGraphGeneration

../../testTranslator:
	cd ../..; $(MAKE) testTranslator

../../testGraphGeneration:
	cd ../..; $(MAKE) testGraphGeneration

# Add the dependence upon the source file to trigger recompilation each time the makefile is run!
# $(TEST_Objects): ../../testTranslator $(srcdir)/$(@:.o=.java)
$(TEST_Objects): $(testTranslator) ../../testTranslator
	$(testTranslator) $(ROSE_FLAGS) $(srcdir)/$(@:.o=.java)

testJavaTraversal:
#	Java library path specifies where the *.so file is located (JavaTraversal.so)
#	The class path (-cp) the ROSE version of the ECJ Java front-end.
#	The we specify the ROSE specific JavaTraversal class file (that we execute)
#	Then we specify two input java source files.
#	java -Djava.library.path=$(top_builddir)/src/frontend/ECJ_ROSE_Connection -cp $(top_srcdir)/src/3rdPartyLibraries/java-parser/ecjROSE.jar:. $(top_srcdir)/src/3rdPartyLibraries/java-parser/JavaTraversal $(srcdir)/HelloWorld.java $(srcdir)/HelloWorld2.java
#	java -Djava.library.path=$(top_builddir)/src/frontend/ECJ_ROSE_Connection -cp $(top_srcdir)/src/3rdPartyLibraries/java-parser/ecjROSE.jar:. JavaTraversal $(srcdir)/HelloWorld.java $(srcdir)/HelloWorld2.java
#	java -Djava.library.path=$(top_builddir)/src/frontend/ECJ_ROSE_Connection -cp $(top_srcdir)/src/3rdPartyLibraries/java-parser/ecjROSE.jar:. libjava_ecj_parser_c_jni_JavaParserActionJNI $(srcdir)/HelloWorld.java $(srcdir)/HelloWorld2.java
#	java -Djava.library.path=$(top_builddir)/src/frontend/ECJ_ROSE_Connection -cp $(top_srcdir)/src/3rdPartyLibraries/java-parser/ecjROSE.jar:. libjava_ecj_parser_c_jni_JavaParserActionJNI 
#	java -Djava.library.path=$(top_builddir)/src/frontend/ECJ_ROSE_Connection/.libs -cp $(top_srcdir)/src/3rdPartyLibraries/java-parser/ecjROSE.jar:. libjava_ecj_parser_c_jni_JavaParserActionJNI 
#	java -Djava.library.path=$(top_builddir)/src/frontend/ECJ_ROSE_Connection/.libs -cp $(top_srcdir)/src/3rdPartyLibraries/java-parser/ecjROSE.jar:. JavaTraversal
#	java -cp $(top_srcdir)/src/3rdPartyLibraries/java-parser/ecjROSE.jar -Djava.library.path=$(top_builddir)/src/frontend/ECJ_ROSE_Connection/.libs JavaTraversal
	java -Djava.library.path=$(top_builddir)/src/frontend/ECJ_ROSE_Connection/.libs -cp $(top_builddir)/src/3rdPartyLibraries/java-parser/ecjROSE.jar:$(top_builddir)/src/3rdPartyLibraries/java-parser:. JavaTraversal $(srcdir)/simple.java

workingJava:
	java -cp $(top_srcdir)/src/3rdPartyLibraries/java-parser/ecjROSE.jar

testECJ:
	java -cp ecjROSE.jar org.eclipse.jdt.internal.compiler.batch.Main $(srcdir)/simple.java

# Test for help option to ECJ (some ECJ options not supported via ROSE).
testECJ_help_option:
	$(testTranslator) -rose:java -rose:verbose 0 -rose:skipfinalCompileStep -ecj:help -c $(srcdir)/simple.java

# Test with the "-rose:java" specification (one way to specify the language in ROSE).
testWithJavaSpecification:
	$(testTranslator) -rose:java $(ROSE_FLAGS) $(srcdir)/test2011_01.java

# Test without the "-rose:java" specification (ROSE can alternatively obtain the source language from the file suffix).
testWithoutJavaSpecification:
	$(testTranslator) $(ROSE_FLAGS) $(srcdir)/test2011_01.java

# Build a rule to allow running all tests in parallel...
extraTests: testWithJavaSpecification testWithoutJavaSpecification testECJ_help_option

# Execute the java file using: java -Djava.library.path=. HelloWorld

check-local:
	@echo "Tests for Java."
#  Run this test explicitly since it has to be run using a specific rule and can't be lumped with the rest
#	These C programs must be called externally to the test codes in the "TESTCODES" make variable
	@$(MAKE) $(PASSING_TEST_Objects) extraTests
#	@$(MAKE) testWithJavaSpecification
#	@$(MAKE) testWithoutJavaSpecification
#	@$(MAKE) testECJ_help_option
#	@$(MAKE) extraTests
	@echo "************************************************************************************************"
	@echo "****** ROSE/tests/CompileTests/Java_tests: make check rule complete (terminated normally) ******"
	@echo "************************************************************************************************"
else
check-local:
	@echo "Tests for Java."
	@echo "**********************************************************************************************"
	@echo "****** ROSE/tests/CompileTests/Java_tests: JAVA LANGUAGE SUPPORT NOT CONFIGURED IN ROSE ******"
	@echo "**********************************************************************************************"
endif

EXTRA_DIST = $(ALL_TESTCODES)

clean-local:
	rm -f *.dot *.class *.java rose_performance_report_lockfile.lock *.out
#	Remove any generated JVM log files that could have been generated (these are rare).
	rm -f *.log
