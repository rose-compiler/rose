# Gradient Editor

# ------------------------------ SOURCE FILE SEARCH -----------------------------------

set ( WIDGETS_FILES )

# All subdirectories are interpreted as source folders
# excludes have to be specified manually
list_subdirectories( subdirs ${CMAKE_CURRENT_SOURCE_DIR} 1  EXCLUDE rose.h.gch QtGradientEditor AsmSourceMapper)
#list_subdirectories( WIDGETS_DIRECTORIES ${CMAKE_CURRENT_SOURCE_DIR} 0  EXCLUDE rose.h.gch QtGradientEditor )


LIST( APPEND subdirs RoseCodeEdit/QCodeEdit 
                     RoseCodeEdit/QCodeEdit/document
                     RoseCodeEdit/QCodeEdit/qnfa
                     RoseCodeEdit/QCodeEdit/widgets )

# All found folders are globbed for source-files
add_qt_folders( WIDGETS_FILES ${subdirs})

# Here some source files can be removed from library
remove_files_from_list ( WIDGETS_FILES AsmSourceMapper/main.cpp
                                       AstGraphWidget/treeTest.cpp )
                                
add_subdirectory ( QtGradientEditor )
                                        
# Set all folders as include dir
# emulates the situation after install where all header files are in one directory
include_directories( ${subdirs} QtGradientEditor )


# Include build-dir (for generated ui_*.h files)
include_directories( ${CMAKE_CURRENT_BINARY_DIR} ${ROSE_INCLUDE_DIRS} )

# QCodeEdit has own CMakeLists
#add_subdirectory ( RoseCodeEdit/QCodeEdit )

# -------------------------------- WIDGETS LIBRARY ---- -------------------------------

add_library            ( rose_widgets SHARED ${WIDGETS_FILES} )
add_precompiled_header ( rose_widgets "${ROSE_ROOT}/include/rose.h")

#SET_TARGET_PROPERTIES( rose_widgets PROPERTIES LINK_FLAGS -Wl,--whole-archive)

target_link_libraries ( rose_widgets ${QT_LIBRARIES}     )   
target_link_libraries ( rose_widgets qtgradienteditor )
target_link_libraries ( rose_widgets rose rosehpct xml2 )

# This experimental flag causes the ptracer to be built
# set_target_properties ( rose_widgets PROPERTIES COMPILE_FLAGS "-DROSEQT_EXPERIMENTAL")



# ----------------------------- ADDITIONAL (TEST-)BINARIES  -----------------------------

# Layout tester
#add_subdirectory ( AstGraphWidget )                       

# AsmSourceMapper
#add_executable        ( mapperTest AsmSourceMapper/main.cpp )   
#target_link_libraries ( mapperTest ${QT_LIBRARIES} rose elf dwarf rose_widgets )
#link_directories      ( /home/heller/programming/dyninstAPI/src/i386-unknown-linux2.4/
#                        lib/API/src/i386-unknown-linux2.4/lib )
