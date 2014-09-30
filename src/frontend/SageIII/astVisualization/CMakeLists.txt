add_library( astVisualization OBJECT
  astGraph.C
  AstAttributeDOT.C
  wholeAST.C
  copyGraph.C
  walrus.C)


########### install files ###############

install(FILES  AstAttributeDOT.h astGraph.h astGraphTemplateImpl.h wholeAST.h
               wholeAST_API.h copyGraph.h walrus.h
               intermediateRepresentationNodeGraphs.h
        DESTINATION ${INCLUDE_INSTALL_DIR})
