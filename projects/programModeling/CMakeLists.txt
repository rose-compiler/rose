
include_directories(${MAPLE_INCLUDE_DIR})

add_executable(mapleBugReport1 mapleBugReport1.c)
target_link_libraries(mapleBugReport1
  ROSE_DLL ${Boost_LIBRARIES} ${MAPLE_LIBRARIES})

add_executable(mapleAPIExample2 mapleAPIExample2.c)
target_link_libraries(mapleAPIExample2
  ROSE_DLL ${Boost_LIBRARIES} ${MAPLE_LIBRARIES})

add_executable(mapleRoseTest mapleRoseTest.C)
target_link_libraries(mapleRoseTest
  ROSE_DLL ${Boost_LIBRARIES} ${MAPLE_LIBRARIES})

add_executable(loopTest loopTest.C mapleSupport.C programModel.C
  memoryAccessModel.C)
target_link_libraries(loopTest
  ROSE_DLL ${Boost_LIBRARIES} ${MAPLE_LIBRARIES})

