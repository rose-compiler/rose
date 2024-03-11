include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

AM_CPPFLAGS = $(ROSE_INCLUDES) -I$(top_srcdir)/src/frontend/Experimental_General_Language_Support

if ROSE_BUILD_JAVA_LANGUAGE_SUPPORT
AM_CPPFLAGS += $(JAVA_JVM_INCLUDE)
endif

noinst_LTLIBRARIES = libsageInterface.la
libsageInterface_la_SOURCES      =		\
     abiStuff.C					\
     generateUniqueName.C			\
     SageBuilderAsm.C				\
     sageBuilder.C				\
     sageBuilder_fortran.C			\
     sageBuilder.h				\
     sageInterfaceAda.C				\
     sageInterfaceAda.h				\
     sageInterface_asm.C			\
     sageInterfaceAsm.C				\
     sageInterface.C				\
     sageInterface.h				\
     sageInterface_type.C

libsageInterface_la_LIBADD       =
libsageInterface_la_DEPENDENCIES =

pkginclude_HEADERS =				\
     abiStuff.h					\
     integerOps.h				\
     SageBuilderAsm.h				\
     sageBuilder.h				\
     sageContainer.h				\
     sageCopy.h					\
     sageFunctors.h				\
     sageGeneric.h				\
     sageHash.h					\
     sageInterfaceAda.h				\
     sageInterfaceAsm.h				\
     sageInterface.h				\
     sageMemory.h				\
     sageRti.h					\
     sageTraversal.h

EXTRA_DIST = CMakeLists.txt
