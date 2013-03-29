add_executable(babelPreprocessor
  sidlFunction.C caselessCompare.C findMethodCalls.C
  options.C babelTransformation.C babelPreprocessor.C
)

target_link_libraries(babelPreprocessor ROSE_DLL ${Boost_LIBRARIES})

#add_executable(babelTest_2 babelTest_2.C)
#add_test(babelTest_2 babelTest_2)

install(
  FILES babelTransformation.h caselessCompare.h findMethodCalls.h simpleA++.h
        transformationOptions.h
  DESTINATION include
)
