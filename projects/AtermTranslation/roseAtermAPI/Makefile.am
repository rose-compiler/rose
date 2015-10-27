include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

AM_CPPFLAGS = $(ROSE_INCLUDES)

# These appear to be the minimal required header files for a consistant compilation of the Aterm API.
include_HEADERS = aterm1.h aterm2.h abool.h afun.h atypes.h encoding.h deprecated.h


# lib_LTLIBRARIES = libroseAtermAPI.la
# lib_LTLIBRARIES = libATerm.la
lib_LTLIBRARIES = libATerm.la

# libroseAtermAPI_la_SOURCES = aterm1.C aterm2.C afun.C roseSpecificCode.C
libATerm_la_SOURCES = aterm1.C aterm2.C afun.C roseSpecificCode.C

all:
	@echo "build links to header files in build tree..."
	mkdir -p include
	rm -f aterm1.h aterm2.h abool.h afun.h atypes.h encoding.h deprecated.h
	rm -f include/aterm1.h include/aterm2.h include/abool.h include/afun.h include/atypes.h include/encoding.h include/deprecated.h
	cd include; ln -s $(srcdir)/aterm1.h aterm1.h
	cd include; ln -s $(srcdir)/aterm2.h aterm2.h
	cd include; ln -s $(srcdir)/abool.h abool.h
	cd include; ln -s $(srcdir)/afun.h afun.h
	cd include; ln -s $(srcdir)/atypes.h atypes.h
	cd include; ln -s $(srcdir)/encoding.h encoding.h
	cd include; ln -s $(srcdir)/deprecated.h deprecated.h
