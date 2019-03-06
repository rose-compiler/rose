include_directories(${CMAKE_SOURCE_DIR}/src/frontend/X10_ROSE_Connection)

if(enable-x10)
  add_library(libX10Traversal x10_main.cpp x10.C)
else()
  add_library(libX10Traversal dummy_x10_main.cpp)
endif()

target_link_libraries(libX10Traversal ${KDE4_KDECORE_LIBS})
set_target_properties(libX10Traversal PROPERTIES VERSION 1.0.0 SOVERSION 1)
set_target_properties(libX10Traversal PROPERTIES OUTPUT_NAME "X10Traversal")
install(TARGETS libX10Traversal DESTINATION lib)

########### install files ###############

install(FILES  jni_x10SourceCodePosition.h jni_x10_token.h jni_x10_utils.h
               x10.h x10SourceCodePosition.h x10_support.h x10_token.h
        DESTINATION ${INCLUDE_INSTALL_DIR})
