add_library(util_stringSupport OBJECT
  escape.C FileNameClassifier.C string_functions.cpp)

########### install files ###############
install(FILES  string_functions.h escape.h StringUtility.h
        DESTINATION ${INCLUDE_INSTALL_DIR})
