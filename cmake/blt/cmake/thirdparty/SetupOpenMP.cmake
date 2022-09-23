# Copyright (c) 2017-2022, Lawrence Livermore National Security, LLC and
# other BLT Project Developers. See the top-level LICENSE file for details
# 
# SPDX-License-Identifier: (BSD-3-Clause)

#################################################
# OpenMP
# (OpenMP support is provided by the compiler)
#################################################

find_package(OpenMP REQUIRED)

# check if the openmp flags used for C/C++ are different from the openmp flags
# used by the Fortran compiler
set(BLT_OPENMP_FLAGS_DIFFER FALSE CACHE BOOL "")
if (ENABLE_FORTRAN)
  string(COMPARE NOTEQUAL "${OpenMP_CXX_FLAGS}" "${OpenMP_Fortran_FLAGS}"
         BLT_OPENMP_FLAGS_DIFFER )
endif()

# avoid generator expressions if possible, as generator expressions can be
# passed as flags to downstream projects that might not be using the same
# languages. See https://github.com/LLNL/blt/issues/205
set(_compile_flags ${OpenMP_CXX_FLAGS})
set(_link_flags  ${OpenMP_CXX_FLAGS})

if(NOT COMPILER_FAMILY_IS_MSVC AND ENABLE_CUDA AND BLT_OPENMP_FLAGS_DIFFER)
    set(_compile_flags
        $<$<AND:$<NOT:$<COMPILE_LANGUAGE:CUDA>>,$<NOT:$<COMPILE_LANGUAGE:Fortran>>>:${OpenMP_CXX_FLAGS}> 
        $<$<COMPILE_LANGUAGE:CUDA>:-Xcompiler=${OpenMP_CXX_FLAGS}>
        $<$<COMPILE_LANGUAGE:Fortran>:${OpenMP_Fortran_FLAGS}>)
elseif(NOT COMPILER_FAMILY_IS_MSVC AND ENABLE_CUDA)
    set(_compile_flags
        $<$<NOT:$<COMPILE_LANGUAGE:CUDA>>:${OpenMP_CXX_FLAGS}> 
        $<$<COMPILE_LANGUAGE:CUDA>:-Xcompiler=${OpenMP_CXX_FLAGS}>)
elseif(NOT COMPILER_FAMILY_IS_MSVC AND BLT_OPENMP_FLAGS_DIFFER)
    set(_compile_flags
        $<$<NOT:$<COMPILE_LANGUAGE:Fortran>>:${OpenMP_CXX_FLAGS}>
        $<$<COMPILE_LANGUAGE:Fortran>:${OpenMP_Fortran_FLAGS}>)
endif()


# Allow user to override
if (BLT_OPENMP_COMPILE_FLAGS)
    set(_compile_flags ${BLT_OPENMP_COMPILE_FLAGS})
endif()
if (BLT_OPENMP_LINK_FLAGS)
    set(_link_flags ${BLT_OPENMP_LINK_FLAGS})
endif()


message(STATUS "OpenMP Compile Flags: ${_compile_flags}")
message(STATUS "OpenMP Link Flags:    ${_link_flags}")

blt_import_library(NAME openmp
                   COMPILE_FLAGS ${_compile_flags}
                   LINK_FLAGS    ${_link_flags}
                   EXPORTABLE    ${BLT_EXPORT_THIRDPARTY})
