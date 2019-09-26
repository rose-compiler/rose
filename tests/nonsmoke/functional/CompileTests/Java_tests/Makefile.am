include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

SUBDIRS = \
	unit_tests

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
     test2011_19.java \
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
     test2011_50.java \
     pgc_2011_30.java \
     pgc_test2011_00.java \
     pgc_test2011_01.java \
     pgc_test2011_02.java \
     pgc_test2011_03.java \
     pgc_test2011_04.java \
     pgc_test2011_05.java \
     pgc_test2011_06.java \
     pgc_test2011_07.java \
     pgc_test2011_08.java \
     pgc_test2011_10.java \
     pgc_test2011_11.java \
     pgc_test2011_12.java \
     pgc_test2011_13.java \
     pgc_test2011_14.java \
     pgc_test2011_15.java \
     pgc_test2011_16.java \
     pgc_test2011_17.java \
     pgc_test2011_18.java \
     pgc_test2011_19.java \
     pgc_test2011_20.java \
     pgc_test2011_21.java \
     \
     cave3_assert2.java \
     cave3_assert3.java \
     cave3_IntLit0.java \
     cave3_DoubleLit0.java \
     cave3_call0.java \
     cave3_call1.java \
     cave3_dowhile0.java \
     cave3_LoadDep0.java \
     cave3_block0.java \
     cave3_constructor0.java \
     cave3_constructor1.java \
     cave3_constructor2.java \
     cave3_constructor3.java \
     cave3_for0.java \
     cave3_for1.java \
     cave3_for2.java \
     cave3_for3.java \
     cave3_for4.java \
     cave3_for5.java \
     cave3_for6.java \
     cave3_for7.java \
     cave3_for8.java \
     cave3_if0.java \
     cave3_if1.java \
     cave3_import0.java \
     cave3_import1.java \
     cave3_instanceof0.java \
     cave3_labelDecl0.java \
     cave3_labelDecl1.java \
     cave3_labelDecl2.java \
     cave3_labelDecl3.java \
     cave3_labelBreak0.java \
     cave3_labelBreak1.java \
     cave3_labelBreak2.java \
     cave3_labelBreak5.java \
     cave3_labelContinue0.java \
     cave3_labelContinue1.java \
     cave3_labelContinue2.java \
     cave3_labelContinue3.java \
     cave3_labelContinueBreak0.java \
     cave3_labelContinueBreak1.java \
     cave3_mthParam0.java \
     cave3_nameResolution0.java \
     cave3_rshift0.java \
     cave3_rshift1.java \
     cave3_synchronized0.java \
     cave3_while0.java \
     cave3_while1.java \
     cave3_while2.java \
     cave3_while3.java \
     cave3_while4.java \
     testQualifiedType.java

#
# These test pass the compilation using ROSE, all AST tests, unparsing to generate 
# Java code, and the final compilation of that code using a backend compiler (javac).
TESTCODE_CURRENTLY_FAILING = 

# These tests pass the compilation using ROSE, all AST tests, and unparsing to generate 
# Java code, but fails the final compilation step of the generated Java code.
TESTCODES_REQUIRED_TO_PASS_FAILING_BACKEND_COMPILE = \
     simple.java

# This is where the Java generics tests are put (currently not required to pass all phases).
TESTCODES_REQUIRED_TO_PASS_FOR_JAVA_6 = \
     test2011_44.java \
     test2011_45.java \
     \
     cave3_assert0.java \
     cave3_assert1.java \
     cave3_import2.java \
     cave3_import3.java \
     cave3_foreach0.java \
     cave3_foreach1.java \
     cave3_foreach3.java \
     \
     InterfaceWithGenericMethod.java

TESTCODES_REQUIRED_TO_PASS_FAILING_FOR_JAVA_6 = 

