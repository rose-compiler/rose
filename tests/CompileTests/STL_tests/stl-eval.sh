#!/bin/bash
# Author: Markus Schordan, 2016.

: ${TOOL1:=g++}
: ${TOOL2:=identityTranslator}

###############################################################################
# the following four lists control the behavior of the script
###############################################################################

STL_CPP98_HEADERS_PASSING="algorithm deque exception functional limits list map memory new numeric queue set stack typeinfo utility valarray vector"
STL_CPP98_HEADERS_FAILING="bitset complex fstream iomanip ios iosfwd iostream istream iterator locale ostream sstream stdexcept streambuf string"

STL_CPP11_HEADERS_PASSING=""
STL_CPP11_HEADERS_FAILING="algorithm bitset complex deque exception fstream functional iomanip ios iosfwd iostream istream iterator limits list locale map memory new numeric ostream queue set sstream stack stdexcept streambuf string typeinfo utility valarray vector"

###############################################################################

# ensure that no old result files are present

GREEN="\033[32;0m"   # Green
RED="\033[31;1m"   # Red
COLOREND="\033[0m"
REDFAIL="${RED}FAIL${COLOREND}"

BS_INCLUDE=-I.

TOOL2_BACKEND=-rose:skipfinalCompileStep

function cleanup {
    rm -f *.o *.pp.C *.ti
    rm -f test_*.* rose_test*.* a.out
}

function test_failed {
  echo -e "${RED}SOME TESTS FAILED (${TOTAL_FAIL}). See above list for details.${COLOREND}"
  exit 1
}

function check {

local STL_HEADERS=$1
local LANG_STANDARD=$2
local COMMENT=$3

T0_PASS=0
T0_FAIL=0
T1_PASS=0
T1_FAIL=0
T2_PASS=0
T2_FAIL=0

for header in ${STL_HEADERS}; do
  printf "TESTING: %-17s: " "$header"
  printf "#include <$header>\nint main(){ return 0; }\n" > test_$header.C
  # option -P: inhibit generation of linemarkers
  cpp -P -std=$LANG_STANDARD -x c++ test_${header}.C test_${header}.pp.C
  LOC=`wc -l test_${header}.pp.C| cut -f1 -d' '`
  printf "%6s LOC : " "$LOC"

  # use a sub shell and redirect coredump output of subshell to /dev/null
  {
   $TOOL1 $BS_INCLUDE test_${header}.C -std=$LANG_STANDARD &> /dev/null
  } > /dev/null 2>&1

  if [ $? -eq 0 ]; then
      echo -n "PASS " # 0
      ((T0_PASS+=1))
      {
          $TOOL2 $TOOL2_BACKEND $BS_INCLUDE test_${header}.pp.C -std=$LANG_STANDARD &> /dev/null
      } > /dev/null 2>&1
      if [ $? -eq 0 ]; then
          if [ -e rose_test_${header}.pp.C ]
          then
              echo -n "PASS" # 1
              ((T1_PASS+=1))
              g++ -std=$LANG_STANDARD rose_test_${header}.pp.C -w -Wfatal-errors > /dev/null 2>&1
              if [ $? -eq 0 ]; then
                  echo -n " PASS : 100.00%" # 2
                  ((T2_PASS+=1))
              else
                  # determine line number of error
                  ERROR_LINE=`g++ -std=${LANG_STANDARD} rose_test_${header}.pp.C -w -Wfatal-errors 2>&1 | egrep rose_test_${header}.pp.C:[0-9] | cut -f2 -d:` 
                  ERROR_PERCENTAGE=`echo "scale=2; ${ERROR_LINE}*100/${LOC}" | bc`
                  echo -en " ${RED}FAIL${COLOREND}"
                  printf " : %6s%% (LINE:%s)" "$ERROR_PERCENTAGE" "$ERROR_LINE" # 2
                  cp rose_test_${header}.pp.C rose_test_$header.t2.fail.C
                  ((T2_FAIL+=1))
              fi            
          else
              echo -n " ${RED}FAIL${COLOREND} [no file generated!] "
              ((T1_FAIL+=1))
          fi
      else
          echo -en "${RED}FAIL ----${COLOREND}" # 1
          ((T1_FAIL+=1))
      fi
  else
      echo -en "${RED}FAIL ---- ----${COLOREND}" # 0
      ((T0_FAIL+=1))
  fi
  # print end of line with (optional) comment
  echo " $COMMENT"
  ((INDEX+=1))
done

if [ "x" == "x$1" ]; then
    echo "No passing tests."
else
    ((TOTAL_FAIL=T0_FAIL+T1_FAIL+T2_FAIL))
    # present statistics
    echo "-----------------------------------------------------------------"
    echo -e "PASS/FAIL                                $T0_PASS/${RED}$T0_FAIL${COLOREND} $T1_PASS/${RED}$T1_FAIL${COLOREND} $T2_PASS/${RED}$T2_FAIL${COLOREND} : TOTAL: ${T2_PASS}/${RED}${TOTAL_FAIL}${COLOREND}"
    echo "-----------------------------------------------------------------"
fi

} # end of function check

#main

cleanup

TOTAL_FAIL=0
echo "-----------------------------------------------------------------"
echo "STL C++98 CHECK"
echo "-----------------------------------------------------------------"
check "$STL_CPP98_HEADERS_PASSING" "c++98" ""
if [ ${TOTAL_FAIL} -gt 0 ]; then
  test_failed
fi

echo
echo "-----------------------------------------------------------------"
echo "STL C++98 CHECK (EXPECTED FAILS)"
echo "-----------------------------------------------------------------"
check "$STL_CPP98_HEADERS_FAILING" "c++98" "[expected fail]"

TOTAL_FAIL=0
echo
echo "-----------------------------------------------------------------"
echo "STL C++11 CHECK"
echo "-----------------------------------------------------------------"
check "$STL_CPP11_HEADERS_PASSING" "c++11" ""
if [ $TOTAL_FAIL -gt 0 ]; then
  test_failed
fi

echo
echo "-----------------------------------------------------------------"
echo "STL C++11 CHECK (EXPECTED FAILS)"
echo "-----------------------------------------------------------------"
check "$STL_CPP11_HEADERS_FAILING" "c++11" "[expected fail]"

echo
echo -e "${GREEN}-----------------------------------------------------------------${COLOREND}"
echo -e "${GREEN}ALL TESTS PASSED (that are expected to pass)${COLOREND}"
echo -e "${GREEN}-----------------------------------------------------------------${COLOREND}"

# clean up (remove all generated files. cleanup is only performed when all tests passed)
cleanup

# NOTES on cpp:
# -fdirectives-only
#    When preprocessing, handle directives, but do not expand macros.
#    The option’s behavior depends on the -E and -fpreprocessed option
