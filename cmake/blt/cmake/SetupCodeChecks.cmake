# Copyright (c) 2017-2022, Lawrence Livermore National Security, LLC and
# other BLT Project Developers. See the top-level LICENSE file for details
# 
# SPDX-License-Identifier: (BSD-3-Clause)
#------------------------------------------------------------------------------
# Targets related to source code checks (formatting, static analysis, etc)
#------------------------------------------------------------------------------

add_custom_target(${BLT_CODE_CHECK_TARGET_NAME})
add_custom_target(${BLT_CODE_STYLE_TARGET_NAME})

set(_BLT_STYLE_VERSION_WARNING_ISSUED FALSE CACHE BOOL "Limits BLT issuing more than one warning for style version")

if(ASTYLE_FOUND)
    set(BLT_REQUIRED_ASTYLE_VERSION "" CACHE STRING "Required version of astyle")
    # targets for verifying formatting
    add_custom_target(astyle_check)
    add_dependencies(${BLT_CODE_CHECK_TARGET_NAME} astyle_check)

    # targets for modifying formatting
    add_custom_target(astyle_style)
    add_dependencies(${BLT_CODE_STYLE_TARGET_NAME} astyle_style)
endif()

if(CLANGFORMAT_FOUND)
    set(BLT_REQUIRED_CLANGFORMAT_VERSION "" CACHE STRING "Required version of clang-format")
    # targets for verifying formatting
    add_custom_target(clangformat_check)
    add_dependencies(${BLT_CODE_CHECK_TARGET_NAME} clangformat_check)

    # targets for modifying formatting
    add_custom_target(clangformat_style)
    add_dependencies(${BLT_CODE_STYLE_TARGET_NAME} clangformat_style)
endif()

if(UNCRUSTIFY_FOUND)
    set(BLT_REQUIRED_UNCRUSTIFY_VERSION "" CACHE STRING "Required version of uncrustify")
    # targets for verifying formatting
    add_custom_target(uncrustify_check)
    add_dependencies(${BLT_CODE_CHECK_TARGET_NAME} uncrustify_check)

    # targets for modifying formatting
    add_custom_target(uncrustify_style)
    add_dependencies(${BLT_CODE_STYLE_TARGET_NAME} uncrustify_style)
endif()

if(YAPF_FOUND)
    set(BLT_REQUIRED_YAPF_VERSION "" CACHE STRING "Required version of yapf")
    # targets for verifying formatting
    add_custom_target(yapf_check)
    add_dependencies(${BLT_CODE_CHECK_TARGET_NAME} yapf_check)

    # targets for modifying formatting
    add_custom_target(yapf_style)
    add_dependencies(${BLT_CODE_STYLE_TARGET_NAME} yapf_style)
endif()
  
if(CMAKEFORMAT_FOUND)
    set(BLT_REQUIRED_CMAKEFORMAT_VERSION "" CACHE STRING "Required version of cmake-format")
    # targets for verifying formatting
    add_custom_target(cmakeformat_check)
    add_dependencies(${BLT_CODE_CHECK_TARGET_NAME} cmakeformat_check)

    # targets for modifying formatting
    add_custom_target(cmakeformat_style)
    add_dependencies(${BLT_CODE_STYLE_TARGET_NAME} cmakeformat_style)
endif()

if(CPPCHECK_FOUND)
    add_custom_target(cppcheck_check)
    add_dependencies(${BLT_CODE_CHECK_TARGET_NAME} cppcheck_check)
endif()

if(CLANGQUERY_FOUND)
    # note: interactive_clang_query_check 
    # is for the use of code developers who
    # want to check specific attributes of
    # specific targets, and does not make 
    # sense as a dependency of check
    add_custom_target(clang_query_check)
    add_custom_target(interactive_clang_query_check)
    add_dependencies(${BLT_CODE_CHECK_TARGET_NAME} clang_query_check)
endif()

if(CLANGTIDY_FOUND)
    add_custom_target(clang_tidy_check)
    add_dependencies(${BLT_CODE_CHECK_TARGET_NAME} clang_tidy_check)
endif()

# Code check targets should only be run on demand
foreach(target 
        check cmakeformat_check yapf_check uncrustify_check astyle_check clangformat_check cppcheck_check
        style cmakeformat_style yapf_style uncrustify_style astyle_style clangformat_style
        clang_query_check interactive_clang_query_check clang_tidy_check)
    if(TARGET ${target})
        set_property(TARGET ${target} PROPERTY EXCLUDE_FROM_ALL TRUE)
        set_property(TARGET ${target} PROPERTY EXCLUDE_FROM_DEFAULT_BUILD TRUE)
    endif()
endforeach()


##------------------------------------------------------------------------------
## blt_add_code_checks( PREFIX               <Base name used for created targets>
##                      SOURCES              [source1 [source2 ...]]
##                      ASTYLE_CFG_FILE      <Path to AStyle config file>
##                      CLANGFORMAT_CFG_FILE <Path to ClangFormat config file>
##                      UNCRUSTIFY_CFG_FILE  <Path to Uncrustify config file>
##                      YAPF_CFG_FILE        <Path to Yapf config file>
##                      CMAKEFORMAT_CFG_FILE <Path to CMakeFormat config file>
##                      CPPCHECK_FLAGS       <List of flags added to Cppcheck>
##                      CLANGQUERY_CHECKER_DIRECTORIES [dir1 [dir2]])
##
## This macro adds all enabled code check targets for the given SOURCES. It
## filters checks based on file extensions.
##------------------------------------------------------------------------------

