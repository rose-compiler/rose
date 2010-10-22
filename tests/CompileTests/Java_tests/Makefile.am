include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

##################################################################################
# These are C testcodes targeting aspects of C that might not be a subset of C++ #
##################################################################################

TESTCODES_REQUIRED_TO_PASS = \
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

ROSE_FLAGS = -rose:java -rose:verbose 0 -rose:skipfinalCompileStep

PASSING_TEST_Objects = ${TESTCODES:.java=.o}
TEST_Objects = ${ALL_TESTCODES:.java=.o}

# if ROSE_USE_OPEN_FORTRAN_PARSER
if ROSE_BUILD_JAVA_LANGUAGE_SUPPORT

testTranslator=../../testTranslator

../../testTranslator:
	cd ../..; $(MAKE) testTranslator

# Add the dependence upon the source file to trigger recompilation each time the makefile is run!
# $(TEST_Objects): ../../testTranslator $(srcdir)/$(@:.o=.java)
$(TEST_Objects): ../../testTranslator
	$(testTranslator) $(ROSE_FLAGS) -c $(srcdir)/$(@:.o=.java)

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

# Execute the java file using: java -Djava.library.path=. HelloWorld

check-local:
	@echo "Tests for Java."
#  Run this test explicitly since it has to be run using a specific rule and can't be lumped with the rest
#	These C programs must be called externally to the test codes in the "TESTCODES" make variable
	@$(MAKE) $(PASSING_TEST_Objects)
	@echo "************************************************************************************************"
	@echo "****** ROSE/tests/CompileTests/Java_tests: make check rule complete (terminated normally) ******"
	@echo "************************************************************************************************"
else
check-local:
	@echo "Tests for Java."
	@echo "************************************************************************************************"
	@echo "****** ROSE/tests/CompileTests/Java_tests: JAVA NOT CONFIGURED IN ROSE ******"
	@echo "************************************************************************************************"
endif

EXTRA_DIST = $(ALL_TESTCODES)

clean-local:
	rm -f *.dot *.class rose_*.java rose_performance_report_lockfile.lock *.out

