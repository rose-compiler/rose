add_library(astMatching OBJECT
  AstMatching.C
  matcherparser.C
  MatchOperation.C
  RoseAst.C
  )

add_dependencies(astMatching rosetta_generated)

install(FILES
  AstMatching.h
  matcherparser_decls.h
  matcherparser.h
  MatchOperation.h
  RoseAst.h
  DESTINATION ${INCLUDE_INSTALL_DIR})
