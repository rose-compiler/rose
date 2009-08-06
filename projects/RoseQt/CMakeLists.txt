
list_subdirectories( WIDGETS_DIRECTORIES ${CMAKE_SOURCE_DIR}/widgets 0  EXCLUDE rose.h.gch )

include_directories  ( ${WIDGETS_DIRECTORIES}
                       ${CMAKE_CURRENT_BINARY_DIR} 
                       ${ROSE_INCLUDE_DIRS} )

SET ( DEMO_HEADER_FILES MainWindow.h )
SET ( DEMO_CPP_FILES    MainWindow.cpp main.cpp )
SET ( DEMO_UI_FILES     MainWindow.ui )

QT4_WRAP_CPP( DEMO_MOC_OUTPUT ${DEMO_HEADER_FILES} )
QT4_WRAP_UI ( DEMO_UI_OUTPUT ${DEMO_UI_FILES}      )

add_executable        ( roseQtDemo ${DEMO_CPP_FILES} ${DEMO_MOC_OUTPUT} ${DEMO_UI_FILES} )
target_link_libraries ( roseQtDemo rose_widgets  )
target_link_libraries ( roseQtDemo xml2 )
target_link_libraries ( roseQtDemo rose rosehpct )
target_link_libraries ( roseQtDemo ${QT_LIBRARIES} ${Boost_LIBRARIES}  )
target_link_libraries ( roseQtDemo elf dwarf )

set_target_properties ( roseQtDemo PROPERTIES COMPILE_FLAGS "-DWITHOUT_QROSE")
