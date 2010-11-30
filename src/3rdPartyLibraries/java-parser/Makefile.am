javadir=$(libdir)

# Liao 8/4/2010, based on Jean Utke's suggestion on [Rose-public] Fortran built-ins
if ROSE_BUILD_JAVA_LANGUAGE_SUPPORT
  include_HEADERS = 
endif

# Put in a target so that this can be called by default to build the Java/ROSE AST translation support.
all: extract_rose_required_ecj_source_files

# This rule is called from the ECJ_ROSE_Connection/Makefile.am
# extract_rose_required_ecj_source_files: buildJavaSupport
# extract_rose_required_ecj_source_files: ${srcdir}/JavaTraversal.java
extract_rose_required_ecj_source_files: JavaTraversal.class
#	@echo "In extract_rose_required_ecj_source_files: After building JavaTraversal.java, copy ecjROSE.jar from srcdir to builddir"
#	cp ${srcdir}/ecjROSE.jar ecjROSE.jar
	@echo "In extract_rose_required_ecj_source_files: output the directory listing."
	@pwd
	@echo "calling ls -l --full-time"
	ls -l 
	@echo "after ls"

# buildJavaSupport: ${srcdir}/JavaTraversal.java
# JavaTraversal.class:
JavaTraversal.class: ${srcdir}/JavaTraversal.java ${srcdir}/JavaParser.java ${srcdir}/JavaParserSupport.java ${srcdir}/ecjASTVisitor.java
	$(JAVAC) -d . -cp ${srcdir}/ecjROSE.jar:${srcdir}:. ${srcdir}/JavaTraversal.java ${srcdir}/JavaParser.java ${srcdir}/JavaParserSupport.java ${srcdir}/ecjASTVisitor.java
#	javah only requires the class file which are in the build directory.
	$(JAVAH) -jni -classpath ${srcdir}/ecjROSE.jar:. JavaTraversal
	$(JAVAH) -jni -classpath ${srcdir}/ecjROSE.jar:. JavaParser
#	DQ (10/13/2010): I want to avoid copying ecjROSE.jar into the build tree, but it is required for now.
#	Only ROSE translators need it, not the javac and javah commands (above).
	@echo "After building JavaTraversal.java, copy ecjROSE.jar from srcdir to builddir (only required for ROSE to find ecjROSE.jar (fix later))"
	cp ${srcdir}/ecjROSE.jar ecjROSE.jar

# I assume this is need the same as for the JavaParserSupport.class rule
JavaParser.class: ${srcdir}/JavaParser.java
	$(JAVAC) -cp ${srcdir}/ecjROSE.jar:${srcdir}:. JavaParser.java
	$(JAVAH) -jni -classpath ${srcdir}/ecjROSE.jar:. JavaParser

# This is needed when distcheck compiles the code, I don't know why.
JavaParserSupport.class: ${srcdir}/JavaParserSupport.java
	$(JAVAC) -cp ${srcdir}/ecjROSE.jar:${srcdir}:. JavaParserSupport.java

# I assume this is need the same as for the JavaParserSupport.class rule
ecjASTVisitor.class: ${srcdir}/ecjASTVisitor.java
	$(JAVAC) -cp ${srcdir}/ecjROSE.jar:${srcdir}:. ecjASTVisitor.java

# These are the commands to generate the files we need (in the build tree).
# JavaTraversal.class JavaTraversal.h: ${srcdir}/JavaTraversal.java
testJavaTraversal: ${srcdir}/JavaTraversal.java
	javac -cp ${srcdir}/ecjROSE.jar ${srcdir}/JavaTraversal.java
	javah -jni JavaTraversal


EXTRA_DIST = CMakeLists.txt ecjROSE.jar JavaTraversal.java JavaParser.java JavaParserSupport.java ecjASTVisitor.java

clean-local:
	rm -f *.dot ecjROSE.jar *.class *.java *.h

# JavaParser* JavaTraversal* JavaParserSupport* ecjASTVisitor* 



