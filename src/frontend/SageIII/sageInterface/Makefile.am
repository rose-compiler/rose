include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

AM_CPPFLAGS = $(ROSE_INCLUDES) -I$(top_srcdir)/src/frontend/Experimental_General_Language_Support

if ROSE_BUILD_JAVA_LANGUAGE_SUPPORT
AM_CPPFLAGS += $(JAVA_JVM_INCLUDE)
endif

noinst_LTLIBRARIES = libsageInterface.la
libsageInterface_la_SOURCES      = \
     sageInterface.h \
     sageInterface.C \
     sageInterface_type.C \
     generateUniqueName.C \
     sageBuilder.h \
     sageBuilder.C \
     sageBuilder_fortran.C \
     abiStuff.C \
     sageBuilder_untypedNodes.C \
     untypedBuilder.C 

if ROSE_BUILD_BINARY_ANALYSIS_SUPPORT
   libsageInterface_la_SOURCES += \
     sageInterfaceAsm.C \
     sageInterface_asm.C \
     SageBuilderAsm.C
endif

libsageInterface_la_LIBADD       =
libsageInterface_la_DEPENDENCIES =

pkginclude_HEADERS = \
     sageInterface.h \
     sageBuilder.h \
     sageGeneric.h \
     sageFunctors.h \
     integerOps.h \
     untypedBuilder.h \
     abiStuff.h \
     sageInterfaceAsm.h \
     SageBuilderAsm.h

EXTRA_DIST = CMakeLists.txt
