add_library( astHiddenTypeAndDeclarationLists OBJECT
  HiddenList.C
  HiddenList_Intersection.C
  HiddenList_Output.C
  NamespacesAndClassTraversal.C)
add_dependencies(astHiddenTypeAndDeclarationLists rosetta_generated)

########### install files ###############

install(FILES
  HiddenList.h  HiddenList_Intersection.h  HiddenList_Output.h
  DESTINATION ${INCLUDE_INSTALL_DIR})
