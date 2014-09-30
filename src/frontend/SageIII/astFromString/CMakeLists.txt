if(NOT enable-internalFrontendDevelopment)
  add_library(astFromString OBJECT AstFromString.cpp ParserBuilder.cpp)
endif()

install(FILES AstFromString.h ParserBuilder.hpp ParserBuilder.tpp
  DESTINATION include)
