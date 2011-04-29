javadir=$(libdir)

OFP_VERSION=@ROSE_OFP_MAJOR_VERSION_NUMBER@.@ROSE_OFP_MINOR_VERSION_NUMBER@.@ROSE_OFP_PATCH_VERSION_NUMBER@

# DQ (4/5/2010): Think about if we need the test using an Automake conditional since the jar file always exists.
if ROSE_BUILD_FORTRAN_LANGUAGE_SUPPORT
   dist_java_DATA = OpenFortranParser-${OFP_VERSION}.jar
endif

EXTERNAL_OFP_JAR_FILE_URL = http://sourceforge.net/projects/fortran-parser/files/OpenFortranParser/version_${OFP_VERSION}/OpenFortranParser-${OFP_VERSION}.jar/download

TARGET_OFP_SOURCE_FILES = token.c jni_token_funcs.c c_actions_jni.c

# DQ (10/9/2010): If the jar file in the source tree is not present then use wget to fetch it from the OFP web site.
# DQ (3/13/2010): I think we need to use: "--no-check-certificate"
# Using --no-check-certificate to handle errors:
#    Resolving sgone.llnl.gov... 128.115.27.11
#    Connecting to sgone.llnl.gov|128.115.27.11|:443... connected.
#    ERROR: cannot verify sgone.llnl.gov's certificate, issued by `/C=US/O=VeriSign, Inc./OU=VeriSign Trust Network/OU=Terms 
#       of use at https://www.verisign.com/rpa (c)09/CN=VeriSign Class 3 Secure Server CA - G2': Unable to locally verify the issuer's authority.
#    To connect to sgone.llnl.gov insecurely, use `--no-check-certificate'.
#    Unable to establish SSL connection.
OpenFortranParser-${OFP_VERSION}.jar:
	@echo "Check the web for this specific version of OFP: OpenFortranParser-${OFP_VERSION}.jar"
	test -e ${srcdir}/OpenFortranParser-${OFP_VERSION}.jar || (wget --no-check-certificate ${EXTERNAL_OFP_JAR_FILE_URL} || exit 1;);
	@if test -e OpenFortranParser-${OFP_VERSION}.jar; then \
      echo "Found valid OpenFortranParser-${OFP_VERSION}.jar file."; \
   else \
      echo "Note: OpenFortranParser-${OFP_VERSION}.jar file is not found."; \
	   @if test -e ${srcdir}/OpenFortranParser-${OFP_VERSION}.jar; then \
         echo "Found valid ${srcdir}/OpenFortranParser-${OFP_VERSION}.jar file."; \
         cp ${srcdir}/OpenFortranParser-${OFP_VERSION}.jar OpenFortranParser-${OFP_VERSION}.jar; \
      else \
         echo "Error: ${srcdir}/OpenFortranParser-${OFP_VERSION}.jar file is not found."; \
      fi; \
   fi;

extract_rose_required_ofp_source_files: token.c
	@echo "files: extract_rose_required_ofp_source_files is out of date compared to token.c"

TEST_OFP_VERSION=0.8.1
TEST_EXTERNAL_OFP_JAR_FILE_URL=http://sourceforge.net/projects/fortran-parser/files/OpenFortranParser/version_${TEST_OFP_VERSION}/OpenFortranParser-${TEST_OFP_VERSION}.jar/download

# This is a rule for testing the download of OFP from the web.
test_get_new_jar_file:
	test -e OpenFortranParser-${TEST_OFP_VERSION}.jar || (wget --no-check-certificate ${TEST_EXTERNAL_OFP_JAR_FILE_URL} || exit 1;);

# DQ (4/4/2010): This needs to be an explicit path list for each file since some are all located in different subdirectories.
OFP_SOURCE_FILES = \
   fortran/ofp/parser/c/FortranParserAction.h \
   fortran/ofp/parser/c/jni/c_actions_jni.c \
   fortran/ofp/parser/c/token.h \
   fortran/ofp/parser/c/token.c \
   fortran/ofp/parser/c/ActionEnums.h \
   fortran/ofp/parser/c/jni/jni_token_funcs.h \
   fortran/ofp/parser/c/jni/jni_token_funcs.c

if ROSE_OFP_CONTAINS_JAVA_FILE
   OFP_SOURCE_FILES += fortran/ofp/parser/java/IFortranParserAction.java
endif

# DQ (4/5/2010): And we need a version of this macro withouth the paths.
OFP_SOURCE_FILES_WITHOUT_PATHS = \
   c_actions_jni.c \
   token.h \
   jni_token_funcs.h \
   jni_token_funcs.c 

# DQ (10/17/2010): This is the better way to control this.
if ROSE_BUILD_FORTRAN_LANGUAGE_SUPPORT
# Liao 8/4/2010, based on Jean Utke's suggestion on [Rose-public] Fortran built-ins
   include_HEADERS = ActionEnums.h token.h FortranParserAction.h
