if(with-roseQt)
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
endif()