macro(blt_add_code_checks)

    set(options )
    set(singleValueArgs PREFIX ASTYLE_CFG_FILE CLANGFORMAT_CFG_FILE UNCRUSTIFY_CFG_FILE YAPF_CFG_FILE CMAKEFORMAT_CFG_FILE)
    set(multiValueArgs SOURCES CPPCHECK_FLAGS CLANGQUERY_CHECKER_DIRECTORIES)

    cmake_parse_arguments(arg
        "${options}" "${singleValueArgs}" "${multiValueArgs}" ${ARGN})

    if (NOT DEFINED arg_PREFIX)
        message(FATAL_ERROR "blt_add_code_checks requires the parameter PREFIX.")
    endif()

    if (NOT DEFINED arg_SOURCES)
        message(FATAL_ERROR "blt_add_code_checks requires the parameter SOURCES.")
    endif()

    # Make the sources relative to the bin directory
    set(_rel_sources)
    foreach(_file ${arg_SOURCES})
        # Get full path
        if(IS_ABSOLUTE ${_file})
            set(_full_path ${_file})
        else()
            set(_full_path ${CMAKE_CURRENT_SOURCE_DIR}/${_file})
        endif()

        file(RELATIVE_PATH _rel_path ${CMAKE_BINARY_DIR} ${_full_path})
        list(APPEND _rel_sources ${_rel_path})
    endforeach()   

    # Generate source lists based on language
    set(_c_sources)
    set(_f_sources)
    set(_py_sources)
    set(_cmake_sources)
    blt_split_source_list_by_language(SOURCES      ${_rel_sources}
                                      C_LIST       _c_sources
                                      Fortran_LIST _f_sources
                                      Python_LIST  _py_sources
                                      CMAKE_LIST   _cmake_sources)

    # Check that no more than one formatting config file was supplied
    # for C-style languages.
    set(_c_formatter_cfgs_supplied)
    foreach (_c_formatter ASTYLE UNCRUSTIFY CLANGFORMAT)
      if (DEFINED arg_${_c_formatter}_CFG_FILE)
        list(APPEND _c_formatter_cfgs_supplied ${_c_formatter}_CFG_FILE)
      endif()
    endforeach()
    list(LENGTH _c_formatter_cfgs_supplied _num_c_formatter_cfgs_supplied)
    if (_num_c_formatter_cfgs_supplied GREATER 1)
      message(FATAL_ERROR
        "blt_add_code_checks macro does not support multiple "
        "style config parameters within the same invocation. "
        "At most one of ASTYLE_CFG_FILE, CLANGFORMAT_CFG_FILE, "
        "and UNCRUSTIFY_CFG_FILE may be supplied. "
        "The following style config parameters were supplied: "
        ${_c_formatter_cfgs_supplied})
    endif()

    # Add code checks
    set(_error_msg "blt_add_code_checks tried to create an already existing target with given PREFIX: ${arg_PREFIX}. ")

    if (ASTYLE_FOUND AND DEFINED arg_ASTYLE_CFG_FILE)
        set(_check_target_name ${arg_PREFIX}_astyle_check)
        blt_error_if_target_exists(${_check_target_name} ${_error_msg})
        set(_style_target_name ${arg_PREFIX}_astyle_style)
        blt_error_if_target_exists(${_style_target_name} ${_error_msg})

        blt_add_astyle_target( NAME              ${_check_target_name}
                               MODIFY_FILES      FALSE
                               CFG_FILE          ${arg_ASTYLE_CFG_FILE} 
                               WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
                               SRC_FILES         ${_c_sources} )

        blt_add_astyle_target( NAME              ${_style_target_name}
                               MODIFY_FILES      TRUE
                               CFG_FILE          ${arg_ASTYLE_CFG_FILE} 
                               WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
                               SRC_FILES         ${_c_sources} )
    endif()

    if (CLANGFORMAT_FOUND AND DEFINED arg_CLANGFORMAT_CFG_FILE)
        set(_check_target_name ${arg_PREFIX}_clangformat_check)
        blt_error_if_target_exists(${_check_target_name} ${_error_msg})
        set(_style_target_name ${arg_PREFIX}_clangformat_style)
        blt_error_if_target_exists(${_style_target_name} ${_error_msg})

        blt_add_clangformat_target( NAME              ${_check_target_name}
                                    MODIFY_FILES      FALSE
                                    CFG_FILE          ${arg_CLANGFORMAT_CFG_FILE} 
                                    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
                                    SRC_FILES         ${_c_sources} )

        blt_add_clangformat_target( NAME              ${_style_target_name}
                                    MODIFY_FILES      TRUE
                                    CFG_FILE          ${arg_CLANGFORMAT_CFG_FILE} 
                                    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
                                    SRC_FILES         ${_c_sources} )
    endif()

    if (UNCRUSTIFY_FOUND AND DEFINED arg_UNCRUSTIFY_CFG_FILE)
        set(_check_target_name ${arg_PREFIX}_uncrustify_check)
        blt_error_if_target_exists(${_check_target_name} ${_error_msg})
        set(_style_target_name ${arg_PREFIX}_uncrustify_style)
        blt_error_if_target_exists(${_style_target_name} ${_error_msg})

        blt_add_uncrustify_target( NAME              ${_check_target_name}
                                   MODIFY_FILES      FALSE
                                   CFG_FILE          ${arg_UNCRUSTIFY_CFG_FILE} 
                                   WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
                                   SRC_FILES         ${_c_sources} )

        blt_add_uncrustify_target( NAME              ${_style_target_name}
                                   MODIFY_FILES      TRUE
                                   CFG_FILE          ${arg_UNCRUSTIFY_CFG_FILE} 
                                   WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
                                   SRC_FILES         ${_c_sources} )
    endif()

    if (YAPF_FOUND AND DEFINED arg_YAPF_CFG_FILE)
        set(_check_target_name ${arg_PREFIX}_yapf_check)
        blt_error_if_target_exists(${_check_target_name} ${_error_msg})
        set(_style_target_name ${arg_PREFIX}_yapf_style)
        blt_error_if_target_exists(${_style_target_name} ${_error_msg})

        blt_add_yapf_target( NAME              ${_check_target_name}
                             MODIFY_FILES      FALSE
                             CFG_FILE          ${arg_YAPF_CFG_FILE}
                             WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
                             SRC_FILES         ${_py_sources} )

        blt_add_yapf_target( NAME              ${_style_target_name}
                             MODIFY_FILES      TRUE
                             CFG_FILE          ${arg_YAPF_CFG_FILE}
                             WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
                             SRC_FILES         ${_py_sources} )
    endif()

    if (CMAKEFORMAT_FOUND AND DEFINED arg_CMAKEFORMAT_CFG_FILE)
        set(_check_target_name ${arg_PREFIX}_cmakeformat_check)
        blt_error_if_target_exists(${_check_target_name} ${_error_msg})
        set(_style_target_name ${arg_PREFIX}_cmakeformat_style)
        blt_error_if_target_exists(${_style_target_name} ${_error_msg})

        blt_add_cmakeformat_target( NAME              ${_check_target_name}
                                    MODIFY_FILES      FALSE
                                    CFG_FILE          ${arg_CMAKEFORMAT_CFG_FILE}
                                    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
                                    SRC_FILES         ${_cmake_sources} )

        blt_add_cmakeformat_target( NAME              ${_style_target_name}
                                    MODIFY_FILES       TRUE
                                    CFG_FILE           ${arg_CMAKEFORMAT_CFG_FILE}
                                    WORKING_DIRECTORY  ${CMAKE_BINARY_DIR}
                                    SRC_FILES          ${_cmake_sources} )
    endif()

    if (CPPCHECK_FOUND)
        set(_cppcheck_target_name ${arg_PREFIX}_cppcheck_check)
        blt_error_if_target_exists(${_cppcheck_target_name} ${_error_msg})

        blt_add_cppcheck_target( NAME              ${_cppcheck_target_name}
                                 WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
                                 SRC_FILES         ${_c_sources}
                                 PREPEND_FLAGS     ${arg_CPPCHECK_FLAGS})
    endif()

    # Append possible checker directories and only enable clang-query if at least one is provided
    set(_clangquery_checker_directories)
    if (DEFINED arg_CLANGQUERY_CHECKER_DIRECTORIES)
      list(APPEND _clangquery_checker_directories ${arg_CLANGQUERY_CHECKER_DIRECTORIES})
    endif()
    if (DEFINED BLT_CLANG_QUERY_CHECKER_DIRECTORIES)
      list(APPEND _clangquery_checker_directories ${BLT_CLANG_QUERY_CHECKER_DIRECTORIES})
    endif()
    blt_list_remove_duplicates(TO _clangquery_checker_directories)
    list(LENGTH _clangquery_checker_directories _len)

    if (CLANGQUERY_FOUND AND (_len GREATER 0))
        set(_clang_query_target_name ${arg_PREFIX}_clang_query_check)
        blt_error_if_target_exists(${_clang_query_target_name} ${_error_msg})
        blt_add_clang_query_target( NAME                ${_clang_query_target_name}
                                    WORKING_DIRECTORY   ${CMAKE_BINARY_DIR}
                                    SRC_FILES           ${_c_sources}
                                    CHECKER_DIRECTORIES ${_clangquery_checker_directories})
    endif()

    if (CLANGTIDY_FOUND)
        set(_clang_tidy_target_name ${arg_PREFIX}_clang_tidy_check)
        blt_error_if_target_exists(${_clang_tidy_target_name} ${_error_msg})
        blt_add_clang_tidy_target( NAME              ${_clang_tidy_target_name}
                                   WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
                                   SRC_FILES         ${_c_sources})
    endif()

