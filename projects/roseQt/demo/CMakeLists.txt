
SET ( HEADER_FILES MainWindow.h )
SET ( CPP_FILES    MainWindow.cpp main.cpp )
SET ( UI_FILES     MainWindow.ui )

QT4_WRAP_CPP(MOC_OUTPUT ${HEADER_FILES} )
QT4_WRAP_UI (UI_OUTPUT ${UI_FILES}      )

list_subdirectories( WIDGETS_DIRECTORIES ${CMAKE_SOURCE_DIR}/widgets 0  EXCLUDE rose.h.gch )

include_directories  ( ${WIDGETS_DIRECTORIES}
                       ${CMAKE_CURRENT_BINARY_DIR} 
                       ${ROSE_INCLUDE_DIRS} )

add_executable        ( roseQtDemo ${CPP_FILES} ${MOC_OUTPUT} ${UI_FILES} )
target_link_libraries ( roseQtDemo rose_widgets  )
target_link_libraries ( roseQtDemo xml2 )
target_link_libraries ( roseQtDemo rose rosehpct )
target_link_libraries ( roseQtDemo ${QT_LIBRARIES} )
target_link_libraries ( roseQtDemo elf dwarf )
