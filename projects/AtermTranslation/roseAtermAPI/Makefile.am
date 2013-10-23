include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

INCLUDES = $(ROSE_INCLUDES)

# These appear to be the minimal required header files for a consistant compilation of the Aterm API.
include_HEADERS = aterm1.h aterm2.h abool.h afun.h atypes.h encoding.h


lib_LTLIBRARIES = libroseAtermAPI.la

libroseAtermAPI_la_SOURCES = aterm1.C aterm2.C afun.C roseSpecificCode.C