endmacro(blt_add_code_checks)

##-----------------------------------------------------------------------------
## blt_add_clang_query_target( NAME                <Created Target Name>
##                             WORKING_DIRECTORY   <Working Directory>
##                             COMMENT             <Additional Comment for Target Invocation>
##                             CHECKERS            <If specified, requires a specific set of checkers>
##                             DIE_ON_MATCH        <If true, matches stop the build>
##                             SRC_FILES           [FILE1 [FILE2 ...]]
##                             CHECKER_DIRECTORIES [dir1 [dir2]])
##
## Creates a new target with the given NAME for running clang_query over the given SRC_FILES
##-----------------------------------------------------------------------------
macro(blt_add_clang_query_target)
    if(CLANGQUERY_FOUND)

        ## parse the arguments to the macro
        set(options)
        set(singleValueArgs NAME COMMENT WORKING_DIRECTORY DIE_ON_MATCH)
        set(multiValueArgs SRC_FILES CHECKERS CHECKER_DIRECTORIES)

        cmake_parse_arguments(arg
            "${options}" "${singleValueArgs}" "${multiValueArgs}" ${ARGN} )

        # Check required parameters
        if(NOT DEFINED arg_NAME)
             message(FATAL_ERROR "blt_add_clang_query_target requires a NAME parameter")
        endif()

        if(NOT DEFINED arg_SRC_FILES)
            message(FATAL_ERROR "blt_add_clang_query_target requires a SRC_FILES parameter")
        endif()

        if(DEFINED arg_WORKING_DIRECTORY)
            set(_wd ${arg_WORKING_DIRECTORY})
        else()
            set(_wd ${CMAKE_CURRENT_SOURCE_DIR})
        endif()

        # Append possible checker directories and only enable clang-query if at least one is provided
        set(_checker_directories)
        if (DEFINED arg_CHECKER_DIRECTORIES)
            list(APPEND _checker_directories ${arg_CHECKER_DIRECTORIES})
        endif()
        if (DEFINED BLT_CLANG_QUERY_CHECKER_DIRECTORIES)
            list(APPEND _checker_directories ${BLT_CLANG_QUERY_CHECKER_DIRECTORIES})
        endif()
        blt_list_remove_duplicates(TO _checker_directories)
        list(LENGTH _checker_directories _len)

        if (_len EQUAL 0)
            message(FATAL_ERROR "blt_add_clang_query_target requires either CLANGQUERY_CHECKER_DIRECTORIES parameter"
                                "or variable BLT_CLANG_QUERY_CHECKER_DIRECTORIES defined.")
        endif()

        set(interactive_target_name interactive_${arg_NAME})
        set(CLANG_QUERY_HELPER_SCRIPT ${BLT_ROOT_DIR}/cmake/clang-query-wrapper.py)
        set(CLANG_QUERY_HELPER_COMMAND python ${CLANG_QUERY_HELPER_SCRIPT} --clang-query ${CLANGQUERY_EXECUTABLE} --checker-directories ${_checker_directories} --compilation-database-path ${CMAKE_BINARY_DIR})

        if(arg_DIE_ON_MATCH)
            set(CLANG_QUERY_HELPER_COMMAND ${CLANG_QUERY_HELPER_COMMAND} --die-on-match)
        endif()

        if(DEFINED arg_CHECKERS)
            STRING(REGEX REPLACE " " ":" CHECKER_ARG_STRING ${arg_CHECKERS})
            add_custom_target(${arg_NAME}
              COMMAND ${CLANG_QUERY_HELPER_COMMAND} -i --checkers=${CHECKER_ARG_STRING} ${arg_SRC_FILES}
                    WORKING_DIRECTORY ${_wd}
                    COMMENT "${arg_COMMENT}Running specified clang_query source code static analysis checks.")
        else() #DEFINED CHECKERS
            add_custom_target(${arg_NAME}
              COMMAND ${CLANG_QUERY_HELPER_COMMAND} ${arg_SRC_FILES}
                    WORKING_DIRECTORY ${_wd}
                    COMMENT "${arg_COMMENT}Running all clang_query source code static analysis checks.")
        endif()

        add_custom_target(${interactive_target_name}
          COMMAND ${CLANG_QUERY_HELPER_COMMAND} -i ${arg_SRC_FILES}
                WORKING_DIRECTORY ${_wd}
                COMMENT "${arg_COMMENT}Running clang_query source code static analysis checks.")

        # hook our new target into the proper dependency chain
        add_dependencies(clang_query_check ${arg_NAME})
        add_dependencies(interactive_clang_query_check ${interactive_target_name})

        # Code check targets should only be run on demand
        set_property(TARGET ${interactive_target_name} PROPERTY EXCLUDE_FROM_ALL TRUE)
        set_property(TARGET ${interactive_target_name} PROPERTY EXCLUDE_FROM_DEFAULT_BUILD TRUE)
        set_property(TARGET ${arg_NAME} PROPERTY EXCLUDE_FROM_ALL TRUE)
        set_property(TARGET ${arg_NAME} PROPERTY EXCLUDE_FROM_DEFAULT_BUILD TRUE)
    endif()
