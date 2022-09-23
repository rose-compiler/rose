# Copyright (c) 2017-2022, Lawrence Livermore National Security, LLC and
# other BLT Project Developers. See the top-level LICENSE file for details
# 
# SPDX-License-Identifier: (BSD-3-Clause)

##------------------------------------------------------------------------------
## blt_git( SOURCE_DIR <dir>
##          GIT_COMMAND <command>
##          OUTPUT_VARIABLE <out>
##          RETURN_CODE <rc>
##          [QUIET] )
##
## Runs the supplied git command on the given Git repository.
##------------------------------------------------------------------------------
macro(blt_git)

    set(options)
    set(singleValueArgs SOURCE_DIR OUTPUT_VARIABLE RETURN_CODE)
    set(multiValueArgs GIT_COMMAND )

    ## parse macro arguments
    cmake_parse_arguments(arg
         "${options}" "${singleValueArgs}" "${multiValueArgs}" ${ARGN} )

    ## ensure required arguments are supplied
    if ( NOT DEFINED arg_SOURCE_DIR )
      message(FATAL_ERROR "SOURCE_DIR is a required argument to blt_git()")
    endif()

    if ( NOT DEFINED arg_GIT_COMMAND )
      message(FATAL_ERROR "GIT_COMMAND is a required argument to blt_git()")
    endif()

    if ( NOT DEFINED arg_OUTPUT_VARIABLE )
      message(FATAL_ERROR "OUTPUT_VARIABLE is a required argument to blt_git()")
    endif()

    if ( NOT DEFINED arg_RETURN_CODE )
      message(FATAL_ERROR "RETURN_CODE is a required argument to blt_git()")
    endif()

    ## check arguments
    if (GIT_FOUND)

      ## assemble the Git command
      set(git_cmd "${GIT_EXECUTABLE}" "${arg_GIT_COMMAND}" )

      ## run it
      execute_process( COMMAND
                         ${git_cmd}
                       WORKING_DIRECTORY
                         "${arg_SOURCE_DIR}"
                       RESULT_VARIABLE
                         ${arg_RETURN_CODE}
                       OUTPUT_VARIABLE
                         ${arg_OUTPUT_VARIABLE}
                       ERROR_QUIET
                       OUTPUT_STRIP_TRAILING_WHITESPACE
                       ERROR_STRIP_TRAILING_WHITESPACE
                       )

    else( )
       message( FATAL_ERROR "Git is not found. Git is required for blt_git()")
    endif()

endmacro(blt_git)


##------------------------------------------------------------------------------
## blt_is_git_repo( OUTPUT_STATE <state>
##                  [SOURCE_DIR <dir>] )
##
## Checks if we are working with a valid Git repository.
##------------------------------------------------------------------------------
macro(blt_is_git_repo)

    set(options)
    set(singleValueArgs OUTPUT_STATE SOURCE_DIR )
    set(multiValueArgs)

    ## parse macro arguments
    cmake_parse_arguments(arg
        "${options}" "${singleValueArgs}" "${multiValueArgs}" ${ARGN})

    ## ensure required variables are supplied
    if ( NOT DEFINED arg_OUTPUT_STATE )
       message(FATAL_ERROR "OUTPUT_STATE is a required argument to blt_is_git_repo")
    endif()

    ## check if SOURCE_DIR was supplied
    if ( NOT DEFINED arg_SOURCE_DIR )
      set(git_dir ${CMAKE_CURRENT_SOURCE_DIR})
    else()
      set(git_dir ${arg_SOURCE_DIR})
    endif()

    blt_git( SOURCE_DIR ${git_dir}
             GIT_COMMAND rev-parse --show-toplevel
             OUTPUT_VARIABLE tmp
             RETURN_CODE rc
             )

    if ( NOT ${rc} EQUAL 0 )
       ## rev-parse failed, this is not a git repo
       set( ${arg_OUTPUT_STATE} FALSE )
    else()
       set( ${arg_OUTPUT_STATE} TRUE )
    endif()

endmacro(blt_is_git_repo)

