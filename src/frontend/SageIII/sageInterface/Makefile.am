include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

INCLUDES = $(ROSE_INCLUDES)

noinst_LTLIBRARIES = libsageInterface.la
libsageInterface_la_SOURCES      = \
     sageInterface.C \
     sageInterface_type.C \
     generateUniqueName.C \
     sageBuilder.C \
     sageBuilder_fortran.C \
     abiStuff.C
#     highLevelInterface.C

#     buildSetOfFrontendSpecificNodes.C

if ROSE_BUILD_BINARY_ANALYSIS_SUPPORT
   libsageInterface_la_SOURCES += \
     sageInterfaceAsm.C \
     sageInterface_asm.C \
     sageBuilderAsm.C 
endif

libsageInterface_la_LIBADD       = 
libsageInterface_la_DEPENDENCIES = 

include_HEADERS = \
     sageInterface.h \
     sageBuilder.h \
     integerOps.h \
     abiStuff.h
#     highLevelInterface.h
#     loopHelpers.h 

# if ROSE_BUILD_BINARY_ANALYSIS_SUPPORT
   include_HEADERS += \
     sageInterfaceAsm.h \
     sageBuilderAsm.h 
# endif


#libsageInterface.la: $(libastFixup_la_OBJECTS) $(libastFixup_la_DEPENDENCIES)
#	-rm -f $@
#	$(CXX_SHARED_LIB_UPDATE) $@ $(libastFixup_la_OBJECTS) $(libastFixup_la_LIBADD)
#	test -f $@ || exit 1


# noinst_PROGRAMS = testForBug
# testForBug_SOURCES = testForBug.C

EXTRA_DIST = CMakeLists.txt
