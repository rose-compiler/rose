add_library(roseBinaryLoader OBJECT
  BinaryLoader.C BinaryLoaderElf.C BinaryLoaderElfObj.C BinaryLoaderPe.C)


########### install files ###############

install(FILES  BinaryLoader.h BinaryLoaderElf.h
               BinaryLoaderElfObj.h BinaryLoaderPe.h
  DESTINATION include)
