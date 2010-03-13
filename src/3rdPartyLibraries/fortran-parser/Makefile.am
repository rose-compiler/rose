javadir=$(libdir)

OFP_VERSION=@ROSE_OFP_MAJOR_VERSION_NUMBER@.@ROSE_OFP_MINOR_VERSION_NUMBER@.@ROSE_OFP_PATCH_VERSION_NUMBER@
# Thing about if we need the test using an Automake conditional since the jar file always exists.
if ROSE_USE_OPEN_FORTRAN_PARSER
# dist_java_DATA = lib/OpenFortranParser-0.7.2.jar
# dist_java_DATA = lib/OpenFortranParser-${OFP_VERSION}.jar
dist_java_DATA = OpenFortranParser-${OFP_VERSION}.jar
endif

# Location of jar file on Open Fortran Parser web site changes from day to day...
# EXTERNAL_OFP_JAR_FILE_URL=http://hivelocity.dl.sourceforge.net/project/fortran-parser/OpenFortranParser/version_${OFP_VERSION}
EXTERNAL_OFP_JAR_FILE_URL=http://iweb.dl.sourceforge.net/project/fortran-parser/OpenFortranParser/version_${OFP_VERSION}

# OFP_JAR_FILE_DISTRIBUTION_PATH=lib
# %.jar:
#	test -d ${OFP_JAR_FILE_DISTRIBUTION_PATH} && cp ${OFP_JAR_FILE_DISTRIBUTION_PATH}/$@  . || wget ${EXTERNAL_OFP_JAR_FILE_URL}/$@
#	test -d ${OFP_JAR_FILE_DISTRIBUTION_PATH} && cp ${srcdir}/${OFP_JAR_FILE_DISTRIBUTION_PATH}/$@  ${OFP_JAR_FILE_DISTRIBUTION_PATH} || wget ${EXTERNAL_OFP_JAR_FILE_URL}/$@
#	test -e $@ || cp ${srcdir}/$@ . || wget ${EXTERNAL_OFP_JAR_FILE_URL}/$@

# ${top_builddir}/src/3rdPartyLibraries/fortran-parser/OpenFortranParser-${OFP_VERSION}.jar:
#	test -e $@ || cp ${srcdir}/$@ . || wget ${EXTERNAL_OFP_JAR_FILE_URL}/$@

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
      (wget --no-check-certificate ${EXTERNAL_OFP_JAR_FILE_URL}/OpenFortranParser-${OFP_VERSION}.jar || exit 1;);
	@if test -e OpenFortranParser-${OFP_VERSION}.jar; then \
      echo "Found valid OpenFortranParser-${OFP_VERSION}.jar file."; \
   else \
      echo "Error: OpenFortranParser-${OFP_VERSION}.jar file is not found."; \
      exit 1; \
   fi;

# TEST_OFP_VERSION=${OFP_VERSION}
TEST_OFP_VERSION=0.8.0
TEST_EXTERNAL_OFP_JAR_FILE_URL=http://hivelocity.dl.sourceforge.net/project/fortran-parser/OpenFortranParser/version_${TEST_OFP_VERSION}

get_new_jar_file:
	test -e OpenFortranParser-${TEST_OFP_VERSION}.jar || \
     (test -e ${srcdir}/OpenFortranParser-${TEST_OFP_VERSION}.jar && cp ${srcdir}/OpenFortranParser-${TEST_OFP_VERSION}.jar .) || \
     wget --no-check-certificate ${TEST_EXTERNAL_OFP_JAR_FILE_URL}/OpenFortranParser-${TEST_OFP_VERSION}.jar

# DQ (3/13/2010): Extract the FortranParserAction.h and c_actions_jni.c files (required by ROSE)
# Note that we also need:
#    token.c
#    token.h
#    ActionEnums.h
#    jni_token_funcs.h
#    jni_token_funcs.c
OLD_extract_rose_required_ofp_source_files: get_jar_file
	@if test -e OpenFortranParser-${OFP_VERSION}.jar; then \
      echo "Found valid OpenFortranParser-${OFP_VERSION}.jar file."; \
   else \
      echo "Error: OpenFortranParser-${OFP_VERSION}.jar file is not found."; \
      exit 1; \
   fi;
	jar xf OpenFortranParser-${OFP_VERSION}.jar fortran/ofp/parser/c/FortranParserAction.h fortran/ofp/parser/c/jni/c_actions_jni.c
#	Move FortranParserAction.h
	@echo "building FortranParserAction.h file"
	@if test -e fortran/ofp/parser/c/FortranParserAction.h; then \
      mv fortran/ofp/parser/c/FortranParserAction.h FortranParserAction.h; \
   else \
      echo "Warning: fortran/ofp/parser/c/FortranParserAction.h is not found in jar file."; \
      if test $(ROSE_OFP_MINOR_VERSION_NUMBER) == "x7" -a $(ROSE_OFP_PATCH_VERSION_NUMBER) == "x2"; then \
         cp ${srcdir}/FortranParserAction.h FortranParserAction.h; \
      else \
         echo "Error: can't use default OFP source files with OpenFortranParser-${OFP_VERSION}.jar file."; \
         exit 1; \
      fi; \
   fi;
