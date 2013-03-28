
include_directories(${KDE4_INCLUDES} ${KDE4_INCLUDE_DIR} ${QT_INCLUDES}
                    ${Boost_INCLUDE_DIRS} ${PYTHON_INCLUDE_DIRS})


########### next target ###############

add_library(rosePythonFrontend
	SagePythonInterface.C SagePythonSupport.C PythonFrontend.C)
#add_dependencies(Python_FRONTEND_LIB ROSETTA  ) 

########### install files ###############

install(FILES  PythonFrontend.h DESTINATION src/frontend/PythonFrontend/CMakeLists.txt )
