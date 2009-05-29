
macro( add_widget OUT_FILES DIRECTORY EXCLUDE )

   set( HEADER )
   set( CPP )
   set( UI )
   set( RES )
   set( EXCLUDES )
   set( MOC_OUTPUT_FILES )
   set( UI_OUTPUT_FILES )
   set( RES_OUTPUT_FILES )

   file( GLOB HEADER . ${CMAKE_CURRENT_SOURCE_DIR}/${DIRECTORY}/*.h )
   file( GLOB CPP    . ${CMAKE_CURRENT_SOURCE_DIR}/${DIRECTORY}/*.cpp )
   file( GLOB UI     . ${CMAKE_CURRENT_SOURCE_DIR}/${DIRECTORY}/*.ui )
   file( GLOB RES    . ${CMAKE_CURRENT_SOURCE_DIR}/${DIRECTORY}/*.qrc )

   foreach( arg ${ARGN} )
      list( REMOVE_ITEM HEADER ${CMAKE_CURRENT_SOURCE_DIR}/${DIRECTORY}/${arg} )
   endforeach( arg )

   QT4_WRAP_CPP(MOC_OUTPUT_FILES ${HEADER})
   QT4_WRAP_UI(UI_OUTPUT_FILES ${UI} )
   QT4_ADD_RESOURCES(RES_OUTPUT_FILES ${RES})

   LIST( APPEND ${OUT_FILES} ${CPP} ${UI_OUTPUT_FILES} ${MOC_OUTPUT_FILES} ${RES_OUTPUT_FILES} )
endmacro( add_widget )
