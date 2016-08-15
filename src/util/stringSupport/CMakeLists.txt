add_library(util_stringSupport OBJECT
  escape.C FileNameClassifier.C FileUtility.C StringUtility.C)

########### install files ###############
install(FILES  string_functions.h escape.h FileUtility.h StringUtility.h
        DESTINATION ${INCLUDE_INSTALL_DIR})
