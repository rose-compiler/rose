#!/bin/bash

# Trap handler to make ctrl-c work in almost all cases:
trap "echo Aborting...; exit;" SIGINT SIGTERM

#$1: CodeThorn's src directory
SRCDIR=$(cd $1; pwd) # store the absolute path

#$2: CodeThorn's build directory
BUILDDIR=$(cd $2; pwd) # store the absolute path

#$3: Only clean temporary test files
MAKE_CLEAN_ONLY=$3

TEST_ROOT_DIR="${SRCDIR}/tests/analyterix/"

INPUT_FILE="subject.C"
ANNOTATED_OUTPUT_FILE="annotated.C"
ARGUMENTS_FILE="arguments"
HAS_TO_FAIL_FILE="fails"
TIMEOUT_FILE="timeout"

REF_FILE_EXT="ref"
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
  echo "$2"
  echo ""
  timeout --foreground "$1s" $2
}

cleanUp() {
  echo "Cleaning up temporary files..."
  # Remove every output file (every file with an output extension except special test files)
  for OUTPUT_EXT in ${OUTPUT_FILE_EXTS}; do
    find "${TEST_ROOT_DIR}" ! -name "${INPUT_FILE}" ! -name "${ARGUMENTS_FILE}" ! -name "${HAS_TO_FAIL_FILE}" ! -name "${TIMEOUT_FILE}" -name "*.${OUTPUT_EXT}" -delete
  done
  echo "Cleaning done."
}

echo "Analyterix test directory: ${TEST_ROOT_DIR}"

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
for currTestDir in "${TEST_ROOT_DIR}"*; do
  # Nothing to do if not a directory:
  [ -d "${currTestDir}" ] || exit 0
  
  # Name of the current test:
  TEST_NAME="$(basename ${currTestDir})"
  
  # Prefix for messages:
  TEST_MSG_PREFIX="Test ${NEUTRAL_COLOR_BEGIN}${TEST_NAME}${COLOR_END}: "
  # Prefix for error messages:
  TEST_ERROR_MSG_PREFIX="${TEST_MSG_PREFIX}${ERROR_MSG_PREFIX}"
  TEST_WARNING_MSG_PREFIX="${TEST_MSG_PREFIX}${WARNING_MSG_PREFIX}"
  TEST_SUCCESS_MSG_PREFIX="${TEST_MSG_PREFIX}${SUCCESS_MSG_PREFIX}"

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
    # Set the current test directory as working directory
    #  (this way path arguments to analyterix can be relative to the test directory)
    echo -e "Entering directory ${currTestDir}..."
    cd "${currTestDir}"
    
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
      TIMEOUT_MSG=" with a timeout of ${TIMEOUT} second(s)"
    fi
    
    # Run analyterix with the given arguments and echo the command before:
    echo -e "${TEST_MSG_PREFIX}Executing analyterix${TIMEOUT_MSG} by using the following command:"
    echoAndRunCommand "${TIMEOUT}" "$BUILDDIR/analyterix ${INPUT_FILE} -rose:output ${ANNOTATED_OUTPUT_FILE} ${ARGUMENTS}"
    # Check whether the execution was not successful:
    ANALYTERIX_EXIT_CODE=$?
    if [ ! "${ANALYTERIX_EXIT_CODE}" -eq "0" ]; then
      if [ "${ANALYTERIX_EXIT_CODE}" -eq "124" ] && [ "${TIMEOUT}" -gt "0" ] ; then
        echo -e "${TEST_ERROR_MSG_PREFIX}Analyterix timed out."
      else
        echo -e "${TEST_ERROR_MSG_PREFIX}Analyterix' exit code is ${ANALYTERIX_EXIT_CODE} (and not 0)."
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
          echo -e "${OUTPUT_FILE_MSG} found. Comparing with reference file..."
          # We found the corresponding output file. Compare it with the reference file:
          diff ${currRefFile} ${CURR_OUTPUT_FILE}
          if [ ! "$?" -eq 0 ]; then
            echo -e "${TEST_ERROR_MSG_PREFIX}Output does not match reference (see diff above)."
            exit 1	
          else
            echo -e "${TEST_MSG_PREFIX}Files match."
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
        echo -e "${TEST_ERROR_MSG_PREFIX}No corresponding output file found."
        exit 1
      fi 
    done
    
    # Test passed:
    echo -e "${TEST_SUCCESS_MSG_PREFIX}Passed."
    exit 0
  )
  # Get test exit code:
  FAILED=$?
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



