
include_directories(${qtWidgets_INCLUDE_DIRS})

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
  RoseWidgetsCollection.cpp
)

target_link_libraries(QtDesignerPlugins
  ROSE_DLL RoseQtWidgets ${Boost_LIBRARIES})