##------------------------------------------------------------------------------
## blt_git_tag( OUTPUT_TAG <tag>
##              RETURN_CODE <rc>
##              [SOURCE_DIR <dir>]
##              [ON_BRANCH <branch>] )
##
## Returns the latest tag on a corresponding Git repository.
##------------------------------------------------------------------------------
macro(blt_git_tag)

    set(options)
    set(singleValueArgs SOURCE_DIR ON_BRANCH OUTPUT_TAG RETURN_CODE )
    set(multiValueArgs)

    ## parse macro arguments
    cmake_parse_arguments(arg
        "${options}" "${singleValueArgs}" "${multiValueArgs}" ${ARGN})

    ## ensure required arguments are supplied
    if ( NOT DEFINED arg_OUTPUT_TAG )
       message(FATAL_ERROR "OUTPUT_TAG is a required argument to blt_git_tag")
    endif()

    if ( NOT DEFINED arg_RETURN_CODE )
       message(FATAL_ERROR "RETURN_CODE is a required argument to blt_git_tag")
    endif()

    ## git command to execute
    if ( NOT DEFINED arg_ON_BRANCH )
      set(git_cmd describe --tags )
    else()
      set(git_cmd describe --tags ${arg_ON_BRANCH} )
    endif()

    ## set working directory
    if ( NOT DEFINED arg_SOURCE_DIR )
      set(git_dir ${CMAKE_CURRENT_SOURCE_DIR})
    else()
      set(git_dir ${arg_SOURCE_DIR})
    endif()

    blt_git( SOURCE_DIR ${git_dir}
             GIT_COMMAND ${git_cmd}
             OUTPUT_VARIABLE ${arg_OUTPUT_TAG}
             RETURN_CODE ${arg_RETURN_CODE}
             )

endmacro(blt_git_tag)

##------------------------------------------------------------------------------
## blt_git_branch( BRANCH_NAME <branch>
##                 RETURN_CODE <rc>
##                 [SOURCE_DIR <dir>] )
##
## Returns the name of the active branch in the checkout space.
##------------------------------------------------------------------------------
macro(blt_git_branch)

    set(options)
    set(singleValueArgs SOURCE_DIR BRANCH_NAME RETURN_CODE)
    set(multiValueArgs)

    ## parse macro arguments
    cmake_parse_arguments(arg
        "${options}" "${singleValueArgs}" "${multiValueArgs}" ${ARGN})

    ## ensure required arguments are supplied
    if ( NOT DEFINED arg_BRANCH_NAME )
       message(FATAL_ERROR "BRANCH_NAME is a required argument to blt_git_branch" )
    endif()

    if ( NOT DEFINED arg_RETURN_CODE )
       message(FATAL_ERROR "RETURN_CODE is a required argument to blt_git_branch")
    endif()

    ## set set working directory
    if ( NOT DEFINED arg_SOURCE_DIR )
       set(git_dir ${CMAKE_CURRENT_SOURCE_DIR})
    else()
       set(git_dir ${arg_SOURCE_DIR})
    endif()

    blt_git( SOURCE_DIR ${git_dir}
             GIT_COMMAND rev-parse --abbrev-ref HEAD
             OUTPUT_VARIABLE ${arg_BRANCH_NAME}
             RETURN_CODE ${arg_RETURN_CODE}
             )

endmacro(blt_git_branch)

##------------------------------------------------------------------------------
## blt_git_hashcode( HASHCODE <hc>
##                   RETURN_CODE <rc>
##                   [SOURCE_DIR <dir>]
##                   [ON_BRANCH <branch>]
##                   )
##
## Returns the SHA-1 hashcode at the tip of a branch.
##------------------------------------------------------------------------------
macro(blt_git_hashcode)

    set(options)
    set(singleValueArgs SOURCE_DIR HASHCODE ON_BRANCH RETURN_CODE)
    set(multiValueArgs)

    ## parse macro arguments
    cmake_parse_arguments(arg
        "${options}" "${singleValueArgs}" "${multiValueArgs}" ${ARGN} )

    ## ensure required arguments are supplied
    if ( NOT DEFINED arg_HASHCODE )
       message(FATAL_ERROR "HASHCODE is a required argument to blt_git_hashcode" )
    endif()

    if ( NOT DEFINED arg_RETURN_CODE )
       message(FATAL_ERROR "RETURN_CODE is a required argument to blt_git_hashcode" )
    endif()

    ## set working directory
    if ( NOT DEFINED arg_SOURCE_DIR )
      set(git_dir ${CMAKE_CURRENT_SOURCE_DIR})
    else()
      set(git_dir ${arg_SOURCE_DIR})
    endif()

    ## set target ref
    if ( NOT DEFINED arg_ON_BRANCH )
      set(git_cmd rev-parse --short HEAD )
    else()
      set(git_cmd rev-parse --short ${arg_ON_BRANCH} )
    endif()

     blt_git( SOURCE_DIR ${git_dir}
              GIT_COMMAND ${git_cmd}
              OUTPUT_VARIABLE ${arg_HASHCODE}
              RETURN_CODE ${arg_RETURN_CODE}
              )

endmacro(blt_git_hashcode)
