# CMake helper functions for ROSE binary analysis tests
#
# These functions help migrate binary analysis tests from Tup to CMake
# while preserving the existing test infrastructure (run-test script and filters).

# Find the test infrastructure scripts
set(ROSE_RUN_TEST "${CMAKE_SOURCE_DIR}/scripts/tup/run-test")

if(NOT EXISTS "${ROSE_RUN_TEST}")
    message(FATAL_ERROR "Cannot find run-test script at ${ROSE_RUN_TEST}")
endif()

# Default timeout for tests (same as Tup)
if(NOT DEFINED ROSE_TEST_TIMEOUT)
    set(ROSE_TEST_TIMEOUT "15m")
endif()

#[=======================================================================[.rst:
rose_add_binary_tester
----------------------

Compile a binary analysis test program (tester).

.. code-block:: cmake

  rose_add_binary_tester(<target>
    SOURCES <source1> [<source2> ...]
    [LIBRARIES <lib1> [<lib2> ...]]
    [COMPILE_DEFINITIONS <def1> [<def2> ...]]
  )

Arguments:

``target``
  Name of the executable target to create

``SOURCES``
  List of source files

``LIBRARIES``
  Libraries to link against (default: Rose::Rose)

``COMPILE_DEFINITIONS``
  Preprocessor definitions to add
#]=======================================================================]
function(rose_add_binary_tester TARGET)
    cmake_parse_arguments(TESTER
        ""
        ""
        "SOURCES;LIBRARIES;COMPILE_DEFINITIONS"
        ${ARGN}
    )

    if(NOT TESTER_SOURCES)
        message(FATAL_ERROR "rose_add_binary_tester: SOURCES required for ${TARGET}")
    endif()

    # Default to linking against Rose library
    if(NOT TESTER_LIBRARIES)
        set(TESTER_LIBRARIES ROSE_DLL)
    endif()

    add_executable(${TARGET} ${TESTER_SOURCES})
    target_link_libraries(${TARGET} PRIVATE ${TESTER_LIBRARIES})

    if(TESTER_COMPILE_DEFINITIONS)
        target_compile_definitions(${TARGET} PRIVATE ${TESTER_COMPILE_DEFINITIONS})
    endif()

    # Testers are part of tests, not installed
    set_target_properties(${TARGET} PROPERTIES
        EXCLUDE_FROM_ALL FALSE
        FOLDER "Tests/BinaryAnalysis/Testers"
    )
endfunction()

#[=======================================================================[.rst:
rose_add_binary_test_with_answer
---------------------------------

Add a binary analysis test that compares output with an answer file.
Uses the Tup-style run-test script.

.. code-block:: cmake

  rose_add_binary_test_with_answer(
    NAME <test_name>
    COMMAND <command> [<arg1> ...]
    ANSWER <answer_file>
    [FILTER <filter_command>]
    [DEPENDS <target1> [<target2> ...]]
    [EXTRA_OUTPUTS <file1> [<file2> ...]]
    [DISABLED <reason>]
  )

Arguments:

``NAME``
  Name of the test (required)

``COMMAND``
  Command to run, including arguments (required)

``ANSWER``
  Path to the answer file to compare against (required)

``FILTER``
  Optional filter command to apply to output before comparison.
  Can be a shell pipeline (e.g., "perl -p semanticsFilter")

``DEPENDS``
  Targets or tests this test depends on

``EXTRA_OUTPUTS``
  Additional output files created by the test

``DISABLED``
  If provided, disables the test with the given reason message
