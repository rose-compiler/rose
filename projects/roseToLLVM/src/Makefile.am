include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

if ROSE_USE_LLVM

#############################################################################
SUBDIRS 		= rosetollvm

#############################################################################
#MAINTAINERCLEANFILES	= Makefile.in

INCLUDES		=						\
	-I$(srcdir)/rosetollvm/include
	$(ROSE_INCLUDES) \
	$(ROSE_LLVM_INCLUDES) \

#############################################################################

EXTRA_DIST		 =

#############################################################################

noinst_HEADERS		=

bin_PROGRAMS		= rose2llvm

rose2llvm_SOURCES	=		\
	translator.cpp

AM_CPPFLAGS = -D__STDC_LIMIT_MACROS -D_STDC_CONSTANT_MACROS -Wno-deprecated

LDADD = $(LIBS_WITH_RPATH) \
		$(ROSE_LIBS) \
		$(ROSE_LLVM_LDFLAGS)

#rose2llvm_CFLAGS		= 					\
#	$(ROSE_CPPFLAGS) $(LLVM_CPPFLAGS)

#rose2llvm_CPPFLAGS	= -Wno-deprecated

#rose2llvm_LDADD		= $(top_builddir)/src/rosetollvm/librosetollvm.la

endif

#############################################################################
