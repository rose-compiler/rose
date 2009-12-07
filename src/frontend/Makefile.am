include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

# Later add EDG_1.4, EDG_3.3, Open64, SAGE_III
# SUBDIRS = SageIII EDG EDG_SAGE_Connection
# SUBDIRS = SageIII EDG EDG_SAGE_Connection Open64_SAGE_Connection BinaryDisassembly
# SUBDIRS = SageIII EDG EDG_SAGE_Connection BinaryDisassembly
# SUBDIRS = SageIII EDG EDG_SAGE_Connection OpenFortranParser_SAGE_Connection BinaryDisassembly ObjdumpDisassembler
# SUBDIRS = SageIII BinaryDisassembly ObjdumpDisassembler OpenFortranParser_SAGE_Connection EDG EDG_SAGE_Connection
SUBDIRS = SageIII CxxFrontend OpenFortranParser_SAGE_Connection BinaryDisassembly Disassemblers BinaryFormats BinaryLoader PHPFrontend


# Until we revisit this this, we want to include specific documentation that has been put into this directory.
# Note that OpenFortranParser_SAGE_Connection has the newer work to support Fortran in ROSE.  This work
# is part of a contract to add Fortran support to ROSE using the Open 64 compiler.  It supports a roughly
# Fortran 77 level of details, however it uses IR nodes that have been extended or removed (in a few cases)
# as part of adding the Fortran 2003 support using the Open Fortan Parser.
EXTRA_DIST = CMakeLists.txt Open64 frontend.docs

