Current OFP version:
  OpenFortranParser-20200704.jar

(10/03/2011): The version of OFP has been changed to a date-based scheme so that more frequent (up to daily) \
versions of OFP can be used by ROSE.  Using jar files from OFP releases is deprecated.

Location of OFP project files:
  https://github.com/OpenFortranProject/open-fortran-parser

Files required for ROSE from the OFP source tree:
  FortranParserAction.h 
  c_actions_jni.c
  token.c
  token.h
  ActionEnums.h
  jni_token_funcs.h
  jni_token_funcs.c
These have been added to this directory and removed from src/frontend/OpenFortranParser_SAGE_Connection.
These files are now automatically extracted from the jar file if they 
exist in the jar file.

(9/22/2019): To get these files inserted into the jar file you need to configure open-fortran-parser with
something like:

  1.  ./configure --enable-c-actions --enable-c-main --with-java-incs="-I/usr/local/opt/openjdk/include"
  2.  edit make.inc
      a. manually set OFP_ENABLE_C_MAIN="yes"
  3.  make
  4.  cd build
  5.  cp OpenFortranParser-0.8.7.jar OpenFortranParser-20200704.jar
      a. this is needed because OpenFortranParser-0.8.7.jar will have the C header files
  6.  manually inspect contents of the jar file for ROSE to see if the required files are present
      a. jar tf OpenFortranParser-20200704.jar
  7.  FIX open-fortran-parser configure and build system so this is unnecessary!

