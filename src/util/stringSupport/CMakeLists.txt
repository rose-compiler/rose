add_library(util_stringSupport OBJECT
  escape.C FileNameClassifier.C FileUtility.C)
########### install files ###############
install(FILES escape.h FileUtility.h
        DESTINATION ${INCLUDE_INSTALL_DIR})
