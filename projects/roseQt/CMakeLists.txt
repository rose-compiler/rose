
cmake_minimum_required( VERSION 2.6 )


if(COMMAND cmake_policy)
   cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

set( CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake )

include( ${CMAKE_MODULE_PATH}/AddQtFolders.cmake )
include( ${CMAKE_MODULE_PATH}/PrecompiledHeaders.cmake )

project( roseQt )

set( CMAKE_BUILD_TYPE Debug )
set( CMAKE_CXX_FLAGS "-ansi -Wall" )
set( CMAKE_CXX_FLAGS_DEBUG "-g3" )
set( CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG -DQT_NO_DEBUG_OUTPUT" )

# Configure and find Rose and Qt
SET ( ROSE_USE_ROSEHPCT 1 )
SET ( QT_USE_QTXML  1 )
SET ( QT_USE_QTSCRIPT 1)
SET ( QT_USE_QTSCRIPTTOOLS 1 )


FIND_PACKAGE ( Rose )
FIND_PACKAGE ( Qt4 )
INCLUDE ( ${QT_USE_FILE} )



if( NOT ROSE_FOUND )
    message( SEND_ERROR "ROSE not found" )
endif( NOT ROSE_FOUND )
if( NOT QT4_FOUND )
    message( SEND_ERROR "QT 4 not found" )
endif( NOT QT4_FOUND )


# Type of build:
set(CMAKE_BUILD_TYPE Debug)
#set(CMAKE_BUILD_TYPE Release)

#Compiler Flags
if(UNIX)
    set(CMAKE_CXX_FLAGS "-ansi -Wall" )
    set(CMAKE_CXX_FLAGS_DEBUG " -g3")
    set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DQT_NO_DEBUG_OUTPUT -DNDEBUG")
endif (UNIX)


#INCLUDE_DIRECTORIES( ${ROSE_INCLUDE_DIRS} )

SET (DWARF_DIR ~/opt/libdwarf )

include_directories( ${DWARF_DIR} )
link_directories( ${DWARF_DIR} )
LINK_DIRECTORIES( ${ROSE_LIBRARY_DIRS} )

ADD_SUBDIRECTORY ( widgets )
ADD_SUBDIRECTORY ( demo )
