# Use --with-libxml2 on configure command-line for ROSE to activate this
#if ROSE_USE_SSL_SUPPORT

include_directories(
  ${ROSE_SOURCE_DIR}/src/roseExtensions/sqlite3x
  ${OPENSSL_INCLUDE_DIR})

if(with-roseQt)
  add_subdirectory(gui)
endif()

add_executable(printOutClones printOutClones.C)
target_link_libraries(printOutClones ROSE_DLL ${Boost_LIBRARIES})

if(NOT "${Boost_VERSION}" STREQUAL "104000")
  add_executable(createVectorsBinary
    createVectorsBinary.C createSignatureVectors.C vectorCompression.C)
  target_link_libraries(createVectorsBinary ROSE_DLL ${Boost_LIBRARIES} ${OPENSSL_LIBRARIES})
endif()
