# Please be careful about where to put your tests and variable settings
# The order matters!!


#----- put platform independent settings in this section-----------------
cmake_minimum_required(VERSION 2.6)
project (ROSE CXX C)

# Why do we have to have a copy of some standard built-in modules inside rose?
set( CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake" "${CMAKE_SOURCE_DIR}/cmake/modules"  ${CMAKE_MODULE_PATH})
#message ("CMAKE_MODULE_PATH = ${CMAKE_MODULE_PATH}")


set(Boost_USE_STATIC_LIBS TRUE)        
set(Boost_DEBUG TRUE)

if (WIN32)
FIND_PACKAGE(Boost REQUIRED) 
#set (Boost_INCLUDE_DIRS ${Boost_INCLUDE_DIRS}/../../ )

#set (Boost_LIBRARY_DIRS ${Boost_INCLUDE_DIRS}/lib/ )
#set (Boost_LIBRARIES ${Boost_INCLUDE_DIRS}/lib/ )
set (BOOST_LIBRARYDIR ${Boost_LIBRARY_DIRS} )
set (BOOST_INCLUDEDIR ${Boost_INCLUDE_DIRS}/ )
#set (BOOST_ROOT ${Boost_INCLUDE_DIRS}/ )

MESSAGE("Boost information:") 
MESSAGE("  BOOST_ROOT: ${BOOST_ROOT}") 
MESSAGE("  Boost_INCLUDE_DIRS: ${Boost_INCLUDE_DIRS}") 
MESSAGE("  Boost_LIBRARIES: ${Boost_LIBRARIES}") 
MESSAGE("  Boost_LIBRARY_DIRS: ${Boost_LIBRARY_DIRS}") 
MESSAGE("  BOOST_LIBRARYDIR : ${BOOST_LIBRARYDIR}") 
MESSAGE("  BOOST_INCLUDEDIR : ${BOOST_INCLUDEDIR}") 


INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS})
LINK_DIRECTORIES(${Boost_LIBRARY_DIRS})
endif (WIN32)

#Ensure that Boost is found
# environment variable BOOST_ROOT can help to find a user-specified path to Boost
SET(Boost_ADDITIONAL_VERSIONS "1.36.0" "1.37" "1.37.0" "1.38.0" "1.39.0" "1.40.0" )
find_package( Boost 1.36.0 COMPONENTS date_time filesystem program_options regex system wave thread REQUIRED)



if(NOT Boost_FOUND)
    message(FATAL_ERROR "Could not find Boost version 1.35.0 or newer command")
endif(NOT Boost_FOUND)
include_directories(${Boost_INCLUDE_DIRS})



# default paths to install header, executable, and libraries
set(INCLUDE_INSTALL_DIR "/usr/include")
set(BIN_INSTALL_DIR      "bin"   )   # The install dir for executables
set(LIB_INSTALL_DIR      "lib" )

set(INSTALL_TARGETS_DEFAULT_ARGS  RUNTIME DESTINATION "${BIN_INSTALL_DIR}"
                                      LIBRARY DESTINATION "${LIB_INSTALL_DIR}"
                                                                        ARCHIVE
                                                                        DESTINATION
                                                                        "${LIB_INSTALL_DIR}"
                                                                        COMPONENT
                                                                        Devel
                                                                        )
# a new definition to tweak code for cmake 
add_definitions (-DUSE_CMAKE)


if (NOT WIN32)
enable_language(Fortran)
endif (NOT WIN32)

# tps (1/4/2010) Do not know how to add flag to CMakeSetup yet
option ( enable-smaller-generated-files "Build with smaller generated files for IR nodes" )
#set( enable-smaller-generated-files 1 )
if ( enable-smaller-generated-files )
add_definitions ("-DsmallerGeneratedFiles" "-DROSE_USE_SMALLER_GENERATED_FILES")
MESSAGE ( STATUS ">>>>>>>>>>>>>>>>>>>>>>>>>> Smaller generated files enabled  "  )
else ( enable-smaller-generated-files )
MESSAGE ( STATUS ">>>>>>>>>>>>>>>>>>>>>>>>> Smaller generated files disabled" )
endif ( enable-smaller-generated-files )


#/CLR  /MDd /TP 
if (WIN32)
# /TP to indicate files are C++
# / CLR common intermediate language
# /GL whole program optimization
# /O1 optimization for small files
# /Ob0 disable inline expansion
# /MP multiple processors compilation
# /0s small files
set( CMAKE_BUILD_TYPE Release )
set( CMAKE_CXX_FLAGS " /TP /MP /O1 /Os /GR- /EHsc /wd4541 " )
set( CMAKE_C_FLAGS "   /wd4541 " )
set( CMAKE_LINKER_FLAGS " /INCREMENTAL:NO" )
set( CMAKE_SHARED_LINKER_FLAGS " /INCREMENTAL:NO" )
else (WIN32)
set( CMAKE_CXX_FLAGS " -fexceptions -DHAVE_CONFIG_H -g -fPIC -fvisibility=hidden" )
set( CMAKE_C_FLAGS " -fexceptions -DHAVE_CONFIG_H -g -fPIC -fvisibility=hidden" )
#set( CMAKE_CXX_FLAGS " -fexceptions -DHAVE_CONFIG_H -g -fPIC " )
#set( CMAKE_C_FLAGS " -fexceptions -DHAVE_CONFIG_H -g -fPIC " )
endif (WIN32)

#set(  CMAKE_VERBOSE_MAKEFILE  true )

set(ROSE_TOP_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(ROSE_TOP_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR})

#Generate our configure file

# --------- please do all system, platform, software tests here --------------
# a set of common features: including endian, stdio.h, printf, size of long int ,etc
#taken from LyX
include(ConfigureChecks)
#include(KDE4Defaults)
#  a collection of macros which extend the built-in cmake commands
include(MacroLibrary)
# Try to find MySQL / MySQL Embedded library
# MYSQL_FOUND,MYSQL_INCLUDE_DIR,MYSQL_LIBRARIES, etc
include(FindMySQL)



# tps (01/04/2010) : added verbose output 
# ------------------------- Begin Generic CMake Variable Logging ------------------

