#!/bin/sh -e

function printBanner {
if test $# = 1 -o $# = 2 ; then
  TMPFILE=/tmp/hudson_${hudson_start_time_seconds}
  echo "*******************************************************************"       >> $TMPFILE
  echo "${1}"                                                                      >> $TMPFILE
  echo ""                                                                          >> $TMPFILE
  if test $# = 2 ; then
    echo "${2}"                                                                    >> $TMPFILE
  else
    echo "`date`"                                                                  >> $TMPFILE
  fi
  echo "-------------------------------------------------------------------"       >> $TMPFILE
  cat $TMPFILE
  rm -rf $TMPFILE
else
  echo "[Error] Usage: printBanner <with-message>"
  exit 1
fi
}

function printBannerWithDate() {
  printBanner ""
}

function printBannerWithElapsedTime() {
if test $# = 3 ; then
  if test $2 -gt $3 ; then
    elapsed_time_in_seconds="`expr $2 - $3`"
  else
    elapsed_time_in_seconds="`expr $3 - $2`"
  fi
 
  set +e
  elapsed_hours="`expr $elapsed_time_in_seconds / 3600`"
  elapsed_time_in_seconds="`expr $elapsed_time_in_seconds % 3600`"
  elapsed_minutes="`expr $elapsed_time_in_seconds / 60`"
  elapsed_seconds="`expr $elapsed_time_in_seconds % 60`"
  set -e

  printBanner "$1" "Duration:  ${elapsed_hours}h ${elapsed_minutes}m ${elapsed_seconds}s"
else
  echo "[Error] Usage: printBannerWithElapsedTime <with-message> <start-time-seconds> <end-time-seconds>"
  exit 1
fi
}

function printBannerToStartStep() {
if test $# = 1 ; then
  printBanner "Starting '${1}' step..."
else
  echo "[Error] Usage: printBannerStartStep <step-name>" 
  exit 1
fi
}

function printBannerToEndStep() {
if test $# = 2 ; then
  printBannerWithElapsedTime \
        "Done with '${1}' step!" \
        $start_time_seconds \
        $(date +%s)
else
  echo "[Error] Usage: printBannerEndStep <step-name> <start-time-in-seconds>" 
  exit 1
fi
}


#########################################################################################
#
#  Dump Hudson environment
#  This script should be sourced in other Hudson run test scripts to
#  uniformly dump the Hudson environment.
#

