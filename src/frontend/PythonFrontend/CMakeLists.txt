if(enable-python)
  include_directories(${KDE4_INCLUDES} ${KDE4_INCLUDE_DIR} ${QT_INCLUDES}
		      ${Boost_INCLUDE_DIRS} ${PYTHON_INCLUDE_DIRS})

  add_library(rosePythonFrontend
	  SagePythonInterface.C SagePythonSupport.C PythonFrontend.C)

  # fix: Cxx_Grammar.h: No such file or directory
  add_dependencies(rosePythonFrontend rosetta_generated)

  install(FILES  PythonFrontend.h DESTINATION src/frontend/PythonFrontend/CMakeLists.txt )
endif()
