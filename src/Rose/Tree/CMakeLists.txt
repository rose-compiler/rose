add_library(roseTree OBJECT
  Base.C
  BasicTypes.C
  Exception.C
  List.C)

install(FILES
  Base.h
  BasicTypes.h
  Exception.h
  List.h

  DESTINATION ${INCLUDE_INSTALL_DIR}/Rose/Tree)
