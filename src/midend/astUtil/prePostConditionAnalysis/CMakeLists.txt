# Specify the directories that contain header files for this subdirectory
include_directories(${CMAKE_SOURCE_DIR}/src/midend/astUtil/astSupport
                    ${shell ${CMAKE_SOURCE_DIR}/bin/rose-config cppflags}
                    ${BOOST_INCLUDEDIR}
                    ./src
                    )

# Install headers
install(FILES src/PrePostConditionAnalysis.h src/PrePostCondition.h
        DESTINATION ${INCLUDE_INSTALL_DIR})
