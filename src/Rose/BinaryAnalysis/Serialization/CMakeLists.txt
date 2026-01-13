# Binary analysis serialization support.
#
# SerialFrame is required for all serialization formats.
# FlatBuffers components are only built when FlatBuffers support is enabled at configure time.
# Boost serialization components are only built when Boost serialization is enabled.

# Start with core files that are always needed
set(SERIALIZATION_SOURCES
  SerialFrame.C
)

# Add Boost-specific backend if enabled
if(ROSE_ENABLE_BOOST_SERIALIZATION)
  list(APPEND SERIALIZATION_SOURCES
    Boost.C
  )
endif()

# Add FlatBuffers-specific backend if enabled
if(ROSE_ENABLE_FLATBUFFERS)
  list(APPEND SERIALIZATION_SOURCES
    FlatBuffers.C
  )
endif()

# Create the library with all available components
add_library(roseBinaryAnalysisSerialization OBJECT
  ${SERIALIZATION_SOURCES}
)

# Add dependencies as needed
if(ROSE_ENABLE_FLATBUFFERS)
  add_dependencies(roseBinaryAnalysisSerialization rosetta_generated)

  install(FILES
    FlatBuffers.h
    FlatBufferSchema.h
    DESTINATION ${INCLUDE_INSTALL_DIR}/Rose/BinaryAnalysis/Serialization
  )
endif()

# Add SerialFrame.h to installation
install(FILES
  SerialFrame.h
  DESTINATION ${INCLUDE_INSTALL_DIR}/Rose/BinaryAnalysis/Serialization
)
