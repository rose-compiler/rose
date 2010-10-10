javadir=$(libdir)

OFP_VERSION=@ROSE_OFP_MAJOR_VERSION_NUMBER@.@ROSE_OFP_MINOR_VERSION_NUMBER@.@ROSE_OFP_PATCH_VERSION_NUMBER@

# DQ (4/5/2010): Think about if we need the test using an Automake conditional since the jar file always exists.
if ROSE_USE_OPEN_FORTRAN_PARSER
dist_java_DATA = OpenFortranParser-${OFP_VERSION}.jar
endif

# Location of jar file on Open Fortran Parser web site changes from day to day...
# To get the current setting, dowload the file from the web (e.g. using Firefox) and then click on the
# entry in the "download window" to select the download link, copy the link into the macro below.
# Later this needs to be setup via configure (if there is not better solution).
# EXTERNAL_OFP_JAR_FILE_URL=http://hivelocity.dl.sourceforge.net/project/fortran-parser/OpenFortranParser/version_${OFP_VERSION}
# EXTERNAL_OFP_JAR_FILE_URL=http://iweb.dl.sourceforge.net/project/fortran-parser/OpenFortranParser/version_${OFP_VERSION}
# EXTERNAL_OFP_JAR_FILE_URL=http://cdnetworks-us-2.dl.sourceforge.net/project/fortran-parser/OpenFortranParser/version_${OFP_VERSION}
# EXTERNAL_OFP_JAR_FILE_URL=http://voxel.dl.sourceforge.net/project/fortran-parser/OpenFortranParser/version_${OFP_VERSION}/OpenFortranParser-${OFP_VERSION}.jar

# This appears to be a stable URL for downloading OFP...
EXTERNAL_OFP_JAR_FILE_URL=http://sourceforge.net/projects/fortran-parser/files/OpenFortranParser/version_${OFP_VERSION}/OpenFortranParser-${OFP_VERSION}.jar/download

TARGET_OFP_SOURCE_FILES = token.c jni_token_funcs.c c_actions_jni.c

# DQ (10/9/2010): I am eternally trying to get the makefiles dependence information correct...
# token.c: OpenFortranParser-${OFP_VERSION}.jar
# jni_token_funcs.c: OpenFortranParser-${OFP_VERSION}.jar
# c_actions_jni.c: OpenFortranParser-${OFP_VERSION}.jar

# DQ (10/9/2010): If the jar file in the build tree if older than that in the source tree then copy the jar file from the source tree to the build tree.
# DQ (9/26/2010): If there is a newer version of OFP then copy it into the compile tree...
# OpenFortranParser-${OFP_VERSION}.jar: ${srcdir}/OpenFortranParser-${OFP_VERSION}.jar
# ${build_dir}/OpenFortranParser-${OFP_VERSION}.jar: ${srcdir}/OpenFortranParser-${OFP_VERSION}.jar
#./OpenFortranParser-${OFP_VERSION}.jar: ${srcdir}/OpenFortranParser-${OFP_VERSION}.jar
#	cp ${srcdir}/OpenFortranParser-${OFP_VERSION}.jar OpenFortranParser-${OFP_VERSION}.jar

# DQ (10/9/2010): If the jar file in the source tree is not present then use wget to fetch it from the OFP web site.
# DQ (3/13/2010): I think we need to use: "--no-check-certificate"
# Using --no-check-certificate to handle errors:
# Resolving sgone.llnl.gov... 128.115.27.11
# Connecting to sgone.llnl.gov|128.115.27.11|:443... connected.
# ERROR: cannot verify sgone.llnl.gov's certificate, issued by `/C=US/O=VeriSign, Inc./OU=VeriSign Trust Network/OU=Terms 
#   of use at https://www.verisign.com/rpa (c)09/CN=VeriSign Class 3 Secure Server CA - G2': Unable to locally verify the issuer's authority.
# To connect to sgone.llnl.gov insecurely, use `--no-check-certificate'.
# Unable to establish SSL connection.
# get_jar_file:
# get_jar_file: OpenFortranParser-${OFP_VERSION}.jar
# OpenFortranParser-${OFP_VERSION}.jar:
# ${srcdir}/OpenFortranParser-${OFP_VERSION}.jar:
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

