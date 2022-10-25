# Copyright (c) 2017-2022, Lawrence Livermore National Security, LLC and
# other BLT Project Developers. See the top-level LICENSE file for details
# 
# SPDX-License-Identifier: (BSD-3-Clause)

############################
# Setup compiler options
############################

#####################################################
# Set some variables to simplify determining compiler
# Compiler string list from:
#   https://cmake.org/cmake/help/latest/variable/CMAKE_LANG_COMPILER_ID.html
####################################################3

# use CMAKE_BUILD_TOOL to identify visual studio
# and CMAKE_CXX_COMPILER_ID for all other cases

if("${CMAKE_BUILD_TOOL}" MATCHES "(msdev|devenv|nmake|MSBuild)")
    set(COMPILER_FAMILY_IS_MSVC 1)
    message(STATUS "Compiler family is MSVC")

    if(CMAKE_GENERATOR_TOOLSET AND "${CMAKE_GENERATOR_TOOLSET}" MATCHES "Intel")
        set(COMPILER_FAMILY_IS_MSVC_INTEL 1) 
        message(STATUS "Toolset is ${CMAKE_GENERATOR_TOOLSET}")
    endif()
else()
    #Determine C/C++ compiler family. 
    if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
        set(C_COMPILER_FAMILY_IS_GNU 1)
        message(STATUS "C Compiler family is GNU")

    elseif("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang") # For Clang or AppleClang
        set(C_COMPILER_FAMILY_IS_CLANG 1)
        message(STATUS "C Compiler family is Clang")

    elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "XL")
        set(C_COMPILER_FAMILY_IS_XL 1)
        message(STATUS "C Compiler family is XL")

    elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Intel")
        set(C_COMPILER_FAMILY_IS_INTEL 1)
        message(STATUS "C Compiler family is Intel")

    elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "IntelLLVM")
        set(C_COMPILER_FAMILY_IS_INTELLLVM 1)
        message(STATUS "C Compiler family is IntelLLVM")

    elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "PGI")
        set(C_COMPILER_FAMILY_IS_PGI 1)
        message(STATUS "C Compiler family is PGI")

    elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Cray")
        set(C_COMPILER_FAMILY_IS_CRAY 1)
        message(STATUS "C Compiler family is Cray")

    else()
        message(STATUS "C Compiler family not set!!!")
    endif()
    # Determine Fortran compiler family 
    if("${CMAKE_Fortran_COMPILER_ID}" STREQUAL "GNU")
        set(Fortran_COMPILER_FAMILY_IS_GNU 1)
        message(STATUS "Fortran Compiler family is GNU")

    elseif("${CMAKE_Fortran_COMPILER_ID}" MATCHES "Clang") # For Clang or AppleClang
        set(Fortran_COMPILER_FAMILY_IS_CLANG 1)
        message(STATUS "Fortran Compiler family is Clang")

    elseif("${CMAKE_Fortran_COMPILER_ID}" MATCHES "Flang") # For Flang compilers
        set(Fortran_COMPILER_FAMILY_IS_CLANG 1 CACHE BOOL "")
        message(STATUS "Fortran Compiler family is Clang")

    elseif("${CMAKE_Fortran_COMPILER_ID}" STREQUAL "XL")
        set(Fortran_COMPILER_FAMILY_IS_XL 1)
        message(STATUS "Fortran Compiler family is XL")

    elseif("${CMAKE_Fortran_COMPILER_ID}" STREQUAL "Intel")
        set(Fortran_COMPILER_FAMILY_IS_INTEL 1)
        message(STATUS "Fortran Compiler family is Intel")

    elseif("${CMAKE_Fortran_COMPILER_ID}" STREQUAL "IntelLLVM")
        set(Fortran_COMPILER_FAMILY_IS_INTELLLVM 1)
        message(STATUS "Fortran Compiler family is IntelLLVM")

    elseif("${CMAKE_Fortran_COMPILER_ID}" STREQUAL "PGI")
        set(Fortran_COMPILER_FAMILY_IS_PGI 1)
        message(STATUS "Fortran Compiler family is PGI")

    elseif("${CMAKE_Fortran_COMPILER_ID}" STREQUAL "Cray")
        set(Fortran_COMPILER_FAMILY_IS_CRAY 1)
        message(STATUS "Fortran Compiler family is Cray")

    elseif(ENABLE_FORTRAN)
        message(STATUS "Fortran Compiler family not set!!!")
    endif()
