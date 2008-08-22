include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

INCLUDES = $(ROSE_INCLUDES)

# This will cause the dynamic library to be built (as libroseExecFormats.so)
noinst_LTLIBRARIES = libroseExecFormats.la

libroseExecFormats_la_DEPENDENCIES =
libroseExecFormats_la_SOURCES = \
   ROSE_ExecGeneric.C \
   ROSE_ExecDOS.C \
   ROSE_ExecELF.C \
   ROSE_ExecPE.C \
   ROSE_ExecNE.C \
   ROSE_ExecLE.C

# DQ (8/21/2008): Removed older version of Robb's code (from before it's use in the IR nodes)
# ExecGeneric.C ExecGeneric.h ExecDOS.C ExecDOS.h ExecELF.C ExecELF.h  ExecLE.C ExecLE.h ExecNE.C ExecNE.h ExecPE.C ExecPE.h

# dist_include_HEADERS = ExecGeneric.h ExecDOS.h  ExecELF.h ExecLE.h ExecNE.h ExecPE.h

include_HEADERS = 
