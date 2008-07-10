include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

INCLUDES = $(ROSE_INCLUDES)

if DEV_LIBS
lib_LTLIBRARIES = libroseExecFormats.la
else
# This will cause the dynamic library to be built (as libroseExecFormats.so)
noinst_LTLIBRARIES = libroseExecFormats.la
endif

libroseExecFormats_la_DEPENDENCIES =
libroseExecFormats_la_SOURCES = ExecGeneric.C ExecGeneric.h ExecDOS.h ExecDOS.C ExecPE.C ExecPE.h ExecELF.C ExecELF.h

dist_include_HEADERS = ExecGeneric.h ExecDOS.h ExecPE.h ExecELF.h

include_HEADERS = 
