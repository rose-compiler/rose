include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

#############################################################################
SUBDIRS 		= rosetollvm

#############################################################################

EXTRA_DIST		 =

#############################################################################

noinst_HEADERS		=

bin_PROGRAMS		= rose2llvm fold

AM_LDFLAGS = $(ROSE_LIBS) $(LLVM_LDFLAGS) $(ROSE_RPATHS) 
AM_CPPFLAGS = $(ROSE_INCLUDES) -I$(srcdir)/include -Wno-deprecated -Wall -Wextra -g -O0 

rose2llvm_SOURCES  = translator.cpp
rose2llvm_CXXFLAGS = -Wno-deprecated 
rose2llvm_CPPFLAGS = \
  -I$(srcdir)/rosetollvm/include \
  $(LLVM_CPPFLAGS) -std=c++11
#rose2llvm_LDFLAGS  = \
#  $(ROSE_LIBS) $(LLVM_LDFLAGS)
rose2llvm_LDADD = \
  $(builddir)/rosetollvm/librosetollvm.la \
  $(LLVM_LIBS)

fold_SOURCES  = fold.cpp
fold_CXXFLAGS = -Wno-deprecated
fold_CPPFLAGS = \
  -I$(srcdir)/rosetollvm/include \
  $(LLVM_CPPFLAGS) -std=c++11
#fold_LDFLAGS  = \
#  $(ROSE_LIBS) $(LLVM_LDFLAGS)
fold_LDADD = \
  $(builddir)/rosetollvm/librosetollvm.la \
  $(LLVM_LIBS)

#############################################################################

clean-local:
	rm -rf *.lo .libs rose2llvm fold 
