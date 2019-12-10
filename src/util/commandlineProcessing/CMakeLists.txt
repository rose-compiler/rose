add_library(util_commandlineProcessing OBJECT
  commandline_processing.C
  sla++.C)

# commandline_processing.C includes rosePublicConfig.h
add_dependencies(util_commandlineProcessing generate_rosePublicConfig)

########### install files ###############
install(FILES
  sla.h commandline_processing.h DESTINATION ${INCLUDE_INSTALL_DIR})
