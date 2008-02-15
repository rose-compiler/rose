INCLUDES = -I$(top_srcdir)/include -I$(top_srcdir)/src/ListT -I$(top_builddir)/include @INCLUDES@ -I$(top_srcdir)/src/unparser

noinst_LIBRARIES = libpadutils.a
libpadutils_a_SOURCES = alignData.C arrayInfo.C cacheInfo.C padding_conditions.C padding_tools.C
noinst_HEADERS = alignData.h arrayInfo.h arrayReference.h cacheInfo.h padding_conditions.h padding_tools.h

## Add debugging option "-g"
AM_CPPFLAGS = -g


clean-local:
	rm -rf Templates.DB ii_files ti_files
