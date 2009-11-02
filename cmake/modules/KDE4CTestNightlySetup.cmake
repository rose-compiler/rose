# The following variables have to be set before including this file:
# CTEST_CMAKE_GENERATOR
# CTEST_UPDATE_TYPE (if update from cvs or svn is required)
# SVN_REPOSITORY or CVS_REPOSITORY and CVS_MODULE
#
# If the project doesn't build with spaces in the path, do the following:
# set(CTEST_AVOID_SPACES TRUE)
# 
# After this file has been included, the regular new style ctest
# scripting commands can be used, e.g.
#
# ctest_empty_binary_directory("${CTEST_BINARY_DIRECTORY}")
# ctest_start(Nightly)
# ctest_update(SOURCE "${CTEST_SOURCE_DIRECTORY}" )
# ctest_configure(BUILD "${CTEST_BINARY_DIRECTORY}" )
# ctest_build(BUILD "${CTEST_BINARY_DIRECTORY}" )
# ctest_test(BUILD "${CTEST_BINARY_DIRECTORY}" )
# ctest_submit()




set(currentDirectory "${CTEST_SCRIPT_DIRECTORY}")

if(NOT EXISTS "${currentDirectory}/CMakeLists.txt")
   message(FATAL_ERROR "This macro must be called from a cmake script in the source tree of your project.")
endif(NOT EXISTS "${currentDirectory}/CMakeLists.txt")

include("${currentDirectory}/CTestConfig.cmake")
include("${currentDirectory}/CTestCustom.cmake" OPTIONAL)

# Set up the directories where the dashboards will be created
# By default this will be "$HOME/Dashboards/<project>/(src|build) dir".
# It can be overriden by the user.
set(DASHBOARD_DIR "$ENV{HOME}/Dashboards" )

if(NOT DEFINED CTEST_SOURCE_DIRECTORY)
   if(CTEST_AVOID_SPACES)
      set(CTEST_SOURCE_DIRECTORY "${DASHBOARD_DIR}/${CTEST_PROJECT_NAME}/srcdir" )
   else(CTEST_AVOID_SPACES)
      set(CTEST_SOURCE_DIRECTORY "${DASHBOARD_DIR}/${CTEST_PROJECT_NAME}/src dir" )
   endif(CTEST_AVOID_SPACES)
endif(NOT DEFINED CTEST_SOURCE_DIRECTORY)

if(NOT DEFINED CTEST_BINARY_DIRECTORY)
   if(CTEST_AVOID_SPACES)
      set(CTEST_BINARY_DIRECTORY "${DASHBOARD_DIR}/${CTEST_PROJECT_NAME}/builddir" )
   else(CTEST_AVOID_SPACES)
      set(CTEST_BINARY_DIRECTORY "${DASHBOARD_DIR}/${CTEST_PROJECT_NAME}/build dir" )
   endif(CTEST_AVOID_SPACES)
endif(NOT DEFINED CTEST_BINARY_DIRECTORY)



site_name(CTEST_SITE)
if(NOT DEFINED CTEST_BUILD_NAME)
   set(CTEST_BUILD_NAME ${CMAKE_SYSTEM_NAME})
endif(NOT DEFINED CTEST_BUILD_NAME)

if("${CTEST_CMAKE_GENERATOR}" MATCHES Makefile)
   find_program(MAKE_EXECUTABLE make gmake)
   set(CTEST_BUILD_COMMAND    "${MAKE_EXECUTABLE}" )
else("${CTEST_CMAKE_GENERATOR}" MATCHES Makefile)
   if(NOT DEFINED CTEST_BUILD_COMMAND)
      message(FATAL_ERROR "CTEST_CMAKE_GENERATOR is set to \"${CTEST_CMAKE_GENERATOR}\", but CTEST_BUILD_COMMAND has not been set")
   endif(NOT DEFINED CTEST_BUILD_COMMAND)
endif("${CTEST_CMAKE_GENERATOR}" MATCHES Makefile)

# set up version control

string(TOLOWER ${CTEST_UPDATE_TYPE} _ctest_vcs)
set(_have_vcs FALSE)

if ("${_ctest_vcs}" STREQUAL svn)
   find_program(SVN_EXECUTABLE svn)
   if (NOT SVN_EXECUTABLE)
      message(FATAL_ERROR "Error: CTEST_UPDATE_TYPE is svn, but could not find svn executable")
   endif (NOT SVN_EXECUTABLE)
   if(NOT SVN_REPOSITORY)
      message(FATAL_ERROR "Error: CTEST_UPDATE_TYPE is svn, but SVN_REPOSITORY is not set")
   endif(NOT SVN_REPOSITORY)
   set(CTEST_UPDATE_COMMAND ${SVN_EXECUTABLE})
   set(CTEST_CHECKOUT_COMMAND "${SVN_EXECUTABLE} co ${SVN_REPOSITORY} \"${CTEST_SOURCE_DIRECTORY}\"")
   set(_have_vcs TRUE)
endif ("${_ctest_vcs}" STREQUAL svn)

if ("${_ctest_vcs}" STREQUAL cvs)
   find_program(CVS_EXECUTABLE cvs cvsnt)
   if (NOT CVS_EXECUTABLE)
      message(FATAL_ERROR "Error: CTEST_UPDATE_TYPE is cvs, but could not find cvs or cvsnt executable")
   endif (NOT CVS_EXECUTABLE)
   if (NOT CVS_REPOSITORY)
      message(FATAL_ERROR "Error: CTEST_UPDATE_TYPE is cvs, but CVS_REPOSITORY is not set")
   endif (NOT CVS_REPOSITORY)
   if (NOT CVS_MODULE)
      message(FATAL_ERROR "Error: CTEST_UPDATE_TYPE is cvs, but CVS_MODULE is not set")
   endif (NOT CVS_MODULE)

   set(CTEST_UPDATE_COMMAND ${CVS_EXECUTABLE})
   set(CTEST_CHECKOUT_COMMAND "${CVS_EXECUTABLE} -d ${CVS_REPOSITORY} co  -d \"${CTEST_SOURCE_DIRECTORY}\" ${CVS_MODULE}")
   set(_have_vcs TRUE)
endif ("${_ctest_vcs}" STREQUAL cvs)
