
include_directories(${KDE4_INCLUDES} ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} )


########### next target ###############

#add_library(libabstractLayer ${libabstractLayer_SRC})

########### install files ###############

install(FILES AstAnnotator.h CPAstAttributeInterface.h DFAstAttribute.h Labeler.h RDAstAttributeInterface.h SgNodeHelper.h VariableIdMapping.h DESTINATION ${INCLUDE_INSTALL_DIR})