# if you are building in-source, this is the same as CMAKE_SOURCE_DIR, otherwise 
# this is the top level directory of your build tree 
MESSAGE( STATUS "CMAKE_BINARY_DIR:         " ${CMAKE_BINARY_DIR} )

# if you are building in-source, this is the same as CMAKE_CURRENT_SOURCE_DIR, otherwise this 
# is the directory where the compiled or generated files from the current CMakeLists.txt will go to 
MESSAGE( STATUS "CMAKE_CURRENT_BINARY_DIR: " ${CMAKE_CURRENT_BINARY_DIR} )

# this is the directory, from which cmake was started, i.e. the top level source directory 
MESSAGE( STATUS "CMAKE_SOURCE_DIR:         " ${CMAKE_SOURCE_DIR} )

# this is the directory where the currently processed CMakeLists.txt is located in 
MESSAGE( STATUS "CMAKE_CURRENT_SOURCE_DIR: " ${CMAKE_CURRENT_SOURCE_DIR} )

# contains the full path to the top level directory of your build tree 
MESSAGE( STATUS "PROJECT_BINARY_DIR: " ${PROJECT_BINARY_DIR} )

# contains the full path to the root of your project source directory,
# i.e. to the nearest directory where CMakeLists.txt contains the PROJECT() command 
MESSAGE( STATUS "PROJECT_SOURCE_DIR: " ${PROJECT_SOURCE_DIR} )

# set this variable to specify a common place where CMake should put all executable files
# (instead of CMAKE_CURRENT_BINARY_DIR)
MESSAGE( STATUS "EXECUTABLE_OUTPUT_PATH: " ${EXECUTABLE_OUTPUT_PATH} )

# set this variable to specify a common place where CMake should put all libraries 
# (instead of CMAKE_CURRENT_BINARY_DIR)
MESSAGE( STATUS "LIBRARY_OUTPUT_PATH:     " ${LIBRARY_OUTPUT_PATH} )

# tell CMake to search first in directories listed in CMAKE_MODULE_PATH
# when you use FIND_PACKAGE() or INCLUDE()
MESSAGE( STATUS "CMAKE_MODULE_PATH: " ${CMAKE_MODULE_PATH} )

# this is the complete path of the cmake which runs currently (e.g. /usr/local/bin/cmake) 
MESSAGE( STATUS "CMAKE_COMMAND: " ${CMAKE_COMMAND} )

# this is the CMake installation directory 
MESSAGE( STATUS "CMAKE_ROOT: " ${CMAKE_ROOT} )

# this is the filename including the complete path of the file where this variable is used. 
MESSAGE( STATUS "CMAKE_CURRENT_LIST_FILE: " ${CMAKE_CURRENT_LIST_FILE} )

# this is linenumber where the variable is used
MESSAGE( STATUS "CMAKE_CURRENT_LIST_LINE: " ${CMAKE_CURRENT_LIST_LINE} )

# this is used when searching for include files e.g. using the FIND_PATH() command.
MESSAGE( STATUS "CMAKE_INCLUDE_PATH: " ${CMAKE_INCLUDE_PATH} )

# this is used when searching for libraries e.g. using the FIND_LIBRARY() command.
MESSAGE( STATUS "CMAKE_LIBRARY_PATH: " ${CMAKE_LIBRARY_PATH} )

# the complete system name, e.g. "Linux-2.4.22", "FreeBSD-5.4-RELEASE" or "Windows 5.1" 
MESSAGE( STATUS "CMAKE_SYSTEM: " ${CMAKE_SYSTEM} )

# the short system name, e.g. "Linux", "FreeBSD" or "Windows"
MESSAGE( STATUS "CMAKE_SYSTEM_NAME: " ${CMAKE_SYSTEM_NAME} )

# only the version part of CMAKE_SYSTEM 
MESSAGE( STATUS "CMAKE_SYSTEM_VERSION: " ${CMAKE_SYSTEM_VERSION} )

# the processor name (e.g. "Intel(R) Pentium(R) M processor 2.00GHz") 
MESSAGE( STATUS "CMAKE_SYSTEM_PROCESSOR: " ${CMAKE_SYSTEM_PROCESSOR} )

# is TRUE on all UNIX-like OS's, including Apple OS X and CygWin
MESSAGE( STATUS "UNIX: " ${UNIX} )

# is TRUE on Windows, including CygWin 
MESSAGE( STATUS "WIN32: " ${WIN32} )

# is TRUE on Apple OS X
MESSAGE( STATUS "APPLE: " ${APPLE} )

# is TRUE when using the MinGW compiler in Windows
MESSAGE( STATUS "MINGW: " ${MINGW} )

# is TRUE on Windows when using the CygWin version of cmake
MESSAGE( STATUS "CYGWIN: " ${CYGWIN} )

# is TRUE on Windows when using a Borland compiler 
MESSAGE( STATUS "BORLAND: " ${BORLAND} )

# Microsoft compiler 
MESSAGE( STATUS "MSVC: " ${MSVC} )
MESSAGE( STATUS "MSVC_IDE: " ${MSVC_IDE} )
MESSAGE( STATUS "MSVC60: " ${MSVC60} )
MESSAGE( STATUS "MSVC70: " ${MSVC70} )
MESSAGE( STATUS "MSVC71: " ${MSVC71} )
MESSAGE( STATUS "MSVC80: " ${MSVC80} )
MESSAGE( STATUS "CMAKE_COMPILER_2005: " ${CMAKE_COMPILER_2005} )




# set this to true if you don't want to rebuild the object files if the rules have changed, 
# but not the actual source files or headers (e.g. if you changed the some compiler switches) 
MESSAGE( STATUS "CMAKE_SKIP_RULE_DEPENDENCY: " ${CMAKE_SKIP_RULE_DEPENDENCY} )

# since CMake 2.1 the install rule depends on all, i.e. everything will be built before installing. 
# If you don't like this, set this one to true.
MESSAGE( STATUS "CMAKE_SKIP_INSTALL_ALL_DEPENDENCY: " ${CMAKE_SKIP_INSTALL_ALL_DEPENDENCY} )