endmacro(blt_add_clang_query_target)

##-----------------------------------------------------------------------------
## blt_add_clang_tidy_target( NAME              <Created Target Name>
##                            WORKING_DIRECTORY <Working Directory>
##                            COMMENT           <Additional Comment for Target Invocation>
##                            CHECKS            <If specified, enables a specific set of checks>
##                            FIX               <If true, apply fixes>
##                            SRC_FILES         [FILE1 [FILE2 ...]] )
##
## Creates a new target with the given NAME for running clang-tidy over the given SRC_FILES
##-----------------------------------------------------------------------------
macro(blt_add_clang_tidy_target)
    if(CLANGTIDY_FOUND)

        ## parse the arguments to the macro
        set(options)
        set(singleValueArgs NAME COMMENT WORKING_DIRECTORY FIX)
        set(multiValueArgs SRC_FILES CHECKS)

        cmake_parse_arguments(arg
            "${options}" "${singleValueArgs}" "${multiValueArgs}" ${ARGN} )

        # Check required parameters
        if(NOT DEFINED arg_NAME)
             message(FATAL_ERROR "blt_add_clang_tidy_target requires a NAME parameter")
        endif()

        if(NOT DEFINED arg_SRC_FILES)
            message(FATAL_ERROR "blt_add_clang_tidy_target requires a SRC_FILES parameter")
        endif()

        if(DEFINED arg_WORKING_DIRECTORY)
            set(_wd ${arg_WORKING_DIRECTORY})
        else()
            set(_wd ${CMAKE_CURRENT_SOURCE_DIR})
        endif()
   
        set(CLANG_TIDY_HELPER_SCRIPT ${BLT_ROOT_DIR}/cmake/run-clang-tidy.py)
        set(CLANG_TIDY_HELPER_COMMAND ${CLANG_TIDY_HELPER_SCRIPT} -clang-tidy-binary=${CLANGTIDY_EXECUTABLE} -p ${CMAKE_BINARY_DIR})

        if(arg_FIX)
            set(CLANG_TIDY_HELPER_COMMAND ${CLANG_TIDY_HELPER_COMMAND} -fix)
        endif()

        if(DEFINED arg_CHECKS)
            STRING(REGEX REPLACE " " "," CHECK_ARG_STRING ${arg_CHECKS})
            add_custom_target(${arg_NAME}
              COMMAND ${CLANG_TIDY_HELPER_COMMAND} -checks=${CHECK_ARG_STRING} ${arg_SRC_FILES}
                    WORKING_DIRECTORY ${_wd}
                    COMMENT "${arg_COMMENT}Running specified clang-tidy source code static analysis checks.")
        else() #DEFINED CHECKERS
            add_custom_target(${arg_NAME}
              COMMAND ${CLANG_TIDY_HELPER_COMMAND} ${arg_SRC_FILES}
                    WORKING_DIRECTORY ${_wd}
                    COMMENT "${arg_COMMENT}Running default clang-tidy source code static analysis checks.")
        endif()

        # hook our new target into the proper dependency chain
        add_dependencies(clang_tidy_check ${arg_NAME})

        # Code check targets should only be run on demand
        set_property(TARGET ${arg_NAME} PROPERTY EXCLUDE_FROM_ALL TRUE)
        set_property(TARGET ${arg_NAME} PROPERTY EXCLUDE_FROM_DEFAULT_BUILD TRUE)
    endif()
endmacro(blt_add_clang_tidy_target)


##-----------------------------------------------------------------------------
## blt_add_cppcheck_target( NAME                <Created Target Name>
##                          WORKING_DIRECTORY   <Working Directory>
##                          PREPEND_FLAGS       <additional flags for cppcheck>
##                          APPEND_FLAGS        <additional flags for cppcheck>
##                          COMMENT             <Additional Comment for Target Invocation>
##                          SRC_FILES           [FILE1 [FILE2 ...]] )
##
## Creates a new target with the given NAME for running cppcheck over the given SRC_FILES
##-----------------------------------------------------------------------------
macro(blt_add_cppcheck_target)

    ## parse the arguments to the macro
    set(options)
    set(singleValueArgs NAME COMMENT WORKING_DIRECTORY)
    set(multiValueArgs SRC_FILES PREPEND_FLAGS APPEND_FLAGS)

    cmake_parse_arguments(arg
        "${options}" "${singleValueArgs}" "${multiValueArgs}" ${ARGN} )

    # Check required parameters
    if(NOT DEFINED arg_NAME)
        message(FATAL_ERROR "blt_add_cppcheck_target requires a NAME parameter")
    endif()

    if(NOT DEFINED arg_SRC_FILES)
        message(FATAL_ERROR "blt_add_cppcheck_target requires a SRC_FILES parameter")
    endif()

    if(DEFINED arg_WORKING_DIRECTORY)
        set(_wd ${arg_WORKING_DIRECTORY})
    else()
        set(_wd ${CMAKE_CURRENT_SOURCE_DIR})
    endif()

    add_custom_target(${arg_NAME}
            COMMAND ${CPPCHECK_EXECUTABLE} ${arg_PREPEND_FLAGS} ${arg_SRC_FILES} ${arg_APPEND_FLAGS}
            WORKING_DIRECTORY ${_wd}
            COMMENT "${arg_COMMENT}Running cppcheck source code static analysis checks.")

    # hook our new target into the proper dependency chain
    add_dependencies(cppcheck_check ${arg_NAME})

    # Code check targets should only be run on demand
    set_property(TARGET ${arg_NAME} PROPERTY EXCLUDE_FROM_ALL TRUE)
    set_property(TARGET ${arg_NAME} PROPERTY EXCLUDE_FROM_DEFAULT_BUILD TRUE)