TESTCODES_REQUIRED_TO_PASS_FOR_JAVA_7 = \
	Test_1_7_binary_literals.java \
	Test_1_7_non_reifiable_formal_parameters.java \
	Test_1_7_strings_in_switch_statement.java \
	Test_1_7_underscores_in_numeric_literal.java \
	14.20.3.try-with-resources/TryWithResources001.java

# Later we will move these tests to the gcjTestSuite subdirectory.
GCJ_TESTCODES_REQUIRED_TO_PASS = \
anfi.java \
anon2.java \
anon3.java \
anon4.java \
anonarray2.java \
anonarray3.java \
anonarray.java \
anon_ctor_itf_arg.java \
anon.java \
Array_1.java \
Array_2.java \
Array_3.java \
ArrayStore2.java \
ArrayStore.java \
assign2.java \
assign.java \
bclink.java \
bytearray.java \
Class_1.java \
CompareNaN.java \
direct_read.java \
direct_write.java \
Divide_1.java \
Divide_2.java \
emptystring.java \
err10.java \
err11.java \
err12.java \
err13.java \
err14.java \
err1.java \
err2.java \
err3.java \
err4.java \
err5.java \
err6.java \
err7.java \
err8.java \
err9.java \
EvaluationOrder.java \
ExtraClassLoader.java \
FileHandleGcTest.java \
final_inner.java \
final_int.java \
Final.java \
final_static_and_friend.java \
Float_1.java \
Float_2.java \
G19990301_01.java \
G19990302_02.java \
G19990303_01.java \
G19990303_02.java \
G19990304_01.java \
G19990310_01.java \
II.java \
indirect.java \
indirect_read.java \
indirect_write.java \
initexc.java \
initfield.java \
inline.java \
inner1.java \
inner2.java \
inner3.java \
inner4.java \
inner_array.java \
inner_interface.java \
instance.java \
instinit2.java \
instinit.java \
InterfaceDispatch.java \
Invoke_1.java \
Invoke_2.java \
invoke_from_inner.java \
InvokeInterface.java \
InvokeReturn.java \
invokethrow.java \
KeepInline.java \
klass.java \
LargeFile.java \
MathBuiltin.java \
Matrix4f.java \
md5test.java \
multiple_finit.java \
N19990310_02.java \
N19990310_3.java \
N19990310_4.java \
N19990310_5.java \
negzero.java \
nested_with_ctor.java \
newarray_overflow.java \
Overflow.java \
override.java \
pr100.java \
pr109.java \
PR12350.java \
PR12416.java \
PR12656.java \
PR12915.java \
pr13107_2.java \
pr13107_3.java \
pr13107.java \
pr133.java \
PR141.java \
PR160.java \
PR162.java \
pr16789.java \
PR16867.java \
pr17536.java \
pr179.java \
pr184.java \
PR18699.java \
PR19870_2.java \
PR19870.java \
PR19921.java \
PR20056.java \
pr21785.java \
pr21844.java \
PR218.java \
pr22211.java \
PR242.java \
pr24321.java \
PR25535.java \
pr25676.java \
PR260.java \
pr26390.java \
PR26858.java \
pr26990.java \
pr27171.java \
PR27908.java \
PR29013.java \
PR29495.java \
PR3096.java \
PR31264.java \
PR36252.java \
PR3731.java \
PR5057_2.java \
PR5057.java \
PR55.java \
PR56.java \
PR6085.java \
PR6204.java \
pr6388.java \
PR6520.java \
PR6729.java \
PR6820.java \
PR7482.java \
pr83.java \
pr8415.java \
pr8676.java \
pr8823.java \
PR8866.java \
pr8945.java \
private_direct_read.java \
private_direct_write.java \
private_indirect_read.java \
private_indirect_write.java \
Process_1.java \
Process_2.java \
Process_3.java \
Process_4.java \
Process_5.java \
Process_6.java \
Process_7.java \
ProxyTest.java \
RH194522.java \
search_outer.java \
Serialization.java \
shatest.java \
Shazam.java \
StackTrace2.java \
stacktrace.java \
StaticConstructor.java \
StringBuffer_1.java \
StringBuffer_overflow.java \
stringconst2.java \
stringconst.java \
String_overflow.java \
stub.java \
SyncGlobal.java \
Synch.java \
SyncTest.java \
test_long.java \
TestProxy.java \
Thread_Alive.java \
Thread_HoldsLock.java \
Thread_Interrupt.java \
Thread_Join.java \
Thread_Monitor.java \
Thread_Sleep_2.java \
Thread_Sleep.java \
Thread_Wait_2.java \
Thread_Wait_Interrupt.java \
Thread_Wait.java \
Throw_1.java \
Throw_2.java \
Throw_3.java \
TLtest.java \
tmi.java \
tp.java \
update_outer.java \
utf8concat.java \
utilTest.java \
verify.java 

