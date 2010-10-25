include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

INCLUDES = $(ROSE_INCLUDES) @MYSQL_INCLUDE@

# binarylibdir = $(libdir)

# DQ (1/25/2008): I think we need to use DEV_LIB here
# binarylib_LTLIBRARIES = libbinaryDatabase.la libbinaryFrontend.la libbinarydisassembly.la
# TPS (03/13/08) added ROSE_USE_BINARY_SQL, so that binary analysis can be compiled without MySQL
# This will cause the dynamic library to be built (as libbinaryDatabase.so)
noinst_LTLIBRARIES = libbinaryFrontend.la #libbinarydisassembly.la
if ROSE_USE_MYSQL
  lib_LTLIBRARIES = libbinaryDatabase.la 
# Isolated part of library that depends on the mysql database
libbinaryDatabase_la_DEPENDENCIES =
libbinaryDatabase_la_SOURCES = RoseBin.cpp  RoseBin_DB.cpp  RoseBin_DB_IDAPRO.cpp 
endif 

# Isolated part of library that is independent of the mysql database
libbinaryFrontend_la_DEPENDENCIES =
libbinaryFrontend_la_SOURCES = RoseBin_support.cpp \
     instructionDispatch.cpp \
     x86InstructionProperties.h \
     x86InstructionProperties.C \
     armInstructionEnum.h armInstructionEnumPrinter.C \
     powerpcInstructionEnum.h powerpcInstructionEnumPrinter.C \
     powerpcInstructionProperties.h powerpcInstructionProperties.C \
     readTicl.C RoseBin_file.cpp RoseFile.cpp \
     RoseBin_buildTree.cpp RoseBin_IDAPRO_buildTree.cpp \
     AST_BIN_Traversal.C


BUILT_SOURCES = armInstructionEnumPrinter.C powerpcInstructionEnumPrinter.C

CLEANFILES = $(BUILT_SOURCES)

armInstructionEnumPrinter.C: $(srcdir)/armInstructionEnum.h
	echo '#include "armInstructionEnum.h"' > armInstructionEnumPrinter.C
	echo '#include <string>' >> armInstructionEnumPrinter.C
	sed -n '/arm_unknown_instruction/,/arm_last_instruction/p' < $(srcdir)/armInstructionEnum.h | sed 's/ = 0//' | tr -d , | sed 's,^ *,",' | sed 's@$$@",@' | (echo 'static const char* armInstructionStrings[] = {'; cat; echo '""};') >> armInstructionEnumPrinter.C
	echo 'std::string toString(ArmInstructionKind k) {static const int s = sizeof(armInstructionStrings) / sizeof(*armInstructionStrings); if ((int)k < 0 || (int)k >= s) return ""; else return armInstructionStrings[(int)k];}' >> armInstructionEnumPrinter.C

powerpcInstructionEnumPrinter.C: $(srcdir)/powerpcInstructionEnum.h
	echo '#include "powerpcInstructionEnum.h"' > powerpcInstructionEnumPrinter.C
	echo '#include <string>' >> powerpcInstructionEnumPrinter.C
	sed -n '/powerpc_unknown_instruction/,/powerpc_last_instruction/p' < $(srcdir)/powerpcInstructionEnum.h | sed 's/ = 0//' | sed 's, *//.*,,' | tr -d , | sed 's,^ *,",' | sed 's/_record/./' | sed 's@$$@",@' | (echo 'static const char* powerpcInstructionStrings[] = {'; cat; echo '""};') >> powerpcInstructionEnumPrinter.C
	echo 'std::string toString(PowerpcInstructionKind k) {static const int s = sizeof(powerpcInstructionStrings) / sizeof(*powerpcInstructionStrings); if ((int)k < 0 || (int)k >= s) return ""; else return powerpcInstructionStrings[(int)k];}' >> powerpcInstructionEnumPrinter.C

# Make sure these SAGE specific header files are placed into a distribution
if ROSE_USE_MYSQL
BINARY_DISASSEMBLY_HEADER_FILES = \
   RoseBin_buildTree.h  RoseBin_DB.h  RoseBin_DB_IDAPRO.h \
   RoseBin.h  RoseBin_IDAPRO_branchGraph.h  RoseBin_IDAPRO_buildTree.h \
   RoseBin_IDAPRO_callGraph.h  RoseBin_IDAPRO_exprTree.h  RoseBin_IDAPRO_substTree.h \
   RoseBin_support.h  MyAstAttribute.h \
   AST_BIN_Traversal.h \
   readTicl.h RoseBin_file.h RoseFile.h \
   RoseBin_abstract.h \
   x86InstructionProperties.h powerpcInstructionProperties.h armInstructionEnum.h powerpcInstructionEnum.h \
   RoseObj.h
else
BINARY_DISASSEMBLY_HEADER_FILES = \
   RoseBin_IDAPRO_branchGraph.h RoseBin_buildTree.h RoseBin_IDAPRO_buildTree.h \
   RoseBin_IDAPRO_callGraph.h  RoseBin_IDAPRO_exprTree.h  RoseBin_IDAPRO_substTree.h \
   RoseBin_support.h  MyAstAttribute.h \
   AST_BIN_Traversal.h \
   readTicl.h RoseBin_file.h RoseFile.h \
   RoseBin_abstract.h \
   x86InstructionProperties.h powerpcInstructionProperties.h armInstructionEnum.h powerpcInstructionEnum.h \
   RoseObj.h
endif
# DQ (1/19/2008): Removed this file from BINARY_DISASSEMBLY_HEADER_FILES since it is not available
# colorTraversal.h  

include_HEADERS = $(BINARY_DISASSEMBLY_HEADER_FILES)

EXTRA_DIST = CMakeLists.txt make-decision-tree.tcl RoseBin_instruction_table_arm RoseBin_instruction_table_x86 #mysql_include

# DQ (9/30/2009): Added automake conditional to handle machines where we test and "tclsh" is not availble.
# Rule for maintainers (requires Tcl interpreter)
$(srcdir)/instructionDispatch.cpp: make-decision-tree.tcl RoseBin_instruction_table_arm RoseBin_instruction_table_x86
if ROSE_USE_TCLSH
	cd $(srcdir) && ../../../config/missing --run tclsh ./make-decision-tree.tcl x86 Arm
else
   echo "Warning: tclsh is not available on this machine."
endif

clean-local:
