
include_directories(${KDE4_INCLUDES} ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} )


########### next target ###############
set( libsageInterface_SRC  
     sageInterface.C 
     sageInterface_asm.C 
     sageInterface_type.C 
     generateUniqueName.C 
     sageBuilder.C 
     sageBuilderAsm.C 
     abiStuff.C
   )

add_library( libsageInterface ${libsageInterface_SRC}  )
add_dependencies( libsageInterface ROSETTA )

########### install files ###############

install(FILES  sageInterface.h       sageBuilder.h       sageBuilderAsm.h       endianWrappers.h       integerOps.h       abiStuff.h DESTINATION ${INCLUDE_INSTALL_DIR})



#original Makefile.am contents follow:

#include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs
#
#INCLUDES = $(ROSE_INCLUDES)
#
#noinst_LTLIBRARIES = libsageInterface.la
#libsageInterface_la_SOURCES      = \
#     sageInterface.C \
#     sageInterface_asm.C \
#     sageInterface_type.C \
#     generateUniqueName.C \
#     sageBuilder.C \
#     sageBuilderAsm.C \
#     abiStuff.C
##     highLevelInterface.C
#
##     buildSetOfFrontendSpecificNodes.C
#
#libsageInterface_la_LIBADD       = 
#libsageInterface_la_DEPENDENCIES = 
#
#include_HEADERS = \
#     sageInterface.h \
#     sageBuilder.h \
#     sageBuilderAsm.h \
#     endianWrappers.h \
#     integerOps.h \
#     abiStuff.h
##     highLevelInterface.h
##     loopHelpers.h 
#
##libsageInterface.la: $(libastFixup_la_OBJECTS) $(libastFixup_la_DEPENDENCIES)
##	-rm -f $@
##	$(CXX_SHARED_LIB_UPDATE) $@ $(libastFixup_la_OBJECTS) $(libastFixup_la_LIBADD)
##	test -f $@ || exit 1
#
#
## noinst_PROGRAMS = testForBug
## testForBug_SOURCES = testForBug.C