endif()


#################################################################################
# PGI doesn't support a SYSTEM flag for include directories. Since this is CMake's
# default for imported targets, we need to disable this feature for PGI.
#################################################################################
if(${C_COMPILER_FAMILY_IS_PGI})
    set(CMAKE_NO_SYSTEM_FROM_IMPORTED TRUE)
endif()

################################################
# Support for extra compiler flags and defines
################################################

message(STATUS "Adding optional BLT definitions and compiler flags")

####################################################
# create relocatable static libs by default
####################################################
set(CMAKE_POSITION_INDEPENDENT_CODE TRUE)

##############################################
# Support extra definitions for all targets
##############################################
if(BLT_DEFINES)
    add_definitions(${BLT_DEFINES})
    message(STATUS "Added \"${BLT_DEFINES}\" to definitions")
endif()

if(COMPILER_FAMILY_IS_MSVC)
    # Visual studio can give a warning that /bigobj is required due to the size of some object files
    set( BLT_CXX_FLAGS "${BLT_CXX_FLAGS} /bigobj" )
    set( BLT_C_FLAGS   "${BLT_C_FLAGS} /bigobj" )
endif()

##########################################
# If set, BLT_<LANG>_FLAGS are added to 
# all targets that use <LANG>-Compiler
##########################################

if(BLT_C_FLAGS)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${BLT_C_FLAGS}")
    message(STATUS "Updated CMAKE_C_FLAGS to \"${CMAKE_C_FLAGS}\"")
endif()

if(BLT_CXX_FLAGS)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${BLT_CXX_FLAGS}")
    message(STATUS "Updated CMAKE_CXX_FLAGS to \"${CMAKE_CXX_FLAGS}\"")
endif()

if(ENABLE_FORTRAN AND BLT_FORTRAN_FLAGS)
    set(CMAKE_Fortran_FLAGS "${CMAKE_Fortran_FLAGS} ${BLT_FORTRAN_FLAGS}")
    message(STATUS "Updated CMAKE_Fortran_FLAGS to \"${CMAKE_Fortran_FLAGS}\"")
endif()

if(ENABLE_CUDA AND BLT_CUDA_FLAGS)
    set(CMAKE_CUDA_FLAGS "${CMAKE_CUDA_FLAGS} ${BLT_CUDA_FLAGS}")
    message(STATUS "Updated CMAKE_CUDA_FLAGS to \"${CMAKE_CUDA_FLAGS}\"")
endif()

if(BLT_EXE_LINKER_FLAGS)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${BLT_EXE_LINKER_FLAGS}")
    message(STATUS "Updated CMAKE_EXE_LINKER_FLAGS to \"${CMAKE_EXE_LINKER_FLAGS}\"")
endif()


###############################################################
# Support extra flags based on CMake configuration type
###############################################################
#
# We guard this approach to avoid issues with CMake generators
# that support multiple configurations, like Visual Studio.
#
###############################################################
if(NOT CMAKE_CONFIGURATION_TYPES)

    set(cfg_types DEBUG RELEASE RELWITHDEBINFO MINSIZEREL)

    foreach(cfg_type in ${cfg_types})
        # flags for the C compiler
        if(BLT_C_FLAGS_${cfg_type})
            set(CMAKE_C_FLAGS_${cfg_type}
                "${CMAKE_C_FLAGS_${cfg_type}} ${BLT_C_FLAGS_${cfg_type}}")
            message(STATUS "Updated CMAKE_C_FLAGS_${cfg_type} to \"${CMAKE_C_FLAGS_${cfg_type}}\"")
        endif()

        # flags for the C++ compiler
        if(BLT_CXX_FLAGS_${cfg_type})
            set(CMAKE_CXX_FLAGS_${cfg_type}
                "${CMAKE_CXX_FLAGS_${cfg_type}} ${BLT_CXX_FLAGS_${cfg_type}}")
            message(STATUS "Updated CMAKE_CXX_FLAGS_${cfg_type} to \"${CMAKE_CXX_FLAGS_${cfg_type}}\"")
        endif()

        # flags for the Fortran compiler
        if(ENABLE_FORTRAN AND BLT_FORTRAN_FLAGS_${cfg_type})
            set(CMAKE_Fortran_FLAGS_${cfg_type}
                "${CMAKE_Fortran_FLAGS_${cfg_type}} ${BLT_FORTRAN_FLAGS_${cfg_type}}")
            message(STATUS "Updated CMAKE_Fortran_FLAGS_${cfg_type} to \"${CMAKE_Fortran_FLAGS_${cfg_type}}\"")
        endif()

    endforeach()