endmacro(blt_add_cppcheck_target)


##------------------------------------------------------------------------------
## blt_add_astyle_target( NAME              <Created Target Name>
##                        MODIFY_FILES      [TRUE | FALSE (default)]
##                        CFG_FILE          <AStyle Configuration File> 
##                        PREPEND_FLAGS     <Additional Flags to AStyle>
##                        APPEND_FLAGS      <Additional Flags to AStyle>
##                        COMMENT           <Additional Comment for Target Invocation>
##                        WORKING_DIRECTORY <Working Directory>
##                        SRC_FILES         [FILE1 [FILE2 ...]] )
##
## Creates a new target with the given NAME for running astyle over the given SRC_FILES.
##------------------------------------------------------------------------------
macro(blt_add_astyle_target)

    ## parse the arguments to the macro
    set(options)
    set(singleValueArgs NAME MODIFY_FILES CFG_FILE COMMENT WORKING_DIRECTORY)
    set(multiValueArgs SRC_FILES PREPEND_FLAGS APPEND_FLAGS)

    cmake_parse_arguments(arg
        "${options}" "${singleValueArgs}" "${multiValueArgs}" ${ARGN} )

    # Check/Set required parameters
    if(NOT DEFINED arg_NAME)
        message(FATAL_ERROR "blt_add_astyle_target requires a NAME parameter")
    endif()

    if(NOT DEFINED arg_CFG_FILE)
        message(FATAL_ERROR "blt_add_astyle_target requires a CFG_FILE parameter")
    endif()

    if(NOT DEFINED arg_SRC_FILES)
        message(FATAL_ERROR "blt_add_astyle_target requires a SRC_FILES parameter")
    endif()

    if(NOT DEFINED arg_MODIFY_FILES)
        set(arg_MODIFY_FILES FALSE)
    endif()

    if(DEFINED arg_WORKING_DIRECTORY)
        set(_wd ${arg_WORKING_DIRECTORY})
    else()
        set(_wd ${CMAKE_CURRENT_SOURCE_DIR})
    endif()

    set(_generate_target TRUE)

    # Check the version -- output is of the form "Artistic Style Version X.Y.Z"
    execute_process(
        COMMAND ${ASTYLE_EXECUTABLE} --version
        OUTPUT_VARIABLE _version_str
        ERROR_VARIABLE  _version_str
        OUTPUT_STRIP_TRAILING_WHITESPACE )
    string(REGEX MATCH "([0-9]+(\\.)?)+$" _astyle_version ${_version_str})

    if(BLT_REQUIRED_ASTYLE_VERSION)
        # The user may only specify a part of the version (e.g. just the maj ver)
        # so check for substring
        string(FIND "${_astyle_version}" ${BLT_REQUIRED_ASTYLE_VERSION} VERSION_POS)
        if (NOT VERSION_POS EQUAL 0)
            set(_generate_target FALSE)
            if(NOT _BLT_STYLE_VERSION_WARNING_ISSUED)
                message(WARNING "blt_add_astyle_target: astyle '${BLT_REQUIRED_ASTYLE_VERSION}' is required, found '${_astyle_version}'. Disabling 'style' build target.")
                set(_BLT_STYLE_VERSION_WARNING_ISSUED TRUE CACHE BOOL "Limits BLT issuing more than one warning for style version" FORCE)
            endif()
        endif()
    endif()

    if(${arg_MODIFY_FILES})
        set(MODIFY_FILES_FLAG --suffix=none)
    else()
        set(MODIFY_FILES_FLAG --dry-run)
        # Skip 'check' target if version is not high enough 
        if(_astyle_version VERSION_LESS 2.05)
            set(_generate_target FALSE)
            message(WARNING "blt_add_astyle_target requires AStyle v2.05 or greater "
                            " for style check targets. "
                            " Current AStyle executable: '${ASTYLE_EXECUTABLE}' "
                            " Current AStyle version is: ${_astyle_version}."    )
        endif()
    endif()

    if(_generate_target)

        # AStyle doesn't report failure when there are files that require formatting.
        # Fix this with a wrapper script that parses the output.
        set(wrapped_astyle_script ${CMAKE_CURRENT_BINARY_DIR}/WrapAstyle_${arg_NAME}.cmake)

        configure_file(
            ${BLT_ROOT_DIR}/cmake/WrapAstyle.cmake.in
            ${wrapped_astyle_script}
            @ONLY )

        add_custom_target(
            ${arg_NAME}
            COMMAND ${CMAKE_COMMAND} -P ${wrapped_astyle_script}
            WORKING_DIRECTORY ${_wd}
            COMMENT "${arg_COMMENT}Running AStyle source code formatting checks.")
        
        # Hook our new target into the proper dependency chain
        if(${arg_MODIFY_FILES})
            add_dependencies(astyle_style ${arg_NAME})
        else()
            add_dependencies(astyle_check ${arg_NAME})
        endif()

        # Code formatting targets should only be run on demand
        set_property(TARGET ${arg_NAME} PROPERTY EXCLUDE_FROM_ALL TRUE)
        set_property(TARGET ${arg_NAME} PROPERTY EXCLUDE_FROM_DEFAULT_BUILD TRUE)
    endif()
endmacro(blt_add_astyle_target)

