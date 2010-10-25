include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

# Later add EDG_1.4, EDG_3.3, Open64, SAGE_III
# SUBDIRS = SageIII EDG EDG_SAGE_Connection
# SUBDIRS = SageIII EDG EDG_SAGE_Connection Open64_SAGE_Connection BinaryDisassembly
# SUBDIRS = SageIII EDG EDG_SAGE_Connection BinaryDisassembly
# SUBDIRS = SageIII EDG EDG_SAGE_Connection OpenFortranParser_SAGE_Connection BinaryDisassembly ObjdumpDisassembler
# SUBDIRS = SageIII BinaryDisassembly ObjdumpDisassembler OpenFortranParser_SAGE_Connection EDG EDG_SAGE_Connection
# SUBDIRS = SageIII CxxFrontend OpenFortranParser_SAGE_Connection BinaryDisassembly Disassemblers BinaryFormats BinaryLoader PHPFrontend

SUBDIRS = SageIII 

# Note that CXX and C are either both defined or both undefined.
if ROSE_BUILD_CXX_LANGUAGE_SUPPORT
   SUBDIRS += CxxFrontend
endif

if ROSE_BUILD_FORTRAN_LANGUAGE_SUPPORT
   SUBDIRS += OpenFortranParser_SAGE_Connection
endif

if ROSE_BUILD_JAVA_LANGUAGE_SUPPORT
   SUBDIRS += ECJ_ROSE_Connection
endif

if ROSE_BUILD_BINARY_ANALYSIS_SUPPORT
   SUBDIRS += BinaryDisassembly Disassemblers BinaryFormats BinaryLoader
else
   SUBDIRS += BinaryFormats
endif

if ROSE_BUILD_PHP_LANGUAGE_SUPPORT
   SUBDIRS += PHPFrontend
endif

# Until we revisit this this, we want to include specific documentation that has been put into this directory.
# Note that OpenFortranParser_SAGE_Connection has the newer work to support Fortran in ROSE.  This work
# is part of a contract to add Fortran support to ROSE using the Open 64 compiler.  It supports a roughly
# Fortran 77 level of details, however it uses IR nodes that have been extended or removed (in a few cases)
# as part of adding the Fortran 2003 support using the Open Fortan Parser.
EXTRA_DIST = CMakeLists.txt Open64 frontend.docs

