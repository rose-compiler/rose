
include_directories(${CMAKE_SOURCE_DIR}/src/midend/binaryAnalyses/libraryIdentification ${KDE4_INCLUDES} ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} )


########### next target ###############

set(libraryIdentificationTest_SRCS libraryIdentificationTest.C)

kde4_add_executable(libraryIdentificationTest ${libraryIdentificationTest_SRCS})

target_link_libraries(libraryIdentificationTest ${KDE4_KDECORE_LIBS})


########### install files ###############