##------------------------------------------------------------------------------
## blt_add_clangformat_target( NAME              <Created Target Name>
##                             MODIFY_FILES      [TRUE | FALSE (default)]
##                             CFG_FILE          <ClangFormat Configuration File> 
##                             PREPEND_FLAGS     <Additional Flags to ClangFormat>
##                             APPEND_FLAGS      <Additional Flags to ClangFormat>
##                             COMMENT           <Additional Comment for Target Invocation>
##                             WORKING_DIRECTORY <Working Directory>
##                             SRC_FILES         [FILE1 [FILE2 ...]] )
##
## Creates a new target with the given NAME for running ClangFormat over the given SRC_FILES.
##------------------------------------------------------------------------------
macro(blt_add_clangformat_target)

    ## parse the arguments to the macro
    set(options)
    set(singleValueArgs NAME MODIFY_FILES CFG_FILE COMMENT WORKING_DIRECTORY)
    set(multiValueArgs SRC_FILES PREPEND_FLAGS APPEND_FLAGS)

    cmake_parse_arguments(arg
        "${options}" "${singleValueArgs}" "${multiValueArgs}" ${ARGN} )

    # Check/Set required parameters
    if(NOT DEFINED arg_NAME)
        message(FATAL_ERROR "blt_add_clangformat_target requires a NAME parameter")
    endif()

    if(NOT DEFINED arg_CFG_FILE)
        message(FATAL_ERROR "blt_add_clangformat_target requires a CFG_FILE parameter")
    endif()

    if(NOT DEFINED arg_SRC_FILES)
        message(FATAL_ERROR "blt_add_clangformat_target requires a SRC_FILES parameter")
    endif()

    if(NOT DEFINED arg_MODIFY_FILES)
        set(arg_MODIFY_FILES FALSE)
    endif()

    if(DEFINED arg_WORKING_DIRECTORY)
        set(_wd ${arg_WORKING_DIRECTORY})
    else()
        set(_wd ${CMAKE_CURRENT_SOURCE_DIR})
    endif()

    set(_generate_target TRUE)

    # If a required version was set, check it
    if(BLT_REQUIRED_CLANGFORMAT_VERSION)
        execute_process(COMMAND ${CLANGFORMAT_EXECUTABLE} --version
                        OUTPUT_VARIABLE _version_str
                        OUTPUT_STRIP_TRAILING_WHITESPACE)
        # The version number is the last token - can contain non-numeric
        string(REGEX MATCH "([0-9a-zA-Z\\-]+\\.[0-9a-zA-Z\\-]+\\.?[0-9a-zA-Z\\-]?)"
               _clangformat_version ${_version_str})
        # The user may only specify a part of the version (e.g. just the maj ver)
        # so check for substring
        string(FIND "${_clangformat_version}" ${BLT_REQUIRED_CLANGFORMAT_VERSION} VERSION_POS)
        if (NOT VERSION_POS EQUAL 0)
            set(_generate_target FALSE)
            if (NOT _BLT_STYLE_VERSION_WARNING_ISSUED)
                message(WARNING "blt_add_clangformat_target: clang-format '${BLT_REQUIRED_CLANGFORMAT_VERSION}'' is required, found '${_clangformat_version}'. Disabling 'style' build target.")
                set(_BLT_STYLE_VERSION_WARNING_ISSUED TRUE CACHE BOOL "Limits BLT issuing more than one warning for style version" FORCE)
            endif()
        endif()
    endif()

    if(_generate_target)
        # Copy config file to given working directory since ClangFormat doesn't support pointing to one
        configure_file(${arg_CFG_FILE} ${arg_WORKING_DIRECTORY}/.clang-format COPYONLY)

        # ClangFormat does not support --dry-run until version 10 which isn't on many machines.
        # For now, use run-clang-format for dry running purposes.
        if(${arg_MODIFY_FILES})
            add_custom_target(${arg_NAME}
                    COMMAND  ${CLANGFORMAT_EXECUTABLE} ${arg_PREPEND_FLAGS}
                        --style=file -i ${arg_SRC_FILES} ${arg_APPEND_FLAGS}
                    WORKING_DIRECTORY ${_wd} 
                    COMMENT "${arg_COMMENT}Running ClangFormat source code formatting checks.")
        else()
            set(_run_clangformat "${BLT_ROOT_DIR}/cmake/run-clang-format.py" --clang-format-executable ${CLANGFORMAT_EXECUTABLE})
            add_custom_target(${arg_NAME}
                    COMMAND ${_run_clangformat} -j1 ${arg_SRC_FILES}
                    WORKING_DIRECTORY ${_wd} 
                    COMMENT "${arg_COMMENT}Running ClangFormat source code formatting checks.")

        endif()

        # Hook our new target into the proper dependency chain
        if(${arg_MODIFY_FILES})
            add_dependencies(clangformat_style ${arg_NAME})
        else()
            add_dependencies(clangformat_check ${arg_NAME})
        endif()

        # Code formatting targets should only be run on demand
        set_property(TARGET ${arg_NAME} PROPERTY EXCLUDE_FROM_ALL TRUE)
        set_property(TARGET ${arg_NAME} PROPERTY EXCLUDE_FROM_DEFAULT_BUILD TRUE)
    endif()
endmacro(blt_add_clangformat_target)