# Older version of shell code to automatically get OFP from the web.
#	test -e OpenFortranParser-${OFP_VERSION}.jar || \
#      (test -e ${srcdir}/OpenFortranParser-${OFP_VERSION}.jar && cp ${srcdir}/OpenFortranParser-${OFP_VERSION}.jar .) || \
#      (wget --no-check-certificate ${EXTERNAL_OFP_JAR_FILE_URL} || exit 1;);
#	@if test -e OpenFortranParser-${OFP_VERSION}.jar; then \
#      echo "Found valid ${srcdir}/OpenFortranParser-${OFP_VERSION}.jar file."; \
#   else \
#      echo "Error: ${srcdir}/OpenFortranParser-${OFP_VERSION}.jar file is not found."; \
#      exit 1; \
#

# token.c: extract_rose_required_ofp_source_files 
# extract_rose_required_ofp_source_files: token.c
# extract_rose_required_ofp_source_files: $(OFP_SOURCE_FILES)
# extract_rose_required_ofp_source_files: $(TARGET_OFP_SOURCE_FILES)
# extract_rose_required_ofp_source_files: $(OFP_SOURCE_FILES_WITHOUT_PATHS)
extract_rose_required_ofp_source_files: token.c
#	@echo "files: extract_rose_required_ofp_source_files is out of date compared to $(TARGET_OFP_SOURCE_FILES)"

#     (wget --no-check-certificate ${EXTERNAL_OFP_JAR_FILE_URL}/OpenFortranParser-${OFP_VERSION}.jar || exit 1;);

TEST_OFP_VERSION=0.8.1
# TEST_EXTERNAL_OFP_JAR_FILE_URL=http://hivelocity.dl.sourceforge.net/project/fortran-parser/OpenFortranParser/version_${TEST_OFP_VERSION}
TEST_EXTERNAL_OFP_JAR_FILE_URL=http://sourceforge.net/projects/fortran-parser/files/OpenFortranParser/version_${TEST_OFP_VERSION}/OpenFortranParser-${TEST_OFP_VERSION}.jar/download

# This is a rule for testing the download of OFP from the web.
test_get_new_jar_file:
#	test -e OpenFortranParser-${TEST_OFP_VERSION}.jar || \
#     (test -e ${srcdir}/OpenFortranParser-${TEST_OFP_VERSION}.jar && cp ${srcdir}/OpenFortranParser-${TEST_OFP_VERSION}.jar .) || \
#     wget --no-check-certificate ${TEST_EXTERNAL_OFP_JAR_FILE_URL}/OpenFortranParser-${TEST_OFP_VERSION}.jar
	test -e OpenFortranParser-${TEST_OFP_VERSION}.jar || (wget --no-check-certificate ${TEST_EXTERNAL_OFP_JAR_FILE_URL} || exit 1;);

# DQ (4/4/2010): This needs to be an explicit path list for each file since some are all located in different subdirectories.
# OFP_SOURCE_FILES = FortranParserAction.h c_actions_jni.c token.h token.c ActionEnums.h jni_token_funcs.h jni_token_funcs.c
# Laksono (07/09/2010): remove IFortranParserAction.java
# DQ (9/26/2010): I need fortran/ofp/parser/java/IFortranParserAction.java so I can get the names of new parameters
# in c_actions that I discover are modified in newer versions of OFP.  Please do not remove.
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
   FortranParserAction.h \
   c_actions_jni.c \
   token.h \
   token.c \
   ActionEnums.h \
   jni_token_funcs.h \
   jni_token_funcs.c 

# Liao 8/4/2010, based on Jean Utke's suggestion on [Rose-public] Fortran built-ins
if ROSE_USE_OPEN_FORTRAN_PARSER
include_HEADERS = ActionEnums.h token.h FortranParserAction.h
endif

# I don't know why this does not seem to work, but that we need three separate rules.
#$(OFP_SOURCE_FILES): OpenFortranParser-${OFP_VERSION}.jar
#	echo "files: $(OFP_SOURCE_FILES) are out of date compared to OpenFortranParser-${OFP_VERSION}.jar"
#token.c: OpenFortranParser-${OFP_VERSION}.jar
#	echo "files: token.c are out of date compared to OpenFortranParser-${OFP_VERSION}.jar"
# jni_token_funcs.c: OpenFortranParser-${OFP_VERSION}.jar
#	echo "files: jni_token_funcs.c are out of date compared to OpenFortranParser-${OFP_VERSION}.jar"
# c_actions_jni.c: OpenFortranParser-${OFP_VERSION}.jar
#	echo "files: c_actions_jni.c are out of date compared to OpenFortranParser-${OFP_VERSION}.jar"

