
include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

AM_CPPFLAGS = $(ROSE_INCLUDES) \
         -I$(top_srcdir)/projects/TALCDataLayout/src/meta \
	 -I$(top_srcdir)/projects/TALCDataLayout/src/utilities

META_PATH=../meta

noinst_LIBRARIES = libtalcruntime.a
libtalcruntime_a_SOURCES = TALC_Runtime.cpp \
			   $(META_PATH)/Parser.cpp \
			   $(META_PATH)/View.cpp \
			   $(META_PATH)/Meta.cpp \
			   $(META_PATH)/Field.cpp \
			   $(META_PATH)/FieldBlock.cpp

libtalcruntime_a_LIBADD = $(LIBS_WITH_RPATH)

EXTRA_DIST= TALC.hpp