#]=======================================================================]
function(rose_add_binary_test_with_answer)
    cmake_parse_arguments(TEST
        ""
        "NAME;ANSWER;FILTER;DISABLED"
        "COMMAND;DEPENDS;EXTRA_OUTPUTS"
        ${ARGN}
    )

    if(NOT TEST_NAME)
        message(FATAL_ERROR "rose_add_binary_test_with_answer: NAME required")
    endif()

    if(NOT TEST_COMMAND)
        message(FATAL_ERROR "rose_add_binary_test_with_answer: COMMAND required")
    endif()

    if(NOT TEST_ANSWER)
        message(FATAL_ERROR "rose_add_binary_test_with_answer: ANSWER required")
    endif()

    # Build the command line for run-test script (Tup style)
    # Usage: run-test [SWITCHES] COMMAND...
    set(run_test_args "")

    # Output file (required by run-test)
    set(output_file "${CMAKE_CURRENT_BINARY_DIR}/${TEST_NAME}.result")
    list(APPEND run_test_args "--output=${output_file}")

    # Timeout
    list(APPEND run_test_args "--timeout=${ROSE_TEST_TIMEOUT}")

    # Answer file
    list(APPEND run_test_args "--answer=${TEST_ANSWER}")

    # Filter
    if(TEST_FILTER)
        list(APPEND run_test_args "--filter=${TEST_FILTER}")
    endif()

    # Extra outputs
    if(TEST_EXTRA_OUTPUTS)
        foreach(extra IN LISTS TEST_EXTRA_OUTPUTS)
            list(APPEND run_test_args "--extra=${extra}")
        endforeach()
    endif()

    # Disabled message
    if(TEST_DISABLED)
        list(APPEND run_test_args "--disabled=${TEST_DISABLED}")
    endif()

    add_test(
        NAME ${TEST_NAME}
        COMMAND env ROSE_ARGS= ${ROSE_RUN_TEST} ${run_test_args} ${TEST_COMMAND}
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    )

    # Set dependencies
    if(TEST_DEPENDS)
        set_tests_properties(${TEST_NAME} PROPERTIES FIXTURES_REQUIRED "${TEST_DEPENDS}")
    endif()

    # Label for organization
    set_tests_properties(${TEST_NAME} PROPERTIES LABELS "BinaryAnalysis")
endfunction()

#[=======================================================================[.rst:
rose_add_binary_test_exit_status
---------------------------------

Add a binary analysis test that checks exit status only.
Uses the Tup-style run-test script.

.. code-block:: cmake

  rose_add_binary_test_exit_status(
    NAME <test_name>
    COMMAND <command> [<arg1> ...]
    [DEPENDS <target1> [<target2> ...]]
    [EXTRA_OUTPUTS <file1> [<file2> ...]]
    [DISABLED <reason>]
  )

Arguments:

``NAME``
  Name of the test (required)

``COMMAND``
  Command to run, including arguments (required)

``DEPENDS``
  Targets or tests this test depends on

``EXTRA_OUTPUTS``
  Additional output files created by the test

``DISABLED``
  If provided, disables the test with the given reason message
#]=======================================================================]
function(rose_add_binary_test_exit_status)
    cmake_parse_arguments(TEST
        ""
        "NAME;DISABLED"
        "COMMAND;DEPENDS;EXTRA_OUTPUTS"
        ${ARGN}
    )

    if(NOT TEST_NAME)
        message(FATAL_ERROR "rose_add_binary_test_exit_status: NAME required")
    endif()

    if(NOT TEST_COMMAND)
        message(FATAL_ERROR "rose_add_binary_test_exit_status: COMMAND required")
    endif()

    # Build the command line for run-test script (Tup style)
    # Usage: run-test [SWITCHES] COMMAND...
    set(run_test_args "")

    # Output file (required by run-test)
    set(output_file "${CMAKE_CURRENT_BINARY_DIR}/${TEST_NAME}.result")
    list(APPEND run_test_args "--output=${output_file}")

    # Timeout
    list(APPEND run_test_args "--timeout=${ROSE_TEST_TIMEOUT}")

    # Extra outputs
    if(TEST_EXTRA_OUTPUTS)
        foreach(extra IN LISTS TEST_EXTRA_OUTPUTS)
            list(APPEND run_test_args "--extra=${extra}")
        endforeach()
    endif()

    # Disabled message
    if(TEST_DISABLED)
        list(APPEND run_test_args "--disabled=${TEST_DISABLED}")
    endif()

    add_test(
        NAME ${TEST_NAME}
        COMMAND env ROSE_ARGS= ${ROSE_RUN_TEST} ${run_test_args} ${TEST_COMMAND}
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    )

    if(TEST_DEPENDS)
        set_tests_properties(${TEST_NAME} PROPERTIES FIXTURES_REQUIRED "${TEST_DEPENDS}")
    endif()

    set_tests_properties(${TEST_NAME} PROPERTIES LABELS "BinaryAnalysis")
endfunction()
