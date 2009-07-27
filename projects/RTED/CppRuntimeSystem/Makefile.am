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
				 TypeSystem.cpp\
				 PointerManager.cpp\
				 StackManager.cpp



if ROSE_WITH_ROSEQT
        libCppRuntimeSystem_la_LIBADD = DebuggerQt/libDebuggerQt.la $(QT_LDFLAGS)
        libCppRuntimeSystem_la_CPPFLAGS = -DROSE_WITH_ROSEQT
endif

bin_PROGRAMS  = RuntimeSystemCppTest

RuntimeSystemCppTest_SOURCES = test.cpp ../RuntimeSystem.cpp
RuntimeSystemCppTest_LDADD   = libCppRuntimeSystem.la 



#DebuggerQt/libDebuggerQt.la


check-local:
	./RuntimeSystemCppTest



