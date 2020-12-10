add_library(roseBinaryLoader OBJECT BinaryLoader.C BinaryLoaderElf.C BinaryLoaderElfObj.C BinaryLoaderPe.C)
add_dependencies(roseBinaryLoader rosetta_generated)

install(
  FILES  BinaryLoader.h BinaryLoaderElf.h BinaryLoaderElfObj.h BinaryLoaderPe.h
  DESTINATION ${INCLUDE_INSTALL_DIR})
