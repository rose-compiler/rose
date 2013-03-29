# Use --with-libxml2 on configure command-line for ROSE to activate this
#if ROSE_USE_SSL_SUPPORT

include_directories(
  ${ROSE_SOURCE_DIR}/src/roseExtensions/sqlite3x
  ${OPENSSL_INCLUDE_DIR}
)

if(with-roseQt)
  add_subdirectory(gui)
endif()

add_executable(printOutClones printOutClones.C)
target_link_libraries(printOutClones
  ROSE_DLL RoseSQLite3xDatabase ${Boost_LIBRARIES})

add_executable(createVectorsBinary
  createVectorsBinary.C createSignatureVectors.C vectorCompression.C
)
target_link_libraries(createVectorsBinary
  ROSE_DLL RoseSQLite3xDatabase ${Boost_LIBRARIES} ${OPENSSL_LIBRARIES}
)
