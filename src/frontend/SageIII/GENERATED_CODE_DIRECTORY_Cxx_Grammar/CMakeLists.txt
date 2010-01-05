
include_directories(${KDE4_INCLUDES} ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} )

# Use this as a way to turn off this optional use to build smaller files
# so that we test this unter Linux before worring about use within Windows.
# tps (01/04/10) : Added name for if condition
if ( smallerGeneratedFiles )
message ( STATUS ">>>>>>>>>>>>>>>>>>>>>> smallerGeneratedFiles : " ${smallerGeneratedFiles} )

########### next target ###############
set( libgeneratedIR_SRC
     SgAccessModifier.C
     SgAsmDwarfRestrictType.C
     SgTest.C
   )

add_library( libgeneratedIR STATIC ${libgeneratedIR_SRC} )
add_dependencies( libgeneratedIR ROSETTA )

########### install files ###############

# Need to define a mechanism to collect a list of all files (later).
install(FILES  SgAccessModifier.h SgAsmDwarfRestrictType.h DESTINATION ${INCLUDE_INSTALL_DIR})

endif ( smallerGeneratedFiles )
