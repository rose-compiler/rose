include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

INCLUDES = $(ROSE_INCLUDES) @MYSQL_INCLUDE@

# binarylibdir = $(libdir)

# DQ (1/25/2008): I think we need to use DEV_LIB here
# binarylib_LTLIBRARIES = libbinaryDatabase.la libbinaryFrontend.la libbinarydisassembly.la
# TPS (03/13/08) added ROSE_USE_BINARY_SQL, so that binary analysis can be compiled without MySQL
# This will cause the dynamic library to be built (as libbinaryDatabase.so)
# noinst_LTLIBRARIES = libbinaryFrontend.la #libbinarydisassembly.la
noinst_LTLIBRARIES = libbinaryFrontend.la

if ROSE_USE_MYSQL
   lib_LTLIBRARIES = libbinaryDatabase.la 
 # Isolated part of library that depends on the mysql database
   libbinaryDatabase_la_DEPENDENCIES =
   libbinaryDatabase_la_SOURCES = RoseBin.cpp  RoseBin_DB.cpp  RoseBin_DB_IDAPRO.cpp 
endif 

# Isolated part of library that is independent of the mysql database
libbinaryFrontend_la_DEPENDENCIES =

if ROSE_BUILD_BINARY_ANALYSIS_SUPPORT
   libbinaryFrontend_la_SOURCES = RoseBin_support.cpp \
      instructionDispatch.cpp \
      powerpcInstructionProperties.h powerpcInstructionProperties.C \
      readTicl.C RoseBin_file.cpp RoseFile.cpp \
      RoseBin_buildTree.cpp RoseBin_IDAPRO_buildTree.cpp \
      AST_BIN_Traversal.C
else
   libbinaryFrontend_la_SOURCES = dummyBinaryFrontend.C
endif


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
   powerpcInstructionProperties.h \
   RoseObj.h
else
BINARY_DISASSEMBLY_HEADER_FILES = \
   RoseBin_IDAPRO_branchGraph.h RoseBin_buildTree.h RoseBin_IDAPRO_buildTree.h \
   RoseBin_IDAPRO_callGraph.h  RoseBin_IDAPRO_exprTree.h  RoseBin_IDAPRO_substTree.h \
   RoseBin_support.h  MyAstAttribute.h \
   AST_BIN_Traversal.h \
   readTicl.h RoseBin_file.h RoseFile.h \
   RoseBin_abstract.h \
   powerpcInstructionProperties.h \
   RoseObj.h
endif

pkginclude_HEADERS = $(BINARY_DISASSEMBLY_HEADER_FILES)

EXTRA_DIST = CMakeLists.txt make-decision-tree.tcl RoseBin_instruction_table_arm RoseBin_instruction_table_x86 dummyBinaryFrontend.C

clean-local:
