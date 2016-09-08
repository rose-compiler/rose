#!/bin/bash

# Trap handler to make ctrl-c work in almost all cases:
# "kill 0" to kill even background processes:
trap 'echo Aborting...; kill -s SIGKILL 0; exit;' SIGINT SIGTERM

#$1: CodeThorn's src directory
SRCDIR=$(cd $1; pwd) # store the absolute path

#$2: CodeThorn's build directory
BUILDDIR=$(cd $2; pwd) # store the absolute path

#$3: Only clean temporary test files
MAKE_CLEAN_ONLY=$3

#$4: Skip analyterix tests
SKIP_ANALYTERIX_TESTS=$4

TEST_ROOT_DIR="${SRCDIR}/tests/analyterix/"
TEST_BUILD_ROOT_DIR="${BUILDDIR}/analyterix_tests/"

INPUT_FILE="subject.C"
ANNOTATED_OUTPUT_FILE="annotated.C"
ARGUMENTS_FILE="arguments"
HAS_TO_FAIL_FILE="fails"
TIMEOUT_FILE="timeout"

REF_FILE_EXT="ref"
COMPARE_MODE_FILE_EXT="cmpmd"
OUTPUT_FILE_EXTS="csv C dot"

# neutral color codes begin
NEUTRAL_COLOR_BEGIN="\e[1m\e[96m\e[40m"
# error color codes begin
ERROR_COLOR_BEGIN="\e[1m\e[91m\e[40m"
# warning color codes begin
WARNING_COLOR_BEGIN="\e[1m\e[93m\e[40m"
# success color codes begin
SUCCESS_COLOR_BEGIN="\e[1m\e[92m\e[40m"
# color codes end
COLOR_END="\e[0m"
  
SUCCESS_MSG_PREFIX="${SUCCESS_COLOR_BEGIN}Success${COLOR_END}: "
WARNING_MSG_PREFIX="${WARNING_COLOR_BEGIN}Warning${COLOR_END}: "  
ERROR_MSG_PREFIX="${ERROR_COLOR_BEGIN}Error${COLOR_END}: "

echoAndRunCommand(){
  echo "$3"
  echo ""
  # "timeout" does not kill the command sometimes. Both the command and timeout are
  #  sleeping then and nothing happens.  
  #timeout --foreground "$1s" $3
  
  # Own timeout implementation:
  # Execute command in background and stop it immediately to get PID:
  $3 &
  COMMAND_PID=$!
  kill -STOP ${COMMAND_PID}
  # If a timeout is requested:
  WATCHDOG_PID="-1"
  if [ "$1" -gt "0" ]; then
    # Start timeout watchdog in background subshell:
    (
      ((TIMEOUT=$1))
      while ((TIMEOUT > 0)); do
        # Check status every second:
        sleep 1s
        # Stop waiting if the command already finished:
        #  ("kill -0 ..." tests whether the process could be killed)
        kill -0 ${COMMAND_PID} || exit 0
        ((TIMEOUT -= 1))
      done
    
      # The command timed out: end it:
      echo -e "$2Timed out: Killing process now..."
      # Try SIGTERM first:
      kill -s SIGTERM ${COMMAND_PID} && kill -0 ${COMMAND_PID} || exit 0
      sleep "1s"
      # Use SIGKILL if the command is still running:
      kill -0 ${COMMAND_PID} && kill -s SIGKILL ${COMMAND_PID}
    ) 2> /dev/null &
    WATCHDOG_PID=$!
  fi
  # Continue command and wait for it to finish:
  kill -CONT ${COMMAND_PID}
  wait ${COMMAND_PID}
  # exit code of "wait" is the exit code of the command:
  COMMAND_EXIT_CODE=$?
  # kill the timeout watchdog if there is one:
  if [ ! "${WATCHDOG_PID}" -eq "-1" ]; then
    kill -s SIGKILL ${WATCHDOG_PID}
    # suppress the shell's message that a background process was killed:
    wait ${WATCHDOG_PID} 2> /dev/null
    echo -e "$2Killed timeout watchdog."
  fi
  return ${COMMAND_EXIT_CODE}
}

