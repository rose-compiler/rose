include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

RuntimeSystemlibdir = $(libdir)
RuntimeSystemlib_LTLIBRARIES = libCppRuntimeSystem.la

SUBDIRS = DebuggerQt

INCLUDES = .  $(ROSE_INCLUDES)

libCppRuntimeSystem_la_SOURCES = CppRuntimeSystem.cpp \
                                 FileManager.cpp \
                                 Util.cpp \
                                 MemoryManager.cpp \
                                 CStdLibManager.cpp \
                                 VariablesType.cpp\
                                 RsType.cpp\
                                 TypeSystem.cpp \
                                 PointerManager.cpp\
                                 StackManager.cpp \
                                 ptrops.c



if ROSE_WITH_ROSEQT
        libCppRuntimeSystem_la_LIBADD = DebuggerQt/libDebuggerQt.la $(QT_LDFLAGS)
        libCppRuntimeSystem_la_CPPFLAGS = -DROSE_WITH_ROSEQT
endif


bin_PROGRAMS  = RuntimeSystemCppTest

RuntimeSystemCppTest_SOURCES = test.cpp
RuntimeSystemCppTest_LDADD   = libCppRuntimeSystem.la


pkginclude_HEADERS = CppRuntimeSystem.h \
					 FileManager.h \
					 Util.h \
					 MemoryManager.h \
					 CStdLibManager.h \
					 VariablesType.h\
					 RsType.h\
					 TypeSystem.h\
					 PointerManager.h\
					 StackManager.h \
					 support.h


#DebuggerQt/libDebuggerQt.la


check-local:
	./RuntimeSystemCppTest


clean-local:
	rm -rf *.dot Templates.DB *~ rose_rose* test *.bin *.txt *.c *.cpp
