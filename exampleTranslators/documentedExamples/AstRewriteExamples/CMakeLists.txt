# DQ (3/21/2009): fails to compile under Cygwin.
if(NOT CYGWIN)
  add_executable(astRewriteExample1 astRewriteExample1.C)
  target_link_libraries(astRewriteExample1 ROSE_DLL ${Boost_LIBRARIES})
endif()
