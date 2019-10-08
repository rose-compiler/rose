Current OFP version:
  OpenFortranParser-20190922.jar

(10/03/2011): The version of OFP has been changed to a date-based scheme so that more frequent (up to daily) \
versions of OFP can be used by ROSE.  Using jar files from OFP releases is deprecated.

Location of OFP jar file:
  # http://sourceforge.net/projects/fortran-parser/files/OpenFortranParser/version_0.7.2/OpenFortranParser-0.8.6.jar

    http://cdnetworks-us-1.dl.sourceforge.net/project/fortran-parser/OpenFortranParser/version_0.7.2/OpenFortranParser-0.8.6.jar
    http://cdnetworks-us-1.dl.sourceforge.net/project/fortran-parser/OpenFortranParser/version_0.7.2/OpenFortranParser-0.8.6.jar

    http://hivelocity.dl.sourceforge.net/project/fortran-parser/OpenFortranParser/version_0.7.2/OpenFortranParser-0.8.6.jar

To get the jar file automatically, we use wget:
   wget http://hivelocity.dl.sourceforge.net/project/fortran-parser/OpenFortranParser/version_0.7.2/OpenFortranParser-0.8.6.jar

(3/13/2010): It seems that the download link to OFP is a different link today:
   http://iweb.dl.sourceforge.net/project/fortran-parser/OpenFortranParser/version_0.7.2/OpenFortranParser-0.8.6.jar

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

  1.  ./configure --enable-c-actions --enable-c-main --with-java-incs="-I/Library/Java/JavaVirtualMachines/jdk1.8.0_172.jdk/Contents/Home/include -I/Library/Java/JavaVirtualMachines/jdk1.8.0_172.jdk/Contents/Home/include/darwin"
  2.  edit make.inc
   a. manually set OFP_ENABLE_C_MAIN="yes"
  3.  make
  4.  cd build
  5.  cp OpenFortranParser-0.8.6.jar OpenFortranParser-20190922.jar
  6.  manually inspect contents of the jar file for ROSE to see if the required files are present
   a. jar tf OpenFortranParser-20190922.jar
  7.  FIX open-fortran-parser configure and build system so this is unnecessary!

