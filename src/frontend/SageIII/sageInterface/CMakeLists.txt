if (enable-binary-analysis)
add_library(sageInterface OBJECT
  sageInterface.C
  sageInterfaceAsm.C
  sageInterface_asm.C
  sageInterface_type.C
  generateUniqueName.C
  sageBuilder.C
  sageBuilder_fortran.C
  sageBuilder_untypedNodes.C
  sageBuilderAsm.C
  abiStuff.C)
else()
add_library(sageInterface OBJECT
  sageInterface.C
  sageInterface_type.C
  generateUniqueName.C
  sageBuilder.C
  sageBuilder_fortran.C
  sageBuilder_untypedNodes.C
  abiStuff.C)
endif()

add_dependencies(sageInterface rosetta_generated)

install(
  FILES  sageInterface.h sageBuilder.h sageBuilderAsm.h integerOps.h abiStuff.h
         sageFunctors.h sageGeneric.h sageInterfaceAsm.h
  DESTINATION ${INCLUDE_INSTALL_DIR}
  )
########### install files ###############