endif()



################################
# RPath Settings
################################
# only apply rpath settings for builds using shared libs
if(BUILD_SHARED_LIBS)
    # use, i.e. don't skip the full RPATH for the build tree
    set(CMAKE_SKIP_BUILD_RPATH  FALSE)

    # when building, don't use the install RPATH already
    # (but later on when installing)
    set(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)
    set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
    set(CMAKE_INSTALL_NAME_DIR "${CMAKE_INSTALL_PREFIX}/lib")

    # add the automatically determined parts of the RPATH
    # which point to directories outside the build tree to the install RPATH
    set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

    # the RPATH to be used when installing, but only if it's not a system directory
    list(FIND CMAKE_PLATFORM_IMPLICIT_LINK_DIRECTORIES "${CMAKE_INSTALL_PREFIX}/lib" isSystemDir)
    if("${isSystemDir}" STREQUAL "-1")
        set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
    endif()
endif()


##################################################################
# Additional compiler warnings and treatment of warnings as errors
##################################################################

blt_append_custom_compiler_flag(
   FLAGS_VAR BLT_ENABLE_ALL_WARNINGS_C_FLAG
     DEFAULT    "-Wall -Wextra"
     CLANG      "-Wall -Wextra" 
                       # Additional  possibilities for clang include: 
                       #       "-Wdocumentation -Wdeprecated -Weverything"
     PGI        "-Minform=warn"
     MSVC       "/W4"
                       # Additional  possibilities for visual studio include:
                       # "/Wall /wd4619 /wd4668 /wd4820 /wd4571 /wd4710"
     XL         " "    # qinfo=<grp> produces additional messages on XL
                       # qflag=<x>:<x> defines min severity level to produce messages on XL
                       #     where x is i info, w warning, e error, s severe; default is: 
                       # (default is  qflag=i:i)
     )

blt_append_custom_compiler_flag(
    FLAGS_VAR BLT_ENABLE_ALL_WARNINGS_CXX_FLAG
     DEFAULT    "-Wall -Wextra"
     CLANG      "-Wall -Wextra" 
                       # Additional  possibilities for clang include: 
                       #       "-Wdocumentation -Wdeprecated -Weverything"
     PGI        "-Minform=warn"
     MSVC       "/W4"
                       # Additional  possibilities for visual studio include:
                       # "/Wall /wd4619 /wd4668 /wd4820 /wd4571 /wd4710"
     XL         " "    # qinfo=<grp> produces additional messages on XL
                       # qflag=<x>:<x> defines min severity level to produce messages on XL
                       #     where x is i info, w warning, e error, s severe; default is: 
                       # (default is  qflag=i:i)
     )

blt_append_custom_compiler_flag(
    FLAGS_VAR BLT_WARNINGS_AS_ERRORS_CXX_FLAG
     DEFAULT  "-Werror"
     MSVC     "/WX"
     XL       "-qhalt=w"
     )

blt_append_custom_compiler_flag(
    FLAGS_VAR BLT_WARNINGS_AS_ERRORS_C_FLAG
     DEFAULT  "-Werror"
     MSVC     "/WX"
     PGI      " "
     XL       "-qhalt=w"
     )

#
# Modify flags to avoid static linking runtime issues on MS Windows.
#
# When building on Windows, you can link in the runtime library
#   - statically (with /MT), or
#   - dynamically (with /MD).
# See https://docs.microsoft.com/en-us/cpp/build/reference/md-mt-ld-use-run-time-library?view=vs-2019.
#
# Mixing /MD with /MT can cause linking errors.  CMake specifies
# /MD when generating project files for MSVC and provides no way to
# change this.  This can be a problem with Google Test in particular,
# which when building statically replaces all /MD with /MT.  HDF5, on
# the other hand, sternly warns against the use of /MT, even when
# built statically.
#
# See https://gitlab.kitware.com/cmake/community/wikis/FAQ#dynamic-replace.
# Once we require CMake >= 3.15 we can address the issue differently, using
# CMAKE_MSVC_RUNTIME_LIBRARY:
# https://cmake.org/cmake/help/latest/variable/CMAKE_MSVC_RUNTIME_LIBRARY.html
#