# If set, runtime paths are not added when using shared libraries. Default it is set to OFF
MESSAGE( STATUS "CMAKE_SKIP_RPATH: " ${CMAKE_SKIP_RPATH} )

# set this to true if you are using makefiles and want to see the full compile and link 
# commands instead of only the shortened ones 
MESSAGE( STATUS "CMAKE_VERBOSE_MAKEFILE: " ${CMAKE_VERBOSE_MAKEFILE} )

# this will cause CMake to not put in the rules that re-run CMake. This might be useful if 
# you want to use the generated build files on another machine. 
MESSAGE( STATUS "CMAKE_SUPPRESS_REGENERATION: " ${CMAKE_SUPPRESS_REGENERATION} )


# A simple way to get switches to the compiler is to use ADD_DEFINITIONS(). 
# But there are also two variables exactly for this purpose: 

# the compiler flags for compiling C sources 
MESSAGE( STATUS "CMAKE_C_FLAGS: " ${CMAKE_C_FLAGS} )

# the compiler flags for compiling C++ sources 
MESSAGE( STATUS "CMAKE_CXX_FLAGS: " ${CMAKE_CXX_FLAGS} )


# Choose the type of build.  Example: SET(CMAKE_BUILD_TYPE Debug) 
MESSAGE( STATUS "CMAKE_BUILD_TYPE: " ${CMAKE_BUILD_TYPE} )

# if this is set to ON, then all libraries are built as shared libraries by default.
MESSAGE( STATUS "BUILD_SHARED_LIBS: " ${BUILD_SHARED_LIBS} )

# the compiler used for C files 
MESSAGE( STATUS "CMAKE_C_COMPILER: " ${CMAKE_C_COMPILER} )

# the compiler used for C++ files 
MESSAGE( STATUS "CMAKE_CXX_COMPILER: " ${CMAKE_CXX_COMPILER} )

# if the compiler is a variant of gcc, this should be set to 1 
MESSAGE( STATUS "CMAKE_COMPILER_IS_GNUCC: " ${CMAKE_COMPILER_IS_GNUCC} )

# if the compiler is a variant of g++, this should be set to 1 
MESSAGE( STATUS "CMAKE_COMPILER_IS_GNUCXX : " ${CMAKE_COMPILER_IS_GNUCXX} )

# the tools for creating libraries 
MESSAGE( STATUS "CMAKE_AR: " ${CMAKE_AR} )
MESSAGE( STATUS "CMAKE_RANLIB: " ${CMAKE_RANLIB} )


# ------------------------- End of Generic CMake Variable Logging ------------------







if (NOT WIN32)
include(FindOpenSSL)
include(FindSQLITE3)
endif (NOT WIN32)

message(Status "Looking for JAVA ...")
include(FindJava)

message(Status "Looking for flex ...")
include(FindFlex)
if(NOT FLEX_FOUND)
    message(FATAL_ERROR "Could not find flex command")
endif(NOT FLEX_FOUND)

message(Status "Looking for BISON ...")
include(FindBison)
FIND_BISON()
if(NOT BISON_FOUND)
    message(FATAL_ERROR "Could not find BISON command")
endif(NOT BISON_FOUND)

# include(ConvenienceLibs) # Not in use



if (NOT WIN32)
find_library(M_LIB m)
find_library(RT_LIB rt)
endif (NOT WIN32)

# define a global variable to collect all common linked third-party libraries for rose
if (NOT WIN32)
set( link_with_libraries ${Boost_LIBRARIES} ${M_LIB} ${RT_LIB} ${OPENSSL_LIBRARIES} )
else (NOT WIN32)
set( link_with_libraries ${Boost_LIBRARIES}   )
endif (NOT WIN32)

# Check compilers and version numbers, Liao 11/25/2009
# the module is located in src/cmake/modules
include(roseChooseBackendCompiler)
include(roseGenerateBackendCompilerSpecificHeaders)

# --------- finish all fundamental tests, now set derivative variables
# Liao 12/1/2009, This is essential to find the right include path from either build or installation tree
# for a translator
message(Status "Running tests ...")
if (HAVE_DLFCN_H)
  if (HAVE_DLADDR)
    set (use_rose_in_build_tree_var TRUE)
    set (CMAKE_SHARED_LINKER_FLAGS ${CMAKE_SHARED_LINKER_FLAGS} -ldl)
# this following line won't work since it only set the environment variable for cmake's session
# not for ctest session. Still no good way to set it within cmake
# fortunately,   
    set ($ENV{ROSE_IN_BUILD_TREE} ${ROSE_TOP_BINARY_DIR})
  endif (HAVE_DLADDR)
else (HAVE_DLFCN_H)
  set (use_rose_in_build_tree_var, FALSE)