GCJ_TESTCODE_CURRENTLY_FAILING = \
   PR35020.java \
   WalkerTest.java


if !ROSE_USING_GCC_VERSION_LATER_4_9
TESTCODES_REQUIRED_TO_PASS += \
     test2011_18.java \
     test2011_20.java 

TESTCODES_REQUIRED_TO_PASS_FOR_JAVA_6 += \
     test2011_43.java \
     test2011_46.java \
     test2011_48.java \
     test2011_49.java \
     \
     pgc_test2011_09.java \
     \
     cave3_foreach2.java \
     cave3_foreach4.java \
     cave3_foreach5.java \
     cave3_foreach6.java \
     cave3_foreach7.java \
     cave3_labelBreak4.java 

 
TESTCODES_REQUIRED_TO_PASS_FOR_JAVA_7 += \
	Test_1_7_Exceptions.java \
	Test_1_7_warnings_and_errors_for_non_reifiable_parameters.java \
	Test_1_7_type_inference_for_generic_instance_creation.java \
	Test_1_7_try_with_resources_statement.java 

endif

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

# Add tests for where we can compiler and generate code, but the generated code does not yet compiler with the backend compiler (javac).
TESTCODES_REQUIRED_TO_PASS_FAILING_BACKEND_COMPILE_Objects = ${TESTCODES_REQUIRED_TO_PASS_FAILING_BACKEND_COMPILE:.java=.o}

# Tests for Java generics
TESTCODES_REQUIRED_TO_PASS_FOR_JAVA_6_Objects = ${TESTCODES_REQUIRED_TO_PASS_FOR_JAVA_6:.java=.o}
TESTCODES_REQUIRED_TO_PASS_FOR_JAVA_7_Objects = ${TESTCODES_REQUIRED_TO_PASS_FOR_JAVA_7:.java=.o}

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
	@echo "Test using Java 1.4 version support."
	$(testTranslator) -rose:java:source 1.4 -rose:java:target 1.4 $(ROSE_FLAGS) $(srcdir)/$(@:.o=.java)
	@echo "Test DOT file graph generation (default) version support."
#	$(testGraphGeneration) $(ROSE_FLAGS) $(srcdir)/$(@:.o=.java)

# Tests that pass all but the final compilation using the backend compiler (javac)
$(TESTCODES_REQUIRED_TO_PASS_FAILING_BACKEND_COMPILE_Objects): $(testTranslator) ../../testTranslator ../../testGraphGeneration
	@echo "Test using Java 1.6 (default) version support."
	$(testTranslator) $(ROSE_FLAGS_SKIP_FINAL_COMPILATION) $(srcdir)/$(@:.o=.java)
	@echo "Test using Java 1.4 version support."
	$(testTranslator) -rose:java:source 1.4 -rose:java:target 1.4 $(ROSE_FLAGS_SKIP_FINAL_COMPILATION) $(srcdir)/$(@:.o=.java)

