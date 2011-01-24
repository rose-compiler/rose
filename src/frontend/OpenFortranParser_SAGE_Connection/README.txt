*********************** WARNING TO ROSE USERS ***********************

   Do not use functions contained in files within this directory.
They are inteteded only for internal use for the Fortran support
within ROSE.  They are contantly being updated and changed and
will not be always representing the same semantics from release
to release of ROSE. 

Functions to build the AST are a part of the SageBuilder namespace
and are located in directory:
    src/frontend/SageIII/sageInterface
and represented by files:
    sageBuilder.h and sageBuilderAsm.h

These files implement the support for building AST subtrees
across all languages supportin within ROSE.  Any bugs and or
missing features should be reports and will be fixed.

Thanks,
Dan


*********************** INTERNAL FORTRAN NOTES ***********************

3/11/2010: Updated to latest version of OFP (0.7.2) with Craig.
#       modified:   config/support-rose.m4
#       modified:   src/3rdPartyLibraries/antlr-jars/CMakeLists.txt
#       modified:   src/3rdPartyLibraries/antlr-jars/Makefile.am
#       modified:   src/3rdPartyLibraries/fortran-parser/CMakeLists.txt
#       modified:   src/3rdPartyLibraries/fortran-parser/Makefile.am
#       modified:   src/frontend/OpenFortranParser_SAGE_Connection/FortranParserAction.h
#       modified:   src/frontend/OpenFortranParser_SAGE_Connection/FortranParserActionROSE.C
#       modified:   src/frontend/OpenFortranParser_SAGE_Connection/c_actions_jni.c
#       modified:   src/frontend/OpenFortranParser_SAGE_Connection/jserver.C
#       modified:   src/frontend/OpenFortranParser_SAGE_Connection/token.c
#       modified:   src/frontend/SageIII/sageSupport.C
#       modified:   tests/CompileTests/Fortran_tests/Makefile.am

We now use a jar file distributed by OFP, so we no longer need the
OFP source code in src/3rdPartyLibraries/fortran-parser directory.

Needed from OFP:
  1) Source code filename and line number of include statement.
     See test2010_81.f90 and test2010_81.h for an example.
     Not clear what file to put the include directive, could
     also be deeply nested.  Need to get the main function into 
     the correct file.

