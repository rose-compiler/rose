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
#	@echo "In extract_rose_required_ecj_source_files: After building JavaTraversal.java, copy ecj-3.8.2.jar from srcdir to builddir"
#	cp ${srcdir}/ecjROSE.jar ecj-3.8.2.jar

# buildJavaSupport: ${srcdir}/JavaTraversal.java
# JavaTraversal.class:
JavaTraversal.class: ${srcdir}/JavaTraversal.java ${srcdir}/JavaParser.java ${srcdir}/JavaParserSupport.java ${srcdir}/ExtendedASTVisitor.java ${srcdir}/ecjASTVisitor.java ${srcdir}/JavaToken.java 
	$(JAVAC) -d . -cp ${srcdir}/ecj-3.8.2.jar:${srcdir}:. ${srcdir}/JavaTraversal.java ${srcdir}/JavaParser.java ${srcdir}/JavaParserSupport.java ${srcdir}/ExtendedASTVisitor.java ${srcdir}/ecjASTVisitor.java ${srcdir}/JavaToken.java ${srcdir}/JavaSourcePositionInformation.java  ${srcdir}/JavaSourcePositionInformationFactory.java 
#	DQ (11/23/2010): javah only requires the class file which are in the build directory.
	$(JAVAH) -jni -classpath ${srcdir}/ecj-3.8.2.jar:. JavaTraversal
	$(JAVAH) -jni -classpath ${srcdir}/ecj-3.8.2.jar:. JavaParser
#	DQ (10/13/2010): I want to avoid copying ecj-3.8.2.jar into the build tree, but it is required for now.
#	Only ROSE translators need it, not the javac and javah commands (above).
	@echo "After building JavaTraversal.java, copy ecj-3.8.2.jar from srcdir to builddir (only required for ROSE to find ecj-3.8.2.jar (fix later))"
	(test -f ecj-3.8.2.jar) || cp ${srcdir}/ecj-3.8.2.jar ecj-3.8.2.jar

# I assume this is needed the same as for the JavaParserSupport.class rule
JavaParser.class: ${srcdir}/JavaParser.java
	$(JAVAC) -cp ${srcdir}/ecj-3.8.2.jar:${srcdir}:. ${srcdir}/JavaParser.java
	$(JAVAH) -jni -classpath ${srcdir}/ecj-3.8.2.jar:. JavaParser

JavaParser.h: ${srcdir}/JavaParser.java
	$(JAVAC) -cp ${srcdir}/ecj-3.8.2.jar:${srcdir}:. ${srcdir}/JavaParser.java
	$(JAVAH) -jni -classpath ${srcdir}/ecj-3.8.2.jar:. JavaParser

# This is needed when distcheck compiles the code, I don't know why.
JavaParserSupport.class: ${srcdir}/JavaParserSupport.java
	$(JAVAC) -cp ${srcdir}/ecj-3.8.2.jar:${srcdir}:. ${srcdir}/JavaParserSupport.java

# I assume this is needed the same as for the JavaParserSupport.class rule
ecjASTVisitor.class:  ${srcdir}/ExtendedASTVisitor.java ${srcdir}/ecjASTVisitor.java
	$(JAVAC) -cp ${srcdir}/ecj-3.8.2.jar:${srcdir}:. ${srcdir}/ExtendedASTVisitor.java ${srcdir}/ecjASTVisitor.java

# The file JavaTraversal.C is no longer used.  It #includes JavaTraversal.h which can
# no longer be built with openjdk (at least by version 12.0).  Thus JavaTraversal.h
# does not need to be created [CR 2019.05.05].
JavaTraversal.class: ${srcdir}/JavaTraversal.java
	$(JAVAC) -cp ${srcdir}/ecj-3.8.2.jar ${srcdir}/JavaTraversal.java
#	$(JAVAH) -jni JavaTraversal

# DQ (4/16/2011): Added support for Java specific tokens (to support source position information in ROSE).
JavaToken.class: ${srcdir}/JavaToken.java
	$(JAVAC) -cp ${srcdir}/ecj-3.8.2.jar:${srcdir}:. ${srcdir}/JavaToken.java

# DQ (4/16/2011): Added support for Java specific tokens (to support source position information in ROSE).
JavaSourcePositionInformation.class: ${srcdir}/JavaSourcePositionInformation.java
	$(JAVAC) -cp ${srcdir}/ecj-3.8.2.jar:${srcdir}:. ${srcdir}/JavaSourcePositionInformation.java

# charles4: (2/23/2012): 
JavaSourcePositionInformationFactory.class: ${srcdir}/JavaSourcePositionInformationFactory.java
	$(JAVAC) -cp ${srcdir}/ecj-3.8.2.jar:${srcdir}:. ${srcdir}/JavaSourcePositionInformationFactory.java

EXTRA_DIST = CMakeLists.txt ecj-3.8.2.jar JavaTraversal.java JavaParser.java JavaParserSupport.java ExtendedASTVisitor.java ecjASTVisitor.java JavaToken.java JavaSourcePositionInformation.java JavaSourcePositionInformationFactory.java UnitInfo.java

install-data-local:
	$(INSTALL_DATA) $(srcdir)/ecj-3.8.2.jar $(libdir)/ecj-3.8.2.jar
	$(INSTALL_DATA) *.class $(libdir)

clean-local:
	rm -f *.dot ecj-3.8.2.jar *.class *.java *.h

# JavaParser* JavaTraversal* JavaParserSupport* ecjASTVisitor* ExtendedASTVisitor*