if ( COMPILER_FAMILY_IS_MSVC AND NOT BUILD_SHARED_LIBS )
  if ( BLT_ENABLE_MSVC_STATIC_MD_TO_MT )
    foreach(_lang C CXX)
      foreach(_build
              FLAGS FLAGS_DEBUG FLAGS_RELEASE
              FLAGS_MINSIZEREL FLAGS_RELWITHDEBINFO)
          set(_flag CMAKE_${_lang}_${_build})
          if(${_flag} MATCHES "/MD")
              string(REGEX REPLACE "/MD" "/MT" ${_flag} "${${_flag}}")
          endif()
      endforeach()
    endforeach()
  elseif (ENABLE_GTEST)
    message(FATAL_ERROR
      "For static linking with MS Visual Studio using GTEST, you must set BLT_ENABLE_MSVC_STATIC_MD_TO_MT to ON in order to enable changing /MD to /MT.")
  endif()
endif()

set(langFlags "CMAKE_C_FLAGS" "CMAKE_CXX_FLAGS")

if (ENABLE_ALL_WARNINGS)
    message(STATUS  "Enabling all compiler warnings on all targets.")

    set (CMAKE_CXX_FLAGS "${BLT_ENABLE_ALL_WARNINGS_CXX_FLAG} ${CMAKE_CXX_FLAGS}")
    set (CMAKE_C_FLAGS "${BLT_ENABLE_ALL_WARNINGS_C_FLAG} ${CMAKE_C_FLAGS}")
endif()

if (ENABLE_WARNINGS_AS_ERRORS)
    message(STATUS  "Enabling treatment of warnings as errors on all targets.")

    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${BLT_WARNINGS_AS_ERRORS_CXX_FLAG}")
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${BLT_WARNINGS_AS_ERRORS_C_FLAG}")
endif()

################################
# Enable Fortran
################################
if(ENABLE_FORTRAN)
    # if enabled but no fortran compiler, halt the configure
    if(CMAKE_Fortran_COMPILER)
        message(STATUS  "Fortran support enabled.")
    else()
        message(FATAL_ERROR "Fortran support selected, but no Fortran compiler was found.")
    endif()

    list(APPEND langFlags "CMAKE_Fortran_FLAGS")
else()
    message(STATUS  "Fortran support disabled.")
endif()

###################################
# Output compiler and linker flags 
###################################
foreach(flagVar ${langFlags}  "CMAKE_EXE_LINKER_FLAGS" )
    message(STATUS "${flagVar} flags are:  ${${flagVar}}")
endforeach()

##################################
# Remove implicit link directories
##################################
if(BLT_CMAKE_IMPLICIT_LINK_DIRECTORIES_EXCLUDE)
    message(STATUS "Removing implicit link directories: ${BLT_CMAKE_IMPLICIT_LINK_DIRECTORIES_EXCLUDE}")
    list(REMOVE_ITEM CMAKE_CXX_IMPLICIT_LINK_DIRECTORIES ${BLT_CMAKE_IMPLICIT_LINK_DIRECTORIES_EXCLUDE})
    message(STATUS "Updated CXX implicit Link Directories: ${CMAKE_CXX_IMPLICIT_LINK_DIRECTORIES}")
    list(REMOVE_ITEM CMAKE_C_IMPLICIT_LINK_DIRECTORIES ${BLT_CMAKE_IMPLICIT_LINK_DIRECTORIES_EXCLUDE})
    message(STATUS "Updated C implicit Link Directories: ${CMAKE_C_IMPLICIT_LINK_DIRECTORIES}")
    if (ENABLE_FORTRAN)
        list(REMOVE_ITEM CMAKE_Fortran_IMPLICIT_LINK_DIRECTORIES ${BLT_CMAKE_IMPLICIT_LINK_DIRECTORIES_EXCLUDE})
        message(STATUS "Updated Fortran implicit Link Directories: ${CMAKE_Fortran_IMPLICIT_LINK_DIRECTORIES}")
    endif ()
    if (ENABLE_CUDA)
        list(REMOVE_ITEM CMAKE_CUDA_IMPLICIT_LINK_DIRECTORIES ${BLT_CMAKE_IMPLICIT_LINK_DIRECTORIES_EXCLUDE})
        message(STATUS "Updated CUDA implicit Link Directories: ${CMAKE_CUDA_IMPLICIT_LINK_DIRECTORIES}")
    endif ()
endif()
