include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

INCLUDES = $(ROSE_INCLUDES)

if DEV_LIBS
lib_LTLIBRARIES = libroseExecFormats.la
else
# This will cause the dynamic library to be built (as libroseExecFormats.so)
noinst_LTLIBRARIES = libroseExecFormats.la
endif

libroseExecFormats_la_DEPENDENCIES =
libroseExecFormats_la_SOURCES = ExecGeneric.C ExecGeneric.h \
                                ExecDOS.C     ExecDOS.h     \
                                ExecELF.C     ExecELF.h     \
				ExecLE.C      ExecLE.h      \
                                ExecNE.C      ExecNE.h      \
                                ExecPE.C      ExecPE.h 

dist_include_HEADERS = ExecGeneric.h ExecDOS.h  ExecELF.h ExecLE.h ExecNE.h ExecPE.h

include_HEADERS = 