# DQ (9/26/2010): Add the jar file as a dependency so that when it is 
# updated it will force the required files to be extracted automatically.
# extract_rose_required_ofp_source_files: get_jar_file
# extract_rose_required_ofp_source_files: get_jar_file OpenFortranParser-${OFP_VERSION}.jar
# extract_rose_required_ofp_source_files: get_jar_file ${OFP_SOURCE_FILES_WITHOUT_PATHS}
# extract_rose_required_ofp_source_files: get_jar_file
# extract_rose_required_ofp_source_files: OpenFortranParser-${OFP_VERSION}.jar
# extract_rose_required_ofp_source_files: token.c jni_token_funcs.c c_actions_jni.c
# OpenFortranParser-${OFP_VERSION}.jar: token.c jni_token_funcs.c c_actions_jni.c
# $(OFP_SOURCE_FILES): OpenFortranParser-${OFP_VERSION}.jar
# YYY: OpenFortranParser-${OFP_VERSION}.jar
# token.c: OpenFortranParser-${OFP_VERSION}.jar
# $(TARGET_OFP_SOURCE_FILES): OpenFortranParser-${OFP_VERSION}.jar
# $(OFP_SOURCE_FILES): OpenFortranParser-${OFP_VERSION}.jar
# extract_rose_required_ofp_source_files: OpenFortranParser-${OFP_VERSION}.jar
# token.c: OpenFortranParser-${OFP_VERSION}.jar
# token.c: ./OpenFortranParser-${OFP_VERSION}.jar
token.c: OpenFortranParser-${OFP_VERSION}.jar
	@if test -e OpenFortranParser-${OFP_VERSION}.jar; then \
      echo "Found valid OpenFortranParser-${OFP_VERSION}.jar file."; \
   else \
      echo "Error: OpenFortranParser-${OFP_VERSION}.jar file is not found."; \
      if test -e ${srcdir}/OpenFortranParser-${OFP_VERSION}.jar; then \
         echo "Found valid ${srcdir}/OpenFortranParser-${OFP_VERSION}.jar file."; \
         cp ${srcdir}/OpenFortranParser-${OFP_VERSION}.jar OpenFortranParser-${OFP_VERSION}.jar; \
      else \
         echo "Error: ${srcdir}/OpenFortranParser-${OFP_VERSION}.jar file is not found."; \
         exit 1; \
      fi; \
   fi;
	jar xf OpenFortranParser-${OFP_VERSION}.jar $(OFP_SOURCE_FILES)
	@list='$(OFP_SOURCE_FILES)'; for f in $$list; do \
	   echo "building $$f file"; \
	   filename=`basename $$f`; \
      echo "filename = $$filename"; \
      if test -e $$f; then \
         echo "Using OFP source file $$f from version $(OFP_VERSION) found in OpenFortranParser-${OFP_VERSION}.jar file."; \
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
            echo "Error: can't use default (version 0.8.1) OFP source file $filename_without_path with OpenFortranParser-${OFP_VERSION}.jar file."; \
            exit 1; \
         fi; \
      fi; \
   done
#	Remove the fortran directory generated from the extraction of files from the jar file.
	@if test -d fortran; then \
      rm -rf fortran; \
   fi;

# DQ (10/4/2010): We only watn to distribute the jar file for OFP, and then
# extract what we require from the jar file (putting it into the build tree).
# OLD COMMENT: We distribute the default version of the OFP jar file (version 0.8.1)
# OLD COMMENT: OpenFortranParser-0.8.1.jar but this is handled using the dist_java_DATA 
# OLD COMMENT: automake variable. Also add the generated/downloaded/unpacked files from 
# OLD COMMENT: the OFP jar file.
# EXTRA_DIST = CMakeLists.txt $(OFP_SOURCE_FILES_WITHOUT_PATHS)
EXTRA_DIST = CMakeLists.txt

clean-local:
	rm -f OpenFortranParser-${OFP_VERSION}.jar FortranParserAction.h c_actions_jni.c
	rm -f token.c token.h ActionEnums.h jni_token_funcs.h jni_token_funcs.c


