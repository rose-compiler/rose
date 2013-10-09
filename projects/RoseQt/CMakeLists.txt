

#CXXFLAGS += -DFRONTEND_CALL=\"$(top_builddir)/src/roseExtensions/qtWidgets/frontend_process\"

include_directories(
  ${ROSE_SOURCE_DIR}/src/roseExtensions/roseHPCToolkit/include
  ${CMAKE_CURRENT_BINARY_DIR}
  ${qtWidgets_INCLUDE_DIRS}
  ${LIBXML2_INCLUDE_DIR}
)

add_subdirectory(AstViewer)

QT4_WRAP_UI(ROSEQT_UI_CPP MainWindow.ui)
QT4_WRAP_CPP(ROSEQT_MOC_CPP MainWindow.h)

add_executable(roseQt
  MainWindow.cpp main.cpp ${ROSEQT_UI_CPP} ${ROSEQT_MOC_CPP})
target_link_librarieS(roseQt ROSE_DLL qrose RoseQtWidgets ${Boost_LIBRARIES})