# Tests requiring Java 1.6 that should pass.
$(TESTCODES_REQUIRED_TO_PASS_FOR_JAVA_6_Objects): $(testTranslator) ../../testTranslator ../../testGraphGeneration
	@echo "Test using Java 1.6 (default) version support."
	$(testTranslator) -rose:java:source 1.6 -rose:java:target 1.6 $(ROSE_FLAGS) $(srcdir)/$(@:.o=.java)

# Tests requiring Java 1.7 that should pass.
$(TESTCODES_REQUIRED_TO_PASS_FOR_JAVA_7_Objects): $(testTranslator) ../../testTranslator ../../testGraphGeneration
	@echo "Test using Java 1.7 version support."
	$(testTranslator) -rose:java:source 1.7 -rose:java:target 1.7 $(ROSE_FLAGS) $(srcdir)/$(@:.o=.java)


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


# DQ (8/14/2011): Rule to build whole AST graphs with all of the file info objects.
# This is useful for debugging the source code position information. We have to 
# skip the final compilation because the -rose:dotgraph:fileInfoFilter will be passed
# to the backend compiler and cause an error.
# CAVE3 disable that for now in the interest of time
graphWithFileInfo: ../../testGraphGeneration
#	../../testGraphGeneration -rose:dotgraph:fileInfoFilter $(ROSE_FLAGS_SKIP_FINAL_COMPILATION) $(srcdir)/test2011_39.java

testGenerics: $(TESTCODES_REQUIRED_TO_PASS_FOR_JAVA_6_Objects) $(TESTCODES_REQUIRED_TO_PASS_FOR_JAVA_7_Objects)

workingJava:
	java -cp $(top_srcdir)/src/3rdPartyLibraries/java-parser/ecjROSE.jar

testECJ:
	java -cp ecjROSE.jar org.eclipse.jdt.internal.compiler.batch.Main $(srcdir)/simple.java

# Test for help option to ECJ (some ECJ options not supported via ROSE).
# DQ (7/21/2011): Currently a file must be specified, but this should be fixed later.
testECJ_help_option: $(testTranslator)
	$(testTranslator) -rose:java -rose:verbose 0 -rose:skipfinalCompileStep -ecj:help -c $(srcdir)/simple.java

# Test with the "-rose:java" specification (one way to specify the language in ROSE).
testWithJavaSpecification: $(testTranslator)
	$(testTranslator) -rose:java $(ROSE_FLAGS) $(srcdir)/testWithJavaSpecification.java

# Test without the "-rose:java" specification (ROSE can alternatively obtain the source language from the file suffix).
testWithoutJavaSpecification: $(testTranslator)
	$(testTranslator) $(ROSE_FLAGS) $(srcdir)/testWithoutJavaSpecification.java

# Test using the specification of Java 1.7 compatability.
testJava_option: $(testTranslator)
	$(testTranslator) -rose:java:source 1.6 -rose:java:target 1.6 -rose:verbose 0 -rose:skipfinalCompileStep -c $(srcdir)/test2011_44.java

#-------------------------------------------------------------------------------
# TOO1 (2/6/2014): Won't fail in some cases, e.g. GCC's "unrecognized option" message
#                  will only fail in version 4.6+.
#                  (See http://gcc.gnu.org/bugzilla/show_bug.cgi?id=15303)
#-------------------------------------------------------------------------------
testJavaCommandlineOptions: $(testTranslator)
	mkdir -p "$(builddir)/tmp"
#-----------------------------
# Java only
#-----------------------------
	$(testTranslator) "$(top_srcdir)/tests/nonsmoke/functional/input_codes/minimal/minimal.java" -rose:java
#-----------------------------
# Classpath
#-----------------------------
	$(testTranslator) "$(top_srcdir)/tests/nonsmoke/functional/input_codes/minimal/minimal.java" -rose:java:classpath "/path/1:/path/2"
	$(testTranslator) "$(top_srcdir)/tests/nonsmoke/functional/input_codes/minimal/minimal.java" -rose:java:cp "/path/1:/path/2"
	$(testTranslator) "$(top_srcdir)/tests/nonsmoke/functional/input_codes/minimal/minimal.java" -classpath "/path/1:/path/2"
	$(testTranslator) "$(top_srcdir)/tests/nonsmoke/functional/input_codes/minimal/minimal.java" -cp "/path/1:/path/2"
