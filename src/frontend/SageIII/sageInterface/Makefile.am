include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

INCLUDES = $(ROSE_INCLUDES)

if DEV_LIBS
lib_LTLIBRARIES = libsageInterface.la
else
noinst_LTLIBRARIES = libsageInterface.la
endif
libsageInterface_la_SOURCES      = \
     sageInterface.C \
     sageInterface_type.C \
     generateUniqueName.C \
     sageBuilder.C
#     highLevelInterface.C

#     buildSetOfFrontendSpecificNodes.C

libsageInterface_la_LIBADD       = 
libsageInterface_la_DEPENDENCIES = 

include_HEADERS = \
     sageInterface.h \
     sageBuilder.h
#     highLevelInterface.h
#     loopHelpers.h 

#libsageInterface.la: $(libastFixup_la_OBJECTS) $(libastFixup_la_DEPENDENCIES)
#	-rm -f $@
#	$(CXX_SHARED_LIB_UPDATE) $@ $(libastFixup_la_OBJECTS) $(libastFixup_la_LIBADD)
#	test -f $@ || exit 1

