add_library(roseFileSystem OBJECT
  FileSystem.C)

add_dependencies(roseFileSystem rosetta_generated)
