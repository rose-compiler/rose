cmake_minimum_required(VERSION 2.4)
project( TextEditPaintDemo )

if(COMMAND cmake_policy)
    cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)  


# Find Libraries
FIND_PACKAGE(Qt4)
INCLUDE(${QT_USE_FILE})


# Type of build:
set(CMAKE_BUILD_TYPE Debug)
#set(CMAKE_BUILD_TYPE Release)

#Compiler Flags
if(UNIX)
    set(CMAKE_CXX_FLAGS "-ansi -Wall -Wshadow" )
    set(CMAKE_CXX_FLAGS_DEBUG " -g3")
    set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DQT_NO_DEBUG_OUTPUT -DNDEBUG")
endif (UNIX)

if(WIN32)
    set(CMAKE_BUILD_TYPE Release)
    set(CMAKE_CXX_FLAGS "-DWIN32 -EHsc -MD")
    set(CMAKE_CXX_FLAGS_DEBUG " -W3 -MDd")    
    set(CMAKE_CXX_FLAGS_RELEASE "-DQT_NO_DEBUG_OUTPUT -DNDEBUG")
endif(WIN32)



# File Listing

set( HEADER_FILES customtextedit.h
                  mainwidget.h)

set( CPP_FILES    customtextedit.cpp
                  mainwidget.cpp
                  main.cpp  )
                  
set( UI_FILES     DemoWidget.ui)
set( RES_FILES    )

# End File Listing




QT4_WRAP_CPP(MOC_OUTPUT_FILES ${HEADER_FILES})
QT4_WRAP_UI(UI_OUTPUT_FILES ${UI_FILES} )
QT4_ADD_RESOURCES(RES_OUTPUT_FILES ${RES_FILES})
SET(CPP_FILES ${CPP_FILES} ${UI_OUTPUT_FILES} ${MOC_OUTPUT_FILES} ${RES_OUTPUT_FILES})

# -----------------------  EXECUTABLES AND LINKING ---------------------


# Main Programm

add_executable (text_edit_paint_demo ${CPP_FILES})
target_link_libraries (text_edit_paint_demo ${QT_LIBRARIES})

