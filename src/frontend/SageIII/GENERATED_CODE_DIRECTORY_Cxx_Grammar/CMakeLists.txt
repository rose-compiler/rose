
include_directories(${KDE4_INCLUDES} ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} )

# Use this as a way to turn off this optional use to build smaller files
# so that we test this unter Linux before worring about use within Windows.
if (0)

########### next target ###############
set( libgeneratedIR_SRC  
     SgAccessModifier.C
     SgAsmDwarfRestrictType.C
   )

add_library( libgeneratedIR STATIC ${libgeneratedIR_SRC} )
add_dependencies( libgeneratedIR ROSETTA )

########### install files ###############

# Need to define a mechanism to collect a list of all files (later).
install(FILES  SgAccessModifier.h SgAsmDwarfRestrictType.h DESTINATION ${INCLUDE_INSTALL_DIR})
endif (0)

