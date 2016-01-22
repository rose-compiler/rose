add_library(util_stringSupport OBJECT
  escape.C FileNameClassifier.C StringUtility.C)

########### install files ###############
install(FILES  string_functions.h escape.h StringUtility.h
        DESTINATION ${INCLUDE_INSTALL_DIR})
