include_directories( ${CMAKE_SOURCE_DIR}/src/frontend/Experimental_General_Language_Support )

add_library(sageInterface OBJECT
  sageInterface.C
  sageInterfaceAda.C
  sageInterfaceAsm.C
  sageInterface_asm.C
  sageInterface_type.C
  generateUniqueName.C
  sageBuilder.C
  sageBuilder_fortran.C
  SageBuilderAsm.C
  abiStuff.C)

add_dependencies(sageInterface rosetta_generated)

install(
  FILES
  sageInterface.h sageBuilder.h SageBuilderAsm.h integerOps.h abiStuff.h sageRti.h sageCopy.h
  sageFunctors.h sageGeneric.h sageInterfaceAsm.h sageContainer.h sageHash.h sageTraversal.h
  sageMemory.h
  
  DESTINATION ${INCLUDE_INSTALL_DIR}
  )
