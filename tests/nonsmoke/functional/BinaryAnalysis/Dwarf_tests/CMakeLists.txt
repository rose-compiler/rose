
include_directories(${KDE4_INCLUDES} ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} )


########### next target ###############

set(dwarfReader_SRCS dwarfReader.C)

kde4_add_executable(dwarfReader ${dwarfReader_SRCS})

target_link_libraries(dwarfReader ${KDE4_KDECORE_LIBS})


########### next target ###############

set(roseDwarfReader_SRCS roseDwarfReader.C)

kde4_add_executable(roseDwarfReader ${roseDwarfReader_SRCS})

target_link_libraries(roseDwarfReader ${KDE4_KDECORE_LIBS})


########### install files ###############
