#Is this file needed at all? If yes, make sure it is executed.

include_directories(${KDE4_INCLUDES} ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} )


set(TestUnparseHeaders_SRCS
    TestUnparseHeaders.C
    UnparseHeadersTransformVisitor.C)

add_executable(TestUnparseHeaders ${TestUnparseHeaders_SRCS})

target_link_libraries(TestUnparseHeaders ROSE_DLL edg33 ${link_with_libraries} )
