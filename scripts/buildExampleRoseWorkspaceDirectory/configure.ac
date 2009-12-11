# This is the build specification file for a ROSE workspace.
AC_INIT([Internal-ROSE-Workspace],[0.1],[dquinlan@llnl.gov])

# require autoconf version 2.59 or greater
AC_PREREQ([2.59])

# Autoconf and Automake specific macro calls.
AC_CONFIG_AUX_DIR(config)
AM_INIT_AUTOMAKE([foreign])
AC_CONFIG_SRCDIR([README])

# Autoheaders need to have this in palce else it is an error.
# we would like to have one of these, but coudl also test
# to make sure that the entries are the same for this and the
# generated "rose_config.h".
AC_CONFIG_HEADER([rose_workspace_config.h])

# These macros should be moved into ROSE directly so that any
# application workspace configuration can easily use them. They
# could maybe also be called within the ROSE SUPPORT ROSE macro.
AX_ROSE
AX_ROSE_LIB

echo "ac_rose_path     = $ac_rose_path"
echo "ax_rose_user_lib = $ax_rose_user_lib"
echo "ac_rose_lib_path = $ac_rose_lib_path"

# This is the macro to force all of the ROSE specific support
# to be called to define all macros as required for arbitrary
# ROSE projects.  The configure commandline should be the same
# for both ROSE/configure and internal_rose_workspace/configure.
ROSE_SUPPORT_ROSE
ROSE_SUPPORT_ROSE_PART_2

# Specify the files to generate (Makefile.in's and any other required files).
AC_CONFIG_FILES([
Makefile
])

if 0; then
developersScratchSpace/Makefile
developersScratchSpace/C_tests/Makefile
developersScratchSpace/C99_tests/Makefile
developersScratchSpace/Cxx_tests/Makefile
developersScratchSpace/C_subset_of_Cxx_tests/Makefile
developersScratchSpace/Fortran_tests/Makefile
developersScratchSpace/Fortran_tests/LANL_POP/Makefile
developersScratchSpace/RoseExample_tests/Makefile
developersScratchSpace/ExpressionTemplateExample_tests/Makefile
developersScratchSpace/PythonExample_tests/Makefile
developersScratchSpace/UPC_tests/Makefile
developersScratchSpace/copyAST_tests/Makefile
developersScratchSpace/colorAST_tests/Makefile
developersScratchSpace/mergeAST_tests/Makefile
developersScratchSpace/unparseToString_tests/Makefile
developersScratchSpace/OpenMP_C_tests/Makefile
developersScratchSpace/boost_tests/Makefile
developersScratchSpace/virtualCFG_tests/Makefile
developersScratchSpace/translator_tests/Makefile
developersScratchSpace/sourcePosition_tests/Makefile
developersScratchSpace/preinclude_tests/Makefile
developersScratchSpace/tokenStream_tests/Makefile
developersScratchSpace/mergeTraversal_tests/Makefile
developersScratchSpace/collectAllCommentsAndDirectives_tests/Makefile
developersScratchSpace/nameQualificationAndTypeElaboration_tests/Makefile
developersScratchSpace/Disassembler_tests/Makefile
developersScratchSpace/Dwarf_tests/Makefile
developersScratchSpace/Yices_tests/Makefile
developersScratchSpace/Compass_tests/Makefile
developersScratchSpace/astEquivalence_tests/Makefile
developersScratchSpace/MicrosoftWindows_tests/Makefile
developersScratchSpace/QRose_tests/Makefile
developersScratchSpace/libraryIdentification_tests/Makefile
developersScratchSpace/Pin_tests/Makefile
developersScratchSpace/SimPoints_tests/Makefile
developersScratchSpace/SimPoints_tests/runpinpoints.sh
developersScratchSpace/SimPoints_tests/Step2.sh
developersScratchSpace/graph_tests/Makefile
developersScratchSpace/fileLocation_tests/Makefile
developersScratchSpace/callGraph_tests/Makefile
developersScratchSpace/constructName_tests/Makefile
developersScratchSpace/dataFlow_tests/Makefile
developersScratchSpace/binaryDataStructureRecognition_tests/Makefile
developersScratchSpace/algorithmRecognition_tests/Makefile
fi

# Generate files required build (Makefiles, etc.)
AC_OUTPUT