#-----------------------------
# Sourcepath
#-----------------------------
	$(testTranslator) "$(top_srcdir)/tests/nonsmoke/functional/input_codes/minimal/minimal.java" -rose:java:sourcepath "/path/1:/path/2"
	$(testTranslator) "$(top_srcdir)/tests/nonsmoke/functional/input_codes/minimal/minimal.java" -sourcepath "/path/1:/path/2"
#-----------------------------
# Destdir
#-----------------------------
	mkdir -p "$(builddir)/tmp/destdir"
	$(testTranslator) "$(top_srcdir)/tests/nonsmoke/functional/input_codes/minimal/minimal.java" -rose:java:d "$(builddir)/tmp/destdir"
	$(testTranslator) "$(top_srcdir)/tests/nonsmoke/functional/input_codes/minimal/minimal.java" -d "$(builddir)/tmp/destdir"
#-----------------------------
# Source Destdir
#-----------------------------
	mkdir -p "$(builddir)/tmp/source_destdir"
	$(testTranslator) "$(top_srcdir)/tests/nonsmoke/functional/input_codes/minimal/minimal.java" -rose:java:ds "$(builddir)/tmp/source_destdir"
#-----------------------------
# Source
#-----------------------------
	$(testTranslator) "$(top_srcdir)/tests/nonsmoke/functional/input_codes/minimal/minimal.java" -source 1.5
	$(testTranslator) "$(top_srcdir)/tests/nonsmoke/functional/input_codes/minimal/minimal.java" -source 1.6
	$(testTranslator) "$(top_srcdir)/tests/nonsmoke/functional/input_codes/minimal/minimal.java" -source 1.7
#-----------------------------
# Target
#-----------------------------
	$(testTranslator) "$(top_srcdir)/tests/nonsmoke/functional/input_codes/minimal/minimal.java" -target 1.6 -source 1.6
	$(testTranslator) "$(top_srcdir)/tests/nonsmoke/functional/input_codes/minimal/minimal.java" -target 1.7 -source 1.7
#-----------------------------
# Encoding
#-----------------------------
	$(testTranslator) "$(top_srcdir)/tests/nonsmoke/functional/input_codes/minimal/minimal.java" -encoding UTF8
	$(testTranslator) "$(top_srcdir)/tests/nonsmoke/functional/input_codes/minimal/minimal.java" -encoding ISO8859-1
	$(testTranslator) "$(top_srcdir)/tests/nonsmoke/functional/input_codes/minimal/minimal.java" -encoding SJIS
	$(testTranslator) "$(top_srcdir)/tests/nonsmoke/functional/input_codes/minimal/minimal.java" -encoding EUCJIS
#-----------------------------
# -s
#-----------------------------
	mkdir -p "$(builddir)/tmp/generated_source_destdir" && \
			$(testTranslator) "$(top_srcdir)/tests/nonsmoke/functional/input_codes/minimal/minimal.java" -s "$(builddir)/tmp/generated_source_destdir"
#-----------------------------
# -nowarn
#-----------------------------
	$(testTranslator) "$(top_srcdir)/tests/nonsmoke/functional/input_codes/minimal/minimal.java" -nowarn
#-----------------------------
# -verbose
#-----------------------------
	$(testTranslator) "$(top_srcdir)/tests/nonsmoke/functional/input_codes/minimal/minimal.java" -verbose
#-----------------------------
# @argfile
#-----------------------------
	echo "$(top_srcdir)/tests/nonsmoke/functional/input_codes/minimal/minimal.java" > argfile.txt && \
			$(testTranslator) @argfile.txt
