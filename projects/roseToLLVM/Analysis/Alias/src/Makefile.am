include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

if ROSE_USE_LLVM

SUBDIRS = src

INCLUDES = -I$(top_srcdir)/projects/roseToLLVM/Analysis/Alias/include \
		   -I$(top_srcdir)/projects/roseToLLVM/src/rosetollvm/include \
		   $(ROSE_INCLUDES) \
		   $(ROSE_LLVM_INCLUDES)

bin_programs = rose2llvmAA

rose2llvmAA_SOURCES  = 

rose2llvmAA_CPPFLAGS = -D__STDC_LIMIT_MACROS -Wno-deprecated

rose2llvm_LDADD = $(top_builddir)/projects/roseToLLVM/src/rosetollvm/librosetollvm.la

endif
