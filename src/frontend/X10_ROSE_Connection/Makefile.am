include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

EXTRA_DIST = CMakeLists.txt

noinst_LTLIBRARIES = libroseX10.la

include_HEADERS = \
	jni_x10SourceCodePosition.h \
	jni_x10_token.h \
	jni_x10_utils.h \
	x10.h \
	x10SourceCodePosition.h \
	x10_support.h \
	x10_token.h \
    x10Parser.h \
    jserver.h

if ROSE_BUILD_X10_LANGUAGE_SUPPORT

X10_DIR = @X10_INSTALL_PATH@

lib_LTLIBRARIES = libx10Traversal.la

libx10Traversal_la_SOURCES = \
	jni_x10SourceCodePosition.C \
	jni_x10_token.C \
	jni_x10_utils.C \
	x10ActionROSE.C \
	x10SourceCodePosition.C \
	x10Traversal.C \
	x10_support.C \
	x10_token.C \
    ParserActionROSE.C


libroseX10_la_SOURCES = \
	x10.C jserver.C \
	x10_main.cpp

# BOOST_CPPFLAGS is included because that's where the -pthread switch is located if user desires it.
INCLUDES =							\
	$(ROSE_INCLUDES)					\
	-I$(top_srcdir)/src/frontend/X10_ROSE_Connection	\
	-I$(X10_DIR)						\
	$(JAVA_JVM_INCLUDE)					\
	$(BOOST_CPPFLAGS)

libroseX10_la_LDFLAGS = $(BOOST_LDFLAGS) $(BOOST_THREAD_LIB)
libx10Traversal_la_LDFLAGS = $(BOOST_LDFLAGS) $(BOOST_THREAD_LIB)

else

libroseX10_la_SOURCES = dummy_x10_main.cpp

endif
