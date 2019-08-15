# setup ROSE_INCLUDE_FLAGS variable (used by some compile tests)
foreach(include ${ROSE_INCLUDES})
  list(APPEND ROSE_INCLUDE_FLAGS "-I${include}")
endforeach()
foreach(include ${Boost_INCLUDE_DIRS})
  list(APPEND ROSE_INCLUDE_FLAGS "-I${include}")
endforeach()

# default values for our compile tests
set(ROSE_FLAGS --edg:no_warnings -w -rose:verbose 0)
set(translator testTranslator)

# define compile_test CMake function
function(compile_test input_file)
  get_filename_component(test_name ${input_file} NAME_WE)

  # optional argument to omit include directories
  if(ARGV1)
    add_test(
      NAME ${test_name}
      COMMAND ${translator}
        ${ROSE_FLAGS} -c ${CMAKE_CURRENT_SOURCE_DIR}/${input_file}
    )
  else()
    add_test(
      NAME ${test_name}
      COMMAND ${translator}
        ${ROSE_FLAGS} ${ROSE_INCLUDE_FLAGS} -I${CMAKE_CURRENT_SOURCE_DIR} -c
        ${CMAKE_CURRENT_SOURCE_DIR}/${input_file}
    )
  endif()
endfunction()

# determine what subdirectories to enter based on our configuration settings.

if(enable-x10)
  add_subdirectory(x10_tests)
endif()

if(enable-cuda)
  add_subdirectory(CudaTests)
endif()

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
   add_subdirectory(C_tests)
   add_subdirectory(C89_std_c89_tests)
   add_subdirectory(C_subset_of_Cxx_tests)
   add_subdirectory(OpenClTests)
else()
  if(enable-c)
    if(NOT "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Xtensa")
      add_subdirectory(C_tests)
      add_subdirectory(MicrosoftWindows_C_tests)
      add_subdirectory(Cxx_tests)
      add_subdirectory(MicrosoftWindows_Cxx_tests)
      add_subdirectory(C_subset_of_Cxx_tests)
      add_subdirectory(C99_tests)
      add_subdirectory(UPC_tests)
    endif()
  endif()

  if(NOT "${EDG_VERSION}" EQUAL "4.3")
    if(NOT enable-java)
      add_subdirectory(OpenMP_tests)
    endif()

    if(enable-fortran)
      add_subdirectory(Fortran_tests)
      # DQ (7/14/2013): Temporarily commented out this test to evaluate
      # progress on tests/nonsmoke/functional/CompileTests directory.
      #add_subdirectory(CAF2_tests)
    endif()

    if(enable-java)
      add_subdirectory(Java_tests)
      add_subdirectory(MicrosoftWindows_Java_tests)
    endif()

    if(enable-c)
      add_subdirectory(copyAST_tests)
# ROSE-1738
#     add_subdirectory(RoseExample_tests)
      add_subdirectory(PythonExample_tests)
      add_subdirectory(colorAST_tests)
      add_subdirectory(unparseToString_tests)
      add_subdirectory(sourcePosition_tests)
      add_subdirectory(boost_tests)
      # requires Valgrind
      #add_subdirectory(uninitializedField_tests)
      add_subdirectory(OvertureCode)
      add_subdirectory(P++Tests)
      add_subdirectory(A++Code)
      add_subdirectory(ExpressionTemplateExample_tests)
      add_subdirectory(hiddenTypeAndDeclarationListTests)
      add_subdirectory(sizeofOperation_tests)
      if (with-wine)
        add_subdirectory(MicrosoftWindows_tests)
      endif()
      add_subdirectory(nameQualificationAndTypeElaboration_tests)
      if(enable-new-edg-interface)
        add_subdirectory(NewEDGInterface_C_tests)
      endif()
      add_subdirectory(UnparseHeadersTests)
      add_subdirectory(UnparseHeadersUsingTokenStream_tests)

      if(NOT "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Xtensa")
        add_subdirectory(mergeAST_tests)
        add_subdirectory(ElsaTestCases)
        add_subdirectory(virtualCFG_tests)
        add_subdirectory(A++Tests)
        add_subdirectory(staticCFG_tests)
      endif()
    endif()
  endif() # end of !ROSE_USE_EDG_VERSION_4_3

  if(enable-python)
     #add_subdirectory(Python_tests)
  endif()
endif() # end of ROSE_USE_CLANG_FRONTEND

