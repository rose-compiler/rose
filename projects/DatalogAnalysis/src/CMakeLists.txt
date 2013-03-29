
include_directories(${ROSE_BINARY_DIR}/projects/DatalogAnalysis/src)

add_subdirectory(DBFactories)

add_executable(relationTranslator
  CFGVisitor.cc main.cc misc.cc ChildVisitor.cc
)

target_link_libraries(relationTranslator
  ROSE_DLL ${Boost_LIBRARIES} DBFactories astBinaryRelations
)
