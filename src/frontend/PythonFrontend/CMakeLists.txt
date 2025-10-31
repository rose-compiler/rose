if(ENABLE_PYTHON)
  include_directories(${Boost_INCLUDE_DIRS} ${PYTHON_INCLUDE_DIRS})

  add_library(rosePythonFrontend
	  SagePythonInterface.C SagePythonSupport.C PythonFrontend.C)

  install(FILES  PythonFrontend.h DESTINATION src/frontend/PythonFrontend/CMakeLists.txt )
endif()
