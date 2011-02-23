
include_directories(${KDE4_INCLUDES} ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} )

########### next target ###############


#add_library( rose_support ${rose_support_src}  )
#add_dependencies( rose_support ROSETTA  )
########### install files ###############

install(FILES  transformationSupport.h     optionDeclaration.h     sourceLocationInheritedAttribute.h     roseTranslators.h     utility_functions.h DESTINATION ${INCLUDE_INSTALL_DIR})
