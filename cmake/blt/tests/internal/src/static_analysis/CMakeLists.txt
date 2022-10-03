# Copyright (c) 2017-2022, Lawrence Livermore National Security, LLC and
# other BLT Project Developers. See the top-level LICENSE file for details
# 
# SPDX-License-Identifier: (BSD-3-Clause)

#####################################
# Test clang-query static analysis
# Clang Query needs to be enabled, with appropriate Flags set up for building and running
# your code
#####################################

set(BLT_CLANG_QUERY_CHECKER_DIRECTORIES ${CMAKE_CURRENT_SOURCE_DIR}/static_analysis_passes)
blt_add_executable( NAME well_analyzed_executable
                    SOURCES well_analyzed_source.cpp
)
blt_add_code_checks(PREFIX all
  SOURCES well_analyzed_source.cpp)
blt_add_clang_query_target(NAME filtered CHECKERS "if-stmt"
  SRC_FILES well_analyzed_source.cpp)

blt_add_executable( NAME subtle_error
                    SOURCES subtle_error_source.cpp
)
blt_add_clang_tidy_target(NAME check_for_subtle_error 
                          CHECKS "clang-analyzer-*"
                          SRC_FILES subtle_error_source.cpp)
