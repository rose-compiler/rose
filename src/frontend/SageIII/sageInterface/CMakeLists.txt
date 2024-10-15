# TODO: best practice is to use target_include_directories() but need to make sure this doesn't break tests 
include_directories(${CMAKE_SOURCE_DIR}/src/frontend/Experimental_General_Language_Support)

# Create sageInterface library 
add_library(sageInterface OBJECT sageInterface.C
                                 sageInterfaceAda.C
                                 sageInterfaceAsm.C
                                 sageInterface_asm.C
                                 sageInterface_type.C
                                 generateUniqueName.C
                                 sageBuilder.C
                                 sageBuilder_fortran.C
                                 SageBuilderAsm.C
                                 abiStuff.C)

# Build rosetta_generated before sageInterface
add_dependencies(sageInterface rosetta_generated)

# Install these FILES to DESTINATION when user runs "make install" 
install(FILES sageInterface.h sageBuilder.h SageBuilderAsm.h sageInterfaceAda.h
              integerOps.h abiStuff.h sageRti.h sageCopy.h sageFunctors.h 
              sageGeneric.h sageInterfaceAsm.h sageContainer.h sageHash.h 
              sageTraversal.h sageMemory.h
        DESTINATION ${CMAKE_INSTALL_PREFIX}/include/rose)
