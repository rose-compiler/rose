if(NOT enable-internalFrontendDevelopment)
  add_library(astFromString OBJECT AstFromString.cpp ParserBuilder.cpp)
  add_dependencies(astFromString rosetta_generated)
endif()

install(FILES AstFromString.h ParserBuilder.hpp ParserBuilder.tpp
  DESTINATION ${INCLUDE_INSTALL_DIR})
