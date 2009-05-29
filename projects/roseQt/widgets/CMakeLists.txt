
set ( WIDGETS_FILES )


add_widget( WIDGETS_FILES   AsmInstructionBar EXCLUDE )
add_widget( WIDGETS_FILES   AstBrowserWidget   EXCLUDE )
add_widget( WIDGETS_FILES   AstGraphWidget     EXCLUDE treeTest.cpp 
                                                       TreeLayoutGenerator.h 
                                                       DisplayNode.h 
                                                       DisplayEdge.h)
					                   
add_widget( WIDGETS_FILES NodeInfoWidget     EXCLUDE )
add_widget( WIDGETS_FILES PropertyTreeWidget EXCLUDE )
add_widget( WIDGETS_FILES RoseFileSelector   EXCLUDE )
add_widget( WIDGETS_FILES KiviatView         EXCLUDE )
add_widget( WIDGETS_FILES MetricsKiviat      EXCLUDE )
add_widget( WIDGETS_FILES MetricsConfig      EXCLUDE )
add_widget( WIDGETS_FILES MetricFilter       EXCLUDE )
add_widget( WIDGETS_FILES QtDesignerPlugins  EXCLUDE )
add_widget( WIDGETS_FILES BeautifiedAst      EXCLUDE BAstNode.h)
add_widget( WIDGETS_FILES RoseCodeEdit       EXCLUDE )
add_widget( WIDGETS_FILES util               EXCLUDE ItemTreeNode.h
                                                     AstFilters.h )
                                                     
                                                     
LIST( APPEND WIDGETS_FILES ${CMAKE_CURRENT_SOURCE_DIR}/AstProcessing/AstProcessing.cpp )
LIST( APPEND WIDGETS_FILES ${CMAKE_CURRENT_SOURCE_DIR}/FlopCounter/FlopCounter.h
                           ${CMAKE_CURRENT_SOURCE_DIR}/FlopCounter/FlopCounter.cpp
                           ${CMAKE_CURRENT_SOURCE_DIR}/FlopCounter/Flops.h
                           ${CMAKE_CURRENT_SOURCE_DIR}/FlopCounter/Flops.cpp )
LIST( APPEND WIDGETS_FILES ${CMAKE_CURRENT_SOURCE_DIR}/AsmSourceMapper/AsmFunctions.cpp )
LIST( APPEND WIDGETS_FILES ${CMAKE_CURRENT_SOURCE_DIR}/InstructionCountAnnotator/InstructionCountAnnotator.cpp 
                           ${CMAKE_CURRENT_SOURCE_DIR}/InstructionCountAnnotator/PTracer.cpp )


include_directories( ${CMAKE_CURRENT_BINARY_DIR} util )

SET ( QCODE_EDIT_PATH RoseCodeEdit/QCodeEdit )
include_directories( ${QCODE_EDIT_PATH} 
                     ${QCODE_EDIT_PATH}/widgets 
                     ${QCODE_EDIT_PATH}/document 
                     ${QCODE_EDIT_PATH}/qnfa )

add_library           ( rose_widgets SHARED ${WIDGETS_FILES} )
add_precompiled_header( rose_widgets "rose.h")


target_link_libraries ( rose_widgets ${QT_LIBRARIES}     )   
target_link_libraries ( rose_widgets qcodeedit     )        
target_link_libraries ( rose_widgets rose rosehpct xml2 )


add_subdirectory ( RoseCodeEdit/QCodeEdit )

# for building of the layouter test
add_subdirectory ( AstGraphWidget )


add_executable( mapperTest AsmSourceMapper/main.cpp )
link_directories( /home/heller/programming/dyninstAPI/src/i386-unknown-linux2.4/lib/API/src/i386-unknown-linux2.4/lib )
target_link_libraries( mapperTest ${QT_LIBRARIES} rose elf dwarf rose_widgets )
