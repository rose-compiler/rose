include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

RuntimeSystemlibdir = $(libdir)
RuntimeSystemlib_LTLIBRARIES = libCppRuntimeSystem.la

AM_CXXFLAGS = -g -Wall -Wextra

if ROSE_WITH_UPC
AM_UPCFLAGS = -g -Wall -Wextra -DWITH_UPC=1 -DIN_TARGET_LIBS
RuntimeSystemlib_LTLIBRARIES += libUpcRuntimeSystem.la
endif

SUBDIRS = DebuggerQt

INCLUDES = . .. $(ROSE_BOOST_PREINCLUDE_PATH) $(ROSE_BOOST_NORMAL_INCLUDE_PATH) -DBOOST_REGEX_MATCH_EXTRA

if ROSE_WITH_ROSEQT
INCLUDES += $(ROSE_QT_INCLUDE)
endif

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

if ROSE_WITH_UPC

nodist_libUpcRuntimeSystem_la_SOURCES = CppRuntimeSystem-upc.cpp \
			                                  FileManager-upc.cpp \
			                                  Util-upc.cpp \
			                                  MemoryManager-upc.cpp \
			                                  CStdLibManager-upc.cpp \
			                                  VariablesType-upc.cpp\
			                                  RsType-upc.cpp\
			                                  TypeSystem-upc.cpp \
			                                  PointerManager-upc.cpp\
			                                  StackManager-upc.cpp \
			                                  ptrops-upc.upc \
			                                  workzone-upc.upc \
			                                  rtedsync-upc.upc

libUpcRuntimeSystem_la_LIBTOOLFLAGS = --tag=CXX
endif

ptrops-upc.upc: $(srcdir)/ptrops.c
	cp $< $@

%-upc.cpp: $(srcdir)/%.cpp
	$(CXX) -E -DWITH_UPC $(INCLUDES) $< -o $@

%-upc.upc: $(srcdir)/%.upc
	cp $< $@


if ROSE_WITH_ROSEQT
        libCppRuntimeSystem_la_LIBADD = DebuggerQt/libDebuggerQt.la $(QT_LDFLAGS)
        libCppRuntimeSystem_la_CPPFLAGS = -DROSE_WITH_ROSEQT
endif


bin_PROGRAMS  = RuntimeSystemCppTest

RuntimeSystemCppTest_SOURCES = test.cpp
RuntimeSystemCppTest_LDADD   = libCppRuntimeSystem.la


include_HEADERS = rted_iface_structs.h \
                  ptrops.h \
                  ptrops_operators.h \
                  rted_typedefs.h

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
					 workzone.h \
					 rtedsync.h

check-local:
	./RuntimeSystemCppTest


clean-local:
	rm -rf *.dot Templates.DB *~ rose_rose* test *.bin *.txt *.c *.cpp *upc.upc
