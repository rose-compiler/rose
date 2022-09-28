###########################################
# Test programs for the NodeId support
###########################################

#This is weird, we build the test binary with 'make' and it gets
#placed in the same directory as rose-cc.  So we have to access it with
#${CMAKE_BINARY_DIR}/bin/testNodeId

add_executable(testNodeId testNodeId.C)
target_link_libraries(testNodeId ROSE_DLL EDG ${link_with_libraries})

set(NODEID_SPECIMENS ${CMAKE_CURRENT_SOURCE_DIR}/../astInliningTests/specimen01_1.C
	${CMAKE_CURRENT_SOURCE_DIR}/../astInliningTests/specimen02_1.C
	${CMAKE_CURRENT_SOURCE_DIR}/../astInliningTests/specimen03_1.C
	${CMAKE_CURRENT_SOURCE_DIR}/../astInliningTests/specimen04_1.C
	${CMAKE_CURRENT_SOURCE_DIR}/../astInliningTests/specimen05_1.C
	${CMAKE_CURRENT_SOURCE_DIR}/../astInliningTests/specimen06_1.C
	${CMAKE_CURRENT_SOURCE_DIR}/../astInliningTests/specimen07_1.C
	${CMAKE_CURRENT_SOURCE_DIR}/../astInliningTests/specimen08_1.C
	${CMAKE_CURRENT_SOURCE_DIR}/../astInliningTests/specimen09_1.C
	${CMAKE_CURRENT_SOURCE_DIR}/../astInliningTests/specimen10_1.C
	${CMAKE_CURRENT_SOURCE_DIR}/../astInliningTests/specimen11_3.C
	${CMAKE_CURRENT_SOURCE_DIR}/../astInliningTests/specimen12_3.C
	${CMAKE_CURRENT_SOURCE_DIR}/../astInliningTests/specimen13_1.C
	${CMAKE_CURRENT_SOURCE_DIR}/../astInliningTests/specimen14_1.C
	${CMAKE_CURRENT_SOURCE_DIR}/../astInliningTests/specimen15_1.C
	${CMAKE_CURRENT_SOURCE_DIR}/../astInliningTests/specimen16_4.C
	${CMAKE_CURRENT_SOURCE_DIR}/../astInliningTests/specimen17_10.
	${CMAKE_CURRENT_SOURCE_DIR}/../astInliningTests/specimen18_1.C
	${CMAKE_CURRENT_SOURCE_DIR}/../astInliningTests/specimen19_1.C
	${CMAKE_CURRENT_SOURCE_DIR}/../astInliningTests/specimen20_1.C
	${CMAKE_CURRENT_SOURCE_DIR}/../astInliningTests/specimen21_1.C
	${CMAKE_CURRENT_SOURCE_DIR}/../astInliningTests/specimen22_1.C
	${CMAKE_CURRENT_SOURCE_DIR}/../astInliningTests/specimen25_1.C )

foreach(specimen ${NODEID_SPECIMENS})
  add_test(
    NAME ${specimen}
    COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/testscript.sh ${CMAKE_BINARY_DIR}/bin/testNodeId ${specimen} .
    )
endforeach()
