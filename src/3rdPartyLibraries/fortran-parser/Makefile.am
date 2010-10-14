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

# DQ (3/13/2010): I think we need to use: "--no-check-certificate"
# Using --no-check-certificate to handle errors:
# Resolving sgone.llnl.gov... 128.115.27.11
# Connecting to sgone.llnl.gov|128.115.27.11|:443... connected.
# ERROR: cannot verify sgone.llnl.gov's certificate, issued by `/C=US/O=VeriSign, Inc./OU=VeriSign Trust Network/OU=Terms 
#   of use at https://www.verisign.com/rpa (c)09/CN=VeriSign Class 3 Secure Server CA - G2': Unable to locally verify the issuer's authority.
# To connect to sgone.llnl.gov insecurely, use `--no-check-certificate'.
# Unable to establish SSL connection.
get_jar_file:
	test -e OpenFortranParser-${OFP_VERSION}.jar || \
      (test -e ${srcdir}/OpenFortranParser-${OFP_VERSION}.jar && cp ${srcdir}/OpenFortranParser-${OFP_VERSION}.jar .) || \
      (wget --no-check-certificate ${EXTERNAL_OFP_JAR_FILE_URL} || exit 1;);
	@if test -e OpenFortranParser-${OFP_VERSION}.jar; then \
      echo "Found valid OpenFortranParser-${OFP_VERSION}.jar file."; \
   else \
      echo "Error: OpenFortranParser-${OFP_VERSION}.jar file is not found."; \
      exit 1; \
   fi;

#     (wget --no-check-certificate ${EXTERNAL_OFP_JAR_FILE_URL}/OpenFortranParser-${OFP_VERSION}.jar || exit 1;);

TEST_OFP_VERSION=0.8.0
TEST_EXTERNAL_OFP_JAR_FILE_URL=http://hivelocity.dl.sourceforge.net/project/fortran-parser/OpenFortranParser/version_${TEST_OFP_VERSION}
test_get_new_jar_file:
	test -e OpenFortranParser-${TEST_OFP_VERSION}.jar || \
     (test -e ${srcdir}/OpenFortranParser-${TEST_OFP_VERSION}.jar && cp ${srcdir}/OpenFortranParser-${TEST_OFP_VERSION}.jar .) || \
     wget --no-check-certificate ${TEST_EXTERNAL_OFP_JAR_FILE_URL}/OpenFortranParser-${TEST_OFP_VERSION}.jar

# DQ (4/4/2010): This needs to be an explicit path list for each file since some are all located in different subdirectories.
# OFP_SOURCE_FILES = FortranParserAction.h c_actions_jni.c token.h token.c ActionEnums.h jni_token_funcs.h jni_token_funcs.c
# Laksono (07/09/2010): remove IFortranParserAction.java
OFP_SOURCE_FILES = \
   fortran/ofp/parser/c/FortranParserAction.h \
   fortran/ofp/parser/c/jni/c_actions_jni.c \
   fortran/ofp/parser/c/token.h \
   fortran/ofp/parser/c/token.c \
   fortran/ofp/parser/c/ActionEnums.h \
   fortran/ofp/parser/c/jni/jni_token_funcs.h \
   fortran/ofp/parser/c/jni/jni_token_funcs.c 

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
extract_rose_required_ofp_source_files: get_jar_file
	@if test -e OpenFortranParser-${OFP_VERSION}.jar; then \
      echo "Found valid OpenFortranParser-${OFP_VERSION}.jar file."; \
   else \
      echo "Error: OpenFortranParser-${OFP_VERSION}.jar file is not found."; \
      exit 1; \
   fi;
	${JAR} xf OpenFortranParser-${OFP_VERSION}.jar $(OFP_SOURCE_FILES)
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

# We distribute the default version of the OFP jar file (version 0.8.1)
# OpenFortranParser-0.8.1.jar but this is handled using the dist_java_DATA 
# automake variable. Also add the generated/downloaded/unpacked files from 
# the OFP jar file.
EXTRA_DIST = CMakeLists.txt $(OFP_SOURCE_FILES_WITHOUT_PATHS)

clean-local:
	rm -f OpenFortranParser-${OFP_VERSION}.jar FortranParserAction.h c_actions_jni.c
	rm -f token.c token.h ActionEnums.h jni_token_funcs.h jni_token_funcs.c 

