add_library(util_stringSupport OBJECT
  escape.C FileNameClassifier.C FileUtility.C)
add_dependencies(util_stringSupport generate_rosePublicConfig)
########### install files ###############
install(FILES escape.h FileUtility.h
        DESTINATION ${INCLUDE_INSTALL_DIR})
