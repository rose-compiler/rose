# Self-contained Qt widgets build - only builds if Qt4 is available
# Suppress policy warning from legacy FindQt4.cmake module
if(POLICY CMP0153)
  cmake_policy(SET CMP0153 OLD)
endif()

find_package(Qt4 4.4 QUIET COMPONENTS QtCore QtGui Qt3Support)
if(NOT QT4_FOUND)
  message(STATUS "Qt4 not found or version < 4.4, skipping qtWidgets")
  return()
endif()

message(STATUS "Building Qt widgets (Qt ${QT_VERSION_MAJOR}.${QT_VERSION_MINOR})")
include(${QT_USE_FILE})

# Qt-specific definitions
add_definitions(-DQT3_SUPPORT)

# Set up include directories for all qtWidgets components
set(qtWidgets_INCLUDE_DIRS
  ${CMAKE_CURRENT_SOURCE_DIR}/AsmInstructionBar
  ${CMAKE_CURRENT_SOURCE_DIR}/AsmView
  ${CMAKE_CURRENT_SOURCE_DIR}/AstBrowserWidget
  ${CMAKE_CURRENT_SOURCE_DIR}/AstGraphWidget
  ${CMAKE_CURRENT_SOURCE_DIR}/AstProcessing
  ${CMAKE_CURRENT_SOURCE_DIR}/BeautifiedAst
  ${CMAKE_CURRENT_SOURCE_DIR}/FlopCounter
  ${CMAKE_CURRENT_SOURCE_DIR}/InstructionCountAnnotator
  ${CMAKE_CURRENT_SOURCE_DIR}/KiviatView
  ${CMAKE_CURRENT_SOURCE_DIR}/MetricsConfig
  ${CMAKE_CURRENT_SOURCE_DIR}/MetricFilter
  ${CMAKE_CURRENT_SOURCE_DIR}/MetricsKiviat
  ${CMAKE_CURRENT_SOURCE_DIR}/NodeInfoWidget
  ${CMAKE_CURRENT_SOURCE_DIR}/ProjectManager
  ${CMAKE_CURRENT_SOURCE_DIR}/PropertyTreeWidget
  ${CMAKE_CURRENT_SOURCE_DIR}/QCodeEditWidget
  ${CMAKE_CURRENT_SOURCE_DIR}/QCodeEditWidget/QCodeEdit
  ${CMAKE_CURRENT_SOURCE_DIR}/QCodeEditWidget/QCodeEdit/widgets
  ${CMAKE_CURRENT_SOURCE_DIR}/QtGradientEditor
  ${CMAKE_CURRENT_SOURCE_DIR}/RoseCodeEdit
  ${CMAKE_CURRENT_SOURCE_DIR}/RoseFileSelector
  ${CMAKE_CURRENT_SOURCE_DIR}/SrcBinView
  ${CMAKE_CURRENT_SOURCE_DIR}/TaskSystem
  ${CMAKE_CURRENT_SOURCE_DIR}/TreeModel
  ${CMAKE_CURRENT_SOURCE_DIR}/util
  ${CMAKE_CURRENT_SOURCE_DIR}/WidgetCreator
)

# Also include qrose component directories
list(APPEND qtWidgets_INCLUDE_DIRS
  ${ROSE_TOP_SRC_DIR}/src/3rdPartyLibraries/qrose/Components/Common
  ${ROSE_TOP_SRC_DIR}/src/3rdPartyLibraries/qrose/Components/QueryBox
  ${ROSE_TOP_SRC_DIR}/src/3rdPartyLibraries/qrose/Components/SourceBox
  ${ROSE_TOP_SRC_DIR}/src/3rdPartyLibraries/qrose/Components/TreeBox
)

include_directories(BEFORE ${qtWidgets_INCLUDE_DIRS})

add_subdirectory(AsmInstructionBar)
add_subdirectory(AsmView)
add_subdirectory(AstBrowserWidget)
add_subdirectory(AstGraphWidget)
add_subdirectory(AstProcessing)
add_subdirectory(BeautifiedAst)
add_subdirectory(FlopCounter)
add_subdirectory(InstructionCountAnnotator)
add_subdirectory(KiviatView)
add_subdirectory(MetricFilter)
add_subdirectory(MetricsConfig)
add_subdirectory(MetricsKiviat)
add_subdirectory(NodeInfoWidget)
add_subdirectory(ProjectManager)
add_subdirectory(PropertyTreeWidget)
add_subdirectory(QtGradientEditor)
add_subdirectory(QCodeEditWidget)
add_subdirectory(RoseCodeEdit)
add_subdirectory(RoseFileSelector)
add_subdirectory(SrcBinView)
add_subdirectory(TaskSystem)
add_subdirectory(TreeModel)
add_subdirectory(util)
add_subdirectory(WidgetCreator)

add_library(RoseQtWidgets
  $<TARGET_OBJECTS:AsmInstructionBar>
  $<TARGET_OBJECTS:AsmView>
  $<TARGET_OBJECTS:AstBrowserWidget>
  $<TARGET_OBJECTS:AstGraphWidget>
  $<TARGET_OBJECTS:AstProcessing>
  $<TARGET_OBJECTS:BeautifiedAst>
  $<TARGET_OBJECTS:FlopCounter>
  $<TARGET_OBJECTS:InstructionCountAnnotator>
  $<TARGET_OBJECTS:KiviatView>
  $<TARGET_OBJECTS:MetricFilter>
  $<TARGET_OBJECTS:MetricsConfig>
  $<TARGET_OBJECTS:MetricsKiviat>
  $<TARGET_OBJECTS:NodeInfoWidget>
  $<TARGET_OBJECTS:ProjectManager>
  $<TARGET_OBJECTS:qtgradienteditor>
  $<TARGET_OBJECTS:PropertyTreeWidget>
  $<TARGET_OBJECTS:RoseCodeEdit>
  $<TARGET_OBJECTS:RoseFileSelector>
  $<TARGET_OBJECTS:SrcBinView>
  $<TARGET_OBJECTS:TaskSystem>
  $<TARGET_OBJECTS:TreeModel>
  $<TARGET_OBJECTS:util>
  $<TARGET_OBJECTS:WidgetCreator>
)

target_link_libraries(RoseQtWidgets QCodeEditWidget ${QT_LIBRARIES})
