
SET ( HEADER_FILES MainWindow.h )
SET ( CPP_FILES    MainWindow.cpp main.cpp )
SET ( UI_FILES     MainWindow.ui )

QT4_WRAP_CPP(MOC_OUTPUT ${HEADER_FILES} )
QT4_WRAP_UI (UI_OUTPUT ${UI_FILES}      )

# Include for the self written widgets
include_directories   ( ${CMAKE_SOURCE_DIR}/src/AsmInstructionBar
                        ${CMAKE_SOURCE_DIR}/src/AstBrowserWidget 
                        ${CMAKE_SOURCE_DIR}/src/AstGraphWidget
                        ${CMAKE_SOURCE_DIR}/src/BeautifiedAst
                        ${CMAKE_SOURCE_DIR}/src/InstructionCountAnnotator
                        ${CMAKE_SOURCE_DIR}/src/NodeInfoWidget
                        ${CMAKE_SOURCE_DIR}/src/PropertyTreeWidget
                        ${CMAKE_SOURCE_DIR}/src/RoseFileSelector
                        ${CMAKE_SOURCE_DIR}/src/KiviatView
                        ${CMAKE_SOURCE_DIR}/src/MetricsKiviat
                        ${CMAKE_SOURCE_DIR}/src/RoseCodeEdit
                        ${CMAKE_SOURCE_DIR}/src/util  )

SET ( QCODE_EDIT_PATH ../src/RoseCodeEdit/QCodeEdit )
include_directories( ${QCODE_EDIT_PATH} 
                     ${QCODE_EDIT_PATH}/widgets 
                     ${QCODE_EDIT_PATH}/document 
                     ${QCODE_EDIT_PATH}/qnfa )
                        


include_directories  ( ${CMAKE_CURRENT_BINARY_DIR} )

add_executable        ( roseQtDemo ${CPP_FILES} ${MOC_OUTPUT} ${UI_FILES} )
target_link_libraries ( roseQtDemo rose_widgets  )
target_link_libraries ( roseQtDemo xml2 )
target_link_libraries ( roseQtDemo rose rosehpct )
target_link_libraries ( roseQtDemo qcodeedit )
target_link_libraries ( roseQtDemo elf dwarf )