endif

OFP_DIR = $(top_builddir)/src/3rdPartyLibraries/fortran-parser
if !ROSE_BUILD_FORTRAN_LANGUAGE_SUPPORT
token.h:
	touch token.h

ActionEnums.h:
	touch ActionEnums.h

FortranParserAction.h:
	touch FortranParserAction.h
endif


# DQ (9/26/2010): Add the jar file as a dependency so that when it is 
# updated it will force the required files to be extracted automatically.
token.c: OpenFortranParser-${OFP_VERSION}.jar
	@echo "***********************************************************************"
	@echo "Extracting required OFP files from OpenFortranParser-${OFP_VERSION}.jar"
	@echo "***********************************************************************"
	@echo "Jar command being used: JAR = ${JAR}"
	@echo "Jar command being used: JAR = $(JAR)"
	@if test -e OpenFortranParser-${OFP_VERSION}.jar; then \
      echo "Found valid OpenFortranParser-${OFP_VERSION}.jar file."; \
   else \
      echo "Note: OpenFortranParser-${OFP_VERSION}.jar file was not found in the build tree (OK)."; \
      if test -e ${srcdir}/OpenFortranParser-${OFP_VERSION}.jar; then \
         echo "Found valid ${srcdir}/OpenFortranParser-${OFP_VERSION}.jar file."; \
         cp ${srcdir}/OpenFortranParser-${OFP_VERSION}.jar OpenFortranParser-${OFP_VERSION}.jar; \
      else \
         echo "Error: ${srcdir}/OpenFortranParser-${OFP_VERSION}.jar file is not found in the source tree or the build tree."; \
         exit 1; \
      fi; \
   fi;
#	DQ (10/13/2010): "jar" is not in the path of some system, but our configure tests 
#	for this and only passes if it is found (and sets the JAR macro, so use it).
#	jar xf OpenFortranParser-${OFP_VERSION}.jar $(OFP_SOURCE_FILES)
	$(JAR) xf OpenFortranParser-${OFP_VERSION}.jar $(OFP_SOURCE_FILES)
	@list='$(OFP_SOURCE_FILES)'; for f in $$list; do \
	   echo "building $$f file"; \
	   filename=`basename $$f`; \
      echo "filename = $$filename"; \
      if test -e $$f; then \
         echo "Using OFP source file $f from version $(OFP_VERSION) found in OpenFortranParser-${OFP_VERSION}.jar file."; \
         cp $$f $$filename; \
      else \
         echo "Warning: $$f is not found in jar file."; \
         if test "x$(OFP_VERSION)" = "x0.8.1"; then \
            echo "Using OFP source file $$f from version $(OFP_VERSION) distributed with ROSE."; \
            if test -e $$filename; then \
               echo "Note: OFP source file $$filename is already present."; \
            else \
               echo "Copying OFP source file $$filename from source tree to build tree."; \
               cp ${srcdir}/$$filename $$filename; \
            fi; \
         else \
            echo "Error: can't use default (version 0.8.1) OFP source file $$filename with OpenFortranParser-${OFP_VERSION}.jar file."; \
            exit 1; \
         fi; \
      fi; \
   done
#	Remove the fortran directory generated from the extraction of files from the jar file.
	@if test -d fortran; then \
      rm -rf fortran; \
   fi;

# DQ (10/4/2010): We only want to distribute the jar file for OFP, and then
# extract what we require from the jar file (putting it into the build tree).
EXTRA_DIST = CMakeLists.txt OpenFortranParser-${OFP_VERSION}.jar


check-local:

# DQ (10/11/2010): Fixing bug in "make distcheck" rule.
# The files (token.c token.h ActionEnums.h jni_token_funcs.h jni_token_funcs.c) are copied 
# by the "make dist" rule into ${distdir}.  I don't know why!  But because they are in the
# source directory of the distribution they are not extracted from the OpenFortranParser-${OFP_VERSION}.jar
# file and thus are not in the build tree for use in compiling sageSupport.C in the src/frontend/SageIII
# directory.
dist-hook:
	@echo "calling ls -l --full-time ${distdir}"
	ls -l --full-time ${distdir}
	@echo "after ls: removing these files from distdir since they should not be copied here"
	rm -f ${distdir}/ActionEnums.h
	rm -f ${distdir}/c_actions_jni.c
	rm -f ${distdir}/FortranParserAction.h
	rm -f ${distdir}/jni_token_funcs.c
	rm -f ${distdir}/token.h
	rm -f ${distdir}/token.c
	@echo "calling ls -l --full-time ${distdir}"
	ls -l ${distdir}
	@echo "(disthook processed normally)"


clean-local:
	rm -f OpenFortranParser-${OFP_VERSION}.jar FortranParserAction.h c_actions_jni.c
	rm -f token.c token.h ActionEnums.h jni_token_funcs.h jni_token_funcs.c


