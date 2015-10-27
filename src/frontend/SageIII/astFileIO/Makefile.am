include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

AM_CPPFLAGS = $(ROSE_INCLUDES)

# noinst_LTLIBRARIES = libastFileIO.la

# DQ (10/18/2007): These have been moved to the src/frontend/SageIII directory
# to provde greater parallelism to the make -jn parallel make feature.
# libastFileIO_la_SOURCES  =  AST_FILE_IO.C StorageClasses.C

# AST_FILE_IO.lo: StorageClassMemoryManagement.h 
StorageClasses.lo: StorageClassMemoryManagement.h StorageClassMemoryManagement.C AstSpecificDataManagingClass.h

pkginclude_HEADERS = \
     StorageClassMemoryManagement.h

# This file is generated
nodist_pkginclude_HEADERS = \
     AstSpecificDataManagingClass.h

ROSETTA_GENERATED_FILES = AstSpecificDataManagingClass.h SourcesOfIRNodesAstFileIOSupport.C

$(ROSETTA_GENERATED_FILES): rosetta-files-built

rosetta-files-built:
	cd ../../../ROSETTA/src; $(MAKE)

#    AST_FILE_IO.h
#    StorageClasses.h

EXTRA_DIST = CMakeLists.txt StorageClassMemoryManagement.C

# DQ (3/28/2006): Remove all generated code
clean-local:
	rm -rf AST_FILE_IO.h AST_FILE_IO.C StorageClasses.h StorageClasses.C
	rm -rf SourcesOfIRNodesAstFileIOSupport.C AstSpecificDataManagingClass.h