endif (HAVE_DLFCN_H)

 set( LIBHARU_SOURCE_DIR ${ROSE_TOP_SRC_DIR}/src/3rdPartyLibraries/libharu-200910170404 )
 set( LIBHARU_BINARY_DIR ${ROSE_TOP_BINARY_DIR}/src/3rdPartyLibraries/libharu-200910170404 )
 set( ROSE_INCLUDES 
     ${ROSE_TOP_BINARY_DIR}
     ${ROSE_TOP_BINARY_DIR}/src/frontend/SageIII/
     ${ROSE_TOP_BINARY_DIR}/src/frontend/SageIII/astFileIO
     ${ROSE_TOP_SRC_DIR}
     ${ROSE_TOP_SRC_DIR}/src/frontend/SageIII 
     ${ROSE_TOP_SRC_DIR}/src/ROSETTA/src/
     ${ROSE_TOP_SRC_DIR}/src 
     ${ROSE_TOP_SRC_DIR}/src/frontend/CxxFrontend/EDG/EDG_SAGE_Connection/
     ${ROSE_TOP_SRC_DIR}/src/frontend/CxxFrontend/EDG/EDG_3.3/src
     ${ROSE_TOP_SRC_DIR}/src/frontend/SageIII 
     ${ROSE_TOP_SRC_DIR}/src/frontend/SageIII/astFixup 
     ${ROSE_TOP_SRC_DIR}/src/frontend/SageIII/astPostProcessing 
     ${ROSE_TOP_SRC_DIR}/src/frontend/SageIII/astMerge 
     ${ROSE_TOP_SRC_DIR}/src/frontend/SageIII/astVisualization 
     ${ROSE_TOP_SRC_DIR}/src/frontend/SageIII/astFileIO 
     ${ROSE_TOP_SRC_DIR}/src/frontend/SageIII/sageInterface 
     ${ROSE_TOP_SRC_DIR}/src/frontend/SageIII/virtualCFG 
     ${ROSE_TOP_SRC_DIR}/src/frontend/SageIII/astTokenStream 
     ${ROSE_TOP_SRC_DIR}/src/frontend/SageIII/astHiddenTypeAndDeclarationLists 
     ${ROSE_TOP_SRC_DIR}/src/frontend/SageIII/astFileIO 
     ${ROSE_TOP_SRC_DIR}/src/frontend/OpenFortranParser_SAGE_Connection 
     ${ROSE_TOP_SRC_DIR}/src/frontend/PHPFrontend 
     ${ROSE_TOP_SRC_DIR}/src/frontend/BinaryFormats 
     ${ROSE_TOP_SRC_DIR}/src/frontend/BinaryLoader
     ${ROSE_TOP_SRC_DIR}/src/frontend/BinaryDisassembly 
     ${ROSE_TOP_SRC_DIR}/src/frontend/Disassemblers 
     ${ROSE_TOP_SRC_DIR}/src/frontend/ExecFormats 
     ${ROSE_TOP_SRC_DIR}/src/backend/unparser 
     ${ROSE_TOP_SRC_DIR}/src/backend/unparser/formatSupport 
     ${ROSE_TOP_SRC_DIR}/src/backend/unparser/languageIndependenceSupport 
     ${ROSE_TOP_SRC_DIR}/src/backend/unparser/CxxCodeGeneration 
     ${ROSE_TOP_SRC_DIR}/src/backend/unparser/FortranCodeGeneration 
     ${ROSE_TOP_SRC_DIR}/src/backend/unparser/PHPCodeGeneration 
     ${ROSE_TOP_SRC_DIR}/src/backend/asmUnparser 
     ${ROSE_TOP_SRC_DIR}/src/util 
     ${ROSE_TOP_SRC_DIR}/src/util/support 
     ${ROSE_TOP_SRC_DIR}/src/util/graphs  
     ${ROSE_TOP_SRC_DIR}/src/util/stringSupport 
     ${ROSE_TOP_SRC_DIR}/src/util/commandlineProcessing 
     ${ROSE_TOP_SRC_DIR}/src/midend/abstractHandle 
     ${ROSE_TOP_SRC_DIR}/src/midend/astDiagnostics 
     ${ROSE_TOP_SRC_DIR}/src/midend/programTransformation/astInlining 
     ${ROSE_TOP_SRC_DIR}/src/midend/programTransformation/astOutlining  
     ${ROSE_TOP_SRC_DIR}/src/midend/astProcessing 
     ${ROSE_TOP_SRC_DIR}/src/midend/astQuery 
     ${ROSE_TOP_SRC_DIR}/src/midend/astRewriteMechanism 
     ${ROSE_TOP_SRC_DIR}/src/midend/astUtil/annotation 
     ${ROSE_TOP_SRC_DIR}/src/midend/astUtil/astInterface 
     ${ROSE_TOP_SRC_DIR}/src/midend/astUtil/astSupport 
     ${ROSE_TOP_SRC_DIR}/src/midend/astUtil/symbolicVal 
     ${ROSE_TOP_SRC_DIR}/src/midend/binaryAnalyses 
     ${ROSE_TOP_SRC_DIR}/src/midend/binaryAnalyses/dataflowanalyses 
     ${ROSE_TOP_SRC_DIR}/src/midend/binaryAnalyses/graph 
     ${ROSE_TOP_SRC_DIR}/src/midend/binaryAnalyses/instructionSemantics 
     ${ROSE_TOP_SRC_DIR}/src/midend/programTransformation/loopProcessing/computation 
     ${ROSE_TOP_SRC_DIR}/src/midend/programTransformation/loopProcessing/depGraph 
     ${ROSE_TOP_SRC_DIR}/src/midend/programTransformation/loopProcessing/depInfo 
     ${ROSE_TOP_SRC_DIR}/src/midend/programTransformation/loopProcessing/driver 
     ${ROSE_TOP_SRC_DIR}/src/midend/programTransformation/loopProcessing/outsideInterface 
     ${ROSE_TOP_SRC_DIR}/src/midend/programTransformation/loopProcessing/prepostTransformation 
     ${ROSE_TOP_SRC_DIR}/src/midend/programTransformation/loopProcessing/slicing 
     ${ROSE_TOP_SRC_DIR}/src/midend/programTransformation/ompLowering 
     ${ROSE_TOP_SRC_DIR}/src/midend/programAnalysis 
     ${ROSE_TOP_SRC_DIR}/src/midend/programAnalysis/CFG 
     ${ROSE_TOP_SRC_DIR}/src/midend/programAnalysis/CallGraphAnalysis 
     ${ROSE_TOP_SRC_DIR}/src/midend/programAnalysis/OAWrap 
     ${ROSE_TOP_SRC_DIR}/src/midend/programAnalysis/dataflowAnalysis 
     ${ROSE_TOP_SRC_DIR}/src/midend/programAnalysis/defUseAnalysis 
     ${ROSE_TOP_SRC_DIR}/src/midend/programAnalysis/distributedMemoryAnalysis 
     ${ROSE_TOP_SRC_DIR}/src/midend/programAnalysis/dominanceAnalysis 
     ${ROSE_TOP_SRC_DIR}/src/midend/programAnalysis/graphAnalysis 
     ${ROSE_TOP_SRC_DIR}/src/midend/programAnalysis/pointerAnal 
     ${ROSE_TOP_SRC_DIR}/src/midend/programAnalysis/staticInterproceduralSlicing 
     ${ROSE_TOP_SRC_DIR}/src/midend/programAnalysis/valuePropagation 
     ${ROSE_TOP_SRC_DIR}/src/midend/programTransformation/partialRedundancyElimination 
     ${ROSE_TOP_SRC_DIR}/src/midend/programTransformation/finiteDifferencing 
     ${ROSE_TOP_SRC_DIR}/src/midend/programTransformation/functionCallNormalization 
     ${ROSE_TOP_SRC_DIR}/src/midend/programTransformation/constantFolding 
     ${ROSE_TOP_SRC_DIR}/src/midend/programTransformation/implicitCodeGeneration 
     ${ROSE_TOP_SRC_DIR}/src/midend/programTransformation/runtimeTransformation 
     ${ROSE_TOP_SRC_DIR}/src/roseSupport 
     ${ROSE_TOP_SRC_DIR}/src/3rdPartyLibraries/MSTL 
     ${ROSE_TOP_SRC_DIR}/src/3rdPartyLibraries/libharu-2.1.0/include 
     ${ROSE_TOP_SRC_DIR}/src/3rdPartyLibraries/libharu-2.1.0/include 
     ${ROSE_TOP_SRC_DIR}/src/3rdPartyLibraries/qrose/Framework 
     ${ROSE_TOP_SRC_DIR}/src/3rdPartyLibraries/qrose/Widgets  
     ${ROSE_TOP_SRC_DIR}/projects/dataBase 
     ${ROSE_TOP_SRC_DIR}/src/util/graphs 
     ${ROSE_TOP_SRC_DIR}/src/3rdPartyLibraries/libharu-2.1.0/win32/include/
     ${ROSE_TOP_SRC_DIR}/src/midend/astUtil/astInterface )

