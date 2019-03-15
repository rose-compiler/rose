if(with-roseQt)
    include_directories(${QT_QT3SUPPORT_INCLUDE_DIR} ${ROSE_BINARY_DIR}/src/roseSupport ${qtWidgets_INCLUDE_DIRS})
    add_definitions(-DQT3_SUPPORT)

    add_library(QtDesignerPlugins
      AsmViewPlugin.cpp
      AstBrowserWidgetPlugin.cpp
      AstGraphWidgetPlugin.cpp
      BAstViewPlugin.cpp
      KiviatViewPlugin.cpp
      MetricsKiviatPlugin.cpp
      NodeInfoWidgetPlugin.cpp
      ProjectViewPlugin.cpp
      PropertyTreeWidgetPlugin.cpp
      QCodeEditWidgetPlugin.cpp
      RoseCodeEditPlugin.cpp
      RoseFileComboBoxPlugin.cpp
      RoseWidgetsCollection.cpp)

    target_link_libraries(QtDesignerPlugins
      ROSE_DLL RoseQtWidgets ${Boost_LIBRARIES})
endif()
