include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

if ROSE_USE_LLVM

INCLUDES = $(ROSE_INCLUDES) $(ROSE_LLVM_INCLUDES) $(srcdir)/../include

bin_PROGRAMS = roseLLVM

roseLLVM_SOURCES = main.cpp VisitorTraversal.cpp

LDADD = $(LIBS_WITH_RPATH) $(ROSE_LIBS) $(ROSE_LLVM_LIB_WITH_PATH)

endif

clean-local:
	rm -rf $(CXX_TEMPLATE_OBJECTS) Templates.DB ii_files ti_files
	rm -f *.o rose_*.C *.sage.C doxygenComment.C \
              *.C.pdf *.C.edg.pdf *.pdf *.dot *.sidl *.int.C rose_directives_list.txt core
	rm -rf ti_files
	rm -f *.docs