cleanUp() {
  echo "Cleaning up test files in build tree ..."
  rm -rf "${TEST_BUILD_ROOT_DIR}" || exit 1
  echo "Cleaning done."
}

isSubset() {
 FIRST="$(cat "$2" | sort -u)"
 SECOND="$(cat "$1" "$2" | sort -u)"
 if [ "${FIRST}" == "${SECOND}" ]; then
   return "0"
 else
   return "1"
 fi
}

echo "Analyterix test source directory: ${TEST_ROOT_DIR}"
echo "Analyterix test build directory: ${TEST_BUILD_ROOT_DIR}"

# Check whether analyterix tests should be skipped:
if [ "$SKIP_ANALYTERIX_TESTS" == "1" ]; then
  echo -e "${WARNING_MSG_PREFIX}Skipping of analyterix tests is requested. Will not perform tests."
  exit 0
fi

# Clean up temporary files that may have been left from a previous test run before performing the tests:
cleanUp

# Check whether only cleaning was requested:
if [ "$MAKE_CLEAN_ONLY" == "1" ]; then
  echo -e "${WARNING_MSG_PREFIX}Only cleaning was requested. Will not perform tests."
  exit 0
fi

# Perform tests:
TEST_COUNT_PASS=0
TEST_COUNT_FAIL=0
FAILED_TESTS=""
echo "Performing tests..."
mkdir "${TEST_BUILD_ROOT_DIR}" || exit 1
for currTestDir in "${TEST_ROOT_DIR}"*; do
  # Nothing to do if not a directory:
  if [ ! -d "${currTestDir}" ]; then 
    continue;
  fi
  
  # Name of the current test:
  TEST_NAME="$(basename ${currTestDir})"
  
  # Prefix for messages:
  TEST_MSG_PREFIX="Test ${NEUTRAL_COLOR_BEGIN}${TEST_NAME}${COLOR_END}: "
  # Prefix for error messages:
  TEST_ERROR_MSG_PREFIX="${TEST_MSG_PREFIX}${ERROR_MSG_PREFIX}"
  TEST_WARNING_MSG_PREFIX="${TEST_MSG_PREFIX}${WARNING_MSG_PREFIX}"
  TEST_SUCCESS_MSG_PREFIX="${TEST_MSG_PREFIX}${SUCCESS_MSG_PREFIX}"

  # Create corresponding folder in build dir:
  currTestBuildDir="${TEST_BUILD_ROOT_DIR}${TEST_NAME}"
  echo -e "${TEST_MSG_PREFIX}Establish test directory in build tree ..."
  mkdir "${currTestBuildDir}" || exit 1
  cp -rf "${currTestDir}/." "${currTestBuildDir}" || exit 1
  
  # Set the current test build directory as working directory
  #  (this way arguments to analyterix can be relative)
  cd "${currTestBuildDir}" || exit 1
  
  # The build test directory is now the main test directory:
  currTestDir="${currTestBuildDir}"
  
  # Check for the subject:
  if [ ! -f ${currTestDir}/${INPUT_FILE} ]; then 
   echo -e "${TEST_ERROR_MSG_PREFIX}Subject/Input \"${currTestDir}/${INPUT_FILE}\" not found."
   exit 1
  fi

  # Does this test have to fail?
  if [ -f ${currTestDir}/${HAS_TO_FAIL_FILE} ]; then # if an fails file exists
    HAS_TO_FAIL="1"
    FAIL_PASS_STRING="fail"
    FAIL_PASS_STRING_INVERT="pass"
  else
    HAS_TO_FAIL="0"
    FAIL_PASS_STRING="pass"
    FAIL_PASS_STRING_INVERT="fail"
  fi

  # Execute test in a subshell (This way "exit" can be used to stop/end the test and the 
  #  result can be handled at one place.)
  ( 
    echo -e "${TEST_MSG_PREFIX}Starting and expecting to ${FAIL_PASS_STRING} ..."
    
    # Individual arguments for analyterix:
    ARGUMENTS=""
    if [ -f ${currTestDir}/${ARGUMENTS_FILE} ]; then # if an arguments file exists
      # Read arguments into ARGUMENTS:
      ARGUMENTS=$(<${currTestDir}/${ARGUMENTS_FILE})
      # Replace new lines:
      ARGUMENTS=$(echo ${ARGUMENTS} | tr -d '\r' | tr -d '\n')
    fi
    
    # Individual timeout for analyterix:
    TIMEOUT="0"
    TIMEOUT_MSG=""
    if [ -f ${currTestDir}/${TIMEOUT_FILE} ]; then # if an timeout file exists
      # Read timeout into TIMEOUT:
      TIMEOUT=$(<${currTestDir}/${TIMEOUT_FILE})
      TIMEOUT_MSG=" with a timeout of ${TIMEOUT} second(s) and"
    fi
    
    # Run analyterix with the given arguments and echo the command before:
    echo -e "${TEST_MSG_PREFIX}Executing analyterix${TIMEOUT_MSG} by using the following command:"
    echoAndRunCommand "${TIMEOUT}" "${TEST_MSG_PREFIX}" "$BUILDDIR/analyterix ${INPUT_FILE} --edg:no_warnings -rose:output ${ANNOTATED_OUTPUT_FILE} ${ARGUMENTS}"
    # Check whether the execution was not successful:
    ANALYTERIX_EXIT_CODE=$?
    if [ ! "${ANALYTERIX_EXIT_CODE}" -eq "0" ]; then
      if [ "${ANALYTERIX_EXIT_CODE}" -eq "124" ] && [ "${TIMEOUT}" -gt "0" ] ; then
        # Currently not used because of own timeout implemenation:
        echo -e "${TEST_WARNING_MSG_PREFIX}Analyterix timed out."
      else
        echo -e "${TEST_WARNING_MSG_PREFIX}Analyterix' exit code is ${ANALYTERIX_EXIT_CODE} (and not 0)."
      fi
      exit 1
    else
      echo -e "${TEST_MSG_PREFIX}Analyterix exited normally."
    fi
    
    # Compare each reference file with the corresponding output file:
    for currRefFile in "${currTestDir}/"*."${REF_FILE_EXT}"; do
      # Check whether there are reference files (even if there are no files the
      #  body of the for loop gets executed once with e.g. "*.ref" as the file name):
      if [ ! -f ${currRefFile} ]; then
        echo -e "${TEST_MSG_PREFIX}No reference files found."
        break;
      fi
      # Reference file name without extension (and without path):
      REF_FILE_BASE=$(basename "${currRefFile}" ${REF_FILE_EXT})
      
      # How should the ref file be compared with the output file? Default is equal:
      COMPARE_MODE="equal"
      # Look for comparision mode file:
      CURR_COMPARE_MODE_FILE="${currTestDir}/${REF_FILE_BASE}${COMPARE_MODE_FILE_EXT}"
      if [ -f ${CURR_COMPARE_MODE_FILE} ]; then
        # Read comparision mode file content into COMPARE_MODE:
        COMPARE_MODE=$(<${CURR_COMPARE_MODE_FILE})
      fi
      
      echo -e "${TEST_MSG_PREFIX}Searching for output file that corresponds to reference file \"${REF_FILE_BASE}${REF_FILE_EXT}\"..."
      OUTPUT_FILE_FOUND="0"
      # For each possible extension: check whether there is a corresponding output file.
      #  For example if there is a reference file "a.ref" then we will look for output files
      #  "a.C", "a.csv" and so on.
      for currOutputExt in ${OUTPUT_FILE_EXTS}; do
        # Absolute path to a possible output file:
        CURR_OUTPUT_FILE="${currTestDir}/${REF_FILE_BASE}${currOutputExt}"
        # First part of message that is used in the if-case as well as in the else-case: 
        OUTPUT_FILE_MSG="${TEST_MSG_PREFIX}Output file \"$(basename "${CURR_OUTPUT_FILE}")\""
        # Does the possible output file exist?
        if [ -f ${CURR_OUTPUT_FILE} ]; then
          # We found the corresponding output file. Compare it with the reference file:
          echo -e "${OUTPUT_FILE_MSG} found. Comparing with reference file by using comparision mode \"${COMPARE_MODE}\"..."
          echo -e "${TEST_MSG_PREFIX}Checking for equality..."
          # Currently the equality test is needed in all cases:
          diff ${currRefFile} ${CURR_OUTPUT_FILE}
          EQUAL_RESULT="$?"
          
          if [ "${COMPARE_MODE}" == "equal" ]; then
            # The result is the result of the equality check:
            COMPARE_RESULT=${EQUAL_RESULT}
            COMPARE_MSG_PART="equal to"
          elif [ "${COMPARE_MODE}" == "subsetOrEqual" ]; then
            # Check for subset if the files are not equal:
            echo -e "${TEST_MSG_PREFIX}Checking for subset..."
            if [ ! "${EQUAL_RESULT}" -eq 0 ]; then
              isSubset "${CURR_OUTPUT_FILE}" "${currRefFile}"
              COMPARE_RESULT="$?"
            fi
            COMPARE_MSG_PART="a subset of or equal to"
          elif [ "${COMPARE_MODE}" == "supersetOrEqual" ]; then
            # Check for superset if the files are not equal:
            echo -e "${TEST_MSG_PREFIX}Checking for superset..."
            if [ ! "${EQUAL_RESULT}" -eq 0 ]; then
              isSubset "${currRefFile}" "${CURR_OUTPUT_FILE}"
              COMPARE_RESULT="$?"
            fi
            COMPARE_MSG_PART="a superset of or equal to"
          else 
            echo -e "${TEST_ERROR_MSG_PREFIX}Unknown comparision mode: ${COMPARE_MODE}"
            exit 2
          fi
          
          if [ ! "${COMPARE_RESULT}" -eq 0 ]; then
            echo -e "${TEST_MSG_PREFIX}Output is not ${COMPARE_MSG_PART} reference."
            exit 1	
          else
            echo -e "${TEST_MSG_PREFIX}Output is ${COMPARE_MSG_PART} reference."
            OUTPUT_FILE_FOUND="1"
          fi
          # It does not make sense to search for further corresponding output files:
          break
        else
          echo -e "${OUTPUT_FILE_MSG} not found."
        fi
      done 
      # It is an error if analyterix did not produce an output file for a reference file:
      if [ "${OUTPUT_FILE_FOUND}" == "0" ]; then
        echo -e "${TEST_MSG_PREFIX}No corresponding output file found."
        exit 1
      fi 
    done
    
    # Test passed:
    echo -e "${TEST_MSG_PREFIX}Passed."
    exit 0
  )
  # Get test exit code:
  FAILED=$?
  if [ "${FAILED}" -eq "2" ]; then
    # Error while executing the test.
    exit 1
  fi
  
  echo -e "${TEST_MSG_PREFIX}Finished. Comparing result with expected result..."
  if [ "${HAS_TO_FAIL}" == "${FAILED}" ]; then
    # Build the result message:
    if [ "${FAILED}" == "1" ]; then
      RESULT_MSG="${TEST_WARNING_MSG_PREFIX}"
    else
      RESULT_MSG="${TEST_SUCCESS_MSG_PREFIX}"
    fi
    echo -e "${RESULT_MSG}Has to ${FAIL_PASS_STRING} and ${FAIL_PASS_STRING}ed (OK)."
  else
    echo -e "${TEST_ERROR_MSG_PREFIX}Has to ${FAIL_PASS_STRING} but ${FAIL_PASS_STRING_INVERT}ed (NOT OK)."
    exit 1
  fi
  # Increase counters and remember failed tests:
  if [ "${FAILED}" == "1" ]; then
    TEST_COUNT_FAIL=$((TEST_COUNT_FAIL+1))
    FAILED_TESTS="${FAILED_TESTS} ${TEST_NAME}"
  else
    TEST_COUNT_PASS=$((TEST_COUNT_PASS+1))
  fi
  
  echo -e "\n${NEUTRAL_COLOR_BEGIN} <=========================================================================> ${COLOR_END}\n"
done

echo -e "${SUCCESS_COLOR_BEGIN}All tests OK.${COLOR_END}"

echo "Test count: $((TEST_COUNT_FAIL+TEST_COUNT_PASS))"
if [ "${TEST_COUNT_FAIL}" -gt "0" ]; then
  echo -e "${WARNING_MSG_PREFIX}${TEST_COUNT_FAIL} test(s) is/are currently failing:${FAILED_TESTS}"
fi

# Tests are ok. Clean up temporary files:
cleanUp