#  TOO (3/16/2011): Centralized environment information to make it more
#   useful for debugging
#  DQ (1/14/2010): Support for dumping the enviroment so that it 
#   can be easily sourced to permit Hudson tests to be reproduced.
#
#########################################################################################
function dumpHudsonEnvironment() {
#########################################################################################
env_dump_file="$PWD/HudsonEnvironment.log"
rm -rf $env_dump_file
#env_dump_file="/tmp/env_dump_file_$hudson_start_time_seconds.$machine_name.hudson"

echo ""                                                                          >> $env_dump_file
echo ""                                                                          >> $env_dump_file
echo ""                                                                          >> $env_dump_file
echo "*******************************************************************"       >> $env_dump_file
echo "                        Test-environment"                                  >> $env_dump_file
echo "                        `date`"                                            >> $env_dump_file
echo "-------------------------------------------------------------------"       >> $env_dump_file
echo ""                                                                          >> $env_dump_file
echo "This output is dumped in:"                                                 >> $env_dump_file   
echo ""                                                                          >> $env_dump_file
echo "$env_dump_file"                                                            >> $env_dump_file   
echo ""                                                                          >> $env_dump_file
echo ""                                                                          >> $env_dump_file
echo "-------------------------------------------------------------------"       >> $env_dump_file
echo "                                                          [General]"       >> $env_dump_file
echo ""                                                                          >> $env_dump_file
echo ""                                                                          >> $env_dump_file
echo "machine                           = $machine"                              >> $env_dump_file
echo "machine_name                      = $machine_name"                         >> $env_dump_file
echo "start_time                        = $start_time"                           >> $env_dump_file
echo "hudson_start_time_seconds         = $hudson_start_time_seconds"            >> $env_dump_file

echo ""                                                                          >> $env_dump_file
echo ""                                                                          >> $env_dump_file
echo "-------------------------------------------------------------------"       >> $env_dump_file
echo "                                                           [Hudson]"       >> $env_dump_file
echo ""                                                                          >> $env_dump_file
echo ""                                                                          >> $env_dump_file
echo "ROSE_TEST_BUILD_SKIP_BUILD_SCRIPT = ${ROSE_TEST_BUILD_SKIP_BUILD_SCRIPT}"  >> $env_dump_file
echo "ROSE_TEST_BUILD_STYLE             = ${ROSE_TEST_BUILD_STYLE}"              >> $env_dump_file
echo "ROSE_TEST_BOOST_PATH              = ${ROSE_TEST_BOOST_PATH}"               >> $env_dump_file
echo "ROSE_TEST_JAVA_PATH               = ${ROSE_TEST_JAVA_PATH}"                >> $env_dump_file
set +e
echo "$ROSE_TEST_BUILD_STYLE" | grep --quiet "make.*full"
if test $? = 0 ; then
# echo "Environment variables required for ROSE_TEST_BUILD_STYLE = full"           >> $env_dump_file
 echo "ROSE_TEST_QT_PATH                 = ${ROSE_TEST_QT_PATH}"                  >> $env_dump_file
 echo "ROSE_TEST_SQLITE_PATH             = ${ROSE_TEST_SQLITE_PATH}"              >> $env_dump_file
 echo "ROSE_TEST_RTED_PATH               = ${ROSE_TEST_RTED_PATH}"                >> $env_dump_file
 echo "ROSE_TEST_OPENGL_PATH             = ${ROSE_TEST_OPENGL_PATH}"              >> $env_dump_file
fi
set -e

echo ""                                                                          >> $env_dump_file
echo ""                                                                          >> $env_dump_file
echo "-------------------------------------------------------------------"       >> $env_dump_file
echo "                                                              [Git]"       >> $env_dump_file
echo ""                                                                          >> $env_dump_file
echo ""                                                                          >> $env_dump_file
# tps (02/04/2010) : Checking the EDG submodule version
echo "git submodule status (EDG version) ="                                      >> $env_dump_file
echo "  `git submodule status`"                                                  >> $env_dump_file

echo ""                                                                          >> $env_dump_file
echo ""                                                                          >> $env_dump_file
echo "-------------------------------------------------------------------"       >> $env_dump_file
echo "                                                     [Library Path]"       >> $env_dump_file
echo ""                                                                          >> $env_dump_file
echo ""                                                                          >> $env_dump_file
if test "x$label" = "x64bit-macos-10.6" ; then
 echo "DYLD_LIBRARY_PATH ="                                                      >> $env_dump_file
 for a_path in `echo $DYLD_LIBRARY_PATH | sed 's/:/ /g'` ; do
   echo "    $a_path"                                                            >> $env_dump_file
 done
echo ""                                                                          >> $env_dump_file
 echo "Copy me: $DYLD_LIBRARY_PATH"                                              >> $env_dump_file
else
 echo "LD_LIBRARY_PATH ="                                                        >> $env_dump_file
 for a_path in `echo $LD_LIBRARY_PATH | sed 's/:/ /g'` ; do
   echo "    $a_path"                                                            >> $env_dump_file
 done
echo ""                                                                          >> $env_dump_file
echo "Copy me: $LD_LIBRARY_PATH"                                                 >> $env_dump_file
fi

echo ""                                                                          >> $env_dump_file
echo ""                                                                          >> $env_dump_file
echo "-------------------------------------------------------------------"       >> $env_dump_file
echo "                                                  [Executable Path]"       >> $env_dump_file
echo ""                                                                          >> $env_dump_file
echo ""                                                                          >> $env_dump_file
echo "PATH ="                                                                    >> $env_dump_file
 for a_path in `echo $PATH | sed 's/:/ /g'` ; do
   echo "    $a_path"                                                            >> $env_dump_file
 done
echo ""                                                                          >> $env_dump_file
echo "Copy me: $PATH"                                                            >> $env_dump_file

echo ""                                                                          >> $env_dump_file
echo ""                                                                          >> $env_dump_file
echo "-------------------------------------------------------------------"       >> $env_dump_file
echo "                                                   [Configure line]"       >> $env_dump_file
echo ""                                                                          >> $env_dump_file
echo ""                                                                          >> $env_dump_file
echo "./configure"                                                               >> $env_dump_file
 for an_option in ${CONFIGURE_FLAGS} ; do 
   echo "    $an_option"                                                         >> $env_dump_file
 done
echo ""                                                                          >> $env_dump_file
echo "Copy me: ./configure ${CONFIGURE_FLAGS}"                                   >> $env_dump_file

echo ""                                                                          >> $env_dump_file
echo ""                                                                          >> $env_dump_file
echo "-------------------------------------------------------------------"       >> $env_dump_file
echo "                                                     [Install tree]"       >> $env_dump_file
echo ""                                                                          >> $env_dump_file
echo ""                                                                          >> $env_dump_file
echo "CONFIGURE_PREFIX_DIR (--prefix)="                                          >> $env_dump_file 
echo ""                                                                          >> $env_dump_file
echo "    $CONFIGURE_PREFIX_DIR"                                                 >> $env_dump_file
echo ""                                                                          >> $env_dump_file
echo ""                                                                          >> $env_dump_file

echo "^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^"       >> $env_dump_file
echo "                        Test-environment"                                  >> $env_dump_file
echo "*******************************************************************"       >> $env_dump_file
echo ""                                                                          >> $env_dump_file
echo ""                                                                          >> $env_dump_file
echo ""                                                                          >> $env_dump_file
echo ""                                                                          >> $env_dump_file

cat $env_dump_file
}
function printHudsonEnvironment {
  dumpHudsonEnvironment
}
