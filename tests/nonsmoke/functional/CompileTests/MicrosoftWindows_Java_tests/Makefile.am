include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

##################################################################################
# These are C testcodes targeting aspects of C that might not be a subset of C++ #
##################################################################################

# These test pass the compilation using ROSE, all AST tests, unparsing to generate 
# Java code, and the final compilation of that code using a backend compiler (javac).
# Liao 6/12/2011, temporarily commented out one test since it failed randomly
# See details at https://github.com/rose-compiler/rose/issues/5
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
     test2011_14.java \
     test2011_15.java \
     test2011_16.java \
     test2011_17.java \
     test2011_18.java \
     test2011_19.java \
     test2011_20.java \
     test2011_21.java \
     test2011_22.java \
     test2011_23.java \
     test2011_24.java \
     test2011_25.java \
     test2011_26.java \
     test2011_27.java \
     test2011_29.java \
     test2011_30.java \
     test2011_31.java \
     test2011_32.java \
     test2011_33.java \
     test2011_34.java \
     test2011_35.java \
     test2011_36.java \
     test2011_37.java \
     test2011_38.java \
     test2011_39.java \
     test2011_40.java \
     test2011_41.java \
     test2011_42.java \
     test2011_47.java \
     test2011_50.java

#
# These test pass the compilation using ROSE, all AST tests, unparsing to generate 
# Java code, and the final compilation of that code using a backend compiler (javac).
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
# ROSE_FLAGS = -rose:verbose 0 -rose:skipfinalCompileStep -rose:skip_syntax_check
# ROSE_FLAGS = -rose:verbose 0 -rose:skipfinalCompileStep
ROSE_FLAGS = -rose:java:d javac-output-classes -rose:verbose 0
ROSE_FLAGS_SKIP_FINAL_COMPILATION = -rose:verbose 0 -rose:skipfinalCompileStep


PASSING_TEST_Objects = ${TESTCODES:.java=.o}
TEST_Objects = ${ALL_TESTCODES:.java=.o}


if ROSE_BUILD_JAVA_LANGUAGE_SUPPORT

testTranslator=../../testTranslator
# testTranslator=../../testGraphGeneration

../../testTranslator:
	cd ../..; $(MAKE) testTranslator

# This is broken, disable that for now in the interest of time.
testGraphGeneration=../../testGraphGeneration
../../testGraphGeneration:
	cd ../..; $(MAKE) testGraphGeneration

# Add the dependence upon the source file to trigger recompilation each time the makefile is run!
# $(TEST_Objects): ../../testTranslator $(srcdir)/$(@:.o=.java)
$(TEST_Objects): $(testTranslator) ../../testTranslator ../../testGraphGeneration
	@echo "Test using Java 1.6 (default) version support."
	$(testTranslator) $(ROSE_FLAGS) $(srcdir)/$(@:.o=.java)
#	@echo "Test using Java 1.4 version support."
#	$(testTranslator) -rose:java:source 1.4 -rose:java:target 1.4 $(ROSE_FLAGS) $(srcdir)/$(@:.o=.java)
#	@echo "Test DOT file graph generation (default) version support."
#	$(testGraphGeneration) $(ROSE_FLAGS) $(srcdir)/$(@:.o=.java)





workingJava:
	java -cp $(top_srcdir)/src/3rdPartyLibraries/java-parser/ecjROSE.jar

testECJ:
	java -cp ecjROSE.jar org.eclipse.jdt.internal.compiler.batch.Main $(srcdir)/simple.java



# Execute the java file using: java -Djava.library.path=. HelloWorld

# ********************************************************************************
# ********************************************************************************
# Support for GCJ test codes (will be moved to the gcjTestSuite subdirectory soon.
# Likely it is too early to expect any of these to pass using ROSE's Java support.
GCJ_TESTCODES = $(GCJ_TESTCODES_REQUIRED_TO_PASS) $(GCJ_TESTCODE_CURRENTLY_FAILING)
gcjTests_passing_Objects = ${GCJ_TESTCODES_REQUIRED_TO_PASS:.java=.class}
gcjTests_Objects = ${GCJ_TESTCODES:.java=.class}

# Test the GCJ test code by only doing syntax checking on them using ROSE (calling only the 
# backend javac to do the syntax checking). This is more of a test of the Makefile than ROSE.
$(gcjTests_Objects): $(testTranslator)
	$(testTranslator) $(ROSE_FLAGS) -rose:exit_after_parser $(srcdir)/gcjTestSuite/$(@:.class=.java)

# This test evaluates the GCJ test code to make sure that they compile with ECJ (all but 2 pass).
testGCJ: $(gcjTests_passing_Objects)
# ********************************************************************************
# ********************************************************************************


conditional-check-local: 
	@echo "Tests for Microsoft Windows Java."
#  Run this test explicitly since it has to be run using a specific rule and can't be lumped with the rest
#	These C programs must be called externally to the test codes in the "TESTCODES" make variable
	@$(MAKE) $(PASSING_TEST_Objects)
	@echo "*****************************************************************************************************************"
	@echo "****** ROSE/tests/nonsmoke/functional/CompileTests/MicrosoftWindows_Java_tests: make check rule complete (terminated normally) ******"
	@echo "*****************************************************************************************************************"
else
conditional-check-local:
	@echo "Tests for Microsoft Windows Java."
	@echo "***************************************************************************************************************"
	@echo "****** ROSE/tests/nonsmoke/functional/CompileTests/MicrosoftWindows_Java_tests: JAVA LANGUAGE SUPPORT NOT CONFIGURED IN ROSE ******"
	@echo "***************************************************************************************************************"
endif

EXTRA_DIST = $(ALL_TESTCODES) testWithJavaSpecification.java testWithoutJavaSpecification.java gcjTestSuite 

clean-local:
	rm -f *.dot *.class *.java rose_performance_report_lockfile.lock *.out
	rm -fr rose-output* ecj-classes-* javac-output-classes javac-syntax-check-classes
#	Remove any generated JVM log files that could have been generated (these are rare).
	rm -f *.log

# driscoll6 (8/10/11) convenience target for inspecting unparser output from all test files
check-view: $(TEST_Objects:.o=.view)

# driscoll6 (8/10/11) convenience target for inspecting unparser output from a particular file
%.view: ../../testTranslator
	@echo "============ TRANSLATING ==========="
	$(testTranslator) $(ROSE_FLAGS) $(srcdir)/$(@:.view=.java) -rose:o $(@:.view=.java)
	@echo "------------- ORIGINAL -------------"
	@cat $(srcdir)/$(@:.view=.java)
	@echo "------------- UNPARSED -------------"
	@cat $(@:.view=.java)
	@echo "-------------- JAVAC ---------------"
	$(JAVAC) $(@:.view=.java)

# DQ (8/17/2011): This is a fix for a interesting problem caused by the VPATH make system.
# The generated java files will be copied from the current build directory instead of the
# source directory.
# Since the generated java files have the same name as in the srcdir, VPATH will
# cause the builddir versions to be copied to the distdir if they exist.  They
# will exist if the "make check" has been run, which is the case for the
# java only language tests.
dist-hook:
	@echo "Removing what might be the wrong versions of the files if make check has been run."
	rm $(distdir)/*.java
	@echo "Copying the correct versions of the java file from the srcdir = $(srcdir) to the distdir = $(distdir)"
	cp -p $(srcdir)/*.java $(distdir)

check-local: conditional-check-local
