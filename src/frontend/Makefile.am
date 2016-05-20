include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

# Later add EDG_1.4, EDG_3.3, Open64, SAGE_III
# SUBDIRS = SageIII EDG EDG_SAGE_Connection
# SUBDIRS = SageIII EDG EDG_SAGE_Connection Open64_SAGE_Connection BinaryDisassembly
# SUBDIRS = SageIII EDG EDG_SAGE_Connection BinaryDisassembly
# SUBDIRS = SageIII EDG EDG_SAGE_Connection OpenFortranParser_SAGE_Connection BinaryDisassembly ObjdumpDisassembler
# SUBDIRS = SageIII BinaryDisassembly ObjdumpDisassembler OpenFortranParser_SAGE_Connection EDG EDG_SAGE_Connection
# SUBDIRS = SageIII CxxFrontend OpenFortranParser_SAGE_Connection BinaryDisassembly Disassemblers BinaryFormats BinaryLoader PHPFrontend

SUBDIRS = \
  SageIII \
  X10_ROSE_Connection

# Note that CXX and C are either both defined or both undefined.
if ROSE_BUILD_CXX_LANGUAGE_SUPPORT
   SUBDIRS += CxxFrontend
endif

if ROSE_BUILD_FORTRAN_LANGUAGE_SUPPORT
   SUBDIRS += OpenFortranParser_SAGE_Connection
endif

if ROSE_EXPERIMENTAL_OFP_ROSE_CONNECTION
   SUBDIRS += Experimental_OpenFortranParser_ROSE_Connection
endif


SUBDIRS += ECJ_ROSE_Connection

SUBDIRS += BinaryDisassembly Disassemblers BinaryFormats BinaryLoader Partitioner2

#if ROSE_BUILD_BINARY_ANALYSIS_SUPPORT
#   SUBDIRS += BinaryDisassembly Disassemblers BinaryFormats BinaryLoader
#else
#  SUBDIRS += BinaryFormats
#endif

if ROSE_BUILD_PHP_LANGUAGE_SUPPORT
   SUBDIRS += PHPFrontend
endif

if ROSE_BUILD_PYTHON_LANGUAGE_SUPPORT
   SUBDIRS += PythonFrontend
endif

if ROSE_BUILD_MATLAB_LANGUAGE_SUPPORT
   SUBDIRS += MatlabFrontend
endif

SUBDIRS += DLX

# Until we revisit this this, we want to include specific documentation that has been put into this directory.
# Note that OpenFortranParser_SAGE_Connection has the newer work to support Fortran in ROSE.  This work
# is part of a contract to add Fortran support to ROSE using the Open 64 compiler.  It supports a roughly
# Fortran 77 level of details, however it uses IR nodes that have been extended or removed (in a few cases)
# as part of adding the Fortran 2003 support using the Open Fortan Parser.
EXTRA_DIST = CMakeLists.txt frontend.docs

#------------------------------------------------------------------------------------------------------------------------
# Header files for the binary partitioner must be installed under the Partitioner2 subdirectory and included as
# <Partitioner2/*.h> to avoid conflicts with other headers.  Thus, we need to list them here instead of in the subdirectory.

nobase_pkginclude_HEADERS=			\
	Partitioner2/AddressUsageMap.h		\
	Partitioner2/BasicBlock.h		\
	Partitioner2/BasicTypes.h		\
	Partitioner2/CfgPath.h			\
	Partitioner2/Config.h			\
	Partitioner2/ControlFlowGraph.h		\
	Partitioner2/DataBlock.h		\
	Partitioner2/DataFlow.h			\
	Partitioner2/Engine.h			\
	Partitioner2/Exception.h		\
	Partitioner2/Function.h			\
	Partitioner2/FunctionCallGraph.h	\
	Partitioner2/GraphViz.h			\
	Partitioner2/InstructionProvider.h	\
	Partitioner2/Modules.h			\
	Partitioner2/ModulesElf.h		\
	Partitioner2/ModulesM68k.h		\
	Partitioner2/ModulesPe.h		\
	Partitioner2/ModulesX86.h		\
	Partitioner2/OwnedDataBlock.h		\
	Partitioner2/Partitioner.h		\
	Partitioner2/Reference.h		\
	Partitioner2/Semantics.h		\
	Partitioner2/Utility.h
