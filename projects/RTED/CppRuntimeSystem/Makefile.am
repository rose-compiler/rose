

RuntimeSystemlibdir = $(libdir)
RuntimeSystemlib_LTLIBRARIES = libCppRuntimeSystem.la



libCppRuntimeSystem_la_SOURCES = CppRuntimeSystem.cpp \
                                 FileManager.cpp \
                                 Util.cpp \
                                 MemoryManager.cpp \
                                 VariablesType.cpp



bin_PROGRAMS  = RuntimeSystemCppTest

RuntimeSystemCppTest_SOURCES = test.cpp
RuntimeSystemCppTest_LDADD   = libCppRuntimeSystem.la


check-local:
	./RuntimeSystemCppTest
