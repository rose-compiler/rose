javadir=$(libdir)

# Liao 8/4/2010, based on Jean Utke's suggestion on [Rose-public] Fortran built-ins
if ROSE_USE_OPEN_FORTRAN_PARSER
# include_HEADERS = ActionEnums.h token.h FortranParserAction.h
  include_HEADERS = 
endif

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
JavaTraversal.class: ${srcdir}/JavaTraversal.java
	@echo "After building JavaTraversal.java, copy ecjROSE.jar from srcdir to builddir"
	cp ${srcdir}/ecjROSE.jar ecjROSE.jar
	@echo "copy the JavaTraversal.java from the srcdir to the build directory."
	cp ${srcdir}/JavaTraversal.java JavaTraversal.java
	@echo "Using the build directory's copy of ecjROSE.jar."
	javac -cp ecjROSE.jar JavaTraversal.java
	javah -jni JavaTraversal


# These are the commands to generate the files we need (in the build tree).
# JavaTraversal.class JavaTraversal.h: ${srcdir}/JavaTraversal.java
testJavaTraversal: ${srcdir}/JavaTraversal.java
	javac -cp ${srcdir}/ecjROSE.jar ${srcdir}/JavaTraversal.java
	javah -jni JavaTraversal


EXTRA_DIST = CMakeLists.txt ecjROSE.jar JavaTraversal.java

clean-local:
	rm -f OpenJavaParser-${ECJ_VERSION}.jar FortranParserAction.h c_actions_jni.c
	rm -f token.c token.h ActionEnums.h jni_token_funcs.h jni_token_funcs.c