#	Move c_actions_jni.c
	@echo "building c_actions_jni.c file"
	@if test -e fortran/ofp/parser/c/jni/c_actions_jni.c; then \
      mv fortran/ofp/parser/c/jni/c_actions_jni.c c_actions_jni.c; \
   else \
      echo "Warning: fortran/ofp/parser/c/c_actions_jni.c is not found in jar file."; \
      cp ${srcdir}/c_actions_jni.c c_actions_jni.c; \
   fi;
#	Move token.h
	@echo "building token.h file"
	@if test -e fortran/ofp/parser/c/jni/token.h; then \
      mv fortran/ofp/parser/c/jni/token.h token.h; \
   else \
      echo "Warning: fortran/ofp/parser/c/token.h is not found in jar file."; \
      cp ${srcdir}/token.h token.h; \
   fi;
#	Move token.c
	@echo "building token.c file"
	@if test -e fortran/ofp/parser/c/jni/token.c; then \
      mv fortran/ofp/parser/c/jni/token.c token.c; \
   else \
      echo "Warning: fortran/ofp/parser/c/token.c is not found in jar file."; \
      cp ${srcdir}/token.c token.c; \
   fi;
#	Move ActionEnums.h
	@echo "building ActionEnums.h file"
	@if test -e fortran/ofp/parser/c/jni/ActionEnums.h; then \
      mv fortran/ofp/parser/c/jni/ActionEnums.h ActionEnums.h; \
   else \
      echo "Warning: fortran/ofp/parser/c/ActionEnums.h is not found in jar file."; \
      cp ${srcdir}/ActionEnums.h ActionEnums.h; \
   fi;
#	Move jni_token_funcs.h
	@echo "building jni_token_funcs.h file"
	@if test -e fortran/ofp/parser/c/jni/jni_token_funcs.h; then \
      mv fortran/ofp/parser/c/jni/jni_token_funcs.h jni_token_funcs.h; \
   else \
      echo "Warning: fortran/ofp/parser/c/jni_token_funcs.h is not found in jar file."; \
      cp ${srcdir}/jni_token_funcs.h jni_token_funcs.h; \
   fi;
#	Move jni_token_funcs.c
	@echo "building jni_token_funcs.c file"
	@if test -e fortran/ofp/parser/c/jni/jni_token_funcs.c; then \
      mv fortran/ofp/parser/c/jni/jni_token_funcs.c jni_token_funcs.c; \
   else \
      echo "Warning: fortran/ofp/parser/c/jni_token_funcs.c is not found in jar file."; \
      cp ${srcdir}/jni_token_funcs.c jni_token_funcs.c; \
   fi;
#	Remove the fortran directory generated from the extraction of files from the jar file.
	@if test -d fortran; then \
      rm -rf fortran; \
   fi;

OFP_SOURCE_FILES = FortranParserAction.h c_actions_jni.c token.h token.c ActionEnums.h jni_token_funcs.h jni_token_funcs.c

extract_rose_required_ofp_source_files: get_jar_file
	@if test -e OpenFortranParser-${OFP_VERSION}.jar; then \
      echo "Found valid OpenFortranParser-${OFP_VERSION}.jar file."; \
   else \
      echo "Error: OpenFortranParser-${OFP_VERSION}.jar file is not found."; \
      exit 1; \
   fi;
	jar xf OpenFortranParser-${OFP_VERSION}.jar fortran/ofp/parser/c/FortranParserAction.h fortran/ofp/parser/c/jni/c_actions_jni.c
	@list='$(OFP_SOURCE_FILES)'; for f in $$list; do \
	   echo "building $$f file"; \
      if test -e fortran/ofp/parser/c/$$f; then \
         echo "Using OFP source file fortran/ofp/parser/c/$$f from version $(OFP_VERSION) found in OpenFortranParser-${OFP_VERSION}.jar file."; \
         mv fortran/ofp/parser/c/$$f $$f; \
      else \
         echo "Warning: fortran/ofp/parser/c/$$f is not found in jar file."; \
         if test "x$(OFP_VERSION)" = "x0.7.2"; then \
            echo "Using OFP source file fortran/ofp/parser/c/$$f from version $(OFP_VERSION) distributed with ROSE."; \
            cp ${srcdir}/$$f $$f; \
         else \
            echo "Error: can't use default (version 0.7.2) OFP source file $$f with OpenFortranParser-${OFP_VERSION}.jar file."; \
            exit 1; \
         fi; \
      fi; \
   done
#	Remove the fortran directory generated from the extraction of files from the jar file.
	@if test -d fortran; then \
      rm -rf fortran; \
   fi;

# Alternative syntax to specify the default version we distribute with ROSE.
# if test "x@ROSE_OFP_MINOR_VERSION_NUMBER@" = "x7" && test "x@ROSE_OFP_PATCH_VERSION_NUMBER@" = "x2"; then


# We distribute the default version of the OFP jar file (version 0.7.2)
# OpenFortranParser-0.7.2.jar but this is handled using the dist_java_DATA 
# automake variable.
EXTRA_DIST = CMakeLists.txt $(OFP_SOURCE_FILES)

clean-local:
	rm -f OpenFortranParser-${OFP_VERSION}.jar FortranParserAction.h c_actions_jni.c
	rm -f token.c token.h ActionEnums.h jni_token_funcs.h jni_token_funcs.c

