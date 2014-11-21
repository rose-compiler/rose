add_library(astMatching OBJECT
  AstMatching.C MatchOperation.C RoseAst.C matcherparser.C)
add_dependencies(astMatching rosetta_generated)

# DQ (6/8/2014): Added header files and source files below.
install(FILES
  RoseAst.h matcherparser_decls.h AstMatching.h MatchOperation.h
  DESTINATION ${INCLUDE_INSTALL_DIR})