IF(WIN32) 
set( ROSE_INCLUDES ${ROSE_INCLUDES}
     ${ROSE_TOP_SRC_DIR}/winspecific )
ENDIF()


include_directories(${KDE4_INCLUDES} ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} )
#include_directories( ${ROSE_INCLUDES} )

message(Status "Enabling QT ...")
set(QT_INCLUDES ${ROSE_INCLUDES} ${Boost_INCLUDE_DIRS} )

add_subdirectory(config)
add_subdirectory(src)
add_subdirectory(tests)

enable_testing()

# put this to the end to catch all variables defined above
# e.g. compiler names, versions etc.
configure_file(${ROSE_TOP_SRC_DIR}/rose_config.h.in.cmake ${ROSE_TOP_BINARY_DIR}/rose_config.h)
message(Status "Finished ...")


########### install files ###############

#install(FILES  rose_config.h DESTINATION ${INCLUDE_INSTALL_DIR} )



#original Makefile.am contents follow:

#include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs
#include $(top_srcdir)/projects/compass/src/compassSupport/compass_dirs.inc
#
## Use automake conditional to build Public and Developer distributions
##    Public: includes all of ROSE and binary of EDG work and parts that
##            are more restricted.
##    Developer: This version includes all EDG source code and some parts that
##             might be more restricted.
## This next line will be removed for binary distributions, leaving the variable empty
#DEVELOPER_DISTRIBUTION_DIRECTORY = developersScratchSpace proposals
#
## DQ (1/18/2009): Added demo directory to support showoffs :-).
## DQ "config" and "src" must preceed any other directories, and 
## "tutorial" and "exampleTranslators" must preceed "docs" since 
## they generate data that is subsequently used in the generated 
## ROSE documentation (examples, input codes, and generated output).
## SUBDIRS = config src projects tests tutorial exampleTranslators docs developersScratchSpace tools proposals scripts
#SUBDIRS = libltdl config src projects tests tutorial exampleTranslators docs tools scripts demo binaries $(DEVELOPER_DISTRIBUTION_DIRECTORY)
#
#
#ACLOCAL_AMFLAGS = -I ./config -I ./acmacros -I ./libltdl -I /usr/share/aclocal
#
## Force rose.docs to be make by default 
## (if rose.docs.in is modified)
#all-local: rose.docs 
#
## DQ (3/21/2009): This takes several minutes under Cygwin, so skip these test there.
#if ROSE_BUILD_OS_IS_CYGWIN
#  PROJECT_SOURCE_CODE_TESTS = 
#else
#
## Liao, 6/27/2009 skip the test if PCH is not used
#if ROSE_PCH 
#PROJECT_SOURCE_CODE_TESTS = detect_roseh detect_rosec
#else  
#PROJECT_SOURCE_CODE_TESTS = 
#endif
#
#endif
#
## all: detect_roseh detect_rosec
#all: $(PROJECT_SOURCE_CODE_TESTS)
#
## tps: script that checks that no header file includes rose.h
## rpm: exclude build directories (they contain "include-staging") since ROSE itself creates some source files.
#detect_roseh:
#	touch $(top_builddir)/src/frontend/SageIII/rose.h.pch
#	rm $(top_builddir)/src/frontend/SageIII/rose.h.pch
#	@echo "***************************************************************************************"
#	@echo "***** ROSE uses pre-compiled headers and needs to check integrity. ********************"
#	@echo "***** ROSE ensures that no header files includes rose.h and that   ********************"
#	@echo "***** the first header file in a source file is rose.h (if used)   ********************"
#	@echo "***************************************************************************************"
#	@echo Testing for rose.h in header files...
#	@if test `find $(top_srcdir) \( -type d -exec test -d "{}/include-staging" \; -prune \) -o -type f \( -name "*.h" -o -name "*.hh" \) -exec grep -H "#include \"rose.h\"\|#include <rose.h>" {} \; |  grep -v "//#include \"rose.h\"\|//#include <rose.h>" | grep -v "// #include \"rose.h\"\|// #include <rose.h>" | wc | awk '{print \$$1}'` -eq 0; then echo "rose.h header check passed."; else echo "header files exist that include rose.h"; find $(top_srcdir) \( -type d -exec test -d "{}/include-staging" \; -prune \) -o -type f \( -name "*.h" -o -name "*.hh" \) -exec grep -H "#include \"rose.h\"\|#include <rose.h>" {} \; | grep -v "//#include \"rose.h\"\|//#include <rose.h>" | grep -v "// #include \"rose.h\"\|// #include <rose.h>" ; exit 1; fi
#
## tps: added script from Andreas with small modifications - find all C files that dont include rose in the top of the file 
## rpm: exclude build directories (they contain "include-staging") since ROSE itself creates some source files.
#detect_rosec:
#	@echo Testing for rose.h in source files...
#	@if test `find $(top_srcdir) \( -type d -exec test -d "{}/include-staging" \; -prune \) -o -type f \( -name "*.C" -o -name "*.cpp" -o -name "*.cc" \) -exec grep -H "#include" {} \; | grep "\"rose.h\"\|<rose.h>" | grep -v "\/\/" | cut -d ':' -f 1 | grep -v "preproc-fortran.cc" | grep -v "preproc-fortran-fixed.cc" | uniq | xargs -n 1 grep -m 1 -HP "^#include" | grep -v "rose.h\|mpi.h" | wc | awk '{print \$$1}'` -eq 0; then echo "rose.h source file check passed.";  else echo "There are source files that dont include rose.h as the first header file."; find $(top_srcdir) \( -type d -exec test -d "{}/include-staging" \; -prune \) -o -type f \( -name "*.C" -o -name "*.cpp" -o -name "*.cc" \) -exec grep -H "#include" {} \; | grep "\"rose.h\"\|<rose.h>" | grep -v "\/\/" | cut -d ':' -f 1 | grep -v "preproc-fortran.cc" | grep -v "preproc-fortran-fixed.cc" | uniq | xargs -n 1 grep -m 1 -HP "^#include" | grep -v "rose.h\|mpi.h"; exit 1; fi
#
#
## DQ (3/9/2009): Additional things that we can enforce within ROSE (written by Andreas).
## Additional rules to enforce:
##    1) Finding using declarations in header files:
##       find -name "*.h" -exec grep -nPH "^[ \t]*using[ \t]+namespace" {} \;
##    2) Finding link dependences in Makefile.am files:
##       find -name Makefile.am -exec grep -nPH "^[ \t]*[a-zA-Z]*[_]?LDADD[ \t]*" {} \;
#
#
## DQ (6/29/2004): I don't think we need this!
## LIBS = @LIBS@ ${SAGE_LIBS}
#
## It seems that it is better to handle the config directory with its own Makefile.am
## This allows us to hide autoconf accessory files (config.guess, etc.)
## Copy the config directory to the distribution: why?
## (because it has the file: config/Makefile.for.ROSE.includes.and.libs)
## EXTRA_DIST = stamp-h.in stamp-h1.in GNU_HEADERS config
## EXTRA_DIST = stamp-h.in stamp-h1.in rose.docs.in GNU_HEADERS COPYWRITE ROSE_ResearchPapers
#EXTRA_DIST = stamp-h.in stamp-h1.in rose.docs.in COPYRIGHT LicenseInformation ROSE_ResearchPapers README.OSX README.Cygwin build ChangeLog2
#
## At some point we want to put tals into the distribution (likely just PDF files, but not yet).
## ROSE_Talks
#
## clean-local explanation:
## Remove template repositories.  No source code is compiled, but configuration
##   tests may have created template repositories here.
#clean-local:
#	rm -rf Templates.DB ii_files ti_files *.csv
#
## DQ (9/8/2006): Modified to remove backend specific header files
#uninstall-local:
#	rm -rf $(DESTDIR)$(includedir)/*_HEADERS
#
#distclean-local:
#	-rm -rf $(top_builddir)/include-staging/
#
#nodist_include_HEADERS = rose_config.h
#
## DQ (4/5/2009): This rule is not called when compiling this directory, it is only
## called when compiling the /src/util directory (perhaps it should be located in 
## that directory's Makefile.am.
## DQ (12/3/2007): Added rose_paths.h so that it would be copied to the install tree.
## JJW (8/25/2008): Changed to a .c file
#include $(top_srcdir)/config/build_rose_paths.Makefile
#
#check-local:
#	@echo '************************************************************'
#	@echo '****** make check rule complete (terminated normally) ******'
#	@echo '************************************************************'
#
#install-exec-local:
#	@echo '*****************************************************************'
#	@echo '***** make install-exec rule complete (terminated normally) *****'
#	@echo '*****************************************************************'
#
#install-data-local:
#	-rm -rf $(DESTDIR)$(includedir)/*_HEADERS
#	-mkdir $(DESTDIR)$(includedir)
#	cp -R $(top_builddir)/include-staging/* $(DESTDIR)$(includedir)/
#	@echo '*****************************************************************'
#	@echo '***** make install-data rule complete (terminated normally) *****'
#	@echo '*****************************************************************'
#
## DQ (4/22/2005): Set this so that make distcheck will use the same 
## --with-edg_source_code=true/false option as were used at configure.
## JJW (5/14/2008): Add the Boost flag the same way, using an internal
## variable from AX_BOOST_* to find the argument to --with-boost when
## this copy of ROSE was originally configured.
#DISTCHECK_CONFIGURE_FLAGS = --with-boost=$(ac_boost_path)
#
## DQ (7/25/2008): If ROSE was originallly configured with Fortran (by tuning 
## on the java support) then make sure it is tested as part of the distcheck rule.
#if ROSE_USE_OPEN_FORTRAN_PARSER
#   DISTCHECK_CONFIGURE_FLAGS += --with-java
#endif
#
## DQ (7/25/2008): If ROSE was originallly configured to use the HPC Toolkit
## then make sure it is tested as part of the distcheck rule.
#if ROSE_BUILD_ROSEHPCT
#   DISTCHECK_CONFIGURE_FLAGS += --enable-rosehpct
#endif
#
## DQ (7/25/2008): If ROSE was originallly configured to use the javaport
## work (generation of Java interface functions for ROSE using SWIG) then 
## make sure it is tested as part of the distcheck rule.
#if ENABLE_JAVAPORT
#   DISTCHECK_CONFIGURE_FLAGS += --with-javaport
#endif
#
## DQ (4/22/2005): To make clear when binaries are in the distribution as 
## opposed to EDG source code we rename the final generated distribution.
## JJW (8/3/2008): This is only used for source distributions now --
## source_with_binary_edg_dist does its own renaming
#rename_distribution:
#	new_name="$(distdir)-source-with-EDG-source.tar.gz"; \
#	cp $(distdir).tar.gz $$new_name
#
## We reset distdir as shown on
## http://jcalcote.wordpress.com/2008/02/23/autoconf-macros-exposed-at3/ to add the SVN
## revision number
#distdir = $(PACKAGE)-$(VERSION)$(DOT_SVNREV)
#
## Automake hook-ish rule to be run after dist rule (but hook runs before gzip so we can't use hook)
## We could build our own ROSE specific wrapper for dist and distcheck.
#dist-after:
#	@echo "Running dist hook rule ..."
#	@$(MAKE) rename_distribution
#
## hook rule to be run after distcheck rule
#distcheck-after:
#	@echo "Running distcheck hook rule ..."
#	@$(MAKE) rename_distribution
#
## CLEANFILES = rose_binary_compatibility_signature rose_binary_compatibility_signature_src
#CLEANFILES = rose_binary_compatibility_signature
#
#find_rose_lock_files:
##	du -a | grep rose_performance_report_lockfile
#	find . -name rose_performance_report_lockfile -print
#
## New rule to simplify generation of documentation (we want to avoid using the
## automake generated "docs" rule because the generation of documentation is
## dependent upon separate tools which the user might not have available
## (true for bothe the LaTeX and html documentation).
## DQ (7/23/2004): Since there is a "docs" subdirectory we need to force the build!
#FORCE_DOCS_TO_BE_MADE:
#
## DQ (7/25/2008): Running "make docs" introduces dependences that are inappropriate
## for a distribution.
#docs: FORCE_DOCS_TO_BE_MADE
#	@if [ -f $(top_srcdir)/src/midend/midend.docs ]; then \
#	   echo "    Generate all possible documentation cd docs; make documentation;"; \
#	   $(MAKE) -Ctutorial docs && \
#	   $(MAKE) -Cprojects/haskellport docs && \
#	   $(MAKE) -Ctests/roseTests/astInterfaceTests check && \
#	   $(MAKE) -Ctests/roseTests/ompLoweringTests check && \
#	   $(MAKE) -Ctests/CompileTests/UPC_tests check && \
#	   $(MAKE) -Cprojects/UpcTranslation/tests check && \
#	   $(MAKE) -Cprojects/autoParallelization/tests check && \
#	   $(MAKE) -Cdocs docs && \
#	   $(MAKE) -C$(compass_build_tooldir)/compass docs; \
#   else \
#      echo "Please look at the ROSE web site for documentation (www.roseCompiler.org)"; \
#   fi
#
#
#clean_binary_distribution: distdir
#	rm -rf $(distdir)/src/frontend/CxxFrontend/EDG*
#	rm -rf $(distdir)/src/frontend/CxxFrontend/Makefile.am # Will be replaced with another version in the binary_edg_tarball rule
#	rm -rf $(distdir)/proposals
#	rm -rf $(distdir)/developersScratchSpace
## Dan said this is OK 5/27/2009	
##	rm -rf $(distdir)/projects/assemblyToSourceAst
#	rm -rf $(distdir)/docs/testDoxygen
#
#if BINARY_EDG_TARBALL_ENABLED
#
#rose_binary_compatibility_signature:
#	find $(srcdir)/src/ROSETTA $(srcdir)/src/frontend/CxxFrontend \( -name \*.C -o -name \*.c -o -name \*.h -o -name \*.code -o -name \*.macro -o -name astNodeList -o -name \*.yy \) -a -print | env LC_ALL=C sort | xargs cat -ve | \
#	@MD5@ | cut -f 1 -d \  > rose_binary_compatibility_signature
#	cat rose_binary_compatibility_signature
#
#.PHONY : rose_binary_compatibility_signature # Always rebuild the file when needed
#
## 1
## This rule generates the simple binary tarball for the directories: EDG and EDG_SAGE_Connection
#binary_edg_tarball: rose_binary_compatibility_signature
#	cd src/ROSETTA/src; $(MAKE)
#	cd src/frontend/CxxFrontend; $(MAKE)
##	BINARY_EDG_DIRNAME=roseBinaryEDG-$(build_triplet_without_redhat)-`cat rose_binary_compatibility_signature` &&
#	BINARY_EDG_DIRNAME=roseBinaryEDG-$(build_triplet_without_redhat)-@GENERATED_COMPILER_NAME_AND_VERSION_SUFFIX@-`cat rose_binary_compatibility_signature` && \
#	rm -rf $${BINARY_EDG_DIRNAME} && \
#	mkdir $${BINARY_EDG_DIRNAME} && \
#	sed "s@^dependency_libs.*@dependency_libs=''@" src/frontend/CxxFrontend/libroseEDG.la > $${BINARY_EDG_DIRNAME}/libroseEDG.la && \
#	cp -Rp src/frontend/CxxFrontend/.libs $${BINARY_EDG_DIRNAME}/ && \
#	tardir=$${BINARY_EDG_DIRNAME} && $(am__tar) | GZIP=$(GZIP_ENV) gzip -c >$${BINARY_EDG_DIRNAME}.tar.gz && \
#	rm -rf $${BINARY_EDG_DIRNAME}
#
## 2
## This rule will insert the binary compatability signature into the filename for the generated binary.
#copy_binary_edg_tarball_to_source_tree: binary_edg_tarball rose_binary_compatibility_signature
##	BINARY_EDG_DIRNAME=roseBinaryEDG-$(build_triplet_without_redhat)-`cat rose_binary_compatibility_signature`
##	BINARY_EDG_DIRNAME=roseBinaryEDG-$(build_triplet_without_redhat)-@GENERATED_COMPILER_NAME_AND_VERSION_SUFFIX@-`cat rose_binary_compatibility_signature` && 
##	$(top_srcdir)/scripts/copy_binary_edg_tarball_to_source_tree $(top_srcdir) "@build_triplet_without_redhat@"
#	$(top_srcdir)/scripts/copy_binary_edg_tarball_to_source_tree $(top_srcdir) "@build_triplet_without_redhat@" "@GENERATED_COMPILER_NAME_AND_VERSION_SUFFIX@"
#
## 3
## Commits the EDG binary into the SVN repository.
## copy_binary_edg_tarball_to_source_tree_svn: binary_edg_tarball rose_binary_compatibility_signature
#copy_binary_edg_tarball_to_source_tree_svn: copy_binary_edg_tarball_to_source_tree
##	BINARY_EDG_DIRNAME=roseBinaryEDG-$(build_triplet_without_redhat)-`cat rose_binary_compatibility_signature` && 
##	$(top_srcdir)/scripts/copy_binary_edg_tarball_to_source_tree_svn `sed -n 5p $(top_srcdir)/.svn/entries` "@build_triplet_without_redhat@"
#	$(top_srcdir)/scripts/copy_binary_edg_tarball_to_source_tree_svn `sed -n 5p $(top_srcdir)/.svn/entries` "@build_triplet_without_redhat@" "@GENERATED_COMPILER_NAME_AND_VERSION_SUFFIX@" `sed -n 4p $(top_srcdir)/.svn/entries`
#
## 4
## Builds source release containing the EDG binary.
## source_with_binary_edg_dist: rose_binary_compatibility_signature
## source_with_binary_edg_dist: copy_binary_edg_tarball_to_source_tree
#
## DQ (9/16/2009): I think it is an error to call the copy_binary_edg_tarball_to_source_tree_svn rule, 
##    call copy_binary_edg_tarball_to_source_tree (as it used to be). It fails to work on a distribution
##    because there in no ".svn/entries" file.
##    ../sourcetree/scripts/copy_binary_edg_tarball_to_source_tree_svn `sed -n 5p ../sourcetree/.svn/entries` "i686-apple-darwin" "GNU-4.0"
##    sed: ../sourcetree/.svn/entries: No such file or directory
## source_with_binary_edg_dist: copy_binary_edg_tarball_to_source_tree_svn
#source_with_binary_edg_dist: copy_binary_edg_tarball_to_source_tree
#	@ROSE_BINARY_COMPATIBILITY_SIGNATURE=`cat rose_binary_compatibility_signature` && \
#	if test `ls -1 $(srcdir)/src/frontend/CxxFrontend/roseBinaryEDG-*-$${ROSE_BINARY_COMPATIBILITY_SIGNATURE}.tar.gz | wc -l` -eq 0; then echo "Error: Could not find any binary versions of EDG with signature $${ROSE_BINARY_COMPATIBILITY_SIGNATURE} in $(srcdir)/src/frontend/CxxFrontend.  This distribution cannot be made because it would not be usable on any platform." >&2; exit 1; fi && \
#	if test `ls -1 $(srcdir)/src/frontend/CxxFrontend/roseBinaryEDG-*-*.tar.gz | grep -v $${ROSE_BINARY_COMPATIBILITY_SIGNATURE} | wc -l` -ne 0; then echo "Error: The following obsolete binary versions of EDG were found in $(srcdir)/src/frontend/CxxFrontend." >&2; ls -1 $(srcdir)/src/frontend/CxxFrontend/roseBinaryEDG-*-*.tar.gz | grep -v $${ROSE_BINARY_COMPATIBILITY_SIGNATURE} >&2; exit 1; fi && \
#	$(MAKE) clean_binary_distribution && \
#	cp $(srcdir)/src/frontend/CxxFrontend/roseBinaryEDG-*-$${ROSE_BINARY_COMPATIBILITY_SIGNATURE}.tar.gz $(distdir)/src/frontend/CxxFrontend && \
#	cat $(top_srcdir)/configure.in | \
#		grep -v '^developersScratchSpace/' | \
#		grep -v '^proposals/' | \
#		grep -v '^src/frontend/CxxFrontend/EDG' | \
#		grep -v '^docs/testDoxygen/' | \
#		sed s/RELEASE_BINARY_COMPATIBILITY_SIGNATURE/$${ROSE_BINARY_COMPATIBILITY_SIGNATURE}/g > \
#		$(distdir)/configure.in
#	cat $(top_srcdir)/Makefile.am | \
#		grep -v 'DEVELOPER_DISTRIBUTION_DIRECTORY *=' > \
#		$(distdir)/Makefile.am # Remove SUBDIRS references to developersScratchSpace and proposals
#	cat $(top_srcdir)/docs/Makefile.am | \
#		grep -v 'DEVELOPER_DISTRIBUTION_DIRECTORY *=' > \
#		$(distdir)/docs/Makefile.am # Remove SUBDIRS reference to testDoxygen
#	cp $(top_srcdir)/src/frontend/CxxFrontend/Makefile.am.forBinaryEDG $(distdir)/src/frontend/CxxFrontend/Makefile.am
#	echo "" >> $(distdir)/src/frontend/CxxFrontend/Makefile.am
#	cd $(distdir)/src/frontend/CxxFrontend; echo "EXTRA_DIST = `echo *.tar.gz`" >> Makefile.am
#	cd $(distdir) && ./build
#	find $(distdir)/ -name Makefile.in~ | xargs rm -rf
#	find $(distdir)/ -depth -name .svn -exec rm -rf {} \;
#	find $(distdir)/ -depth -name autom4te.cache -exec rm -rf {} \;
#	tardir=$(distdir) && $(am__tar) | GZIP=$(GZIP_ENV) gzip -c >$(PACKAGE)-$(VERSION)-source-with-EDG-binary$(DOT_SVNREV).tar.gz
#	$(am__remove_distdir)
#
## DQ (9/10/2009): Build a tarball for testing at NMI (ROSE without EDG source)
## Some of the names of these makefile rules should be changed to be more specific.
#$(PACKAGE)-$(VERSION)-source-with-EDG-binary.tar.gz: source_with_binary_edg_dist
##	$(MAKE) copy_binary_edg_tarball_to_source_tree
##	$(MAKE) copy_binary_edg_tarball_to_source_tree_svn
##	$(MAKE) source_with_binary_edg_dist
#
## DQ (9/14/2009): this can fail if the most recent binaries are not available.
#binary_tarball: $(PACKAGE)-$(VERSION)-source-with-EDG-binary.tar.gz
#
#endif
