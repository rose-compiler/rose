### Generate the library ###
if (enable-binary-analysis)
add_library(binaryFrontend OBJECT
  RoseBin_support.cpp instructionDispatch.cpp
  powerpcInstructionProperties.h powerpcInstructionProperties.C readTicl.C
  RoseBin_file.cpp RoseFile.cpp RoseBin_buildTree.cpp
  RoseBin_IDAPRO_buildTree.cpp AST_BIN_Traversal.C
)
else()
add_library(binaryFrontend OBJECT
  dummyBinaryFrontend.C
)
endif()
add_dependencies(binaryFrontend rosetta_generated)


########### install files ###############

install(FILES
    AST_BIN_Traversal.h MyAstAttribute.h
    powerpcInstructionProperties.h readTicl.h
    RoseBin_abstract.h RoseBin_buildTree.h RoseBin_file.h
    RoseBin_IDAPRO_branchGraph.h RoseBin_IDAPRO_buildTree.h
    RoseBin_IDAPRO_callGraph.h RoseBin_IDAPRO_exprTree.h
    RoseBin_IDAPRO_substTree.h RoseBin_support.h RoseFile.h
    RoseObj.h
  DESTINATION include)