##------------------------------------------------------------------------------
## blt_add_uncrustify_target( NAME              <Created Target Name>
##                            MODIFY_FILES      [TRUE | FALSE (default)]
##                            CFG_FILE          <Uncrustify Configuration File> 
##                            PREPEND_FLAGS     <Additional Flags to Uncrustify>
##                            APPEND_FLAGS      <Additional Flags to Uncrustify>
##                            COMMENT           <Additional Comment for Target Invocation>
##                            WORKING_DIRECTORY <Working Directory>
##                            SRC_FILES         [FILE1 [FILE2 ...]] )
##
## Creates a new target with the given NAME for running uncrustify over the given SRC_FILES.
##------------------------------------------------------------------------------
macro(blt_add_uncrustify_target)
    
    ## parse the arguments to the macro
    set(options)
    set(singleValueArgs NAME MODIFY_FILES CFG_FILE COMMENT WORKING_DIRECTORY)
    set(multiValueArgs SRC_FILES PREPEND_FLAGS APPEND_FLAGS)

    cmake_parse_arguments(arg
        "${options}" "${singleValueArgs}" "${multiValueArgs}" ${ARGN} )

    # Check/Set required parameters
    if(NOT DEFINED arg_NAME)
        message(FATAL_ERROR "blt_add_uncrustify_target requires a NAME parameter")
    endif()

    if(NOT DEFINED arg_CFG_FILE)
        message(FATAL_ERROR "blt_add_uncrustify_target requires a CFG_FILE parameter")
    endif()

    if(NOT DEFINED arg_SRC_FILES)
        message(FATAL_ERROR "blt_add_uncrustify_target requires a SRC_FILES parameter")
    endif()

    if(NOT DEFINED arg_MODIFY_FILES)
        set(arg_MODIFY_FILES FALSE)
    endif()

    if(DEFINED arg_WORKING_DIRECTORY)
        set(_wd ${arg_WORKING_DIRECTORY})
    else()
        set(_wd ${CMAKE_CURRENT_SOURCE_DIR})
    endif()

    set(_generate_target TRUE)

    # Check the version -- output is of the form "uncrustify X.Y.Z"
    execute_process(
        COMMAND ${UNCRUSTIFY_EXECUTABLE} --version
        OUTPUT_VARIABLE _version_str
        OUTPUT_STRIP_TRAILING_WHITESPACE )
    string(REGEX MATCH "([0-9]+(\\.)?)+(_[a-zA-Z])?" _uncrustify_version ${_version_str})

    if(BLT_REQUIRED_UNCRUSTIFY_VERSION)
        # The user may only specify a part of the version (e.g. just the maj ver)
        # so check for substring
        string(FIND "${_uncrustify_version}" ${BLT_REQUIRED_UNCRUSTIFY_VERSION} VERSION_POS)
        if (NOT VERSION_POS EQUAL 0)
            set(_generate_target FALSE)
            if (NOT _BLT_STYLE_VERSION_WARNING_ISSUED)
                message(WARNING "blt_add_uncrustify_target: uncrustify '${BLT_REQUIRED_UNCRUSTIFY_VERSION}' is required, found '${_uncrustify_version}'. Disabling 'style' build target.")
                set(_BLT_STYLE_VERSION_WARNING_ISSUED TRUE CACHE BOOL "Limits BLT issuing more than one warning for style version" FORCE)
            endif()
        endif()
    endif()

    if(${arg_MODIFY_FILES})
        set(MODIFY_FILES_FLAG --replace;--no-backup)
    else()
        set(MODIFY_FILES_FLAG "--check")

        # Skip 'check' target if version is not high enough 
        if(_uncrustify_version VERSION_LESS 0.61)
            set(_generate_target FALSE)
            message(WARNING "blt_add_uncrustify_target requires uncrustify v0.61 or greater "
                            " for style check targets. "
                            " Current uncrustify executable: '${UNCRUSTIFY_EXECUTABLE}' "
                            " Current uncrustify version is: ${_uncrustify_version}."    )
        endif()
    endif()

    if(_generate_target)
        add_custom_target(${arg_NAME}
                COMMAND ${UNCRUSTIFY_EXECUTABLE} ${arg_PREPEND_FLAGS}
                    -c ${arg_CFG_FILE} ${MODIFY_FILES_FLAG} ${arg_SRC_FILES} ${arg_APPEND_FLAGS}
                WORKING_DIRECTORY ${_wd} 
                COMMENT "${arg_COMMENT}Running uncrustify source code formatting checks.")
            
        # hook our new target into the proper dependency chain
        if(${arg_MODIFY_FILES})
            add_dependencies(uncrustify_style ${arg_NAME})
        else()
            add_dependencies(uncrustify_check ${arg_NAME})
        endif()

        # Code formatting targets should only be run on demand
        set_property(TARGET ${arg_NAME} PROPERTY EXCLUDE_FROM_ALL TRUE)
        set_property(TARGET ${arg_NAME} PROPERTY EXCLUDE_FROM_DEFAULT_BUILD TRUE)
    endif()

endmacro(blt_add_uncrustify_target)


##------------------------------------------------------------------------------
## blt_add_yapf_target( NAME              <Created Target Name>
##                      MODIFY_FILES      [TRUE | FALSE (default)]
##                      CFG_FILE          <Yapf Configuration File>
##                      PREPEND_FLAGS     <Additional Flags to Yapf>
##                      APPEND_FLAGS      <Additional Flags to Yapf>
##                      COMMENT           <Additional Comment for Target Invocation>
##                      WORKING_DIRECTORY <Working Directory>
##                      SRC_FILES         [FILE1 [FILE2 ...]] )
##
## Creates a new target with the given NAME for running yapf over the given SRC_FILES.
##------------------------------------------------------------------------------
macro(blt_add_yapf_target)

    ## parse the arguments to the macro
    set(options)
    set(singleValueArgs NAME MODIFY_FILES CFG_FILE COMMENT WORKING_DIRECTORY)
    set(multiValueArgs SRC_FILES PREPEND_FLAGS APPEND_FLAGS)

    cmake_parse_arguments(arg
        "${options}" "${singleValueArgs}" "${multiValueArgs}" ${ARGN} )

    # Check/Set required parameters
    if(NOT DEFINED arg_NAME)
        message(FATAL_ERROR "blt_add_yapf_target requires a NAME parameter")
    endif()

    if(NOT DEFINED arg_CFG_FILE)
        message(FATAL_ERROR "blt_add_yapf_target requires a CFG_FILE parameter")
    endif()

    if(NOT DEFINED arg_SRC_FILES)
        message(FATAL_ERROR "blt_add_yapf_target requires a SRC_FILES parameter")
    endif()

    if(NOT DEFINED arg_MODIFY_FILES)
        set(arg_MODIFY_FILES FALSE)
    endif()

    if(DEFINED arg_WORKING_DIRECTORY)
        set(_wd ${arg_WORKING_DIRECTORY})
    else()
        set(_wd ${CMAKE_CURRENT_SOURCE_DIR})
    endif()

    set(_generate_target TRUE)

    # Check the version -- output is of the form "yapf X.Y.Z"
    execute_process(
        COMMAND ${YAPF_EXECUTABLE} --version
        OUTPUT_VARIABLE _version_str
        ERROR_VARIABLE  _version_str
        OUTPUT_STRIP_TRAILING_WHITESPACE )
    string(REGEX MATCH "([0-9]+(\\.)?)+$" _yapf_version ${_version_str})

    if(BLT_REQUIRED_YAPF_VERSION)
        # The user may only specify a part of the version (e.g. just the maj ver)
        # so check for substring
        string(FIND "${_yapf_version}" ${BLT_REQUIRED_YAPF_VERSION} VERSION_POS)
        if (NOT VERSION_POS EQUAL 0)
            set(_generate_target FALSE)
            if (NOT _BLT_STYLE_VERSION_WARNING_ISSUED)
                message(WARNING "blt_add_yapf_target: yapf ${BLT_REQUIRED_YAPF_VERSION} is required, found ${_yapf_version}.  Disabling 'style' build target.")
                set(_BLT_STYLE_VERSION_WARNING_ISSUED TRUE CACHE BOOL "Limits BLT issuing more than one warning for style version" FORCE)
            endif()
        endif()
    endif()

    if(${arg_MODIFY_FILES})
        set(MODIFY_FILES_FLAG --in-place)
    else()
        set(MODIFY_FILES_FLAG --diff)
    endif()

    if(_generate_target)
        add_custom_target(
            ${arg_NAME}
            COMMAND ${YAPF_EXECUTABLE} ${arg_PREPEND_FLAGS}
                --style ${arg_CFG_FILE} ${MODIFY_FILES_FLAG} ${arg_SRC_FILES} ${arg_APPEND_FLAGS}
            WORKING_DIRECTORY ${_wd}
            COMMENT "${arg_COMMENT}Running Yapf source code formatting checks.")

        # Hook our new target into the proper dependency chain
        if(${arg_MODIFY_FILES})
            add_dependencies(yapf_style ${arg_NAME})
        else()
            add_dependencies(yapf_check ${arg_NAME})
        endif()

        # Code formatting targets should only be run on demand
        set_property(TARGET ${arg_NAME} PROPERTY EXCLUDE_FROM_ALL TRUE)
        set_property(TARGET ${arg_NAME} PROPERTY EXCLUDE_FROM_DEFAULT_BUILD TRUE)
    endif()
