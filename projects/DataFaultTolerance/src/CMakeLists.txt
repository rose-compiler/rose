include_directories(${ROSE_SOURCE_DIR}/projects/DataFaultTolerance/includes)

#add_subdirectory(element)

add_executable(DataFaultToleranceTransformation
  faultToleranceArrayPass.C AstFromString.C pragmaHandling.C arrayBase.C)
target_link_libraries(DataFaultToleranceTransformation
  ROSE_DLL ${Boost_LIBRARIES}
)

add_library(faulttolerancearray
  faultToleranceArrayLib.C faultToleranceArrayLibUtility.C
)
target_link_libraries(faulttolerancearray
  ROSE_DLL ${Boost_LIBRARIES}
)
