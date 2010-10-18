include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

INCLUDES = $(ROSE_INCLUDES)

noinst_LTLIBRARIES = libsageInterface.la
libsageInterface_la_SOURCES      = \
     sageInterface.C \
     sageInterfaceAsm.C \
     sageInterface_asm.C \
     sageInterface_type.C \
     generateUniqueName.C \
     sageBuilder.C \
     sageBuilderAsm.C \
     abiStuff.C
#     highLevelInterface.C

#     buildSetOfFrontendSpecificNodes.C

libsageInterface_la_LIBADD       = 
libsageInterface_la_DEPENDENCIES = 

include_HEADERS = \
     sageInterface.h \
     sageInterfaceAsm.h \
     sageBuilder.h \
     sageBuilderAsm.h \
     integerOps.h \
     abiStuff.h
#     highLevelInterface.h
#     loopHelpers.h 

#libsageInterface.la: $(libastFixup_la_OBJECTS) $(libastFixup_la_DEPENDENCIES)
#	-rm -f $@
#	$(CXX_SHARED_LIB_UPDATE) $@ $(libastFixup_la_OBJECTS) $(libastFixup_la_LIBADD)
#	test -f $@ || exit 1


# noinst_PROGRAMS = testForBug
# testForBug_SOURCES = testForBug.C

EXTRA_DIST = CMakeLists.txt