#-----------------------------
# JVM Options
#-----------------------------
# Increase ECJ's JVM stack size
	$(testTranslator) "$(top_srcdir)/tests/nonsmoke/functional/input_codes/minimal/minimal.java" -rose:java:ecj:jvm_options "-Xss5m"
#-----------------------------
# Remote JVM Debugging
# TOO1 (2/6/2014): Can't perform this test since it is interactive
#-----------------------------
#	$(testTranslator) "$(top_srcdir)/tests/nonsmoke/functional/input_codes/minimal/minimal.java" -rose:java:ecj:enable_remote_debugging

# Build a rule to allow running all tests in parallel...
extraTests: testWithJavaSpecification testWithoutJavaSpecification testECJ_help_option testJava_option testJavaCommandlineOptions graphWithFileInfo

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
# DQ (2/19/2017): NEW VERSION OF LOGIC
# DQ (2/18/2017): More specific logic required for portability.
# When using EDG 4.9 these files don't compile on later versions of GNU, CLANG, and Intel compilers 
# but we still want them tests as we transition to EDG 4.12 (where they all work fine independent of 
# the compiler vendor and GNU compiler version).
# Jave support is independent of EDG 4.9 or EDG 4.12 or later (run all tests independent of the C/C++ frontend).
	mkdir -p "$(builddir)/javac-output-classes"
if USING_GNU_COMPILER
#if !ROSE_USING_GCC_VERSION_LATER_4_9
	@echo "Some specific Java tests are skipped with any later GNU compiler (GNU 4.9 or later)."
# conditional-check-local: extraTests
	@$(MAKE) extraTests
	@echo "Tests for Java."
#  Run this test explicitly since it has to be run using a specific rule and can't be lumped with the rest
#	These C programs must be called externally to the test codes in the "TESTCODES" make variable
	@$(MAKE) $(TESTCODES_REQUIRED_TO_PASS_FAILING_BACKEND_COMPILE_Objects)
	@$(MAKE) $(PASSING_TEST_Objects)
	@$(MAKE) $(TESTCODES_REQUIRED_TO_PASS_FOR_JAVA_6_Objects)
	@$(MAKE) $(TESTCODES_REQUIRED_TO_PASS_FOR_JAVA_7_Objects)
#endif # NOT ROSE_USING_GCC_VERSION_LATER_4_9
else
if USING_CLANG_COMPILER
# Exclude this list of files in the case of CLANG compiler.
	@echo "USING_CLANG_COMPILER == true"
else # NOT USING_CLANG_COMPILER
	@echo "USING_CLANG_COMPILER == false"
if USING_INTEL_COMPILER
# Exclude this list of files in the case of Intel compiler.
	@echo "USING_INTEL_COMPILER == true"
else # NOT USING_INTEL_COMPILER
# This is an unknown compiler.
	@echo "USING_INTEL_COMPILER == false"
endif # USING_INTEL_COMPILER
endif # USING_CLANG_COMPILER
endif # USING_GNU_COMPILER
	@echo "************************************************************************************************"
	@echo "****** ROSE/tests/nonsmoke/functional/CompileTests/Java_tests: make check rule complete (terminated normally) ******"
	@echo "************************************************************************************************"
else
conditional-check-local:
	@echo "Tests for Java."
	@echo "**********************************************************************************************"
	@echo "****** ROSE/tests/nonsmoke/functional/CompileTests/Java_tests: JAVA LANGUAGE SUPPORT NOT CONFIGURED IN ROSE ******"
	@echo "**********************************************************************************************"
endif

EXTRA_DIST = $(ALL_TESTCODES) testWithJavaSpecification.java testWithoutJavaSpecification.java gcjTestSuite 

clean-local:
	rm -f *.dot *.class *.java rose_performance_report_lockfile.lock *.out
	rm -fr rose-output* ecj-classes-* javac-output-classes javac-syntax-check-classes
#	Remove any generated JVM log files that could have been generated (these are rare).
	rm -f *.log
	rm -f core.*
	rm -rf ./tmp

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
