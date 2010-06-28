include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

if ROSE_USE_LLVM

#HEADERSDIR = $(srcdir)/../include

INCLUDES = $(ROSE_INCLUDES) $(ROSE_LLVM_INCLUDES) -I$(srcdir)/../include

bin_PROGRAMS = roseLLVM

roseLLVM_SOURCES = main.cpp VisitorTraversal.cpp util.cpp FunctionToLLVM.cpp

AM_CPPFLAGS = -D__STDC_LIMIT_MACROS -D__STDC_CONSTANT_MACROS
#noinst_HEADERS = $(HEADERSDIR)/llvm-interface.h \
#				 $(HEADERSDIR)/VisitorTraversal.h

LDADD = $(LIBS_WITH_RPATH) $(ROSE_LIBS) $(ROSE_LLVM_LIB_WITH_PATH)

endif

clean-local:
	rm -rf $(CXX_TEMPLATE_OBJECTS) Templates.DB ii_files ti_files
	rm -f *.o rose_*.C *.sage.C doxygenComment.C \
              *.C.pdf *.C.edg.pdf *.pdf *.dot *.sidl *.int.C rose_directives_list.txt core
	rm -rf ti_files
	rm -f *.docs