endmacro(blt_add_yapf_target)


##------------------------------------------------------------------------------
## blt_add_cmakeformat_target( NAME              <Created Target Name>
##                             MODIFY_FILES      [TRUE | FALSE (default)]
##                             CFG_FILE          <cmake-format Configuration File>
##                             PREPEND_FLAGS     <Additional Flags to cmake-format>
##                             APPEND_FLAGS      <Additional Flags to cmake-format>
##                             COMMENT           <Additional Comment for Target Invocation>
##                             WORKING_DIRECTORY <Working Directory>
##                             SRC_FILES         [FILE1 [FILE2 ...]] )
##
## Creates a new target with the given NAME for running cmake-format over the given SRC_FILES.
##------------------------------------------------------------------------------
macro(blt_add_cmakeformat_target)

    ## parse the arguments to the macro
    set(options)
    set(singleValueArgs NAME MODIFY_FILES CFG_FILE COMMENT WORKING_DIRECTORY)
    set(multiValueArgs SRC_FILES PREPEND_FLAGS APPEND_FLAGS)

    cmake_parse_arguments(arg
        "${options}" "${singleValueArgs}" "${multiValueArgs}" ${ARGN} )

    # Check/Set required parameters
    if(NOT DEFINED arg_NAME)
        message(FATAL_ERROR "blt_add_cmakeformat_target requires a NAME parameter")
    endif()

    if(NOT DEFINED arg_CFG_FILE)
        message(FATAL_ERROR "blt_add_cmakeformat_target requires a CFG_FILE parameter")
    endif()

    if(NOT DEFINED arg_SRC_FILES)
        message(FATAL_ERROR "blt_add_cmakeformat_target requires a SRC_FILES parameter")
    endif()

    if(NOT DEFINED arg_MODIFY_FILES)
        set(arg_MODIFY_FILES FALSE)
    endif()

    if(DEFINED arg_WORKING_DIRECTORY)
        set(_wd ${arg_WORKING_DIRECTORY})
    else()
        set(_wd ${CMAKE_CURRENT_SOURCE_DIR})
    endif()

    set(_generate_target TRUE)

    # Check the version -- output is of the form "X.Y.Z"
    execute_process(
        COMMAND ${CMAKEFORMAT_EXECUTABLE} --version
        OUTPUT_VARIABLE _version_str
        ERROR_VARIABLE  _version_str
        OUTPUT_STRIP_TRAILING_WHITESPACE )
    string(REGEX MATCH "([0-9]+(\\.)?)+$" _cmakeformat_version ${_version_str})

    if(BLT_REQUIRED_CMAKEFORMAT_VERSION)
        # The user may only specify a part of the version (e.g. just the maj ver)
        # so check for substring
        string(FIND "${_cmakeformat_version}" ${BLT_REQUIRED_CMAKEFORMAT_VERSION} VERSION_POS)
        if (NOT VERSION_POS EQUAL 0)
            set(_generate_target FALSE)
            if(NOT _BLT_STYLE_VERSION_WARNING_ISSUED)
                message(WARNING "blt_add_cmakeformat_target: cmake-format '${BLT_REQUIRED_CMAKEFORMAT_VERSION}' is required, found '${_cmakeformat_version}'. Disabling 'style' build target.")
                set(_BLT_STYLE_VERSION_WARNING_ISSUED TRUE CACHE BOOL "Limits BLT issuing more than one warning for style version" FORCE)
            endif()
        endif()
    endif()

    if(${arg_MODIFY_FILES})
        set(MODIFY_FILES_FLAG --in-place)
    else()
        set(MODIFY_FILES_FLAG --check)
    endif()

    if(_generate_target)
        add_custom_target(
            ${arg_NAME}
            COMMAND ${CMAKEFORMAT_EXECUTABLE} ${arg_PREPEND_FLAGS}
                --config-files  ${arg_CFG_FILE} ${MODIFY_FILES_FLAG} ${arg_SRC_FILES} ${arg_APPEND_FLAGS}
            WORKING_DIRECTORY ${_wd}
            COMMENT "${arg_COMMENT}Running CMakeFormat source code formatting checks.")
        
        # Hook our new target into the proper dependency chain
        if(${arg_MODIFY_FILES})
            add_dependencies(cmakeformat_style ${arg_NAME})
        else()
            add_dependencies(cmakeformat_check ${arg_NAME})
        endif()

        # Code formatting targets should only be run on demand
        set_property(TARGET ${arg_NAME} PROPERTY EXCLUDE_FROM_ALL TRUE)
        set_property(TARGET ${arg_NAME} PROPERTY EXCLUDE_FROM_DEFAULT_BUILD TRUE)
    endif()
endmacro(blt_add_cmakeformat_target)
