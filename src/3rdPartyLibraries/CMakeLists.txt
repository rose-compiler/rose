include_directories(${KDE4_INCLUDES} ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} )

add_subdirectory(MSTL)
add_subdirectory(antlr-jars)
add_subdirectory(qrose)
add_subdirectory(fortran-parser)
add_subdirectory(java-parser)
add_subdirectory(POET)

# Build and install nlohmann json for Unti Test Generator
add_subdirectory(json) 
