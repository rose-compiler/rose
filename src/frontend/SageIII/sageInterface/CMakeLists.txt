add_library(sageInterface OBJECT
  sageInterface.C
  sageInterface_asm.C
  sageInterface_type.C
  generateUniqueName.C
  sageBuilder.C
  sageBuilder_fortran.C
  sageBuilder_untypedNodes.C
  sageBuilderAsm.C
  abiStuff.C)
add_dependencies(sageInterface rosetta_generated)

########### install files ###############
install(
  FILES  sageInterface.h sageBuilder.h sageBuilderAsm.h integerOps.h abiStuff.h
         sageFunctors.h sageGeneric.h sageInterfaceAsm.h
  DESTINATION ${INCLUDE_INSTALL_DIR}
  )
