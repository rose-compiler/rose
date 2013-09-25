
add_executable(RTC
  RTC.C
  CheckBase.C
  ArithCheck.C
  QSTraversal.C
  BinarySimpleTraversal.C
  CheckBase.h
  ArithCheck.h
  QSTraversal.h
  BinarySimpleTraversal.h
  RTC.h)
target_link_libraries(RTC ROSE_DLL ${Boost_LIBRARIES})

