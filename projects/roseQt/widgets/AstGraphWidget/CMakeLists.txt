

#SET ( HEADER  TreeLayoutGenerator.h )

SET ( SRC   DisplayEdge.cpp
            DisplayNode.cpp
            TreeLayoutGenerator.cpp
            treeTest.cpp) 

#QT4_WRAP_CPP(MOC_OUTPUT ${HEADER} )

add_executable        ( treeTest ${SRC} ${MOC_OUTPUT} )
target_link_libraries ( treeTest ${QT_LIBRARIES} )
