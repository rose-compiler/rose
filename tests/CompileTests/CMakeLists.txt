foreach(include ${ROSE_INCLUDES})
  list(APPEND ROSE_INCLUDE_FLAGS "-I${include}")
endforeach()

# default values for our compile tests
set(ROSE_FLAGS --edg:no_warnings -w -rose:verbose 0)
set(translator testTranslator)

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

add_subdirectory(A++Code)
add_subdirectory(A++Tests)
add_subdirectory(boost_tests)
add_subdirectory(C99_tests)

if(enable-fortran)
  add_subdirectory(CAF2_tests)
endif()

add_subdirectory(C++Code)
#add_subdirectory(colorAST_tests)
#add_subdirectory(copyAST_tests)
#add_subdirectory(C_subset_of_Cxx_tests)
#add_subdirectory(C_tests)
#add_subdirectory(CudaTests)
#add_subdirectory(Cxx_tests)
#add_subdirectory(EDG_4_x)
#add_subdirectory(ElsaTestCases)
#add_subdirectory(ExpressionTemplateExample_tests)
#add_subdirectory(Fortran_tests)
#add_subdirectory(hiddenTypeAndDeclarationListTests)
#add_subdirectory(Java_tests)
#add_subdirectory(mergeAST_tests)
#add_subdirectory(MicrosoftWindows_tests)
#add_subdirectory(nameQualificationAndTypeElaboration_tests)
#add_subdirectory(NewEDGInterface_C_tests)
#add_subdirectory(OpenClTests)
#add_subdirectory(OpenMP_tests)
#add_subdirectory(OvertureCode)
#add_subdirectory(P++Tests)
#add_subdirectory(PythonExample_tests)
#add_subdirectory(Python_tests)
#add_subdirectory(RoseExample_tests)
#add_subdirectory(sizeofOperation_tests)
#add_subdirectory(sourcePosition_tests)
#add_subdirectory(staticCFG_tests)
#add_subdirectory(uninitializedField_tests)
#add_subdirectory(UnparseHeadersTests)
#add_subdirectory(unparseToString_tests)
#add_subdirectory(UPC_tests)
#add_subdirectory(virtualCFG_tests)
